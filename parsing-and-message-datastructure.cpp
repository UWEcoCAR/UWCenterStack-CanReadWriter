struct signalMsg {
    String Name;
    int startBit
    int lengthBit
    double scale;
    int offset;
    String Identifier
    signalMsg(String nameSig, int startInt, int lengthInt, double scaleInt, int offsetInt, String unitId) 
    : Name(nameSig), startBit(startInt), lengthBit(lengthInt), scale(scaleInt), offset(offsetInt), Identifier(unitId)  { }
};

multimap<int, signalMsg> createIdSignalMap() {
	multimap<int, signalMsg> m;
	m.insert(pair<int, signalMsg>("1956", signalMsg("UW_Motor_Speed", 0, 16, 1, 0, "rpm")));
	m.insert(pair<int, signalMsg>("1956", signalMsg("UW_Motor_Torque", 16, 16, 0.1, 0), "Nm"));
	m.insert(pair<int, signalMsg>("1956", signalMsg("UW_Accel_Pedal_Position", 39, 8, 0.392156862745098, 0, "%")));
	m.insert(pair<int, signalMsg>("1956", signalMsg("UW_Brake_On_Off", 47, 1, 1, 0, "")));
	m.insert(pair<int, signalMsg>("1956", signalMsg("UW_Commanded_Gear", 46, 4, 1, 0, "")));
	m.insert(pair<int, signalMsg>("1956", signalMsg("UW_Gear_Ratio",55, 8, 0.03125, 0, "")));

	m.insert(pair<int, signalMsg>("1957", signalMsg("UW_BRUSA_AC_V", 7, 16, 0.1, 0, "V")));
	m.insert(pair<int, signalMsg>("1957", signalMsg("UW_BRUSA_AC_I", 23, 16, 0.01, 0, "A")));

	m.insert(pair<int, signalMsg>("1955", signalMsg("UW_Motor_Temperature", 7, 16, 0.1, 0, "degC")));
	m.insert(pair<int, signalMsg>("1955", signalMsg("UW_Vehicle_Speed", 23, 15, 0.015625, 0, "km / h")));
	m.insert(pair<int, signalMsg>("1955", signalMsg("UW_Engine_Speed", 39, 16, 0.25, 0), "rpm"));
	m.insert(pair<int, signalMsg>("1955", signalMsg("UW_Engine_Torque",55, 12, 0.5, -848, "Nm")));

	m.insert(pair<int, signalMsg>("1954", signalMsg("UW_ESS_ibat", 7, 16, 0.025, -1000, "amps")));
	m.insert(pair<int, signalMsg>("1954", signalMsg("UW_ESS_vbat", 23, 12, 0.25, 0, "volts")));
	m.insert(pair<int, signalMsg>("1954", signalMsg("UW_ESS_cell_tmax", 27, 8, 0.5, -40, "DegC")));
	m.insert(pair<int, signalMsg>("1954", signalMsg("UW_ESS_SOC", 35, 8, 0.5, 0, "%")));
	m.insert(pair<int, signalMsg>("1954", signalMsg("UW_Eng_Temp", 43, 8, 1, -40, "deg C")));

}

// BO_ 1956 UW_Competition_Signals_3: 8 HSCU
//  SG_ UW_Gear_Ratio : 55|8@0+ (0.03125,0) [0|7.96875] "" Vector__XXX

//  SG_ UW_Commanded_Gear : 46|4@0+ (1,0) [0|15] "" Vector__XXX
//  SG_ UW_Brake_On_Off : 47|1@0+ (1,0) [0|0] "" Vector__XXX

//  SG_ UW_Accel_Pedal_Position : 39|8@0+ (0.392156862745098,0) [0|100] "%" Vector__XXX
//  SG_ UW_Motor_Torque : 16|16@1- (0.1,0) [-3276.8|3276.7] "Nm" Vector__XXX

//  SG_ UW_Motor_Speed : 0|16@1- (1,0) [-32768|32767] "rpm" Vector__XXX


// BO_ 1957 UW_Competition_Signals_4: 8 HSCU
//  SG_ UW_BRUSA_AC_I : 23|16@0+ (0.01,0) [0|50] "A" Vector__XXX

//  SG_ UW_BRUSA_AC_V : 7|16@0+ (0.1,0) [0|500] "V" Vector__XXX

// BO_ 1955 UW_Competition_Signals_2: 8 HSCU

//  SG_ UW_Engine_Torque : 55|12@0+ (0.5,-848) [-848|1199.5] "Nm" Vector__XXX
//  SG_ UW_Engine_Speed : 39|16@0+ (0.25,0) [0|16383.75] "rpm" Vector__XXX

//  SG_ UW_Vehicle_Speed : 23|15@0+ (0.015625,0) [0|511.984375] "km / h" Vector__XXX
//  SG_ UW_Motor_Temperature : 7|16@0- (0.1,0) [-3276.8|3276.7] "degC" Vector__XXX


// BO_ 1954 UW_Competition_Signals_1: 8 HSCU
//  SG_ UW_Eng_Temp : 43|8@0+ (1,-40) [-40|215] "deg C" Vector__XXX

//  SG_ UW_ESS_SOC : 35|8@0+ (0.5,0) [0|100] "%" Vector__XXX
//  SG_ UW_ESS_cell_tmax : 27|8@0+ (0.5,-40) [-40|85] "DegC" Vector__XXX

//  SG_ UW_ESS_vbat : 23|12@0+ (0.25,0) [0|1000] "volts" Vector__XXX
//  SG_ UW_ESS_ibat : 7|16@0+ (0.025,-1000) [-1000|500] "amps" Vector__XXX