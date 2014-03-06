var canReadWriter = require('./build/Release/canReadWriter');
var events = require('events');
var util = require('util');
var eventsEmitter = new events.EventEmitter();

var CanReadWriter = module.exports = function() {
    // init stuff
};

util.inherits(CanReadWriter, events.EventEmitter);

// will run the initialization in C++. takes in the callback to be run by C++ once
// a meaningful message is found
CanReadWriter.prototype.Init = function(func) {
  return canReadWriter.readResult(func);
}
