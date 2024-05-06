#include "SimData.h"
#include <regex>

#include "WasmModuleClient.cpp"

void SimData::TimerProc() {
	
	
	//if (RegVarsGet->size() > 0) {
		GetAllData();
		//SUCC(SimConnect_RequestDataOnSimObject(HSimConnect, 15, 15, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE));
	//}
	
	//SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
}

HRESULT SimData::GetAllData() {
	Context c = { this, SIMDATA_ID };
	HRESULT result;
	
	result = SimConnect_RequestDataOnSimObject(HSimConnect, REQ_DATA, DEF_DATA, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
	/*result = SimConnect_RequestClientData(HSimConnect, A32NX_ALLDATA_ID, REQ_ALL_DATA, A32NX_ALLDATA_DEFINITION,
		SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0);
	result = SimConnect_RequestClientData(HSimConnect, clientData.AREA_RESPONSE_ID, (SIMCONNECT_DATA_REQUEST_ID)clientData.DATA_DEFINITION_ID, clientData.DATA_DEFINITION_ID,
		SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0);*/
	DWORD cbData;
	SIMCONNECT_RECV* pData;
	while (SUCCEEDED(SimConnect_GetNextDispatch(HSimConnect, &pData, &cbData))) {
		FDispatchProc(pData, cbData, (void*)(&c));
	}
	SetServiceStates(DEBOARDING, AllData[FSDT_GSX_DEBOARDING_STATE].Value);
	SetServiceStates(CATERING, AllData[FSDT_GSX_CATERING_STATE].Value);
	SetServiceStates(REFUELING, AllData[FSDT_GSX_REFUELING_STATE].Value);
	SetServiceStates(BOARDING, AllData[FSDT_GSX_BOARDING_STATE].Value);
	SetServiceStates(DEPARTURE, AllData[FSDT_GSX_DEPARTURE_STATE].Value);
	SetServiceStates(DEICING, AllData[FSDT_GSX_DEICING_STATE].Value);
	//RefreshLVarsList();
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
		if (pObjData->dwRequestID == (uint)lContext->data->clientData.DATA_DEFINITION_ID)
		{
			//char* simData = ((char*)&pObjData->dwData);
			char* simData;
			DWORD size_title;
			SimConnect_RetrieveString(pObjData, pObjData->dwSize, &pObjData->dwData, &simData, &size_title);
			//*SimVarsGet[RegVarsGet->at(i)] = *((double*)&pObjData->dwData + i);
			std::string str = simData;
			if (str == "MF.Pong")
			{
				if (!lContext->data->_wasmConnected)
				{
					// Next add runtime client                    
					WasmModuleClient::AddAdditionalClient(lContext->data->HSimConnect, lContext->data->runtimeData.NAME.c_str(), lContext->data->clientData);
				}
			}
			// Runtime client was added		
			else if (str.rfind(lContext->data->runtimeData.NAME))
			{
				SimData::InitializeClientDataAreas(lContext->data->HSimConnect, lContext->data->runtimeData);
				lContext->data->_wasmConnected = true;
				lContext->data->RegisterLVars();
			}
		}
		// Runtime Client Callback
		else if (pObjData->dwRequestID == (uint)lContext->data->runtimeData.DATA_DEFINITION_ID)
		{
			char* simData;
			DWORD size_title;
			SimConnect_RetrieveString(pObjData, pObjData->dwSize, &pObjData->dwData, &simData, &size_title);
			std::string str = simData;
			
			if (str.find("MF.SimVars.Set.") != std::string::npos) {
				std::string prefix = "MF.SimVars.Set.";
				std::string varID = str.substr(prefix.length()+1);
				lContext->data->ExportData.version = std::stoi(varID) - 21000;
			}
			if (str == "MF.LVars.List.Start")
			{
				lContext->data->ResponseStatus = "LVars.List.Receiving";
				lContext->data->dLVars.clear();
			}
			else if (str == "MF.LVars.List.End")
			{
				lContext->data->ResponseStatus = "LVars.List.Completed";
			}
			else if (lContext->data->ResponseStatus == "LVars.List.Receiving")
			{
				lContext->data->dLVars.push_back(simData);
			}

#if DEBUG
			// this only for debug compilation
			// it slows down the client immensly.
			Log.Instance.log($"Received {simData.Data}.", LogSeverity.Debug);
#endif

			}
		// SimVar value callback
		else if (pObjData->dwRequestID > MOBIFLIGHT_STRINGVAR_ID_OFFSET) // -> is string SimVar
		{
			char* simData;
			DWORD size_title;
			SimConnect_RetrieveString(pObjData, pObjData->dwSize, &pObjData->dwData, &simData, &size_title);
			std::string str = simData;

			int simVarIndex = (int)(pObjData->dwRequestID - MOBIFLIGHT_STRINGVAR_ID_OFFSET - 1);

			if (lContext->data->StringSimVars.size() <= simVarIndex || simVarIndex < 0) return;
			lContext->data->StringSimVars[simVarIndex].Data = str;
		}
		else // -> Must be float SimVar
		{

			float simData = *(float*)&pObjData->dwData;
			DWORD size_title = sizeof(float);
			size_title = sizeof(double);

			//INT64 ptrAllData = (INT64)(&lContext->data->AllData);

			int simVarIndex = (int)(pObjData->dwRequestID) - SIMVAR_DATA_DEFINITION_OFFSET;

			if (lContext->data->SimVars.size() <= simVarIndex || simVarIndex < 0) return;
			//double* var = (double*)(ptrAllData + (sizeof(double) * simVarIndex));
			lContext->data->SimVars[simVarIndex].Data = simData;
			//lContext->data->SendLog("Var " + QString(lContext->data->SimVars[simVarIndex].Name.c_str()) + " is change from " + QString::number(*var) + " to " + QString::number(simData));
			lContext->data->AllData[simVarIndex].Value = simData;
			switch (simVarIndex) {
			case FSDT_GSX_DEBOARDING_STATE: {
				lContext->data->SendLog("Service " + QString(lContext->data->GSXServicesName[0].c_str()) + " " + QString(lContext->data->GSXServiceStatesName[(int)simData].c_str()));
				break;
			}
			case FSDT_GSX_CATERING_STATE: {
				lContext->data->SendLog("Service " + QString(lContext->data->GSXServicesName[1].c_str()) + " " + QString(lContext->data->GSXServiceStatesName[(int)simData].c_str()));
				break;
			}
			case FSDT_GSX_REFUELING_STATE: {
				lContext->data->SendLog("Service " + QString(lContext->data->GSXServicesName[2].c_str()) + " " + QString(lContext->data->GSXServiceStatesName[(int)simData].c_str()));
				break;
			}
			case FSDT_GSX_BOARDING_STATE: {
				lContext->data->SendLog("Service " + QString(lContext->data->GSXServicesName[3].c_str()) + " " + QString(lContext->data->GSXServiceStatesName[(int)simData].c_str()));
				break;
			}
			case FSDT_GSX_DEPARTURE_STATE: {
				lContext->data->SendLog("Service " + QString(lContext->data->GSXServicesName[4].c_str()) + " " + QString(lContext->data->GSXServiceStatesName[(int)simData].c_str()));
				break;
			}
			case FSDT_GSX_DEICING_STATE: {
				lContext->data->SendLog("Service " + QString(lContext->data->GSXServicesName[5].c_str()) + " " + QString(lContext->data->GSXServiceStatesName[(int)simData].c_str()));
				break;
			}
			}

			//*var = simData;

		}
		/*switch (pObjData->dwRequestID) {
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
		}*/
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
		case SIMCONNECT_DEFINE_ID::AIRCRAFT_NAME: {
			char* title;
			DWORD size_title;
			SimConnect_RetrieveString(pObjData, pObjData->dwSize, &pObjData->dwData, &title, &size_title);
			//char* title = ((char*)&pObjData->dwData);
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
	case SIMCONNECT_RECV_ID_OPEN: {
		lContext->data->SendLog("OPEN\n");
		// register Events TO DO
			/*foreach(string GroupKey in Events.Keys) {
				foreach(Tuple<string, uint> eventItem in Events[GroupKey])
				{
					var prefix = "";
					if (GroupKey != STANDARD_EVENT_GROUP) prefix = "MobiFlight.";
					(sender).MapClientEventToSimEvent((MOBIFLIGHT_EVENTS)eventItem.Item2, prefix + eventItem.Item1);
				}
			}*/
			// register receive data events
			//SimConnect_OnRecvClientData += SimConnectCache_OnRecvClientData;

			// initialize init client
		SimData::InitializeClientDataAreas(lContext->data->HSimConnect, lContext->data->clientData);


		WasmModuleClient::Ping(lContext->data->HSimConnect, lContext->data->clientData);
		break;
	}
	case SIMCONNECT_RECV_ID_EXCEPTION: {
		SIMCONNECT_RECV_EXCEPTION* exc = (SIMCONNECT_RECV_EXCEPTION*)pData;
		switch (exc->dwException) {
			case SIMCONNECT_EXCEPTION_NONE: {
				lContext->data->SendLog("EXCEPTION=" + QString("NONE") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_ERROR: {
				lContext->data->SendLog("EXCEPTION=" + QString("ERROR") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_SIZE_MISMATCH: {
				lContext->data->SendLog("EXCEPTION=" + QString("SIZE MISMATCH") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID: {
				lContext->data->SendLog("EXCEPTION=" + QString("UNRECOGNIZED ID") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_UNOPENED: {
				lContext->data->SendLog("EXCEPTION=" + QString("UNOPENED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_VERSION_MISMATCH: {
				lContext->data->SendLog("EXCEPTION=" + QString("VERSION MISMATCH") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_TOO_MANY_GROUPS: {
				lContext->data->SendLog("EXCEPTION=" + QString("TOO MANY GROUPS") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED: {
				lContext->data->SendLog("EXCEPTION=" + QString("NAME UNRECOGNIZED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_TOO_MANY_EVENT_NAMES: {
				lContext->data->SendLog("EXCEPTION=" + QString("TOO MANY EVENT NAMES") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_EVENT_ID_DUPLICATE: {
				lContext->data->SendLog("EXCEPTION=" + QString("EVENT ID DUPLICATE") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_TOO_MANY_MAPS: {
				lContext->data->SendLog("EXCEPTION=" + QString("TOO MANY MAPS") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_TOO_MANY_OBJECTS: {
				lContext->data->SendLog("EXCEPTION=" + QString("TOO MANY OBJECTS") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_TOO_MANY_REQUESTS: {
				lContext->data->SendLog("EXCEPTION=" + QString("TOO MANY REQUESTS") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_WEATHER_INVALID_PORT: {
				lContext->data->SendLog("EXCEPTION=" + QString("WEATHER INVALID PORT") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_WEATHER_INVALID_METAR: {
				lContext->data->SendLog("EXCEPTION=" + QString("WEATHER INVALID METAR") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION: {
				lContext->data->SendLog("EXCEPTION=" + QString("WEATHER UNABLE TO GET OBSERVATION") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_CREATE_STATION: {
				lContext->data->SendLog("EXCEPTION=" + QString("WEATHER UNABLE TO CREATE STATION") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_REMOVE_STATION: {
				lContext->data->SendLog("EXCEPTION=" + QString("WEATHER UNABLE TO REMOVE STATION") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_INVALID_DATA_TYPE: {
				lContext->data->SendLog("EXCEPTION=" + QString("INVALID DATA TYPE") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_INVALID_DATA_SIZE: {
				lContext->data->SendLog("EXCEPTION=" + QString("INVALID DATA SIZE") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_DATA_ERROR: {
				lContext->data->SendLog("EXCEPTION=" + QString("DATA ERROR") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_INVALID_ARRAY: {
				lContext->data->SendLog("EXCEPTION=" + QString("INVALID ARRAY") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED: {
				lContext->data->SendLog("EXCEPTION=" + QString("CREATE OBJECT FAILED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_LOAD_FLIGHTPLAN_FAILED: {
				lContext->data->SendLog("EXCEPTION=" + QString("(<L:OAD FLIGHTPLAN FAILED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE: {
				lContext->data->SendLog("EXCEPTION=" + QString("OPERATION INVALID FOR OBJECT TYPE") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION: {
				lContext->data->SendLog("EXCEPTION=" + QString("ILLEGAL OPERATION") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED: {
				lContext->data->SendLog("EXCEPTION=" + QString("ALREADY SUBSCRIBED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_INVALID_ENUM: {
				lContext->data->SendLog("EXCEPTION=" + QString("INVALID ENUM") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_DEFINITION_ERROR: {
				lContext->data->SendLog("EXCEPTION=" + QString("DEFINITION ERROR") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_DUPLICATE_ID: {
				lContext->data->SendLog("EXCEPTION=" + QString("DUPLICATE ID") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_DATUM_ID: {
				lContext->data->SendLog("EXCEPTION=" + QString("DATUM ID") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_OUT_OF_BOUNDS: {
				lContext->data->SendLog("EXCEPTION=" + QString("OUT OF BOUNDS") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_ALREADY_CREATED: {
				lContext->data->SendLog("EXCEPTION=" + QString("ALREADY CREATED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_OBJECT_OUTSIDE_REALITY_BUBBLE: {
				lContext->data->SendLog("EXCEPTION=" + QString("OBJECT OUTSIDE REALITY BUBBLE") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_OBJECT_CONTAINER: {
				lContext->data->SendLog("EXCEPTION=" + QString("OBJECT CONTAINER") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_OBJECT_AI: {
				lContext->data->SendLog("EXCEPTION=" + QString("OBJECT AI") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_OBJECT_ATC: {
				lContext->data->SendLog("EXCEPTION=" + QString("OBJECT ATC") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_OBJECT_SCHEDULE: {
				lContext->data->SendLog("EXCEPTION=" + QString("OBJECT SCHEDULE") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_JETWAY_DATA: {
				lContext->data->SendLog("EXCEPTION=" + QString("JETWAY DATA") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_ACTION_NOT_FOUND: {
				lContext->data->SendLog("EXCEPTION=" + QString("ACTION NOT FOUND") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_NOT_AN_ACTION: {
				lContext->data->SendLog("EXCEPTION=" + QString("NOT AN ACTION") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_INCORRECT_ACTION_PARAMS: {
				lContext->data->SendLog("EXCEPTION=" + QString("INCORRECT ACTION PARAMS") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_GET_INPUT_EVENT_FAILED: {
				lContext->data->SendLog("EXCEPTION=" + QString("GET INPUT EVENT FAILED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
			case SIMCONNECT_EXCEPTION_SET_INPUT_EVENT_FAILED: {
				lContext->data->SendLog("EXCEPTION=" + QString("SET INPUT EVENT FAILED") + "  SendID = " + QString::number(exc->dwSendID) + "  uOffset = " + QString::number(exc->dwIndex) + "  cbData = " + QString::number(cbData));
				break;
			}
		default: {
			lContext->data->SendLog("***** EXCEPTION=" + QString::number(exc->dwException) + "  SendID=" + QString::number(exc->dwSendID) + "  uOffset=" + QString::number(exc->dwIndex) + "  cbData=" + QString::number(cbData));
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

void SimData::RefreshLVarsList()
{
	if (HSimConnect == NULL) return;
	WasmModuleClient::GetLVarList(HSimConnect, runtimeData);
}

void SimData::InitializeClientDataAreas(HANDLE HSimConnect, WasmModuleClientData clientData)
{
	// register Client Data (for SimVars)
	SimConnect_MapClientDataNameToID(HSimConnect, (clientData.NAME + ".LVars").c_str(), clientData.AREA_SIMVAR_ID);
	SimConnect_CreateClientData(HSimConnect, clientData.AREA_SIMVAR_ID, 4096, SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT);

	// register Client Data (for WASM Module Commands)
	SimConnect_MapClientDataNameToID(HSimConnect, (clientData.NAME + ".Command").c_str(), clientData.AREA_COMMAND_ID);
	SimConnect_CreateClientData(HSimConnect, clientData.AREA_COMMAND_ID, MOBIFLIGHT_MESSAGE_SIZE, SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT);

	// register Client Data (for WASM Module Responses)
	SimConnect_MapClientDataNameToID(HSimConnect, (clientData.NAME + ".Response").c_str(), clientData.AREA_RESPONSE_ID);
	SimConnect_CreateClientData(HSimConnect, clientData.AREA_RESPONSE_ID, MOBIFLIGHT_MESSAGE_SIZE, SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT);

	// register Client Data (for String-SimVars)
	SimConnect_MapClientDataNameToID(HSimConnect, (clientData.NAME + ".StringVars").c_str(), clientData.AREA_STRINGSIMVAR_ID);
	SimConnect_CreateClientData(HSimConnect, clientData.AREA_STRINGSIMVAR_ID, MOBIFLIGHT_STRINGVAR_DATAAREA_SIZE, SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT);

	SimConnect_AddToClientDataDefinition(HSimConnect, clientData.DATA_DEFINITION_ID,
		clientData.RESPONSE_OFFSET, MOBIFLIGHT_MESSAGE_SIZE);

	//SimConnect_RegisterStruct<SIMCONNECT_RECV_CLIENT_DATA, ResponseString>(clientData.DATA_DEFINITION_ID);
	SimConnect_RequestClientData(
		HSimConnect,
		clientData.AREA_RESPONSE_ID,
		(SIMCONNECT_DATA_REQUEST_ID)clientData.DATA_DEFINITION_ID,
		clientData.DATA_DEFINITION_ID,
		SIMCONNECT_CLIENT_DATA_PERIOD_ONCE,
		SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT,
		0,
		0,
		0
	);
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

SimData::SimData(HANDLE hSimConnect, WasmModuleClientData clientData, WasmModuleClientData runtimeData) {
	this->clientData = clientData;
	this->runtimeData = runtimeData;
	HSimConnect = hSimConnect;
	//DataT = this;
  RegVarsGet = new std::vector<DWORD>();
	RegStringsGet = new std::vector<DWORD>();
	RegVarsSet = new std::vector<DWORD>();
	RegVarsSetGet = new std::vector<DWORD>();
	Timer = new QTimer(this);
	AirportData = NULL;
	//AllData.version = 0;
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

	WasmModuleClient::SetConfig(HSimConnect, "MAX_VARS_PER_FRAME", "5", clientData);




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
	//AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH3);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH2);
	AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH3);
	//AddData(HSimConnect, FUELSYSTEM_PUMP_SWITCH2);
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
	AddData(HSimConnect, PUSHBACK_STATE);
	AddData(HSimConnect, PUSHBACK_ATTACHED);

	



	

	connect(Timer, SIGNAL(timeout()), this, SLOT(TimerProc()));
	Timer->start(120);
}

void SimData::AddLocalData(std::string LVarName, int priority) {
	std::string StringValue = "";
	double DoubleValue = 0.0;
	std::string LVarFullName = "(L:" + LVarName + ")";
	if (GetSimVar(LVarFullName, &StringValue, &DoubleValue, priority) == FSUIPCOffsetType::Float) {
		SendLog(QString(LVarFullName.c_str()) + " = " + QString::number(DoubleValue));
	}
	else {
		SendLog(QString(LVarFullName.c_str()) + " = " + QString(StringValue.c_str()));
	}
}

void SimData::RegisterLVars() {
	WasmModuleClient::SendWasmCmd(HSimConnect, "MF.SimVars.Clear", runtimeData);
	for (int i = 0; i < AllData.size(); i++) {
		AddLocalData(AllData[i].Name, AllData[i].Priority);
	}
	double val = 2.0;
	//SetDataL2(SIMDATA_ID, XMLVAR_SWITCH_OVHD_INTLT_SEATBELT_Position, &val);
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
	//emit SendLog("Send event " + QString(EventString[EventID]) + " with " + QString::number(dwData));
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

FSUIPCOffsetType SimData::RegisterSimVar(std::string SimVarName)
{
	// Matches presets like "(A:TITLE,String)" in different variations. These will most likely be of type String an will therefore be treated as String.
	std::regex r("\\(.*A.*:.*,.*String.*\\)");
	
	//Match stringPreset = Regex.Match(SimVarName, "\\(.*A.*:.*,.*String.*\\)", RegexOptions.IgnoreCase);
	FSUIPCOffsetType simVarType = std::regex_match(SimVarName, r) ? FSUIPCOffsetType::String : FSUIPCOffsetType::Float;

	if (simVarType == FSUIPCOffsetType::Float)
	{
		// Register SimVar as float
		SimVar newSimVar = SimVar{(UINT)(SimVars.size() + SIMVAR_DATA_DEFINITION_OFFSET), SimVarName, 0.0};
		SimVars.push_back(newSimVar);
		if (MaxClientDataDefinition < (SimVars.size() + SIMVAR_DATA_DEFINITION_OFFSET - 1 + StringSimVars.size()))
		{
			// Register SimVar as float
			SimConnect_AddToClientDataDefinition(
				HSimConnect,
				(SIMCONNECT_CLIENT_DATA_DEFINITION_ID)newSimVar.ID,
				(uint)((newSimVar.ID - SIMVAR_DATA_DEFINITION_OFFSET) * sizeof(float)),
				sizeof(float),
				0,
				0);

			//m_oSimConnect ? .RegisterStruct<SIMCONNECT_RECV_CLIENT_DATA, ClientDataValue>((SIMCONNECT_DEFINE_ID)newSimVar.ID);

			SimConnect_RequestClientData(
				HSimConnect,
				runtimeData.AREA_SIMVAR_ID,
				(SIMCONNECT_DATA_REQUEST_ID)newSimVar.ID,
				(SIMCONNECT_CLIENT_DATA_DEFINITION_ID)newSimVar.ID,
				SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET,
				SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED,
				0,
				0,
				0
			);

			MaxClientDataDefinition = (uint)(SimVars.size() + SIMVAR_DATA_DEFINITION_OFFSET - 1 + StringSimVars.size());
		}
	}
	else
	{
		// Register SimVar as string (different ID-offset, size and a separate data-area)
		StringSimVar newStringSimVar = StringSimVar{(uint)StringSimVars.size() + 1 + MOBIFLIGHT_STRINGVAR_ID_OFFSET, SimVarName, ""};
		StringSimVars.push_back(newStringSimVar);
		if (MaxClientDataDefinition < (SimVars.size() + SIMVAR_DATA_DEFINITION_OFFSET - 1 + StringSimVars.size()))
		{
			SimConnect_AddToClientDataDefinition(
				HSimConnect,
				(SIMCONNECT_CLIENT_DATA_DEFINITION_ID)(newStringSimVar.ID),
				(uint)((StringSimVars.size() - 1) * MOBIFLIGHT_STRINGVAR_SIZE),
				MOBIFLIGHT_STRINGVAR_SIZE,
				0,
				0);

			//m_oSimConnect ? .RegisterStruct<SIMCONNECT_RECV_CLIENT_DATA, ClientDataStringValue>((SIMCONNECT_DEFINE_ID)newStringSimVar.ID);

			SimConnect_RequestClientData(
				HSimConnect,
				runtimeData.AREA_STRINGSIMVAR_ID,
				(SIMCONNECT_DATA_REQUEST_ID)(newStringSimVar.ID),
				(SIMCONNECT_CLIENT_DATA_DEFINITION_ID)(newStringSimVar.ID),
				SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET,
				SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED,
				0,
				0,
				0
			);

			MaxClientDataDefinition = (uint)(SimVars.size() + SIMVAR_DATA_DEFINITION_OFFSET - 1 + StringSimVars.size());
		}
	}

	return simVarType;
}

FSUIPCOffsetType SimData::GetSimVar(std::string simVarName, std::string* stringVal, double* floatVal, int priority)
{
	FSUIPCOffsetType simVarType = FSUIPCOffsetType::Float;
	bool isFloat = false;
	bool isString = false;

	*stringVal = "0";
	*floatVal = 0.0F;

	//if (!IsConnected())
	//	return simVarType;

	if (simVarName == "")
		return simVarType;

	std::vector<SimVar>::iterator var = std::find_if(SimVars.begin(), SimVars.end(), [&](SimVar& sv) { return sv.Name == simVarName; });
		//isFloat = true; //TO DO
	//isFloat = std::find_if(SimVars.begin(), SimVars.end(), [&simVarName](SimVar& sv) { return sv.Name == simVarName; });
	if (!isFloat)
	{
		std::vector<StringSimVar>::iterator var = std::find_if(StringSimVars.begin(), StringSimVars.end(), [&](StringSimVar& ssv) { return ssv.Name == simVarName; });
		//isString = true; //TO DO

		if (!isString)
		{
			simVarType = RegisterSimVar(simVarName);
			if (simVarType == FSUIPCOffsetType::String)
			{
				WasmModuleClient::AddStringSimVar(HSimConnect, simVarName.c_str(), runtimeData);
			}
			else
			{
				WasmModuleClient::AddSimVar(HSimConnect, simVarName.c_str(), runtimeData, priority);
			}
		}
		else
		{
			simVarType = FSUIPCOffsetType::String;
		}
	}

	if (simVarType == FSUIPCOffsetType::Float)
	{
		std::vector<SimVar>::iterator var = std::find_if(SimVars.begin(), SimVars.end(), [&](SimVar& sv) { return sv.Name == simVarName; });
		*floatVal = var->Data;
		//*floatVal = SimVars.Find(lvar => lvar.Name == simVarName).Data;
	}
	else
	{
		std::vector<StringSimVar>::iterator var = std::find_if(StringSimVars.begin(), StringSimVars.end(), [&](StringSimVar& ssv) { return ssv.Name == simVarName; });
		*stringVal = var->Data;
		//*stringVal = StringSimVars.Find(lvar = > lvar.Name == simVarName).Data;
		*stringVal = (*stringVal) == "" ? "0" : *stringVal;
	}

	return simVarType;
}

void SimData::SetDataL2(DWORD sender, DWORD var, double* val, const char* unit)
{
	setlocale(LC_ALL, "");
	//RegisterEvent((CLIENTEVENTS)var);	
	std::string SimVarCode = "(>" + AllData[var].Name + ")";
	std::string strVal = std::to_string(*val);
	std::size_t pos = strVal.find(",");
	if (pos != std::string::npos) {
		strVal.replace(pos, 1, ".");
	}
	std::string ExecuteCode = "(" + strVal + " " + SimVarCode + ")";
	WasmModuleClient::SetSimVar(HSimConnect, ExecuteCode.c_str(), runtimeData);
	Context c = { this, 53 };
	while ((ExportData.version != var)) {
		SimConnect_RequestClientData(
			HSimConnect,
			runtimeData.AREA_RESPONSE_ID,
			(SIMCONNECT_DATA_REQUEST_ID)runtimeData.DATA_DEFINITION_ID,
			runtimeData.DATA_DEFINITION_ID,
			SIMCONNECT_CLIENT_DATA_PERIOD_ONCE,
			SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT,
			0,
			0,
			0
		);
		SUCC(SimConnect_CallDispatch(HSimConnect, FDispatchProc, (void*)(&c)));
		Sleep(1);
		if (ExportData.version == -2) {
			SendLog(QString("Fail set var ") + QString(AllData[var].Name.c_str()));
			break;
		}
	};
	ExportData.version = -1;
	emit SetDataChange(var, sender);
	set54 = false;
}


void SimData::SetDataL(DWORD sender, DWORD var, double* val, const char* unit)
{
	//emit SendLog("Set " + QString(EventString[var]) + " to " + QString::number(*val));
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
		Sleep(1);
	};
	//ExportData.version = false;
	Version = ExportData.version;
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
		Sleep(1);
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
	emit SendLog("Set " + QString(SimVarsText[var]) + " to " + QString::number(*val));
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
