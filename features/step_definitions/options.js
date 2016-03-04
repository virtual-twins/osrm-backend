// 'use strict';

var assert = require('assert');

module.exports = function () {
    // this.World = require('../support/world');
    // this.World();

    this.When(/^I run "osrm\-routed\s?(.*?)"$/, { timeout: this.SHUTDOWN_TIMEOUT }, (options) => {
        this.runBin('osrm-routed', options);
        // TODO does this timeout work like this? RB version was
        // begin
        //   Timeout.timeout(SHUTDOWN_TIMEOUT) { run_bin 'osrm-routed', options }
        // rescue Timeout::Error
        //   raise "*** osrm-routed didn't quit. Maybe the --trial option wasn't used?"
        // end
    });

    this.When(/^I run "osrm\-extract\s?(.*?)"$/, (options) => {
        this.runBin('osrm-extract', options);
    });

    this.When(/^I run "osrm\-prepare\s?(.*?)"$/, (options) => {
        this.runBin('osrm-prepare', options);
    });

    this.When(/^I run "osrm\-datastore\s?(.*?)"$/, (options) => {
        this.runBin('osrm-datastore', options)
    });

    this.Then(/^it should exit with code (\d+)$/, (code) => {
        assert.equal(this.exitCode, parseInt(code));
    });

    this.Then(/^stdout should contain "(.*?)"$/, (str) => {
        assert.ok(this.stdout.indexOf(str) > -1);
    });

    this.Then(/^stderr should contain "(.*?)"$/, (str) => {
        assert.ok(this.stdout.indexOf(str) > -1);
    });

    this.Then(/^stdout should contain \/(.*)\/$/, (regexStr) => {
        var re = new RegExp(regexStr);
        assert.ok(this.stdout.match(re));
    });

    this.Then(/^stderr should contain \/(.*)\/$/, (regexStr) => {
        var re = new RegExp(regexStr);
        assert.ok(this.stdout.match(re));
    });

    this.Then(/^stdout should be empty$/, () => {
        assert.equal(this.stdout.trim(), '');
    });

    this.Then(/^stderr should be empty$/, () => {
        assert.equal(this.stderr.trim(), '');
    });

    this.Then(/^stdout should contain (\d+) lines?$/, (lines) => {
        assert.equal(this.stdout.lines.length, parseInt(lines));
    });

    this.Given(/^the query options$/, (table) => {
        table.rowsHash.forEach((k, v) => {
            this.queryParams.push([k, v]);
        });
    });
}
