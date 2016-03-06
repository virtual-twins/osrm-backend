var fs = require('fs');
var util = require('util');
var path = require('path');
var sha1 = require('sha1');

module.exports = function () {
    this.hashOfFiles = (paths) => {
        paths = Array.isArray(paths) ? paths : [paths];
        var buf = '';
        paths.forEach((path) => {
            fs.readFile(path, (err, data) => {
                buf += data;
            });
        });
        return sha1(buf);
    }

    this.profileHash = () => {
        this.profileHashes = this.profileHashes || {};
        if (!this.profileHashes[this.profile]) this.profileHashes[this.profile] = this.hashOfFiles(path.resolve(this.PROFILES_PATH, this.profile + '.lua'));
    }

    var osmHash = this.osmHash = () => {
        if (!this._osmHash.length) {
            this._osmHash = sha1(this.osmStr.xml || '');
        }
        return this._osmHash;
    }

    this.fingerprintOSM = () => {
        if (!this._fingerprintOSM.length) {
            this._fingerprintOSM = sha1(osmHash());
        }
        return this._fingerprintOSM;
    }

    return this;
}
