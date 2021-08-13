#pragma once

/*#include "headers.h"


#define MHR(chr) if (chr == E_FAIL) hr = E_FAIL; else hr = S_OK;
#define SUCC(chr) if (chr == E_FAIL) {/*TO DO Add error behavior*\};

namespace PMDG_TEST {

#define PMDG_ADD_DATA_NAME			"PMDG_Additional_Data"
#define PMDG_ADD_DATA_ID			0x4E877777
#define PMDG_ADD_DATA_DEFINITION	0x4E877778

	HANDLE  hSimConnect = NULL; //Идентификатор подключения SimConnect
	HDC hDC = NULL;
	HDC aDC = NULL;
	HPEN hPenSolidRed;
	HPEN hPenSolid2Blue;
	HPEN hPenSolid2Green;
	HPEN hPenDottedRed;
	HPEN hPenSolid2Grey;
	HPEN hPenSolid2Black;
	HPEN hOldPen;
	HRESULT hr; //Результат выполнения команд SimConnect
	//A20N_Data A20N; //Переменная для приема данных B737NGX
	bool GetNGXData = FALSE; //Индикатор первого приема данных B737NGX
	bool AircraftRunning = false; //Индикатор загрузки B737NGX
	SIMMATH::DSHEH* Ways[500]; //
	SIMMATH::DSHEH CurrentPos;  //Текущее положение самолета
	int STARPoint = 0;
	int AppPoint = 0;
	int SIDPoint = 0;
	int RoutePoint = 0;
	int CountWays = 0; //Количество путей
	int CurrentWay = 0; //Текущий путь 
	bool flare = true;
	bool TOGA = FALSE; //Индикатор активности включения режима TO/GA
	bool Connected = FALSE; //Индикатор связи с SimConnect
	int     quit = 0; //Индикатор окончания работы программы
	bool SetDataPitch = true; //Индикатор изменения принимаемых данных о тангаже
	bool SetDataBank = true; //Индикатор изменения принимаемых данных о крене
	double YScaleGraph = 30;
	double XScaleGraph = 60;
	long CurrentGraphIndex = 0;
	long MaxIndexGraph = 0;
	GraphPair* GraphArray = new GraphPair[MaxIndexGraphS];
	GraphPair* GraphArray2 = new GraphPair[MaxIndexGraphS];
	GraphPair* GraphArray3 = new GraphPair[MaxIndexGraphS];
	bool TestControl = FALSE;
	alterPid VSPID = { -0.0008, 0.0, 0, 0.1, 1.0, 0.0, 1.5, 0.001, 0.0, 0.0, 0.0, 0, -15.0, 15.0, true };
	alterPid PPID = { -0.2, 0.0, 0, 0.1, 1.0, 0.0, 2.0, 0.001, 0.0, 0.0, 0.0, 0, -1.0, 1.0, true };
	SPid pidBank = { 0.0, 0.0, 1500.0, 15.0, 5.0, 0.0, 0.0, 0.0, 0.0 };
	SPid pidPitch = { 0.0, 0.0, PPID.Kp, PPID.Ti, PPID.Td, 0.0, 0.0, 0.0, 0.0 };
	SPid pidVSpeed = { 0.0, 0.0, VSPID.Kp, VSPID.Ti, VSPID.Td, 0.0, 0.0, 0.0, 0.0 };
	SPid* SelectedPid;

	static enum MODE {
		PREPARE,
		START,
		PUSHBACK,
		TAXI_OUT,
		TAKE_OFF,
		ABORT_TAKE_OFF,
		CIRCLE_TAXI,
		CLIMB,
		CRUISE,
		DESCENT,
		LANDING,
		ABORT_LANDING,
		BRAKE,
		TAXI_IN,
		PARK,
		SHUTDOWN,
		SECURE
	};
	MODE mode = initmode; //Начальный режим (определяется в INITVARS.h)
	static enum DATA_REQUEST_ID {
		REQ_NGX_DATA, //Запрос данных B737NGX
		REQ_NGX_CONTROL, //Запрос контроля B737NGX
		REQ_AIR_PATH, //Запрос пути к файлу *.air загруженного самолета
		REQ_AIRCRAFT_USER, //Запрос переменных необходимых для программы во всех режимах полета
		REQ_SPEEDS, //Запрос данных скорости
		REQ_PUSH_SPEED, //Запрос данных скорости
		REQ_THRUST, //Запрос данных тяги
		REQ_ADD_NGX_DATA, //Запрос дополнительных данных, формируемых LED.DLL
		REQ_ELEVATOR, //Запрос положения руля высоты
		REQ_WEATHER_LATLON,
		REQ_DOUBLES,
		REQ_BOOLS,
		REQ_UINT,
	};
	static enum RECV_ID_ASSIGNED_OBJECT_ID {
		REQ_ATTACH,
		REQ_WING,
		REQ_ATTACH1,
		REQ_ATTACH2,
		REQ_ATTACH3,
		REQ_ATTACH4,
		REQ_ATTACH5,
		REQ_ATTACH6,
		REQ_ATTACH7,
		REQ_ATTACH8,
		REQ_ATTACH9,
		REQ_ATTACH10,
	};
	static enum DATA_DEFINE_ID {
		DEF_AIRCRAFT_USER, //Структура aircraftData необходимая для программы во всех режимах полета
		DEF_SPEEDS, //Структура tspeeds данных скорости
		DEF_THRUST, //Структура tthrust данных тяги
		DEF_THROTTLE, //Структура tthrottle данных РУД
		DEF_ELEVATOR, //Переменная положения руля высоты
		DEF_AILERON, ////Переменная положения руля крена
		DEF_SPOILER,
		DEF_BRAKE,
		DEF_SPEED,
		DEF_A20N_INSTRUMENTS,
		DEFINITION3,
		DEF_DOUBLES,
		DEF_BOOLS,
		DEF_UINT,
	};
	static enum EVENT_ID {
		EVENT_SIM_START,	// Сим загружен
		EVENT_LOGO_LIGHT_SWITCH, //Осталось от СДК
		EVENT_FLIGHT_DIRECTOR_SWITCH, //Осталось от СДК
		EVENT_HEADING_SELECTOR, //Осталось от СДК
		EVENT_KEYBOARD_A, //Нажата комбинация ctrl+a
		EVENT_KEYBOARD_B, //Нажата комбинация ctrl+b
		EVENT_KEYBOARD_C, //Нажата комбинация ctrl+c
		EVENT_KEYBOARD_D, //Нажата комбинация ctrl+d
		EVENT_THROTTLE_SET, //Установка РУД
		EVENT_RUDDER_SET, //Установка руля направления
		EVENT_ELEVATOR_SET, //Установка руля высоты
		EVENT_AILERON_SET, //Установка руля крена
		EVENT_CONTROL_STAND_FLAPS_LEVER_30, //Установка положения закрылок 30 градусов
		EVENT_CONTROL_STAND_FLAPS_LEVER_20, //Установка положения закрылок 5 градусов
		EVENT_CONTROL_STAND_FLAPS_LEVER_15, //Установка положения закрылок 5 градусов
		EVENT_CONTROL_STAND_FLAPS_LEVER_5, //Установка положения закрылок 5 градусов
		EVENT_CONTROL_STAND_FLAPS_LEVER_1, //Установка положения закрылок 1 градус
		EVENT_CONTROL_STAND_FLAPS_LEVER_0, //Установка положения закрылок 0 градусов
		EVENT_TOGA, //Включение режима TO/GA взлета/ухода
		EVENT_TEXT,
		EVENT_CONTROL_STAND_SPEED_BRAKE_LEVER_DOWN,
		EVENT_CONTROL_STAND_SPEED_BRAKE_LEVER_UP,
		EVENT_GEAR_LEVER,
		EVENT_GEAR_OFF,
		EVENT_GEAR_UN,
		EVENT_GEAR,
		KEY_AP_ALT_VAR_INCd,
		KEY_AP_ALT_VAR_DECd,
		KEY_FLAPS_INCRd,
		KEY_FLAPS_DECRd,
		KEY_SPOILERS_TOGGLEd,
		A320_Neo_CDU_MODE_SELECTED_ALTITUDEd,
		TOGGLE_FLIGHT_DIRECTORd,

		EVENT1,
		EVENT2,
		EVENT_MENU_1,
		EVENT_MENU_2,
		EVENT_TEXT_1,
		EVENT_MESSAGE_1,
		EVENT_TEXT_WINDOW_CREATED,
		EVENT_OH_ELEC_GRD_PWR_SWITCH
	};
	static enum INPUT_ID {
		INPUT0,
		INPUT1
	};
	static enum GROUP_ID {
		GROUP_KEYBOARD,		// Нажатие клавиш
		GROUP0, // Прочие события
		GROUP1, // Меню
		GROUP_ELEVATOR // Событие перестановки руля высоты
	};
	static enum TYPE_PATH {
		TAXIWAY,
		RUNWAY,
		WAYPOINT,
		FIXHEADING,
		HEADINGUNTILALT,
		HEADINGUNTILRADIALTOFIX,
		FIXATORABOVE,
		TURNLEFTDIRECTFIX,
		TURNRIGHTDIRECTFIX,
		TURNRIGHTDIRECTFIXOVERFLY,
		TURNLEFTDIRECTFIXOVERFLY,
		TURNRIGHTDIRECTFIXOVERFLYATORABOVE,
		TURNLEFTDIRECTFIXOVERFLYATORABOVE,
		FIX,
		TRKINTERCEPTRADIAL,
		TRKINTERCEPTRADIALTOFIXATORABOVE,
		FIXOVERFLY,
		FIXOVERFLYATORABOVE,
		HDGVECTORS,
		HEADINGUNTILFROMDIST,
		GSWAY,
		GATE,
	};



	void CALLBACK FDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);




	double GetAnglePM180(double angle) {
		double Tangle;
		Tangle = angle;
		while (Tangle > 180) {
			Tangle = Tangle - 360;
		}
		while (Tangle < -180) {
			Tangle = Tangle + 360;
		}
		return Tangle;
	}





	double GetAngle360(double angle) {
		double Tangle;
		Tangle = angle;
		while (Tangle < 0) {
			Tangle = Tangle + 360;
		}
		while (Tangle >= 360) {
			Tangle = Tangle - 360;
		}
		return Tangle;
	}




	double GetDistanceToWayType(int wayType) {
		int icw = CurrentWay;
		double D = CurrentPos.D;
		while ((Ways[icw]->typePath != GSWAY) && (icw < (CountWays - 1))) {
			D += Ways[icw + 1]->D;
			icw++;
		}
		return D;
	}




	double GetFixDA(double da, double angle) {
		if (((angle < 0) && (da > 0)) || ((angle > 0) && (da < 0))) {
			return -da;
		}
		return da;
	}




	double AngleLimitS(double angle, double limit) {
		if (angle > limit) {
			return limit;
		}
		else if (angle < -limit) {
			return -limit;
		}
		return angle;
	}




	double CalcToNewWay(bool changeWay = true) {
		double REangle = 0.045;
		if ((CurrentWay + 1) < CountWays) {
			REangle = Ways[CurrentWay + 1]->SH - Ways[CurrentWay]->EH;
		}
		REangle = GetAnglePM180(REangle);
		REangle = fabs(-REangle * 5 * PMDG_TEST::speeds.AS / 100000);
		if (AircraftData.simOnGround == 0) {
			if (REangle < 2.0) {
				REangle = 2.0;
			}
		}
		double EangleRel = CurrentPos.EH - Ways[CurrentWay]->EH;
		EangleRel = GetAnglePM180(EangleRel);
		if ((CurrentPos.typePath == HEADINGUNTILALT)) {
			if ((AircraftData.alt > CurrentPos.Ella.Altitude)) {
				CurrentWay = CurrentWay + 1;
			}
		}
		else if (CurrentPos.typePath == HEADINGUNTILFROMDIST) {
			if (CurrentPos.D > CurrentPos.Dist) {
				CurrentWay = CurrentWay + 1;
			}
		}
		else if (CurrentPos.typePath == TRKINTERCEPTRADIAL) {
			static int fl;
			if (abs(abs(CurrentPos.EH) - abs(Ways[CurrentWay]->Dist)) < fl) {
				if (abs(abs(CurrentPos.EH) - abs(Ways[CurrentWay]->Dist)) < 1.0) {
					CurrentWay = CurrentWay + 1;
					fl = 0;
				}
				if ((CurrentPos.D < REangle)) {
					CurrentWay = CurrentWay + 1;
					fl = 0;
				}
			}
			else {
				CurrentWay = CurrentWay + 1;
				fl = 0;
			}
			fl = abs(abs(CurrentPos.EH) - abs(Ways[CurrentWay]->Dist)) < fl;
		}
		else if ((CurrentPos.typePath == FIX) || (CurrentPos.typePath == TURNLEFTDIRECTFIX) || (CurrentPos.typePath == TURNRIGHTDIRECTFIX)) {
			if ((CurrentPos.D < REangle)) {
				CurrentWay = CurrentWay + 1;
			}
		}
		else if (changeWay) {
			if ((EangleRel > 60) || (EangleRel < -60) || (CurrentPos.D < REangle)) {
				CurrentWay = CurrentWay + 1;
			}
		}
		return REangle;
	}




	bool SetTimeOff(int IDREQ, int TimeOffset) {
		int i = 0;
		int Find = -1;
		clock_t eTime = clock();
		struct sTimerOffs {
			int IDReq;
			clock_t sTime;
			int TimeOff;
		};
		static sTimerOffs IDs[100];
		for (i = 0; i < 100; i++) {
			if (IDs[i].IDReq == IDREQ) {
				Find = i;
				break;
			}
		}

		if (Find == -1) {
			for (i = 0; i < sizeof(IDs); i++) {
				if (IDs[i].IDReq == 0) {
					Find = i;
					break;
				}
			}
			IDs[Find].IDReq = IDREQ;
			IDs[Find].sTime = clock();
			IDs[Find].TimeOff = TimeOffset;
			return false;
		}
		else {
			if ((IDs[Find].sTime + IDs[Find].TimeOff) < eTime) {
				IDs[Find].IDReq = 0;
				return true;
			}
			else {
				return false;
			}
		}
	}




	void AddText(QString text, bool simDisplay = false) {
		/*if (MainForm::mainForm->textBox1->InvokeRequired) {
			SetText^ d = gcnew SetText(AddText);
			text = text + "\n" + gcnew String(metar.c_str());
			MainForm::mainForm->Invoke(d, gcnew array<Object^> {text, simDisplay});
		}
		else {
			if (simDisplay) {
				String^ sss = text + "\n" + gcnew String(metar.c_str());
				char* ctext = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(sss).ToPointer();
				int szz = text->Length;
				//SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_PRINT_RED, 1, EVENT_TEXT, sss->Length + 1, ctext);
			}
			MainForm::mainForm->textBox1->Clear();
			MainForm::mainForm->textBox1->AppendText(text);
		}*\
	}



	void RegisterEvent(int clientEvent) {
		if (!RegEvent[clientEvent]) {
			hr = SimConnect_MapClientEventToSimEvent(hSimConnect, clientEvent, EventString[clientEvent]);
			RegEvent[clientEvent] = true;
		}
		return;
	}
	void RegisterDoubleVar(double** data, char* unit = "") {
		if (long long(*data) == NULL) {
			int i = 0;
			*data = &RegVarsDouble[VarsDoubleCount];
			long long sVars = sizeof(cVars) / sizeof(void*);
			long long p = ((long long)data - (long long)&Vars);
			long long ip = p / sizeof(void*);
			if (!std::strcmp(unit, "")) {
				hr = SimConnect_AddToDataDefinition(hSimConnect, DEF_DOUBLES, SimVarsText[ip], SimUnitsText[ip]);
			}
			else {
				hr = SimConnect_AddToDataDefinition(hSimConnect, DEF_DOUBLES, SimVarsText[ip], unit);
			}
			VarsDoubleCount++;
		}
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_DOUBLES, DEF_DOUBLES, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
		Sleep(30);
		SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
		return;
	}
	void RegisterBoolVar(bool** data) {
		if (long long(*data) == NULL) {
			int i = 0;
			*data = &RegVarsBool[VarsBoolCount];
			long long sVars = sizeof(cVars) / sizeof(void*);
			long long p = ((long long)data - (long long)&Vars);
			long long ip = p / sizeof(void*);
			hr = SimConnect_AddToDataDefinition(hSimConnect, DEF_BOOLS, SimVarsText[ip], SimUnitsText[ip]);
			VarsBoolCount++;
		}
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_BOOLS, DEF_BOOLS, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
		Sleep(30);
		SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
		return;
	}
	void RegisterUIntVar(unsigned int** data) {
		if (long long(*data) == NULL) {
			int i = 0;
			*data = &RegVarsUInt[VarsUIntCount];
			long long sVars = sizeof(cVars) / sizeof(void*);
			long long p = ((long long)data - (long long)&Vars);
			long long ip = p / sizeof(void*);
			hr = SimConnect_AddToDataDefinition(hSimConnect, DEF_UINT, SimVarsText[ip], SimUnitsText[ip]);
			VarsUIntCount++;
		}
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_UINT, DEF_UINT, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
		Sleep(30);
		SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
		return;
	}

	SIMCONNECTAPI ExSimConnect_TransmitClientEvent(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_CLIENT_EVENT_ID EventID, DWORD dwData, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, SIMCONNECT_EVENT_FLAG Flags) {
		RegisterEvent(EventID);
		return SimConnect_TransmitClientEvent(hSimConnect, ObjectID, EventID, dwData, GroupID, Flags);
	}

	
	bool setLightMask(unsigned int** DefaultParameter, double Parameter) {
		RegisterUIntVar(DefaultParameter);
		int i = 500;
		while (**DefaultParameter != Parameter) {
			SimConnect_SetDataOnSimObject(hSimConnect, DEF_UINT, 0, 0, 1, sizeof(double), &Parameter);
			double back = **DefaultParameter;
			while ((**DefaultParameter == back) && (i > 0)) {
				hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_UINT, DEF_UINT, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
				Sleep(30);
				SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
				i--;
			}
			i--;
		}
		return true;
	}
	bool setA20NDataDouble(int EventUp, int EventDown, double Parameter, double** DefaultParameter, double lay, char* unit = "") {
		RegisterEvent(EventUp);
		RegisterEvent(EventDown);
		RegisterDoubleVar(DefaultParameter, unit);
		int i = 500;
		while (((**DefaultParameter + lay < Parameter) ||
			(**DefaultParameter - lay > Parameter)) && (i > 0)) {

			//while (i) {
			if (**DefaultParameter + lay < Parameter) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventUp, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			else if (**DefaultParameter - lay > Parameter) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventDown, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			double back = **DefaultParameter;
			while ((**DefaultParameter == back) && (i > 0)) {
				hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_DOUBLES, DEF_DOUBLES, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
				Sleep(30);
				SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
				i--;
			}
			i--;
		}
		if (i) {
			return false;
		}
		return true;
	}
	bool getA20NDataDouble(double** DefaultParameter, char* unit = "") {
		RegisterDoubleVar(DefaultParameter, unit);
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_DOUBLES, DEF_DOUBLES, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
		Sleep(30);
		SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
		return true;
	}
	bool getA20NDataBool(bool** DefaultParameter, char* unit = "") {
		RegisterBoolVar(DefaultParameter);
		hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_BOOLS, DEF_BOOLS, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
		Sleep(30);
		SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
		return true;
	}
	double NormailzeHeading(double source, double destination) {
		destination = GetAnglePM180(destination);
		source = GetAnglePM180(source);
		double angle = source - destination;
		return GetAnglePM180(angle);
	}
	bool setA20NDataHeading(int EventUp, int EventDown, double Parameter, double** DefaultParameter, double lay, char* unit = "") {
		RegisterEvent(EventUp);
		RegisterEvent(EventDown);
		RegisterDoubleVar(DefaultParameter, unit);
		int i = 500;
		while (((**DefaultParameter + lay < Parameter) ||
			(**DefaultParameter - lay > Parameter)) && (i > 0)) {
			double DeltaHeading = NormailzeHeading(**DefaultParameter, Parameter);
			if (DeltaHeading + lay < 0) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventUp, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			else if (DeltaHeading - lay > 0) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventDown, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			double back = **DefaultParameter;
			while ((**DefaultParameter == back) && (i > 0)) {
				hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_DOUBLES, DEF_DOUBLES, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
				Sleep(30);
				SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
				i--;
			}
			i--;
		}
		if (i) {
			return false;
		}
		return true;
	}
	bool setA20NDataUShort(int EventUp, int EventDown, UINT Parameter, UINT* DefaultParameter) {
		int i = 100;
		while ((*DefaultParameter != Parameter) && i) {
			if (*DefaultParameter < Parameter) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventUp, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			else if (*DefaultParameter > Parameter) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventDown, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			UINT back = *DefaultParameter;
			while (*DefaultParameter == back) {
				SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
				Sleep(30);
			}
			i--;
		}
		if (i) {
			return false;
		}
		return true;
	}
	bool setA20NDataBool(int EventUp, int EventDown, bool Parameter, bool** DefaultParameter, int number = 0) {
		RegisterEvent(EventUp);
		RegisterEvent(EventDown);
		RegisterBoolVar(DefaultParameter);
		int i = 100;
		while ((**DefaultParameter != Parameter) && (i > 0)) {
			if (**DefaultParameter < Parameter) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventUp, number, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			else if (**DefaultParameter > Parameter) {
				SimConnect_TransmitClientEvent(hSimConnect, 0, EventDown, number, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			}
			UINT back = **DefaultParameter;
			while ((**DefaultParameter == back) && (i > 0)) {
				hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQ_BOOLS, DEF_BOOLS, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
				Sleep(30);
				SimConnect_CallDispatch(hSimConnect, FDispatchProc, NULL);
				i--;
			}
			i--;
		}
		if (i) {
			return false;
		}
		return true;
	}


	



	HRESULT SetUpBrake(double percent) {
		percent = percent / 32.767 - 16384;
		HRESULT hr = S_OK;
		MHR(ExSimConnect_TransmitClientEvent(hSimConnect, 0, KEY_AXIS_LEFT_BRAKE_SET, percent, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY));
		MHR(ExSimConnect_TransmitClientEvent(hSimConnect, 0, KEY_AXIS_RIGHT_BRAKE_SET, percent, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY));
		return hr;
	}




	float GetAngleToDesc(float alt){
		return (alt / 48148) + 2.0;//2.19 old(2.938 - old, 2.748 - new ) 
	}
}*/