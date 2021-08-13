/*#include "PMDG_NGX.h"
#include "Route.h"
#include <WinError.h>

namespace PMDG_TEST
{
	
	CRoute::CRoute() {
		HKEY key;
		DWORD  Disposition;
		DWORD Type, Size;
		HRESULT hr = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Lockheed Martin\\Prepar3D v5", 0,NULL,0, KEY_ALL_ACCESS,NULL, &key, &Disposition);
		if (hr == ERROR_SUCCESS) {
			Size = 256;
			wchar_t path[256];
			hr = RegQueryValueExW(key, L"SetupPath", NULL, NULL, (LPBYTE)path, &Size);
			pathSim = gcnew String(path);
			RegCloseKey(key);
		}
	}




	CRoute::~CRoute(){}




	SIMMATH::DSHEH *CRoute::GetSID(char* airport, char* sid) {
		char buff[250]; // буфер промежуточного хранения считываемого из файла текста
		OpenSSFile(airport);
		array<String^>^ FindSid = GetSectionByName("sid", gcnew String(sid));
		for (int i = 0; i < FindSid->Length; i++) {}
		SIMMATH::DSHEH* SIDs = new SIMMATH::DSHEH[50];
		return SIDs;
	}




	void CRoute::OpenSSFile(char* airport) {
		String^ filename = gcnew String(pathSim + "\\PMDG\\SIDSTARS\\" + gcnew String(airport) + ".txt");
		try {
			fsidstar = File::ReadAllText(filename);
		}
		catch (Exception^ e) {
      if (dynamic_cast<FileNotFoundException^>(e)) {
        Console::WriteLine("file " + filename + " not found");
      }
		}
	}




	array<String^>^ CRoute::GetSectionByName(String^ section, String^ name) {
		String^ STARTSECTION;
		String^ ENDSECTION;
		if ((section->ToUpper() == "RNW") ||
			(section->ToUpper() == "SID") ||
			(section->ToUpper() == "STAR")) {
			STARTSECTION = section->ToUpper()+"S";
			ENDSECTION = "END" + STARTSECTION;
		}
		else if ((section->ToUpper() == "FIX") ||
			(section->ToUpper() == "APROACH") ||
			(section->ToUpper() == "GATE")) {
			STARTSECTION = section->ToUpper() + "ES";
			ENDSECTION = "END" + STARTSECTION;
		}
		int STARTINDEX = fsidstar->IndexOf(STARTSECTION);
		int ENDINDEX = fsidstar->IndexOf(ENDSECTION);
		String^ FOUNDSECTION;
		if ((STARTINDEX > -1) && (ENDINDEX > -1)) {
			String^ MEMBERS = fsidstar->Substring(STARTINDEX + STARTSECTION->Length, ENDINDEX - STARTINDEX - STARTSECTION->Length);
			int FINDINDEX = MEMBERS->IndexOf(section->ToUpper() +" "+ name->ToUpper());
			if (FINDINDEX > -1) {
				int ENDFINDINDEX = MEMBERS->IndexOf(section->ToUpper(), FINDINDEX + section->ToUpper()->Length);
				if (ENDFINDINDEX > -1) {
					FOUNDSECTION = MEMBERS->Substring(FINDINDEX, ENDFINDINDEX - FINDINDEX);
				}
				else {
					FOUNDSECTION = MEMBERS->Substring(FINDINDEX);
				}
				return  FOUNDSECTION->Split(gcnew array<Char> { ' ', '\n' });
			}
		}
		return nullptr;
	}
}
*/





