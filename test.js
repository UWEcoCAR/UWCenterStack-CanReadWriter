var CanReadWriter = require('./CanReadWriter');
var canReadWriter = new CanReadWriter();
var _ = require('underscore');

console.log("---- Starting Tests ----");
canReadWriter.on('transGear', function(rpm) {
//    console.log(rpm);
});
canReadWriter.write('ventFanSpeed', 0);
setInterval(function() { 
    canReadWriter.write('diagnosticMode', 1);
    //canReadWriter.write('driverTemp', 0); 
    //canReadWriter.write('passengerTemp', 0); 
   // canReadWriter.write('toggleDefrost', 0);
    canReadWriter.writeHs('hvacCommand', 0);
}, 1000);