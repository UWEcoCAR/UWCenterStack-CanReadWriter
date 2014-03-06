var assert = require('chai').assert; // did not really need to use this for my step
var events = require('events');
var eventEmitter = new events.EventEmitter();
var CanReadWriter = require('./CanReadWriter');
var canReadWriter = new CanReadWriter();

// registering 2 callbacks for a "voltage" signal
canReadWriter.on("voltage", function(a) { console.log(a); });
canReadWriter.on("voltage", function(b) { console.log(b+1); });

// initialize with the given callback that will execute all listeners for the
// "voltage" event. This will output 5 and then 6 in the console.

// TODO: was unable to just replace canReadWriter.emit for _Emit. Needed a wrapper
//       function. Will continue to look into this.
var _Emit = function(a, b) {
  canReadWriter.emit(a, b);
}
canReadWriter.Init(_Emit);

