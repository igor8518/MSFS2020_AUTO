#include "WayLoader.h"
/*#include "PMDG_NGX.h"
#include "WayLoader.h"
#include "AirportData.h"
#include <algorithm>
#include <msclr/marshal_cppstd.h>

namespace PMDG_TEST {
  using namespace System;
  using namespace System::Text::RegularExpressions;
  using namespace msclr::interop;


  std::string AddSID(IAirportData* PAirportData, aircraftData* PaircraftData, TPath* RWEnd, double RWHead) {
    typedef struct TSIDS {
      int t = 0;
      int j = 0;
      std::vector<WayPointA>* wayPoint = new std::vector<WayPointA>();
      std::string name = "";
    } ;
    std::vector<TSIDS>* SIDss = new std::vector<TSIDS>();
    std::string SIDName = "";
    std::vector<WayPointA>* wayPoint = new std::vector<WayPointA>();
    int j, r, t;
    bool FindSid = false;
    for (j = 0; j < PAirportData->GetAirport()->sidstar->SIDS->size(); j++) {
      for (r = 0; r < PAirportData->GetAirport()->sidstar->SIDS->at(j).Runways->size(); r++) {
        if (PAirportData->GetAirport()->sidstar->SIDS->at(j).Runways->at(r) == RWEnd->name) {
          if (PAirportData->GetAirport()->sidstar->SIDS->at(j).WayPoints->at(PAirportData->GetAirport()->sidstar->SIDS->at(j).WayPoints->size() - 1).FIXName == WP->at(1).ICAO) {
            t = -1;
            FindSid = true;
            break;
          }
          else {
            if (PAirportData->GetAirport()->sidstar->SIDS->at(j).TRANSITIONS != NULL) {
              for (t = 0; t < PAirportData->GetAirport()->sidstar->SIDS->at(j).TRANSITIONS->size(); t++) {
                if (PAirportData->GetAirport()->sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->at(PAirportData->GetAirport()->sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->size() - 1).FIXName == WP->at(1).ICAO) {
                  FindSid = true;
                  break;
                }
              }
            }
          }
        }
      }
      if (FindSid) {
        for (int i = 0; i < PAirportData->GetAirport()->sidstar->SIDS->at(j).WayPoints->size(); i++) {
          wayPoint->push_back(PAirportData->GetAirport()->sidstar->SIDS->at(j).WayPoints->at(i));
          wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
        }
        if (t >= 0) {
          for (int i = 0; i < PAirportData->GetAirport()->sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->size(); i++) {
            wayPoint->push_back(PAirportData->GetAirport()->sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->at(i));
            wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
          }
        }
        SIDName = PAirportData->GetAirport()->sidstar->SIDS->at(j).Name;
        SIDss->push_back({ t, j, wayPoint, SIDName });
        FindSid = false;
        break;//
      }
    }
    if ((!FindSid) && (SIDss->size() == 0)) {
      AddWayPoint(RWEnd->Lon, RWEnd->Lat, 1750, HEADINGUNTILALT, "(1750)", RWHead - PaircraftData->MagVar, 0, 0.0);
      AddWayPoint(WP->at(1).lon, WP->at(1).lat, CruiseLevel, FIX, WP->at(1).ICAO);
    }
    if (SIDss->size() > 1) {
      for (int i = 0; i < SIDss->at(0).wayPoint->size(); i++) {
        FIXX fixx = {};
        bool FindFIX = false;
        for (int fix = 0; fix < PAirportData->GetAirport()->sidstar->FIXES->size(); fix++) {
          if (PAirportData->GetAirport()->sidstar->FIXES->at(fix).Name == wayPoint->at(i).FIXName) {
            FindFIX = true;
            fixx = PAirportData->GetAirport()->sidstar->FIXES->at(fix);
            break;
          }
        }
        AddSidStarTrack(&SIDss->at(0).wayPoint->at(i), RWEnd, &fixx);
      }
    }
    else {
    for (int i = 0; i < wayPoint->size(); i++) {
        FIXX fixx = {};
        bool FindFIX = false;
        for (int fix = 0; fix < PAirportData->GetAirport()->sidstar->FIXES->size(); fix++) {
          if (PAirportData->GetAirport()->sidstar->FIXES->at(fix).Name == wayPoint->at(i).FIXName) {
            FindFIX = true;
            fixx = PAirportData->GetAirport()->sidstar->FIXES->at(fix);
            break;
          }
        }
        AddSidStarTrack(&wayPoint->at(i), RWEnd, &fixx);
      }
    }
    delete(wayPoint);
    wayPoint = NULL;
    return SIDName;
  }




  std::string AddSTAR(IAirportData* PAirportData, aircraftData* PaircraftData, DATA_RUNWAY* runway) {
    std::string STARName = "";
    std::vector<WayPointA>* wayPoint = new std::vector<WayPointA>();
    int i, j, j1, r, r2, t, t2;
    bool FindStar = false;
    bool FindAppr = false;
    for (j = 0; j < PAirportData->GetAirport()->sidstar->APPROACHES->size(); j++) {
      for (r = 0; r < PAirportData->GetAirport()->sidstar->APPROACHES->at(j).Runways->size(); r++) {
        if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).Runways->at(r) == runway->Name) {
          if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).WayPoints->at(0).FIXName == WP->at(WP->size() - 2).ICAO) {
            t = -1;
            FindAppr = true;
            break;
          }
          else {
            if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS != NULL) {
              for (t = 0; t < PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS->size(); t++) {
                if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(0).FIXName == WP->at(WP->size() - 2).ICAO) {
                    FindAppr = true;
                  break;
                }
              }
            }
          }
        }
      }
      if (FindAppr) {
        break;
      }
    }
    if (!FindAppr) {
      for (i = 0; i < PAirportData->GetAirport()->sidstar->STARS->size(); i++) {
        for (r2 = 0; r2 < PAirportData->GetAirport()->sidstar->STARS->at(i).Runways->size(); r2++) {
          if (PAirportData->GetAirport()->sidstar->STARS->at(i).Runways->at(r2) == runway->Name) {
            if (PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(0).FIXName == WP->at(WP->size() - 2).ICAO) {
              t2 = -1;
              for (j = 0; j < PAirportData->GetAirport()->sidstar->APPROACHES->size(); j++) {
                for (r = 0; r < PAirportData->GetAirport()->sidstar->APPROACHES->at(j).Runways->size(); r++) {
                  if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).Runways->at(r) == runway->Name) {
                    std::string ss1 = PAirportData->GetAirport()->sidstar->APPROACHES->at(j).WayPoints->at(0).FIXName;
                    std::string ss2 = PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->size() - 1).FIXName;
                    if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).WayPoints->at(0).FIXName ==
                      PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->size() - 1).FIXName) {
                      t = -1;
                      FindAppr = true;
                      FindStar = true;
                      break;
                    }
                    else {
                      if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS != NULL) {
                        for (t = 0; t < PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS->size(); t++) {
                          if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(0).FIXName ==
                            PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->size() - 1).FIXName) {
                            FindAppr = true;
                            FindStar = true;
                            break;
                          }
                        }
                      }
                    }
                  }
                }
                if (FindAppr) {
                  break;
                }
              }
              break;
            }
            else {
              if (PAirportData->GetAirport()->sidstar->STARS->at(i).TRANSITIONS != NULL) {
                for (t2 = 0; t2 < PAirportData->GetAirport()->sidstar->STARS->at(i).TRANSITIONS->size(); t2++) {
                  if (PAirportData->GetAirport()->sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->at(0).FIXName == WP->at(WP->size() - 2).ICAO) {
                    for (j1 = 0; j1 < PAirportData->GetAirport()->sidstar->APPROACHES->size(); j1++) {
                      for (r = 0; r < PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).Runways->size(); r++) {
                        if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).Runways->at(r) == runway->Name) {
                          if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).WayPoints->at(0).FIXName ==
                            PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->size() - 1).FIXName) {
                            t = -1;
                            FindAppr = true;
                            FindStar = true;
                            j = j1;
                            break;
                          }
                          else {
                            if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).TRANSITIONS != NULL)
                            {
                              for (t = 0; t < PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).TRANSITIONS->size(); t++)

                              {
                                std::string ss1 = PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).TRANSITIONS->at(t).WayPoints->at(0).FIXName;
                                std::string ss2 = PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->size() - 1).FIXName;
                                if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).TRANSITIONS != NULL)
                                {
                                  if (PAirportData->GetAirport()->sidstar->APPROACHES->at(j1).TRANSITIONS->at(t).WayPoints->at(0).FIXName ==
                                    PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->size() - 1).FIXName)
                                  {
                                    FindAppr = true;
                                    FindStar = true;
                                    j = j1;
                                    break;
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                      if (FindAppr) {
                        break;
                      }
                    }
                    break;
                  }
                }
              }
            }
          }
        }
        if (FindStar) {
          break;
        }
      }
    }
    if (FindStar) {
      if (t2 >= 0) {
        for (int ii = 0; ii < PAirportData->GetAirport()->sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->size(); ii++) {
          wayPoint->push_back(PAirportData->GetAirport()->sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->at(ii));
          wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
        }
      }
      for (int ii = 0; ii < PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->size() - 1; ii++) {
        wayPoint->push_back(PAirportData->GetAirport()->sidstar->STARS->at(i).WayPoints->at(ii));
        wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
      }
      STARName = PAirportData->GetAirport()->sidstar->STARS->at(i).Name;
    }
    if (FindAppr) {
      if (t >= 0) {
        for (int ii = 0; ii < PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->size(); ii++) {
          wayPoint->push_back(PAirportData->GetAirport()->sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(ii));
          wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
        }
      }
      AppPoint = wayPoint->size();
      for (int ii = 0; ii < PAirportData->GetAirport()->sidstar->APPROACHES->at(j).WayPoints->size(); ii++) {
        wayPoint->push_back(PAirportData->GetAirport()->sidstar->APPROACHES->at(j).WayPoints->at(ii));
        wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
      }
      STARName = STARName + " - " + PAirportData->GetAirport()->sidstar->APPROACHES->at(j).Name;
    }
    else {
      SIMCONNECT_DATA_LATLONALT latlon = SIMMATH::GetDALatLon(runway->sLatitude, runway->sLongitude, runway->eHeading, 18.0);
      AddWayPoint(latlon.Longitude, latlon.Latitude, WP->at(WP->size() - 1).alt + 2000.0, FIX, "GS 18.0 " + runway->Name, 0, FIXALT);
      AddWayPoint(runway->sLongitude, runway->sLatitude, WP->at(WP->size() - 1).alt + 2000.0, WAYPOINT, "FINAL " + runway->Name, 0, FIXALT);
      AddWayPoint(runway->sLongitude, runway->sLatitude, WP->at(WP->size() - 1).alt, GSWAY, runway->Name, 0, FIXALT);
      AddWayPoint(runway->eLongitude, runway->eLatitude, WP->at(WP->size() - 1).alt, RUNWAY, runway->Name, 0, FIXALT);
      STARName = "GS 18.0 ";
    }
    for (int i = 0; i < wayPoint->size(); i++) {
      FIXX fixx = {};
      bool FindFIX = false;
      for (int fix = 0; fix < PAirportData->GetAirport()->sidstar->FIXES->size(); fix++) {
        if (PAirportData->GetAirport()->sidstar->FIXES->at(fix).Name == wayPoint->at(i).FIXName) {
          FindFIX = true;
          fixx = PAirportData->GetAirport()->sidstar->FIXES->at(fix);
          break;
        }
      }
      if (i == AppPoint) {
        AppPoint = CountWays;
      }
      AddSidStarTrack(&wayPoint->at(i), NULL, &fixx, runway);
    }
    delete(wayPoint);
    wayPoint = NULL;
    return STARName;
  }




  void AddSidStarTrack(WayPointA* wayPoint, TPath* RWEnd, FIXX* fixx, DATA_RUNWAY* runway) {
    if (wayPoint->TypeName == "TRKUNTIL") {
      if (RWEnd == NULL) {
        AddWayPoint(runway->eLongitude, runway->eLatitude, wayPoint->ALT, HEADINGUNTILALT, std::string("(" + std::to_string((DWORD)wayPoint->ALT) + ")"), wayPoint->TRK, 0, 0.0);
      }
      else {
        AddWayPoint(RWEnd->Lon, RWEnd->Lat, wayPoint->ALT, HEADINGUNTILALT, std::string("(" + std::to_string((DWORD)wayPoint->ALT) + ")"), wayPoint->TRK, 0, 0.0);
      }
    }
    else if (wayPoint->TypeName == "TRKINTERCEPTRADIALTOFIX") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TRKINTERCEPTRADIAL, "(INTRAD) " + wayPoint->FIXName, wayPoint->TRK, 0, 0.0, wayPoint->RADIAL);
    }
    else if (wayPoint->TypeName == "FIXATORABOVE") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORABOVE, 0.0);
    }
    else if (wayPoint->TypeName == "FIXOVERFLYATORABOVE") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORABOVE, 0.0);
    }
    else if (wayPoint->TypeName == "FIXATORBELOW") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORBELOW, 0.0);
    }
    else if (wayPoint->TypeName == "FIXATORBELOWATORABOVE") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORBELOW, 0.0);
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORABOVE, 0.0);
    }
    else if (wayPoint->TypeName == "FIXATORBELOWATORABOVE") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORBELOW, 0.0);
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORABOVE, 0.0);
    }
    else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXATORBELOWATORABOVE") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TURNRIGHTDIRECTFIX, wayPoint->FIXName, wayPoint->TRK, ALTORABOVE, 0.0);
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TURNRIGHTDIRECTFIX, wayPoint->FIXName, wayPoint->TRK, ALTORBELOW, 0.0);
    }
    else if (wayPoint->TypeName == "FIX") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, 0, 0.0);
    }
    else if (wayPoint->TypeName == "FIXSPEED") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, 0, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "FIXOVERFLY") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, 0, 0.0);
    }
    else if (wayPoint->TypeName == "FIXFALT") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, FIXALT, 0.0);
    }
    else if (wayPoint->TypeName == "FIXFALTSPEED") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, FIXALT, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "FIXOVERFLYFALT") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, FIXALT, 0.0);
    }
    else if (wayPoint->TypeName == "RNW") {
      AddWayPoint(runway->sLongitude, runway->sLatitude, WP->at(WP->size() - 1).alt, GSWAY, runway->Name, 0, FIXALT);
      AddWayPoint(runway->eLongitude, runway->eLatitude, WP->at(WP->size() - 1).alt, RUNWAY, runway->Name, 0, FIXALT);
    }
    else if (wayPoint->TypeName == "FIXOVERFLYATORABOVESPEED") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, ALTORABOVE, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEEDLEGTIME") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, FIXALT, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEED") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, wayPoint->TYPE, wayPoint->FIXName, wayPoint->TRK, FIXALT, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNLEFTDIRECTFIX") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TURNLEFTDIRECTFIX, wayPoint->FIXName, 0, 0, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIX") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TURNRIGHTDIRECTFIX, wayPoint->FIXName, 0, 0, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXOVERFLYFALT") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TURNRIGHTDIRECTFIX, wayPoint->FIXName, 0, FIXALT, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXOVERFLYFALT") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TURNLEFTDIRECTFIX, wayPoint->FIXName, 0, FIXALT, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXOVERFLY") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TURNLEFTDIRECTFIX, wayPoint->FIXName, 0, 0, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXFALT") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TURNRIGHTDIRECTFIX, wayPoint->FIXName, 0, FIXALT, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXFALT") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TURNLEFTDIRECTFIX, wayPoint->FIXName, 0, FIXALT, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXATORABOVE") {
      AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TURNLEFTDIRECTFIX, wayPoint->FIXName, 0, ALTORABOVE, wayPoint->SPEED);
    }
    else if (wayPoint->TypeName == "TRKUNTILFROMFIXSPEED") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, HEADINGUNTILFROMDIST, wayPoint->FIXName, wayPoint->TRK, 0, wayPoint->SPEED, wayPoint->LegDist);
    }
    else if (wayPoint->TypeName == "TRKUNTILFROMFIX") {
      AddWayPoint(fixx->Lon, fixx->Lat, 0.0, HEADINGUNTILFROMDIST, wayPoint->FIXName, wayPoint->TRK, 0, 0.0, wayPoint->LegDist);
    }
    else {
      std::cout << wayPoint->TypeName;
    }
  }




  void FillWays() {
      Stream^ myStream;
      OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;
      int countWP = 0;
      StreamReader^ sr;
      openFileDialog1->Filter = "PMDG rte files (*.rte)|*.rte|All files (*.*)|*.*";
      openFileDialog1->FilterIndex = 1;
      openFileDialog1->RestoreDirectory = true;
      if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        try {   // Open the text file using a stream reader.
          sr = gcnew StreamReader(openFileDialog1->FileName);
        WP = new std::vector<WayPoint>();
        while (!sr->EndOfStream) {
          String^ line = sr->ReadLine();
          try {
            countWP = Convert::ToInt32(line);
            WayPoint wp;
            for (int i = 0; i < countWP; i++) {
              int j = 0;
              line = sr->ReadLine();
              while (line == "") {
                line = sr->ReadLine();
              }
              wp.ICAO = marshal_as<std::string>(line);
              line = sr->ReadLine();
              wp.Type = Convert::ToInt32(line);
              line = sr->ReadLine();
              wp.AirWay = marshal_as<std::string>(line);
              line = sr->ReadLine();
              String^ LatLon = line;
              Regex^ re = gcnew Regex(
                "(\\d+)\\s*([NS])\\s*(\\d+\\.\\d*)\\s*([EW])\\s*(\\d+\\.\\d*)"
                "\\s*(-?\\d+)",
                (RegexOptions)
                (RegexOptions::CultureInvariant
                  | RegexOptions::Compiled
                  ));
              Match^ ms = re->Match(line);
              if (ms->Groups[2]->Value == "S") {
                wp.lat = -Convert::ToDouble(ms->Groups[3]->Value);
              }
              else {
                wp.lat = Convert::ToDouble(ms->Groups[3]->Value);
              }
              if (ms->Groups[4]->Value == "W") {
                wp.lon = -Convert::ToDouble(ms->Groups[5]->Value);
              }
              else {
                wp.lon = Convert::ToDouble(ms->Groups[5]->Value);
              }
              wp.alt = Convert::ToDouble(ms->Groups[6]->Value);
              if (wp.Type == 1) {
                j = 10;
              }
              else {
                j = 4;
              }
              for (int k = 0; k < j; k++) {
                line = sr->ReadLine();
              }
              WP->push_back(wp);
            }
          }
          catch (Exception ^ e) {
            continue;
          }
        }
      }
      catch (Exception ^ e) {
      }
      sr->Close();
    }
  }




  void AddWayPoint(double lon, double lat, double alt, int pointType, std::string PointName, double heading, int fixAlt, double speed, double radial) {
    static int point = 0;
    if ((point == 0) && (CountWays == 0)) {
      Ways[CountWays] = new SIMMATH::DSHEH;
      Ways[CountWays]->Slla.Longitude = lon;
      Ways[CountWays]->Slla.Latitude = lat;
      Ways[CountWays]->Slla.Altitude = alt;
      point = 1;
    }
    else if ((point == 1) && (CountWays == 0)) {
      Ways[CountWays]->Ella.Longitude = lon;
      Ways[CountWays]->Ella.Latitude = lat;
      Ways[CountWays]->Ella.Altitude = alt;
      Ways[CountWays]->typePath = pointType;
      Ways[CountWays]->nameEndPoint = PointName;
      Ways[CountWays]->fixAlt = fixAlt;
      Ways[CountWays]->Speed = speed;
      SIMMATH::DOrtoKM(Ways[CountWays]);
      if ((pointType == FIXHEADING) || (pointType == HEADINGUNTILALT)) {
        Ways[CountWays]->H = heading;
      }
      if (pointType == HEADINGUNTILFROMDIST) {
        Ways[CountWays]->Dist = radial;
        Ways[CountWays]->H = heading;
      }
      if (pointType == TRKINTERCEPTRADIAL) {
        Ways[CountWays]->Dist = radial;
        Ways[CountWays]->H = heading;
      }
      CountWays++;
      point = 0;
    }
    else if (CountWays != 0) {
      Ways[CountWays] = new SIMMATH::DSHEH;
      Ways[CountWays]->Slla.Longitude = Ways[CountWays - 1]->Ella.Longitude;
      Ways[CountWays]->Slla.Latitude = Ways[CountWays - 1]->Ella.Latitude;
      Ways[CountWays]->Slla.Altitude = Ways[CountWays - 1]->Ella.Altitude;
      Ways[CountWays]->Ella.Longitude = lon;
      Ways[CountWays]->Ella.Latitude = lat;
      Ways[CountWays]->Ella.Altitude = alt;
      Ways[CountWays]->typePath = pointType;
      Ways[CountWays]->nameEndPoint = PointName;
      Ways[CountWays]->fixAlt = fixAlt;
      Ways[CountWays]->Speed = speed;
      SIMMATH::DOrtoKM(Ways[CountWays]);
      if ((pointType == FIXHEADING) || (pointType == HEADINGUNTILALT)) {
        Ways[CountWays]->H = heading;
      }
      if (pointType == HEADINGUNTILFROMDIST) {
        Ways[CountWays]->Dist = radial;
        Ways[CountWays]->H = heading;
      }
      if (pointType == TRKINTERCEPTRADIAL) {
        Ways[CountWays]->Dist = radial;
        Ways[CountWays]->H = heading;
      }
      CountWays++;
    }
  }
}
*/

void FillWays()
{
}
