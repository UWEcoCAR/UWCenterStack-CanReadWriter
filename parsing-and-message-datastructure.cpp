#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstring>

using namespace std;

// Struct definition plus constructor for struct
struct signalDef {
	public:
	    string name;
	    int startBit;
	    int length;
	    double scale;
	    int offset;
	    string identifier;

	    signalDef(string nameSig, int startInt, int lengthInt, double scaleInt, int offsetInt, string unitId) : 
	    name(nameSig), 
	    startBit(startInt), 
	    length(lengthInt), 
	    scale(scaleInt), 
	    offset(offsetInt), 
	    identifier(unitId)  { }
};

// Define signalMap data structure
// Keys are ints and values are signalDef types
typedef unordered_multimap<int, signalDef> signalMap;

// Creates a signalMap of ints and vectors
signalMap createSignalMap() {

	signalMap m = {

		{1954, signalDef("batteryCurrent", 7, 16, 0.025, -1000, "amps")},
		{1954, signalDef("batteryVoltage", 23, 12, 0.25, 0, "volts")},
		{1954, signalDef("batteryTemp", 27, 8, 0.5, -40, "DegC")},
		{1954, signalDef("batterySoc", 35, 8, 0.5, 0, "%")},
		{1954, signalDef("engineTemp", 43, 8, 1, -40, "deg C")},
	
		
		{1955, signalDef("engineTorque",55, 12, 0.5, -848, "Nm")},
		{1955, signalDef("engineRpm", 39, 16, 0.25, 0, "rpm")},
		{1955, signalDef("vehicleSpeed", 23, 15, 0.015625, 0, "km / h")},
		{1955, signalDef("motorTemp", 7, 16, 0.1, 0, "degC")},

		{1956, signalDef("transRatio", 55, 8, 0.03125, 0, "")},
		{1956, signalDef("transGear", 46, 4, 1, 0, "")},
		{1956, signalDef("vehicleBrake", 47, 1, 1, 0, "")},
		{1956, signalDef("vehicleAccel", 39, 8, 0.392156862745098, 0, "%")},
		{1956, signalDef("motorTorque", 16, 16, 0.1, 0, "Nm")},
		{1956, signalDef("motorRpm", 0, 16, 1, 0, "rpm")},

		{1957, signalDef("chargerCurrent", 23, 16, 0.01, 0, "A")},
		{1957, signalDef("chargerVoltage", 7, 16, 0.1, 0, "V")},
	};

	return m;
}

// Takes an id and byte array and prints out the corresponding signal definitions
void parse(signalMap m, unsigned long id, unsigned char message[], unsigned int byteLength) { 
	int length = byteLength;
	int mask;
	double signal;
	unsigned long data = 0;
	for (int i = 0; i < length; i++) {
    	data += ((unsigned long)message[i] << (i*8));
  	}
  	cout << data << endl;
  	auto range = m.equal_range(id);
	for (auto it = range.first; it != range.second; ++it) {
		signalDef ourSignal = it->second;
		mask = ((1 << ourSignal.length) - 1) << ourSignal.startBit;
		signal = (data & mask) >> ourSignal.startBit;
		signal = signal * ourSignal.scale;
		signal = signal + ourSignal.offset;
		cout <<" Signal "<< ourSignal.name <<":  Start Bit- " <<ourSignal.startBit<<"  Length Bit- "<< ourSignal.length
		<< "  Scale- "<< ourSignal.scale <<"  Offset- " << ourSignal.offset
		<< " Signal- " << signal << " " << ourSignal.identifier <<endl;
	}
}

// For testing purposes
// Run ./"filename".out to run main
int main() {
	signalMap m = createSignalMap();
	unsigned char bytes[8] ={155, 203, 87, 23, 219, 196, 32, 0};
	parse(m, 1954, bytes, 8);
	return 1;
}
