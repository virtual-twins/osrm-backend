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
            console.log("RUNNING LAUNCH");
            this.osrmUp(() => {
                console.log("IS UP");
                this.waitForConnection(cb);
            });
        };

        runLaunch(limit((e) => { if (e) callback(e); callback(); }));
    }

    shutdown () {
        var limit = Timeout(this.scope.SHUTDOWN_TIMEOUT, { err: this.scope.RoutedError('Shutting down osrm-routed timed out.')});

        var runShutdown = (cb) => {
            this.osrmDown(cb);
        };

        runShutdown(limit((e) => { if (e) throw e; }));
    }

    osrmUp (callback) {
        // TODO i feel super dubious about this
        console.log('hello? HELLOOOOO?');
        callback(true);
        // if (this.pid) return !!waitpid(this.pid);
    }

    osrmDown (callback) {
        // TODO less but still dubious
        if (this.pid) process.kill(this.pid, this.scope.TERMSIGNAL);
        callback();
    }

    kill () {
        if (this.pid) process.kill(this.pid, 'SIGKILL');
    }

    waitForConnection (callback) {
        var socket = net.connect({
            port: this.scope.OSRM_PORT,
            host: '127.0.0.1'
        })
            .on('connect', (c) => {
                console.log("HEY HEY< CONNECTED");
                callback();
            })
            .on('error', (e) => {
                console.log('errrrrr error', e);
                setTimeout(callback, 100);
            });
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
        this.shutdown(() =>
            this.launch(() =>
                // callback(() =>
                this.shutdown(callback)));
    }

    osrmUp (callback) {
        console.log("HELLO IS UP");
        if (this.pid) return callback();
        var writeToLog = (data) => {
            console.error('ACK', data.toString());
            fs.appendFileSync(this.scope.OSRM_ROUTED_LOG_FILE, data);
        }

        var child = spawn(util.format('%s%s/osrm-routed', this.scope.LOAD_LIBRARIES, this.scope.BIN_PATH), [this.input_file, util.format('-p%d', this.scope.OSRM_PORT)], {detached: true});
        this.pid = child.pid;
        child.stdout.on('data', writeToLog);
        child.stderr.on('data', writeToLog);

        callback();
    }
}

var OSRMDatastoreLoader = class extends OSRMBaseLoader {
    constructor (scope) {
        super(scope);
    }

    load (inputFile, callback) {
        this.inputFile = inputFile;
        var startDir = process.cwd();
        this.loadData(() => {
            if (!this.pid) return this.launch(callback)});
    }

    loadData (callback) {
        this.scope.runBin('osrm-datastore', this.inputFile, callback);
    }

    osrmUp (callback) {
        // TODO 'return if osrm_up?'
        console.log("HELLOOOOOO IS UP");
        if (this.pid) return callback();
        var writeToLog = (data) => {
            console.error('ACK', data.toString());
            fs.appendFileSync(this.scope.OSRM_ROUTED_LOG_FILE, data);
        }

        var child = spawn(util.format('%s%s/osrm-routed', this.scope.LOAD_LIBRARIES, this.scope.BIN_PATH), ['--shared-memory=1', util.format('-p%d', this.scope.OSRM_PORT)], {detached: true});
        console.log(child);
        this.pid = child.pid;
        child.stdout.on('data', writeToLog);
        child.stderr.on('data', writeToLog);

        callback();
    }
}

module.exports = {
    OSRMBaseLoader: OSRMBaseLoader,

    _OSRMLoader: class {
        constructor (scope) {
            this.scope = scope;
            this.loader = null;
        }

        load (inputFile, block, callback) {
            console.log("LOAD CALLED")
            var method = this.scope.loadMethod,
                loader;
            if (method === 'datastore') {
                this.loader = new OSRMDatastoreLoader(this.scope);
                this.loader.load(inputFile, block, callback);
            } else if (method === 'directly') {
                this.loader = new OSRMDirectLoader(this.scope);
                this.loader.load(inputFile, block, callback);
            } else {
                throw new Error('*** Unknown load method ' + method);
            }
        }

        shutdown () {
            if (!this.loader) {
                console.error('what there is no loader?');
            }
            this.loader.shutdown();
        }
    }
}
