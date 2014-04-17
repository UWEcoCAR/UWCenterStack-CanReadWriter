var canReadWriter = require('./build/Release/canReadWriter');
var events = require('events');
var util = require('util');

var CanReadWriter = module.exports = function() {
	var self = this;
    return canReadWriter.start(function(name, value) {
		 self.emit(name, value);
	});
};

util.inherits(CanReadWriter, events.EventEmitter);

CanReadWriter.prototype.write = canReadWriter.write;