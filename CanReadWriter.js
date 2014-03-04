var canReadWriter = require('./build/Release/canReadWriter');

var CanReadWriter = module.exports = function() {
    // init stuff
};

CanReadWriter.prototype.multiply = function(a, b) {
    return canReadWriter.multiply(a, b);
}
