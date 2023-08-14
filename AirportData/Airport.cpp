#include "stdafx.h"
#include "Airport.h"
//#include "../../../../../../Program%20Files%20(x86)/Windows%20Kits/10/Include/10.0.17763.0/ucrt/stdio.h"
//#include "sidstar.h"
//#include "sidstar.h"
//#include <ssfunc.h>



extern FILE* yyin, * yyout;
extern int ScanToken();
extern int yyparse();
extern sfix pfix;
extern sroute proute;
extern srnwsid prnwsid;
extern ssid psid;
extern ssidstar sidstar2;
extern int yydebug;
extern SidStar* gSidStar;



Airport::Airport(BGLXData* BGLX, DWORD AirportDataOffset, IAirportData* AirportData)
{
	this->BGLX = BGLX;
	this->AirportData = AirportData;
	this->AirportDataOffset = AirportDataOffset;
	GetAirportData(false);
	mode = false;
}

Airport::Airport(std::string BGLFile, DWORD AirportDataOffset, IAirportData* AirportData)
{
	this->AirportData = AirportData;
	BGLX = new BGLXData(BGLFile);
	this->AirportDataOffset = AirportDataOffset;
	GetAirportData(true);
	GetAirportData(false);
	mode = true;
}

HRESULT Airport::GetAirportData(bool full)
{
	if (full)
	{
		Records = BGLX->GetAirportRecordHierarhy(AirportDataOffset);
	}
	else
	{
		PAirportInformation = new AirportInfo;
		PAddAirportInformation = new AddAirportInfo;

		BGLX->BGLXFile->Read(PAirportInformation, AirportDataOffset, sizeof(*PAirportInformation));
		if ((PAirportInformation->ID != 0x03) && (PAirportInformation->ID != 0x3C) && (PAirportInformation->ID != 0xAB) && (PAirportInformation->ID != 0x56))
		{
			PAirportInformation = NULL;
			PAddAirportInformation = NULL;
			return E_NOTIMPL;
		}
		
	}
	return NOERROR;
}

HRESULT Airport::GetAirportRect() {
	double MinLat = 360.0, MinLon = 360.0, MaxLat = -360.0, MaxLon = -360.0;
	for (int i = 0; i < PTaxiwayPoints->size(); i++) {
		if (PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayPoints->at(i).Lat) < MinLat) {
			MinLat = PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayPoints->at(i).Lat);
		}
		if (PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayPoints->at(i).Lat) > MaxLat) {
			MaxLat = PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayPoints->at(i).Lat);
		}
		if (PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayPoints->at(i).Lon) < MinLon) {
			MinLon = PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayPoints->at(i).Lon);
		}
		if (PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayPoints->at(i).Lon) > MaxLon) {
			MaxLon = PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayPoints->at(i).Lon);
		}
	}
	for (int i = 0; i < PTaxiwayParks->size(); i++) {
		if (PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayParks->at(i).Lat) < MinLat) {
			MinLat = PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayParks->at(i).Lat);
		}
		if (PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayParks->at(i).Lat) > MaxLat) {
			MaxLat = PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayParks->at(i).Lat);
		}
		if (PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayParks->at(i).Lon) < MinLon) {
			MinLon = PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayParks->at(i).Lon);
		}
		if (PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayParks->at(i).Lon) > MaxLon) {
			MaxLon = PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayParks->at(i).Lon);
		}
	}
	double dLat = MaxLat - MinLat;
	double dLon = MaxLon - MinLon;
	PAddAirportInformation->MinLat = MinLat;
	PAddAirportInformation->MinLon = MinLon;
	PAddAirportInformation->MaxLat = MaxLat;
	PAddAirportInformation->MaxLon = MaxLon;
	PAddAirportInformation->DLat = dLat;
	PAddAirportInformation->DLon = dLon;
	return NOERROR;
}

HRESULT Airport::GetFixes(ReadStreamText* SIDSTARFile)
{
	std::string* str = new std::string("");
	std::string* sNSWE = new std::string();
	std::string* sD = new std::string();
	std::string* sM = new std::string();
	FIXX fix;
	while (*str != "ENDFIXES")
	{
		SIDSTARFile->Read(str, 0);
		if (*str == "FIX")
		{
			SIDSTARFile->Read(str, 0);
			fix.Name = *str;
			SIDSTARFile->Read(str, 0);
			if (*str == "LATLON")
			{		
				SIDSTARFile->Read(sNSWE, 0);
				SIDSTARFile->Read(sD, 0);
				SIDSTARFile->Read(sM, 0);
				if (*sNSWE == "S")
				{
					fix.Lat = -std::stod(*sM) / 60.0 - std::stod(*sD);
				}
				else
				{
					fix.Lat = std::stod(*sM) / 60.0 + std::stod(*sD);
				}
				SIDSTARFile->Read(sNSWE, 0);
				SIDSTARFile->Read(sD, 0);
				SIDSTARFile->Read(sM, 0);
				if (*sNSWE == "W")
				{
					fix.Lon = -std::stod(*sM) / 60.0 - std::stod(*sD);
				}
				else
				{
					fix.Lon = std::stod(*sM) / 60.0 + std::stod(*sD);
				}

				if (sidstar == NULL)
				{
					sidstar = new SIDSTAR();
				}
				if (sidstar->FIXES == NULL)
				{
					sidstar->FIXES = new std::vector<FIXX>();
				}
				sidstar->FIXES->push_back(fix);
			}
		}
	}
	delete (str);
	delete (sNSWE);
	delete (sD);
	delete (sM);
	str = NULL;
	sNSWE = NULL;
	sD = NULL;
	sM = NULL;
	return NOERROR;
}
HRESULT Airport::GetSids(ReadStreamText* SIDSTARFile)
{
	std::string* str = new std::string("");

	Points sid;
	TRANSITION transition;
	WayPointA wayPoint;	
	SIDSTARFile->Read(str, 0);
	int SIDTRANSITION = 0;
	DWORD typeSC;
	while (*str != "ENDSIDS" && *str != "ENDSTARS" && *str != "ENDAPPROACHES")
	{	
		if (*str == "SID" || *str == "STAR" || *str == "APPROACH"  ||*str == "TRANSITION")
		{
			SIDTRANSITION = 0;
			if (sidstar == NULL)
			{
				sidstar = new SIDSTAR();
			}
			if (*str == "SID")
			{
				if (sidstar->SIDS == NULL)
				{
					sidstar->SIDS = new std::vector<Points>();
				}
				typeSC = 0;
			}
			else if (*str == "STAR")
			{
				if (sidstar->STARS == NULL)
				{
					sidstar->STARS = new std::vector<Points>();
				}
				typeSC = 1;
			}
			else if (*str == "APPROACH")
			{
				if (sidstar->APPROACHES == NULL)
				{
					sidstar->APPROACHES = new std::vector<Points>();
				}
				typeSC = 2;
			}
			
			
			if (*str == "SID" || *str == "STAR" || *str == "APPROACH")
			{
				sid = {};
				SIDTRANSITION = 1;
				SIDSTARFile->Read(str, 0);
				sid.Name = *str;
				SIDSTARFile->Read(str, 0);
				if (typeSC == 0)
				{
					sidstar->SIDS->push_back(sid);
				}
				else if (typeSC == 1)
				{
					sidstar->STARS->push_back(sid);
				}
				else if (typeSC == 2)
				{
					sidstar->APPROACHES->push_back(sid);
				}
				if (*str == "RNW")
				{
					if (typeSC == 0)
					{
						if (sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways == NULL)
						{
							sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways = new std::vector<std::string>();
						}
						SIDSTARFile->Read(str, 0);
						sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways->push_back(*str);
						SIDSTARFile->Read(str, 0);
					}
					else if (typeSC == 1)
					{
						if (sidstar->STARS->at(sidstar->STARS->size() - 1).Runways == NULL)
						{
							sidstar->STARS->at(sidstar->STARS->size() - 1).Runways = new std::vector<std::string>();
						}
						SIDSTARFile->Read(str, 0);
						sidstar->STARS->at(sidstar->STARS->size() - 1).Runways->push_back(*str);
						SIDSTARFile->Read(str, 0);
					}
					else if (typeSC == 2)
					{
						if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways == NULL)
						{
							sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways = new std::vector<std::string>();
						}
						SIDSTARFile->Read(str, 0);
						sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways->push_back(*str);
						SIDSTARFile->Read(str, 0);
					}
					
				}
				if (typeSC == 0)
				{
					if (sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS == NULL)
					{
						sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS = new std::vector<TRANSITION>();
					}
				}
				else if (typeSC == 1)
				{
					if (sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS == NULL)
					{
						sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS = new std::vector<TRANSITION>();
					}
				}
				else if (typeSC == 2)
				{
					if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS == NULL)
					{
						sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS = new std::vector<TRANSITION>();
					}
				}
				
			}
			else if (*str == "TRANSITION")
			{
				transition = {};
				SIDTRANSITION = 2;
				if (typeSC == 0)
				{
					if (sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS == NULL)
					{
						sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS = new std::vector<TRANSITION>();
					}
					SIDSTARFile->Read(str, 0);
					transition.Name = *str;
					SIDSTARFile->Read(str, 0);
					sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS->push_back(transition);
				}
				else if (typeSC == 1)
				{
					if (sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS == NULL)
					{
						sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS = new std::vector<TRANSITION>();
					}
					SIDSTARFile->Read(str, 0);
					transition.Name = *str;
					SIDSTARFile->Read(str, 0);
					sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS->push_back(transition);
				}
				else if (typeSC == 2)
				{
					if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS == NULL)
					{
						sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS = new std::vector<TRANSITION>();
					}
					SIDSTARFile->Read(str, 0);
					transition.Name = *str;
					SIDSTARFile->Read(str, 0);
					sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS->push_back(transition);
				}
				
			}
			else if (*str == "RNW")
			{
				if (typeSC == 0)
				{
					if (sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways == NULL)
					{
						sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways = new std::vector<std::string>();
					}
					SIDSTARFile->Read(str, 0);
					sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways->push_back(*str);
					SIDSTARFile->Read(str, 0);
				}
				else if (typeSC == 1)
				{
					if (sidstar->STARS->at(sidstar->STARS->size() - 1).Runways == NULL)
					{
						sidstar->STARS->at(sidstar->STARS->size() - 1).Runways = new std::vector<std::string>();
					}
					SIDSTARFile->Read(str, 0);
					sidstar->STARS->at(sidstar->STARS->size() - 1).Runways->push_back(*str);
					SIDSTARFile->Read(str, 0);
				}
				else if (typeSC == 2)
				{
					if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways == NULL)
					{
						sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways = new std::vector<std::string>();
					}
					SIDSTARFile->Read(str, 0);
					sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways->push_back(*str);
					SIDSTARFile->Read(str, 0);
				}
				
			}
			
			while (*str != "SID" && *str != "STAR" && *str != "APPROACH" && *str != "TRANSITION" && *str != "ENDSIDS" && *str != "ENDSTARS" && *str != "ENDAPPROACHES")
			{		
				wayPoint = {};
				
				if (*str == "TURN")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "RIGHT")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
						if (*str == "DIRECT")
						{
							wayPoint.TypeName = wayPoint.TypeName + *str;
							SIDSTARFile->Read(str, 0);
						}
					}
					if (*str == "LEFT")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
						if (*str == "DIRECT")
						{
							wayPoint.TypeName = wayPoint.TypeName + *str;
							SIDSTARFile->Read(str, 0);
						}
					}
				}
				if (*str == "TRK")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					wayPoint.TRK = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "INTERCEPT")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "RADIAL")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
						wayPoint.RADIAL = std::stod(*str);
						SIDSTARFile->Read(str, 0);
					}

				}
				if (*str == "TO")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "FROM")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "HOLD")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "AT")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
				}
				if (*str == "FIX")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "OVERFLY")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
					wayPoint.FIXName = *str;
					SIDSTARFile->Read(str, 0);

				}
				
				if (*str == "AT")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "OR")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
					if (*str == "ABOVE")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
					if (*str == "BELOW")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
					wayPoint.ALT = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "AT")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "OR")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
					if (*str == "ABOVE")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
					if (*str == "BELOW")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
					}
					wayPoint.ALT2 = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "UNTIL")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					std::string s = *str;
					wayPoint.ALT = std::stod(*str);
					SIDSTARFile->Read(str, 0);
					if (*str == "FROM")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						wayPoint.LegDist = std::stod(s);
						SIDSTARFile->Read(str, 0);
						if (*str == "FIX")
						{
							wayPoint.TypeName = wayPoint.TypeName + *str;
							SIDSTARFile->Read(str, 0);
							if (*str == "OVERFLY")
							{
								wayPoint.TypeName = wayPoint.TypeName + *str;
								SIDSTARFile->Read(str, 0);
							}
							wayPoint.FIXName = *str;
							SIDSTARFile->Read(str, 0);

						}
					}
					else
					{
						wayPoint.ALT = std::stod(s);
					}
				}


				if (*str == "RIGHT")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "TURN")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
						if (*str == "INBOUNDCOURSE")
						{
							wayPoint.TypeName = wayPoint.TypeName + *str;
							SIDSTARFile->Read(str, 0);
							wayPoint.TRK = std::stod(*str);
							SIDSTARFile->Read(str, 0);
						}
					}
				}
				if (*str == "LEFT")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					if (*str == "TURN")
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						SIDSTARFile->Read(str, 0);
						if (*str == "INBOUNDCOURSE")
						{
							wayPoint.TypeName = wayPoint.TypeName + *str;
							SIDSTARFile->Read(str, 0);
							wayPoint.TRK = std::stod(*str);
							SIDSTARFile->Read(str, 0);
						}
					}
				}
				if (*str == "ALT")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					wayPoint.ALT = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}

				if (*str == "LEGTIME")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					wayPoint.LegTime = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "LEGDIST")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					wayPoint.LegDist = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "VECTORS")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
				}
				if (*str == "HDG")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					wayPoint.TRK = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}
				
				if (*str == "RNW")
				{
					
					if (typeSC == 0)
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						if (sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways == NULL)
						{
							sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways = new std::vector<std::string>();
						}
						SIDSTARFile->Read(str, 0);
						//wayPoint.FIXName = *str;
						sidstar->SIDS->at(sidstar->SIDS->size() - 1).Runways->push_back(*str);
						SIDSTARFile->Read(str, 0);
					}
					else if (typeSC == 1)
					{
						if (sidstar->STARS->at(sidstar->STARS->size() - 1).Runways == NULL)
						{
							sidstar->STARS->at(sidstar->STARS->size() - 1).Runways = new std::vector<std::string>();
						}
						//if (sidstar->STARS->at(sidstar->STARS->size() - 1).Legs == NULL)
						//{
						//	sidstar->STARS->at(sidstar->STARS->size() - 1).Legs = new std::vector<WayPointA>();
						//}
						SIDSTARFile->Read(str, 0);
						wayPoint.FIXName = "RNW";

						//sidstar->STARS->at(sidstar->STARS->size() - 1).Legs->push_back(wayPoint);
						sidstar->STARS->at(sidstar->STARS->size() - 1).Runways->push_back(*str);
						SIDSTARFile->Read(str, 0);
					}
					else if (typeSC == 2)
					{
						wayPoint.TypeName = wayPoint.TypeName + *str;
						if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways == NULL)
						{
							sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways = new std::vector<std::string>();
						}
						//if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Legs == NULL)
						//{
						//	sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Legs = new std::vector<WayPointA>();
						//}

						SIDSTARFile->Read(str, 0);
						wayPoint.FIXName = *str;
						//sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Legs->push_back(wayPoint);
						sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Runways->push_back(*str);
						SIDSTARFile->Read(str, 0);
					}


				}
				try
				{
					
					DWORD alt = wayPoint.ALT = std::stod(*str);
					wayPoint.ALT = std::stod(*str);
					SIDSTARFile->Read(str, 0);
					wayPoint.TypeName = wayPoint.TypeName + "FALT";
				}
				catch (...)
				{

				}
				if (*str == "SPEED")
				{
					wayPoint.TypeName = wayPoint.TypeName + *str;
					SIDSTARFile->Read(str, 0);
					wayPoint.SPEED = std::stod(*str);
					SIDSTARFile->Read(str, 0);
				}

				if (SIDTRANSITION == 1)
				{
					if (typeSC == 0)
					{
						if (sidstar->SIDS->at(sidstar->SIDS->size() - 1).Legs == NULL)
						{
							sidstar->SIDS->at(sidstar->SIDS->size() - 1).Legs = new std::vector<WayPointA>();
						}
						sidstar->SIDS->at(sidstar->SIDS->size() - 1).Legs->push_back(wayPoint);
					}
					else if (typeSC == 1)
					{
						if (sidstar->STARS->at(sidstar->STARS->size() - 1).Legs == NULL)
						{
							sidstar->STARS->at(sidstar->STARS->size() - 1).Legs = new std::vector<WayPointA>();
						}
						if (wayPoint.FIXName == "RNW")
						{
							wayPoint = {};

						}
						else
						{
							sidstar->STARS->at(sidstar->STARS->size() - 1).Legs->push_back(wayPoint);
						}
					}
					else if (typeSC == 2)
					{
						if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Legs == NULL)
						{
							sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Legs = new std::vector<WayPointA>();
						}
						sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).Legs->push_back(wayPoint);
					}
					
				}
				if (SIDTRANSITION == 2)
				{
					if (typeSC == 0)
					{
						if (sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS->at(sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS->size() - 1).Legs == NULL)
						{
							sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS->at(sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS->size() - 1).Legs = new std::vector<WayPointA>();
						}
						sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS->at(sidstar->SIDS->at(sidstar->SIDS->size() - 1).TRANSITIONS->size() - 1).Legs->push_back(wayPoint);
					}
					else if (typeSC == 1)
					{
						if (sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS->at(sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS->size() - 1).Legs == NULL)
						{
							sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS->at(sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS->size() - 1).Legs = new std::vector<WayPointA>();
						}
						sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS->at(sidstar->STARS->at(sidstar->STARS->size() - 1).TRANSITIONS->size() - 1).Legs->push_back(wayPoint);
					}
					else if (typeSC == 2)
					{
						if (sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS->at(sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS->size() - 1).Legs == NULL)
						{
							sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS->at(sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS->size() - 1).Legs = new std::vector<WayPointA>();
						}
						sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS->at(sidstar->APPROACHES->at(sidstar->APPROACHES->size() - 1).TRANSITIONS->size() - 1).Legs->push_back(wayPoint);
					}
					
				}
				

			}

		}
	}
	delete (str);
	str = NULL;
	return NOERROR;
}
std::string Airport::GetTypeNameFromPoint(sroute point)
{
	std::string name = "";
	switch (point.type)
	{
	case FIX:
	{
		name = "FIX";
		if (point.alt != 0)
		{
			switch (point.alttype)
			{
			case AT:
			{
				name += "FALT";
				break;
			}
			case ATORABOVE:
			{
				name += "ATORABOVE";
				break;
			}
			case ATORBELOW:
			{
				name += "ATORBELOW";
				break;
			}
			case ABOVEBELOW:
			{
				name += "ATORBELOWATORABOVE";
				break;
			}
			}
		}
		if (point.speed != 0)
		{
			name += "SPEED";
		}
		break;

	}
	case FIXOVERFY:
	{
		name = "FIXOVERFLY";
		if (point.alt != 0)
		{
			switch (point.alttype)
			{
			case AT:
			{
				name += "FALT";
				break;
			}
			case ATORABOVE:
			{
				name += "ATORABOVE";
				break;
			}
			case ATORBELOW:
			{
				name += "ATORBELOW";
				break;
			}
			case ABOVEBELOW:
			{
				name += "ATORBELOWATORABOVE";
				break;
			}
			}
		}
		if (point.speed != 0)
		{
			name += "SPEED";
		}
		break;
	}
	case HDGINTERCEPTTOFIX:
	{
		name = "TRKINTERCEPTRADIALTOFIX";
		break;
	}
	//HDGINTERCEPTTOFIX,
	//HDGINTERCEPTFROMFIX,
	//TRKINTERCEPTTOFIX,
	//TRKINTERCEPTFROMFIX,
	case TURNRIGHT:
	{
		name = "TURNRIGHTDIRECTFIX";
		if (point.alt != 0)
		{
			switch (point.alttype)
			{
			case AT:
			{
				name += "FALT";
				break;
			}
			case ATORABOVE:
			{
				name += "ATORABOVE";
				break;
			}
			case ATORBELOW:
			{
				name += "ATORBELOW";
				break;
			}
			case ABOVEBELOW:
			{
				name += "ATORBELOWATORABOVE";
				break;
			}
			}
		}
		break;
	}
	case TURNLEFT:
	{
		name = "TURNLEFTDIRECTFIX";
		if (point.alt != 0)
		{
			switch (point.alttype)
			{
			case AT:
			{
				name += "FALT";
				break;
			}
			case ATORABOVE:
			{
				name += "ATORABOVE";
				break;
			}
			case ATORBELOW:
			{
				name += "ATORBELOW";
				break;
			}
			case ABOVEBELOW:
			{
				name += "ATORBELOWATORABOVE";
				break;
			}
			}
		}
		break;
	}
	case TURNRIGHTOVERFLY:
	{
		name = "TURNRIGHTDIRECTFIXOVERFLY";
		if (point.alt != 0)
		{
			switch (point.alttype)
			{
			case AT:
			{
				name += "FALT";
				break;
			}
			case ATORABOVE:
			{
				name += "ATORABOVE";
				break;
			}
			case ATORBELOW:
			{
				name += "ATORBELOW";
				break;
			}
			case ABOVEBELOW:
			{
				name += "ATORBELOWATORABOVE";
				break;
			}
			}
		}
		break;
	}
	case TURNLEFTOVERFLY:
	{
		name = "TURNLEFTDIRECTFIXOVERFLY";
		if (point.alt != 0)
		{
			switch (point.alttype)
			{
			case AT:
			{
				name += "FALT";
				break;
			}
			case ATORABOVE:
			{
				name += "ATORABOVE";
				break;
			}
			case ATORBELOW:
			{
				name += "ATORBELOW";
				break;
			}
			case ABOVEBELOW:
			{
				name += "ATORBELOWATORABOVE";
				break;
			}
			}
		}
		break;
	}
	case TRKUNTIL:
	{
		name = "TRKUNTIL";
		break;
	}
	case TRKUNTILFROMFIX:
	{
		name = "TRKUNTILFROMFIX";
		if (point.speed != 0)
		{
			name += "SPEED";
		}
		break;
	}
	//HDGUNTIL,
	//HDGVECTORS,
	case RNW:
	{
		name = "RNW";
		break;
	}
	default:
	{
		name = "ERROR";
	}
	}
	return name;
}
HRESULT Airport::GetSIDSTAR() {
	std::string ICAO = BGLXData::DecodeICAO(PAirportInformation->ICAO);
	std::string* str = new std::string();
	fopen_s(&yyin, (AirportData->RootSim + "\\PMDG\\SIDSTARS\\" + std::string(ICAO.begin(), ICAO.end()) + ".txt").c_str(),"r");
  fopen_s(&yyout, (AirportData->RootSim + "\\PMDG\\SIDSTARS\\" + std::string(ICAO.begin(), ICAO.end()) + ".out").c_str(), "w");
  while (!feof(yyin)) {
    yydebug = 0;
		if (yyparse() == 1)
		{
			//sidstar = gSidStar;
			sidstar2 = *(ssidstar*)gSidStar;
			sidstar = new SIDSTAR();
			if (sidstar->FIXES == NULL)
			{
				sidstar->FIXES = new std::vector<FIXX>();
			}
			FIXX tfix;
			for (int i = 0; i < sidstar2.efixes->size(); i++)
			{
				tfix.Lat = sidstar2.efixes->at(i).lat;
				tfix.Lon = sidstar2.efixes->at(i).lon;
				tfix.Name = sidstar2.efixes->at(i).name;
				sidstar->FIXES->push_back(tfix);
			}

			if (sidstar->SIDS == NULL)
			{
				sidstar->SIDS = new std::vector<Points>();
			}
			if (sidstar2.esids != NULL) {
				for (int i = 0; i < sidstar2.esids->size(); i++)
				{


					for (int k = 0; k < sidstar2.esids->at(i).ernws->size(); k++)
					{
						Points tpoints;
						tpoints.Runways = new std::vector<std::string>();
						WayPointA twaypointa;
						TRANSITION ttransition;
						tpoints.Name = sidstar2.esids->at(i).name;
						tpoints.Runways->push_back(sidstar2.esids->at(i).ernws->at(k).name);
						tpoints.Legs = new std::vector<WayPointA>();
						for (int m = 0; m < sidstar2.esids->at(i).ernws->at(k).eroute->size(); m++)
						{
							twaypointa.ALT = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).alt;
							twaypointa.ALT2 = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).alt2;
							twaypointa.FIXName = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).name;
							twaypointa.LegDist = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).dist;
							twaypointa.LegTime = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).legtime;
							twaypointa.RADIAL = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).radial;
							twaypointa.SPEED = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).speed;
							twaypointa.TRK = sidstar2.esids->at(i).ernws->at(k).eroute->at(m).hdg + sidstar2.esids->at(i).ernws->at(k).eroute->at(m).trk;
							twaypointa.TypeName = GetTypeNameFromPoint(sidstar2.esids->at(i).ernws->at(k).eroute->at(m));
							tpoints.Legs->push_back(twaypointa);
						}
						if (sidstar2.esids->at(i).etrans)
						{
							tpoints.TRANSITIONS = new std::vector<TRANSITION>();
							for (int m = 0; m < sidstar2.esids->at(i).etrans->size(); m++)
							{
								ttransition.Name = sidstar2.esids->at(i).etrans->at(m).name;
								ttransition.Legs = new std::vector<WayPointA>();
								for (int n = 0; n < sidstar2.esids->at(i).etrans->at(m).eroute->size(); n++)
								{
									twaypointa.ALT = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).alt;
									twaypointa.ALT2 = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).alt2;
									twaypointa.FIXName = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).name;
									twaypointa.LegDist = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).dist;
									twaypointa.LegTime = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).legtime;
									twaypointa.RADIAL = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).radial;
									twaypointa.SPEED = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).speed;
									twaypointa.TRK = sidstar2.esids->at(i).etrans->at(m).eroute->at(n).hdg + sidstar2.esids->at(i).etrans->at(m).eroute->at(n).trk;
									twaypointa.TypeName = GetTypeNameFromPoint(sidstar2.esids->at(i).etrans->at(m).eroute->at(n));
									ttransition.Legs->push_back(twaypointa);
								}
								tpoints.TRANSITIONS->push_back(ttransition);
							}
						}
						else
						{
							tpoints.TRANSITIONS = NULL;
						}
						sidstar->SIDS->push_back(tpoints);
					}

				}
		}
			if (sidstar->STARS == NULL)
			{
				sidstar->STARS = new std::vector<Points>();
			}
			for (int i = 0; i < sidstar2.estars->size(); i++)
			{
				for (int k = 0; k < sidstar2.estars->at(i).ernws->size(); k++)
				{
					Points tpoints;
					tpoints.Runways = new std::vector<std::string>();
					WayPointA twaypointa;
					TRANSITION ttransition;
					tpoints.Name = sidstar2.estars->at(i).name;
					for (int s = 0; s < sidstar2.estars->at(i).ernw->size(); s++)
					{
						tpoints.Runways->push_back(sidstar2.estars->at(i).ernw->at(s));
					}
					tpoints.Legs = new std::vector<WayPointA>();
					for (int m = 0; m < sidstar2.estars->at(i).ernws->at(k).eroute->size(); m++)
					{
						twaypointa.ALT = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).alt;
						twaypointa.ALT2 = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).alt2;
						twaypointa.FIXName = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).name;
						twaypointa.LegDist = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).dist;
						twaypointa.LegTime = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).legtime;
						twaypointa.RADIAL = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).radial;
						twaypointa.SPEED = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).speed;
						twaypointa.TRK = sidstar2.estars->at(i).ernws->at(k).eroute->at(m).hdg + sidstar2.estars->at(i).ernws->at(k).eroute->at(m).trk;
						twaypointa.TypeName = GetTypeNameFromPoint(sidstar2.estars->at(i).ernws->at(k).eroute->at(m));
						tpoints.Legs->push_back(twaypointa);
					}
					if (sidstar2.estars->at(i).etrans)
					{
						tpoints.TRANSITIONS = new std::vector<TRANSITION>();
						for (int m = 0; m < sidstar2.estars->at(i).etrans->size(); m++)
						{
							ttransition.Name = sidstar2.estars->at(i).etrans->at(m).name;
							ttransition.Legs = new std::vector<WayPointA>();
							for (int n = 0; n < sidstar2.estars->at(i).etrans->at(m).eroute->size(); n++)
							{
								twaypointa.ALT = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).alt;
								twaypointa.ALT2 = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).alt2;
								twaypointa.FIXName = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).name;
								twaypointa.LegDist = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).dist;
								twaypointa.LegTime = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).legtime;
								twaypointa.RADIAL = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).radial;
								twaypointa.SPEED = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).speed;
								twaypointa.TRK = sidstar2.estars->at(i).etrans->at(m).eroute->at(n).hdg + sidstar2.estars->at(i).etrans->at(m).eroute->at(n).trk;
								twaypointa.TypeName = GetTypeNameFromPoint(sidstar2.estars->at(i).etrans->at(m).eroute->at(n));
								ttransition.Legs->push_back(twaypointa);
							}
							tpoints.TRANSITIONS->push_back(ttransition);
						}
					}
					else
					{
						tpoints.TRANSITIONS = NULL;
					}
					
					sidstar->STARS->push_back(tpoints);
				}
			}
			if (sidstar->APPROACHES == NULL)
			{
				sidstar->APPROACHES = new std::vector<Points>();
			}
			for (int i = 0; i < sidstar2.eapproaches->size(); i++)
			{
				for (int k = 0; k < sidstar2.eapproaches->at(i).ernws->size(); k++)
				{
					Points tpoints;
					tpoints.Runways = new std::vector<std::string>();
					WayPointA twaypointa;
					TRANSITION ttransition;
					tpoints.Name = sidstar2.eapproaches->at(i).name;
					for (int s = 0; s < sidstar2.eapproaches->at(i).ernw->size(); s++)
					{
						tpoints.Runways->push_back(sidstar2.eapproaches->at(i).ernw->at(s));
					}
					tpoints.Legs = new std::vector<WayPointA>();
					for (int m = 0; m < sidstar2.eapproaches->at(i).ernws->at(k).eroute->size(); m++)
					{
						twaypointa.ALT = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).alt;
						twaypointa.ALT2 = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).alt2;
						twaypointa.FIXName = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).name;
						twaypointa.LegDist = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).dist;
						twaypointa.LegTime = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).legtime;
						twaypointa.RADIAL = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).radial;
						twaypointa.SPEED = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).speed;
						twaypointa.TRK = sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).hdg + sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m).trk;
						twaypointa.TypeName = GetTypeNameFromPoint(sidstar2.eapproaches->at(i).ernws->at(k).eroute->at(m));
						tpoints.Legs->push_back(twaypointa);
					}
					if (sidstar2.eapproaches->at(i).etrans)
					{
						tpoints.TRANSITIONS = new std::vector<TRANSITION>();
						for (int m = 0; m < sidstar2.eapproaches->at(i).etrans->size(); m++)
						{
							ttransition.Name = sidstar2.eapproaches->at(i).etrans->at(m).name;
							ttransition.Legs = new std::vector<WayPointA>();
							for (int n = 0; n < sidstar2.eapproaches->at(i).etrans->at(m).eroute->size(); n++)
							{
								twaypointa.ALT = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).alt;
								twaypointa.ALT2 = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).alt2;
								twaypointa.FIXName = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).name;
								twaypointa.LegDist = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).dist;
								twaypointa.LegTime = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).legtime;
								twaypointa.RADIAL = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).radial;
								twaypointa.SPEED = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).speed;
								twaypointa.TRK = sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).hdg + sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n).trk;
								twaypointa.TypeName = GetTypeNameFromPoint(sidstar2.eapproaches->at(i).etrans->at(m).eroute->at(n));
								ttransition.Legs->push_back(twaypointa);
							}
							tpoints.TRANSITIONS->push_back(ttransition);
						}
					}
					else
					{
						tpoints.TRANSITIONS = NULL;
					}

					sidstar->APPROACHES->push_back(tpoints);
				}
			}
			/*struct Points
			{
			std::string Name;
			std::vector<std::string>* Runways;
			std::vector<WayPointA>* Legs;
			std::vector<TRANSITION>* TRANSITIONS;
			};*/
			
			// "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEEDLEGTIME")			
			// "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEED")		
			
     
			return NOERROR;
		}
		/*if (tokenId == TOKEN_ERROR)
		{
			std::cerr << "wrong character in input" << std::endl;
		}
		else
		{
			ExecuteCommand(interpreter, static_cast<Token>(tokenId));
		}*/
	}
  
	/*while (!SIDSTARFile->Read(str, 0))
	{		
		if (*str == "FIXES")
		{
			GetFixes(SIDSTARFile);
		}
		else if (*str == "SIDS")
		{
			GetSids(SIDSTARFile);
		}
		else if (*str == "STARS")
		{
			GetSids(SIDSTARFile);
		}
		else if (*str == "APPROACHES")
		{
			GetSids(SIDSTARFile);
		}
	}
	delete(SIDSTARFile);
	SIDSTARFile = NULL;
	return NOERROR;*/
	
}
//HRESULT Airport::GetSIDSTAR()
//{
//	std::string ICAO = BGLXData::DecodeICAO(PAirportInformation->ICAO);
//	ReadStreamText* SIDSTARFile = new ReadStreamText(AirportData->RootSim + L"PMDG\\SIDSTARS\\" + std::wstring(ICAO.begin(), ICAO.end()) + L".txt");
//}

HRESULT Airport::GetRunwayInformation() {
	if (Records)
	{
		std::string ds[] = { "", "L", "R", "C", "W", "A", "B" };
		if (PRunways)
		{
			PRunways->clear();
		}
		else
		{
			PRunways = new std::vector<RunwayInfoMSFS>();
		}
		if (POneWayRunways) {
			POneWayRunways->clear();
		}
		else {
			POneWayRunways = new std::vector<DATA_RUNWAY>();
		}
		if (PPointsRunway) {
			PPointsRunway->clear();
		}
		else {
			PPointsRunway = new std::vector<POINTS_RUNWAY>();
		}

		for (int i = 0; i < PTaxiwayPaths->size(); i++) {
			if ((PTaxiwayPaths->at(i).Type & 0xf) == 2) {
				//PTaxiwayPaths->at(i).Type = PTaxiwayPaths->at(i).Type & 0xf1;
			}
		}

		for (int i = 0; i < Records->size(); i++)
		{
			if (Records->at(i).ID == 0xCE)
			{
				DWORD Offset = Records->at(i).Offset;
				
				RunwayInfoMSFS Runway;
				DATA_RUNWAY OneRunway;
				RunwayOffsetThreshold PrimaryOT;
				RunwayOffsetThreshold SecondaryOT;
				double PrimaryRunwayDeviation = 0.0;
				double SecondaryRunwayDeviation = 0.0;
				
				double sAlt = -1.0, eAlt = -1.0;
				BGLX->BGLXFile->Read(&Runway, Offset, sizeof(RunwayInfoMSFS));

				for (int j = i+1; j < Records->size(); j++) {
					RunwayDeform RunwayDef;
					if (Records->at(j).ID == 0xCE) {
						break;
					}
					if (Records->at(j).ID == 0x3E) {
						BGLX->BGLXFile->Read(&RunwayDef, Records->at(j).Offset, sizeof(RunwayDeform));
						if (RunwayDef.Ratio == 0.0) {
							sAlt = RunwayDef.Alt;
						}
						else if (RunwayDef.Ratio == 1.0) {
							eAlt = RunwayDef.Alt;
						}

					}
					if (Records->at(j).ID == 0x05) {
						BGLX->BGLXFile->Read(&PrimaryOT, Records->at(j).Offset, sizeof(RunwayOffsetThreshold));
						PrimaryRunwayDeviation = PrimaryOT.Length;
					}
					if (Records->at(j).ID == 0x06) {
						BGLX->BGLXFile->Read(&SecondaryOT, Records->at(j).Offset, sizeof(RunwayOffsetThreshold));
						SecondaryRunwayDeviation = SecondaryOT.Length;
					}
				}

				if (sAlt == -1.0 || eAlt == -1.0) {
					sAlt = eAlt = double(Runway.Alt) / 1000.0;
				}
				
				PMDG_TEST::SIMMATH::DSHEH dsheh;
				dsheh.Ella = PMDG_TEST::SIMMATH::GetDALatLon(PMDG_TEST::SIMMATH::DecodeLat(Runway.Lat), PMDG_TEST::SIMMATH::DecodeLon(Runway.Lon), Runway.Heading, (Runway.Distance)/2000);
				dsheh.Slla = PMDG_TEST::SIMMATH::GetDALatLon(PMDG_TEST::SIMMATH::DecodeLat(Runway.Lat), PMDG_TEST::SIMMATH::DecodeLon(Runway.Lon), PMDG_TEST::SIMMATH::Constrain360(Runway.Heading - 180.0), (Runway.Distance) / 2000);

				PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
				OneRunway.sLatitude = dsheh.Slla.Latitude;
				OneRunway.sLongitude = dsheh.Slla.Longitude;
				OneRunway.sHeading = dsheh.SH;
				OneRunway.eLatitude = dsheh.Ella.Latitude;
				OneRunway.eLongitude = dsheh.Ella.Longitude;
				OneRunway.eHeading = dsheh.EH;
				OneRunway.alt = sAlt * 3.28084;
				OneRunway.Dev = PrimaryRunwayDeviation / 1000.0;
				if (Runway.PrimaryNumber < 10) {
					OneRunway.Name = "0" + std::to_string(Runway.PrimaryNumber) + ds[Runway.PrimaryDesignator];
				} 
				else {
					OneRunway.Name = std::to_string(Runway.PrimaryNumber) + ds[Runway.PrimaryDesignator];
				}
				POneWayRunways->push_back(OneRunway);

				dsheh.Slla = PMDG_TEST::SIMMATH::GetDALatLon(PMDG_TEST::SIMMATH::DecodeLat(Runway.Lat), PMDG_TEST::SIMMATH::DecodeLon(Runway.Lon), Runway.Heading, (Runway.Distance ) / 2000);
				dsheh.Ella = PMDG_TEST::SIMMATH::GetDALatLon(PMDG_TEST::SIMMATH::DecodeLat(Runway.Lat), PMDG_TEST::SIMMATH::DecodeLon(Runway.Lon), PMDG_TEST::SIMMATH::Constrain360(Runway.Heading - 180.0), (Runway.Distance) / 2000);

				PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
				OneRunway.sLatitude = dsheh.Slla.Latitude;
				OneRunway.sLongitude = dsheh.Slla.Longitude;
				OneRunway.sHeading = dsheh.SH;
				OneRunway.eLatitude = dsheh.Ella.Latitude;
				OneRunway.eLongitude = dsheh.Ella.Longitude;
				OneRunway.eHeading = dsheh.EH;
				OneRunway.alt = eAlt * 3.28084;
				OneRunway.Dev = SecondaryRunwayDeviation / 1000.0;
				if (Runway.SecondaryNumber < 10) {
					OneRunway.Name = "0" + std::to_string(Runway.SecondaryNumber) + ds[Runway.SecondaryDesignator];
				}
				else {
					OneRunway.Name = std::to_string(Runway.SecondaryNumber) + ds[Runway.SecondaryDesignator];
				}
				
				POneWayRunways->push_back(OneRunway);
				PRunways->push_back(Runway);
				for (int j = 0; j < PTaxiwayPoints->size(); j++) {
					dsheh.Ella = PMDG_TEST::SIMMATH::GetDALatLon(PMDG_TEST::SIMMATH::DecodeLat(Runway.Lat), PMDG_TEST::SIMMATH::DecodeLon(Runway.Lon), PMDG_TEST::SIMMATH::Constrain360(Runway.Heading - 180.0), Runway.Distance / 2000);
					POINTS_RUNWAY PointRunway;
					PointRunway.Runway = POneWayRunways->size() - 2;
					PointRunway.Run = Runway.PrimaryDesignator * 0x1000 + Runway.PrimaryNumber;
					dsheh.Slla.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayPoints->at(j).Lat);
					dsheh.Slla.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayPoints->at(j).Lon);
					PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
					double a = PMDG_TEST::SIMMATH::GetFixDA(sin(PMDG_TEST::SIMMATH::Constrain180(dsheh.EH - POneWayRunways->at(PointRunway.Runway).eHeading) * M_PI / 180) * dsheh.D, PMDG_TEST::SIMMATH::Constrain180(dsheh.EH - POneWayRunways->at(PointRunway.Runway).eHeading));
					PointRunway.DistToEnd = dsheh.D;
					PointRunway.DistToCenterLine = abs(a);
					PointRunway.TaxiwayPoint = j;
					PointRunway.Lat = dsheh.Ella.Latitude;
					PointRunway.Lon = dsheh.Ella.Longitude;
					if ((dsheh.D <= (Runway.Distance / 1000)) && (abs(a) <= (Runway.Width / 2000)) && (abs(PMDG_TEST::SIMMATH::Constrain180(dsheh.EH - POneWayRunways->at(PointRunway.Runway).eHeading))) > 90.0) {
						PTaxiwayPoints->at(PointRunway.TaxiwayPoint).Flag = 0xFF;
						PPointsRunway->push_back(PointRunway);
						PPointsRunway->push_back(PointRunway);
					}	
				}
				for (int j = 0; j < PTaxiwayPoints->size(); j++) {
					dsheh.Ella = PMDG_TEST::SIMMATH::GetDALatLon(PMDG_TEST::SIMMATH::DecodeLat(Runway.Lat), PMDG_TEST::SIMMATH::DecodeLon(Runway.Lon), PMDG_TEST::SIMMATH::Constrain360(Runway.Heading - 180.0), Runway.Distance / 2000);
					POINTS_RUNWAY PointRunway;
					PointRunway.Runway = POneWayRunways->size() - 1;
					PointRunway.Run = Runway.PrimaryDesignator * 0x1000 + Runway.PrimaryNumber;
					dsheh.Slla.Latitude = PMDG_TEST::SIMMATH::DecodeLat(PTaxiwayPoints->at(j).Lat);
					dsheh.Slla.Longitude = PMDG_TEST::SIMMATH::DecodeLon(PTaxiwayPoints->at(j).Lon);
					PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
					double a = PMDG_TEST::SIMMATH::GetFixDA(sin(PMDG_TEST::SIMMATH::Constrain180(dsheh.EH - POneWayRunways->at(PointRunway.Runway).eHeading) * M_PI / 180) * dsheh.D, PMDG_TEST::SIMMATH::Constrain180(dsheh.EH - POneWayRunways->at(PointRunway.Runway).eHeading));
					PointRunway.DistToEnd = dsheh.D;
					PointRunway.DistToCenterLine = abs(a);
					PointRunway.TaxiwayPoint = j;
					PointRunway.Lat = dsheh.Ella.Latitude;
					PointRunway.Lon = dsheh.Ella.Longitude;
					if ((dsheh.D <= (Runway.Distance / 1000)) && (abs(a) <= (Runway.Width / 2000)) && (abs(PMDG_TEST::SIMMATH::Constrain180(dsheh.EH - POneWayRunways->at(PointRunway.Runway).eHeading))) < 90.0) {
						//PPointsRunway->push_back(PointRunway);
						//PPointsRunway->push_back(PointRunway);
					}
				}
				double max = -50000, min = 50000;
				int maxPoint = -1, minPoint = -1;
				for (int j = 0; j < PPointsRunway->size(); j++) {
					if ((PPointsRunway->at(j).Runway == (POneWayRunways->size() - 2)) || (PPointsRunway->at(j).Runway == (POneWayRunways->size() - 1))) {
						if (PPointsRunway->at(j).DistToEnd < min) {
							min = PPointsRunway->at(j).DistToEnd;
							minPoint = j;
						}
						if (PPointsRunway->at(j).DistToEnd > max) {
							max = PPointsRunway->at(j).DistToEnd;
							maxPoint = j;
						}
					}
				}
				TaxiwayPoints tPoint;
				TaxiwayPaths tPath;
				if ((minPoint >= 0) || (maxPoint >= 0)) {
					tPoint.Lat = PMDG_TEST::SIMMATH::EncodeLat(POneWayRunways->at(POneWayRunways->size() - 2).sLatitude);
					tPoint.Lon = PMDG_TEST::SIMMATH::EncodeLon(POneWayRunways->at(POneWayRunways->size() - 2).sLongitude);
					tPoint.Type = 1;
					tPoint.Flag = 0xFF;
					POINTS_RUNWAY PointRunway;
					PointRunway.DistToEnd = 0;
					PointRunway.DistToCenterLine = 0;
					PointRunway.TaxiwayPoint = PTaxiwayPoints->size();
					PointRunway.Lat = POneWayRunways->at(POneWayRunways->size() - 2).sLatitude;
					PointRunway.Lon = POneWayRunways->at(POneWayRunways->size() - 2).sLongitude;
					PointRunway.Run = Runway.PrimaryDesignator * 0x1000 + Runway.PrimaryNumber;
					PTaxiwayPoints->push_back(tPoint);
					PPointsRunway->push_back(PointRunway);
					tPath.IndexStartPoint = PPointsRunway->at(minPoint).TaxiwayPoint;
					tPath.End = PTaxiwayPoints->size() - 1;
					tPath.Type = 1; //???
					tPath.Unk1 = 0x5555;
					tPath.IndexEndPoint = Runway.PrimaryDesignator * 0x1000;
					tPath.TaxiNameIndex = 0; //  Runway.PrimaryNumber;
					PTaxiwayPaths->push_back(tPath);
					tPoint.Lat = PMDG_TEST::SIMMATH::EncodeLat(POneWayRunways->at(POneWayRunways->size() - 2).eLatitude);
					tPoint.Lon = PMDG_TEST::SIMMATH::EncodeLon(POneWayRunways->at(POneWayRunways->size() - 2).eLongitude);
					tPoint.Type = 1;

					tPoint.Flag = 0xFF;

					PointRunway.DistToEnd = Runway.Distance;
					PointRunway.DistToCenterLine = 0;
					PointRunway.TaxiwayPoint = PTaxiwayPoints->size();
					PointRunway.Lat = POneWayRunways->at(POneWayRunways->size() - 2).eLatitude;
					PointRunway.Lon = POneWayRunways->at(POneWayRunways->size() - 2).eLongitude;
					PointRunway.Run = Runway.PrimaryDesignator * 0x1000 + Runway.PrimaryNumber;
					PTaxiwayPoints->push_back(tPoint);
					PPointsRunway->push_back(PointRunway);
					tPath.IndexStartPoint = PPointsRunway->at(maxPoint).TaxiwayPoint;
					tPath.End = PTaxiwayPoints->size() - 1;
					tPath.Type = 1; //???
					tPath.Unk1 = 0x5555;
					tPath.IndexEndPoint = Runway.PrimaryDesignator * 0x1000;
					tPath.TaxiNameIndex = 0; // Runway.PrimaryNumber;
					PTaxiwayPaths->push_back(tPath);

					tPath.IndexStartPoint = PTaxiwayPoints->size() - 1;
					tPath.End = PTaxiwayPoints->size() - 2;
					tPath.Type = 2; 
					tPath.Unk1 = 0x5555;
					tPath.IndexEndPoint = Runway.PrimaryDesignator * 0x1000;
					tPath.TaxiNameIndex = Runway.PrimaryNumber;
					PTaxiwayPaths->push_back(tPath);
				}
			}
		}
		return NOERROR;
	}
	return E_UNEXPECTED;
}


HRESULT Airport::GetTaxiwayInformation()
{
	if (Records)
	{
		for (int i = 0; i < Records->size(); i++)
		{
			if (Records->at(i).ID == 0x1A)
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayPoints)
				{
					PHTaxiwayPoints = new TaxiwayPointsHeadrer;
				}
				if (PTaxiwayPoints)
				{
					PTaxiwayPoints->clear();
				}
				else
				{
					PTaxiwayPoints = new std::vector<TaxiwayPoints>();
				}
				BGLX->BGLXFile->Read(PHTaxiwayPoints, Offset, sizeof(*PHTaxiwayPoints));
				Offset = Offset + sizeof(*PHTaxiwayPoints);
				for (int j = 0; j < PHTaxiwayPoints->CountTaxiwayPoints; j++)
				{
					TaxiwayPoints TaxiwayPoint;
					BGLX->BGLXFile->Read(&TaxiwayPoint, Offset, sizeof(TaxiwayPoint));
					PTaxiwayPoints->push_back(TaxiwayPoint);
					Offset = Offset + sizeof(TaxiwayPoint);

				}
			}
			if (Records->at(i).ID == 0xAC)
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayPoints)
				{
					PHTaxiwayPoints = new TaxiwayPointsHeadrer;
				}
				if (PTaxiwayPoints)
				{
					PTaxiwayPoints->clear();
				}
				else
				{
					PTaxiwayPoints = new std::vector<TaxiwayPoints>();
				}
				if (!PHTaxiwayPointsP3D)
				{
					PHTaxiwayPointsP3D = new TaxiwayPointsHeadrerP3D;
				}
				if (PTaxiwayPointsP3D)
				{
					PTaxiwayPoints->clear();
				}
				else
				{
					PTaxiwayPointsP3D = new std::vector<TaxiwayPointsP3D>();
				}
				BGLX->BGLXFile->Read(PHTaxiwayPointsP3D, Offset, sizeof(TaxiwayPointsP3D));
				PHTaxiwayPoints->CountTaxiwayPoints = PHTaxiwayPointsP3D->CountTaxiwayPoints;
				PHTaxiwayPoints->ID = PHTaxiwayPointsP3D->ID;
				PHTaxiwayPoints->SizeOfRecord = PHTaxiwayPointsP3D->SizeOfRecord;

				Offset = Offset + sizeof(*PHTaxiwayPointsP3D);
				for (int j = 0; j < PHTaxiwayPointsP3D->CountTaxiwayPoints; j++)
				{
					TaxiwayPoints TaxiwayPoint;					
					TaxiwayPointsP3D TaxiwayPointP3D;
					BGLX->BGLXFile->Read(&TaxiwayPointP3D, Offset, sizeof(TaxiwayPointsP3D));
					PTaxiwayPointsP3D->push_back(TaxiwayPointP3D);
					TaxiwayPoint.Flag = TaxiwayPointP3D.Flag;
					TaxiwayPoint.Lat = TaxiwayPointP3D.Lat;
					TaxiwayPoint.Lon = TaxiwayPointP3D.Lon;
					TaxiwayPoint.Type = TaxiwayPointP3D.Type;
					TaxiwayPoint.Unk1 = TaxiwayPointP3D.Unk1;
					PTaxiwayPoints->push_back(TaxiwayPoint);
					Offset = Offset + sizeof(TaxiwayPointP3D);

				}
			}
			if (Records->at(i).ID == 0x1B)
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayParks)
				{
					PHTaxiwayParks = new TaxiwayParksHeader;
				}
				if (PTaxiwayParks)
				{
					PTaxiwayParks->clear();
				}
				else
				{
					PTaxiwayParks = new std::vector<TaxiwayParks>();
				}
				BGLX->BGLXFile->Read(PHTaxiwayParks, Offset, sizeof(*PHTaxiwayParks));
				Offset = Offset + sizeof(*PHTaxiwayParks);
				for (int j = 0; j < PHTaxiwayParks->CountTaxiwayParks; j++)
				{
					TaxiwayParksFS2004 TaxiwayPark2004;
					TaxiwayParks TaxiwayPark;
					BGLX->BGLXFile->Read(&TaxiwayPark2004, Offset, sizeof(TaxiwayPark2004));

					Offset = Offset + sizeof(TaxiwayPark2004) - 8;
					WORD CountCodes = (TaxiwayPark2004.TaxiParkInfo & 0xFF000000) >> 24;
					TaxiwayPark2004.PTaxiwayParksCodes = new std::vector<TaxiwayParksCodes>();
					for (int k = 0; k < CountCodes; k++)
					{
						TaxiwayParksCodes TaxiwayParkCodes;
						BGLX->BGLXFile->Read(&TaxiwayParkCodes, Offset, sizeof(TaxiwayParkCodes));
						TaxiwayPark2004.PTaxiwayParksCodes->push_back(TaxiwayParkCodes);
						Offset = Offset + sizeof(TaxiwayParkCodes);
					}
					TaxiwayPark.Heading = TaxiwayPark2004.Heading;
					TaxiwayPark.Lat = TaxiwayPark2004.Lat;
					TaxiwayPark.Lon = TaxiwayPark2004.Lon;
					TaxiwayPark.PTaxiwayParksCodes = TaxiwayPark2004.PTaxiwayParksCodes;
					TaxiwayPark.Radius = TaxiwayPark2004.Radius;
					TaxiwayPark.TaxiParkInfo = TaxiwayPark2004.TaxiParkInfo;
					PTaxiwayParks->push_back(TaxiwayPark);
				}
			}
			if ((Records->at(i).ID == 0x1C)||(Records->at(i).ID == 0x41) || (Records->at(i).ID == 0xAE))
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayPaths)
				{
					PHTaxiwayPaths = new TaxiwayPathsHeader;
				}
				if (PTaxiwayPaths)
				{
					PTaxiwayPaths->clear();
				}
				else
				{
					PTaxiwayPaths = new std::vector<TaxiwayPaths>();
				}				
				BGLX->BGLXFile->Read(PHTaxiwayPaths, Offset, sizeof(*PHTaxiwayPaths));
				Offset = Offset + sizeof(*PHTaxiwayPaths);
				for (int j = 0; j < PHTaxiwayPaths->CountTaxiwayParks; j++)
				{
					TaxiwayPaths TaxiwayPath;
					TaxiwayPaths TaxiwayPathEmpty;
					BGLX->BGLXFile->Read(&TaxiwayPath, Offset, sizeof(TaxiwayPath) - sizeof(WORD));
					//TaxiwayPath.End = TaxiwayPath.IndexEndPoint & 0x0fff;
					TaxiwayPath.End = TaxiwayPath.End;
					if (Records->at(i).ID == 0xAE) {
						BGLX->BGLXFile->Read(&TaxiwayPathEmpty, Offset, sizeof(TaxiwayPathEmpty));
						Offset = Offset + sizeof(TaxiwayPathEmpty);
					}
					if ((TaxiwayPath.Type & 0xf) == 2) {
						//TaxiwayPath.Type = TaxiwayPath.Type & 0xf1;
					}
					PTaxiwayPaths->push_back(TaxiwayPath);

					Offset = Offset + sizeof(TaxiwayPath);

				}
			}
			if (Records->at(i).ID == 0xD4)
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayPaths)
				{
					PHTaxiwayPaths = new TaxiwayPathsHeader;
				}
				if (PTaxiwayPaths)
				{
					PTaxiwayPaths->clear();
				}
				else
				{
					PTaxiwayPaths = new std::vector<TaxiwayPaths>();
				}
				if (PTaxiwayPathsMSFS)
				{
					PTaxiwayPathsMSFS->clear();
				}
				else
				{
					PTaxiwayPathsMSFS = new std::vector<TaxiwayPathsMSFS>();
				}
				BGLX->BGLXFile->Read(PHTaxiwayPaths, Offset, sizeof(*PHTaxiwayPaths));
				Offset = Offset + sizeof(*PHTaxiwayPaths);
				for (int j = 0; j < PHTaxiwayPaths->CountTaxiwayParks; j++)
				{
					TaxiwayPaths TaxiwayPath;
					TaxiwayPathsMSFS TaxiwayPathMSFS;
					BGLX->BGLXFile->Read(&TaxiwayPathMSFS, Offset, sizeof(TaxiwayPathMSFS));
					PTaxiwayPathsMSFS->push_back(TaxiwayPathMSFS);
					Offset = Offset + sizeof(TaxiwayPathMSFS);
					TaxiwayPath.Dashed = TaxiwayPathMSFS.Dashed;
					//TaxiwayPath.IndexEndPoint = TaxiwayPathMSFS.IndexEndPoint;
					TaxiwayPath.IndexEndPoint = TaxiwayPathMSFS.LegacyEnd;
					TaxiwayPath.End = TaxiwayPathMSFS.IndexEndPoint;
					TaxiwayPath.IndexStartPoint = TaxiwayPathMSFS.IndexStartPoint;
					TaxiwayPath.Surface = TaxiwayPathMSFS.Surface;
					TaxiwayPath.TaxiNameIndex = TaxiwayPathMSFS.TaxiNameIndex;
					TaxiwayPath.Type = TaxiwayPathMSFS.Type;
					TaxiwayPath.Unk1 = TaxiwayPathMSFS.Unk1;
					TaxiwayPath.WeightLimit = TaxiwayPathMSFS.WeightLimit;
					TaxiwayPath.Width = TaxiwayPathMSFS.Width;
					if ((TaxiwayPath.Type & 0xf) == 2) {
						//TaxiwayPath.Type = TaxiwayPath.Type & 0xf1;
					}
					PTaxiwayPaths->push_back(TaxiwayPath);
				}
			}
			if (Records->at(i).ID == 0x1D)
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayNames)
				{
					PHTaxiwayNames = new TaxiwayNamesHeader;
				}
				if (PTaxiwayNames)
				{
					PTaxiwayNames->clear();
				}
				else
				{
					PTaxiwayNames = new std::vector<TaxiwayNames>();
				}
				BGLX->BGLXFile->Read(PHTaxiwayNames, Offset, sizeof(*PHTaxiwayNames));
				Offset = Offset + sizeof(*PHTaxiwayNames);
				for (int j = 0; j < PHTaxiwayNames->CountTaxiwayNames; j++)
				{
					TaxiwayNames TaxiwayName;
					BGLX->BGLXFile->Read(&TaxiwayName, Offset, sizeof(TaxiwayName));
					PTaxiwayNames->push_back(TaxiwayName);
					Offset = Offset + sizeof(TaxiwayName);

				}
			}
			if (Records->at(i).ID == 0xE7) {
				DWORD Offset = Records->at(i).Offset;

				if (!PHTaxiwayParksMSFS)
				{
					PHTaxiwayParksMSFS = new TaxiwayParksHeaderMSFS;
				}
				if (PTaxiwayParksMSFS)
				{
					PTaxiwayParksMSFS->clear();
				}
				else
				{
					PTaxiwayParksMSFS = new std::vector<TaxiwayParksMSFS>();
				}

				if (!PHTaxiwayParks)
				{
					PHTaxiwayParks = new TaxiwayParksHeader;
				}
				if (PTaxiwayParks)
				{
					PTaxiwayParks->clear();
				}
				else
				{
					PTaxiwayParks = new std::vector<TaxiwayParks>();
				}


				BGLX->BGLXFile->Read(PHTaxiwayParks, Offset, sizeof(*PHTaxiwayParks));
				Offset = Offset + sizeof(*PHTaxiwayParks);
				for (int j = 0; j < PHTaxiwayParks->CountTaxiwayParks; j++)
				{
					TaxiwayParksMSFS TaxiwayParkMSFS;
					TaxiwayParks TaxiwayPark;
					BGLX->BGLXFile->Read(&TaxiwayParkMSFS, Offset, sizeof(TaxiwayParkMSFS));

					Offset = Offset + sizeof(TaxiwayParkMSFS) - 8;
					WORD CountCodes = (TaxiwayParkMSFS.TaxiParkInfo & 0xFF000000) >> 24;
					TaxiwayParkMSFS.PTaxiwayParksCodes = new std::vector<TaxiwayParksCodes>();
					for (int k = 0; k < CountCodes; k++)
					{
						TaxiwayParksCodes TaxiwayParkCodes;
						BGLX->BGLXFile->Read(&TaxiwayParkCodes, Offset, sizeof(TaxiwayParkCodes));
						TaxiwayParkMSFS.PTaxiwayParksCodes->push_back(TaxiwayParkCodes);
						Offset = Offset + sizeof(TaxiwayParkCodes);
					}
					PTaxiwayParksMSFS->push_back(TaxiwayParkMSFS);
					TaxiwayPark.Heading = TaxiwayParkMSFS.Heading;
					TaxiwayPark.Lat = TaxiwayParkMSFS.Lat;
					TaxiwayPark.Lon = TaxiwayParkMSFS.Lon;
					TaxiwayPark.PTaxiwayParksCodes = TaxiwayParkMSFS.PTaxiwayParksCodes;
					TaxiwayPark.Radius = TaxiwayParkMSFS.Radius;
					TaxiwayPark.TaxiParkInfo = TaxiwayParkMSFS.TaxiParkInfo;
					TaxiwayPark.TeeOffset1 = TaxiwayParkMSFS.TeeOffset1;
					TaxiwayPark.TeeOffset2 = TaxiwayParkMSFS.TeeOffset2;
					TaxiwayPark.TeeOffset3 = TaxiwayParkMSFS.TeeOffset3;
					TaxiwayPark.TeeOffset4 = TaxiwayParkMSFS.TeeOffset4;
					PTaxiwayParks->push_back(TaxiwayPark);
				}
			}
			if (Records->at(i).ID == 0x3D)
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayParks)
				{
					PHTaxiwayParks = new TaxiwayParksHeader;
				}
				if (PTaxiwayParks)
				{
					PTaxiwayParks->clear();
				}
				else
				{
					PTaxiwayParks = new std::vector<TaxiwayParks>();
				}
				BGLX->BGLXFile->Read(PHTaxiwayParks, Offset, sizeof(*PHTaxiwayParks));
				Offset = Offset + sizeof(*PHTaxiwayParks);
				for (int j = 0; j < PHTaxiwayParks->CountTaxiwayParks; j++)
				{
					TaxiwayParks TaxiwayPark;
					BGLX->BGLXFile->Read(&TaxiwayPark, Offset, sizeof(TaxiwayPark));
					
					Offset = Offset + sizeof(TaxiwayPark) - 8;
					WORD CountCodes = (TaxiwayPark.TaxiParkInfo & 0xFF000000) >> 24;
					TaxiwayPark.PTaxiwayParksCodes = new std::vector<TaxiwayParksCodes>();
					for (int k = 0; k < CountCodes; k++)
					{
						TaxiwayParksCodes TaxiwayParkCodes;
						BGLX->BGLXFile->Read(&TaxiwayParkCodes, Offset, sizeof(TaxiwayParkCodes));
						TaxiwayPark.PTaxiwayParksCodes->push_back(TaxiwayParkCodes);
						Offset = Offset + sizeof(TaxiwayParkCodes);
					}
					PTaxiwayParks->push_back(TaxiwayPark);
				}
			}

			if (Records->at(i).ID == 0xAD)
			{
				DWORD Offset = Records->at(i).Offset;
				if (!PHTaxiwayParksP3D)
				{
					PHTaxiwayParksP3D = new TaxiwayParksHeaderP3D;
				}
				if (PTaxiwayParksP3D)
				{
					PTaxiwayParksP3D->clear();
				}
				else
				{
					PTaxiwayParksP3D = new std::vector<TaxiwayParksP3D>();
				}

				if (!PHTaxiwayParks)
				{
					PHTaxiwayParks = new TaxiwayParksHeader;
				}
				if (PTaxiwayParks)
				{
					PTaxiwayParks->clear();
				}
				else
				{
					PTaxiwayParks = new std::vector<TaxiwayParks>();
				}
				BGLX->BGLXFile->Read(PHTaxiwayParks, Offset, sizeof(*PHTaxiwayParks));
				Offset = Offset + sizeof(*PHTaxiwayParks);
				for (int j = 0; j < PHTaxiwayParks->CountTaxiwayParks; j++)
				{
					TaxiwayParks TaxiwayPark;
					TaxiwayParksP3D TaxiwayParkP3D;
					BGLX->BGLXFile->Read(&TaxiwayParkP3D, Offset, sizeof(TaxiwayParkP3D));

					Offset = Offset + sizeof(TaxiwayParkP3D) - 8;
					WORD CountCodes = (TaxiwayParkP3D.TaxiParkInfo & 0xFF000000) >> 24;
					TaxiwayParkP3D.PTaxiwayParksCodes = new std::vector<TaxiwayParksCodes>();
					for (int k = 0; k < CountCodes; k++)
					{
						TaxiwayParksCodes TaxiwayParkCodes;
						BGLX->BGLXFile->Read(&TaxiwayParkCodes, Offset, sizeof(TaxiwayParkCodes));
						TaxiwayPark.PTaxiwayParksCodes->push_back(TaxiwayParkCodes);
						Offset = Offset + sizeof(TaxiwayParkCodes);
					}
					PTaxiwayParksP3D->push_back(TaxiwayParkP3D);
					TaxiwayPark.Heading = TaxiwayParkP3D.Heading;
					TaxiwayPark.Lat = TaxiwayParkP3D.Lat;
					TaxiwayPark.Lon = TaxiwayParkP3D.Lon;
					TaxiwayPark.PTaxiwayParksCodes = TaxiwayParkP3D.PTaxiwayParksCodes;
					TaxiwayPark.Radius = TaxiwayParkP3D.Radius;
					TaxiwayPark.TaxiParkInfo = TaxiwayParkP3D.TaxiParkInfo;
					TaxiwayPark.TeeOffset1 = TaxiwayParkP3D.TeeOffset1;
					TaxiwayPark.TeeOffset2 = TaxiwayParkP3D.TeeOffset2;
					TaxiwayPark.TeeOffset3 = TaxiwayParkP3D.TeeOffset3;
					TaxiwayPark.TeeOffset4 = TaxiwayParkP3D.TeeOffset4;
					PTaxiwayParks->push_back(TaxiwayPark);
				}
			}
			
		}
		return NOERROR;
	}
}


Airport::~Airport()
{
	if (BGLX && mode)
	{
		delete BGLX;
		BGLX = NULL;
	}
	if (PAirportInformation)
	{
		delete PAirportInformation;
		PAirportInformation = NULL;
	}
	if (PAddAirportInformation)
	{
		delete PAddAirportInformation;
		PAddAirportInformation = NULL;
	}
	if (Records)
	{
		delete Records;
		Records = NULL;
	}
}



