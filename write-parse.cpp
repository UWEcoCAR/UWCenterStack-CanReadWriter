#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

// Struct definition plus constructor for struct
struct messageDef {
  long id;
  unsigned long message;
  int startBit;
  int length;

  messageDef(long idInt, unsigned long ourMessage, int startInt, int ourLength) :
    id(idInt),
    message(ourMessage),
    startBit(startInt),
    length(ourLength) { }
};

// Struct to return later in parse method
struct canMessage {
  long id;
  unsigned char data[8];
  unsigned int length;
};

// Define messageMap data structure
// Keys are strings and values are messageDef types
typedef unordered_map<string, messageDef> messageMap;
messageMap createMessageMap() {
  messageMap m = {

    {"Diagnostic Message", messageDef(
      101, 0x000000003E01FE07, -1, 8)},

    {"A/C(On/Off)", messageDef(
      251, 0x000000010104AE07, -1, 8)},

    {"Auto Temp(On/Off)", messageDef(
      251, 0x000000080804AE07, -1, 8)},

    {"Recirculate(On/Off)", messageDef(
      251, 0x000000040404AE07, -1, 8)},

    {"Rear Defrost(On/Off)", messageDef(
      251, 0x000000101004AE07, -1, 8)},

    {"Front Defrost(On/Off)", messageDef(
      251, 0x000101000004AE07, -1, 8)},

    {"Vent Top(On/Off)", messageDef(
      251, 0x000000404004AE07, -1, 8)},

    {"Vent Floor(On/Off)", messageDef(
      251, 0x000000808004AE07, -1, 8)},

    {"Vent Fan Speed", messageDef(
      251, 0x000000000802AE07, 56, 8)},

    {"Temp Inc/Dec Driver", messageDef(
      251, 0x000000000102AE07, 32, 8)},

    {"Temp Inc/Dec Passenger", messageDef(
      251, 0x000000000202AE07, 32, 8)}
  };

  return m;
}

// Takes a value and string of a message
// Returns a canMessage struct with an updated byte value to be written
canMessage* parse(messageMap m, string name, unsigned long value) {
  canMessage* c = new canMessage;
  int startBit = m.at(name).startBit;
  c->id = m.at(name).id;
  c->length = m.at(name).length;
  unsigned long message = m.at(name).message;
  if (startBit != -1) {
    message = (message + (value << startBit));
  }
  for (int i = 0; i < c->length; i++) {
    c->data[i] = (unsigned char)message;
    message = message >> 8;
  }
  // cout <<message<<endl;
  return c;
}

// For testing
int main(){
  messageMap m = createMessageMap();
  canMessage* c = parse(m, "Vent Fan Speed", 50);
  for (int i = 0; i < 8; i++)
    cout << (int)c->data[i];
  return 1;
}
