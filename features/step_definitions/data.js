var util = require('util');
var path = require('path');
var fs = require('fs');
var OSM = require('../support/build_osm');

module.exports = function () {
    var DB = OSM.DB;

    this.Given(/^the profile "([^"]*)"$/, (profile) => {
        this.setProfile(profile);
    });

    // this.Given(/^the import format "(.*?)"$/, (format, callback) => {
    //     this.setInputFormat(format);
    //     callback();
    // });

    this.Given(/^the extract extra arguments "(.*?)"$/, (args) => {
        this.setExtractArgs(args);
    });

    this.Given(/^a grid size of (\d+) meters$/, (meters) => {
        this.setGridSize(meters);
    });

    this.Given(/^the origin ([-+]?[0-9]*\.?[0-9]+),([-+]?[0-9]*\.?[0-9]+)$/, (lat, lon) => {
        this.setOrigin([parseFloat(lon), parseFloat(lat)]);
    });

    this.Given(/^the shortcuts$/, (table) => {
        table.hashes().forEach((row) => {
          // TODO what is shortcutsHash
          // Given /^the shortcuts$/ do |table|
          //   table.hashes.each do |row|
          //     shortcuts_hash[ row['key'] ] = row['value']
          //   end
          // end
            shortcutsHash[row.key] = row.value;
        });
    });

    this.Given(/^the node map$/, (table) => {
        table.raw.forEach((row, ri) => {
            row.forEach((name, ci) => {
                if (name) {
                    if (name.length !== 1) throw new Error(util.format('*** node invalid name %s, must be single characters', name));
                    if (!name.match(/[a-z0-9]/)) throw new Error(util.format('*** invalid node name %s, must me alphanumeric', name));

                    if (name.match(/[a-z]/)) {
                        if (nameNodeHash[name]) throw new Error(util.format('*** duplicate node %s', name));
                        var lonLat = this.tableCoordToLonLat(ci, ri);
                        this.addOSMNode(name, lonLat[0], lonLat[1], null);
                    } else {
                        if (locationHash[name]) throw new Error(util.format('*** duplicate node %s'), name);
                        var lonLat = this.tableCoordToLonLat(ci, ri);
                        this.addLocation(name, lonLat[0], lonLat[1], null);
                    }
                }
            });
        });
    });

    this.Given(/^the node locations$/, (table) => {
        table.hashes().forEach((row) => {
            var name = row.node;
            if (this.findNodeByName(name)) throw new Error(util.format('*** duplicate node %s'), name);

            if (name.match(/[a-z]/)) {
                var id = row.id && parseInt(row.id);
                this.addOSMNode(name, parseFloat(row.lon), parseFloat(row.lat), id);
            } else {
                this.addLocation(name, parseFloat(row.lon), parseFloat(row.lat));
            }
        });
    });

    this.Given(/^the nodes$/, (table) => {
        table.hashes().forEach((row) => {
            var name = row.node,
            node = this.findNodeByName(name);
            if (!node) throw new Error(util.format('*** unknown node %s'), name);
            node.push(row);
            // TODO not convinced by this ^
            // Given /^the nodes$/ do |table|
            //   table.hashes.each do |row|
            //     name = row.delete 'node'
            //     node = find_node_by_name(name)
            //     raise "*** unknown node '#{c}'" unless node
            //     node << row
            //   end
            // end
        });
    });

    this.Given(/^the ways$/, (table) => {
        if (this.osm_str) throw new Error('*** Map data already defined - did you pass an input file in this scenaria?');

        table.hashes().forEach((row) => {
            var way = new OSM.Way(this.makeOSMId, OSM_USER, OSM_TIMESTAMP, OSM_UID);

            var nodes = row.nodes;
            if (this.nameWayHash.nodes) throw new Error(util.format('*** duplicate way %s', nodes));

            for (var i=0; i<nodes.length; i++) {
                var c = nodes[i];
                if (!c.match(/[a-z]/)) throw new Error(util.format('*** ways can only use names a-z, %s'), name);
                var node = this.findNodeByName(c);
                if (!node) throw new Error(util.format('*** unknown node %s'), c);
                way.addNode(node);
            }

            var tags = {
                highway: 'primary'
            };

            for (var key in row) {
                tags[key] = row[key];
            }

            if (row.highway === '(nil)') delete tags.highway;

            if (row.name === undefined)
                tags.name = nodes;
            else if (row.name === '""' || row.name === "''")
                tags.name = '';
            else if (row.name === '' || row.name === '(nil)')
                delete tags.name;
            else
                tags.name = row.name;

            way.setTags(tags);
            DB.addWay(way);
            this.nameWayHash[nodes] = way;
        });
    });

    this.Given(/^the relations$/, (table) => {
        if (this.osm_str) throw new Error('*** Map data already defined - did you pass an input file in this scenaria?');

        table.hashes().forEach((row) => {
            var relation = new OSM.Relation(this.makeOSMId, OSM_USER, OSM_TIMESTAMP);

            for (var key in row) {
                var isNode = key.match(/^node:(.*)/),
                    isWay = key.match(/^way:(.*)/),
                    isColonSeparated = key.match(/^(.*):(.*)/);
                if (isNode) {
                    row[key].split(',').map(function(v) { return v.trim(); }).forEach(function(nodeName) {
                        if (nodeName.length !== 1) throw new Error(util.format('*** invalid relation node member "%s"'), nodeName);
                        var node = this.findNodeByName(nodeName);
                        if (!node) throw new Error(util.format('*** unknown relation node member "%s"'), nodeName);
                        relation.addMember('node', node.id, isNode[1]);
                    });
                } else if (isWay) {
                    row[key].split(',').map(function(v) { return v.trim(); }).forEach(function(wayName) {
                        var way = this.findWayByName(wayName);
                        if (!way) throw new Error(util.format('*** unknown relation way member "%s"'), wayName);
                        relation.addMember('way', way.id, isWay[1]);
                    });
                } else if (isColonSeparated && isColonSeparated[1] !== 'restriction') {
                    throw new Error(util.format('*** unknown relation member type "%s:%s", must be either "node" or "way"'), isColonSeparated[1], isColonSeparated[2]);
                } else {
                    relation.addTag(key, row[key]);
                }
            }
            relation.uid = OSM_UID;
            DB.addRelation(relation);
        });
    });

    this.Given(/^the input file ([^"]*)$/, (file) => {
        if (path.extname(file) !== '.osm') throw new Error('*** Input file must be in .osm format');
        this.osm_str = fs.readFileSync(file, 'utf8');
    });

    this.Given(/^the raster source$/, (data) => {
        fs.writeFileSync(path.resolve(this.TEST_FOLDER, 'rastersource.asc'), data);
    });

    this.Given(/^the data has been saved to disk$/, () => {
        try {
            this.writeInputData();
        } catch(e) {
            this.processError = e;
        }
    });

    this.Given(/^the data has been extracted$/, () => {
        try {
            this.writeInputData();
            if (!this.extracted) this.extractData();
        } catch(e) {
            this.processError = e;
        }
    });

    this.Given(/^the data has been prepared$/, () => {
        try {
            this.reprocess();
        } catch(e) {
            this.processError = e;
        }
    });

    this.Given(/^osrm\-routed is stopped$/, () => {
        try {
            this.OSRMLoader.shutdown();
        } catch(e) {
            this.processError = e;
        }
    });

    this.Given(/^data is loaded directly/, () => {
        this.loadMethod = 'directly';
    });

    this.Given(/^data is loaded with datastore$/, () => {
        this.loadMethod = 'datastore';
    });

    this.Given(/^the HTTP method "([^"]*)"$/, (method) => {
        this.httpMethod = method;
    });
}
