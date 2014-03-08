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
    string unitId;

    signalDef(string nameSig, int startInt, int lengthInt, double scaleInt, int offsetInt, string ID) : 
    name(nameSig), 
    startBit(startInt), 
    length(lengthInt), 
    scale(scaleInt), 
    offset(offsetInt), 
    unitId(ID)  { }
};

// Define signalMap data structure
// Keys are ints and values are signalDef types
typedef unordered_multimap<int, signalDef> signalMap;

// Creates a signalMap of ints and vectors
signalMap createSignalMap() {

  signalMap m = {

    {1954, signalDef("batteryCurrent", 48, 16, 0.025, -1000, "amps")},
    {1954, signalDef("batteryVoltage", 36, 12, 0.25, 0, "volts")},
    {1954, signalDef("batteryTemp", 28, 8, 0.5, -40, "Deg C")},
    {1954, signalDef("batterySoc", 20, 8, 0.5, 0, "%")},
    {1954, signalDef("engineTemp", 12, 8, 1, -40, "Deg C")},

    
    {1955, signalDef("engineTorque", 4, 12, 0.5, -848, "Nm")},
    {1955, signalDef("engineRpm", 16, 16, 0.25, 0, "rpm")},
    {1955, signalDef("vehicleSpeed", 33, 15, 0.015625, 0, "km / h")},
    {1955, signalDef("motorTemp", 48, 16, 0.1, 0, "degC")},

    {1956, signalDef("transRatio", 8, 8, 0.03125, 0, "")},
    {1956, signalDef("transGear", 19, 4, 1, 0, "")},
    {1956, signalDef("vehicleBrake", 23, 1, 1, 0, "")},
    {1956, signalDef("vehicleAccel", 24, 8, 0.392156862745098, 0, "%")},
    {1956, signalDef("motorTorque", 32, 16, 0.1, 0, "Nm")},
    {1956, signalDef("motorRpm", 48, 16, 1, 0, "rpm")},

    {1957, signalDef("chargerCurrent", 32, 16, 0.01, 0, "A")},
    {1957, signalDef("chargerVoltage",48, 16, 0.1, 0, "V")},
  };

  return m;
}

// Takes an id and byte array and prints out the corresponding signal definitions
void parse(signalMap m, unsigned long id, unsigned char message[], unsigned int byteLength) { 
  int length = byteLength;
  unsigned long mask;
  double signal;
  unsigned long data = 0;
  for (int i = 0; i < length; i++) {
    data += ((unsigned long)message[i] << ((length-1-i)*8));
  }
  cout << data << endl;
  auto range = m.equal_range(id);
  for (auto it = range.first; it != range.second; ++it) {
    signalDef ourSignal = it->second;
    unsigned long ones = ((1 << ourSignal.length) - 1);
    mask = ones << ourSignal.startBit;
    signal = (data & mask) >> ourSignal.startBit;
    signal *= ourSignal.scale;
    signal += ourSignal.offset;
    // cout << "Our signal: \t" << signal << endl;
  }
}

// For testing purposes
// Run ./"filename".out to run main
// int main() {
//   signalMap m = createSignalMap();
//   unsigned char bytes[8] ={0x9C, 0x68, 0x00, 0xC5, 0x80, 0x43, 0xA0, 0x00};
//   // unsigned char bytes[8] ={0x00, 0x0A, 0x00, 0x80, 0x00, 0x04, 0x6A, 0x20};
//   parse(m, 1954, bytes, 8);
//   return 1;
// }
