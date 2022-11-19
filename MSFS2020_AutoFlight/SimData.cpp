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
		case EXTERNAL_SYSTEM_SET:
		{
			//lContext->data->SendLog("[GSX PopUp] timeout: " + QString::number(evt->dwData));
			QFile data("C:\\Program Files (x86)\\Addon Manager\\MSFS\\fsdreamteam-gsx-pro\\html_ui\\InGamePanels\\FSDT_GSX_Panel\\tooltip");
			data.open(QIODevice::ReadOnly);
			QTextStream stream(&data);
			QStringList strings(stream.readAll().split("\n"));
			data.close();
			lContext->data->SendLog("[GSX PopUp]: " + strings[0]);
			break;
		}
		case EXTERNAL_SYSTEM_TOGGLE: {
			QFile data("C:\\Program Files (x86)\\Addon Manager\\MSFS\\fsdreamteam-gsx-pro\\html_ui\\InGamePanels\\FSDT_GSX_Panel\\menu");
			data.open(QIODevice::ReadOnly);
			QTextStream stream(&data);
			//QStringList strings(stream.readAll().split("\n"));
			lContext->data->menu = stream.readAll().split("\n");
			data.close();
			switch (evt->dwData) {
			case 1: {
				//lContext->data->SendLog("[GSX Menu] Reload");
				lContext->data->SendLog("[GSX menu]: " + lContext->data->menu[0]);
				lContext->data->NewGSXQuestion = true;
				break;
			}
			case 2: {
				lContext->data->SendLog("[GSX Menu] Hide)");
				break;
			}
			case 3: {
				lContext->data->SendLog("[GSX Menu] Timeout");
				break;
			}
			case 4: {
				lContext->data->SendLog("[GSX Menu] Force close");
				break;
			}
			default: {
				lContext->data->SendLog("[GSX] N/A: " + QString::number(evt->dwData));
				for (int i = 0; i < lContext->data->menu.size(); i++) {
					lContext->data->SendLog("[GSX menu]: " + lContext->data->menu[i]);
				}
				break;
			}
			}
			break;
		}
		case EVENT_REGISTER_VARIABLE_QUERY: {
			lContext->data->SendLog("EVENT_REGISTER_VARIABLE_QUERY: " + QString::number(evt->dwData, 'f', 3));
			break;
		}
		case EVENT_REGISTER_VARIABLE_RESPONSE: {
			lContext->data->SendLog("EVENT_REGISTER_VARIABLE_RESPONSE: " + QString::number(evt->dwData, 'f', 3));
			break;
		}
		case EVENT_GET_VARIABLE_QUERY: {
			lContext->data->SendLog("EVENT_GET_VARIABLE_QUERY: " + QString((char*)evt->dwData));
			break;
		}
		case EVENT_GET_VARIABLE_RESPONSE: {
			lContext->data->SendLog("EVENT_GET_VARIABLE_RESPONSE: " + QString::number(evt->dwData, 'f', 3));
			break;
		}
		case EVENT_SET_VARIABLE_QUERY: {
			lContext->data->SendLog("EVENT_SET_VARIABLE_QUERY: " + QString::number(evt->dwData, 'f', 3));
			break;
		}
		case EVENT_SET_VARIABLE_RESPONSE: {
			lContext->data->SendLog("EVENT_SET_VARIABLE_RESPONSE: " + QString::number(evt->dwData, 'f', 3));
			break;
		}
		case EVENT_EXEC_CODE_QUERY: {
			lContext->data->SendLog("EVENT_EXEC_CODE_QUERY: " + QString::number(evt->dwData, 'f', 3));
			break;
		}
		case EVENT_EXEC_CODE_RESPONSE: {
			lContext->data->SendLog("EVENT_EXEC_CODE_RESPONSE: " + QString::number(evt->dwData, 'f', 3));
			break;
		}
		case EVENT_MENU_OPEN: {
			lContext->data->SendLog("EVENT_MENU_OPEN: " + QString::number(evt->dwData));
			break;
		}
		case EVENT_MENU_CHOISE: {
			if ((evt->dwData + 1) < lContext->data->menu.size()) {
				lContext->data->SendLog(lContext->data->menu[evt->dwData + 1]);
				lContext->data->NewGSXQuestion = false;
			}
			break;
		}
		default: {

			lContext->data->SendLog("COUATL: " + QString::number(evt->uEventID));
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
			//lContext->data->SendLog("***** EXCEPTION=" + QString::number(exc->dwException) + "  SendID=" + QString::number(exc->dwSendID) + "  uOffset=" + QString::number(exc->dwIndex) + "  cbData=" + QString::number(cbData));
			break;
		}
					 break;
		}
		break;
	}
	default: {
		//lContext->data->SendLog("UNKNOWN DATA RECEIVED: pData->dwID=" + QString::number(pData->dwID) + " cbData=" + QString::number(cbData));
		break;
	}
	}
}

int SimData::AddData(HANDLE hSimConnect, DWORD simData, const char* unitName = "") {
	HRESULT hr;
	if (std::string(unitName) == "") {
		hr = SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[simData], SimUnitsText[simData]);
	}
	else {
		hr = SimConnect_AddToDataDefinition(HSimConnect, DEF_DATA, SimVarsText[simData], unitName);
	}
	if (hr != 0) {
		return hr;
	}
	SimVarsGet[simData] = new double(0.0);
	RegVarsGet->push_back(simData);
	return 0;
}

SimData::SimData(HANDLE hSimConnect) {
	HSimConnect = hSimConnect;
	//DataT = this;
  RegVarsGet = new std::vector<DWORD>();
	RegStringsGet = new std::vector<DWORD>();
	RegVarsSet = new std::vector<DWORD>();
	RegVarsSetGet = new std::vector<DWORD>();
	Timer = new QTimer(this);
	AirportData = NULL;
	AllData.version = 0;
	GData.version = 0;
	VerAll = 0;
	Version = 0;
	set50 = false;
	set51 = false;
	set53 = false;
	set54 = false;
	set60 = false;
	Quit = false;
	RegEvent = std::vector<bool>(1700, false);






	AddData(HSimConnect, MAGVAR);
	AddData(HSimConnect, AMBIENT_WIND_DIRECTION, "degree");
	AddData(HSimConnect, AMBIENT_WIND_VELOCITY);
	AddData(HSimConnect, AMBIENT_PRESSURE, "inHG");
	AddData(HSimConnect, SEA_LEVEL_PRESSURE, "inHG");
	AddData(HSimConnect, PLANE_PITCH_DEGREES, "Degrees");
	AddData(HSimConnect, PLANE_BANK_DEGREES, "Degrees");
	AddData(HSimConnect, ELEVATOR_POSITION);
	AddData(HSimConnect, AILERON_POSITION);
	AddData(HSimConnect, RUDDER_PEDAL_POSITION);
	AddData(HSimConnect, RUDDER_TRIM, "degree");
	AddData(HSimConnect, ELEVATOR_TRIM_POSITION, "degree");
	AddData(HSimConnect, BRAKE_PARKING_POSITION);
	AddData(HSimConnect, FLAPS_HANDLE_INDEX);
	AddData(HSimConnect, SPOILERS_ARMED);
	AddData(HSimConnect, SPOILERS_HANDLE_POSITION);
	AddData(HSimConnect, GEAR_HANDLE_POSITION);
	AddData(HSimConnect, SIM_ON_GROUND);
	AddData(HSimConnect, GROUND_VELOCITY, "Knots");
	AddData(HSimConnect, VELOCITY_BODY_Z);
	AddData(HSimConnect, AIRSPEED_TRUE);
	AddData(HSimConnect, AIRSPEED_INDICATED);
	AddData(HSimConnect, VERTICAL_SPEED, "feet per minute");
	AddData(HSimConnect, ROTATION_VELOCITY_BODY_Y, "degrees per second");
	AddData(HSimConnect, DELTA_HEADING_RATE, "degrees per second");
	AddData(HSimConnect, ACCELERATION_BODY_Z);
	AddData(HSimConnect, PLANE_LONGITUDE, "degree");
	AddData(HSimConnect, PLANE_LATITUDE, "degree");
	AddData(HSimConnect, PLANE_ALTITUDE);
	AddData(HSimConnect, PLANE_ALT_ABOVE_GROUND);
	AddData(HSimConnect, INDICATED_ALTITUDE);
	AddData(HSimConnect, PLANE_HEADING_DEGREES_TRUE, "Degrees");
	AddData(HSimConnect, PUSHBACK_AVAILABLE);
	AddData(HSimConnect, ENG_N1_RPM1, "percent over 100");
	AddData(HSimConnect, ENG_N1_RPM2, "percent over 100");
	AddData(HSimConnect, TURB_ENG_N11, "percent");
	AddData(HSimConnect, TURB_ENG_N12, "percent");
	AddData(HSimConnect, TURB_ENG_IGNITION_SWITCH_EX11);
	AddData(HSimConnect, TURB_ENG_IGNITION_SWITCH_EX12);
	AddData(HSimConnect, GENERAL_ENG_STARTER1);
	AddData(HSimConnect, GENERAL_ENG_STARTER2);
	AddData(HSimConnect, AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1);
	AddData(HSimConnect, AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE2);
	AddData(HSimConnect, AUTOPILOT_ALTITUDE_LOCK_VAR);
	AddData(HSimConnect, AUTOPILOT_HEADING_LOCK_DIR);
	AddData(HSimConnect, AUTOPILOT_AIRSPEED_HOLD_VAR);
	AddData(HSimConnect, AUTOPILOT_VERTICAL_HOLD_VAR, "feet per minute");
	AddData(HSimConnect, ANTISKID_BRAKES_ACTIVE);
	AddData(HSimConnect, KOHLSMAN_SETTING_HG);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON17);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON18);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON19);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON20);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON21);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON22);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON77);
	AddData(HSimConnect, CIRCUIT_SWITCH_ON80);
	AddData(HSimConnect, EXTERNAL_POWER_AVAILABLE1);
	AddData(HSimConnect, EXTERNAL_POWER_ON1);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH1);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH2);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH3);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH4);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH5);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH6);
	AddData(HSimConnect, LIGHT_POTENTIOMETER7);
	AddData(HSimConnect, LIGHT_CABIN);
	AddData(HSimConnect, LIGHT_NAV);
	AddData(HSimConnect, LIGHT_LOGO);
	AddData(HSimConnect, LIGHT_BEACON);
	AddData(HSimConnect, LIGHT_PANEL4);
	AddData(HSimConnect, LIGHT_POTENTIOMETER86);
	AddData(HSimConnect, LIGHT_LANDING);
	AddData(HSimConnect, LIGHT_LANDING1);
	AddData(HSimConnect, LIGHT_LANDING2);
	AddData(HSimConnect, LIGHT_LANDING3);
	AddData(HSimConnect, LIGHT_LANDING4);
	AddData(HSimConnect, LIGHT_TAXI);
	AddData(HSimConnect, LIGHT_TAXI1);
	AddData(HSimConnect, LIGHT_TAXI2);
	AddData(HSimConnect, LIGHT_TAXI3);
	AddData(HSimConnect, LIGHT_TAXI4);
	AddData(HSimConnect, LIGHT_STROBE);
	AddData(HSimConnect, NEW_EXIT_OPEN0);
	AddData(HSimConnect, NEW_EXIT_OPEN1);
	AddData(HSimConnect, NEW_EXIT_OPEN2);
	AddData(HSimConnect, NEW_EXIT_OPEN3);
	AddData(HSimConnect, NEW_EXIT_OPEN4);
	AddData(HSimConnect, NEW_EXIT_OPEN5);
	AddData(HSimConnect, NEW_EXIT_OPEN6);
	AddData(HSimConnect, NEW_EXIT_OPEN7);
	AddData(HSimConnect, ATC_CLEARED_TAXI);
	AddData(HSimConnect, ATC_CLEARED_TAKEOF);
	AddData(HSimConnect, ATC_CLEARED_LANDING);
	AddData(HSimConnect, ATC_CLEARED_IFR);
	AddData(HSimConnect, ATC_AIRPORT_IS_TOWERED);
	AddData(HSimConnect, CABIN_SEATBELTS_ALERT_SWITCH);
	AddData(HSimConnect, TOTAL_WEIGHT, "kg");
	AddData(HSimConnect, YOKE_Y_POSITION);
	

	connect(Timer, SIGNAL(timeout()), this, SLOT(TimerProc()));
	Timer->start(60);
}

SimData::~SimData() {
  RegVarsGet->clear();
	
  delete RegVarsGet;
  Timer->stop();
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
void SimData::SendEvent2(DWORD sender, DWORD EventID, long dwData, DWORD var, double val, const char* unit) {
	RegisterEvent((CLIENTEVENTS)EventID);
	RegisterVarSet(sender, var, val, unit);
	for (int i = 0; i < RegVarsSet->size(); i++) {
			SendVars[i] = *SimVarsSet[RegVarsSet->at(i)];
	}
	SUCC(SimConnect_SetDataOnSimObject(HSimConnect, sender + 1, SIMCONNECT_OBJECT_ID_USER, 0, 1, RegVarsSet->size() * sizeof(double), &SendVars));
	SUCC(SimConnect_TransmitClientEvent(HSimConnect, SIMCONNECT_OBJECT_ID_USER, EventID, dwData, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY));
	return;
}

bool SimData::RegisterVarGet(DWORD sender, DWORD data, const char* unit) {
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
void SimData::RegisterVarSet(DWORD sender, DWORD data, double var, const char* unit) {
	
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


//void SimData::GetData(DWORD sender, DWORD var, double* val, const char* unit)
//{
	/*Context c = { this, 50 };
	if (RegisterVarGet(15, var, unit)) {
		SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, 15, 15, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
		while (!set50) {

			SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
		};
	}*/
	//*val = *SimVarsGet[var];
	//emit GetDataChange(var, sender);
	//set50 = false;
//}


/*void SimData::GetDataL(DWORD sender, DWORD var, double* val, const char* unit)
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



void SimData::SetDataL(DWORD sender, DWORD var, double* val, const char* unit)
{
	int f = 100000;
	//RegisterVarGet(sender, var, unit);
	//ExportData.version = false;
	SendEvent(sender, var, Version);
	//SUCC(SimConnect_RequestClientData(HSimConnect, A32NX_LOCAL_DATA_ID, REQ_LOCAL_DATA, A32NX_LOCAL_DATA_DEFINITION,
	//SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0));
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


void SimData::SetData(DWORD sender, DWORD var, double* val, const char* unit)
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


/*void SimData::SetSetData(DWORD sender, DWORD var1, double* val1, DWORD var2, double* val2, char* unit1, char* unit2)
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
}*/


/*void SimData::SetGetData(DWORD sender, DWORD varSet, DWORD varGet, double* val, const char* unit)
{
}*/
