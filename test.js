var CanReadWriter = require('./CanReadWriter');
var canReadWriter = new CanReadWriter();
var _ = require('underscore');

console.log("---- Starting Tests ----");
var signals = ['batteryVoltage', 'batteryCurrent'];
_.each(signals, function(signal) {
	canReadWriter.on(signal, function(value) {
		console.log(signal + ': ' + value);
	});	
});

