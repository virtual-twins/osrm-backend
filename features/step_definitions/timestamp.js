// 'use strict';

var assert = require('assert');

module.exports = function () {
    // this.World = require('../support/world');
    // this.World();

    this.Then(/^I should get a valid timestamp/, () => {
        this.step('I should get a response');
        this.step('response should be valid JSON');
        this.step('response should be well-formed');
        assert.equal(typeof this.json.timestamp, 'string');
        assert.equal(this.json.timestamp, '2000-01-01T00:00:00Z');
    });
}
