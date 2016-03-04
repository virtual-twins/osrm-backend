// 'use strict';

var fs = require('fs');
var util = require('util');
var exec = require('child_process').exec;

module.exports = function () {
    this.runBin = (bin, options, callback) => {
        var opts = options.slice();

        if (opts.match('{osmBase}')) {
            if (!this.osmFile) throw new Error('*** {osmBase} is missing');
            opts = opts.replace('{osmBase}', this.osmFile);
        }

        if (opts.match('{extractedBase}')) {
            if (!this.extractedFile) throw new Error('*** {extractedBase} is missing');
            opts = ops.replace('{extractedBase}', this.extractedFile);
        }

        if (opts.match('{preparedBase}')) {
            if (!this.preparedFile) throw new Error('*** {preparedBase} is missing');
            opts = ops.replace('{preparedBase}', this.preparedFile);
        }

        if (opts.match('{profile}')) {
            opts = ops.replace('{profile}', [this.PROFILES_PATH, this.profile + '.lua'].join('/'));
        }

        var cmd = util.format('%s%s%s/%s%s%s %s 2>error.log', this.QQ, this.LOAD_LIBRARIES, this.BIN_PATH, bin, this.EXE, this.QQ, opts);
        exec(cmd, (err, stdout, stderr) => {
            this.stdout = stdout;
            this.stderr = fs.readFileSync('error.log');
            this.exitCode = err && err.code || 0;
            callback(err, stdout, stderr);
            // TODO ? ^
        });
    }
}
