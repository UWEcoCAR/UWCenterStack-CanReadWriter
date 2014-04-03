var CanReadWriter = require('./CanReadWriter');
var canReadWriter = new CanReadWriter();
var _ = require('underscore');

console.log("---- Starting Tests ----");

canReadWriter.write('ventFanSpeed', 100);