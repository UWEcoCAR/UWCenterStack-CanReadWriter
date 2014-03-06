#include <iostream>
#include <vector>
#include <map>
#include <cstring>

using namespace std;

// Struct definition plus constructor for struct
struct signalDef {
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
// Keys are ints and values are signalDefVec types
typedef vector<signalDef> signalDefVec;
typedef std::map<int, signalDefVec> signalMap;


// Creates a signalMap of ints and vectors
signalMap createSignalMap() {
	string batteryVoltage = "UW_ESS_vbat";
    string batteryCurrent = "UW_ESS_ibat";
    string batterySoc = "UW_ESS_SOC";
    string batteryTemp = "UW_ESS_cell_tmax";
    string motorRpm = "UW_Motor_Speed";
    string motorTorque = "UW_Motor_Temperature";
    string motorTemp = "UW_Motor_Temperature";
    string transGear = "UW_Commanded_Gear";
    string transRatio = "UW_Gear_Ratio";
    string engineRpm = "UW_Engine_Speed";
    string engineTorque = "UW_Engine_Torque";
    string engineTemp = "UW_Eng_Temperature";
    string vehicleAccel = "UW_Accel_Pedal_Position";
    string vehicleBrake = "UW_Accel_Pedal_Position";
    string vehicleSpeed = "UW_Vehicle_Speed";
    string chargerVoltage = "UW_BRUSA_AC_V";
    string chargerCurrent = "UW_BRUSA_AC_I";

	std::vector<signalDef> id1957;
	id1957.insert(id1957.begin(), signalDef(chargerCurrent, 23, 16, 0.01, 0, "A"));
	id1957.insert(id1957.begin(), signalDef(chargerVoltage, 7, 16, 0.1, 0, "V"));

	std::vector<signalDef> id1954;
	id1954.insert(id1954.begin(), signalDef(engineTemp, 43, 8, 1, -40, "deg C"));
	id1954.insert(id1954.begin(), signalDef(batterySoc, 35, 8, 0.5, 0, "%"));
	id1954.insert(id1954.begin(), signalDef(batteryTemp, 27, 8, 0.5, -40, "DegC"));
	id1954.insert(id1954.begin(), signalDef(batteryVoltage, 23, 12, 0.25, 0, "volts"));
	id1954.insert(id1954.begin(), signalDef(batteryCurrent, 7, 16, 0.025, -1000, "amps"));

	std::vector<signalDef> id1955;
	id1955.insert(id1955.begin(), signalDef(engineTorque,55, 12, 0.5, -848, "Nm"));
	id1955.insert(id1955.begin(), signalDef(engineRpm, 39, 16, 0.25, 0, "rpm"));
	id1955.insert(id1955.begin(), signalDef(vehicleSpeed, 23, 15, 0.015625, 0, "km / h"));
	id1955.insert(id1955.begin(), signalDef(motorTemp, 7, 16, 0.1, 0, "degC"));

	std::vector<signalDef> id1956;
	id1956.insert(id1956.begin(), signalDef(transRatio,55, 8, 0.03125, 0, ""));
	id1956.insert(id1956.begin(), signalDef(transGear, 46, 4, 1, 0, ""));
	id1956.insert(id1956.begin(), signalDef(vehicleBrake, 47, 1, 1, 0, ""));
	id1956.insert(id1956.begin(), signalDef(vehicleAccel, 39, 8, 0.392156862745098, 0, "%"));
	id1956.insert(id1956.begin(), signalDef(motorTorque, 16, 16, 0.1, 0, "Nm"));
	id1956.insert(id1956.begin(), signalDef(motorRpm, 0, 16, 1, 0, "rpm"));
	
	signalMap m;

	m.insert(std::pair<int, vector<signalDef> >(1954, id1954));
	m.insert(std::pair<int, vector<signalDef> >(1955, id1955));
	m.insert(std::pair<int, vector<signalDef> >(1956, id1956));
	m.insert(std::pair<int, vector<signalDef> >(1957, id1957));

	return m;
}

// Takes an id and prints out the corresponding signal definitions
// TODO: Accept an unsigned char array and turn into a long
void parse(signalMap m, int id, long bytes) { 
	int data = bytes;
	vector<signalDef> signalData;
	int mask;
	double signal;

	switch (id) {

		//Print ids for 1954
		case 1954:
			signalData = m[1954];

			cout << "BO_ " << id << " UW_Competition_Signals_1: 8 HSCU "<< endl;
			
			mask = ((1 << signalData[0].length) - 1) << signalData[0].startBit;
			signal = (data & mask) >> signalData[0].startBit;
			signal = signal * signalData[0].scale;
			signal = signal + signalData[0].offset;
			cout <<" SG_ "<< signalData[0].name <<": " <<signalData[0].startBit<<"|"<< signalData[0].length
			<< "@0+ ("<<signalData[0].scale<<signalData[0].offset<<") "
			<< signal << " \"" << signalData[0].identifier << "\" Vector__XXX" <<endl;

			// signal 2
			mask = ((1 << signalData[1].length) - 1) << signalData[1].startBit;
			signal = (data & mask) >> signalData[1].startBit;
			signal = signal * signalData[1].scale;
			signal = signal + signalData[1].offset;
			cout <<" SG_ "<< signalData[1].name <<": " <<signalData[1].startBit<<"|"<< signalData[1].length
			<< "@0+ ("<<signalData[1].scale<<signalData[1].offset<<") "
			<< signal << " \"" << signalData[1].identifier << "\" Vector__XXX" <<endl;
			// signal 3
			mask = ((1 << signalData[2].length) - 1) << signalData[2].startBit;
			signal = (data & mask) >> signalData[2].startBit;
			signal = signal * signalData[2].scale;
			signal = signal + signalData[2].offset;
			cout <<" SG_ "<< signalData[2].name <<": " <<signalData[2].startBit<<"|"<< signalData[2].length
			<< "@0+ ("<<signalData[2].scale<<signalData[2].offset<<") "
			<< signal << " \"" << signalData[2].identifier << "\" Vector__XXX" <<endl;

			// signal 4
			mask = ((1 << signalData[3].length) - 1) << signalData[3].startBit;
			signal = (data & mask) >> signalData[3].startBit;
			signal = signal * signalData[3].scale;
			signal = signal + signalData[3].offset;
			cout <<" SG_ "<< signalData[3].name <<": " <<signalData[3].startBit<<"|"<< signalData[3].length
			<< "@0+ ("<<signalData[3].scale<<signalData[3].offset<<") "
			<< signal << " \"" << signalData[3].identifier << "\" Vector__XXX" <<endl;

			mask = ((1 << signalData[4].length) - 1) << signalData[4].startBit;
			signal = (data & mask) >> signalData[4].startBit;
			signal = signal * signalData[4].scale;
			signal = signal + signalData[4].offset;
			cout <<" SG_ "<< signalData[4].name <<": " <<signalData[4].startBit<<"|"<< signalData[4].length
			<< "@0+ ("<<signalData[4].scale<<signalData[4].offset<<") "
			<< signal << " \"" << signalData[4].identifier << "\" Vector__XXX" <<endl;

			break;

		// print ids for 1955
		case 1955:
			signalData = m[1955];
			cout << "BO_ " << id << " UW_Competition_Signals_2: 8 HSCU "<< endl;

			mask = ((1 << signalData[0].length) - 1) << signalData[0].startBit;
			signal = (data & mask) >> signalData[0].startBit;
			signal = signal * signalData[0].scale;
			signal = signal + signalData[0].offset;
			cout <<" SG_ "<< signalData[0].name <<": " <<signalData[0].startBit<<"|"<< signalData[0].length
			<< "@0+ ("<<signalData[0].scale<<signalData[0].offset<<") "
			<< signal << " \"" << signalData[0].identifier << "\" Vector__XXX" <<endl;

			// signal 2
			mask = ((1 << signalData[1].length) - 1) << signalData[1].startBit;
			signal = (data & mask) >> signalData[1].startBit;
			signal = signal * signalData[1].scale;
			signal = signal + signalData[1].offset;
			cout <<" SG_ "<< signalData[1].name <<": " <<signalData[1].startBit<<"|"<< signalData[1].length
			<< "@0+ ("<<signalData[1].scale<<signalData[1].offset<<") "
			<< signal << " \"" << signalData[1].identifier << "\" Vector__XXX" <<endl;
			// signal 3
			mask = ((1 << signalData[2].length) - 1) << signalData[2].startBit;
			signal = (data & mask) >> signalData[2].startBit;
			signal = signal * signalData[2].scale;
			signal = signal + signalData[2].offset;
			cout <<" SG_ "<< signalData[2].name <<": " <<signalData[2].startBit<<"|"<< signalData[2].length
			<< "@0+ ("<<signalData[2].scale<<signalData[2].offset<<") "
			<< signal << " \"" << signalData[2].identifier << "\" Vector__XXX" <<endl;

			// signal 4
			mask = ((1 << signalData[3].length) - 1) << signalData[3].startBit;
			signal = (data & mask) >> signalData[3].startBit;
			signal = signal * signalData[3].scale;
			signal = signal + signalData[3].offset;
			cout <<" SG_ "<< signalData[3].name <<": " <<signalData[3].startBit<<"|"<< signalData[3].length
			<< "@0+ ("<<signalData[3].scale<<signalData[3].offset<<") "
			<< signal << " \"" << signalData[3].identifier << "\" Vector__XXX" <<endl;

			break;

		// Print signals for id 1956
		case 1956:
			signalData = m[1956];

			cout << "BO_ " << id << " UW_Competition_Signals_3: 8 HSCU "<< endl;
			// signal 1
			mask = ((1 << signalData[0].length) - 1) << signalData[0].startBit;
			signal = (data & mask) >> signalData[0].startBit;
			signal = signal * signalData[0].scale;
			signal = signal + signalData[0].offset;
			cout <<" SG_ "<< signalData[0].name <<": " <<signalData[0].startBit<<"|"<< signalData[0].length
			<< "@0+ ("<<signalData[0].scale<<signalData[0].offset<<") "
			<< signal << " \"" << signalData[0].identifier << "\" Vector__XXX" <<endl;

			// signal 2
			mask = ((1 << signalData[1].length) - 1) << signalData[1].startBit;
			signal = (data & mask) >> signalData[1].startBit;
			signal = signal * signalData[1].scale;
			signal = signal + signalData[1].offset;
			cout <<" SG_ "<< signalData[1].name <<": " <<signalData[1].startBit<<"|"<< signalData[1].length
			<< "@0+ ("<<signalData[1].scale<<signalData[1].offset<<") "
			<< signal << " \"" << signalData[1].identifier << "\" Vector__XXX" <<endl;
			// signal 3
			mask = ((1 << signalData[2].length) - 1) << signalData[2].startBit;
			signal = (data & mask) >> signalData[2].startBit;
			signal = signal * signalData[2].scale;
			signal = signal + signalData[2].offset;
			cout <<" SG_ "<< signalData[2].name <<": " <<signalData[2].startBit<<"|"<< signalData[2].length
			<< "@0+ ("<<signalData[2].scale<<signalData[2].offset<<") "
			<< signal << " \"" << signalData[2].identifier << "\" Vector__XXX" <<endl;

			// signal 4
			mask = ((1 << signalData[3].length) - 1) << signalData[3].startBit;
			signal = (data & mask) >> signalData[3].startBit;
			signal = signal * signalData[3].scale;
			signal = signal + signalData[3].offset;
			cout <<" SG_ "<< signalData[3].name <<": " <<signalData[3].startBit<<"|"<< signalData[3].length
			<< "@0+ ("<<signalData[3].scale<<signalData[3].offset<<") "
			<< signal << " \"" << signalData[3].identifier << "\" Vector__XXX" <<endl;
			// signal 5
			mask = ((1 << signalData[4].length) - 1) << signalData[4].startBit;
			signal = (data & mask) >> signalData[4].startBit;
			signal = signal * signalData[4].scale;
			signal = signal + signalData[4].offset;
			cout <<" SG_ "<< signalData[4].name <<": " <<signalData[4].startBit<<"|"<< signalData[4].length
			<< "@0+ ("<<signalData[4].scale<<signalData[4].offset<<") "
			<< signal << " \"" << signalData[4].identifier << "\" Vector__XXX" <<endl;
			// signal 6
			mask = ((1 << signalData[5].length) - 1) << signalData[5].startBit;
			signal = (data & mask) >> signalData[5].startBit;
			signal = signal * signalData[5].scale;
			signal = signal + signalData[5].offset;
			cout <<" SG_ "<< signalData[5].name <<": " <<signalData[4].startBit<<"|"<< signalData[4].length
			<< "@0+ ("<<signalData[5].scale<<signalData[5].offset<<") "
			<< signal << " \"" << signalData[5].identifier << "\" Vector__XXX" <<endl;

			break;

		// print ids for 1954
		case 1957:
			signalData = m[1957];
			cout << "BO_ " << id << " UW_Competition_Signals_4: 8 HSCU "<< endl;

			// signal 1
			mask = ((1 << signalData[0].length) - 1) << signalData[0].startBit;
			signal = (data & mask) >> signalData[0].startBit;
			signal = signal * signalData[0].scale;
			signal = signal + signalData[0].offset;
			cout <<" SG_ "<< signalData[0].name <<": " <<signalData[0].startBit<<"|"<< signalData[0].length
			<< "@0+ ("<<signalData[0].scale<<signalData[0].offset<<") "
			<< signal << " \"" << signalData[0].identifier << "\" Vector__XXX" <<endl;

			// signal 2
			mask = ((1 << signalData[1].length) - 1) << signalData[1].startBit;
			signal = (data & mask) >> signalData[1].startBit;
			signal = signal * signalData[1].scale;
			signal = signal + signalData[1].offset;
			cout <<" SG_ "<< signalData[1].name <<": " <<signalData[1].startBit<<"|"<< signalData[1].length
			<< "@0+ ("<<signalData[1].scale<<signalData[1].offset<<") "
			<< signal << " \"" << signalData[1].identifier << "\" Vector__XXX" <<endl;

			break;
	}
}

// For testing purposes
// Run ./"filename".out to run main
int main() {
	signalMap m = createSignalMap();
	long bytes = 01110000101010100;
	parse(m, 1954, bytes);
	parse(m, 1955, bytes);
	parse(m, 1956, bytes);
	parse(m, 1957, bytes);

	return 1;
}
