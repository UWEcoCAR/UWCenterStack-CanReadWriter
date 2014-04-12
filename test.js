var CanReadWriter = require('./CanReadWriter');
var canReadWriter = new CanReadWriter();
var _ = require('underscore');

console.log("---- Starting Tests ----");

setInterval(function() { canReadWriter.write('ventFanSpeed', 0); }, 1000);