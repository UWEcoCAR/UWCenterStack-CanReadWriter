var assert = require('chai').assert; // did not really need to use this for my step
var events = require('events');
var eventEmitter = new events.EventEmitter();
var CanReadWriter = require('./CanReadWriter');
var canReadWriter = new CanReadWriter();

// registering 2 callbacks for a "voltage" signal
canReadWriter.registerReadCallback("voltage", function(a) { console.log(a); });
canReadWriter.registerReadCallback("voltage", function(b) { console.log(b+1); });

// initialize with the given callback that will execute all listeners for the
// "voltage" event. This will output 5 and then 6 in the console.
canReadWriter.init(canReadWriter.executeReadCallbacks);

