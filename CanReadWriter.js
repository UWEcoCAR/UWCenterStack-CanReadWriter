var canReadWriter = require('./build/Release/canReadWriter');
var events = require('events');
var eventsEmitter = new events.EventEmitter();

var CanReadWriter = module.exports = function() {
    // init stuff
};

// will run the initialization in C++. takes in the callback to be run by C++ once
// a meaningful message is found
CanReadWriter.prototype.init = function(func) {
  return canReadWriter.readResult(func);
}

// executes the callbacks for the given signal with a parameter 
// that is the given value
CanReadWriter.prototype.executeReadCallbacks = function(signalName, value) {
  eventsEmitter.emit(signalName, value);
}


// will register the callback for the given signal to be ran later
CanReadWriter.prototype.registerReadCallback = function(signalName, callback) {
  eventsEmitter.on(signalName, callback);
}
