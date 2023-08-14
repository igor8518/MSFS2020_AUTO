// lvars_bridge.cpp
#include <MSFS\MSFS.h>
#include <MSFS\MSFS_WindowsTypes.h>
#include <SimConnect.h>
#include <MSFS\Legacy\gauges.h>
#include <iostream>
#include <string.h>
#include <vector>

#include "lvars_bridge.h"


HANDLE g_hSimConnect; //1289
HRESULT hr; //2580

const char* var_10324_init[] = {
	"FSDT_GSX_AIRCRAFT_EXIT_1_TOGGLE",
	"FSDT_GSX_AIRCRAFT_EXIT_2_TOGGLE",
	"FSDT_GSX_AIRCRAFT_EXIT_3_TOGGLE",
	"FSDT_GSX_AIRCRAFT_EXIT_4_TOGGLE",
	"FSDT_GSX_AIRCRAFT_EXIT_5_TOGGLE",
	"FSDT_GSX_AIRCRAFT_CARGO_1_TOGGLE",
	"FSDT_GSX_AIRCRAFT_CARGO_2_TOGGLE",
	"FSDT_GSX_AIRCRAFT_CARGO_3_TOGGLE",
	"FSDT_GSX_AIRCRAFT_SERVICE_1_TOGGLE",
	"FSDT_GSX_AIRCRAFT_SERVICE_2_TOGGLE",
	"FSDT_GSX_AIRCRAFT_SERVICE_3_TOGGLE",
	"FSDT_GSX_AIRCRAFT_MAINCARGO_TOGGLE",
	"FSDT_GSX_DEICING_STATE",
	"FSDT_GSX_DEICING_TYPE",
	"FSDT_GSX_DEICING_CONCENTRATION",
	"FSDT_GSX_NUMPASSENGERS",
	"FSDT_GSX_NUMPASSENGERS_BOARDING",
	"FSDT_GSX_NUMPASSENGERS_BOARDING_TOTAL",
	"FSDT_GSX_NUMPASSENGERS_DEBOARDING",
	"FSDT_GSX_NUMPASSENGERS_DEBOARDING_TOTAL",
	"FSDT_GSX_NUMPASSENGERS_TOTAL",
	"FSDT_GSX_BATTERY_VOLTAGE",
	"FSDT_GSX_MAX_BATTERY_VOLTAGE",
	"FSDT_GSX_DISABLE_DOORS_MSG",
	"FSDT_GSX_DISABLE_DETECT_REFUELING",
	"FSDT_GSX_BOARDING_CARGO_PERCENT",
	"FSDT_GSX_DEBOARDING_CARGO_PERCENT",
	"FSDT_GSX_NUMPILOTS",
	"FSDT_GSX_PILOTS_NOT_BOARDING",
	"FSDT_GSX_PILOTS_NOT_DEBOARDING",
	"FSDT_GSX_PILOTS_ON_BOARD",
	"FSDT_GSX_NUMCREW",
	"FSDT_GSX_CREW_NOT_BOARDING",
	"FSDT_GSX_CREW_NOT_DEBOARDING",
	"FSDT_GSX_CREW_ON_BOARD",
	"FSDT_GSX_FUEL_COUNTER",
	"FSDT_GSX_FUEL_COUNTER_MAX",
	"FSDT_GSX_FUEL_PRICE",
	"FSDT_GSX_FUELHOSE_CONNECTED",
	"FSDT_GSX_STATE",
	"FSDT_GSX_ENGINE_START_CONFIRMED",
	"FSDT_GSX_MENU_OPEN",
	"FSDT_GSX_PASSENGERGATE",
	"FSDT_GSX_CARGO_STAND",
};

std::vector<std::string> var_10324(var_10324_init, std::end(var_10324_init));
int var_10336 = 0;
int var_10337;
ID g_QQQ; //2110
ID g_menuOpen; //2111
ID g_menuChoice; //2112

enum eIDs {
	ID_REGISTER_VARIABLE,
	ID_GET_VARIABLE,
	ID_SET_VARIABLE,
	ID_EXEC_CODE,
};

enum eDefs {
	DEF_REGISTER_VARIABLE,
	DEF_GET_VARIABLE,
	DEF_SET_VARIABLE,
	DEF_EXEC_CODE,
};

enum eRequests {
	REQ_REGISTER_VARIABLE,
	REQ_GET_VARIABLE,
	REQ_SET_VARIABLE,
	REQ_EXEC_CODE,
};

enum eObjects {
	OBJ_VARIABLE,
};

enum eEvents {
	EVENT_REGISTER_VARIABLE_QUERY,
	EVENT_REGISTER_VARIABLE_RESPONSE,
	EVENT_GET_VARIABLE_QUERY,
	EVENT_GET_VARIABLE_RESPONSE,
	EVENT_SET_VARIABLE_QUERY,
	EVENT_SET_VARIABLE_RESPONSE,
	EVENT_EXEC_CODE_QUERY,
	EVENT_EXEC_CODE_RESPONSE,
	EVENT_MENU_OPEN,
	EVENT_MENU_CHOISE,
	EVENT_6HZ,
	EVENT_SIM_START,
	EVENT_FLIGHT_LOADED,
};

enum eGroups {
	GROUP_0,
	GROUP_1,
};

void resetVars(void) {
	for (std::string str: var_10324) {
		int varID = check_named_variable(str.c_str());
		if (varID == -1) {
			varID = register_named_variable(str.c_str());
		}
		set_named_variable_value(varID, 0.0);
	}
}

void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);

extern "C" MSFS_CALLBACK void module_init(void) {
	g_hSimConnect = 0;
	hr = SimConnect_Open(&g_hSimConnect, "fsdt-msfs-bridge", NULL, 0, 0, 0);
	if (hr != S_OK) {
		fprintf(stderr, "fsdt-msfs-bridge: Could not open SimConnect connection.\n");
		return;
	}
	hr = SimConnect_CallDispatch(g_hSimConnect, MyDispatchProc, NULL);
	if (hr != S_OK) {
		fprintf(stderr, "fsdt-msfs-bridge: Could not set dispatch proc.\n");
		return;
	}
	fprintf(stdout, "fsdt-msfs-bridge: Simconnect Dispatch proc set.\n");
}

extern "C" MSFS_CALLBACK void module_deinit(void)
{

	if (!g_hSimConnect)
		return;
	HRESULT hr = SimConnect_Close(g_hSimConnect);
	if (hr != S_OK)
	{
		fprintf(stderr, "Could not close SimConnect connection.\n");
	}

}



void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
	//1468 stdout
		//356 stderr
	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_OPEN:
	{
		fprintf(stdout, "fsdt-msfs-bridge: Opened SimConnect connection.\n");

		SimConnect_MapClientDataNameToID(g_hSimConnect, "WASM.APIData.RegisterVariable", ID_REGISTER_VARIABLE);
		SimConnect_MapClientDataNameToID(g_hSimConnect, "WASM.APIData.GetVariable", ID_GET_VARIABLE);
		SimConnect_MapClientDataNameToID(g_hSimConnect, "WASM.APIData.SetVariable", ID_SET_VARIABLE);
		SimConnect_MapClientDataNameToID(g_hSimConnect, "WASM.APIData.ExecCode", ID_EXEC_CODE);

		SimConnect_CreateClientData(g_hSimConnect, ID_REGISTER_VARIABLE, 144, 0);
		SimConnect_CreateClientData(g_hSimConnect, ID_SET_VARIABLE, 16, 0);
		SimConnect_CreateClientData(g_hSimConnect, ID_GET_VARIABLE, 16, 0);
		SimConnect_CreateClientData(g_hSimConnect, ID_EXEC_CODE, 1032, 0);

		SimConnect_AddToClientDataDefinition(g_hSimConnect, DEF_REGISTER_VARIABLE, -1, 144, 0.0, -1);
		SimConnect_AddToClientDataDefinition(g_hSimConnect, DEF_SET_VARIABLE, -1, 16, 0.0, -1);
		SimConnect_AddToClientDataDefinition(g_hSimConnect, DEF_GET_VARIABLE, -1, 16, 0.0, -1);
		SimConnect_AddToClientDataDefinition(g_hSimConnect, DEF_EXEC_CODE, -1, 1032, 0.0, -1);

		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_REGISTER_VARIABLE_QUERY, "WASM.RegisterVariableQuery");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_REGISTER_VARIABLE_RESPONSE, "WASM.RegisterVariableResponse");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_GET_VARIABLE_QUERY, "WASM.GetVariableQuery");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_GET_VARIABLE_RESPONSE, "WASM.GetVariableResponse");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_SET_VARIABLE_QUERY, "WASM.SetVariableQuery");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_SET_VARIABLE_RESPONSE, "WASM.SetVariableResponse");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_EXEC_CODE_QUERY, "WASM.ExecCodeQuery");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_EXEC_CODE_RESPONSE, "WASM.ExecCodeResponse");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_MENU_OPEN, "WASM.MenuOpen");
		SimConnect_MapClientEventToSimEvent(g_hSimConnect, EVENT_MENU_CHOISE, "WASM.MenuChoice");

		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_REGISTER_VARIABLE_QUERY, TRUE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_REGISTER_VARIABLE_RESPONSE, FALSE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_GET_VARIABLE_QUERY, TRUE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_GET_VARIABLE_RESPONSE, FALSE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_SET_VARIABLE_QUERY, TRUE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_SET_VARIABLE_RESPONSE, FALSE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_EXEC_CODE_QUERY, TRUE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_EXEC_CODE_RESPONSE, FALSE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_MENU_OPEN, FALSE);
		SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP_0, EVENT_MENU_CHOISE, FALSE);

		SimConnect_SetNotificationGroupPriority(g_hSimConnect, GROUP_0, SIMCONNECT_GROUP_PRIORITY_HIGHEST_MASKABLE);

		SimConnect_SubscribeToSystemEvent(g_hSimConnect, EVENT_6HZ, "6Hz");
		SimConnect_SubscribeToSystemEvent(g_hSimConnect, EVENT_SIM_START, "SimStart");
		SimConnect_SubscribeToSystemEvent(g_hSimConnect, EVENT_FLIGHT_LOADED, "FlightLoaded");
		break;
	}
	case SIMCONNECT_RECV_ID_EVENT_FILENAME:
	{
		/*var uf : int = f[311] : int;
		f[308]:int = uf;
		var vf : int_ptr@1 = f[308]:int;
		var wf : int = vf[16];
		var xf : int = 12;
		var yf : int = wf != xf;
		if (yf) goto B_h;
		var zf : int = f[308] : int;
		var ag : int = 24;
		var bg : int = zf + ag;
		var cg : int = 1216;
		var dg : int = f + cg;
		var eg : int = dg;
		string_string_nullptr(eg, bg);
		var fg : int = 1200;
		var gg : int = f + fg;
		var hg : int = gg;
		var ig : int = 7711;
		string_string_nullptr(hg, ig);
		var jg : int = 1216;
		var kg : int = f + jg;
		var lg : int = kg;
		var mg : int = string_begin_1(lg);
		f[298]:int = mg;
		var ng : int = 1216;
		var og : int = f + ng;
		var pg : int = og;
		var qg : int = string_end(pg);
		f[296]:int = qg;
		var rg : int = 1216;
		var sg : int = f + rg;
		var tg : int = sg;
		var ug : int = string_begin_1(tg);
		f[294]:int = ug;
		var vg : int = f[298] : int;
		var wg : int = f[296] : int;
		var xg : int = f[294] : int;
		var yg : int =
			wrap_iter_transform_wrap_iter_wrap_iter_MyDispatchProc_0(vg, wg, xg);
		f[290]:int = yg;
		var zg : int = 1216;
		var ah : int = f + zg;
		var bh : int = ah;
		var ch : int = string_size_1(bh);
		var dh : int = 1200;
		var eh : int = f + dh;
		var fh : int = eh;
		var gh : int = string_size_1(fh);
		var hh : int = ch;
		var ih : int = gh;
		var jh : int = hh >= ih;
		var kh : int = 0;
		var lh : int = 1;
		var mh : int = jh & lh;
		var nh : int = kh;
		if (eqz(mh)) goto B_i;
		var oh : int = 1216;
		var ph : int = f + oh;
		var qh : int = ph;
		var rh : int = string_size_1(qh);
		var sh : int = 1200;
		var th : int = f + sh;
		var uh : int = th;
		var vh : int = string_size_1(uh);
		var wh : int = rh - vh;
		var xh : int = 1200;
		var yh : int = f + xh;
		var zh : int = yh;
		var ai : int = string_size_1(zh);
		var bi : int = 1216;
		var ci : int = f + bi;
		var di : int = ci;
		var ei : int = 1200;
		var fi : int = f + ei;
		var gi : int = fi;
		var hi : int = string_compare_8(di, wh, ai, gi);
		var ii : int = 0;
		var ji : int = ii;
		var ki : int = hi;
		var li : int = ji == ki;
		nh = li;
		label B_i :
		var mi : int = nh;
		var ni : int = 1;
		var oi : int = mi & ni;
		var pi : byte_ptr = 0;
		pi[10337] = oi;
		var qi : ubyte_ptr = 0;
		var ri : int = qi[10337];
		var si : int = 1;
		var ti : int = ri & si;
		if (ti) goto B_j;
		var ui : int = 7724;
		var vi : int = check_named_variable(ui);
		var wi : int_ptr = 0;
		wi[2112] = vi;
		var xi : int_ptr = 0;
		var yi : int = xi[2112];
		var zi : int = -1;
		var aj : int = yi;
		var bj : int = zi;
		var cj : int = aj == bj;
		var dj : int = 1;
		var ej : int = cj & dj;
		if (eqz(ej)) goto B_k;
		var fj : int = 7724;
		var gj : int = register_named_variable(fj);
		var hj : int_ptr = 0;
		hj[2112] = gj;
		label B_k :
		var ij : int_ptr = 0;
		var jj : int = ij[2112];
		var kj : double = -2.0;
		set_named_variable_value(jj, kj);
		label B_j :
		var lj : int = 1200;
		var mj : int = f + lj;
		var nj : int = mj;
		string_string(nj);
		var oj : int = 1216;
		var pj : int = f + oj;
		var qj : int = pj;
		string_string(qj);
		label B_h :*/
	}
	case SIMCONNECT_RECV_ID_EVENT:
	{ 
		SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;
		switch (evt->uEventID) {
		case EVENT_SIM_START: {
			//label B_s 
			g_menuOpen = check_named_variable("FSDT_GSX_MENU_OPEN"); //7078
			if (g_menuOpen == -1) {
				g_menuOpen = register_named_variable("FSDT_GSX_MENU_OPEN");
			}
			g_menuChoice = check_named_variable("FSDT_GSX_MENU_CHOICE"); //7724
			if (g_menuChoice == -1) {
				g_menuChoice = register_named_variable("FSDT_GSX_MENU_CHOICE");
			}
			fprintf(stdout, "fsdt-msfs-bridge: FSDT_GSX_MENU_OPEN LVar registered with id %d\n", g_menuOpen);
			fprintf(stdout, "fsdt-msfs-bridge: FSDT_GSX_MENU_CHOICE LVar registered with id %d\n", g_menuChoice); 
			break;
		}
		case EVENT_6HZ: {
			//label B_r :
			if (var_10336 != 1) {
				break;
			}
			if (var_10337 != 1) {
				double var = get_named_variable_value(g_menuOpen);
				if (var == 1.0) {
					fprintf(stdout, "fsdt-msfs-bridge: You opened the JS menu\n");
					SimConnect_TransmitClientEvent(g_menuChoice, OBJ_VARIABLE, EVENT_MENU_OPEN, 0, GROUP_1, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
					set_named_variable_value(g_menuOpen, 0.0);
				}
				var = get_named_variable_value(g_menuChoice);
				int vm;
				if (abs(var) >= 2147483648.0) {
					vm = -2147483648;
				}
				else {
					vm = int(var);
				}
				if (vm != -2) {
					fprintf(stdout, "fsdt-msfs-bridge: You chose option %d\n", vm);
				}
				SimConnect_TransmitClientEvent(g_menuChoice, OBJ_VARIABLE, EVENT_MENU_CHOISE, vm, GROUP_1, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
				set_named_variable_value(g_menuChoice, -2.0);
			}
			double var = get_named_variable_value(g_QQQ);
			if (var != 0.0) {
				var_10336 = 0;
			}
			break;
		}
		case EVENT_REGISTER_VARIABLE_QUERY: {
			SimConnect_RequestClientData(g_hSimConnect, ID_REGISTER_VARIABLE, REQ_REGISTER_VARIABLE, DEF_REGISTER_VARIABLE, SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, 0, 0, 0, 0);
			break;
		}
		case EVENT_GET_VARIABLE_QUERY: {
			double var = get_named_variable_value(evt->dwData);
			SimConnect_SetClientData(g_hSimConnect, ID_GET_VARIABLE, DEF_GET_VARIABLE, 0, 0, 16, &var);
			SimConnect_TransmitClientEvent(g_hSimConnect, OBJ_VARIABLE, EVENT_GET_VARIABLE_RESPONSE, 0, GROUP_1, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			break;
		}
		case EVENT_SET_VARIABLE_QUERY: {
			SimConnect_RequestClientData(g_hSimConnect, ID_SET_VARIABLE, REQ_SET_VARIABLE, DEF_SET_VARIABLE, SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, 0, 0, 0, 0);
			break;
		}
		case EVENT_EXEC_CODE_QUERY: {
			SimConnect_RequestClientData(g_hSimConnect, ID_EXEC_CODE, REQ_EXEC_CODE, DEF_EXEC_CODE, SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, 0, 0, 0, 0);
			break;
		}
		default: {
			break;
		}
		}
		break;
	}
	case SIMCONNECT_RECV_ID_CLIENT_DATA:
	{
		struct VARIABLE {
			ID varID;
			PCSTRINGZ varName;
		};
		VARIABLE*lvar_276 = 0;
		VARIABLE*lvar_277 = 0;
		VARIABLE*lvar_278 = 0;
		SIMCONNECT_RECV_CLIENT_DATA* pObjData = (SIMCONNECT_RECV_CLIENT_DATA*)pData;
		switch (pObjData->dwDefineID) {
		case DEF_EXEC_CODE: {
			lvar_278 = (VARIABLE*)&pObjData->dwData;
			break;
		}
		case DEF_REGISTER_VARIABLE: {
			lvar_277 = (VARIABLE*)&pObjData->dwData;
			break;
		}
		case DEF_GET_VARIABLE:
		case DEF_SET_VARIABLE:{
			lvar_276 = (VARIABLE*)&pObjData->dwData;
			break;
		}
		default: {
			break;
		}
		}

		switch (pObjData->dwRequestID) {
		case REQ_REGISTER_VARIABLE: {	
			int varID = check_named_variable(lvar_277->varName);
			if (varID == -1) {
				varID = register_named_variable(lvar_277->varName);
			}
			SimConnect_TransmitClientEvent(g_hSimConnect, OBJ_VARIABLE, EVENT_REGISTER_VARIABLE_RESPONSE, varID, GROUP_1, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
			if (var_10336 != 1) {
				g_QQQ = check_named_variable("FSDT_GSX_COUATL_STARTED");
				if (g_QQQ == -1) {
					g_QQQ = register_named_variable("FSDT_GSX_COUATL_STARTED");
				}
				set_named_variable_value(g_QQQ, 1.0);
				var_10336 = 1;
				resetVars();
			}
			break;
		}
		case REQ_SET_VARIABLE: {
			/*label B_ia : //REQ_SET_VARIABLE
		var as : int_ptr = f[276] : int;
		var bs : int = as[0];
		var cs : double_ptr = f[276] : int;
		var ds : double = cs[1];
		set_named_variable_value(bs, ds);
		var es : long_ptr = 0;
		var fs : long = es[1289];
		var gs : int = 0;
		var hs : int = 5;
		var is : int = 1;
		var js : int = 16;
		SimConnect_TransmitClientEvent(fs, gs, hs, gs, is, js);
		goto B_ga;*/
			break;
		}
		case REQ_EXEC_CODE: {
			/*label B_ha : //REQ_EXEC_CODE
		var ks : int = f[278] : int;
		var ls : int = 8;
		var ms : int = ks + ls;
		f[274]:int = ms;
		var ns : int = f[274] : int;
		var os : int = 1088;
		var ps : int = f + os;
		var qs : int = ps;
		var rs : int = 0;
		var ss : int = execute_calculator_code(ns, qs, rs, rs);
		var ts : int = 0;
		var us : int = ss;
		var vs : int = ts;
		var ws : int = us != vs;
		var xs : int = 1;
		var ys : int = ws & xs;
		f[1087]:byte = ys;
		var zs : int = 48;
		var at : int = f + zs;
		var bt : int = at;
		var ct : int = 1032;
		var dt : int = 0;
		memset(bt, dt, ct);
		var et : double = f[136] : double;
		f[6]:double = et;
		var ft : long_ptr = 0;
		var gt : long = ft[1289];
		var ht : int = 48;
		var it : int = f + ht;
		var jt : int = it;
		var kt : int = 3;
		var lt : int = 0;
		var mt : int = 1032;
		SimConnect_SetClientData(gt, kt, kt, lt, lt, mt, jt);
		var nt : long_ptr = 0;
		var ot : long = nt[1289];
		var pt : int = 0;
		var qt : int = 7;
		var rt : int = 1;
		var st : int = 16;
		SimConnect_TransmitClientEvent(ot, pt, qt, pt, rt, st);*/
			break;
		}
		default: {
			/*label B_ga : //Default
		goto B_a;*/
			break;
		}
		}
		break;
	}
	case SIMCONNECT_RECV_ID_QUIT:
	{
		unregister_all_named_vars();

		if (g_hSimConnect != 0) {
			fprintf(stderr, "fsdt-msfs-bridge: Simconnect handle not ready.\n");
			break;
		}
		hr = SimConnect_Close(g_hSimConnect);
		if (hr != 0) {
			fprintf(stderr, "fsdt-msfs-bridge: Could not close SimConnect connection.\n");
			break;
		}
		fprintf(stderr, "fsdt-msfs-bridge: Closed SimConnect connection.\n");
		break;
	}
	default:
		break;
	}
}
