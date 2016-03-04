'use strict';

var fs = require('fs');
var net = require('net');
var spawn = require('child_process').spawn;
var util = require('util');
var Timeout = require('node-timeout');
// var waitpid = require('waitpid');

var OSRMBaseLoader = class {
    constructor (scope) {
        this.scope = scope;
        this.pid = null;
    }

    launch (callback) {
        var limit = Timeout(this.scope.LAUNCH_TIMEOUT, { err: this.scope.RoutedError('Launching osrm-routed timed out.') });

        var runLaunch = (cb) => {
            this.osrmUp(() => {
                this.waitForConnection(cb);
            });
        };

        runLaunch(limit((e) => { if (e) callback(e); callback(); }));
    }

    shutdown (callback) {
        var limit = Timeout(scope.SHUTDOWN_TIMEOUT, { err: this.scope.RoutedError('Shutting down osrm-routed timed out.')});

        var runShutdown = (cb) => {
            this.osrmDown(cb);
        };

        runShutdown(limit((e) => { if (e) callback(e); callback(); }));
    }

    osrmUp () {
        // TODO i feel super dubious about this
        console.log('hello?');
        return true;
        // if (this.pid) return !!waitpid(this.pid);
    }

    osrmDown () {
        // TODO less but still dubious
        if (this.pid) process.kill(this.pid, this.scope.TERMSIGNAL);
    }

    kill () {
        if (this.pid) process.kill(this.pid, 'KILL');
    }

    waitForConnection (callback) {
        var socket = net.connect({
            port: this.scope.OSRM_PORT,
            host: '127.0.0.1'
        })
            .on('connect', callback)
            .on('error', () => setTimeout(callback, 100));
    }

    waitForShutdown (callback) {
        // TODO i'm dubious ab this too
        var up = this.osrmUp(),
            check = () => {
                up = this.osrmUp();
            };
        while (up) {
            setTimeout(check, 100);
        }
        callback();
    }
}

var OSRMDirectLoader = class extends OSRMBaseLoader {
    constructor (scope) {
        super(scope);
    }

    load (inputFile, callback) {
        this.inputFile = inputFile;
        var startDir = process.cwd();
        // process.chdir(this.scope.TEST_FOLDER);
        this.shutdown(() =>
            this.launch(() =>
                callback(() =>
                        this.shutdown(() => {
                            // process.chdir(startDir);
                        })
                    )));
    }

    osrmUp () {
        if (this.pid) return;
        var writeToLog = (data) => {
            console.error('ACK', data.toString());
            fs.appendFileSync(this.scope.OSRM_ROUTED_LOG_FILE, data);
        }

        var child = spawn(util.format('%s%s/osrm-routed', this.scope.LOAD_LIBRARIES, this.scope.BIN_PATH), [this.input_file, util.format('--port "%d"', this.scope.OSRM_PORT)], {detached: true});
        child.stdout.on('data', writeToLog);
        child.stderr.on('data', writeToLog);

        // TODO return sth??
    }
}

var OSRMDatastoreLoader = class extends OSRMBaseLoader {
    constructor (scope) {
        super(scope);
    }

    load (inputFile, callback) {
        this.inputFile = inputFile;
        var startDir = process.cwd();
        // process.chdir(this.scope.TEST_FOLDER);
        this.loadData(() => {
            if (!this.pid) return this.launch(() =>
                callback(() => {
                    // process.chdir(startDir);
                }))});
    }

    loadData (callback) {
        this.scope.runBin('osrm-datastore', this.inputFile, callback);
    }

    osrmUp () {
        // TODO 'return if osrm_up?'
        if (this.pid) return;
        var writeToLog = (data) => {
            console.error('ACK', data.toString());
            fs.appendFileSync(this.scope.OSRM_ROUTED_LOG_FILE, data);
        }

        var child = spawn(util.format('%s%s/osrm-routed', this.scope.LOAD_LIBRARIES, this.scope.BIN_PATH), ['--shared-memory=1', util.format('--port "%d"', this.scope.OSRM_PORT)], {detached: true});
        child.stdout.on('data', writeToLog);
        child.stderr.on('data', writeToLog);

        // TODO return sth??
    }
}

module.exports = {
    OSRMBaseLoader: OSRMBaseLoader,

    _OSRMLoader: class {
        constructor (scope) {
            this.scope = scope;
        }

        load (inputFile, block, callback) {
            var method = this.scope.loadMethod,
                loader;
            if (method === 'datastore') {
                loader = new OSRMDatastoreLoader(this.scope);
                loader.load(inputFile, block, callback);
            } else if (method === 'directly') {
                loader = new OSRMDirectLoader(this.scope);
                loader.load(inputFile, block, callback);
            } else {
                throw new Error('*** Unknown load method ' + method);
            }
        }
    }
}
