'use strict';

module.exports = {
    Location: class {
        constructor (lat, lon) {
            this.lat = lat;
            this.lon = lon;
        }
    },

    _osmStr: class {
        constructor (DB) {
            this.str = null;
            this.DB = DB;
        }

        get xml () {
            if (!this.str) this.str = this.DB.toXML();
            return this.str;
        }
    },

    FuzzyMatch: class {
        match (got, want) {
            var matchPercent = want.match(/(.*)\s+~(.+)%$/),
                matchAbs = want.match(/(.*)\s+\+\-(.+)$/),
                matchRe = want.match(/^\/(.*)\/$/);

            if (got === want) {
                return true;
            } else if (matchPercent) {         // percentage range: 100 ~ 5%
                var target = parseFloat(matchPercent[1]),
                    percentage = parseFloat(matchPercent[2]);
                if (target === 0) {
                    return true;
                } else {
                    var ratio = Math.abs(1 - parseFloat(got) / target);
                    return 100 * ratio < percentage;
                }
            } else if (matchAbs) {             // absolute range: 100 +-5
                var margin = parseFloat(matchAbs[2]),
                    fromR = parseFloat(matchAbs[1]) - margin,
                    toR = parseFloat(matchAbs[1]) + margin;
                return parseFloat(got) >= fromR && parseFloat(got) <= toR;
            } else if (matchRe) {               // regex: /a,b,.*/
                return got.match(matchRe[1]);
            } else {
                return false;
            }
        }

        matchLocation (got, want) {
            return this.match(got[0], util.format('%d ~0.0025%', want.lat)) &&
                this.match(got[1], util.format('%d ~0.0025%', want.lon));
        }
    },

    TableDiffError: class extends Error {
        constructor (expected, actual) {
            super();
            this.headers = expected.raw()[0];
            this.expected = expected.hashes();
            this.actual = actual;
            this.diff = [];
            this.hasErrors = false;

            this.expected.forEach((row, i) => {
                var rowError = false;

                for (var j in row) {
                    if (row[j] != actual[i][j]) {
                        rowError = true;
                        this.hasErrors = true;
                        break;
                    }
                }
                if (rowError) {
                    this.diff.push(Object.assign({}, row, {status: 'undefined'}));
                    this.diff.push(Object.assign({}, actual[i], {status: 'comment'}));
                } else {
                    this.diff.push(row);
                }
            });
        }

        get string () {
            if (!this.hasErrors) return null;

            var s = ['Tables were not identical:'];
            s.push(this.headers.map(key => '    ' + key).join(' | '));
            this.diff.forEach((row) => {
                var rowString = '| ';
                this.headers.forEach((header) => {
                    if (!row.status) rowString += '    ' + row[header] + ' | ';
                    else if (row.status === 'undefined') rowString += '(-) ' + row[header] + ' | ';
                    else rowString += '(+) ' + row[header] + ' | ';
                });
                rowString += ' |';
                s.push(rowString);
            });
            return s.join('\n') + '\n\nTODO spacing/formatting';
        }
    }
}
