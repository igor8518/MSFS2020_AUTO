#include "CabinWork.h"


CabinWork::CabinWork(HANDLE hSimConnect) : HSimConnect(hSimConnect) {
	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(TimerProc()));
	Timer->start(60);
}


void CabinWork::CLPreliminaryCocpitPrep(int* Status)
{
	if (*Status != 2) {
		*Status = 1;
		WindowsAndDoors(5, 1);
		WindowsAndDoors(2, 1);
		Sleep(2000);
		SendEvent(KEY_TOGGLE_JETWAY, 1);
		EngineMasterSwitches(1, 0);
		EngineMasterSwitches(2, 0);
		EngineModeSelector(NORMAL);
		Wiper(1, 0);//More tests set potentiometer correctly
		Wiper(2, 0);//More tests
		BatterySwitch(1, 1);
		BatterySwitch(2, 1);
		ExternalPower(1);
		NavLight(1);
		CabinLight(1);
		APUFireTest();
		Sleep(2000);
		APUSet(1);
		NavLight(1);
		CabinLight(1);
		SetDataL(A32NX_INITFLIGHT_STATE, 1);
		ParkBrake = 1;
		//AccuPressCheck
		//BrakesPressCheck
		Flaps = 0;
		Spoilers = 0;
		ProbeWindowHeat(0);
		APUBleedSet(1);
		//CheckOverheadWhiteLights
		XBleedSet(1);
		//CargoHeatSelectorSet
		//ElecPanelCheck
		//VentPanelCheck
		//ECAMPages RCL DOOR HYD ENG
		//EmergencyEquipment
		//RainRepellent
		//ExteriorCheck
		*Status = 2;
	}
}


void CabinWork::CLCocpitPreparation(int* Status)
{
	if (*Status != 2) {
		*Status = 1;
		Sleep(2000);
		//OVERHEAD PANEL
		//RecorderGroundControll();
		//CvrTest();
		EvacCapt(1);
		ADIRS(0, 1);
		ADIRS(1, 1);
		ADIRS(2, 1);
		NavLight(1);
		NoSmokeSign(1);
		EmerExitSign(1);
		SeatBeltSign(1);
		Sleep(3000);
		SendEvent(KEY_TOGGLE_RAMPTRUCK, 1);
		Sleep(3000);
		SendEvent(KEY_REQUEST_CATERING, 0);
		Sleep(3000);
		SendEvent(KEY_REQUEST_FUEL, 0);
		Sleep(3000);
		SendEvent(KEY_REQUEST_LUGGAGE, 0);
		LandingElevation(0); //knob = (val + 2000) / 193.182 MAX = 88% min -2000; max 15000.016
		PackFlow(1);
		//ECAMPages ELEC
		BatterySwitch(1, 0);
		BatterySwitch(2, 0);
		Sleep(1000);
		BatterySwitch(1, 1);
		BatterySwitch(2, 1);
		//FuelModeSelector(MANUAL);
		EngFireTest(1);
		EngFireTest(2);
		//AudioSwitchingPanel(1)
		//PA ReceptionKnob
		//MAINTENANCE PANEL
		//AllLightsAreOut
		//RMP(On)
		//GreenNavLight
		//SelLight
		//ComunicationFrequencies
		//ObTineIfrClearance
		//FMGS INITIALIZATION
		//EngineAndAircraftTypeCheck
		//DatabaseValidy
		//NavaidDeselection As required
		//FlightPlanInitialization
		//AlignIrsPrompt
		//FlightPlanCheck
		//Winds
		//FlightPlanDiscontinuities
		//SecondaryPlanApropriate
		//R-NavPagw
		//ZFCG_ZFW
		//BlockFuel
		//V1Set
		//VRSet
		//V2Set
		//FlexToTempSet
		//AltInitial((data->AllData.AIRLINER_CRUISE_ALTITUDE));
		//TrustRedAccAlt(1800, 1800);
		////////////////////////////////////////FlapRem(2);
		//GLARESHIELD
		//FloodLightOver(0.1); // NOT WORK 787 A32NX UPDATE
		BarometricRef();
		FDSet(1, 1);
		FDSet(2, 1);
		//EFUS
		NDMode(1, 3);
		NDRange(1, 0);
		NDMode(2, 3);
		NDRange(2, 0);
		ADFVOR1(1, 0);
		ADFVOR2(1, 0);
		ADFVOR1(2, 0);
		ADFVOR2(2, 0);
		//FCU
		//FCUCheck
		SpeedMachMode(0);
		HdgVsTrkFpa(0);
		Alt = DataT->AllData.AIRLINER_CRUISE_ALTITUDE;
		//LATERAL CONSOLE
		//Oxygwn mask
		CrewSupply(0);
		//IntReceptionKnob
		//EcamDoorPageRegularLo
		//INSTRUMENT PANELS
		//PFDBright;
		//NDBright;
		//LoudSpeaker
		//PFDCheck
		//NDCheck
		//CENTER INSTRUMENT PANELS
		//StbyAsiCheck
		//StbyAltimeterCheck
		//StbyArtificialHorizonCheck
		//ClockCheck
		ASandNS(1);
		//PEDESTAL
		//AcpIntPress
		//AcpVhf
		WxrSys(1);
		WxrWs(0);
		//WxrGain(0);
		WxrMode(3);
		//SwitchingPanelCheck
		//EcamSts
		//EcamPress
		emit SendCommand(SET_THROTTLE, 0, 10);
		EngineMasterSwitches(1, 0);
		EngineMasterSwitches(2, 0);
		EngineModeSelector(NORMAL);
		ParkBrake = 0;
		Sleep(2000);
		ParkBrake = 1;
		//GravityGearExtension
		//AtcSet(1);
		//AltitudeReporting(1);
		WxrSys(0);
		//FMGSAirfieldData
		IrsAligned();
		while (DataT->AllData.A32NX_INITFLIGHT_STATE != 20);
		SendEvent(KEY_TOGGLE_JETWAY, 0);
		//Sleep(30000);
		//FMGSAtcClereance
		//FMGSGrossWeightSet
		//FMGSTakeOffData
		//FMGSFlightPlanACheck
		//FMGSFlightPlanBCheck
		//SquarkCodeSet
		//FuelQuantityCheck
		//TakeOffBriefing
		*Status = 2;
	}
}


void CabinWork::CLBeforeStart(int* Status) {
	*Status = 1;
	Sleep(2000);
	//LoadSheetCheck
	//TakeOffDataCheck
	//SeatBels
	//Seat
	//RudderPedals
	//MCDUTackeOffConfig
	ExternalPower(0);
	//PushBackClearance
	//StartUpClearance
	//NoseWheelStearingCheck
	//WindowsAndDoors(0, 0);
	//WindowsAndDoors(1, 0);
	WindowsAndDoors(2, 0);
	WindowsAndDoors(3, 0);
	WindowsAndDoors(4, 0);
	WindowsAndDoors(5, 0);
	Sleep(60000);
	//WindowsAndDoors(6, 0);
	//WindowsAndDoors(7, 0);
	BeaconLigts(1);
	emit SendCommand(SET_THROTTLE, 0, 10);
	//ParkingBrakeAccuPress
	ParkBrake = 1;
	//PushBack
	*Status = 2;
}


void CabinWork::CLEngineStart(int* Status) {
	if (*Status != 2) {
		*Status = 1;
		Sleep(2000);
		FuelPump(2, 1);
		FuelPump(5, 1);
		FuelPump(1, 1);
		FuelPump(4, 1);
		FuelPump(3, 1);
		FuelPump(6, 1);
		EngineModeSelector(2);
		Sleep(5000);
		//"Starting engine two
		EngineMasterSwitches(2, 1);
		EngineIdleParameters(2);
		//"Starting engine one
		EngineMasterSwitches(1, 1);
		EngineIdleParameters(1);
		*Status = 2;
	}
}


void CabinWork::CLAfterStart(int* Status) {
	if (*Status != 2) {
		*Status = 1;
		Sleep(2000);
		EngineModeSelector(1);
		APUBleedSet(0);
		Spoilers = -1;
		emit SendCommand(SET_RUDDER_TRIM, 0, 10);
		Flaps = 2;
		emit SendCommand(SET_PITCH_TRIM, 2, 10);
		//ECAMStatusPageCheck
		EngineAntiIce(1, 1);
		EngineAntiIce(2, 1);
		WingAntiIce(1);
		APUOff();
		//EcamDoor
		//ClearToDisconnect
		*Status = 2;
	}
}


void CabinWork::CLBeforeTaxi(int flightLevel, int heading, int* Status) {
	if (*Status != 2) {
		*Status = 1;
		Sleep(2000);
		CabinLight(0);
		//TaxiClearance
		LightTaxi = 1;
		LightRunway = 1;
		//FlightControls TO DO Adjust PID
		Sleep(5000);
		SendCommand(SET_AILERON, -16383, 0);
		Sleep(2000);
		SendCommand(SET_AILERON, 0, 0);
		Sleep(2000);
		SendCommand(SET_AILERON, 16383, 0);
		Sleep(2000);
		SendCommand(SET_AILERON, 0, 0);
		Sleep(2000);
		SendCommand(SET_ELEVATOR, -16382, 0);
		Sleep(2000);
		SendCommand(SET_ELEVATOR, 0, 0);
		Sleep(2000);
		SendCommand(SET_ELEVATOR, 16383, 0);
		Sleep(2000);
		SendCommand(SET_ELEVATOR, 0, 0);
		Sleep(2000);
		SendCommand(SET_RUDDER, -16382, 0);
		Sleep(2000);
		SendCommand(SET_RUDDER, 0, 0);
		Sleep(2000);
		SendCommand(SET_RUDDER, 16383, 0);
		Sleep(2000);
		SendCommand(SET_RUDDER, 0, 0);
		Sleep(5000);
		SetAutoBrakes(3);
		//ATC Clearance
		//TO DATA Runway
		//TO DATA FlapsLever
		//TO DATA V1, VR, V2
		//TO DATA FLX
		//FMGS FlightPlan
		//FMGS InitialClimb
		//AltSet(flightLevel);
		Alt = flightLevel;
		//HdgSel(heading);
		Heading = heading;
		//SpeedSel(250);
		FDSet(1, 1);
		FDSet(2, 1);
		//Flight mode annuncator chech
		//Flight instruments check
		WxrSys(0);
		WxrWs(1);
		ParkBrake = 0;
		//Squawk code
		//Takeoff briefing
		*Status = 2;
	}
}


void CabinWork::CLBeforeTakeoff(int* Status) {
	if (*Status != 2) {
		*Status = 1;
		APUSet(0);
		Sleep(2000);
		CabinReport();
		Sleep(2000);
		TOConfig();
		//ECAM TakeOff memo No blue
		//Brake temperature check
		//Takeoff line up clearance
		//Approach path clear of traffic
		EngineModeSelector(2);
		TcasMode(2);
		TcasTraffic(1);
		Pack(0, 1);
		Pack(1, 1);
		//StrobeLight(1);
		LightStrobe = 1;
		LightLanding = 1;
		//Sliding table
		//ATC when cleared for T/O
		*Status = 2;
	}
}


void CabinWork::CLAfterTakeoff(int* Status) {
	if (*Status != 2) {
		*Status = 1;
		Spoilers = 0;
		EngineModeSelector(1);
		TcasTraffic(1);
		*Status = 2;
	}
}


void CabinWork::CLLanding(int* Status) {
	if (*Status != 2) {
		*Status = 1;
		WingAntiIce(0);
		LandingLight(1);
		EngineModeSelector(2);
		CabinReport();
		*Status = 2;
	}
}


void CabinWork::CLAfterLanding(int* Status) {
	if (*Status != 2) {
		*Status = 1;
		LandingLight(0);
		EngineModeSelector(1);
		TcasMode(0);
		APUSet(1);
		WxrMode(0);
		//Predective weather off
		*Status = 2;
	}
}


void CabinWork::CLParking(int* Status) {
	if (*Status != 2) {
		*Status = 1;
		Sleep(5000);
		CabinLight(1);
		EngineAntiIce(1, 0);
		EngineAntiIce(2, 0);
		WingAntiIce(0);
		APUBleedSet(1);
		EngineMasterSwitches(1, 0);
		EngineMasterSwitches(2, 0);
		BeaconLigts(0);
		FuelPump(2, 0);
		FuelPump(5, 0);
		FuelPump(1, 0);
		FuelPump(4, 0);
		FuelPump(3, 0);
		FuelPump(6, 0);
		SeatBeltSign(0);
		NoSmokeSign(2);
		Sleep(10000);
		WindowsAndDoors(5, 1);
		WindowsAndDoors(2, 1);
		Sleep(2000);
		SendEvent(KEY_TOGGLE_JETWAY, 1);
		*Status = 2;
	}
}


void CabinWork::EngineModeSelector(DWORD mode) {
	int i = 1000;
	while ((DataT->GData.TURB_ENG_IGNITION_SWITCH_EX11 != mode) && i) {
			if (mode == CRANK) {
				emit SendEvent(KEY_TURBINE_IGNITION_SWITCH_SET, mode);
			}
			else {
				emit SendEvent(KEY_TURBINE_IGNITION_SWITCH_SET1, mode);
			}
			i--;
	}
	while ((DataT->GData.TURB_ENG_IGNITION_SWITCH_EX12 != mode) && i) {
			if (mode == CRANK) {
				emit SendEvent(KEY_TURBINE_IGNITION_SWITCH_SET, mode);
			}
			else {
				emit SendEvent(KEY_TURBINE_IGNITION_SWITCH_SET2, mode);
			}
			i--;
	}
}


void CabinWork::EngineMasterSwitches(DWORD num, DWORD onoff) {
	int i = 1000;
	if (num == 1) {
		if (DataT->GData.GENERAL_ENG_STARTER1 != onoff) {
			if (onoff == 1) {
				emit SendEvent(KEY_FUELSYSTEM_VALVE_OPEN, num);
			}
			else {
				emit SendEvent(KEY_FUELSYSTEM_VALVE_CLOSE, num);
			}
		}
	}
	else {
		if (DataT->GData.GENERAL_ENG_STARTER2 != onoff) {
			if (onoff == 1) {
				emit SendEvent(KEY_FUELSYSTEM_VALVE_OPEN, num);
			}
			else {
				emit SendEvent(KEY_FUELSYSTEM_VALVE_CLOSE, num);
			}
		}
	}
}


void CabinWork::Wiper(DWORD num, DWORD mode) {
	if (num == 1) {
		if ((DataT->GData.CIRCUIT_SWITCH_ON77 != mode)) {
			emit SendEvent(KEY_ELECTRICAL_CIRCUIT_TOGGLE, 77);
		}
	}
	else {
		if ((DataT->GData.CIRCUIT_SWITCH_ON80 != mode)) {
			emit SendEvent(KEY_ELECTRICAL_CIRCUIT_TOGGLE, 80);
		}
	}
}


void CabinWork::BatterySwitch(DWORD num, DWORD onoff) {		
	SetDataL(A32NX_OVHD_ELEC_BAT_1_PB_IS_AUTO + num - 1, onoff);
}


void CabinWork::ExternalPower(DWORD onoff) {
	if (onoff == 1) {
		if (DataT->GData.EXTERNAL_POWER_AVAILABLE1 == 1) {
			if (DataT->GData.EXTERNAL_POWER_ON1 != onoff) {
				emit SendEvent(KEY_TOGGLE_EXTERNAL_POWER, 1);
			}
		}
	}
	else {
		if (DataT->GData.EXTERNAL_POWER_ON1 != onoff) {
			emit SendEvent(KEY_TOGGLE_EXTERNAL_POWER, 1);
		}
	}
}


void CabinWork::APUSet(DWORD onoff) {
	if (onoff == 1) {
		if ((DataT->AllData.A32NX_OVHD_APU_START_PB_IS_AVAILABLE) != 1) {
			
				if (SetDataL(A32NX_OVHD_APU_MASTER_SW_PB_IS_ON, 1.0) == 1) {
					while ((DataT->AllData.A32NX_APU_FLAP_OPEN_PERCENTAGE) != 100);
						if (SetDataL(A32NX_OVHD_APU_START_PB_IS_ON, 1.0)) {
							while ((DataT->AllData.A32NX_OVHD_APU_START_PB_IS_AVAILABLE) != 1);
						}
				}
			
		}
	}
	else {
		if ((DataT->AllData.A32NX_OVHD_APU_START_PB_IS_AVAILABLE) == 1) {
			SetDataL(A32NX_OVHD_APU_MASTER_SW_PB_IS_ON, 0.0);
		}
		while ((DataT->AllData.A32NX_APU_FLAP_OPEN_PERCENTAGE) > 0.1);
	}
}


void CabinWork::APUOff() {
	SetDataL(A32NX_OVHD_APU_MASTER_SW_PB_IS_ON, 0.0);
}


void CabinWork::FuelPump(DWORD num, DWORD onoff) {
	DWORD var;
	if (num < 7) {
		switch (num) {
		case 1: {
			if (DataT->GData.FUELSYSTEM_PUMP_SWITCH1 != onoff) {
				emit SendEvent(KEY_FUELSYSTEM_PUMP_TOGGLE, num);
			}
			break;
		}
		case 2: {
			if (DataT->GData.FUELSYSTEM_PUMP_SWITCH2 != onoff) {
				emit SendEvent(KEY_FUELSYSTEM_PUMP_TOGGLE, num);
			}
			break;
		}
		case 3: {
			if (DataT->GData.FUELSYSTEM_PUMP_SWITCH3 != onoff) {
				emit SendEvent(KEY_FUELSYSTEM_PUMP_TOGGLE, num);
			}
			break;
		}
		case 4: {
			if (DataT->GData.FUELSYSTEM_PUMP_SWITCH4 != onoff) {
				emit SendEvent(KEY_FUELSYSTEM_PUMP_TOGGLE, num);
			}
			break;
		}
		case 5: {
			if (DataT->GData.FUELSYSTEM_PUMP_SWITCH5 != onoff) {
				emit SendEvent(KEY_FUELSYSTEM_PUMP_TOGGLE, num);
			}
			break;
		}
		case 6: {
			if (DataT->GData.FUELSYSTEM_PUMP_SWITCH6 != onoff) {
				emit SendEvent(KEY_FUELSYSTEM_PUMP_TOGGLE, num);
			}
			break;
		}
		}
		
	}
}


void CabinWork::APUFireTest() {
	if ((DataT->AllData.A32NX_OVHD_APU_START_PB_IS_ON) != 1) {
		SetDataL(A32NX_FIRE_TEST_APU, 1);
		while (!(DataT->AllData.A32NX_MASTER_WARNING)) {
			
		}
		Sleep(2000);
		SetDataL(A32NX_FIRE_TEST_APU, 0);
		while ((DataT->AllData.A32NX_MASTER_WARNING));
		Sleep(1000);
	}
}


void CabinWork::APUBleedSet(DWORD onoff) {
	SetDataL(A32NX_OVHD_PNEU_APU_BLEED_PB_IS_ON, onoff);
}


void CabinWork::CabinLight(DWORD mode) {
	if (DataT->GData.EXTERNAL_POWER_ON1 || (DataT->AllData.A32NX_OVHD_APU_START_PB_IS_ON) || mode == 0)
	{
		switch (mode) {
		case 0: {
			if (DataT->GData.LIGHT_POTENTIOMETER7 != 0) {
				emit SendEvent(KEY_LIGHT_POTENTIOMETER_7_SET, 0);
			}
			if (DataT->GData.LIGHT_CABIN != 0) {
				emit SendEvent(KEY_TOGGLE_CABIN_LIGHTS, 0);
			}
			break;
		}
		case 1: {
			if (DataT->GData.LIGHT_POTENTIOMETER7 != 0.5) {
				emit SendEvent(KEY_LIGHT_POTENTIOMETER_7_SET, 50);
			}
			if (DataT->GData.LIGHT_CABIN != 1) {
				emit SendEvent(KEY_TOGGLE_CABIN_LIGHTS, 1);
			}
			break;
		}
		case 2: {
			if (DataT->GData.LIGHT_POTENTIOMETER7 != 1) {
				emit SendEvent(KEY_LIGHT_POTENTIOMETER_7_SET, 100);
			}
			if (DataT->GData.LIGHT_CABIN != 1) {
				emit SendEvent(KEY_TOGGLE_CABIN_LIGHTS, 1);
			}
			break;
		}
		default: {
			if (DataT->GData.LIGHT_POTENTIOMETER7 != 1) {
				emit SendEvent(KEY_LIGHT_POTENTIOMETER_7_SET, 100);
			}
			if (DataT->GData.LIGHT_CABIN != 1) {
				emit SendEvent(KEY_TOGGLE_CABIN_LIGHTS, 1);
			}
			break;
		}
		}
	}
}


void CabinWork::ParkingBrakeSet(DWORD onoff) {
	if (DataT->AllData.A32NX_PARK_BRAKE_LEVER_POS != onoff) {
		SetDataL(A32NX_PARK_BRAKE_LEVER_POS, onoff);
	}
}


void CabinWork::FlapsSet(DWORD pos) {
	
	for (int i = DataT->GData.FLAPS_HANDLE_INDEX; i != pos; ) {
		Sleep(60);
		i = (DataT->AllData.A32NX_FLAPS_HANDLE_INDEX);
		if (i < pos) {
			emit SendEvent(KEY_FLAPS_UP + i + 1, 0);
		}
		else if (i>pos) {
			emit SendEvent(KEY_FLAPS_UP + i - 1, 0);
		}
	}
}


long CabinWork::IncDec16384_100(double curr, bool dir) {
	
	if (dir) {
		if (curr == 0) {
			return 0;
		}
		return (curr - 0.01) * 16383;
	}
	else {
		if (curr == 1) {
			return 16383;
		}
		return (curr + 0.01) * 16383;
	}
}


void CabinWork::SpoilersSet(double pos) {
	if (Spoilers < 0) {
		emit SendEvent(KEY_SPOILERS_SET, 0);
		emit SendEvent(KEY_SPOILERS_ARM_ON, 0);
	}
	else {
		emit SendEvent(KEY_SPOILERS_ARM_OFF, 0);
		emit SendEvent(KEY_SPOILERS_SET, 0);
	}
}


void CabinWork::ProbeWindowHeat(DWORD onoff) {
	if ((DataT->AllData.A32NX_MAN_PITOT_HEAT) != onoff) {
		SetDataL(A32NX_MAN_PITOT_HEAT, onoff);
	}
}


void CabinWork::XBleedSet(DWORD mode) {
	if ((DataT->AllData.A32NX_KNOB_OVHD_AIRCOND_XBLEED_Position) != mode) {
		SetDataL(A32NX_KNOB_OVHD_AIRCOND_XBLEED_Position, mode);
	}
}


void CabinWork::GearSet(DWORD pos) {
	if (DataT->GData.GEAR_HANDLE_POSITION != pos) {
		SendEvent(KEY_GEAR_SET, pos);
	}
}


void CabinWork::EvacCapt(DWORD mode) {
	if ((DataT->AllData.A32NX_EVAC_CAPT_TOGGLE) != mode) {
		SetDataL(A32NX_EVAC_CAPT_TOGGLE, mode);
	}
}


void CabinWork::ADIRS(DWORD num, DWORD mode) {
	double var=0;
	switch (num) {
	case 0: {
		var = DataT->AllData.A32NX_OVHD_ADIRS_IR_1_MODE_SELECTOR_KNOB;
		break;
	}
	case 1: {
		var = DataT->AllData.A32NX_OVHD_ADIRS_IR_2_MODE_SELECTOR_KNOB;
		break;
	}
	case 2: {
		var = DataT->AllData.A32NX_OVHD_ADIRS_IR_3_MODE_SELECTOR_KNOB;
		break;
	}
	}
	if (var != mode) {
		SetDataL(A32NX_OVHD_ADIRS_IR_1_MODE_SELECTOR_KNOB + num, mode);
		while ((DataT->AllData.A32NX_OVHD_ADIRS_ON_BAT_IS_ILLUMINATED) != 1);
		while ((DataT->AllData.A32NX_OVHD_ADIRS_ON_BAT_IS_ILLUMINATED) != 0);
	}
}


void CabinWork::NavLight(DWORD onoff) {
	if (DataT->GData.EXTERNAL_POWER_ON1 || (DataT->AllData.A32NX_OVHD_APU_START_PB_IS_ON) || onoff != 1)
	{
		if (DataT->GData.LIGHT_NAV != onoff) {
			SetData(LIGHT_NAV, onoff);
		}
		if (DataT->GData.LIGHT_LOGO != onoff) {
			SetData(LIGHT_NAV, onoff);
		}
	}
}


void CabinWork::NoSmokeSign(DWORD mode) {
	if ((DataT->AllData.XMLVAR_SWITCH_OVHD_INTLT_NOSMOKING_Position) != mode) {
		SetDataL(XMLVAR_SWITCH_OVHD_INTLT_NOSMOKING_Position, mode);
	}
}


void CabinWork::EmerExitSign(DWORD mode) {
	if ((DataT->AllData.XMLVAR_SWITCH_OVHD_INTLT_EMEREXIT_Position) != mode) {
		SetDataL(XMLVAR_SWITCH_OVHD_INTLT_EMEREXIT_Position, mode);
	}
}


void CabinWork::SeatBeltSign(DWORD mode) {
	if (DataT->GData.CABIN_SEATBELTS_ALERT_SWITCH != mode) {
		SendEvent(KEY_CABIN_SEATBELTS_ALERT_SWITCH_TOGGLE, 0);
	}
}


void CabinWork::LandingElevation(double pos) {
	pos = pos * 100;
	if ((DataT->AllData.XMLVAR_KNOB_OVHD_CABINPRESS_LDGELEV) != pos) {
		SetDataL(XMLVAR_KNOB_OVHD_CABINPRESS_LDGELEV, pos);
	}
}


void CabinWork::PackFlow(DWORD mode) {
	if ((DataT->AllData.A32NX_KNOB_OVHD_AIRCOND_PACKFLOW_Position) != mode) {
		SetDataL(A32NX_KNOB_OVHD_AIRCOND_PACKFLOW_Position, mode);
	}
}


void CabinWork::EngFireTest(DWORD num) {
	SetDataL(A32NX_FIRE_TEST_ENG1 + num - 1, 1);
	while (!(DataT->AllData.A32NX_MASTER_WARNING));
	Sleep(2000);
	SetDataL(A32NX_FIRE_TEST_ENG1 + num - 1, 0);
	while ((DataT->AllData.A32NX_MASTER_WARNING));
	Sleep(1000);
}


void CabinWork::FlapRem(DWORD pos) {
	if ((DataT->AllData.A32NX_TO_CONFIG_FLAPS) != pos) {
		SetDataL(A32NX_TO_CONFIG_FLAPS, pos);
	}
}


void CabinWork::FloodLightOver(double flood) {
	if (flood == 0.0) {
		if (DataT->GData.LIGHT_PANEL4 != 0) {
			SetData(LIGHT_PANEL4, 0);
		}
		if (DataT->GData.LIGHT_POTENTIOMETER86 != flood) {
			emit SendEvent(KEY_LIGHT_POTENTIOMETER_SET, 86);
			emit SendEvent(KEY_LIGHT_POTENTIOMETER_SET, 0);
			SetData(LIGHT_POTENTIOMETER86, flood);
		}
	}
	else {
		if (DataT->GData.LIGHT_PANEL4 != 1) {
			SetData(LIGHT_PANEL4, 1);
		}
		if (DataT->GData.LIGHT_POTENTIOMETER86 != flood) {
			emit SendEvent(KEY_LIGHT_POTENTIOMETER_SET, 86);
			emit SendEvent(KEY_LIGHT_POTENTIOMETER_SET, 10);
			SetData(LIGHT_POTENTIOMETER86, flood);
		}
	}
		
}


void CabinWork::BaroMode(DWORD mode) {
	if ((DataT->AllData.XMLVAR_Baro1_Mode) != mode) {
		SetDataL(XMLVAR_Baro1_Mode, mode);
	}
}


void CabinWork::BarometricRef() {
	long currpos;
	double pog = 0.01;
	double ambBaro = DataT->GData.SEA_LEVEL_PRESSURE;
		double ppos = DataT->GData.KOHLSMAN_SETTING_HG;
		while (((ppos + pog) < ambBaro) || ((ppos - pog) > ambBaro)) {
			if ((DataT->AllData.XMLVAR_Baro1_Mode) != 1) {
				SetDataL(XMLVAR_Baro1_Mode, 1);
			}
			if ((DataT->AllData.XMLVAR_Baro_Selector_HPA_1) != 0) {
				SetDataL(XMLVAR_Baro_Selector_HPA_1, 0);
			}
			if (ppos < ambBaro) {
				emit SendEvent(KEY_KOHLSMAN_INC, 0);
			}
			else {
				emit SendEvent(KEY_KOHLSMAN_DEC, 0);
			}
			Sleep(20);
			ppos = DataT->GData.KOHLSMAN_SETTING_HG;
		}
}


void CabinWork::FDSet(DWORD num, DWORD onoff) {
	if (DataT->GData.AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1 - 1 + num != onoff) {
		emit SendEvent(KEY_TOGGLE_FLIGHT_DIRECTOR, 0);
	}
}


void CabinWork::NDMode(DWORD num, DWORD mode) {
	double var;
	switch (num) {
	case 1: {
		var = DataT->AllData.A320_Neo_MFD_NAV_MODE_1;
		break;
	}
	case 2: {
		var = DataT->AllData.A320_Neo_MFD_NAV_MODE_2;
		break;
	}
	}
	if (var != mode) {
		SetDataL(A320_Neo_MFD_NAV_MODE_1, mode);
	}
}


void CabinWork::NDRange(DWORD num, DWORD range) {
	double var;
	switch (num) {
	case 1: {
		var = DataT->AllData.A320_Neo_MFD_Range_1;
		break;
	}
	case 2: {
		var = DataT->AllData.A320_Neo_MFD_Range_2;
		break;
	}
	}
	if (var != range) {
		SetDataL(A320_Neo_MFD_Range_1, range);
	}
}


void CabinWork::ADFVOR1(DWORD num, DWORD mode) {
	double var;
	switch (num) {
	case 1: {
		var = DataT->AllData.XMLVAR_NAV_AID_SWITCH_L1_State;
		break;
	}
	case 2: {
		var = DataT->AllData.XMLVAR_NAV_AID_SWITCH_L1_State;
		break;
	}
	}
	if (var != mode) {
		SetDataL(XMLVAR_NAV_AID_SWITCH_L1_State, mode);
	}
}


void CabinWork::ADFVOR2(DWORD num, DWORD mode) {
	double var;
	switch (num) {
	case 1: {
		var = DataT->AllData.XMLVAR_NAV_AID_SWITCH_L2_State;
		break;
	}
	case 2: {
		var = DataT->AllData.XMLVAR_NAV_AID_SWITCH_L2_State;
		break;
	}
	}
	if (var!= mode) {
		SetDataL(XMLVAR_NAV_AID_SWITCH_L2_State, mode);
	}
}


void CabinWork::SpeedMachMode(DWORD mode) {
	if (mode) {
		emit SendEvent(KEY_AP_MANAGED_SPEED_IN_MACH_ON, 0);
	} 
	else {
		emit SendEvent(KEY_AP_MANAGED_SPEED_IN_MACH_OFF, 0);
	}
}


void CabinWork::HdgVsTrkFpa(DWORD mode) {
	if ((DataT->AllData.A32NX_TRK_FPA_MODE_ACTIVE) != mode) {
		SetDataL(A32NX_TRK_FPA_MODE_ACTIVE, mode);
	}
}


void CabinWork::AltSet(DWORD alt) {
	if (DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 != alt) {
		alt = int(alt / 100) * 100;
		DWORD currAlt;
		SetAltitudeIncrement(1);
		if ((int(DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 / 1000) * 1000) != (int(alt / 1000) * 1000)) {
			currAlt = int(DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 / 1000) * 1000;
			if (alt > (int(DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 / 1000) * 1000)) {
				currAlt = currAlt + 1000;
			}
			else {
				currAlt = currAlt - 1000;
			}
			if (currAlt < 100) {
				currAlt = 100;
			}
			emit SendEvent(A32NX_FCU_ALT_SET, currAlt);
			Sleep(100);
		}
		if (alt != (int(DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 / 100) * 100)) {
			SetAltitudeIncrement(0);
			if ((int(DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 / 100) * 100) != (int(alt / 100) * 100)) {
				currAlt = int(DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 / 100) * 100;
				if (alt > (int(DataT->GData.AUTOPILOT_ALTITUDE_LOCK_VAR3 / 100) * 100)) {
					currAlt = currAlt + 100;
				}
				else {
					currAlt = currAlt - 100;
				}
				if (currAlt < 100) {
					currAlt = 100;
				}
				emit SendEvent(A32NX_FCU_ALT_SET, currAlt);
				Sleep(100);
			}
		}
	}
}


void CabinWork::SetAltitudeIncrement(DWORD alt) {
	if (alt == 0) {
		alt = 100;
	}
	else {
		alt = 1000;
	}
	if ((DataT->AllData.XMLVAR_Autopilot_Altitude_Increment) != alt) {
		SetDataL(XMLVAR_Autopilot_Altitude_Increment, alt);
	}
}


void CabinWork::AltInitial(DWORD alt) {
	if ((DataT->AllData.AIRLINER_CRUISE_ALTITUDE) != alt) {
		SetDataL(AIRLINER_CRUISE_ALTITUDE, alt);
	}
}


void CabinWork::CrewSupply(DWORD offon) {
	if ((DataT->AllData.PUSH_OVHD_OXYGEN_CREW) != offon) {
		SetDataL(PUSH_OVHD_OXYGEN_CREW, offon);
	}
}


void CabinWork::ASandNS(DWORD onoff) {
	if (DataT->GData.ANTISKID_BRAKES_ACTIVE != onoff) {
		emit SendEvent(KEY_ANTISKID_BRAKES_TOGGLE, 0);
	}
}


void CabinWork::WxrSys(DWORD mode) {
	if ((DataT->AllData.XMLVAR_A320_WeatherRadar_Sys) != mode) {
		SetDataL(XMLVAR_A320_WeatherRadar_Sys, mode);
	}
}


void CabinWork::WxrWs(DWORD onoff) {
	if ((DataT->AllData.A32NX_SWITCH_RADAR_PWS_Position) != onoff) {
		SetDataL(A32NX_SWITCH_RADAR_PWS_Position, onoff);
	}
}


void CabinWork::WxrMode(DWORD mode) {
	if ((DataT->AllData.XMLVAR_A320_WEATHERRADAR_MODE) != mode) {
		SetDataL(XMLVAR_A320_WeatherRadar_Mode, mode);
	}
}


void CabinWork::IrsAligned() {
	ADIRS(0, 1);
	ADIRS(1, 1);
	ADIRS(2, 1);
	while ((DataT->AllData.A32NX_ADIRS_ADIRU_1_STATE) != 2);
	while ((DataT->AllData.A32NX_ADIRS_ADIRU_2_STATE) != 2);
	while ((DataT->AllData.A32NX_ADIRS_ADIRU_3_STATE) != 2);
}


void CabinWork::WindowsAndDoors(DWORD num, DWORD openclose) {
	switch (num) {
	case 0: {
		if (DataT->GData.NEW_EXIT_OPEN0 != openclose) {
			emit SendEvent(KEY_TOGGLE_AIRCRAFT_EXIT, num + 1);
			while (DataT->GData.NEW_EXIT_OPEN0 != openclose);
		}
		break;
	}
	case 1: {
		if (DataT->GData.NEW_EXIT_OPEN1 != openclose) {
			emit SendEvent(KEY_TOGGLE_AIRCRAFT_EXIT, num + 1);
			while (DataT->GData.NEW_EXIT_OPEN1 != openclose);
		}
		break;
	}
	case 2: {
		if (DataT->GData.NEW_EXIT_OPEN2 != openclose) {
			emit SendEvent(KEY_TOGGLE_AIRCRAFT_EXIT, num + 1);
			while (DataT->GData.NEW_EXIT_OPEN2 != openclose);
		}
		break;
	}
	case 3: {
		if (DataT->GData.NEW_EXIT_OPEN3 != openclose) {
			emit SendEvent(KEY_TOGGLE_AIRCRAFT_EXIT, num + 1);
			while (DataT->GData.NEW_EXIT_OPEN3 != openclose);
		}
		break;
	}
	case 4: {
		if (DataT->GData.NEW_EXIT_OPEN4 != openclose) {
			emit SendEvent(KEY_TOGGLE_AIRCRAFT_EXIT, num + 1);
			while (DataT->GData.NEW_EXIT_OPEN4 != openclose);
		}
		break;
	}
	case 5: {
		if (DataT->GData.NEW_EXIT_OPEN5 != openclose) {
			emit SendEvent(KEY_TOGGLE_AIRCRAFT_EXIT, num + 1);
			while (DataT->GData.NEW_EXIT_OPEN5 != openclose);
		}
		break;
	}
		 
	}
}


void CabinWork::BeaconLigts(DWORD onoff) {
		if (DataT->GData.LIGHT_BEACON != onoff) {
			//emit SendEvent(KEY_TOGGLE_BEACON_LIGHTS, 0);
			SetData(LIGHT_BEACON, onoff);
		}
}


void CabinWork::EngineIdleParameters(DWORD num) {
	switch(num) {
	case 1: {
		while (DataT->GData.ENG_N1_RPM1 < 0.150); //0.195
		while ((DataT->AllData.A32NX_ENGINE_STATE_1) != 1);
		Sleep(15000);
		break;
	}
	case 2: {
		while (DataT->GData.ENG_N1_RPM2 < 0.150); //0.195
		while ((DataT->AllData.A32NX_ENGINE_STATE_2) != 1);
		Sleep(15000);
		break;
	}
	}
}


void CabinWork::EngineAntiIce(DWORD num, DWORD onoff) {
	double var;
	switch (num) {
	case 1: {
		var = DataT->AllData.XMLVAR_Momentary_PUSH_OVHD_ANTIICE_ENG1_Pressed;
		break;
	}
	case 2: {
		var = DataT->AllData.XMLVAR_Momentary_PUSH_OVHD_ANTIICE_ENG1_Pressed;
		break;
	}
	}
	if (var != onoff) {
		SetDataL(XMLVAR_Momentary_PUSH_OVHD_ANTIICE_ENG1_Pressed - 1 + num, onoff);
	}
}


void CabinWork::WingAntiIce(DWORD onoff) {
	if ((DataT->AllData.XMLVAR_Momentary_PUSH_OVHD_ANTIICE_WING_Pressed) != onoff) {
		SetDataL(XMLVAR_Momentary_PUSH_OVHD_ANTIICE_WING_Pressed, onoff);
	}
}


void CabinWork::NoseLight(DWORD pos) {
	switch (pos) {
	case 0: {
		if (DataT->GData.LIGHT_TAXI != 0) {
			SetData(CIRCUIT_SWITCH_ON17, 0);
			SetData(CIRCUIT_SWITCH_ON20, 0);
		}
		if (DataT->GData.LIGHT_LANDING != 0) {
			SetData(CIRCUIT_SWITCH_ON17, 0);
			SetData(CIRCUIT_SWITCH_ON20, 0);
		}
		break;
	}
	case 1: {
		if (DataT->GData.LIGHT_TAXI1 != 1) {
			SetData(CIRCUIT_SWITCH_ON17, 0);
			SetData(CIRCUIT_SWITCH_ON20, 1);
		}
		if (DataT->GData.LIGHT_LANDING1 != 0) {
			SetData(CIRCUIT_SWITCH_ON17, 0);
			SetData(CIRCUIT_SWITCH_ON20, 1);
		}
		break;
	}
	case 2: {
		if (DataT->GData.LIGHT_TAXI1 != 0) {
			SetData(CIRCUIT_SWITCH_ON17, 1);
			SetData(CIRCUIT_SWITCH_ON20, 0);
		}
		if (DataT->GData.LIGHT_LANDING1 != 1) {
			SetData(CIRCUIT_SWITCH_ON17, 1);
			SetData(CIRCUIT_SWITCH_ON20, 0);
		}
		break;
	}
	default: {
		if (DataT->GData.LIGHT_TAXI1 != 0) {
			SetData(CIRCUIT_SWITCH_ON17, 0);
			SetData(CIRCUIT_SWITCH_ON20, 1);
		}
		if (DataT->GData.LIGHT_LANDING1 != 1) {
			SetData(CIRCUIT_SWITCH_ON17, 0);
			SetData(CIRCUIT_SWITCH_ON20, 1);
		}
		break;
	}
	}
}


void CabinWork::RunwayLight(DWORD onoff) {
	if (DataT->GData.LIGHT_TAXI2 != onoff) {
		SetData(CIRCUIT_SWITCH_ON21, onoff);
		SetData(CIRCUIT_SWITCH_ON22, onoff);
	}
}


void CabinWork::LandingLight(DWORD onoff) {
	if (onoff) {
		if ((DataT->GData.LIGHT_LANDING2 == 0) ||
			(DataT->GData.LIGHT_LANDING3 == 0)) {
			SetDataL(LIGHTING_LANDING_2, 0);
			SetDataL(LIGHTING_LANDING_3, 0);
			SetDataL(LANDING_2_RETRACTED, 0);
			SetDataL(LANDING_3_RETRACTED, 0);
			Sleep(10000);
			SetData(LIGHT_LANDING2, 1);
			SetData(LIGHT_LANDING3, 1);
		}
	}
	else {
		if ((DataT->GData.LIGHT_LANDING3 != 0) ||
			(DataT->GData.LIGHT_LANDING2 != 0)) {
			SetData(LIGHT_LANDING2, 0);
			SetData(LIGHT_LANDING3, 0);
			SetDataL(LIGHTING_LANDING_2, 2);
			SetDataL(LIGHTING_LANDING_3, 2);
			SetDataL(LANDING_2_RETRACTED, 1);
			SetDataL(LANDING_3_RETRACTED, 1);
		}
	}
}


void CabinWork::StrobeLight(DWORD onoff) {
	if (DataT->GData.LIGHT_STROBE != onoff) {
		emit SendEvent(KEY_STROBES_SET, onoff);
	}
}


void CabinWork::SetAutoBrakes(DWORD num) {
	if ((DataT->AllData.A32NX_AUTOBRAKES_ARMED_MODE) != num) {
		SetDataL(A32NX_AUTOBRAKES_ARMED_MODE, num);
	}
}


void CabinWork::HdgSel(double heading) {
	while (round(DataT->AllData.A32NX_AUTOPILOT_HEADING_SELECTED) != Utils::Constrain360(heading)) {
		double prev = DataT->AllData.A32NX_AUTOPILOT_HEADING_SELECTED;
		if (round(Utils::Constrain180(DataT->AllData.A32NX_AUTOPILOT_HEADING_SELECTED - Utils::Constrain360(heading))) < 0) {
			SendEvent(A32NX_FCU_HDG_INC, 1);
		}
		else {
			SendEvent(A32NX_FCU_HDG_DEC, 1);
		}
		while (DataT->AllData.A32NX_AUTOPILOT_HEADING_SELECTED == prev);
	}
}


void CabinWork::SpeedSel(DWORD speed) {
	while (DataT->GData.AUTOPILOT_AIRSPEED_HOLD_VAR != speed) {
		if (DataT->GData.AUTOPILOT_AIRSPEED_HOLD_VAR < speed) {
			SendEvent(A32NX_FCU_SPD_SET, DataT->GData.AUTOPILOT_AIRSPEED_HOLD_VAR + 1);
		}
		else {
			SendEvent(A32NX_FCU_SPD_SET, DataT->GData.AUTOPILOT_AIRSPEED_HOLD_VAR - 1);
		}
		Sleep(60);
	}
}


void CabinWork::VSSel(double VS) {
	if (VS < -4000) {
		VS = -4000;
		if (VS > 4000) {
			VS = 4000;
		}
	}
	while (DataT->AllData.A32NX_AUTOPILOT_VS_SELECTED != VS) {
		if (DataT->AllData.A32NX_AUTOPILOT_VS_SELECTED < VS) {
			SendEvent(A32NX_FCU_VS_SET, DataT->AllData.A32NX_AUTOPILOT_VS_SELECTED + 100);
		}
		else {
			SendEvent(A32NX_FCU_VS_SET, DataT->AllData.A32NX_AUTOPILOT_VS_SELECTED - 100);
		}
		Sleep(60);
	}
	SendEvent(A32NX_FCU_VS_PULL, 1);
}


void CabinWork::CabinReport() {
	if ((DataT->AllData.A32NX_CABIN_READY) != 1) {
		SetDataL(PUSH_OVHD_CALLS_ALL, 1);
		Sleep(1000);
		SetDataL(PUSH_OVHD_CALLS_ALL, 0);
		Sleep(2000);
	}
}


void CabinWork::TOConfig() {
	if ((DataT->AllData.A32NX_FWC_TOCONFIG) != 1) {
		SetDataL(A32NX_BTN_TOCONFIG, 1);
		Sleep(1000);
		SetDataL(A32NX_BTN_TOCONFIG, 0);
		Sleep(2000);
	}
}


void CabinWork::TcasMode(DWORD mode) {
	if ((DataT->AllData.A32NX_SWITCH_TCAS_Position) != mode) {
		SetDataL(A32NX_SWITCH_TCAS_Position, mode);
	}
}


void CabinWork::TcasTraffic(DWORD mode) {
	if ((DataT->AllData.A32NX_SWITCH_TCAS_TRAFFIC_POSITION) != mode) {
		SetDataL(A32NX_SWITCH_TCAS_Traffic_Position, mode);
	}
}


void CabinWork::Pack(DWORD num, DWORD onoff) {
	double var;
	switch (num) {
	case 1: {
		var = DataT->AllData.A32NX_AIRCOND_PACK1_TOGGLE;
		break;
	}
	case 2: {
		var = DataT->AllData.A32NX_AIRCOND_PACK1_TOGGLE;
		break;
	}
	}
	if (var!= onoff) {
		SetDataL(A32NX_AIRCOND_PACK1_TOGGLE - 1 + num, onoff);
	}
}


double CabinWork::SetData(DWORD var, double val, char* unit) {
	emit SetDataSignal(CABINWORK_ID, var, &val, unit);
	while (!SetDataChanged);
	SetDataChanged = false;
	return  val;
}


double CabinWork::SetDataL(DWORD var, double val, char* unit) {
	emit SetDataSignalL(CABINWORK_ID, var, &val, unit);
	while (!SetDataChanged);
	SetDataChanged = false;
	return  val;
}


DWORD CabinWork::SendEvent(DWORD EventID, long dwData)
{
	SendEventSignal(CABINWORK_ID, EventID, dwData);
	return 0;
}


DWORD CabinWork::SendEvent2(DWORD EventID, long dwData, DWORD var, double val, char* unit)
{
	SendEventSignal2(CABINWORK_ID, EventID, dwData, var, val, unit);
	return 0;
}


void CabinWork::SetDataChange(DWORD var, DWORD sender) {
	if (sender == CABINWORK_ID) {
		SetDataChanged = true;
	}
}


void CabinWork::SetATHR(DWORD onoff)
{
	if (onoff) {
		if (DataT->AllData.A32NX_AUTOTHRUST_MODE == 0) {
			SendEvent(A32NX_FCU_ATHR_PUSH, 1);
		}
	}
	else {
		if (DataT->AllData.A32NX_AUTOTHRUST_MODE > 0) {
			SendEvent(A32NX_FCU_ATHR_PUSH, 1);
		}
	}
}


void CabinWork::ReceiveCommand(DWORD command, double parameter1, double parameter2) {
	switch (command) {
	case LIGHTSTROBE_SET: {
		LightStrobe = parameter1;
		break;
	}
	case LIGHTLANDING_SET: {
		LightLanding = parameter1;
		break;
	}
	case LIGHTRUNWAY_SET: {
		LightRunway = parameter1;
		break;
	}
	case LIGHTTAXI_SET: {
		LightTaxi = parameter1;
		break;
	}
	case HDG_SEL: {
		HeadingMode = 0;
		Heading = parameter1;
		break;
	}
	case SPD_SEL: {
		Speed = parameter1;
		break;
	}
	case SET_ATHR: {
		SetATHR(parameter1);
		break;
	}
	case PARKBRAKE_SET: {
		ParkBrake = parameter1;
		break;
	}
	case GEAR_SET: {
		Gears = parameter1;
		break;
	}
	case FLAPS_SET: {
		Flaps = parameter1;
		break;
	}
	case BARO_MODE: {
		BaroM = parameter1;
		break;
	}
	case SAVE_DATA: {
		SetDataL(SAVE_VARS, 1);
		break;
	}
	case PUSH_ALT: {
		if (AltMode != 7) {
			AltMode = 5;
		}
		break;
	}
	case PUSH_ALT_TEST: {
		AltMode = 7;
		break;
	}
	case PULL_ALT: {
		if (AltMode != 7) {
			AltMode = 0;
		}
		break;
	}
	case ALT_SEL: {
		Alt = parameter1;
		break;
	}
	case PUSH_VS: {
		if (AltMode != 7) {
			AltMode = 2;
		}
		break;
	}
	case VS_SEL: {
		if (AltMode != 7) {
			AltMode = 3;
			VS = parameter1;
		}
		break;
	}
	case PUSH_HDG: {
		HeadingMode = 1;
		break;
	}
	case PULL_HDG: {
		HeadingMode = 0;
		break;
	}
	case PUSH_SPD: {
		SpeedMode = 1;
		break;
	}
	case PULL_SPD: {
		SpeedMode = 0;
		break;
	}
	case SPOILER_SET: {
		Spoilers = parameter1;
		break;
	}
	case AUTOBRAKES_SET: {
		SetAutoBrakes(parameter1);
		break;
	}
	}
}
	

void CabinWork::TimerProc() {

	StrobeLight(LightStrobe);
	LandingLight(LightLanding);
	RunwayLight(LightRunway);
	NoseLight(LightTaxi);
	if (LightTaxi > 0) {
		RunwayLight(1);
	}
	else {
		RunwayLight(0);
	}
	ParkingBrakeSet(ParkBrake);
	GearSet(Gears);
	FlapsSet(Flaps);
	BaroMode(BaroM);
	if (BaroM == 1) {
		BarometricRef();
	}
	SpoilersSet(Spoilers);
	//if ((DataT == NULL) || (!DataT->AllData.A32NX_AUTOPILOT_ACTIVE)) {
		if (SpeedMode == 1) {
			SendEvent(A32NX_FCU_SPD_PUSH, 1);
		}
		else if (SpeedMode == 0) {
			//SendEvent(A32NX_FCU_SPD_PULL, 1);
			SpeedMode = -1;
			//SpeedSel(Speed);
		}
		if (HeadingMode == 1) {
			SendEvent(A32NX_FCU_HDG_PUSH, 1);
			HeadingMode = -1;
		}
		else if (HeadingMode == 0) {
			SendEvent(A32NX_FCU_HDG_PULL, 1);
			SendEvent(A32NX_FCU_HDG_SET, Heading);
			HeadingMode = -1;
			//HdgSel(Heading);
		}
		if (Alt < 100) {
			AltSet(100);
		}
		else {
			AltSet(Alt);
		}
		if (AltMode == 0) {
			SendEvent(A32NX_FCU_ALT_PULL, 1);
			AltMode = -1;
		}
		else if (AltMode == 2) {
			SendEvent(A32NX_FCU_VS_PUSH, 1);
			AltMode = -1;
		}
		else if (AltMode == 3) {
			VSSel(VS);
		}
		else if (AltMode == 5) {
			SendEvent(A32NX_FCU_ALT_PUSH, 1);
			AltMode = -1;
		}
	//}
	if (AltMode == 7) {
		SendEvent(A32NX_FCU_ALT_PUSH, 1);
		AltMode = -1;
	}
}