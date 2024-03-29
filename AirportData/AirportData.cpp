// AirportData.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "AirportData.h"
#include <nlohmann\json.hpp>

enum DATA_REQUEST_ID {
	TRAFFIC_DATA_REQUEST = 0xa0
};

enum DATA_DEFINE_ID {
	DEF_TRAFFIC_DATA = 0xa0
};

struct TrafficData
{
	double Lat;
	double Lon;
	double simOnGround;
};

#pragma pack(push, 1)
struct P3D4FileListHeader
{
	DWORD Magic1;
	DWORD Magic2;
	DWORD AnyData1[26];
	DWORD FileCount;
	//BYTE AnyData2[3];
};

struct MSFSFileListItem
{
	DWORD FileNo;
	char Path[512];
};

struct LayoutItem {
	std::string path;
	unsigned int size;
	unsigned int date;
	std::string filename;
};

struct P3D4FileListData
{
	DWORD ID;
	BYTE AnyData[0x81];
	DWORD Size;
};
#pragma pack(pop)

class AirportData : public IAirportData
{
public:
	AirportData();
	~AirportData();



private:
	std::vector<TrafficData>* PTrafficArray;
	DWORD CountTraffs = 0;
	HANDLE hThread;
	UINT threadID;
	int MaskForFiles = 0x11000;
	int m = 10;
	UINT FillDB();
	static UINT WorkThread(void* Mask);
	int GetProgress();
	int ProgressMin = 0;
	int ProgressMax = 100;
	int Progress = 0;
	int c;
	HRESULT hr;
	AirportInfo FindAirport = {};
	double NearAirport = 0.0;
	//std::wstring FileListPathP3D4 = L"C:\\ProgramData\\Lockheed Martin\\Prepar3D v5\\SceneryIndexes_x64\\";
	std::string FileListPathP3D4 = "C:\\users\\igor8\\AppData\\Roaming\\Microsoft Flight Simulator\\";
	std::string MigoAddBGL = "\\Migo Addons\\PMDG_AUTO\\Scenery\\";
	std::string SearchFilter = "*.dat";
	std::string SearchFilterBGL = "*.bgl";
	HANDLE hSimConnect = NULL;
	void Release();
	IAirport* GetAirport();
	// Тестовое получение ИКАО аэропортов
	HRESULT TEST_GetAirportICAOs();
	void FindReplace(std::string& line, std::string& oldString, std::string& newString);
	Airport* PAirport = NULL;

	std::vector<AirportList>* PAirpotList = NULL;
	std::vector<std::string>* PFileList = NULL;

	std::vector<std::string>* GetFileList(DWORD mask);
	std::vector<AirportList>* GetAirportList(std::vector<std::string>* FileList);


	AirportData::AirportList GetAirportByIcao(std::string icao);

	AirportData::AirportList GetNearAirport(double Lat, double Lon, int index = -1);

	DWORD GetNearTaxiwayPoint(double Lat, double Lon);

	DWORD GetNearTaxiwayPath(double Lat, double Lon, double heading);

	HRESULT ReadFileList(ReadStream* FileList, WriteStream* BGLList, std::vector<std::string>* FList, int Mask, std::vector<LayoutItem>* layoutList, std::string fullPath);

	std::vector<RunwayPaths>* GetRunways();

	double GetAnglePM180(double angle);

	std::vector<int>* GetRunwayStart(double lat, double lon, double direction);

	DATA_RUNWAY GetStartCoord(int index);

	std::vector<std::vector<TPath>> GetPathesToRunways(DWORD start, DWORD heading, double direction);

	std::vector<TPath> GetPath(DWORD start, DWORD heading, double direction);
	std::vector<TPath> GetPath1(DWORD start, DWORD heading, int end);

	void DispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);

	void SetHSim(HANDLE hSim);

	std::vector<DWORD> GetPark(double radius, std::string code);

	std::vector<TPath> GetPathGate(DWORD path, DWORD heading, double radius, TaxiwayParks* park);




};

AirportData::AirportData()
{
	HKEY hKey;
	LPSTR lpEnd;
	DWORD lSize = 512;
	CHAR pData[512];
	PTrafficArray = new std::vector<TrafficData>();
	/*LPCWSTR lpSubKey = L"SOFTWARE\\Lockheed Martin\\Prepar3D v5";

	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, lpSubKey, NULL, KEY_READ, &hKey) == ERROR_SUCCESS)*/
	//FILE* optFile = fopen((FileListPathP3D4 + "UserCfg.opt").c_str(), "r");
	ReadStreamText* optFile = new ReadStreamText(FileListPathP3D4 + "UserCfg.opt");
	std::string line = "";
	int lineNo = 1;
	bool findLine = false;
	while (optFile->ReadLn(&line, lineNo) == 0) {
		if (line.find("InstalledPackagesPath") != -1) {
			findLine = true;
			break;
		}
		lineNo++;
	}
	if (findLine)
	{
		//optFile->Read(&RootSim, lineNo);
		int sPath = line.find(" ") + 2;
		RootSim = line.substr(sPath, line.length() - sPath - 1);
	}
	else
	{
		RootSim = "F:\\Microsoft Flight Simulator\\Packages\\";
	}
	hThread = (HANDLE)_beginthreadex(NULL, 0, &WorkThread, this, 0, &threadID);
	if (hThread)
	{
		Progress = 0;
	}
	else
	{
		Progress = -1;
	}
	c = 100;
}

AirportData::~AirportData()
{
	if (PFileList)
	{
		delete PFileList;
		PFileList = NULL;
	}
	if (PAirpotList)
	{
		delete PAirpotList;
		PAirpotList = NULL;
	}
}

UINT __stdcall AirportData::WorkThread(void* object)
{
	AirportData* ThisObject = (AirportData*)object;
	ThisObject->FillDB();
	ThisObject->Progress = 100;
	return 0;
}


UINT  AirportData::FillDB()
{
	if (PFileList)
	{
		delete PFileList;
		PFileList = NULL;
	}
	if (PAirpotList)
	{
		delete PAirpotList;
		PAirpotList = NULL;
	}
	Progress = 0;
	ProgressMin = 0;
	ProgressMax = 50;
	PFileList = GetFileList(MaskForFiles);
	Progress = 50;
	ProgressMin = 50;
	ProgressMax = 100;
	PAirpotList = GetAirportList(PFileList);
	Progress = 100;
	return 0;
}
int AirportData::GetProgress()
{
	return Progress;
}
std::vector<AirportList>* AirportData::GetAirportList(std::vector<std::string>* FileList)
{
	std::vector<AirportList>* AirpotList = new std::vector<AirportList>();
	WriteStream* APList = new WriteStream(RootSim + "\\APList.csv");
	for (int i = 0; i < FileList->size(); i++)
	{
		Progress = ProgressMin + int(FLOAT((FLOAT)i / (FLOAT)FileList->size()) * FLOAT(ProgressMax - ProgressMin));
		if (i == 2000)
		{
			std::wcout << "1";
		}
		BGLXData* BGLX = new BGLXData(FileList->at(i));
		std::vector<BGLXData::FSXBGLSection>* BGLSectionAirport = BGLX->GetSection(0x03);
		//std::vector<BGLXData::FSXBGLSection>* BGLSectionAirport1 = BGLX->GetSection(0xAB);


		if (BGLSectionAirport)
		{

			std::vector<BGLXData::FSXBGLSubSection>* BGLSubSections = BGLX->GetSubSection(BGLSectionAirport);

			for (int j = 0; j < BGLSubSections->size(); j++)
			{
				DWORD CurrOffset = BGLSubSections->at(j).OffsetRecords;
				for (DWORD k = 0; k < BGLSubSections->at(j).CountRecordsSubSection; k++)
				{
					Airport* airport = new Airport(BGLX, CurrOffset, this);


					if (airport->PAirportInformation != NULL)
					{
						//PMDG_TEST::SIMMATH::DSHEH dsheh = { 73.8, 164.1 ,0.0,airport->DecodeLat(airport->PAirportInformation->Lat), airport->DecodeLon(airport->PAirportInformation->Lon), 0.0 };
						//PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
						/*if ((NearAirport == 0.0) || (NearAirport > dsheh.D))
						{
							NearAirport = dsheh.D;
							FindAirport = *airport->PAirportInformation;

						}*/
						if (airport->PAirportInformation->RunwayCount > 0)
						{
							AirportList AL = {
								airport->PAirportInformation->ID,
								BGLXData::DecodeICAO(airport->PAirportInformation->ICAO),
								FileList->at(i),
								CurrOffset,
								BGLXData::DecodeLat(airport->PAirportInformation->Lat),
								BGLXData::DecodeLon(airport->PAirportInformation->Lon),
								0
							};
							if (AL.Version == 0x03)
							{
								std::wcout << "\n" << AL.ICAO.c_str() << " - Layer: " << AL.Layer << " " << AL.File.c_str() << "\n   ";
							}
							/*	for (int m = 0; m < AirpotList->size(); m++)
								{
									if (AirpotList->at(m).ICAO == AL.ICAO)
									{
										AL.Layer = AirpotList->at(m).Layer + 1;
										//std::wcout << "\n" << AL.ICAO.c_str() << " - Layer: " << AL.Layer << " " <<  AL.File.c_str() << "\n   ";
									}
								}*/

							APList->WriteLn(AL.ICAO + ";" + std::to_string(AL.Version) + ";" + std::to_string(AL.Layer) + ";" + std::to_string(AL.Lon) + ";" + std::to_string(AL.lat) + ";" + AL.File + "\n");
							AirpotList->push_back(AL);
						}
						CurrOffset = CurrOffset + airport->PAirportInformation->SizeOfRecord;
					}

					delete airport;
					airport = NULL;

				}

			}
			if (BGLSectionAirport)
			{
				delete BGLSectionAirport;
				BGLSectionAirport = NULL;
			}
			if (BGLSubSections)
			{
				delete BGLSubSections;
				BGLSubSections = NULL;
			}



		}
		delete BGLX;
		BGLX = NULL;
	}
	if (APList) {
		delete APList;
		APList = NULL;
	}
	PAirpotList = AirpotList;

	std::wcout << "\n" << AirpotList->size() << "\n";
	return AirpotList;
}

AirportList AirportData::GetAirportByIcao(std::string icao) {
	AirportList findAP;
	for (int i = 0; i < PAirpotList->size(); i++) {
		if ((PAirpotList->at(i).ICAO == icao) && (PAirpotList->at(i).File.find("Community") != std::string::npos)) {
			return GetNearAirport(PAirpotList->at(i).lat, PAirpotList->at(i).Lon, i);
		}
	}
	for (int i = 0; i < PAirpotList->size(); i++) {
		if (PAirpotList->at(i).ICAO == icao) {
			return GetNearAirport(PAirpotList->at(i).lat, PAirpotList->at(i).Lon, i);
		}
	}
	return GetNearAirport(0.0, 0.0);
}

AirportList AirportData::GetNearAirport(double Lat, double Lon, int indexx)
{
	PMDG_TEST::SIMMATH::DSHEH dsheh;
	double d = 40000.0;
	int index = 0;
	int startIndex;
	if (indexx >= 0) {
		startIndex = indexx;
	}
	else {
		startIndex = 0;
	}
	for (int i = startIndex; i < PAirpotList->size(); i++)
	{
		dsheh = { PAirpotList->at(i).lat, PAirpotList->at(i).Lon, 0.0, Lat, Lon, 0.0 };
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
		if (dsheh.D < 10.0)
		{
			PAirport = new Airport(PAirpotList->at(i).File, PAirpotList->at(i).FileOffset, this);

			d = dsheh.D;
			index = i;
			//PAirport = new Airport(PAirpotList->at(index).File, PAirpotList->at(index).FileOffset);
			PAirport->GetTaxiwayInformation();
			PAirport->GetRunwayInformation();
			PAirport->GetAirportRect();
			PAirport->GetSIDSTAR();
			std::vector<TaxiwayPaths>* tpaths = PAirport->PTaxiwayPaths;
			std::vector<TaxiwayPoints>* tpoints = PAirport->PTaxiwayPoints;
			std::vector<TaxiwayParks>* tParks = PAirport->PTaxiwayParks;
			//Обновляем размер матрицы ребер
			DWORD m = 0;
			if (tpaths != NULL)
			{
				m = tpaths->size();
			}




			//Обновляем размер матрицы вершин+		tpaths	0x0000000000000000 <NULL>	std::vector<TaxiwayPaths,std::allocator<TaxiwayPaths> > *
			DWORD n = 0;
			if (tpoints != NULL)
			{
				n = tpoints->size();
			}

			DWORD z = 0;
			if (tParks != NULL)
			{
				z = tParks->size();
			}

			for (int i = 0; i < m; i++)
			{
				if ((tpaths->at(i).Type & 0xf) == 0x3)
				{
					//tpaths->at(i).IndexEndPoint = tpaths->at(i).IndexEndPoint + tpoints->size();
					tpaths->at(i).End = tpaths->at(i).End + tpoints->size();
				}
				tpaths->push_back(tpaths->at(i));
				//tpaths->at(tpaths->size() - 1).IndexEndPoint = tpaths->at(i).IndexStartPoint;
				tpaths->at(tpaths->size() - 1).End = tpaths->at(i).IndexStartPoint;
				//tpaths->at(tpaths->size() - 1).IndexStartPoint = tpaths->at(i).IndexEndPoint & 0xfff;
				tpaths->at(tpaths->size() - 1).IndexStartPoint = tpaths->at(i).End;
			}

			for (int i = 0; i < z; i++)
			{
				tpoints->push_back(tpoints->at(0));
				tpoints->at(tpoints->size() - 1).Lat = tParks->at(i).Lat;
				tpoints->at(tpoints->size() - 1).Lon = tParks->at(i).Lon;
			}
			return PAirpotList->at(i);
		}
	}
	//PAirport = new Airport(PAirpotList->at(PAirpotList->size() - 1).File, PAirpotList->at(PAirpotList->size() - 1).FileOffset);
	PAirport = new Airport(PAirpotList->at(index).File, PAirpotList->at(index).FileOffset, this);
	PAirport->GetTaxiwayInformation();
	std::vector<TaxiwayPaths>* tpaths = PAirport->PTaxiwayPaths;
	std::vector<TaxiwayPoints>* tpoints = PAirport->PTaxiwayPoints;
	std::vector<TaxiwayParks>* tParks = PAirport->PTaxiwayParks;
	if ((tpaths != NULL) && (tpoints != NULL) && (tParks != NULL)) {
		for (int i = 0; i < m; i++)
		{
			if ((tpaths->at(i).Type & 0xf) == 0x3)
			{
				//tpaths->at(i).IndexEndPoint = tpaths->at(i).IndexEndPoint + tpoints->size();
				tpaths->at(i).End = tpaths->at(i).End + tpoints->size();
			}
			tpaths->push_back(tpaths->at(i));
			//tpaths->at(tpaths->size() - 1).IndexEndPoint = tpaths->at(i).IndexStartPoint;
			tpaths->at(tpaths->size() - 1).End = tpaths->at(i).IndexStartPoint;
			//tpaths->at(tpaths->size() - 1).IndexStartPoint = tpaths->at(i).IndexEndPoint & 0xfff;
			tpaths->at(tpaths->size() - 1).IndexStartPoint = tpaths->at(i).End;
		}

		for (int i = 0; i < tParks->size(); i++)
		{
			tpoints->push_back(tpoints->at(0));
			tpoints->at(tpoints->size() - 1).Lat = tParks->at(i).Lat;
			tpoints->at(tpoints->size() - 1).Lon = tParks->at(i).Lon;
		}
	}
	return PAirpotList->at(index);
	//PAirpotList
}

DWORD AirportData::GetNearTaxiwayPoint(double Lat, double Lon)
{
	PAirport->GetTaxiwayInformation();
	PMDG_TEST::SIMMATH::DSHEH dsheh;
	double D = 5000.0;
	DWORD CurrIndex = -1;
	if (PAirport->PTaxiwayPoints != NULL)
	{
		for (int i = PAirport->PTaxiwayPoints->size() - 1; i >= 0; i--)
		{
			dsheh = { BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(i).Lat), BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(i).Lon), 0.0, Lat, Lon, 0.0 };
			PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
			if (dsheh.D < D)
			{
				D = dsheh.D;
				CurrIndex = i;
			}
			if (D == 0.0)
			{
				std::cout << i;
			}
		}
		for (int i = PAirport->PTaxiwayParks->size() - 1; i >= 0; i--)
		{
			dsheh = { BGLXData::DecodeLat(PAirport->PTaxiwayParks->at(i).Lat), BGLXData::DecodeLon(PAirport->PTaxiwayParks->at(i).Lon), 0.0, Lat, Lon, 0.0 };
			PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
			if (dsheh.D < D)
			{
				D = dsheh.D;
				CurrIndex = i + PAirport->PTaxiwayPoints->size() + (PAirport->PRunways->size() * 2);
			}
			if (D == 0.0)
			{
				std::cout << i;
			}
		}
	}
	return CurrIndex;
}

DWORD AirportData::GetNearTaxiwayPath(double Lat, double Lon, double heading)
{
	PMDG_TEST::SIMMATH::DSHEH dsheh0;
	PMDG_TEST::SIMMATH::DSHEH dsheh1;
	PMDG_TEST::SIMMATH::DSHEH dsheh2;
	double RA = 5000.0;
	DWORD CurrIndex = -1;
	for (int i = PAirport->PTaxiwayPaths->size() - 1; i >= 0; i--)
	{
		if (PAirport->PTaxiwayPaths->at(i).Unk1 == 0x5555) {
			continue;
		}
		if (((PAirport->PTaxiwayPaths->at(i).Type & 0xf) == 1) || ((PAirport->PTaxiwayPaths->at(i).Type & 0xf) == 2) || ((PAirport->PTaxiwayPaths->at(i).Type & 0xf) == 3) || ((PAirport->PTaxiwayPaths->at(i).Type & 0xf) == 4))
		{
			dsheh0 = {
				BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexStartPoint).Lat),
				BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexStartPoint).Lon), 0.0,
				//BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lat),
				//BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lon), 0.0 };
			BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).End).Lat),
				BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).End).Lon), 0.0
			};
			dsheh2 = { Lat, Lon, 0.0,
				//BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lat),
				//BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lon), 0.0 };
			BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).End).Lat),
				BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).End).Lon), 0.0
			};
		}
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh0);
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh2);
		double ra1;
		ra1 = dsheh2.EH - dsheh0.EH;
		if (ra1 > 180)
		{
			ra1 = ra1 - 360;
		}
		else if (ra1 < -180)
		{
			ra1 = ra1 + 360;
		}

		if (((PAirport->PTaxiwayPaths->at(i).Type & 0xf) == 1) || ((PAirport->PTaxiwayPaths->at(i).Type & 0xf) == 2) || ((PAirport->PTaxiwayPaths->at(i).Type & 0xf) == 4))
		{
			dsheh1 = {
				//BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lat),
				//BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lon), 0.0,
				BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).End).Lat),
				BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).End).Lon), 0.0,
				BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexStartPoint).Lat),
				BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexStartPoint).Lon), 0.0
			};
			dsheh2 = { Lat, Lon, 0.0,
				BGLXData::DecodeLat(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexStartPoint).Lat),
				BGLXData::DecodeLon(PAirport->PTaxiwayPoints->at(PAirport->PTaxiwayPaths->at(i).IndexStartPoint).Lon), 0.0 };
		}
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh1);
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh2);
		double ra2;
		ra2 = dsheh2.EH - dsheh1.EH;
		if (ra2 > 180)
		{
			ra2 = ra2 - 360;
		}
		else if (ra2 < -180)
		{
			ra2 = ra2 + 360;
		}
		double ra3 = dsheh0.EH - heading;
		if (ra3 > 180)
		{
			ra3 = ra3 - 360;
		}
		else if (ra3 < -180)
		{
			ra3 = ra3 + 360;
		}





		double ra = abs(ra1) + abs(ra2) + abs(ra3);
		if (ra < RA)
		{
			RA = ra;
			CurrIndex = i;
		}

	}
	return CurrIndex;
}

void AirportData::Release()
{
	delete this;
}


IAirportData* GetAirportData()
{
	return new AirportData();
}

IAirport* AirportData::GetAirport()
{
	PAirport->GetTaxiwayInformation();
	return PAirport;
}

// Тестовое получение ИКАО аэропортов
HRESULT AirportData::TEST_GetAirportICAOs()
{
	return NOERROR;
}

void AirportData::FindReplace(std::string& line, std::string& oldString, std::string& newString) {
	const size_t oldSize = oldString.length();

	// do nothing if line is shorter than the string to find
	if (oldSize > line.length()) return;

	const size_t newSize = newString.length();
	for (size_t pos = 0; ; pos += newSize) {
		// Locate the substring to replace
		pos = line.find(oldString, pos);
		if (pos == std::string::npos) return;
		if (oldSize == newSize) {
			// if they're same size, use std::string::replace
			line.replace(pos, oldSize, newString);
		}
		else {
			// if not same size, replace by erasing and inserting
			line.erase(pos, oldSize);
			line.insert(pos, newString);
		}
	}
}


std::vector<std::string>* AirportData::GetFileList(DWORD mask)
{
	bool find = false;
	using json = nlohmann::json;
	WIN32_FIND_DATAA FindFileData;
	MSFSFileListItem fileListItem;
	std::vector<LayoutItem> layoutArray;
	//HANDLE  FindHandle = FindFirstFileExA((FileListPathP3D4 + "SceneryIndexes\\*.dat").c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);
	WriteStream* BGLList = new WriteStream(RootSim + "BGLList.csv");

	std::vector<std::string>* FileList = new std::vector<std::string>();
	std::string Path = "";
	json j;
	//if (FindHandle)
	{
		HANDLE  FindDat = FindFirstFileExA((FileListPathP3D4 + "SceneryIndexes\\*.dat").c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);
		//ReadStream* FIndex = new ReadStream(FileListPathP3D4 + "SceneryIndexes\\" + FindFileData.cFileName);
		DWORD countFiles = 0;
		//FIndex->Read(&countFiles, 32, sizeof(countFiles));
		if (FindDat != (HANDLE)-1) {
			find = true;
		}
		while (find) {
			//FIndex->Read(&fileListItem, i * 516 + 36, sizeof(fileListItem));
			Path = FindFileData.cFileName;
			std::string StringDAT = ".dat";
			std::string LayoutJSON = "";
			FindReplace(Path, StringDAT, LayoutJSON);
			std::string layoutFullPath = "";
			std::string fullPath = "";
			layoutFullPath = RootSim + "\\Official\\steam\\" + Path + "\\layout.json";
			fullPath = RootSim + "\\Official\\steam\\" + Path + "\\";
			HANDLE FindHandle = FindFirstFileExA((layoutFullPath).c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);
			if (FindHandle == (HANDLE)-1) {
				layoutFullPath = RootSim + "\\Community\\" + Path + "\\layout.json";
				fullPath = RootSim + "\\Community\\" + Path + "\\";
				FindHandle = FindFirstFileExA((layoutFullPath).c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);
			}
			if (FindHandle != (HANDLE)-1) {
				std::ifstream jfs((layoutFullPath).c_str());
				jfs >> j;
				int i2 = 0, m = 0;
				//layoutArray.push_back({ "",0,0,"" });
				while (j["content"][i2].is_object()) {
					std::string path = j["content"][i2]["path"];

					std::string sstr = path.substr(path.find_last_of("."), path.length() - path.find_last_of("."));
					std::transform(sstr.begin(), sstr.end(), sstr.begin(), ::toupper);
					if (sstr == ".BGL") {
						layoutArray.push_back({ "",0,0,"" });
						layoutArray[m].path = path;
						layoutArray[m].filename = layoutArray[m].path.substr(layoutArray[m].path.find_last_of('/') + 1, layoutArray[m].path.length() - layoutArray[m].path.find_last_of('/') - 1);
						std::transform(layoutArray[m].filename.begin(), layoutArray[m].filename.end(), layoutArray[m].filename.begin(), ::toupper);
						m++;
					}
					i2++;
				}
				jfs.close();
				/*fullPath = fullPath.substr(0, fullPath.find_last_of("\\") + 1);
				std::string fileBase = "Filelist";
				if (fileListItem.FileNo >= 1000) {
					fileBase = fileBase + "0" + std::to_string(fileListItem.FileNo) + ".dat";
				}
				else if (fileListItem.FileNo >= 100) {
					fileBase = fileBase + "00" + std::to_string(fileListItem.FileNo) + ".dat";
				}
				else if (fileListItem.FileNo >= 10) {
					fileBase = fileBase + "000" + std::to_string(fileListItem.FileNo) + ".dat";
				}
				else if (fileListItem.FileNo >= 1) {
					fileBase = fileBase + "0000" + std::to_string(fileListItem.FileNo) + ".dat";
				}
				ReadStream* FList = new ReadStream(FileListPathP3D4 + "SceneryIndexes\\" + fileBase);
				*/
				ReadFileList(NULL, BGLList, FileList, mask, &layoutArray, fullPath);
				layoutArray.clear();
				find = FindNextFileA(FindDat, &FindFileData);
				//delete FList;
				//FList = NULL;
			}
		}
		//delete FIndex;
		//FIndex = NULL;
	}



	/*FindHandle = FindFirstFileExA((FileListPathP3D4 + "SceneryIndexes\\" + SearchFilter).c_str(), FindExInfoStandard, &FindFileData, FindExSearchNameMatch, NULL, 0);


	if (FindHandle)
	{

		ReadStream* FList = new ReadStream(FileListPathP3D4 + "SceneryIndexes\\" + FindFileData.cFileName);
		ReadFileList(FList, BGLList, FileList, mask);
		delete FList;
		FList = NULL;

	}
	else
	{
		std::cout << "Отсутствует кэш";

	}
	int i = 0;
	while (FindNextFileA(FindHandle, &FindFileData))
	{
		Progress = ProgressMin + int(FLOAT((FLOAT)i / 161.0)*FLOAT(ProgressMax - ProgressMin));
		ReadStream* FList = new ReadStream(FileListPathP3D4 + "SceneryIndexes\\" + FindFileData.cFileName);
		ReadFileList(FList, BGLList, FileList, mask);
		delete FList;
		FList = NULL;
		i++;
	}*/
	WIN32_FIND_DATAA FindBGLData;
	HANDLE  FindHandleBGL = FindFirstFileExA((RootSim + MigoAddBGL + SearchFilterBGL).c_str(), FindExInfoStandard, &FindBGLData, FindExSearchNameMatch, NULL, 0);
	if (FindHandleBGL)
	{
		FileList->push_back((RootSim + MigoAddBGL + FindBGLData.cFileName).c_str());
		while (FindNextFileA(FindHandleBGL, &FindBGLData))
		{
			FileList->push_back((RootSim + MigoAddBGL + FindBGLData.cFileName).c_str());
		}
	}
	if (FileList->size() == 0)
	{
		delete FileList;
		FileList = NULL;
		return NULL;
	}
	delete BGLList;
	BGLList = NULL;
	return FileList;
}


HRESULT AirportData::ReadFileList(ReadStream* FileList, WriteStream* BGLList, std::vector<std::string>* FList, int Mask, std::vector<LayoutItem>* layoutList, std::string fullPath)
{
	for (int j = 0; j < layoutList->size(); j++) {



		FList->push_back(fullPath + layoutList->at(j).path);
	}
	return 0;

}

std::vector<RunwayPaths>* AirportData::GetRunways()
{
	PAirport->GetTaxiwayInformation();
	PAirport->GetRunwayInformation();
	struct PointInRunway
	{
		WORD RunwayPoint;
		WORD Runway;
	} RP;
	RunwayPaths TRPh;
	if (PAirport->RPth == NULL)
	{
		PAirport->RPth = new std::vector<RunwayPaths>();
	}
	else
	{
		PAirport->RPth->clear();
	}
	std::vector<PointInRunway> RunwayPoints;
	std::vector<WORD> Runways;
	//std::vector<RunwayPaths> RPth;

	WORD RunwayStart = 0;
	WORD RunwayEnd = 0;
	std::string RunwayDesign = "";
	/*for (int i = 0; i < PAirport->PTaxiwayPaths->size(); i++)
	{
		if (((PAirport->PTaxiwayPaths->at(i).Type) & (0xf)) == 2)
		{
			RP.Runway = (PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xf000) + (PAirport->PTaxiwayPaths->at(i).TaxiNameIndex);
			//RP.RunwayPoint = PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0x0fff;
			RP.RunwayPoint = PAirport->PTaxiwayPaths->at(i).End; // For MSFS
			//RunwayPoints.push_back(RP);
			RunwayPoints.push_back(RP);
			RP.Runway = (PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xf000) + (PAirport->PTaxiwayPaths->at(i).TaxiNameIndex);
			RP.RunwayPoint = PAirport->PTaxiwayPaths->at(i).IndexStartPoint;
			RunwayPoints.push_back(RP);
			bool f = false;
			for (int j = 0; j < Runways.size(); j++)
			{
				if (Runways.at(j) == ((PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xf000) + (PAirport->PTaxiwayPaths->at(i).TaxiNameIndex)))
				{
					f = true;
					break;
				}
			}
			if (!f)
			{
				Runways.push_back((PAirport->PTaxiwayPaths->at(i).IndexEndPoint & 0xf000) + (PAirport->PTaxiwayPaths->at(i).TaxiNameIndex));
			}
		}
	}*/
	for (int i = 0; i < PAirport->PRunways->size(); i++) {
		Runways.push_back(PAirport->PRunways->at(i).PrimaryDesignator * 0x1000 + PAirport->PRunways->at(i).PrimaryNumber);
	}
	for (int i = 0; i < PAirport->PPointsRunway->size(); i++) {
		RP.Runway = PAirport->PPointsRunway->at(i).Run;
		RP.RunwayPoint = PAirport->PPointsRunway->at(i).TaxiwayPoint;

		RunwayPoints.push_back(RP);
	}
	for (int i = 0; i < Runways.size(); i++)
	{
		for (int j = 0; j < RunwayPoints.size(); j = j + 2)
		{
			if (RunwayPoints.at(j).Runway == Runways.at(i))
			{
				int k = 0;
				for (k = 0; k < PAirport->RPth->size(); k++)
				{
					if (PAirport->RPth->at(k).Runway == Runways.at(i))
					{
						break;
					}
				}
				if (k < PAirport->RPth->size())
				{
					PAirport->RPth->at(k).Path.push_back(RunwayPoints.at(j + 1).RunwayPoint);
					PAirport->RPth->at(k).Path.push_back(RunwayPoints.at(j).RunwayPoint);
				}
				else
				{
					TRPh.Path.clear();
					TRPh.Path.push_back(RunwayPoints.at(j + 1).RunwayPoint);
					TRPh.Path.push_back(RunwayPoints.at(j).RunwayPoint);
					TRPh.Runway = Runways.at(i);
					PAirport->RPth->push_back(TRPh);
				}

			}
		}
		if (PAirport->RPth->size() <= i) {
			TRPh.Path.clear();
			TRPh.Path.push_back(-1);
			TRPh.Path.push_back(-1);
			TRPh.Runway = Runways.at(i);
			PAirport->RPth->push_back(TRPh);
		}
	}
	for (int i = 0; i < PAirport->RPth->size(); i++)
	{
		if (PAirport->RPth->at(i).Path.at(0) == 65535) {
			continue;
		}
		int counter = 0;
		for (int j = 0; j < PAirport->RPth->at(i).Path.size(); j++)
		{

			if ((j + 1) != PAirport->RPth->at(i).Path.size())
			{
				int k = 0;
				for (k = 0; k < PAirport->RPth->at(i).Path.size(); k++)
				{
					if ((PAirport->RPth->at(i).Path.at(j) == PAirport->RPth->at(i).Path.at(k)) && (j != k))
					{

						break;
					}
				}

				if (k == PAirport->RPth->at(i).Path.size())
				{
					counter++;
					//PAirport->RPth->at(i).Path.push_back(PAirport->RPth->at(i).Path.at(j));
				}
				if (counter == 2)
				{
					break;
				}

			}
			else if (counter < 2)
			{
				//PAirport->RPth->at(i).Path.push_back(PAirport->RPth->at(i).Path.at(j));
			}
		}
		PMDG_TEST::SIMMATH::DSHEH dsheh1 = {
			PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lat),
			PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lon), 0.0,
			PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lat),
			PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lon), 0.0
		};
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh1);
		if (abs(GetAnglePM180(dsheh1.EH - GetAnglePM180((PAirport->RPth->at(i).Runway & 0xff) * 10))) > 100.0)
		{
			//WORD p1, p2;
			
			WORD p1 = PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2);
			PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2) = PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1);
			PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1) = p1;
		}
	}
	PMDG_TEST::SIMMATH::PreparePaths(PAirport->PTaxiwayPoints, PAirport->PTaxiwayPaths, PAirport->PTaxiwayNames, PAirport->PTaxiwayParks, PAirport->RPth);

	return PAirport->RPth;


}
double AirportData::GetAnglePM180(double angle)
{
	double Tangle;
	Tangle = angle;
	while (Tangle > 180)
	{
		Tangle = Tangle - 360;
	}
	while (Tangle < -180)
	{
		Tangle = Tangle + 360;
	}
	return Tangle;
}

std::vector<int>* AirportData::GetRunwayStart(double lat, double lon, double direction)
{

	GetRunways();
	int a = 0;
	if (PAirport->Starts == NULL)
	{
		PAirport->Starts = new std::vector<int>;
	}
	else
	{
		PAirport->Starts->clear();
	}

	std::string ds[] = { "", "L", "R", "C", "W", "A", "B" };
	while (PAirport->Starts->size() == 0)
	{
		for (int i = 0; i < PAirport->RPth->size(); i++)
		{
			std::string R1;
			if (((PAirport->RPth->at(i).Runway & 0xff) < 10))
			{
				R1 = "0" + std::to_string(PAirport->RPth->at(i).Runway & 0xff) + ds[PAirport->RPth->at(i).Runway >> 12];
			}
			else
			{
				R1 = std::to_string(PAirport->RPth->at(i).Runway & 0xff) + ds[PAirport->RPth->at(i).Runway >> 12];
			}
			PMDG_TEST::SIMMATH::DSHEH dsheh1 = { 0 };
			if (PAirport->RPth->at(i).Path.at(0) != 65535) {
				dsheh1 = {
					PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lat),
					PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lon), 0.0,
					PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lat),
					PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lon), 0.0
				};
				PMDG_TEST::SIMMATH::DOrtoKM(&dsheh1);
			} 

			if (abs(GetAnglePM180(dsheh1.EH - GetAnglePM180((PAirport->RPth->at(i).Runway & 0xff) * 10))) > 100.0)
			{
				if (PAirport->RPth->at(i).Path.at(0) != 65535) {
					dsheh1 = {
						PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lat),
						PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lon), 0.0,
						PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lat),
						PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lon), 0.0
					};
					PMDG_TEST::SIMMATH::DOrtoKM(&dsheh1);
				}
			}
			double RAngle = dsheh1.EH - direction;
			if (RAngle > 180)
			{
				RAngle = RAngle - 360;
			}
			else if (RAngle < -180)
			{
				RAngle = RAngle + 360;
			}
			if (abs(RAngle) < (30.0 + a))
			{
				if (dsheh1.D > 1.0)
				{
					PAirport->Starts->push_back(i + 1);
				}
			}
			std::string R2 = "";
			if ((PAirport->RPth->at(i).Runway & 0xff) <= 18)
			{
				R2 = std::to_string((PAirport->RPth->at(i).Runway & 0xff) + 18);
			}
			else
			{
				if (((PAirport->RPth->at(i).Runway & 0xff) - 18) < 10)
				{
					R2 = "0" + std::to_string((PAirport->RPth->at(i).Runway & 0xff) - 18);
				}
				else
				{
					R2 = std::to_string((PAirport->RPth->at(i).Runway & 0xff) - 18);
				}
			}
			switch (PAirport->RPth->at(i).Runway >> 12)
			{
			case 1:
			{
				R2 = R2 + ds[2];
				break;
			}
			case 2:
			{
				R2 = R2 + ds[1];
				break;
			}
			case 5:
			{
				R2 = R2 + ds[6];
				break;
			}
			case 6:
			{
				R2 = R2 + ds[5];
				break;
			}
			default:
			{
				R2 = R2 + ds[PAirport->RPth->at(i).Runway >> 12];
				break;
			}
			}
			PMDG_TEST::SIMMATH::DSHEH dsheh2 = { 0 };
			if (PAirport->RPth->at(i).Path.at(0) != 65535) {
				dsheh2 = {
					PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lat),
					PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lon), 0.0,
					PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lat),
					PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lon), 0.0
				};
				PMDG_TEST::SIMMATH::DOrtoKM(&dsheh2);
			}
			if (abs(GetAnglePM180(dsheh2.EH - GetAnglePM180(((PAirport->RPth->at(i).Runway & 0xff) - 18) * 10))) > 100.0)
			{
				if (PAirport->RPth->at(i).Path.at(0) != 65535) {
					dsheh2 = {
						PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lat),
						PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 1))).Lon), 0.0,
						PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lat),
						PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(i).Path.at(PAirport->RPth->at(i).Path.size() - 2))).Lon), 0.0
					};
					PMDG_TEST::SIMMATH::DOrtoKM(&dsheh2);
				}
			}
			RAngle = dsheh2.EH - direction;
			if (RAngle > 180)
			{
				RAngle = RAngle - 360;
			}
			else if (RAngle < -180)
			{
				RAngle = RAngle + 360;
			}
			if (abs(RAngle) < (30.0 + a))
			{
				if (dsheh2.D > 2.0)
				{
					PAirport->Starts->push_back(0 - (i + 1));
				}
			}
			PAirport->RPth->at(i).RunwayName1 = R1;
			PAirport->RPth->at(i).RunwayName2 = R2;
		}
		a = a + 10;
	}
	double d = 800000.0;
	int index = -1;
	for (int i = 0; i < PAirport->Starts->size(); i++)
	{
		PMDG_TEST::SIMMATH::DSHEH dsheh;
		if (PAirport->Starts->at(i) < 0)
		{
			/*	if (start >= PAirport->PTaxiwayPoints->size())
				{
					dsheh.Slla.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lat);
					dsheh.Slla.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lon);
				}
				else
				{
					dsheh.Slla.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at(start).Lat);
					dsheh.Slla.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at(start).Lon);
				}
				dsheh.Ella.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((RPth.at(abs(Starts.at(i)) - 1).Path.at(RPth.at(abs(Starts.at(i)) - 1).Path.size() - 2))).Lat);
				dsheh.Ella.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((RPth.at(abs(Starts.at(i)) - 1).Path.at(RPth.at(abs(Starts.at(i)) - 1).Path.size() - 2))).Lon);

				dsheh.Slla.Latitude = lat;
				dsheh.Slla.Longitude = lon;
				dsheh.Ella.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.at(PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.size() - 2))).Lat);
				dsheh.Ella.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.at(PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.size() - 2))).Lon);
			}
			else
			{
				//dsheh.Slla.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at(start).Lat);
				//dsheh.Slla.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at(start).Lon);
				/*if (start >= PAirport->PTaxiwayPoints->size())
				{
					dsheh.Slla.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lat);
					dsheh.Slla.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lon);
				}
				else
				{
					dsheh.Slla.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at(start).Lat);
					dsheh.Slla.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at(start).Lon);
				}
				dsheh.Ella.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((RPth.at(abs(Starts.at(i)) - 1).Path.at(RPth.at(abs(Starts.at(i)) - 1).Path.size() - 1))).Lat);
				dsheh.Ella.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((RPth.at(abs(Starts.at(i)) - 1).Path.at(RPth.at(abs(Starts.at(i)) - 1).Path.size() - 1))).Lon);
				*/
			dsheh.Slla.Latitude = lat;
			dsheh.Slla.Longitude = lon;
			dsheh.Ella.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.at(PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.size() - 1))).Lat);
			dsheh.Ella.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at((PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.at(PAirport->RPth->at(abs(PAirport->Starts->at(i)) - 1).Path.size() - 1))).Lon);
		}
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
		if (dsheh.D < d)
		{
			index = i;
		}

	}
	return PAirport->Starts;


}

DATA_RUNWAY AirportData::GetStartCoord(int index)
{
	DATA_RUNWAY latlonhed;
	if (index < 0) {
		latlonhed = PAirport->POneWayRunways->at((abs(index)-1) * 2+1);
	}
	else {
		latlonhed = PAirport->POneWayRunways->at((index - 1) * 2);
	}
	return latlonhed;
	if (index < 0)
	{
		WORD s = PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 2);
		WORD e = PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 1);
		latlonhed.sLatitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at(s).Lat);
		latlonhed.sLongitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at(s).Lon);
		latlonhed.eLatitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at(e).Lat);
		latlonhed.eLongitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at(e).Lon);
		PMDG_TEST::SIMMATH::DSHEH dsheh = {
			latlonhed.sLatitude,
			latlonhed.sLongitude, 0.0,
			latlonhed.eLatitude,
			latlonhed.eLongitude, 0.0 };
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
		latlonhed.sHeading = PMDG_TEST::SIMMATH::GetAngle360(dsheh.SH);
		latlonhed.eHeading = PMDG_TEST::SIMMATH::GetAngle360(latlonhed.sHeading - 180.0);
		latlonhed.Name = PAirport->RPth->at(abs(index) - 1).RunwayName2;
	}
	else
	{
		WORD s = PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 1);
		WORD e = PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 2);
		latlonhed.sLatitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at(s).Lat);
		latlonhed.sLongitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at(s).Lon);
		latlonhed.eLatitude = PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayPoints->at(e).Lat);
		latlonhed.eLongitude = PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayPoints->at(e).Lon);
		PMDG_TEST::SIMMATH::DSHEH dsheh = {
			latlonhed.sLatitude,
			latlonhed.sLongitude, 0.0,
			latlonhed.eLatitude,
			latlonhed.eLongitude, 0.0 };
		PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
		latlonhed.sHeading = PMDG_TEST::SIMMATH::GetAngle360(dsheh.SH);
		latlonhed.eHeading = PMDG_TEST::SIMMATH::GetAngle360(latlonhed.sHeading - 180.0);
		latlonhed.Name = PAirport->RPth->at(abs(index) - 1).RunwayName1;
	}
	return latlonhed;
}

std::vector<TPath> AirportData::GetPath1(DWORD start, DWORD heading, int index)
{
	/*int index;
	if (start >= PAirport->PTaxiwayPoints->size())
	{
		index = GetRunwayStart(PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lat, PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lon, direction)->at(0);
	}
	else
	{
		index = GetRunwayStart(PAirport->PTaxiwayPoints->at(start).Lat, PAirport->PTaxiwayPoints->at(start).Lon, direction)->at(0);
	}*/
	if (PAirport->RPth->at(abs(index) - 1).Path.at(0) == 65535) {
		ReturnPath = new  std::vector<TPath>;
		return *ReturnPath;
	}

	if (d != NULL)
	{
		delete d;
		d = NULL;
	}
	INT64* p = new INT64;
	//if (PAirport->Starts->at(index) < 0)
	if (index < 0)
	{

		ReturnPath = new  std::vector<TPath>(PMDG_TEST::SIMMATH::solve(PAirport->PTaxiwayPoints,
			PAirport->PTaxiwayPaths,
			PAirport->PTaxiwayNames,
			PAirport->PTaxiwayParks, start,
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 2),
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 1),
			PAirport->RPth->at(abs(index) - 1).RunwayName2, heading, p));
	}
	else
	{
		ReturnPath = new  std::vector<TPath>(PMDG_TEST::SIMMATH::solve(PAirport->PTaxiwayPoints,
			PAirport->PTaxiwayPaths,
			PAirport->PTaxiwayNames,
			PAirport->PTaxiwayParks, start,
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 1),
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 2),
			PAirport->RPth->at(abs(index) - 1).RunwayName1, heading, p));
	}
	d = (std::vector<double>*) * p;
	return *ReturnPath;

}

std::vector<TPath> AirportData::GetPath(DWORD start, DWORD heading, double direction)
{
	int index;
	if (start >= PAirport->PTaxiwayPoints->size())
	{
		index = GetRunwayStart(PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lat, PAirport->PTaxiwayParks->at(start - PAirport->PTaxiwayPoints->size()).Lon, direction)->at(0);
	}
	else
	{
		index = GetRunwayStart(PAirport->PTaxiwayPoints->at(start).Lat, PAirport->PTaxiwayPoints->at(start).Lon, direction)->at(0);
	}


	if (d != NULL)
	{
		delete d;
		d = NULL;
	}
	INT64* p = new INT64;
	//if (PAirport->Starts->at(index) < 0)
	if (index < 0)
	{

		ReturnPath = new  std::vector<TPath>(PMDG_TEST::SIMMATH::solve(PAirport->PTaxiwayPoints,
			PAirport->PTaxiwayPaths,
			PAirport->PTaxiwayNames,
			PAirport->PTaxiwayParks, start,
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 2),
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 1),
			PAirport->RPth->at(abs(index) - 1).RunwayName2, heading, p));
	}
	else
	{
		ReturnPath = new  std::vector<TPath>(PMDG_TEST::SIMMATH::solve(PAirport->PTaxiwayPoints,
			PAirport->PTaxiwayPaths,
			PAirport->PTaxiwayNames,
			PAirport->PTaxiwayParks, start,
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 1),
			PAirport->RPth->at(abs(index) - 1).Path.at(PAirport->RPth->at(abs(index) - 1).Path.size() - 2),
			PAirport->RPth->at(abs(index) - 1).RunwayName1, heading, p));
	}
	d = (std::vector<double>*) * p;
	return *ReturnPath;

}

void CALLBACK AirportData::DispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{

	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
	{
		SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;

		switch (pObjData->dwRequestID)
		{
		case TRAFFIC_DATA_REQUEST:
		{
			if (CountTraffs == 0)
			{
				PTrafficArray->resize(pObjData->dwoutof);
				CountTraffs = pObjData->dwoutof;
				if (CountTraffs == 1) {
					hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, TRAFFIC_DATA_REQUEST, DEF_TRAFFIC_DATA, 10000, SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT);
				}
			}
			else
			{
				PTrafficArray->at(pObjData->dwentrynumber - 1) = *(TrafficData*)(&(pObjData->dwData));
				CountTraffs--;
				if (CountTraffs <= 1)
				{
					hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, TRAFFIC_DATA_REQUEST, DEF_TRAFFIC_DATA, 10000, SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT);
					CountTraffs = 0;
				}
			}
			if (pObjData->dwoutof == 0) {
				hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, TRAFFIC_DATA_REQUEST, DEF_TRAFFIC_DATA, 10000, SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT);
				CountTraffs = 0;
			}
			break;
		}


		default:
		{
			break;
		}
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

void AirportData::SetHSim(HANDLE hSim)
{
	hSimConnect = hSim;
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEF_TRAFFIC_DATA, "Plane Latitude", "degrees");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEF_TRAFFIC_DATA, "Plane Longitude", "degrees");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEF_TRAFFIC_DATA, "SIM ON GROUND", "bool");
	hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, TRAFFIC_DATA_REQUEST, DEF_TRAFFIC_DATA, 1000, SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT);
}

std::vector<DWORD>  AirportData::GetPark(double radius, std::string code)
{
	for (int i = 0; i < 1000; i++)
	{
		if (PTrafficArray->size() != 0)
		{
			break;
		}
		Sleep(100);
	}
	std::vector<DWORD> Parks;
	std::vector<DWORD> ParkRel = { 0x9, 0x3, 0xa, 0x4, 0x5, 0x6, 0x1, 0x2, 0xb, 0x8, 0x7 };
	std::vector<DWORD> FindCodes;
	for (int i = 0; i < PAirport->PTaxiwayParks->size(); i++)
	{
		for (int k = 0; k < PAirport->PTaxiwayParks->at(i).PTaxiwayParksCodes->size(); k++)
		{
			if (PAirport->PTaxiwayParks->at(i).PTaxiwayParksCodes->at(k).AirlineDesignator == code)
			{
				FindCodes.push_back(i);
			}
		}
	}
	for (int n = 0; n < 2; n++)
	{
		if (FindCodes.size() == 0)
		{
			for (int j = 0; j < ParkRel.size(); j++)
			{
				for (int i = 0; i < PAirport->PTaxiwayParks->size(); i++)
				{

					if (((PAirport->PTaxiwayParks->at(i).TaxiParkInfo & 0xf00) >> 8) == ParkRel.at(j) && (radius <= PAirport->PTaxiwayParks->at(i).Radius))
					{
						bool Busy = false;
						for (int m = 0; m < PTrafficArray->size(); m++)
						{
							if (PTrafficArray->at(m).simOnGround > 0.0)
							{
								PMDG_TEST::SIMMATH::DSHEH dsheh = {
									PTrafficArray->at(m).Lat,
									PTrafficArray->at(m).Lon, 0.0,
									PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayParks->at(i).Lat),
									PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayParks->at(i).Lon), 0.0 };
								PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
								if (dsheh.D < (PAirport->PTaxiwayParks->at(i).Radius * 2.0 / 1000.0))
								{
									Busy = true;
									break;
								}
							}
						}
						if (!Busy)
						{
							Parks.push_back(i);
							//return i;
						}
					}
				}
			}
		}
		else
		{
			for (int j = 0; j < ParkRel.size(); j++)
			{
				for (int i = 0; i < FindCodes.size(); i++)
				{
					double minD = 100000000.0;
					bool Busy = false;
					if (((PAirport->PTaxiwayParks->at(FindCodes[i]).TaxiParkInfo & 0xf00) >> 8) == ParkRel.at(j) && (radius <= PAirport->PTaxiwayParks->at(FindCodes[i]).Radius))
					{
						for (int m = 0; m < PTrafficArray->size(); m++)
						{
							if (PTrafficArray->at(m).simOnGround > 0.0)
							{
								PMDG_TEST::SIMMATH::DSHEH dsheh = {
									PTrafficArray->at(m).Lat,
									PTrafficArray->at(m).Lon, 0.0,
									PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayParks->at(FindCodes[i]).Lat),
									PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayParks->at(FindCodes[i]).Lon), 0.0 };
								PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
								if (minD > dsheh.D)
								{
									minD = dsheh.D;
								}

								if (dsheh.D < (PAirport->PTaxiwayParks->at(FindCodes[i]).Radius * 2.0 / 1000.0))
								{
									Busy = true;
									break;
								}
							}

						}
						if (!Busy)
						{
							Parks.push_back(FindCodes[i]);
							//return FindCodes[i];
						}

					}
				}
				if (Parks.size() > 0) {
					return Parks;
				}
			}
		}
		if (Parks.size() > 0) {
			return Parks;
		}
		for (int i = 0; i < PAirport->PTaxiwayParks->size(); i++)
		{
			double minD = 100000000.0;
			bool Busy = false;
			if (radius <= PAirport->PTaxiwayParks->at(i).Radius) {
				for (int m = 0; m < PTrafficArray->size(); m++)
				{
					if (PTrafficArray->at(m).simOnGround > 0.0)
					{
						PMDG_TEST::SIMMATH::DSHEH dsheh = {
							PTrafficArray->at(m).Lat,
							PTrafficArray->at(m).Lon, 0.0,
							PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayParks->at(i).Lat),
							PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayParks->at(i).Lon), 0.0 };
						PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
						if (minD > dsheh.D)
						{
							minD = dsheh.D;
						}

						if (dsheh.D < (PAirport->PTaxiwayParks->at(i).Radius * 2.0 / 1000.0))
						{
							Busy = true;
							break;
						}
					}

				}
				if (!Busy)
				{
					Parks.push_back(i);
					//return i;
				}
			}
		}
		if (Parks.size() > 0) {
			return Parks;
		}
		for (int i = 0; i < PAirport->PTaxiwayParks->size(); i++)
		{
			double minD = 100000000.0;
			bool Busy = false;

			for (int m = 0; m < PTrafficArray->size(); m++)
			{
				if (PTrafficArray->at(m).simOnGround > 0.0)
				{
					PMDG_TEST::SIMMATH::DSHEH dsheh = {
						PTrafficArray->at(m).Lat,
						PTrafficArray->at(m).Lon, 0.0,
						PMDG_TEST::SIMMATH::DecodeLat(PAirport->PTaxiwayParks->at(i).Lat),
						PMDG_TEST::SIMMATH::DecodeLon(PAirport->PTaxiwayParks->at(i).Lon), 0.0 };
					PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
					if (minD > dsheh.D)
					{
						minD = dsheh.D;
					}

					if (dsheh.D < (PAirport->PTaxiwayParks->at(i).Radius * 2.0 / 1000.0))
					{
						Busy = true;
						break;
					}
				}

			}
			if (!Busy)
			{
				Parks.push_back(i);
				//return i;
			}

		}
		FindCodes.clear();
	}
	return Parks;
}

std::vector<TPath> AirportData::GetPathGate(DWORD path, DWORD heading, double radius, TaxiwayParks* park)
{
	std::vector<DWORD> Park = GetPark(radius, "SBI");
	DWORD FindPark = 0;
	if (Park.size() == 0) {
		*park = PAirport->PTaxiwayParks->at(0);
	}
	else {
		srand(time(0));
		FindPark = Park.at(rand() % Park.size());
		*park = PAirport->PTaxiwayParks->at(FindPark);
	}
	//DWORD Park = 15;
	std::vector<TPath> ReturnPath;

	ReturnPath = PMDG_TEST::SIMMATH::solveGate(PAirport->PTaxiwayPoints,
		PAirport->PTaxiwayPaths,
		PAirport->PTaxiwayNames,
		PAirport->PTaxiwayParks,
		//(PAirport->PTaxiwayPaths->at(path).IndexEndPoint & 0xfff),
		(PAirport->PTaxiwayPaths->at(path).End),
		FindPark, heading);


	return ReturnPath;

}