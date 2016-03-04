// 'use strict';

var request = require('request');

module.exports = function () {
    // Converts an array [["param","val1"], ["param","val2"]] into param=val1&param=val2
    this.paramsToString = (params) => {
        var kvPairs = params.map((kv) => kv[0].toString() + '=' + kv[1].toString());
        var url = kvPairs.length ? kvPairs.join('&') : '';
        return url;
    }

    this.sendRequest = (baseUri, parameters, callback) => {
        setTimeout(() => {
            var uriString = baseUri,
                params = this.paramsToString(parameters);

            if (params.length) uriString += '?' + params;

            var options = this.httpMethod === 'POST' ? {
                method: 'POST',
                body: params,
                url: uriString          // TODO is this right? or does it need w/o params?
            } : uriString;

            request(options, (err, res, body) => {
                if (err && err.code === 'ECONNREFUSED') {           // TODO is this the code? or sth else?
                    throw new Error('*** osrm-routed is not running.');
                } else if (err && err.statusCode === 408) {         // TODO i think this is timeout code -- verify and/or clarify
                    throw new Error('*** osrm-routed did not respond.');
                }

                return callback(err, res, body);
            });

        }, this.OSRM_TIMEOUT);
    }
}
