// 'use strict';

var util = require('util');

module.exports = function () {
    // this.World = require('../support/world');
    // this.World();

    this.When(/^I route I should get$/, (table) => {
        this.reprocess();
        var actual = [];

        this.OSRMLoader.load(this, util.format('%s.osrm', this.preparedFile), () => {
            table.hashes().forEach((row, ri) => {
                var got,
                    response,
                    json;
                if (row.request) {
                    got = { request: row.request };
                    response = requestUrl(row.request);
                } else {
                    var defaultParams = this.queryParams;
                    var userParams = [];
                    got = {};
                    for (var key in row) {
                        var match = k.match(/param:(.*)/);
                        if (match) {
                            if (row[k] === '(nil)') {
                                userParams.push([match[1], null]);
                            } else if (row[k]) {
                                userParams.push([match[1], row[k]]);
                            }
                            got[k] = row[k];
                        }
                    }

                    var params = this.overwriteParams(defaultParams, userParams),
                        waypoints = [],
                        bearings = [];

                    if (row.bearings) {
                        got.bearings = row.bearings;
                        bearings = row.bearings.split(' ').filter(b => !!b);
                    }

                    if (row.from && row.to) {
                        var fromNode = this.findNodeByName(row.from);
                        if (!fromNode) throw new Error(util.format('*** unknown from-node "%s"'), row.from);
                        waypoints.push(fromNode);

                        var toNode = this.findNodeByName(row.to);
                        if (!toNode) throw new Error(util.format('*** unknown to-node "%s"'), row.to);
                        waypoints.push(toNode);

                        got.from = row.from;
                        got.to = row.to;
                        response = this.requestRoute(waypoints, bearings, params);
                    } else if (row.waypoints) {
                        row.waypoints.split(',').forEach((n) => {
                            // TODO again this might need to be trimmed *before* split
                            var node = this.findNodeByName(n.trim());
                            if (!node) throw new Error('*** unknown waypoint node "%s"', n.trim());
                            waypoints.push(node);
                        });
                        got.waypoints = row.waypoints;
                        response = requestRoute(waypoints, bearings, params);
                    } else {
                        throw new Error('*** no waypoints');
                    }
                }

                var instructions, bearings, compasses, turns, modes, times, distances;

                if (response.body.length) {
                    json = JSON.parse(response.body);

                    if (json.status === 200) {
                        instructions = wayList(json.route_instructions);
                        bearings = bearingList(json.route_instructions);
                        compasses = compassList(json.route_instructions);
                        turns = turnList(json.route_instructions);
                        modes = modeList(json.route_instructions);
                        times = timeList(json.route_instructions);
                        distances = distanceList(json.route_instructions);
                    }
                }

                if (table.headers.status) {
                    got.status = json.status.toString();
                }

                if (table.headers.message) {
                    got.message = json.status_message;
                }

                if (table.headers['#']) {
                    // comment column
                    got['#'] = row['#'];
                }

                // TODO this feels like has been repeated from elsewhere.....

                if (table.headers.start) {
                    got.start = instructions ? json.route_summary.start_point : null;
                }

                if (table.headers.end) {
                    got.end = instructions ? json.route_summary.end_point : null;
                }

                if (table.headers.geometry) {
                    got.geometry = json.route_geometry;
                }

                if (table.headers.route) {
                    got.route = (instructions || '').trim();

                    if (table.headers.alternative) {
                        got.alternative = json.found_alternative ?
                            wayList(json.alternative_instructions[0]) : '';
                    }

                    var distance = json.route_summary.total_distance,
                        time = json.route_summary.total_time;

                    if (table.headers.distance) {
                        if (row.distance.length) {
                            if (!row.distance.match(/\d+m/))
                                throw new Error('*** Distance must be specified in meters. (ex: 250m)');
                            got.distance = instructions ? util.format('%dm', distance) : '';
                        }
                    }

                    if (table.headers.time) {
                        if (!row.time.match(/\d+s/))
                            throw new Error('*** Time must be specied in seconds. (ex: 60s)');
                        got.time = instructions ? util.format('%ds', time) : '';
                    }

                    if (table.headers.speed) {
                        if (row.speed !== '' && instructions) {
                            if (!row.speed.match(/\d+ km\/h/))
                                throw new Error('*** Speed must be specied in km/h. (ex: 50 km/h)');
                            var speed = time > 0 ? Math.round(3.6*distance/time) : null;
                            got.speed = util.format('%d km/h', speed);
                        } else {
                            got.speed = '';
                        }
                    }

                    function putValue(key, value) {
                        if (table.headers[key]) got[key] = instructions ? value : '';
                    }

                    putValue('bearing', bearings);
                    putValue('compass', compasses);
                    putValue('turns', turns);
                    putValue('modes', modes);
                    putValue('times', times);
                    putValue('distances', distances);
                }

                ok = true;

                for (var key in row) {
                    if (this.FuzzyMatch.match(got[key], row[key])) {
                        got[key] = row[key];
                    } else {
                        ok = false;
                    }
                }

                if (!ok) {
                    this.logFail(row, got, { route: { query: this.query, response: response }});
                }

                actual.push(got);
            });
        });

        // TODO again
        return table.diff(actual);
    });

    this.When(/^I route (\d+) times I should get$/, (n, table) => {
        var ok = true;
        for (var i=0; i<n; i++) {
            if (!step('I route I should get', table)) ok = false;
        }
        return ok;
    });
}
