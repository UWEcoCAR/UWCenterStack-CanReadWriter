var canReadWriter = require('./build/Release/canReadWriter');
var events = require('events');
var util = require('util');

var CanReadWriter = module.exports = function() {
    var self = this;
    this._mailbox = {};
    return canReadWriter.start(function(name, value) {
        mailbox[name] = value;
        self.emit(name, value);
    });
};

util.inherits(CanReadWriter, events.EventEmitter);

CanReadWriter.prototype.write = canReadWriter.write;
CanReadWriter.prototype.getMail = function(address) {
    return this._mailbox[address];
};

if (process.env.CAN === 'false') {
    module.exports = function() {};
    module.exports.prototype.write = function() {};
    module.exports.prototype.getMail = function() {};
}