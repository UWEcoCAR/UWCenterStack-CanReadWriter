var assert = require('chai').assert;
var CanReadWriter = require('./CanReadWriter');
var canReadWriter = new CanReadWriter();

assert.equal(canReadWriter.multiply(5, 6), 30, 'multiply failed');

console.log('----- All Tests Passed -----');
