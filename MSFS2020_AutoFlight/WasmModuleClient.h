#pragma once
#include "Structs.h"

class WasmModuleClient
{
public:
        static void Ping(HANDLE hSimConnect, WasmModuleClientData clientData)
        {
            if (hSimConnect == NULL) return;

            SendWasmCmd(hSimConnect, "MF.Ping", clientData);
            DummyCommand(hSimConnect, clientData);
        }

        static void Stop(HANDLE hSimConnect, WasmModuleClientData clientData)
        {
            if (hSimConnect == NULL) return;

            SendWasmCmd(hSimConnect, "MF.SimVars.Clear", clientData);
        }

        static void GetLVarList(HANDLE hSimConnect, WasmModuleClientData clientData)
        {
            if (hSimConnect == NULL) return;

            SendWasmCmd(hSimConnect, "MF.LVars.List", clientData);
            DummyCommand(hSimConnect, clientData);
        }

        static void DummyCommand(HANDLE hSimConnect, WasmModuleClientData clientData)
        {
            if (hSimConnect == NULL) return;

            SendWasmCmd(hSimConnect, "MF.DummyCmd", clientData);
        }

        static void SendWasmCmd(HANDLE hSimConnect, std::string command, WasmModuleClientData clientData)
        {
            if (hSimConnect == NULL) return;
            char str[1024] = {0};
            std::strcpy(str, command.c_str());
            //uint size = command.size();
            void* ptr = (void*)command.c_str();
            SimConnect_SetClientData(
                hSimConnect,
                clientData.AREA_COMMAND_ID,
                (SIMCONNECT_CLIENT_DATA_DEFINITION_ID)clientData.DATA_DEFINITION_ID,
                SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT,
                0,
                MOBIFLIGHT_MESSAGE_SIZE,
                (void*)&str
            );
        }

        static void SetConfig(HANDLE hSimConnect, const char* ConfigName, const char* ConfigValue, WasmModuleClientData clientData)
        {
            if (hSimConnect == NULL) return;

            SendWasmCmd(hSimConnect, std::string("MF.Config." + std::string(ConfigName) + ".Set." + std::string(ConfigValue)).c_str(), clientData);
            DummyCommand(hSimConnect, clientData);
        }

        static void AddAdditionalClient(HANDLE hSimConnect, const char* clientName, WasmModuleClientData clientData)
        {
            WasmModuleClient::DummyCommand(hSimConnect, clientData);
            WasmModuleClient::SendWasmCmd(hSimConnect, std::string("MF.Clients.Add." + std::string(clientName)).c_str(), clientData);
        }

        static void SetSimVar(HANDLE hSimConnect, const char* SimVarCode, WasmModuleClientData clientData)
        {
            WasmModuleClient::SendWasmCmd(hSimConnect, std::string("MF.SimVars.Set." + std::string(SimVarCode)).c_str(), clientData);
            WasmModuleClient::DummyCommand(hSimConnect, clientData);
        }

        static void AddSimVar(HANDLE hSimConnect, const char* SimVarName, WasmModuleClientData clientData, int priority)
        {
            switch (priority) {
            case 0: {
                WasmModuleClient::SendWasmCmd(hSimConnect, std::string("MF.SimVars.Add.0." + std::string(SimVarName)).c_str(), clientData);
                break;
            }
            case 2: {
                WasmModuleClient::SendWasmCmd(hSimConnect, std::string("MF.SimVars.Add.2." + std::string(SimVarName)).c_str(), clientData);
                break;
            }
            default: { // 1
                WasmModuleClient::SendWasmCmd(hSimConnect, std::string("MF.SimVars.Add.1." + std::string(SimVarName)).c_str(), clientData);
                break;
            }
            }            
        }

        static void AddStringSimVar(HANDLE hSimConnect, const char* SimVarName, WasmModuleClientData clientData)
        {
            WasmModuleClient::SendWasmCmd(hSimConnect, std::string("MF.SimVars.AddString." + std::string(SimVarName)).c_str(), clientData);
        }

};

