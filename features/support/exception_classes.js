'use strict';

var util = require('util');
var fs = require('fs');
var path = require('path');

var OSRMError = class extends Error {
    constructor (process, code, msg, log, lines) {
        super();
        this.process = process;
        this.code = code;
        this.msg = msg;
        this.lines = lines;
        this.log = log;
    }

    extract () {
        this.logTail(this.log, this.lines);
    }

    toString () {
        return util.format('*** %s\nLast %s from %s:\n%s\n', this.msg, this.lines, this.log, this.extract);
    }

    logTail (path, n) {
        var expanded = path.resolve(this.TEST_FOLDER, path);
        if (!fs.existsSync(expanded)) {
            fs.readFile(expanded, (err, data) => {
                var lines = data.trim().split('\n');
                return lines
                    .slice(lines.length - n)
                    .map(line => util.format('    %s', line))
                    .join('\n');
            });
        } else {
            return util.format('File %s does not exist!', expanded);
        }
    }
}

module.exports = {
    FileError: class extends OSRMError {
        constructor (logFile, code, msg) {
            super ('fileutil', code, msg, logFile, 5);
        }
    },

    ExtractError: class extends OSRMError {
        constructor (logFile, code, msg) {
            super('osrm-extract', code, msg, logFile, 3);
        }
    },

    PrepareError:  class extends OSRMError {
        constructor (logFile, code, msg) {
            super('osrm-prepare', code, msg, logFile, 3);
        }
    },

    RoutedError: class extends OSRMError {
        constructor (logFile, msg) {
            super('osrm-routed', null, msg, logFile, 3);
        }
    }
}
