#include "SimData.h"
//#include "main.cpp"

void SimData::TimerProc() {
	
	
	if (RegVarsGet->size() > 0) {
		GetAllData();
		//SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, 15, 15, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
	}
	
	//SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
}

HRESULT SimData::GetAllData() {
	Context c = { this, SIMDATA_ID };
	HRESULT result = SimConnect_RequestDataOnSimObject(HSimConnect, REQ_DATA, DEF_DATA, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
	result = SimConnect_RequestClientData(HSimConnect, A32NX_ALLDATA_ID, REQ_ALL_DATA, A32NX_ALLDATA_DEFINITION,
		SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0);
	DWORD cbData;
	SIMCONNECT_RECV* pData;
	while (SUCCEEDED(SimConnect_GetNextDispatch(HSimConnect, &pData, &cbData))) {
		FDispatchProc(pData, cbData, (void*)(&c));
	}
	return result;
}

void CALLBACK SimData::FDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* context) {
	Context* lContext = (Context*)context;
	if (lContext->data->AirportData != NULL) {
		lContext->data->AirportData->DispatchProc(pData, cbData, lContext);
	}
	switch (pData->dwID) {
	case SIMCONNECT_RECV_ID_CLIENT_DATA: { // Receive and process the NGX data block 
		SIMCONNECT_RECV_CLIENT_DATA* pObjData = (SIMCONNECT_RECV_CLIENT_DATA*)pData;
		switch (pObjData->dwRequestID) {
		case REQ_CONTROL: {
			A32NX_Control* pS = (A32NX_Control*)&pObjData->dwData;
			lContext->data->Control = *pS;
			break;
		}
		case REQ_LOCAL_DATA: {
			sExportData* pS = (sExportData*)&pObjData->dwData;
			lContext->data->ExportData = *pS;
			break;
		}
		case REQ_ALL_DATA: {
			sAllData* pS = (sAllData*)&pObjData->dwData;
			if (pS->version > lContext->data->VerAll) {
				lContext->data->AllData = *pS;
				lContext->data->VerAll = pS->version;
			}
			break;
		}
		default: {
			//AddText("NON Data request and not control request");
			break;
		}
		}
		break;
	}
	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
		SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
		sGData* pS = (sGData*)&pObjData->dwData;
		switch (pObjData->dwRequestID) {
		case REQ_DATA: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsGetCount(); i++) {
				*SimVarsGet[RegVarsGet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->GData.version++;
			lContext->data->GData = *pS;
			break;
		}
		case REQ_DATA_GET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsGetCount(); i++) {
				*SimVarsGet[RegVarsGet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set50 = true;
			break;
		}
		case REQ_DATA_SET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsSetCount(); i++) {
				*SimVarsSet[RegVarsSet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set51 = true;
			break;
		}
		case REQ_MAINLOGIC_GET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsGetCount(); i++) {
				*SimVarsGet[RegVarsGet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set50 = true;
			break;
		}
		case REQ_MAINLOGIC_SET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsSetCount(); i++) {
				*SimVarsSet[RegVarsSet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set51 = true;
			break;
		}
		case REQ_CABINWORK_GET: {
			int i = 0;
			for (i = 0; i < lContext->data->GetRegisteredVarsGetCount(); i++) {
				if (i < pObjData->dwDefineCount) {
					*SimVarsGet[RegVarsGet->at(i)] = *((double*)&pObjData->dwData + i);
				}
				else {
					break;
				}
			}
			if (i == lContext->data->GetRegisteredVarsGetCount()) {
				lContext->data->set50 = true;
			}
			break;
		}
		case REQ_CABINWORK_SET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsSetCount(); i++) {
				*SimVarsSet[RegVarsSet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set51 = true;
			break;
		}
		case REQ_CABINWORK_SET_SET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsSetCount(); i++) {
				*SimVarsSet[RegVarsSet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set53 = true;
			break;
		}
		case REQ_PLANESWORK_GET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsGetCount(); i++) {
				*SimVarsGet[RegVarsGet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set50 = true;
			break;
		}
		case REQ_PLANESWORK_SET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsSetCount(); i++) {
				*SimVarsSet[RegVarsSet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set51 = true;
			break;
		}
		case REQ_PLANESWORK_SET_SET: {
			for (int i = 0; i < lContext->data->GetRegisteredVarsSetCount(); i++) {
				*SimVarsSet[RegVarsSet->at(i)] = *((double*)&pObjData->dwData + i);
			}
			lContext->data->set53 = true;
			break;
		}
		case REQ_STRING: {
			int size = 0;
			for (int i = 0; i < lContext->data->GetRegisteredStringsGetCount(); i++) {
				*SimStringsGet[RegStringsGet->at(i)] = *((char*)&pObjData->dwData + size);
				size = size + std::strlen((char*)&pObjData->dwData + size);
			}
			lContext->data->set60 = true;
			break;
		}
								
		default: {
			lContext->data->SendLog("NA SIMOBJECT DATA: " + QString::number(pObjData->dwRequestID));
			break;
		}
		}
		break;
	}
	case SIMCONNECT_RECV_ID_EVENT: {
		SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;
		switch (evt->uEventID) {
		/*case KEY_EVENT_ELEVATOR_SET: {
			data->SetDataPitch = true;
			break;
		}*/
		case EVENT_SIM_START: {
			SUCC(SimConnect_RequestSystemState(lContext->data->HSimConnect, REQ_AIR_PATH, "AircraftLoaded"));
			break;
		}
		case EVENT_6HZ: {
			//lContext->data->GetData(-1, ZULU_TIME);
			//double dsec = *SimVarsGet[ZULU_TIME];
			//QTime t(0, 0, 0);
			//t = t.addSecs((int)dsec);
			//emit lContext->data->SendText(t.toString("hh:mm:ss"), false);
			//delete a;
			break;
		}
		default: {
			lContext->data->SendLog("NA ID EVENT: " + QString::number(evt->uEventID));
			break;
		}
		}
		break;
	}
	case SIMCONNECT_RECV_ID_SYSTEM_STATE: {// Track aircraft changes 
		SIMCONNECT_RECV_SYSTEM_STATE* evt = (SIMCONNECT_RECV_SYSTEM_STATE*)pData;
		switch (evt->dwRequestID) {
		case REQ_AIR_PATH: {
			if (strstr(evt->szString, "PMDG 737") != NULL) {
				lContext->data->isPMDG737 = true;
				lContext->data->isA320 = false;
				lContext->data->SendLog("Select B737");
			}
			else if (strstr(evt->szString, "Asobo_A320_NEO") != NULL) {
				lContext->data->isA320 = true;
				lContext->data->isPMDG737 = false;
				lContext->data->SendLog("Select A320");
			}
			else {
				lContext->data->isPMDG737 = false;
				lContext->data->isA320 = false;
				lContext->data->SendLog("Select: " + QString(evt->szString));
			}
			break;
		}
		default: {
			lContext->data->SendLog("NA SYSTEM STATE: " + QString::number(evt->dwRequestID));
			break;
		}
		}
		break;
	}
	case SIMCONNECT_RECV_ID_QUIT: {
		lContext->data->SendLog("QUIT\n");
		lContext->data->Quit = 1;
		break;
	}
	case SIMCONNECT_RECV_ID_EXCEPTION: {
		SIMCONNECT_RECV_EXCEPTION* exc = (SIMCONNECT_RECV_EXCEPTION*)pData;
		switch (exc->dwException) {
			/*case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION: {
				Metar = "ERROR METAR";
				metar = std::string(Metar);
				break;
			}*/
		default: {
			lContext->data->SendLog("***** EXCEPTION=" + QString::number(exc->dwException) + "  SendID=" + QString::number(exc->dwSendID) + "  uOffset=" + QString::number(exc->dwIndex) + "  cbData=" + QString::number(cbData));
			break;
		}
					 break;
		}
		break;
	}
	default: {
		lContext->data->SendLog("UNKNOWN DATA RECEIVED: pData->dwID=" + QString::number(pData->dwID) + " cbData=" + QString::number(cbData));
		break;
	}
	}
}

SimData::SimData(HANDLE hSimConnect) {
	HSimConnect = hSimConnect;
	//DataT = this;
  RegVarsGet = new std::vector<DWORD>();
	RegStringsGet = new std::vector<DWORD>();
	RegVarsSet = new std::vector<DWORD>();
	RegVarsSetGet = new std::vector<DWORD>();
	Timer = new QTimer(this);
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[MAGVAR], SimUnitsText[MAGVAR]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AMBIENT_WIND_DIRECTION], "degree"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AMBIENT_WIND_VELOCITY], SimUnitsText[AMBIENT_WIND_VELOCITY]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AMBIENT_PRESSURE], "inHG"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[SEA_LEVEL_PRESSURE], "inHG"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PLANE_PITCH_DEGREES], "Degrees"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PLANE_BANK_DEGREES], "Degrees"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ELEVATOR_POSITION], SimUnitsText[ELEVATOR_POSITION]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AILERON_POSITION], SimUnitsText[AILERON_POSITION]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[RUDDER_PEDAL_POSITION], SimUnitsText[RUDDER_PEDAL_POSITION]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[RUDDER_TRIM], "degree"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ELEVATOR_TRIM_POSITION], "degree"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[BRAKE_PARKING_POSITION], SimUnitsText[BRAKE_PARKING_POSITION]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[FLAPS_HANDLE_INDEX], SimUnitsText[FLAPS_HANDLE_INDEX]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[SPOILERS_ARMED], SimUnitsText[SPOILERS_ARMED]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[SPOILERS_HANDLE_POSITION], SimUnitsText[SPOILERS_HANDLE_POSITION]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[GEAR_HANDLE_POSITION], SimUnitsText[GEAR_HANDLE_POSITION]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[SIM_ON_GROUND], SimUnitsText[SIM_ON_GROUND]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[GROUND_VELOCITY], "Knots"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[VELOCITY_BODY_Z], SimUnitsText[VELOCITY_BODY_Z]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AIRSPEED_TRUE], SimUnitsText[AIRSPEED_TRUE]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AIRSPEED_INDICATED], SimUnitsText[AIRSPEED_INDICATED]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[VERTICAL_SPEED], SimUnitsText[VERTICAL_SPEED]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ROTATION_VELOCITY_BODY_Y], "degrees per second"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[DELTA_HEADING_RATE], "degrees per second"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ACCELERATION_BODY_Z], SimUnitsText[ACCELERATION_BODY_Z]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PLANE_LONGITUDE], "degree"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PLANE_LATITUDE], "degree"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PLANE_ALTITUDE], SimUnitsText[PLANE_ALTITUDE]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PLANE_ALT_ABOVE_GROUND], SimUnitsText[PLANE_ALT_ABOVE_GROUND]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[INDICATED_ALTITUDE], SimUnitsText[INDICATED_ALTITUDE]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PLANE_HEADING_DEGREES_TRUE], "Degrees"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[PUSHBACK_AVAILABLE], SimUnitsText[PUSHBACK_AVAILABLE]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ENG_N1_RPM1], "percent over 100"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ENG_N1_RPM2], "percent over 100"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[TURB_ENG_N11], "percent"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[TURB_ENG_N12], "percent"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[TURB_ENG_IGNITION_SWITCH_EX11], SimUnitsText[TURB_ENG_IGNITION_SWITCH_EX11]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[TURB_ENG_IGNITION_SWITCH_EX12], SimUnitsText[TURB_ENG_IGNITION_SWITCH_EX12]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[GENERAL_ENG_STARTER1], SimUnitsText[GENERAL_ENG_STARTER1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[GENERAL_ENG_STARTER2], SimUnitsText[GENERAL_ENG_STARTER2]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1], SimUnitsText[AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE2], SimUnitsText[AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE2]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AUTOPILOT_ALTITUDE_LOCK_VAR], SimUnitsText[AUTOPILOT_ALTITUDE_LOCK_VAR]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AUTOPILOT_HEADING_LOCK_DIR], SimUnitsText[AUTOPILOT_HEADING_LOCK_DIR]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AUTOPILOT_AIRSPEED_HOLD_VAR], SimUnitsText[AUTOPILOT_AIRSPEED_HOLD_VAR]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[AUTOPILOT_VERTICAL_HOLD_VAR], "feet per minute"));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ANTISKID_BRAKES_ACTIVE], SimUnitsText[ANTISKID_BRAKES_ACTIVE]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[KOHLSMAN_SETTING_HG], SimUnitsText[KOHLSMAN_SETTING_HG]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[CIRCUIT_SWITCH_ON77], SimUnitsText[CIRCUIT_SWITCH_ON77]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[CIRCUIT_SWITCH_ON80], SimUnitsText[CIRCUIT_SWITCH_ON80]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[EXTERNAL_POWER_AVAILABLE1], SimUnitsText[EXTERNAL_POWER_AVAILABLE1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[EXTERNAL_POWER_ON1], SimUnitsText[EXTERNAL_POWER_ON1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[FUELSYSTEM_PUMP_SWITCH1], SimUnitsText[FUELSYSTEM_PUMP_SWITCH1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[FUELSYSTEM_PUMP_SWITCH2], SimUnitsText[FUELSYSTEM_PUMP_SWITCH2]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[FUELSYSTEM_PUMP_SWITCH3], SimUnitsText[FUELSYSTEM_PUMP_SWITCH3]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[FUELSYSTEM_PUMP_SWITCH4], SimUnitsText[FUELSYSTEM_PUMP_SWITCH4]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[FUELSYSTEM_PUMP_SWITCH5], SimUnitsText[FUELSYSTEM_PUMP_SWITCH5]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[FUELSYSTEM_PUMP_SWITCH6], SimUnitsText[FUELSYSTEM_PUMP_SWITCH6]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_POTENTIOMETER7], SimUnitsText[LIGHT_POTENTIOMETER7]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_CABIN], SimUnitsText[LIGHT_CABIN]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_NAV], SimUnitsText[LIGHT_NAV]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_LOGO], SimUnitsText[LIGHT_LOGO]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_BEACON], SimUnitsText[LIGHT_LOGO]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_PANEL4], SimUnitsText[LIGHT_PANEL4]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_POTENTIOMETER86], SimUnitsText[LIGHT_POTENTIOMETER86]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_LANDING], SimUnitsText[LIGHT_LANDING]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_LANDING1], SimUnitsText[LIGHT_LANDING1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_LANDING2], SimUnitsText[LIGHT_LANDING2]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_LANDING3], SimUnitsText[LIGHT_LANDING3]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_LANDING4], SimUnitsText[LIGHT_LANDING4]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_TAXI], SimUnitsText[LIGHT_TAXI]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_TAXI1], SimUnitsText[LIGHT_TAXI1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_TAXI2], SimUnitsText[LIGHT_TAXI2]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_TAXI3], SimUnitsText[LIGHT_TAXI3]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_TAXI4], SimUnitsText[LIGHT_TAXI4]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[LIGHT_STROBE], SimUnitsText[LIGHT_STROBE]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN0], SimUnitsText[NEW_EXIT_OPEN0]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN1], SimUnitsText[NEW_EXIT_OPEN1]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN2], SimUnitsText[NEW_EXIT_OPEN2]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN3], SimUnitsText[NEW_EXIT_OPEN3]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN4], SimUnitsText[NEW_EXIT_OPEN4]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN5], SimUnitsText[NEW_EXIT_OPEN5]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN6], SimUnitsText[NEW_EXIT_OPEN6]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[NEW_EXIT_OPEN7], SimUnitsText[NEW_EXIT_OPEN7]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ATC_CLEARED_TAXI], SimUnitsText[ATC_CLEARED_TAXI]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ATC_CLEARED_TAKEOF], SimUnitsText[ATC_CLEARED_TAKEOF]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ATC_CLEARED_LANDING], SimUnitsText[ATC_CLEARED_LANDING]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ATC_CLEARED_IFR], SimUnitsText[ATC_CLEARED_IFR]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ATC_AIRPORT_IS_TOWERED], SimUnitsText[ATC_AIRPORT_IS_TOWERED]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ATC_RUNWAY_SELECTED], SimUnitsText[ATC_RUNWAY_SELECTED]));
	SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[ATC_RUNWAY_AIRPORT_NAME], NULL, SIMCONNECT_DATATYPE_STRING256));
	

	SimVarsGet[MAGVAR] = new double(0.0);
	SimVarsGet[AMBIENT_WIND_DIRECTION] = new double(0.0);
	SimVarsGet[AMBIENT_WIND_VELOCITY] = new double(0.0);
	SimVarsGet[AMBIENT_PRESSURE] = new double(0.0);
	SimVarsGet[SEA_LEVEL_PRESSURE] = new double(0.0);
	SimVarsGet[PLANE_PITCH_DEGREES] = new double(0.0);
	SimVarsGet[PLANE_BANK_DEGREES] = new double(0.0);
	SimVarsGet[ELEVATOR_POSITION] = new double(0.0);
	SimVarsGet[AILERON_POSITION] = new double(0.0);
	SimVarsGet[RUDDER_PEDAL_POSITION] = new double(0.0);
	SimVarsGet[RUDDER_TRIM] = new double(0.0);
	SimVarsGet[ELEVATOR_TRIM_POSITION] = new double(0.0);
	SimVarsGet[BRAKE_PARKING_POSITION] = new double(0.0);
	SimVarsGet[FLAPS_HANDLE_INDEX] = new double(0.0);
	SimVarsGet[SPOILERS_ARMED] = new double(0.0);
	SimVarsGet[SPOILERS_HANDLE_POSITION] = new double(0.0);
	SimVarsGet[GEAR_HANDLE_POSITION] = new double(0.0);
	SimVarsGet[SIM_ON_GROUND] = new double(0.0);
	SimVarsGet[GROUND_VELOCITY] = new double(0.0);
	SimVarsGet[VELOCITY_BODY_Z] = new double(0.0);
	SimVarsGet[AIRSPEED_TRUE] = new double(0.0);
	SimVarsGet[AIRSPEED_INDICATED] = new double(0.0);
	SimVarsGet[VERTICAL_SPEED] = new double(0.0);
	SimVarsGet[ROTATION_VELOCITY_BODY_Y] = new double(0.0);
	SimVarsGet[DELTA_HEADING_RATE] = new double(0.0);
	SimVarsGet[ACCELERATION_BODY_Z] = new double(0.0);
	SimVarsGet[PLANE_LONGITUDE] = new double(0.0);
	SimVarsGet[PLANE_LATITUDE] = new double(0.0);
	SimVarsGet[PLANE_ALTITUDE] = new double(0.0);
	SimVarsGet[PLANE_ALT_ABOVE_GROUND] = new double(0.0);
	SimVarsGet[INDICATED_ALTITUDE] = new double(0.0);
	SimVarsGet[PLANE_HEADING_DEGREES_TRUE] = new double(0.0);
	SimVarsGet[PUSHBACK_AVAILABLE] = new double(0.0);
	SimVarsGet[ENG_N1_RPM1] = new double(0.0);
	SimVarsGet[ENG_N1_RPM2] = new double(0.0); 
	SimVarsGet[TURB_ENG_N11] = new double(0.0);
	SimVarsGet[TURB_ENG_N12] = new double(0.0); 
	SimVarsGet[TURB_ENG_IGNITION_SWITCH_EX11] = new double(0.0);
	SimVarsGet[TURB_ENG_IGNITION_SWITCH_EX12] = new double(0.0);
	SimVarsGet[GENERAL_ENG_STARTER1] = new double(0.0);
	SimVarsGet[GENERAL_ENG_STARTER2] = new double(0.0);
	SimVarsGet[AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1] = new double(0.0);
	SimVarsGet[AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE2] = new double(0.0);
	SimVarsGet[AUTOPILOT_ALTITUDE_LOCK_VAR] = new double(0.0);
	SimVarsGet[AUTOPILOT_HEADING_LOCK_DIR] = new double(0.0);
	SimVarsGet[AUTOPILOT_AIRSPEED_HOLD_VAR] = new double(0.0);
	SimVarsGet[AUTOPILOT_VERTICAL_HOLD_VAR] = new double(0.0);
	SimVarsGet[ANTISKID_BRAKES_ACTIVE] = new double(0.0);
	SimVarsGet[KOHLSMAN_SETTING_HG] = new double(0.0);
	SimVarsGet[CIRCUIT_SWITCH_ON77] = new double(0.0);
	SimVarsGet[CIRCUIT_SWITCH_ON80] = new double(0.0);
	SimVarsGet[EXTERNAL_POWER_AVAILABLE1] = new double(0.0);
	SimVarsGet[EXTERNAL_POWER_ON1] = new double(0.0);
	SimVarsGet[FUELSYSTEM_PUMP_SWITCH1] = new double(0.0);
	SimVarsGet[FUELSYSTEM_PUMP_SWITCH2] = new double(0.0);
	SimVarsGet[FUELSYSTEM_PUMP_SWITCH3] = new double(0.0);
	SimVarsGet[FUELSYSTEM_PUMP_SWITCH4] = new double(0.0);
	SimVarsGet[FUELSYSTEM_PUMP_SWITCH5] = new double(0.0);
	SimVarsGet[FUELSYSTEM_PUMP_SWITCH6] = new double(0.0);
	SimVarsGet[LIGHT_POTENTIOMETER7] = new double(0.0);
	SimVarsGet[LIGHT_CABIN] = new double(0.0);
	SimVarsGet[LIGHT_NAV] = new double(0.0);
	SimVarsGet[LIGHT_LOGO] = new double(0.0);
	SimVarsGet[LIGHT_BEACON] = new double(0.0);
	SimVarsGet[LIGHT_PANEL4] = new double(0.0);
	SimVarsGet[LIGHT_POTENTIOMETER86] = new double(0.0);
	SimVarsGet[LIGHT_LANDING] = new double(0.0);
	SimVarsGet[LIGHT_LANDING1] = new double(0.0);
	SimVarsGet[LIGHT_LANDING2] = new double(0.0);
	SimVarsGet[LIGHT_LANDING3] = new double(0.0);
	SimVarsGet[LIGHT_LANDING4] = new double(0.0);
	SimVarsGet[LIGHT_TAXI] = new double(0.0);
	SimVarsGet[LIGHT_TAXI1] = new double(0.0);
	SimVarsGet[LIGHT_TAXI2] = new double(0.0);
	SimVarsGet[LIGHT_TAXI3] = new double(0.0);
	SimVarsGet[LIGHT_TAXI4] = new double(0.0);
	SimVarsGet[LIGHT_STROBE] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN0] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN1] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN2] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN3] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN4] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN5] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN6] = new double(0.0);
	SimVarsGet[NEW_EXIT_OPEN7] = new double(0.0);
	SimVarsGet[ATC_CLEARED_TAXI] = new double(0.0);
	SimVarsGet[ATC_CLEARED_TAKEOF] = new double(0.0);
	SimVarsGet[ATC_CLEARED_LANDING] = new double(0.0);
	SimVarsGet[ATC_CLEARED_IFR] = new double(0.0);
	SimVarsGet[ATC_AIRPORT_IS_TOWERED] = new double(0.0);
	/*SimVarsGet[ATC_RUNWAY_AIRPORT_NAME] = new double(0.0);
	SimVarsGet[ATC_RUNWAY_AIRPORT_NAME2] = new double(0.0);
	SimVarsGet[ATC_RUNWAY_AIRPORT_NAME3] = new double(0.0);
	SimVarsGet[ATC_RUNWAY_AIRPORT_NAME4] = new double(0.0);
	SimVarsGet[ATC_RUNWAY_AIRPORT_NAME5] = new double(0.0);
	SimVarsGet[ATC_RUNWAY_AIRPORT_NAME6] = new double(0.0);
	SimVarsGet[ATC_RUNWAY_AIRPORT_NAME7] = new double(0.0);
	SimVarsGet[ATC_RUNWAY_AIRPORT_NAME8] = new double(0.0);*/
	

	RegVarsGet->push_back(MAGVAR);
	RegVarsGet->push_back(AMBIENT_WIND_DIRECTION);
	RegVarsGet->push_back(AMBIENT_WIND_VELOCITY);
	RegVarsGet->push_back(AMBIENT_PRESSURE);
	RegVarsGet->push_back(SEA_LEVEL_PRESSURE);
	RegVarsGet->push_back(PLANE_PITCH_DEGREES);
	RegVarsGet->push_back(PLANE_BANK_DEGREES);
	RegVarsGet->push_back(ELEVATOR_POSITION);
	RegVarsGet->push_back(AILERON_POSITION);
	RegVarsGet->push_back(RUDDER_PEDAL_POSITION);
	RegVarsGet->push_back(RUDDER_TRIM);
	RegVarsGet->push_back(ELEVATOR_TRIM_POSITION);
	RegVarsGet->push_back(BRAKE_PARKING_POSITION);
	RegVarsGet->push_back(FLAPS_HANDLE_INDEX);
	RegVarsGet->push_back(SPOILERS_ARMED);
	RegVarsGet->push_back(SPOILERS_HANDLE_POSITION);
	RegVarsGet->push_back(GEAR_HANDLE_POSITION);
	RegVarsGet->push_back(SIM_ON_GROUND);
	RegVarsGet->push_back(GROUND_VELOCITY);
	RegVarsGet->push_back(VELOCITY_BODY_Z);
	RegVarsGet->push_back(AIRSPEED_TRUE);
	RegVarsGet->push_back(AIRSPEED_INDICATED);
	RegVarsGet->push_back(VERTICAL_SPEED);
	RegVarsGet->push_back(ROTATION_VELOCITY_BODY_Y);
	RegVarsGet->push_back(DELTA_HEADING_RATE);
	RegVarsGet->push_back(ACCELERATION_BODY_Z);
	RegVarsGet->push_back(PLANE_LONGITUDE);
	RegVarsGet->push_back(PLANE_LATITUDE);
	RegVarsGet->push_back(PLANE_ALTITUDE);
	RegVarsGet->push_back(PLANE_ALT_ABOVE_GROUND);
	RegVarsGet->push_back(INDICATED_ALTITUDE);
	RegVarsGet->push_back(PLANE_HEADING_DEGREES_TRUE);
	RegVarsGet->push_back(PUSHBACK_AVAILABLE);
	RegVarsGet->push_back(ENG_N1_RPM1);
	RegVarsGet->push_back(ENG_N1_RPM2);
	RegVarsGet->push_back(TURB_ENG_N11);
	RegVarsGet->push_back(TURB_ENG_N12);
	RegVarsGet->push_back(TURB_ENG_IGNITION_SWITCH_EX11);
	RegVarsGet->push_back(TURB_ENG_IGNITION_SWITCH_EX12);
	RegVarsGet->push_back(GENERAL_ENG_STARTER1);
	RegVarsGet->push_back(GENERAL_ENG_STARTER2);
	RegVarsGet->push_back(AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1);
	RegVarsGet->push_back(AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE2);
	RegVarsGet->push_back(AUTOPILOT_ALTITUDE_LOCK_VAR);
	RegVarsGet->push_back(AUTOPILOT_HEADING_LOCK_DIR);
	RegVarsGet->push_back(AUTOPILOT_AIRSPEED_HOLD_VAR);
	RegVarsGet->push_back(AUTOPILOT_VERTICAL_HOLD_VAR);
	RegVarsGet->push_back(ANTISKID_BRAKES_ACTIVE);
	RegVarsGet->push_back(KOHLSMAN_SETTING_HG);
	RegVarsGet->push_back(CIRCUIT_SWITCH_ON77);
	RegVarsGet->push_back(CIRCUIT_SWITCH_ON80);
	RegVarsGet->push_back(EXTERNAL_POWER_AVAILABLE1);
	RegVarsGet->push_back(EXTERNAL_POWER_ON1);
	RegVarsGet->push_back(FUELSYSTEM_PUMP_SWITCH1);
	RegVarsGet->push_back(FUELSYSTEM_PUMP_SWITCH2);
	RegVarsGet->push_back(FUELSYSTEM_PUMP_SWITCH3);
	RegVarsGet->push_back(FUELSYSTEM_PUMP_SWITCH4);
	RegVarsGet->push_back(FUELSYSTEM_PUMP_SWITCH5);
	RegVarsGet->push_back(FUELSYSTEM_PUMP_SWITCH6);
	RegVarsGet->push_back(LIGHT_POTENTIOMETER7);
	RegVarsGet->push_back(LIGHT_CABIN);
	RegVarsGet->push_back(LIGHT_NAV);
	RegVarsGet->push_back(LIGHT_LOGO);
	RegVarsGet->push_back(LIGHT_BEACON);
	RegVarsGet->push_back(LIGHT_PANEL4);
	RegVarsGet->push_back(LIGHT_POTENTIOMETER86);
	RegVarsGet->push_back(LIGHT_LANDING);
	RegVarsGet->push_back(LIGHT_LANDING1);
	RegVarsGet->push_back(LIGHT_LANDING2);
	RegVarsGet->push_back(LIGHT_LANDING3);
	RegVarsGet->push_back(LIGHT_LANDING4);
	RegVarsGet->push_back(LIGHT_TAXI);
	RegVarsGet->push_back(LIGHT_TAXI1);
	RegVarsGet->push_back(LIGHT_TAXI2);
	RegVarsGet->push_back(LIGHT_TAXI3);
	RegVarsGet->push_back(LIGHT_TAXI4);
	RegVarsGet->push_back(LIGHT_STROBE);
	RegVarsGet->push_back(NEW_EXIT_OPEN0);
	RegVarsGet->push_back(NEW_EXIT_OPEN1);
	RegVarsGet->push_back(NEW_EXIT_OPEN2);
	RegVarsGet->push_back(NEW_EXIT_OPEN3);
	RegVarsGet->push_back(NEW_EXIT_OPEN4);
	RegVarsGet->push_back(NEW_EXIT_OPEN5);
	RegVarsGet->push_back(NEW_EXIT_OPEN6);
	RegVarsGet->push_back(NEW_EXIT_OPEN7);
	RegVarsGet->push_back(ATC_CLEARED_TAXI);
	RegVarsGet->push_back(ATC_CLEARED_TAKEOF);
	RegVarsGet->push_back(ATC_CLEARED_LANDING);
	RegVarsGet->push_back(ATC_CLEARED_IFR);
	RegVarsGet->push_back(ATC_AIRPORT_IS_TOWERED);
	/*RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME);
	RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME2);
	RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME3);
	RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME4);
	RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME5);
	RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME6);
	RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME7);
	RegVarsGet->push_back(ATC_RUNWAY_AIRPORT_NAME8);*/

	connect(Timer, SIGNAL(timeout()), this, SLOT(TimerProc()));
	Timer->start(60);
}

SimData::~SimData() {
  RegVarsGet->clear();
	
  delete RegVarsGet;
}

unsigned int SimData::GetRegisteredVarsGetCount()
{
  return RegVarsGet->size();
}
unsigned int SimData::GetRegisteredVarsSetCount()
{
	return RegVarsSet->size();
}
unsigned int SimData::GetRegisteredStringsGetCount()
{
	return RegStringsGet->size();
}
unsigned int SimData::GetRegisteredVarsSetGetCount()
{
	return RegVarsSetGet->size();
}





void SimData::SendEvent(DWORD sender, DWORD EventID, long dwData) {
	RegisterEvent((CLIENTEVENTS)EventID);
	SUCC(SimConnect_TransmitClientEvent(HSimConnect, SIMCONNECT_OBJECT_ID_USER, EventID, dwData, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY));
	// Test
	//Control.Parameter = 555.555;
	//SimConnect_SetClientData(HSimConnect, A32NX_CONTROL_ID, A32NX_CONTROL_DEFINITION,
//		0, 0, sizeof(A32NX_Control), &Control);
	// Test
	return;
}
void SimData::SendEvent2(DWORD sender, DWORD EventID, long dwData, DWORD var, double val, char* unit) {
	RegisterEvent((CLIENTEVENTS)EventID);
	RegisterVarSet(sender, var, val, unit);
	for (int i = 0; i < RegVarsSet->size(); i++) {
			SendVars[i] = *SimVarsSet[RegVarsSet->at(i)];
	}
	SUCC(SimConnect_SetDataOnSimObject(HSimConnect, sender + 1, SIMCONNECT_OBJECT_ID_USER, 0, 1, RegVarsSet->size() * sizeof(double), &SendVars));
	SUCC(SimConnect_TransmitClientEvent(HSimConnect, SIMCONNECT_OBJECT_ID_USER, EventID, dwData, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY));
	return;
}

bool SimData::RegisterVarGet(DWORD sender, DWORD data, char* unit) {
	if (SimVarsGet[data] == NULL) {
		int i = 0;
			
		SimVarsGet[data] = new double(0.0);
		if (!std::strcmp(unit, "")) {
			SUCC(SimConnect_AddToDataDefinition(HSimConnect, sender, SimVarsText[data], SimUnitsText[data]));
		}
		else {
			SUCC(SimConnect_AddToDataDefinition(HSimConnect, sender, SimVarsText[data], unit));
		}
		if (RegVarsGet->size() == 1) {
			SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, sender, 15, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
		}
		return true;
	}
	return false;;
}
void SimData::RegisterStringGet(DWORD sender, DWORD data) {
	if (SimStringsGet[data] == NULL) {
		int i = 0;
		RegStringsGet->push_back(data);
		SimStringsGet[data] = new std::string("");
		SUCC(SimConnect_AddToDataDefinition(HSimConnect, sender, SimVarsText[data], SimUnitsText[data]));
	}
	return;
}
void SimData::RegisterVarSet(DWORD sender, DWORD data, double var, char* unit) {
	
	if (SimVarsSet[data] == NULL) {
		int i = 0;
		RegVarsSet->push_back(data);
		SimVarsSet[data] = new double(var);
		if (!std::strcmp(unit, "")) {
			SUCC(SimConnect_AddToDataDefinition(HSimConnect, sender + 1, SimVarsText[data], SimUnitsText[data]));
		}
		else {
			SUCC(SimConnect_AddToDataDefinition(HSimConnect, sender + 1, SimVarsText[data], unit));
		}
	} 
	else {
		*SimVarsSet[data] = var;
	}
	//for (int i = 0; i < RegVarsSet->size(); i++) {
//		SendVars[i] = *SimVarsSet[RegVarsSet->at(i)];
//	}
//	SUCC(SimConnect_SetDataOnSimObject(HSimConnect, DEF_DATA_SET, SIMCONNECT_OBJECT_ID_USER, 0, 1, RegVarsSet->size()*sizeof(double), &SendVars));
//	SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, REQ_DATA_SET, DEF_DATA_SET, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
	return;
}


void SimData::RegisterEvent(DWORD clientEvent) {
	if (!RegEvent[clientEvent]) {
		SUCC(SimConnect_MapClientEventToSimEvent(HSimConnect, clientEvent, EventString[clientEvent]));
		RegEvent[clientEvent] = true;
	}
	return;
}


void SimData::GetDataString(DWORD sender, DWORD var, std::string* val)
{
	RegisterStringGet(19, var);
	SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, REQ_STRING, 19, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
	Context c = { this, 60 };

	while (!set60) {
		SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
	};
	*val = *SimVarsGet[var];
	emit GetDataChange(var, sender);
	set50 = false;
}


void SimData::GetData(DWORD sender, DWORD var, double* val, char* unit)
{
	/*Context c = { this, 50 };
	if (RegisterVarGet(15, var, unit)) {
		SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, 15, 15, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
		while (!set50) {

			SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
		};
	}*/
	*val = *SimVarsGet[var];
	emit GetDataChange(var, sender);
	//set50 = false;
}


/*void SimData::GetDataL(DWORD sender, DWORD var, double* val, char* unit)
{
	int f = 100000;
	SendEvent(sender, var, Version);
	Context c = { this, 53 };
	while (ExportData.version != Version) {
		SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
	};
	*val = ExportData.Data;
	Version++;
	LVars[ExportData.id] = var;
	emit GetDataChange(var, sender);
	set53 = false;
}*/



void SimData::SetDataL(DWORD sender, DWORD var, double* val, char* unit)
{
	int f = 100000;
	//RegisterVarGet(sender, var, unit);
	//ExportData.version = false;
	SendEvent(sender, var, Version);
	//SUCC(SimConnect_RequestClientData(HSimConnect, A32NX_LOCAL_DATA_ID, REQ_LOCAL_DATA, A32NX_LOCAL_DATA_DEFINITION,
//		SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0));
	Context c = { this, 53 };
	while (ExportData.version != Version) {
		/*f--;
		if (f == 0) {
			SendEvent(sender, var, Version);
			f = 100000;
		}*/
		SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
	};
	//ExportData.version = false;
	Version++;
	Control.version = Version;
	Control.id = ExportData.id;
	Control.Parameter = *val;
	SimConnect_SetClientData(HSimConnect, A32NX_CONTROL_ID, A32NX_CONTROL_DEFINITION, 0, 0, sizeof(Control), &Control);
	//SUCC(SimConnect_RequestClientData(HSimConnect, A32NX_LOCAL_DATA_ID, REQ_LOCAL_DATA, A32NX_LOCAL_DATA_DEFINITION,
//		SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0));
	c = { this, 54 };
	f = 100000;
	while (ExportData.version != Version) {
		/*f--;
		if (f == 0) {
			Control.version = Version;
			Control.id = ExportData.id;
			Control.Parameter = *val;
			SimConnect_SetClientData(HSimConnect, A32NX_CONTROL_ID, A32NX_CONTROL_DEFINITION, 0, 0, sizeof(Control), &Control);
			f = 100000;
		}*/
		SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
	};
	//ExportData.Changed = false;
	Version++;
	*val = ExportData.Data;
	
	LVars[ExportData.id] = var;
	emit SetDataChange(var, sender);
	set54 = false;
}


void SimData::SetData(DWORD sender, DWORD var, double* val, char* unit)
{
	Context c;
	if (RegVarsSet->size() > 0) {
		SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, sender + 1, 15 + 1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
		c = { this, 51 };
		while (!set51) {
			SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
		};
		set51 = false;
	}
	RegisterVarSet(15, var, *val, unit);
	for (int i = 0; i < RegVarsSet->size(); i++) {
		SendVars[i] = *SimVarsSet[RegVarsSet->at(i)];
	}
	SUCC(SimConnect_SetDataOnSimObject(HSimConnect, 15 + 1, SIMCONNECT_OBJECT_ID_USER, 0, 1, RegVarsSet->size() * sizeof(double), &SendVars));
	SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, sender + 1, 15 + 1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
	c = { this, 53 };
	while (!set51) {
		SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
	};
	*val = *SimVarsSet[var];
	emit SetDataChange(var, sender);
	set51 = false;
}


void SimData::SetSetData(DWORD sender, DWORD var1, double* val1, DWORD var2, double* val2, char* unit1, char* unit2)
{
	Context c;
	if (RegVarsSet->size() > 0) {
		SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, sender + 1, 15 + 1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
		c = { this, 51 };
		while (!set51) {
			SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
		};
		set51 = false;
	}
	RegisterVarSet(15, var1, *val1, unit1);
	RegisterVarSet(15, var2, *val2, unit2);
	for (int i = 0; i < RegVarsSet->size(); i++) {
		SendVars[i] = *SimVarsSet[RegVarsSet->at(i)];
	}
	SUCC(SimConnect_SetDataOnSimObject(HSimConnect, 15 + 1, SIMCONNECT_OBJECT_ID_USER, 0, 1, RegVarsSet->size() * sizeof(double), &SendVars));
	SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, sender + 1, 15 + 1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
	c = { this, 51 };
	while (!set51) {
		SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
	};
	*val1 = *SimVarsSet[var1];
	*val2 = *SimVarsSet[var2];
	emit SetSetDataChange(var1, var2, sender);
	set51 = false;
}


void SimData::SetGetData(DWORD sender, DWORD varSet, DWORD varGet, double* val, char* unit)
{
}
