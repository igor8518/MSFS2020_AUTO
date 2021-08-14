//------------------------------------------------------------------------------
//
//  PMDG 737NGX external connection sample 
// 
//------------------------------------------------------------------------------
#include "mainlogic.h"
#include <winerror.h>



double MainLogic::GetData(DWORD var, char* unit) {
	double lVar;
	emit GetDataSignal(MAINLOGIC_ID, var, &lVar, unit);
	while (!GetDataChanged);
	GetDataChanged = false;
	return lVar;
}

double MainLogic::GetDataL(DWORD var, char* unit) {
	double lVar;
	emit GetDataSignalL(MAINLOGIC_ID, var, &lVar, unit);
	while (!GetDataChanged);
	GetDataChanged = false;
	return lVar;
}

double MainLogic::SetData(DWORD var, double val, char* unit) {
	emit SetDataSignal(MAINLOGIC_ID, var, &val, unit);
	while (!SetDataChanged);
	SetDataChanged = false;
	return  val;
}

void MainLogic::GetDataChange(DWORD var, DWORD sender) {
	if (sender == MAINLOGIC_ID) {
		GetDataChanged = true;
	}
}
void MainLogic::SetDataChange(DWORD var, DWORD sender) {
	if (sender == MAINLOGIC_ID) {
		SetDataChanged = true;
	}
}

void MainLogic::SetGetDataChange(DWORD var, DWORD sender)
{
}

DWORD MainLogic::SendEvent(DWORD EventID, DWORD dwData)
{
	emit SendEventSignal(MAINLOGIC_ID, EventID, dwData);
	return 0;
}

void MainLogic::Disconnect() {
	
	emit ButtonModify(ui->ConnectButton, "Disconnected...", "background: lightblue; color: darkblue; border: 1px solid darkblue");
	SUCC(SimConnect_Close(HSimConnect));
	emit ButtonModify(ui->ConnectButton, "Connect", " background: " + DefColorBgrnd.name() + " color : " + DefColorTxt.name() + " border : 1px solid " + DefColorTxt.name());
	Connected = FALSE;
	emit SendLog("Disconect");
}




Ui::MSFS2020_AutoFlightClass* MainLogic::GetUi()
{
	return ui;
}



MainLogic::MainLogic(PlanesWork* planesWork, MSFS2020_AutoFlight* mainObject, QObject* parent) : QObject(parent) {
	this->planesWork = planesWork;
	qRegisterMetaType<DWORD>("DWORD");
	this->ui = mainObject->GetUi();
	utils = new Utils(mainObject, &HSimConnect);
	TUtils = new ExQThread("utils", utils);
	utils->moveToThread(TUtils);
	TUtils->start();
	MainTimer = new QTimer(this);
	ObjectConnectors = new std::vector<QMetaObject::Connection>();
	ObjectConnectors->push_back(connect(this, SIGNAL(SendText(QString, bool)), utils, SLOT(SetText(QString, bool))));
	ObjectConnectors->push_back(connect(this, SIGNAL(SendLog(QString)), utils, SLOT(AddLog(QString))));
	ObjectConnectors->push_back(connect(this, SIGNAL(ButtonModify(QPushButton*, QString, QString)), mainObject, SLOT(ButtonModify(QPushButton*, QString, QString))));
	emit SendLog("READY");
	//Mode = START;
}



MainLogic::~MainLogic()
{
	for (int i = 0; i < ObjectConnectors->size(); i++) {
		disconnect(ObjectConnectors->at(i));
	}
	TUtils->exit();
	TUtils->wait();
	delete TUtils;
	delete utils;
	delete ObjectConnectors;
	delete MainTimer;
}

void MainLogic::SetMode(DWORD mode) {
	Mode = mode;
}

std::string MainLogic::GetDataString(DWORD var) {
	std::string lVar;
	emit GetDataStringSignal(MAINLOGIC_ID, var, &lVar);
	while (!GetDataChanged);
	GetDataChanged = false;
	return lVar;
}

void MainLogic::AddWayPoint(double lon, double lat, double alt, QString pointType, QString PointName, double heading, int fixAlt, double speed, double radial, QJsonObject* fix) {
	if (fix != NULL) {
		if (fix->value("ident").toString() == "TOC") {
			return;
		}
		if (fix->value("ident").toString() == "TOD") {
			return;
		}
	}
	static int point = 0;
	sWayPoint sw;
	if ((point == 0) && (WayPoints->size() == 0)) {
		//Ways[CountWays] = new SIMMATH::DSHEH;
		sw.Lon = lon;
		sw.Lat = lat;
		sw.Altitude = alt;
		if (fix != NULL) {
			sw.AirWay = fix->value("via_airway").toString();
			sw.Ident = fix->value("ident").toString();
			sw.Name = fix->value("name").toString();
			//sw.Type = fix->value("type").toString();
			sw.Type = pointType;
			sw.Lat = fix->value("pos_lat").toString().toDouble();
			sw.Lon = fix->value("pos_long").toString().toDouble();
			sw.Stage = fix->value("stage").toString();
			sw.SidStar = (bool)fix->value("is_sid_star").toString().toInt();
			sw.Distance = fix->value("distance").toString().toDouble();
			sw.TrackTrue = fix->value("track_true").toString().toDouble();
			sw.TrackMag = fix->value("track_mag").toString().toDouble();
			sw.HeadingTrue = fix->value("heading_true").toString().toDouble();
			sw.HeadingMag = fix->value("heading_mag").toString().toDouble();
			if (alt != 0) {
				sw.Altitude = alt;
			}
			else {
				sw.Altitude = fix->value("altitude_feet").toString().toDouble();
			}

			//sw.IndSpeed = fix->value("ind_airspeed").toString().toDouble();
			sw.IndSpeed = speed;
			sw.TrueSpeed = fix->value("true_airspeed").toString().toDouble();
			sw.Mach = fix->value("mach").toString().toDouble();
			sw.MachThousandths = fix->value("mach_thousandths").toString().toDouble();
			sw.WindComponent = fix->value("wind_component").toString().toDouble();
			sw.GroundSpeed = fix->value("groundspeed").toString().toDouble();
			sw.TimeLeg = fix->value("time_leg").toString().toDouble();
			sw.TimeTotal = fix->value("time_total").toString().toDouble();
			sw.FuelFlow = fix->value("fuel_flow").toString().toDouble();
			sw.FuelLeg = fix->value("fuel_leg").toString().toDouble();
			sw.FuelTotalUsed = fix->value("fuel_totalused").toString().toDouble();
			sw.FuelMinOnboard = fix->value("fuel_min_onboard").toString().toDouble();
			sw.FuelPlanOnboard = fix->value("fuel_plan_onboard").toString().toDouble();
			sw.OAT = fix->value("oat").toString().toDouble();
			sw.OATIsaDev = fix->value("oat_isa_dev").toString().toDouble();
			sw.WindDir = fix->value("wind_dir").toString().toDouble();
			sw.WindSpeed = fix->value("wind_spd").toString().toDouble();
			sw.Shear = fix->value("shear").toString().toDouble();
			sw.TropoPause = fix->value("tropopause_feet").toString().toDouble();
			sw.GroundHeight = fix->value("ground_height").toString().toDouble();
			sw.Mora = fix->value("mora").toString().toDouble();
			sw.Fir = fix->value("fir").toString();
			sw.FirUnits = fix->value("fir_units").toString();
			sw.FirValidLevels = fix->value("fir_valid_levels").toString();
		}
		WayPoints->push_back(sw);
		point = 1;
	}
	else if ((point == 1) && (WayPoints->size() == 1)) {

		WayPoints->at(WayPoints->size() - 1).ELon = lon;
		WayPoints->at(WayPoints->size() - 1).ELat = lat;
		WayPoints->at(WayPoints->size() - 1).EAltitude = alt;
		WayPoints->at(WayPoints->size() - 1).Type = pointType;
		WayPoints->at(WayPoints->size() - 1).Name = PointName;
		WayPoints->at(WayPoints->size() - 1).FixAlt = fixAlt;
		WayPoints->at(WayPoints->size() - 1).IndSpeed = speed;
		
		if (fix != NULL) {
			WayPoints->at(WayPoints->size() - 1).AirWay = fix->value("via_airway").toString();
			WayPoints->at(WayPoints->size() - 1).Ident = fix->value("ident").toString();
			WayPoints->at(WayPoints->size() - 1).Name = fix->value("name").toString();
			//WayPoints->at(WayPoints->size() - 1).Type = fix->value("type").toString();
			WayPoints->at(WayPoints->size() - 1).Type = pointType;
			WayPoints->at(WayPoints->size() - 1).ELat = fix->value("pos_lat").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).ELon = fix->value("pos_long").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).Stage = fix->value("stage").toString();
			WayPoints->at(WayPoints->size() - 1).SidStar = (bool)fix->value("is_sid_star").toString().toInt();
			WayPoints->at(WayPoints->size() - 1).Distance = fix->value("distance").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).TrackTrue = fix->value("track_true").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).TrackMag = fix->value("track_mag").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).HeadingTrue = fix->value("heading_true").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).HeadingMag = fix->value("heading_mag").toString().toDouble();
			if (alt != 0) {
				WayPoints->at(WayPoints->size() - 1).EAltitude = alt;
			}
			else {
				WayPoints->at(WayPoints->size() - 1).EAltitude = fix->value("altitude_feet").toString().toDouble();
			}
			//WayPoints->at(WayPoints->size() - 1).IndSpeed = fix->value("ind_airspeed").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).IndSpeed = speed;
			WayPoints->at(WayPoints->size() - 1).TrueSpeed = fix->value("true_airspeed").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).Mach = fix->value("mach").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).MachThousandths = fix->value("mach_thousandths").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).WindComponent = fix->value("wind_component").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).GroundSpeed = fix->value("groundspeed").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).TimeLeg = fix->value("time_leg").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).TimeTotal = fix->value("time_total").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).FuelFlow = fix->value("fuel_flow").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).FuelLeg = fix->value("fuel_leg").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).FuelTotalUsed = fix->value("fuel_totalused").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).FuelMinOnboard = fix->value("fuel_min_onboard").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).FuelPlanOnboard = fix->value("fuel_plan_onboard").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).OAT = fix->value("oat").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).OATIsaDev = fix->value("oat_isa_dev").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).WindDir = fix->value("wind_dir").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).WindSpeed = fix->value("wind_spd").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).Shear = fix->value("shear").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).TropoPause = fix->value("tropopause_feet").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).GroundHeight = fix->value("ground_height").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).Mora = fix->value("mora").toString().toDouble();
			WayPoints->at(WayPoints->size() - 1).Fir = fix->value("fir").toString();
			WayPoints->at(WayPoints->size() - 1).FirUnits = fix->value("fir_units").toString();
			WayPoints->at(WayPoints->size() - 1).FirValidLevels = fix->value("fir_valid_levels").toString();
		}
		Utils::DOrtoKM(&WayPoints->at(WayPoints->size() - 1));
		if ((pointType == "FIXHEADING") || (pointType == "HEADINGUNTILALT")) {
			WayPoints->at(WayPoints->size() - 1).H = heading;
		}
		if (pointType == "HEADINGUNTILFROMDIST") {
			WayPoints->at(WayPoints->size() - 1).Dist = radial;
			WayPoints->at(WayPoints->size() - 1).H = heading;
		}
		if (pointType == "TRKINTERCEPTRADIAL") {
			WayPoints->at(WayPoints->size() - 1).Dist = radial;
			WayPoints->at(WayPoints->size() - 1).H = heading;
		}
		//CountWays++;
		point = 0;
	}
	else if (WayPoints->size() > 0) {
		
		sw.Lon = WayPoints->at(WayPoints->size() - 1).ELon;
		sw.Lat = WayPoints->at(WayPoints->size() - 1).ELat;
		sw.Altitude = WayPoints->at(WayPoints->size() - 1).EAltitude;
		sw.ELon = lon;
		sw.ELat = lat;
		sw.EAltitude = alt;
		sw.Type = pointType;
		sw.Name = PointName;
		sw.FixAlt = fixAlt;
		sw.IndSpeed = speed;
		if (fix != NULL) {
			sw.AirWay = fix->value("via_airway").toString();
			sw.Ident = fix->value("ident").toString();
			sw.Name = fix->value("name").toString();
			//sw.Type = fix->value("type").toString();
			sw.Type = pointType;
			sw.ELat = fix->value("pos_lat").toString().toDouble();
			sw.ELon = fix->value("pos_long").toString().toDouble();
			sw.Stage = fix->value("stage").toString();
			sw.SidStar = (bool)fix->value("is_sid_star").toString().toInt();
			sw.Distance = fix->value("distance").toString().toDouble();
			sw.TrackTrue = fix->value("track_true").toString().toDouble();
			sw.TrackMag = fix->value("track_mag").toString().toDouble();
			sw.HeadingTrue = fix->value("heading_true").toString().toDouble();
			sw.HeadingMag = fix->value("heading_mag").toString().toDouble();
			if (alt != 0) {
				sw.EAltitude = alt;
			}
			else {
				sw.EAltitude = fix->value("altitude_feet").toString().toDouble();
			}
			//sw.IndSpeed = fix->value("ind_airspeed").toString().toDouble();
			sw.IndSpeed = speed;
			sw.TrueSpeed = fix->value("true_airspeed").toString().toDouble();
			sw.Mach = fix->value("mach").toString().toDouble();
			sw.MachThousandths = fix->value("mach_thousandths").toString().toDouble();
			sw.WindComponent = fix->value("wind_component").toString().toDouble();
			sw.GroundSpeed = fix->value("groundspeed").toString().toDouble();
			sw.TimeLeg = fix->value("time_leg").toString().toDouble();
			sw.TimeTotal = fix->value("time_total").toString().toDouble();
			sw.FuelFlow = fix->value("fuel_flow").toString().toDouble();
			sw.FuelLeg = fix->value("fuel_leg").toString().toDouble();
			sw.FuelTotalUsed = fix->value("fuel_totalused").toString().toDouble();
			sw.FuelMinOnboard = fix->value("fuel_min_onboard").toString().toDouble();
			sw.FuelPlanOnboard = fix->value("fuel_plan_onboard").toString().toDouble();
			sw.OAT = fix->value("oat").toString().toDouble();
			sw.OATIsaDev = fix->value("oat_isa_dev").toString().toDouble();
			sw.WindDir = fix->value("wind_dir").toString().toDouble();
			sw.WindSpeed = fix->value("wind_spd").toString().toDouble();
			sw.Shear = fix->value("shear").toString().toDouble();
			sw.TropoPause = fix->value("tropopause_feet").toString().toDouble();
			sw.GroundHeight = fix->value("ground_height").toString().toDouble();
			sw.Mora = fix->value("mora").toString().toDouble();
			sw.Fir = fix->value("fir").toString();
			sw.FirUnits = fix->value("fir_units").toString();
			sw.FirValidLevels = fix->value("fir_valid_levels").toString();
		}
		Utils::DOrtoKM(&sw);
		if ((pointType == "FIXHEADING") || (pointType == "HEADINGUNTILALT")) {
			sw.H = heading;
		}
		if (pointType == "HEADINGUNTILFROMDIST") {
			sw.Dist = radial;
			sw.H = heading;
		}
		if (pointType == "TRKINTERCEPTRADIAL") {
			sw.Dist = radial;
			sw.H = heading;
		}
		if (pointType == "TRKUNTILALT") {
			sw.H = heading;
		}
		WayPoints->push_back(sw);
		//CountWays++;
	}
}

VOID MainLogic::TimerProc()
{

	
	
	//emit SendLog("Tick");
//std::string OrigICAO = GetDataString(GPS_WP_NEXT_ID);
	if (Mode == START) {
		//SendCommand(SET_THROTTLE, 13130, 0);
		if (WayPoints == NULL) {
			WayPoints = new std::vector<sWayPoint>();


			mgr = new QNetworkAccessManager(this);
			//const QUrl url(QStringLiteral("http://www.simbrief.com/system/dispatch.php?airline=ABC&fltnum=1234&type=B738&orig=KORD&dest=KSFO&deph=16&depm=30&steh=4&stem=30"));
			const QUrl url(QStringLiteral("http://www.simbrief.com/api/xml.fetcher.php?json=1&username=autom"));
			QNetworkRequest request(url);
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

			QJsonObject obj;
			obj["orig"] = "USSS";
			obj["dest"] = "UUDD";
			QJsonDocument doc(obj);
			QByteArray data = doc.toJson();
			// or
			// QByteArray data("{\"key1\":\"value1\",\"key2\":\"value2\"}");
			QNetworkReply* reply = mgr->post(request, data);

			QObject::connect(reply, &QNetworkReply::finished, [=]() {
				if (reply->error() == QNetworkReply::NoError) {
					document = QJsonDocument::fromJson(reply->readAll());
					Mode = PLANREADY;

					//QString contents = QString::fromUtf8(reply->readAll());
					//qDebug() << contents;
				}
				else {
					QString err = reply->errorString();
					qDebug() << err;
				}
				reply->deleteLater();

				});
		}

	}
	if (Mode == PLANREADY) {

		QJsonObject root = document.object();
		FlightCruise = root["general"].toObject()["initial_altitude"].toString().toDouble();
		MaximumAltitude = FlightCruise;
		ja = root["navlog"].toObject()["fix"].toArray();


		//QString link = root["files"].toObject()["directory"].toString();
		QString link = "http://www.simbrief.com/system/briefing.fmsdl.php?formatget=flightplans/";
		QJsonArray jf = root["files"].toObject()["file"].toArray();
		for (int i = 0; i < jf.count(); i++) {
			if (jf[i].toObject()["name"] == "FS2020") {
				PLNFile = jf[i].toObject()["link"].toString();
				link = link + PLNFile;
				QUrl file(link);
				QNetworkRequest requestFile(file);
				requestFile.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
				QNetworkReply* replyFile = mgr->post(requestFile, QByteArray(NULL));

				QObject::connect(replyFile, &QNetworkReply::finished, [=]() {
					if (replyFile->error() == QNetworkReply::NoError) {
						//QString PLNFile = QString::fromLatin1(replyFile->readAll());
						QFile* pFile = new QFile(getenv("TEMP") + QString("\\") + PLNFile);
						// Создаём файл или открываем его на перезапись ...
						if (pFile->open(QFile::WriteOnly)) {
							pFile->write(replyFile->readAll());  // ... и записываем всю информацию со страницы в файл
							pFile->close();                  // закрываем файл
							qDebug() << "Downloading is completed";
							PLNFile = getenv("TEMP") + QString("\\") + PLNFile;
						}
					}
					else {
						QString err = replyFile->errorString();
						qDebug() << err;
					}
					replyFile->deleteLater();
					});
				break;
			}
		}
		Mode = PREPARE;
		//Mode = FILLWAY;
	}
	if (Mode == FILLWAY) {
		if (AirportData == NULL) {
			if (GetData(PLANE_LATITUDE, "degree") != 0.0 && GetData(PLANE_LONGITUDE, "degree") != 0.0) {
				AirportData = ::GetAirportData();
				data->AirportData = AirportData;
				AirportData->SetHSim(HSimConnect);
				while (1) {
					if (AirportData->GetProgress() == 100) {
						break;
					}
				}
			}
		}
		else {
			

			//AirportList ApList = AirportData->GetNearAirport(GetData(PLANE_LATITUDE, "degree"), GetData(PLANE_LONGITUDE, "degree"));
			QJsonObject root = document.object();
			//QString originIcao = root["origin"].toObject()["icao_code"].toString().toStdString();
			AirportList OrigApt = AirportData->GetAirportByIcao(root["origin"].toObject()["icao_code"].toString().toStdString());
			
			//AirportList AltApt = AirportData->GetAirportByIcao(root["alternate"].toObject()["icao_code"].toString().toStdString());
			if (OrigApt.ICAO != "RJCI") {
				DWORD index = AirportData->GetNearTaxiwayPoint(GetData(PLANE_LATITUDE, "degree"), GetData(PLANE_LONGITUDE, "degree"));

				if (GetData(SIM_ON_GROUND) != 0.0) {
					//std::vector<RunwayPaths> *RP = new std::vector<RunwayPaths>();
					std::vector<RunwayPaths>* RP = AirportData->GetRunways();
					AirportData->GetRunwayStart(0, 0, 0);
					//std::vector<TPath> Path = PAirportData->GetPath(index, PaircraftData->hed, PaircraftData->WindDirection);
					std::vector<std::vector<TPath>>* RunwayWays = new std::vector<std::vector<TPath>>();
					std::vector<TPath> P;
					for (int i = 1; i <= RP->size(); i++) {
						P = AirportData->GetPath1(index, GetData(PLANE_HEADING_DEGREES_TRUE), i);

						RP->at(i - 1).RunwayName1 = P[P.size() - 1].name;
						RP->at(i - 1).R1Dist = Utils::GetPathLength(&P);
						RP->at(i - 1).Deg1 = Utils::GetRWAngle(&P);
						RunwayWays->push_back(P);
						P = AirportData->GetPath1(index, GetData(PLANE_HEADING_DEGREES_TRUE), -i);
						RP->at(i - 1).RunwayName2 = P[P.size() - 1].name;
						RP->at(i - 1).R2Dist = Utils::GetPathLength(&P);
						RP->at(i - 1).Deg2 = Utils::GetRWAngle(&P);
						RunwayWays->push_back(P);
						RP->at(i - 1).Lenght = Utils::GetRWLength(&P);
					}
					int DirWind = 5;
					std::vector<std::string>* RWSForTL = new std::vector<std::string>();
					while (1) {
						for (int i = 0; i < RP->size(); i++) {
							if (abs(GetData(AMBIENT_WIND_DIRECTION) - RP->at(i).Deg1 - GetData(MAGVAR)) < DirWind) {
								RWSForTL->push_back(RP->at(i).RunwayName1);
							}
							if (abs(GetData(AMBIENT_WIND_DIRECTION) - RP->at(i).Deg2 - GetData(MAGVAR)) < DirWind) {
								RWSForTL->push_back(RP->at(i).RunwayName2);
							}
						}
						if (RWSForTL->size() != 0) {
							break;
						}
						else
						{
							DirWind = DirWind + 5;
						}
					}
					double DToRW = 10000;
					int RWIndex = -1;
					for (int i = 0; i < RP->size(); i++)
					{
						for (int j = 0; j < RWSForTL->size(); j++)
						{
							if (RWSForTL->at(j) == RP->at(i).RunwayName1) {
								if (DToRW > RP->at(i).R1Dist) {
									DToRW = RP->at(i).R1Dist;
									for (int k = 0; k < RunwayWays->size(); k++) {
										if (RunwayWays->at(k).at(RunwayWays->at(k).size() - 1).name == RP->at(i).RunwayName1) {
											RWIndex = k;
											break;
										}
									}
								}
							}
							if (RWSForTL->at(j) == RP->at(i).RunwayName2) {
								if (DToRW > RP->at(i).R2Dist) {
									DToRW = RP->at(i).R2Dist;
									for (int k = 0; k < RunwayWays->size(); k++) {
										if (RunwayWays->at(k).at(RunwayWays->at(k).size() - 1).name == RP->at(i).RunwayName2) {
											RWIndex = k;
											break;
										}
									}
								}
							}
						}
					}
					double DtoRun = 0.0;
					for (int i = 0; i < RunwayWays->size(); i++) {
						//char ss[50] = "";
						//sprintf(ss, "%s\0", RunwayWays->at(i).at(RunwayWays->at(i).size() - 1).name.c_str());
						//strcat((char*)Menu1, ss);
						std::string Recom = "";

						for (int k = 0; k < RWSForTL->size(); k++) {
							if (RWSForTL->at(k) == RunwayWays->at(i).at(RunwayWays->at(i).size() - 1).name) {
								Recom = " (Recomented)";
							}
							if (RWIndex == i) {
								Recom = " (Very recomented)";
								rrr = RWIndex;
							}
						}
						
						for (int m = 0; m < RP->size(); m++)
						{
							if (RP->at(m).RunwayName1 == RunwayWays->at(i).at(RunwayWays->at(i).size() - 1).name) {
								DtoRun = RP->at(m).R1Dist;
							}
							if (RP->at(m).RunwayName2 == RunwayWays->at(i).at(RunwayWays->at(i).size() - 1).name) {
								DtoRun = RP->at(m).R2Dist;
							}
						}
					}
					std::string RWFromSB = root["origin"].toObject()["plan_rwy"].toString().toStdString();
					for (int RC = 0; RC < RunwayWays->size(); RC++) {
						if (RunwayWays->at(RC).at(RunwayWays->at(RC).size() - 1).name == RWFromSB) {
							rrr = RC;
							break;
						}
					}
					//SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 30.0, EVENT_MENU_1, sizeof(Menu1), (void*)Menu1.str().c_str());
					//SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_SCROLL_BLUE, 0, EVENT_TEXT_1, sizeof(Text1), (void*)Text1);
					//SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MESSAGE_WINDOW, 0, EVENT_MESSAGE_1, sizeof(Message1), (void*)Message1);
					/*while (rrr < 0) {
						hr = SimConnect_CallDispatch(hSimConnect, MyDispatchProc, NULL);
					}*/
					if (rrr == RunwayWays->size() || rrr == 55) {
						rrr = RWIndex;
					}
					else if (rrr > RunwayWays->size()) {
						rrr = 0;
					}
					//for (int i = 0; i < Path.size() - 1; i++) {
					for (int i = 0; i < RunwayWays->at(rrr).size() - 1; i++) {
						AddWayPoint(RunwayWays->at(rrr).at(i).Lon, RunwayWays->at(rrr).at(i).Lat, 0.0, TYPE_PATHS[RunwayWays->at(rrr)[i].Type], QString(RunwayWays->at(rrr).at(i).name.c_str()), 0, 0, 0, 0);
					}
					sWayPoint HeadTakeOff;
					HeadTakeOff.Lat = RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 2).Lat;
					HeadTakeOff.Lon = RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 2).Lon;
					HeadTakeOff.Altitude = 0.0;
					HeadTakeOff.ELat = RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 1).Lat;
					HeadTakeOff.ELon = RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 1).Lon;
					HeadTakeOff.EAltitude = 0.0;
					Utils::DOrtoKM(&HeadTakeOff);
					AddWayPoint(RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 1).Lon, RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 1).Lat, 5555, "RUNWAY", QString(RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 1).name.c_str()), 0, 0, 0, 0);
					SIDPoint = WayPoints->size() - 2;
					SimBriefSID = " ";
					FixSID = " ";
					SimBriefSTAR = " ";
					FixSTAR = " ";
					QJsonObject fix;
					fix = ja[1].toObject();
					if (fix.value("is_sid_star").toString() == "1") {
						SimBriefSID = fix.value("via_airway").toString();
					}
					for (int i = 1; i < ja.count() - 1; i++) {
						fix = ja[i].toObject();
						if (fix.value("is_sid_star").toString() == "0") {
							FixSID = fix.value("ident").toString();
							break;
						}
					}
					fix = ja[ja.count() - 1].toObject();
					if (fix.value("is_sid_star").toString() == "1") {
						SimBriefSTAR = fix.value("via_airway").toString();
					}
					for (int i = ja.count() - 2; i > 1; i--) {
						fix = ja[i].toObject();
						if (fix.value("is_sid_star").toString() == "0") {
							FixSTAR = fix.value("ident").toString();
							break;
						}
					}


					OrigSidStar = AirportData->GetAirport()->sidstar;
					std::vector<TSIDS>* OrigSids = AddSID(OrigSidStar, &RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 1), HeadTakeOff.EndHeadingTrue, FixSID, SimBriefSID);
					WayPoints->at(SIDPoint).Name = QString(OrigSids->at(0).SID.c_str());
					for (int i = 0; i < OrigSids->size(); i++) {
						if (OrigSids->at(i).SID == SimBriefSID.toStdString()) {
							for (int j = 0; j < OrigSids->at(i).wayPoint->size(); j++) {
								FIXX fixx = {};
								bool FindFIX = false;
								for (int fix = 0; fix < OrigSidStar->FIXES->size(); fix++) {
									if (OrigSidStar->FIXES->at(fix).Name == OrigSids->at(i).wayPoint->at(j).FIXName) {
										FindFIX = true;
										fixx = OrigSidStar->FIXES->at(fix);
										break;
									}
								}
								AddSidStarTrack(&OrigSids->at(i).wayPoint->at(j), &RunwayWays->at(rrr).at(RunwayWays->at(rrr).size() - 1), &fixx);
							}
							WayPoints->at(SIDPoint).Name = QString(OrigSids->at(i).SID.c_str());
							break;
						}
					}
					
					RunwayHeading = HeadTakeOff.EndHeadingTrue;
					//CountWays = SIDPoint + 2;
					//
					RoutePoint = WayPoints->size();
					for (int sr = SIDPoint; sr < RoutePoint; sr++) {
						for (int sp = sr; sp < RoutePoint; sp++) {
							if (WayPoints->at(sr).IndSpeed == 0) {
								if (WayPoints->at(sp).IndSpeed != 0) {
									WayPoints->at(sr).IndSpeed = WayPoints->at(sp).IndSpeed;
									break;
								}
							}
						}
					}
					/*for (int i = 0; i < RunwayWays->size(); i = i + 2) {
						RunWaysPaths.push_back({ "", "", "", 0, RunwayWays->at(i).at(RunwayWays->at(rrr).size() - 2).Lat, RunwayWays->at(i).at(RunwayWays->at(rrr).size() - 2).Lon,
							RunwayWays->at(i).at(RunwayWays->at(rrr).size() - 1).Lat, RunwayWays->at(i).at(RunwayWays->at(rrr).size() - 1).Lon
							});
						Utils::DOrtoKM(&RunWaysPaths.at(i));
					}*/
					for (int i = 0; i < RunwayWays->size(); i=i+1) {
						RunWaysPaths.push_back({ "", "", "", 0, RunwayWays->at(i).at(RunwayWays->at(i).size() - 2).Lat, RunwayWays->at(i).at(RunwayWays->at(i).size() - 2).Lon,
							RunwayWays->at(i).at(RunwayWays->at(i).size() - 1).Lat, RunwayWays->at(i).at(RunwayWays->at(i).size() - 1).Lon
							});
						Utils::DOrtoKM(&RunWaysPaths.at(i));
					}
				}
			}
			else {
				AddWayPoint(0, 0, FlightCruise, "FIX", "", 0, CRUISEALT, 0, 0, &ja[0].toObject());
			}

			sWayPoint tWP;
			QJsonObject fix;
			for (int i = 1; i < ja.count() - 1; i++) {
				fix = ja[i].toObject();
				if (fix.value("is_sid_star").toString() == "0") {
					AddWayPoint(0, 0, 0, "WAYPOINT", "", 0, CRUISEALT, 0, 0, &fix);
				}
			}
			fix = ja[ja.count() - 1].toObject();
			//AddWayPoint(0, 0, 0, "WAYPOINT", "", 0, FIXALT, 0, 0, &fix);
			std::string RWDest = root["destination"].toObject()["plan_rwy"].toString().toStdString();
			STARPoint = WayPoints->size() - 1;
			AirportList DestApt = AirportData->GetAirportByIcao(root["destination"].toObject()["icao_code"].toString().toStdString());
			DestSidStar = AirportData->GetAirport()->sidstar;

			std::vector<int>* StartIndex;
			fix = ja[ja.count() - 1].toObject();
			StartIndex = AirportData->GetRunwayStart(fix.value("pos_lat").toDouble(), fix.value("pos_long").toDouble(), fix.value("wind_dir").toDouble());
			for (int iii = 0; iii < 1 /*StartIndex->size()*/; iii++) {
				DATA_RUNWAY runway = AirportData->GetStartCoord(StartIndex->at(iii));
				runway.alt = AirportData->GetAirport()->PAirportInformation->Alt/1000* 3.28;
				SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway.sLatitude, runway.sLongitude, runway.eHeading, 18.0);
				//CountWays = CountWays - 1;
				//WayPoints->pop_back();
				STARPoint = WayPoints->size() - 1;
				std::vector<TSTARS>* OrigStars = AddSTAR(DestSidStar, &runway, FixSTAR, SimBriefSTAR);
				int Star = 0;
				bool FindStar = false;;
				for (int i = 0; i < OrigStars->size(); i++) {
					if (QString(OrigStars->at(i).STAR.c_str()) == SimBriefSTAR) {
						FindStar = true;
						Star = i;
					}
				}
				if (OrigStars->size() > 0) {
					WayPoints->at(STARPoint).Name = QString((OrigStars->at(Star).STAR + " -> " + OrigStars->at(Star).APPROACH).c_str());
					for (int i = 0; i < OrigStars->at(Star).wayPoint->size(); i++) {
						FIXX fixx = {};
						bool FindFIX = false;
						for (int fix = 0; fix < DestSidStar->FIXES->size(); fix++) {
							if (DestSidStar->FIXES->at(fix).Name == OrigStars->at(Star).wayPoint->at(i).FIXName) {
								FindFIX = true;
								fixx = DestSidStar->FIXES->at(fix);
								break;
							}
						}
						if (i == AppPoint) {
							AppPoint = WayPoints->size() - 1;
						}
						AddSidStarTrack(&OrigStars->at(Star).wayPoint->at(i), NULL, &fixx, &runway);
					}
				}
				//WayPoints->at(STARPoint).Name = AddSTAR(DestSidStar, &runway, FixSTAR, SimBriefSTAR);
				if (WayPoints->at(STARPoint).Name != "") {
					break;
				}
			}
			AddWayPoint(0, 0, 0, "WAYPOINT", "", 0, FIXALT, 0, 0, &fix);
			WayPoints->at(WayPoints->size() - 1).Altitude = AirportData->GetAirport()->PAirportInformation->Alt/1000* 3.28;
			WayPoints->at(WayPoints->size() - 1).EAltitude = AirportData->GetAirport()->PAirportInformation->Alt/1000* 3.28;



				

			//std::vector<RunwayPaths> *RP = new std::vector<RunwayPaths>();

			
			STAR = true;



			Mode = PREPARE;
			//Mode = TAXIOUT;
			//Mode = TAKEOFF;
			//Mode = CRUISE;
			//SendCommand(HDG_SEL, 180, 0);
			//Mode = ENGINESTART;
		}
	}
	if (Mode == PREPARE) {
		if (preliminaryCocpitPrep == 0) {
			if (PLNFile != "") {
				QByteArray ba = PLNFile.toLocal8Bit();
				const char* PLNChar = ba.data();
				
				emit PreliminaryCocpitPrep(&preliminaryCocpitPrep);
				//emit SetModeSignal(PREPARE);
				SimConnect_FlightPlanLoad(HSimConnect, PLNChar);
			}
		}
			if (preliminaryCocpitPrep == 2) {
			if (cocpitPreparation == 0) {
				emit CocpitPreparation(&cocpitPreparation);
				//while (data->GData.ATC_RUNWAY_SELECTED != 1);
				AtcRwy = std::string(data->GData.ATC_RUNWAY_AIRPORT_NAME);
				
			}
			if (SIDPoint == 0) {
				//SendCommand(SET_THROTTLE, 8193, 0);
				Mode = FILLWAY;
			}
			else {
				if (cocpitPreparation == 2) {
					if (beforeStart == 0) {
						emit BeforeStart(&beforeStart);
						Mode = STARTPUSHBACK;
					}
				}
			}
		}
		
		

	}
	if (Mode == PUSHBACK || Mode == STARTPUSHBACK) {

		//DrawAirport(PAirportData, PaircraftData->lat, PaircraftData->lon, PaircraftData->hed);
		
			CurrentPos = WayPoints->at(CurrentWay);
			if (CurrentPos.Type == TYPE_PATHS[11]) {
				if (Mode == STARTPUSHBACK) {
					if (GetData(PUSHBACK_AVAILABLE));
					//SendEvent(KEY_PARKING_BRAKES, 1);
					SendEvent(KEY_PUSHBACK_SET, 1);
					while (GetData(GROUND_VELOCITY) < 0.02);
					SetData(PUSHBACK_WAIT, 1);
					Sleep(60000);
					//SendEvent(KEY_PARKING_BRAKES, 0);
					SendCommand(PARKBRAKE_SET, 0, 0);
					SetData(PUSHBACK_WAIT, 0);
					Mode = PUSHBACK;
				}
				//emit SendCommand(SET_PUSHBACK_SPEED, 5, 10);
				//SetData(GROUND_VELOCITY, 5);
				CurrentPos.Lat = GetData(PLANE_LATITUDE, "degree");
				CurrentPos.Lon = GetData(PLANE_LONGITUDE, "degree");
				Utils::DOrtoKM(&CurrentPos);
				double DCommon = Utils::DToType(CurrentWay, WayPoints, "RUNWAY", &CurrentPos);

				double REangle; //Subtraction from start and end points heading on ortodrome
				if ((CurrentWay + 1) >= WayPoints->size()) {
					REangle = (WayPoints->at(0).HeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue);
				}
				else {
					REangle = WayPoints->at(CurrentWay + 1).HeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue;
				}

				REangle = Utils::Constrain180(REangle); //Угол между текущей и следующей линиями маршрута

				double DToHed = 0.014486872954506089683063561200355 + 0.001; // (tan(REangle) * 8.364);
				double EangleRel = CurrentPos.EndHeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue; //Отклонение текущего угла от линии маршрута

				EangleRel = Utils::Constrain180(EangleRel);

				if ((abs(EangleRel) > 60) || (CurrentPos.Distance < abs(DToHed))) {
					CurrentWay = CurrentWay + 1;  //Если отклонение текущего угла от линии маршрута больше 60* или расстояние до точки маршрута меньше REangle*0.0002, то расчитываем следующюю линию маршрута

				}
				else {
					double HeadingRel = PBHeadWithWay(&WayPoints->at(CurrentWay), true);
				}
			}
			else {
				if (Mode == PUSHBACK) {
					
					SetData(PUSHBACK_WAIT, 1);
					//emit SendCommand(SET_PUSHBACK_SPEED, 0, 10);
					//SetData(GROUND_VELOCITY, 0);
					SendEvent(KEY_PUSHBACK_SET, 0);
					while (GetData(GROUND_VELOCITY) > 0.005) {
						SetData(PUSHBACK_WAIT, 1);
						SetData(GROUND_VELOCITY, 0);
					};
					SetData(PUSHBACK_WAIT, 1);
					//SendEvent(KEY_PARKING_BRAKES, 1);
					SendCommand(PARKBRAKE_SET, 1, 0);
					//SendEvent(KEY_PUSHBACK_SET, 1);
				}
				Mode = ENGINESTART;
				Sleep(30000);
				//brakes = { 0.0,0.0 };
				//SimConnect_SetDataOnSimObject(hSimConnect, DEF_BRAKE, 0, 0, 0, sizeof(brakes), &brakes.LBrake);
			}
	}	
	if (Mode == ENGINESTART) {
		
		if (beforeStart == 2) {
			if (engineStart == 0) {
				emit EngineStart(&engineStart);
			}

		}
		if (engineStart == 2) {
			if (afterStart == 0) {
				emit AfterStart(&afterStart);
			}
		}
		if (afterStart == 2) {
			if (beforeTaxi == 0) {
				emit BeforeTaxi(FlightCruise, RunwayHeading, &beforeTaxi);
			}
		}
		double V2 = data->AllData.AIRLINER_V2_SPEED;
		
		if ((beforeTaxi == 2)&&(V2 != 0)) {
			SendCommand(SET_GSPEED, -1, 0);
			Sleep(5000);
			Mode = TAXIOUT;
			Taxiway = CurrentWay;
			SendCommand(PARKBRAKE_SET, 0, 0);
		}
	}
	if ((Mode == TAXIOUT) || (Mode == TAXIIN)) {
		CurrentPos = WayPoints->at(CurrentWay);
		CurrentPos.Lat = GetData(PLANE_LATITUDE, "degree");
		CurrentPos.Lon = GetData(PLANE_LONGITUDE, "degree");
		Utils::DOrtoKM(&CurrentPos);
		double DCommon = Utils::DToType(CurrentWay, WayPoints, "RUNWAY", &CurrentPos);
		double DToRunway = 1;
		if (Mode == TAXIOUT) {
			DToRunway = Utils::DToRunway(CurrentWay, WayPoints, &CurrentPos);
		}
		if (Mode == TAXIOUT) {
			SendText("MODE: TAXI_OUT To " + WayPoints->at(SIDPoint + 1).Name + " - " + WayPoints->at(SIDPoint).Name + " Current: " + QString::number(CurrentPos.Distance, 'f', 3) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \n \
			Speed " + QString::number(speed) + "; Current way: " + QString::number(CurrentWay) + " Name: " + CurrentPos.Name + " Type: " + CurrentPos.Type + " Total ways: " + QString::number(WayPoints->size()), false);
		}
		else {
			SendText("MODE: TAXI_IN To " + WayPoints->at(SIDPoint + 1).Name + " - " + QString(ParkN->c_str()) + " Current: " + QString::number(CurrentPos.Distance, 'f', 3) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \n \
			Speed " + QString::number(speed) + "; Current way: " + QString::number(CurrentWay) + " Name: " + CurrentPos.Name + " Type: " + CurrentPos.Type + " Total ways: " + QString::number(WayPoints->size()), false);
		}
		double kDToHed = 0.00004;
		double REangle;
		if ((CurrentWay + 1) <= WayPoints->size()) {
			REangle = Utils::Constrain180(WayPoints->at(CurrentWay).HeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue);
		}
		else {
			REangle = Utils::Constrain180(WayPoints->at(CurrentWay + 1).HeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue);
		}
		double DToHed = abs(REangle) * GetData(GROUND_VELOCITY) * kDToHed;
		if (DToHed < 0.025) {
			DToHed = 0.025;
		}
		if ((CurrentWay + 1) >= WayPoints->size()) {
			DToHed = 0.001;
		}
		double EangleRel = Utils::Constrain180(CurrentPos.HeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue);

		double a = Utils::GetFixDA(sin(EangleRel * M_PI / 180) * CurrentPos.Distance, EangleRel);

		double RWDist = Utils::RunwayDictCalc(&RunWaysPaths, &CurrentPos);
		if (RWDist < 0.1) {
			SendCommand(LIGHTSTROBE_SET, 1, 0);
			if (DToRunway < 0.15) {
				if (beforeTakeoff == 0) {
					emit BeforeTakeoff(&beforeTakeoff);
				}
			}
		}
		else {
			SendCommand(LIGHTSTROBE_SET, 0, 0);
		}

		double HeadingRel = ManHeadWithWay(&WayPoints->at(CurrentWay));
		if (minDCommon > DCommon) {
			minDCommon = DCommon;
		}
		if (((CurrentWay + 1) >= WayPoints->size()) && (DCommon < 0.003) && ((minDCommon + 0.001) < DCommon)) {
			SendCommand(SET_GSPEED, -5, 0);
			double brake = 0.5; // (PaircraftData->GS - speed) / 70.0;

			//if (data->GData.GROUND_VELOCITY < 0.1) {
				//SendCommand(SET_THROTTLE, 0, 0);
				//SendCommand(PARKBRAKE_SET, 1, 0);

				SendText("DONE!", true);
				Mode = DONE;
			//}
		}
		if ((abs(EangleRel) > 60) || (CurrentPos.Distance < DToHed)) {
			if (((CurrentWay + 1) >= WayPoints->size()) && (DCommon < 0.003) && ((minDCommon + 0.001) < DCommon)) {
				SendCommand(SET_GSPEED, -5, 0);

				//if (data->GData.GROUND_VELOCITY < 0.1) {
					//SendCommand(SET_THROTTLE, 0, 0);
					//SendCommand(PARKBRAKE_SET, 1, 0);

					SendText("DONE!", true);
					Mode = DONE;
					//mode = START;
					//CurrentWay = CurrentWay + 1;
					if (CurrentWay + 1 < WayPoints->size()) {
						CurrentWay = CurrentWay + 1;
					}
				//}
			}
			else {
				if (CurrentWay + 1 < WayPoints->size()) {
					CurrentWay = CurrentWay + 1;
				}
			}
		}
		else {
			if ((abs(HeadingRel) < 5.0) && (abs(a) < 0.005) && (CurrentPos.Distance > 0.200)) {
				if (((CurrentWay + 1) != WayPoints->size()) && ((WayPoints->at(CurrentWay).Type == "RUNWAY"))) {
					speed = -1;
					SendCommand(LIGHTLANDING_SET, 1, 0);
					SendCommand(LIGHTTAXI_SET, 2, 0);
					if (GetData(GROUND_VELOCITY) < 1) {

						SendCommand(SET_TRUST, 40, 0);
						while ((GetData(ENG_N1_RPM1) < 0.4) && (GetData(ENG_N1_RPM2) < 0.4));
						//speed = 180;
						Mode = TAKEOFF;
						//CurrentWay = Taxiway;

					}
				}
				else {
					SendCommand(LIGHTLANDING_SET, 0, 0);
					SendCommand(LIGHTTAXI_SET, 1, 0);
					speed = 20;
					//speed = 5;
				}
			}
			else {
				if ((abs(EangleRel) > 60) || (CurrentPos.Distance < (DToHed + 0.15))) {
					speed = 20;
					//speed = 5;
					if (abs(REangle) > 10) {
						speed = 15;
						//speed = 5;
					}
					if (abs(REangle) > 30) {
						speed = 12;
						//speed = 5;
					}
					if (abs(REangle) > 50) {
						speed = 10;
						//speed = 5;
					}
					if (abs(REangle) > 100) {
						speed = 5;
					}
					if (abs(HeadingRel) > 5) {
						speed = 5;
					}
				}
				else {
					speed = 20;
					//speed = 5;
				}
				if (abs(a) > 0.005) {
					if ((abs(HeadingRel) > 5)) {
						speed = 5;
					}
					else if ((abs(HeadingRel) > 10)) {
						speed = 7;
						//speed = 5;
					}
					else if ((abs(HeadingRel) > 15)) {
						speed = 10;
						//speed = 5;
					}
					else {
						speed = 12;
						//speed = 5;
					}
				}
				if (DCommon < 0.2) {
					speed = 10;
					if (DCommon < 0.100) {
						speed = 5;
					}
					if (DCommon < 0.002) {
						SendCommand(SET_GSPEED, -5, 0);
						double brake = 0.5; // (PaircraftData->GS - speed) / 70.0;

						//if (data->GData.GROUND_VELOCITY < 0.1) {
							//SendCommand(SET_THROTTLE, 0, 0);
							//SendCommand(PARKBRAKE_SET, 1, 0);

							SendText("DONE!", true);
							Mode = DONE;
							//mode = START;
							//CurrentWay = CurrentWay + 1;
							if (CurrentWay + 1 < WayPoints->size()) {
								CurrentWay = CurrentWay + 1;
							}
						//}
					}
					if ((CurrentWay + 1) >= WayPoints->size()) {
						speed = 3;
					}
					if (DCommon < 0.100) {
						speed = 3;
					}
				}
			}
			if ((Mode != TAKEOFF) && (Mode != DONE)) {
				if (beforeTakeoff != 1) {
					//if (data->GData.ATC_CLEARED_TAXI || data->GData.ATC_CLEARED_TAKEOF) {
						SendCommand(SET_GSPEED, speed, 0);
					/* }
					else {
						SendCommand(SET_GSPEED, -1, 0);
					}*/
				}
				else {
					SendCommand(SET_GSPEED, -1, 0);
				}
			}
		}
	}
	if (Mode == DONE) {
		if (data->GData.GROUND_VELOCITY < 0.1) {
			SendCommand(SET_GSPEED, 0, 0);
			SendCommand(PARKBRAKE_SET, 1, 0);
			SendCommand(SET_THROTTLE, 0, 0);
		}
	}
	if (Mode == TAKEOFF) {
		//SendCommand(SET_THROTTLE, 3900, 0);
		SendCommand(SET_ATHR, 1, 0);
		double NPitch = data->AllData.A32NX_FLIGHT_DIRECTOR_PITCH;
		double NBank = data->AllData.A32NX_FLIGHT_DIRECTOR_BANK;
		//NPitch = -15;
		double ISpeed = GetData(AIRSPEED_INDICATED);
		double SOG = GetData(SIM_ON_GROUND);
		double VSpeed = GetData(VERTICAL_SPEED);
		double Alt = GetData(PLANE_ALTITUDE);
		double Lat = GetData(PLANE_LATITUDE, "degree");
		double Lon = GetData(PLANE_LONGITUDE, "degree");
		double IAlt = GetData(INDICATED_ALTITUDE);
		double GAlt = GetData(PLANE_ALT_ABOVE_GROUND);

		double V1 = (data->AllData.AIRLINER_V1_SPEED);
		double VR = (data->AllData.AIRLINER_VR_SPEED);
		double V2 = (data->AllData.AIRLINER_V2_SPEED);
		double VF = (data->AllData.A32NX_SPEEDS_F);
		double VS = (data->AllData.A32NX_SPEEDS_S);
		double VGD = (data->AllData.A32NX_SPEEDS_GD);

		if (GetData(ENG_N1_RPM1, "percent over 100") > 0.40) {
			if (!TOGA) {
				//setThrust(100);
				SendEvent(KEY_AXIS_LEFT_BRAKE_SET, -16383);
				SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, -16383);
				SendCommand(SET_THROTTLE, 16383, 0);
				TOGA = TRUE;
			}
		}

		if (ISpeed > 230) {
			
			//NPitch = GetData(AUTOPILOT_FLIGHT_DIRECTOR_PITCH, "degree");
		}
		else if (ISpeed > VS) {
			SendCommand(FLAPS_SET, 0.0, 0);

			//NPitch = GetData(AUTOPILOT_FLIGHT_DIRECTOR_PITCH, "degree");

		}
		else if (ISpeed > VF) {
			SendCommand(FLAPS_SET, 1.0, 0);
			//NPitch = GetData(AUTOPILOT_FLIGHT_DIRECTOR_PITCH, "degree");
		}

		if ((ISpeed < V2) && (SOG== FALSE)) {
			if (NPitch < 10) {
				//NPitch = GetData(AUTOPILOT_FLIGHT_DIRECTOR_PITCH, "degree");
			}
			else {

				//NPitch = GetData(AUTOPILOT_FLIGHT_DIRECTOR_PITCH, "degree");
				//NPitch = -15;

			}
			SendCommand(SET_RUDDER, 0, 0);
		}
		//ManPitchWithFD(NPitch);
		if (ISpeed > VR) {
			
				if (SOG == 1) {
					ManPitchWithFD(-7.5);
				}
				if (GetData(AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1)) {
					if ((GAlt < 500) && (SOG == 0)) {
						ManPitchWithFD(NPitch);
					}
					else {
						ManPitchWithFD(-15);
					}
				}
				else if (SOG == 0) {
					ManPitchWithFD(-15);
				}
			if ((VSpeed > 10) && (SOG == FALSE)) {
				double intpParameter = 0.0;
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
					
				}
				//TOGA = FALSE;
				//CurrentWay = CurrentWay + 1;
				//PPID.FT = true;
				//Mode = CLIMB;
			}

			if (TOGA) {
				if (GAlt > 1800) {
					SendCommand(SET_THROTTLE, 3900, 0);
					TOGA = false;
					Mode = CRUISE;
				}
			}
			
			//ManPitchWithFD(GetData(NPitch);
			
			

		}
		else if ((ISpeed > V1) && (ISpeed <= VR)) {
			//ManPitchWithFD(0);
			SendCommand(SET_ELEVATOR, -2500, 0);

		}
		else {
			SendCommand(SET_ELEVATOR, 3000, 20);
		}
		if (GAlt > 500) {

			//ManBankWithFD(NBank);
			//BankWithHead(CurrentPos.HeadingMag);
		}
		else {
			ManBankWithFD(-1);
		}
		CurrentPos = WayPoints->at(CurrentWay);
		CurrentPos.Lat = Lat;
		CurrentPos.Lon = Lon;
		Utils::DOrtoKM(&CurrentPos);
		double DCommon = Utils::DToType(CurrentWay, WayPoints, "GSWAY", &CurrentPos);

		double DToChange = CalcToNewWay(true);
		CHAR s[512];
		SendText("MODE: TAKEOFF Current: " + QString::number(CurrentPos.Distance, 'f', 3) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \nCurrent way: " + QString::number(CurrentWay) + " Type: " + CurrentPos.Type + " Total ways: " + QString::number(WayPoints->size()) + " D for head: " + QString::number(DToChange, 'f', 3), false);
		if (SOG == 1) {
			double HeadingRel = ManHeadWithWay(&WayPoints->at(CurrentWay));
			SendCommand(SET_AILERON, 0, 20);
		}
		else {
			if (GAlt > 500) {
				//double HeadingRel = BankWithHead(Utils::Constrain180(CurrentPos.HeadingTrue));
				ManHeadWithWay(&WayPoints->at(CurrentWay));
			}
			else {
				AltBankWithPos(0);
				SendCommand(SET_RUDDER, 0, 20);
				//double HeadingRel = BankWithHead(Utils::Constrain180(CurrentPos.HeadingTrue));
			}
		}
	}
	/*if (Mode == CLIMB) {

		double NPitch = (data->AllData.A32NX_FLIGHT_DIRECTOR_PITCH);
		double NBank = (data->AllData.A32NX_FLIGHT_DIRECTOR_BANK);
		double ISpeed = GetData(AIRSPEED_INDICATED);
		double TSpeed = GetData(AIRSPEED_TRUE);
		double SOG = GetData(SIM_ON_GROUND);
		double VSpeed = GetData(VERTICAL_SPEED);
		double Alt = GetData(PLANE_ALTITUDE);
		double Lat = GetData(PLANE_LATITUDE, "degree");
		double Lon = GetData(PLANE_LONGITUDE, "degree");
		double WindDir = GetData(AMBIENT_WIND_DIRECTION, "degree");
		double WindSpeed = GetData(AMBIENT_WIND_VELOCITY);
		double IAlt = GetData(INDICATED_ALTITUDE);
		double MagVar = GetData(MAGVAR);

		//double V1 = (data->AllData.AIRLINER_V1_SPEED);
		//double VR = (data->AllData.AIRLINER_VR_SPEED);
		//double V2 = (data->AllData.AIRLINER_V2_SPEED);
		double VF = (data->AllData.A32NX_SPEEDS_F);
		double VS = (data->AllData.A32NX_SPEEDS_S);
		double VGD = (data->AllData.A32NX_SPEEDS_GD);
		
		if (ISpeed > data->AllData.A32NX_SPEEDS_MANAGED_PFD) {
			if (ISpeed < VF + 5) {
				SendCommand(FLAPS_SET, 4.0, 0);
				SendCommand(LIGHTLANDING_SET, 1, 20);
				SendCommand(GEAR_SET, 1.0, 0);
			}
			else if (ISpeed < VS - 15) {
				SendCommand(FLAPS_SET, 3.0, 0);
			}
			else if (ISpeed < VS - 10) {
				SendCommand(GEAR_SET, 1.0, 0);
			}
			else if (ISpeed < VS + 5) {
				SendCommand(FLAPS_SET, 2.0, 0);
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			else if (ISpeed < VGD + 5) {
				SendCommand(LIGHTLANDING_SET, 0, 20);
				SendCommand(FLAPS_SET, 1.0, 0);
			}
		}
		else {
			if (ISpeed > VGD) {
				SendCommand(LIGHTLANDING_SET, 0, 20);
			}
			else if (ISpeed > VS) {
				SendCommand(FLAPS_SET, 0.0, 0);
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			else if (ISpeed > VF) {
				SendCommand(FLAPS_SET, 1.0, 0);
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			else
			{
				SendCommand(GEAR_SET, 0.0, 0);
			}
		}
		
		CurrentPos = WayPoints->at(CurrentWay);
		CurrentPos.Lat = Lat;
		CurrentPos.Lon = Lon;
		if (CurrentPos.IndSpeed == 0.0) {
			if (IAlt > 10500) {
				SendCommand(LIGHTTAXI_SET, 0, 0);
				SendCommand(LIGHTRUNWAY_SET, 0, 0);
				SendCommand(BARO_MODE, 2, 0);
			}
			else if (IAlt < 9500) {
				SendCommand(LIGHTTAXI_SET, 2, 0);
				SendCommand(LIGHTRUNWAY_SET, 1, 0);
				SendCommand(BARO_MODE, 0, 0);
			}
		}
		else {
			
		}
		if (IAlt < 10000) {
			
		}
		else {
			
		}
		if (IAlt > 28000) {
			
		}
		if (IAlt > 5000) {

		}
		Utils::DOrtoKM(&CurrentPos);
		double DCommon = Utils::DToType(CurrentWay, WayPoints, "GSWAY", &CurrentPos);
		if (CurrentPos.Type == "WAYPOINT") {
			FlightCruise = CurrentPos.Altitude;
		}
		double DToChange = CalcToNewWay(true);
		if (((DCommon < 300.0) || ((CurrentWay + 1) == WayPoints->size())) && (!STAR)) {
			//AirportList ApList = AirportData->GetNearAirport(WayPoints->at(WayPoints->size() - 1).ELat, WayPoints->at(WayPoints->size() - 1).ELon);
			AirportList ApList = AirportData->GetAirportByIcao(WayPoints->at(WayPoints->size() - 1).Ident.toStdString());
			std::vector<int>* StartIndex;
			if (WindSpeed>= 0.0) {
				StartIndex = AirportData->GetRunwayStart(Lat, Lon, WindDir);
			}
			else {
				StartIndex = AirportData->GetRunwayStart(Lat, Lon, WindDir);
			}
			for (int iii = 0; iii < StartIndex->size(); iii++) {
				DATA_RUNWAY runway = AirportData->GetStartCoord(StartIndex->at(iii));
				SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway.sLatitude, runway.sLongitude, runway.eHeading, 18.0);
				DWORD CW = WayPoints->size() - 1;
				STARPoint = CW - 1;
				WayPoints->at(STARPoint).Name = AddSTAR(AirportData, &runway);
				if (WayPoints->at(STARPoint).Name != "") {
					break;
				}
			}
			STAR = true;
		}
		CHAR s[512];
		double AngleToDesc = GetDescentAngle(WayPoints);
		double VSD = ManVSWithAngle(AngleToDesc);
		if (DCommon > 15) {
			SendCommand(ALT_SEL, FlightCruise, 20);
		}
		else {
			SendCommand(ALT_SEL, 6000, 20);
		}
		if (FlightCruise > (IAlt - 100)) {
			SendCommand(PULL_ALT, 0, 0);
		}
		if (AngleToDesc >= GetAngleToDesc(Alt)) {
			if (FlightCruise < (IAlt - 100)) {

				SendCommand(VS_SEL, int(VSD / 100) * 100, 20);
			}
			else {
				SendCommand(PULL_ALT, 0, 0);
			}
			//Mode = DESCENT;
		}
		if (CurrentPos.Type != "GSWAY") {
			int Alt = FlightCruise / 100;
			double HeadingRel;
			if (SOG == 0) {
				//AltPitchWithPos(-NPitch);
				ManPitchWithFD(NPitch);
				if ((CurrentPos.Type == "HEADINGUNTILALT") || (CurrentPos.Type == "HEADINGUNTILFROMDIST")) {
					double AngleWind = WindDir - CurrentPos.HeadingMag;
					double AngleDrift = asin(sin(AngleWind / 180 * M_PI) * WindSpeed / TSpeed) * 180 / M_PI;
					HeadingRel = BankWithHead(Utils::Constrain180(WayPoints->at(CurrentWay).HeadingTrue + MagVar));
				}
				else if (CurrentPos.Type == "TRKINTERCEPTRADIAL") {
					double AngleWind = WindDir - CurrentPos.HeadingMag;
					double AngleDrift = asin(sin(AngleWind / 180 * M_PI) * WindSpeed / TSpeed) * 180 / M_PI;
					HeadingRel = BankWithHead(Utils::Constrain180(WayPoints->at(CurrentWay).HeadingTrue + MagVar));
				}
				else {
					HeadingRel = ManHeadWithWay(&WayPoints->at(CurrentWay));
				}
			}
			double DToChange = CalcToNewWay();
			SendText("MODE: CLIMB \"" + WayPoints->at(STARPoint).Name + "\" Current: " + QString::number(CurrentPos.Distance, 'f', 3) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \nCurrent way: " + QString::number(CurrentWay) + " Type: " + CurrentPos.Type + " Name: \"" + CurrentPos.Name + "\" Total ways: " + QString::number(WayPoints->size()) + " D for head: " + QString::number(DToChange, 'f', 3) + " \n VS: " + QString::number(VSD) + " \n ATD: " + QString::number(AngleToDesc, 'f', 3) + " Current angle: " + QString::number(GetAngleToDesc(IAlt), 'f', 3),false);
		}
		else {
			//Mode = LANDING;
		}
		if ((IAlt + 100) > FlightCruise) {
			Mode = CRUISE;
		}
		if (AngleToDesc >= ((IAlt / 48148) + 2.19)) {
			//Mode = DESCENT;
		}
	}*/
	if (Mode == CRUISE) {
		QJsonObject root = document.object();
		double NPitch = (data->AllData.A32NX_FLIGHT_DIRECTOR_PITCH);
		double NBank = (data->AllData.A32NX_FLIGHT_DIRECTOR_BANK);
		double ISpeed = GetData(AIRSPEED_INDICATED);
		double GSpeed = data->GData.GROUND_VELOCITY;
		double TSpeed = GetData(AIRSPEED_TRUE);
		double SOG = GetData(SIM_ON_GROUND);
		double VSpeed = GetData(VERTICAL_SPEED);
		double Alt = GetData(INDICATED_ALTITUDE);
		double Lat = GetData(PLANE_LATITUDE, "degree");
		double Lon = GetData(PLANE_LONGITUDE, "degree");
		double WindDir = GetData(AMBIENT_WIND_DIRECTION, "degree");
		double WindSpeed = GetData(AMBIENT_WIND_VELOCITY);
		double IAlt = GetData(INDICATED_ALTITUDE);
		double MagVar = GetData(MAGVAR);
		double GAlt = data->GData.PLANE_ALT_ABOVE_GROUND;
		double Hed = data->GData.PLANE_HEADING_DEGREES_TRUE;

		//double V1 = (data->AllData.AIRLINER_V1_SPEED);
		//double VR = (data->AllData.AIRLINER_VR_SPEED);
		//double V2 = (data->AllData.AIRLINER_V2_SPEED);
		double VF = (data->AllData.A32NX_SPEEDS_F);
		double VS = (data->AllData.A32NX_SPEEDS_S);
		double VGD = (data->AllData.A32NX_SPEEDS_GD);
		double VSD;
		CurrentPos = WayPoints->at(CurrentWay);
		CurrentPos.Lat = Lat;
		CurrentPos.Lon = Lon;
		Utils::DOrtoKM(&CurrentPos);
		int icw = CurrentWay;
		double DCommon = CurrentPos.Distance;
		while ((WayPoints->at(icw).Type != "RUNWAY") && (icw < (WayPoints->size() - 1))) {
			DCommon += WayPoints->at(icw + 1).Distance;
			icw++;
		}

		if (DCommon < 20) {
			CurrentWay = icw;
		}
		if (DCommon < 15) {
			
		}
		else if (DCommon < 20) {
			
		}
		else if (DCommon < 25) {
			
		}
		else if (DCommon < 30) {
			
		}

		if (ISpeed > data->AllData.A32NX_SPEEDS_MANAGED_PFD) {
			if (ISpeed < VF + 5) {
				SendCommand(FLAPS_SET, 4.0, 0);
				SendCommand(LIGHTLANDING_SET, 1, 20);
				SendCommand(GEAR_SET, 1.0, 0);
			}
			else if (ISpeed < VS - 15) {
				SendCommand(FLAPS_SET, 3.0, 0);
			}
			else if (ISpeed < VS - 10) {
				SendCommand(GEAR_SET, 1.0, 0);
			}
			else if (ISpeed < VS + 5) {
				SendCommand(FLAPS_SET, 2.0, 0);
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			else if (ISpeed < VGD + 5) {
				SendCommand(LIGHTLANDING_SET, 0, 20);
				SendCommand(FLAPS_SET, 1.0, 0);
			}
		}
		else {
			if (ISpeed > VGD) {
				SendCommand(LIGHTLANDING_SET, 0, 20);
			}
			else if (ISpeed > VS) {
				SendCommand(FLAPS_SET, 0.0, 0);
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			else if (ISpeed > VF) {
				SendCommand(FLAPS_SET, 1.0, 0);
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			else
			{
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
		}



		if (((DCommon < 300.0) || ((CurrentWay + 1) == WayPoints->size())) && (!STAR)) {
			//AirportList ApList = AirportData->GetNearAirport(WayPoints->at(WayPoints->size() - 1).ELat, WayPoints->at(WayPoints->size() - 1).ELon);
			AirportList ApList = AirportData->GetAirportByIcao(WayPoints->at(WayPoints->size() - 1).Ident.toStdString());
			std::vector<int>* StartIndex;
			if (WindSpeed >= 0.0) {
				StartIndex = AirportData->GetRunwayStart(Lat, Lon, WindDir);
			}
			else {
				StartIndex = AirportData->GetRunwayStart(Lat, Lon, WindDir);
			}
			//std::vector<RunwayPaths> *RP = new std::vector<RunwayPaths>();

			for (int iii = 0; iii < StartIndex->size(); iii++) {
				DATA_RUNWAY runway = AirportData->GetStartCoord(StartIndex->at(iii));
				SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway.sLatitude, runway.sLongitude, runway.eHeading, 18.0);
				//CountWays = CountWays - 1;
				WayPoints->pop_back();
				STARPoint = WayPoints->size() - 1;
				//WayPoints->at(STARPoint).Name = AddSTAR(AirportData, &runway, FixSTAR, SimBriefSTAR);
				if (WayPoints->at(STARPoint).Name != "") {
					break;
				}
			}
			STAR = true;
		}
		CHAR s[512];
		if (IAlt > 10500) {
			SendCommand(LIGHTTAXI_SET, 0, 0);
			SendCommand(LIGHTRUNWAY_SET, 0, 0);
			SendCommand(BARO_MODE, 2, 0);
		}
		else if (IAlt < 9500) {
			SendCommand(LIGHTTAXI_SET, 2, 0);
			SendCommand(LIGHTRUNWAY_SET, 1, 0);
			SendCommand(BARO_MODE, 1, 0);
			//SendCommand(BARO_SEL, 0, 0);
		}
		double AngleToDesc = GetDescentAngle(WayPoints);
		//double VSD = ManVSWithAngle(AngleToDesc);
		

		if (FlightCruise < 1000) {
			FlightCruise = 1000;
		}

		if (DCommon > 17) {
			if (IAlt > 10000) {
				VSD = ManVSWithGlide(&WayPoints->at(CurrentWay), 3.0, CurrentPos.FinalAlt - 500);
			}
			else {
				VSD = ManVSWithGlide(&WayPoints->at(CurrentWay), 2.7, CurrentPos.FinalAlt - 500);
			}
			//VSD = ManVSWithAngle(AngleToDesc);
			SendCommand(ALT_SEL, FlightCruise, 20);
			if (FlightCruise > (IAlt - 100)) {
				SendCommand(PULL_ALT, 0, 0);
			}
			if (AngleToDesc >= 2.5 /*GetAngleToDesc(IAlt)*/) {
				if (FlightCruise > (IAlt - 100)) {
					if (FlightCruise != MaximumAltitude) {
						SendCommand(PULL_ALT, 0, 0);
					}
					else {
						SendCommand(PUSH_ALT, 0, 0);
					}
				}
				else {

					

					if ((IAlt - 100) > FlightCruise) {
						if ((ISpeed - 10) < data->AllData.A32NX_SPEEDS_MANAGED_PFD) {
							if (((IAlt - FlightCruise) < 1000) && (VSD < -1000)) {
								SendCommand(VS_SEL, int((FlightCruise - IAlt) / 100) * 100, 20);
							}
							else {
								if (ISpeed > 200) {
									if (VSD < -2500) {
										SendCommand(VS_SEL, -2500, 20);
									}
									else {
										SendCommand(VS_SEL, int(VSD / 100) * 100/* - 200*/, 20);
									}
								}
								else {
									if (VSD < -2000) {
										SendCommand(VS_SEL, -2000, 20);
									}
									else {
										SendCommand(VS_SEL, int(VSD / 100) * 100/* - 200*/, 20);
									}
								}
							}
						}
						else {
							if (ISpeed > 200) {
								if (VSD < -1500 + (ISpeed - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) {
									if ((-1500 + (ISpeed - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) > -500) {
										SendCommand(VS_SEL, -500, 20);
									}
									else {
										SendCommand(VS_SEL, int((-1500 + (ISpeed - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) / 100) * 100, 20);
									}
								}
								else {
									SendCommand(VS_SEL, int(VSD / 100) * 100, 20);
								}
							}
							else {
								if (VSD < -1000 + (ISpeed - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) {
									if ((-1000 + (ISpeed - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) > -500) {
										SendCommand(VS_SEL, -500, 20);
									}
									else {
										SendCommand(VS_SEL, int((-1000 + (ISpeed - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) / 100) * 100, 20);
									}
								}
								else {
									SendCommand(VS_SEL, int(VSD / 100) * 100, 20);
								}
							}
						}
					}
					else {
						SendCommand(PULL_ALT, 0, 0);
					}
				}
			}
		}
		else {
			//LANDING
			SendCommand(AUTOBRAKES_SET, 1, 0);
			VSD = ManVSWithGlide(&WayPoints->at(CurrentWay), 3.0, WayPoints->at(CurrentWay).EAltitude);
			//VSD = ManVSWithAngle(3);
			
			if (SOG) {
				if (GSpeed < 15) {
					SendCommand(SET_THROTTLE, 0, 0);
					SendCommand(SET_GSPEED, -1, 0);
					SendCommand(SPOILER_SET, 0, 0);
					SendCommand(LIGHTSTROBE_SET, 0, 0);
					SendCommand(FLAPS_SET, 0, 0);

					AirportList ApList = AirportData->GetAirportByIcao(root["destination"].toObject()["icao_code"].toString().toStdString());
					DWORD indexPath = AirportData->GetNearTaxiwayPath(Lat, Lon, Hed);
					TaxiwayParks* Park = new TaxiwayParks();
					std::vector<TPath> Path = AirportData->GetPathGate(indexPath, Hed, 15.0, Park);
					DWORD PI = Park->TaxiParkInfo;
					DWORD PIi = (PI >> 12) & 0xfff;
					DWORD PIt = (PI >> 8) & 0xf;
					DWORD PIn = PI & 0x1f;
					ParkN = new std::string(ParkType[PIt] + " " + std::to_string(PIi) + " " + ParkName[PIn]);
					WayPoints->resize(CurrentWay + 1);
					//CountWays = CurrentWay + 1;
					AddWayPoint(Lon, Lat, 0.0, "RUNWAY", "Go out", Hed, 0, 0, 0);
					for (int i = 0; i < Path.size() - 1; i++) {
						AddWayPoint(Path.at(i).Lon, Path.at(i).Lat, 0.0, TYPE_PATHS[Path[i].Type], QString(Path.at(i).name.c_str()), 0, 0, 0, 0);
					}
					AddWayPoint(Path.at(Path.size() - 1).Lon, Path.at(Path.size() - 1).Lat, 0, "GATE", QString(Path.at(Path.size() - 1).name.c_str()),0, 0, 0, 0);
					CurrentWay = CurrentWay + 2;


					Mode = TAXIIN;
				}
				else if (ISpeed > 60) {
					SendCommand(SET_THROTTLE, -13000, 0);
				}
				else if (ISpeed > 40) {
					SendCommand(SET_THROTTLE, -2500, 0);
				}
				
				else {
					SendCommand(SET_THROTTLE, 0, 0);
					SendCommand(SET_GSPEED, -1, 0);
				}
			}
			else if (GAlt < 40) {
				SendCommand(SET_THROTTLE, 0, 0);
			}
			else {
				SendCommand(ALT_SEL, 100, 20);
				SendCommand(VS_SEL, int(VSD / 100) * 100, 20);
				//SendCommand(PULL_VS, 0, 0);
				SendCommand(SPOILER_SET, -1, 0);
				SendCommand(PARKBRAKE_SET, 0, 0);
			}
		}

		double HeadingRel;
		if (SOG == 0) {
			ManPitchWithFD(NPitch);
			//AltPitchWithPos(-NPitch);
			HeadingRel = ManHeadWithWay(&WayPoints->at(CurrentWay));

		}
		else {
			ManPitchWithFD(0);
			//AltPitchWithPos(-15);
			HeadingRel = ManHeadWithWay(&WayPoints->at(CurrentWay));
		}
		
		double DToChange = CalcToNewWay();

		SendText("MODE: CRUISE \"" + WayPoints->at(STARPoint).Name + "\" Current: " + QString::number(CurrentPos.Distance, 'f', 3) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \nCurrent way: " + QString::number(CurrentWay) + " Type: " + CurrentPos.Type + " Name: \"" + CurrentPos.Name + "\" Total ways: " + QString::number(WayPoints->size()) + " D for head: " + QString::number(DToChange, 'f', 3) + " \n VS: " + QString::number(VSD) + " D to alt: " + QString::number(CurrentPos.DistToAlt, 'f', 3) + " Next alt: " + QString::number(CurrentPos.FinalAlt, 'f', 3) + " \n ATD: " + QString::number(AngleToDesc, 'f', 3) + " Current angle: " + QString::number(GetAngleToDesc(IAlt), 'f', 3), false);

	}
	 /*if (Mode == DESCENT) {
		 double NPitch = (data->AllData.A32NX_FLIGHT_DIRECTOR_PITCH);
		 double NBank = (data->AllData.A32NX_FLIGHT_DIRECTOR_BANK);
		 double ISpeed = GetData(AIRSPEED_INDICATED);
		 double TSpeed = GetData(AIRSPEED_TRUE);
		 double SOG = GetData(SIM_ON_GROUND);
		 double VSpeed = GetData(VERTICAL_SPEED);
		 //double Alt = GetData(PLANE_ALTITUDE);
		 double Lat = GetData(PLANE_LATITUDE, "degree");
		 double Lon = GetData(PLANE_LONGITUDE, "degree");
		 double WindDir = GetData(AMBIENT_WIND_DIRECTION, "degree");
		 double WindSpeed = GetData(AMBIENT_WIND_VELOCITY);
		 double IAlt = GetData(INDICATED_ALTITUDE);
		 double MagVar = GetData(MAGVAR);
		 double Baro = GetData(AMBIENT_PRESSURE);

		 //double V1 = (data->AllData.AIRLINER_V1_SPEED);
		 //double VR = (data->AllData.AIRLINER_VR_SPEED);
		 //double V2 = (data->AllData.AIRLINER_V2_SPEED);
		 double VF = (data->AllData.A32NX_SPEEDS_F);
		 double VS = (data->AllData.A32NX_SPEEDS_S);
		 double VGD = (data->AllData.A32NX_SPEEDS_GD);


		 if (ISpeed < VF + 5) {
			 SendCommand(FLAPS_SET, 4.0, 0);
			 SendCommand(LIGHTLANDING_SET, 1, 20);
			 SendCommand(GEAR_SET, 1.0, 0);
		 }
		 else if (ISpeed < VS - 15) {
			 SendCommand(FLAPS_SET, 3.0, 0);
		 }
		 else if (ISpeed < VS - 10) {
			 SendCommand(GEAR_SET, 1.0, 0);
		 }
		 else if (ISpeed < VS + 5) {
			 SendCommand(FLAPS_SET, 2.0, 0);
			 SendCommand(GEAR_SET, 0.0, 0);
		 }
		 else if (ISpeed < VGD + 5) {
			 SendCommand(LIGHTLANDING_SET, 0, 20);
			 SendCommand(FLAPS_SET, 1.0, 0);
		 }

		 if ((IAlt - 100) < FlightCruise) {
			 Mode = CRUISE;
		 }

		 if (IAlt < 9500) {
			 SendCommand(BARO_SEL, 0, 0);

		 }
		 if ((IAlt + 100) < FlightCruise) {
			 Mode = CLIMB;
		 }
		 //Programming flaps and gears TO DO

		 if (IAlt > 10500) {
			 SendCommand(LIGHTTAXI_SET, 0, 0);
			 SendCommand(LIGHTRUNWAY_SET, 0, 0);
			 SendCommand(BARO_MODE, 2, 0);
		 }
		 else if (IAlt < 9500) {
			 SendCommand(LIGHTTAXI_SET, 2, 0);
			 SendCommand(LIGHTRUNWAY_SET, 1, 0);
			 SendCommand(BARO_SEL, 0, 0);
		 }

		 if (CurrentWay >= WayPoints->size()) {
			 //CurrentWay = AppPoint;
			 //abortLanding = false;
		 }
		 CurrentPos = WayPoints->at(CurrentWay);
		 CurrentPos.Lat = Lat;
		 CurrentPos.Lon = Lon;
		 Utils::DOrtoKM(&CurrentPos);
		 int icw = CurrentWay;
		 double DCommon = CurrentPos.Distance;
		 while ((WayPoints->at(icw).Type != "RUNWAY") && (icw < (WayPoints->size() - 1))) {
			 DCommon += WayPoints->at(icw + 1).Distance;
			 icw++;
		 }
		 if (!abortLanding) {
			 if (DCommon < 15.0) {
				 SendCommand(SPOILER_SET, 0, 0);
				 flare = true;
				 VSPID.FT = true;
				 PPID.FT = true;
				 //Mode = LANDING;
			 }
			 if (DCommon < 20) {
				 
			 }
			 else if (DCommon < 25) {
				 
			 }
			 else if (DCommon < 30) {
				 
			 }
			 else if (DCommon < 35) {
				 
			 }
			 else if (DCommon > 40 && IAlt < 10000.0 && !abortLanding2) {
				 
			 }
		 }
		 else {

			 int appcount = WayPoints->size() - AppPoint;
			 int newapppoint = WayPoints->size();
			 for (int ii = 0; ii < appcount; ii++) {
				 //Ways[CountWays] = Ways[AppPoint+ii];
				 AddWayPoint(WayPoints->at(AppPoint + ii).Lon,
					 WayPoints->at(AppPoint + ii).Lat,
					 WayPoints->at(AppPoint + ii).Altitude,
					 WayPoints->at(AppPoint + ii).Type,
					 WayPoints->at(AppPoint + ii).Name,
					 WayPoints->at(AppPoint + ii).EndHeadingTrue,
					 WayPoints->at(AppPoint + ii).FixAlt,
					 WayPoints->at(AppPoint + ii).IndSpeed,
					 WayPoints->at(AppPoint + ii).H);

			 }
			 AppPoint = newapppoint;
			 abortLanding = false;
		 }
		 CHAR s[512];
		 if (((DCommon < 300.0) || ((CurrentWay + 1) == WayPoints->size())) && (!STAR)) {
			 //AirportList ApList = AirportData->GetNearAirport(WayPoints->at(WayPoints->size() - 1).ELat, WayPoints->at(WayPoints->size() - 1).ELon);
			 AirportList ApList = AirportData->GetAirportByIcao(WayPoints->at(WayPoints->size() - 1).Ident.toStdString());
			 std::vector<int>* StartIndex;
			 if (WindSpeed >= 0.0) {
				 StartIndex = AirportData->GetRunwayStart(Lat, Lon, WindDir);
			 }
			 else {
				 StartIndex = AirportData->GetRunwayStart(Lat, Lon, WindDir);
			 }
			 //std::vector<RunwayPaths> *RP = new std::vector<RunwayPaths>();

			 for (int iii = 0; iii < StartIndex->size(); iii++) {
				 DATA_RUNWAY runway = AirportData->GetStartCoord(StartIndex->at(iii));
				 SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway.sLatitude, runway.sLongitude, runway.eHeading, 18.0);
				 //CountWays = CountWays - 1;
				 STARPoint = WayPoints->size() - 1;
				 WayPoints->at(STARPoint).Name = AddSTAR(AirportData, &runway);
				 if (WayPoints->at(STARPoint).Name != "") {
					 break;
				 }
			 }
			 STAR = true;
		 }
		 double currentAngle = GetDescentAngle(WayPoints);
		 double VSD = ManVSWithAngle(currentAngle);
		 //int VS = VSD;
		 if (SetTimeOff(DESCENT, 30000)) {
			 if ((VSD + (20.0 * DCommon)) < (VS * 60.0)) {
				 SendCommand(SPOILER_SET, 1, 0);
			 }
			 else {
				 SendCommand(SPOILER_SET, 0, 0);
			 }
		 }
		 if ((CurrentPos.Type!= "RUNWAY") && ((CurrentPos.Type != "GSWAY"))) {
			 int Alt = CurrentPos.EAltitude / 100;
			 
			 if (abortLanding2 && IAlt > 1800.0) {
				 SendCommand(ALT_SEL, Alt * 100, 0);
				 abortLanding2 = false;
			 }
			 double HeadingRel;
			 if (SOG == 0) {
				 ManPitchWithFD(NPitch);


				 HeadingRel = ManHeadWithWay(&WayPoints->at(CurrentWay));

			 }
			 else {
				 HeadingRel = ManHeadWithWay(&WayPoints->at(CurrentWay));
			 }

			 double DToChange = CalcToNewWay();
			 SendText("MODE: DESCENT \"" + WayPoints->at(STARPoint).Name + "\" Current: " + QString::number(CurrentPos.Distance, 'f', 3) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \nCurrent way: " + QString::number(CurrentWay) + " Type: " + CurrentPos.Type + " Name: \"" + CurrentPos.Name + "\" Total ways: " + QString::number(WayPoints->size()) + " D for head: " + QString::number(DToChange, 'f', 3) + " \n VS: " + QString::number(VSD) + " EOD: " + QString::number(currentAngle, 'f', 3) + " Current angle: " + QString::number(GetAngleToDesc(IAlt), 'f', 3), false);
		 }
		 else {
			 SendCommand(SPOILER_SET, 0, 0);
			 VSPID.FT = true;
			 PPID.FT = true;
			 //Mode = LANDING;
		 }
	 }*/
		
	if (Quit) {
		Connect();
	}
}

bool MainLogic::SetTimeOff(int IDREQ, int TimeOffset) {
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

float MainLogic::GetAngleToDesc(float alt) {
	return (alt / 48148) + 2.0;//2.19 old(2.938 - old, 2.748 - new ) 
}

double MainLogic::GetDescentAngle(std::vector<sWayPoint>* Way) {
	if (CurrentWay != 0) {
		double MaxAlt = MaximumAltitude;
		int wi = CurrentWay;

		while ((Way->at(wi).FixAlt != CRUISEALT) && (wi < (Way->size() - 1))) {
			wi++;
		}
		if (wi != Way->size()) {
			MaximumAltitude = Way->at(wi).EAltitude;
		}
		for (int i = wi - 1; i >= CurrentWay; i--) {
			if ((MaxAlt > Way->at(i).EAltitude) && ((Way->at(i).FixAlt == ALTORBELOW) || (Way->at(i).FixAlt == FIXALT))) {
				MaxAlt = Way->at(i).EAltitude;
				
			}
		}
		FlightCruise = (int(MaxAlt / 100.0)) * 100;
		wi = CurrentWay;
		while (((Way->at(wi).FixAlt != FIXALT)) && (wi < (Way->size() - 1))) {
			wi++;
		}
		if (wi == Way->size()) {
			return -1.0;
		}
		double angle = 0.0;
		double doubleAlt = 0.0;
		int fi = wi;
		for (fi = wi; fi >= CurrentWay; fi--) {
			double D = 0.0;
			int icw;
			for (int icw = fi; icw > CurrentWay; icw--) {
				D += Way->at(icw).Distance;
			}
			D += CurrentPos.Distance;
			
			double Tangle = atan((GetData(PLANE_ALTITUDE) - Way->at(fi).EAltitude) / 3280.84 / D) * 180.0 / M_PI;
			if (doubleAlt < Way->at(fi).EAltitude) {
				doubleAlt = Way->at(fi).EAltitude;
			}
			switch (int(Way->at(fi).FixAlt)) {
			case FIXALT: {
				angle = Tangle;
				CurrentPos.FinalAlt = Way->at(fi).EAltitude;
				doubleAlt = Way->at(fi).EAltitude;
				CurrentPos.DistToAlt = D;
				break;
			}
			case ALTORBELOW: {
				if (angle < Tangle) {
					angle = Tangle;
					CurrentPos.FinalAlt = Way->at(fi).EAltitude;
					if (doubleAlt > Way->at(fi).EAltitude) {
						doubleAlt = Way->at(fi).EAltitude;
						CurrentPos.DistToAlt = D;
					}
				}
				break;
			}
			case ALTORABOVE: {
				if (angle > Tangle) {
					angle = Tangle;
					CurrentPos.FinalAlt = Way->at(fi).EAltitude;
					if (doubleAlt < Way->at(fi).EAltitude) {
						doubleAlt = Way->at(fi).EAltitude;
						CurrentPos.DistToAlt = D;
					}
				}
				break;
			}
			default: {
				break;
			}
			}
		}
		CurrentPos.Altitude = doubleAlt;
		FlightCruise = (int(doubleAlt / 100.0)) * 100;
		return angle;
	}
	return 0;
}

double MainLogic::ManVSWithAngle(double GS) {

	double TToTarget = (CurrentPos.DistToAlt) / GetData(GROUND_VELOCITY) / 1.852;
	double CurrentNeedAlt = tan(GS / 180.0 * M_PI) * CurrentPos.DistToAlt * 3280.84 + CurrentPos.EAltitude;
	double VSWay = -(CurrentNeedAlt - CurrentPos.EAltitude)/**0.0003048*/ / TToTarget / 60/**54.681*/;
	int VS = (VSWay);// +(CurrentNeedAlt - PaircraftData->alt) * 10);
	if (VS > 0) {
		VS = 0;
	}
	return VS;
}

double MainLogic::ManVSWithGlide(sWayPoint* Way, double GS, double TAlt) {
	double GSpeed = data->GData.GROUND_VELOCITY;
	double Alt = data->GData.INDICATED_ALTITUDE;
	double GAlt = data->GData.PLANE_ALT_ABOVE_GROUND;
	double TToNextWay;
	double CurrentNeedAlt;
	if (Way->Type == "RUNWAY") {
		TToNextWay = (CurrentPos.Distance - Way->Distance + 0.05) / GSpeed * 1.852;
		CurrentNeedAlt = (((tan(GS / 180 * M_PI)) * (CurrentPos.Distance - Way->Distance + 0.05)) * 3280.84) + Way->EAltitude;
	}
	else {
		TToNextWay = (CurrentPos.DistToAlt + 0.05) / GSpeed * 1.852;
		CurrentNeedAlt = (((tan(GS / 180 * M_PI)) * (CurrentPos.DistToAlt)) * 3280.84) + TAlt;
	}
	double TFromWay = Way->Distance / GSpeed * 1.852;
	//
	
	double VSWay = -(CurrentNeedAlt) * 0.0003048 / TToNextWay * 54.681;
	int VS = (VSWay + (CurrentNeedAlt - Alt) * 5);
	if (Way->Type == "RUNWAY") {
		if (VS > 0) {
			VS = 0;
		}
		else
		{
			VS = VS;
		}
	}
	else {
		if (VS > 0) {
			VS = 0;
		}
		else
		{
			VS = VS;
		}
	}
	if ((Alt - WayPoints->at(WayPoints->size() - 1).EAltitude) < -VS / 6) {
		VS = -GAlt * 6;
	}
	return VS;
}


void MainLogic::Connect() {
	Quit = FALSE;
	
	DefColorBgrnd = ui->ConnectButton->palette().color(QPalette::Background);
	DefColorTxt = ui->ConnectButton->palette().color(QPalette::WindowText);
	
	
	if ((!Connected)) {	
		emit ButtonModify(ui->ConnectButton, "Try connected...", "background: lightblue; color: darkblue; border: 1px solid darkblue");
		if (SUCCEEDED(SimConnect_Open(&HSimConnect, "MSFS 2020 AutoFlight", NULL, 0, 0, 0))) {
			
			
			emit SendLog("Connected to Simulator");
			emit ButtonModify(ui->ConnectButton, "Disconnect", "background: lightgreen; color: darkgreen; border: 1px solid darkgreen");

			SUCC(SimConnect_MapClientDataNameToID(HSimConnect, A32NX_LOCAL_DATA_NAME, A32NX_LOCAL_DATA_ID));
			SUCC(SimConnect_AddToClientDataDefinition(HSimConnect, A32NX_LOCAL_DATA_DEFINITION, 0, sizeof(ExportData)));
			SUCC(SimConnect_RequestClientData(HSimConnect, A32NX_LOCAL_DATA_ID, REQ_LOCAL_DATA, A32NX_LOCAL_DATA_DEFINITION,
						SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0));
			SUCC(SimConnect_MapClientDataNameToID(HSimConnect, A32NX_CONTROL_NAME, A32NX_CONTROL_ID));
			SUCC(SimConnect_AddToClientDataDefinition(HSimConnect, A32NX_CONTROL_DEFINITION, 0, sizeof(A32NX_Control), 0, 0));
			//SUCC(SimConnect_RequestClientData(HSimConnect, A32NX_CONTROL_ID, REQ_CONTROL, A32NX_CONTROL_DEFINITION,
//				SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0));


			HRESULT hr = (SimConnect_MapClientDataNameToID(HSimConnect, A32NX_ALLDATA_NAME, A32NX_ALLDATA_ID));
			hr = (SimConnect_AddToClientDataDefinition(HSimConnect, A32NX_ALLDATA_DEFINITION, 0, sizeof(sAllData)));
			//hr = (SimConnect_RequestClientData(HSimConnect, A32NX_ALLDATA_ID, REQ_ALL_DATA, A32NX_ALLDATA_DEFINITION,
//				SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0));


			SUCC(SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_TEXT));
			SUCC(SimConnect_RequestSystemState(HSimConnect, REQ_AIR_PATH, "AircraftLoaded"));
			SUCC(SimConnect_SubscribeToSystemEvent(HSimConnect, EVENT_SIM_START, "SimStart"));
			//SUCC(SimConnect_SubscribeToSystemEvent(HSimConnect, EVENT_6HZ, "6Hz"));
			SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_AIRCRAFT_USER, "Title", NULL, SIMCONNECT_DATATYPE_STRING256));
			//SUCC(SimConnect_MapClientEventToSimEvent(HSimConnect, A32NX_APU_FLAP_OPEN_PERCENTAGE, "Autoflight.A32NX_APU_FLAP_OPEN_PERCENTAGE"));


			




			data = new SimData(HSimConnect);
			
			TData = new ExQThread("data", utils);
			cabinWork = new CabinWork(HSimConnect);
			TCabinWork = new ExQThread("cabinWork", utils);
			cabinWork->DataT = data;
			data->moveToThread(TData);
			cabinWork->moveToThread(TCabinWork);
			
			
			TData->start();
			
			ConnectConnectors = new std::vector<QMetaObject::Connection>();
			ConnectConnectors->push_back(connect(this, SIGNAL(GetDataSignal(DWORD, DWORD, double*, char*)), data, SLOT(GetData(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(GetDataSignalL(DWORD, DWORD, double*, char*)), data, SLOT(GetDataL(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(GetDataStringSignal(DWORD, DWORD, std::string*)), data, SLOT(GetDataString(DWORD, DWORD, std::string*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SetDataSignal(DWORD, DWORD, double*, char*)), data, SLOT(SetData(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SetGetDataSignal(DWORD, DWORD, DWORD, double*, char*)), data, SLOT(SetGetData(DWORD, DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SendEventSignal(DWORD, DWORD, long)), data, SLOT(SendEvent(DWORD, DWORD, long))));

			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(GetDataSignal(DWORD, DWORD, double*, char*)), data, SLOT(GetData(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(GetDataSignalL(DWORD, DWORD, double*, char*)), data, SLOT(GetDataL(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetDataSignal(DWORD, DWORD, double*, char*)), data, SLOT(SetData(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetDataSignalL(DWORD, DWORD, double*, char*)), data, SLOT(SetDataL(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetGetDataSignal(DWORD, DWORD, DWORD, double*, char*)), data, SLOT(SetGetData(DWORD, DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetSetDataSignal(DWORD, DWORD, double*, DWORD, double*, char*, char*)), data, SLOT(SetSetData(DWORD, DWORD, double*, DWORD, double*, char*, char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendEventSignal(DWORD, DWORD, long)), data, SLOT(SendEvent(DWORD, DWORD, long))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendEventSignal2(DWORD, DWORD, long, DWORD, double, char*)), data, SLOT(SendEvent2(DWORD, DWORD, long, DWORD, double, char*))));

			ConnectConnectors->push_back(connect(planesWork, SIGNAL(GetDataSignal(DWORD, DWORD, double*, char*)), data, SLOT(GetData(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(GetDataSignalL(DWORD, DWORD, double*, char*)), data, SLOT(GetDataL(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetDataSignal(DWORD, DWORD, double*, char*)), data, SLOT(SetData(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetDataSignalL(DWORD, DWORD, double*, char*)), data, SLOT(SetDataL(DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetGetDataSignal(DWORD, DWORD, DWORD, double*, char*)), data, SLOT(SetGetData(DWORD, DWORD, DWORD, double*, char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetSetDataSignal(DWORD, DWORD, double*, DWORD, double*, char*, char*)), data, SLOT(SetSetData(DWORD, DWORD, double*, DWORD, double*, char*, char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendEventSignal(DWORD, DWORD, long)), data, SLOT(SendEvent(DWORD, DWORD, long))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendEventSignal2(DWORD, DWORD, long, DWORD, double, char*)), data, SLOT(SendEvent2(DWORD, DWORD, long, DWORD, double, char*))));


			ConnectConnectors->push_back(connect(data, SIGNAL(GetDataChange(DWORD, DWORD)), this, SLOT(GetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetDataChange(DWORD, DWORD)), this, SLOT(SetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetGetDataChange(DWORD, DWORD)), this, SLOT(SetGetDataChange(DWORD, DWORD)), Qt::DirectConnection));

			ConnectConnectors->push_back(connect(data, SIGNAL(GetDataChange(DWORD, DWORD)), cabinWork, SLOT(SetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetDataChange(DWORD, DWORD)), cabinWork, SLOT(GetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetGetDataChange(DWORD, DWORD, DWORD)), cabinWork, SLOT(SetGetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetSetDataChange(DWORD, DWORD, DWORD)), cabinWork, SLOT(SetSetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));

			ConnectConnectors->push_back(connect(data, SIGNAL(GetDataChange(DWORD, DWORD)), planesWork, SLOT(SetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetDataChange(DWORD, DWORD)), planesWork, SLOT(GetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetGetDataChange(DWORD, DWORD, DWORD)), planesWork, SLOT(SetGetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetSetDataChange(DWORD, DWORD, DWORD)), planesWork, SLOT(SetSetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));


			ConnectConnectors->push_back(connect(MainTimer, SIGNAL(timeout()), this, SLOT(TimerProc())));

			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendText(QString, bool)), utils, SLOT(SetText(QString, bool))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendLog(QString)), utils, SLOT(AddLog(QString))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendText(QString, bool)), utils, SLOT(SetText(QString, bool))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendLog(QString)), utils, SLOT(AddLog(QString))));
			ConnectConnectors->push_back(connect(data, SIGNAL(SendText(QString, bool)), utils, SLOT(SetText(QString, bool))));
			ConnectConnectors->push_back(connect(data, SIGNAL(SendLog(QString)), utils, SLOT(AddLog(QString))));


			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendCommand(DWORD, double, double)), planesWork, SLOT(ReceiveCommand(DWORD, double, double))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SendCommand(DWORD, double, double)), planesWork, SLOT(ReceiveCommand(DWORD, double, double))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SendCommand(DWORD, double, double)), cabinWork, SLOT(ReceiveCommand(DWORD, double, double))));

			
			ConnectConnectors->push_back(connect(this, SIGNAL(PreliminaryCocpitPrep(int*)), cabinWork, SLOT(PreliminaryCocpitPrep(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CocpitPreparation(int*)), cabinWork, SLOT(CocpitPreparation(int*)))); 
			ConnectConnectors->push_back(connect(this, SIGNAL(BeforeStart(int*)), cabinWork, SLOT(BeforeStart(int*)))); 
			ConnectConnectors->push_back(connect(this, SIGNAL(EngineStart(int*)), cabinWork, SLOT(EngineStart(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(AfterStart(int*)), cabinWork, SLOT(AfterStart(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(BeforeTaxi(int, int, int*)), cabinWork, SLOT(BeforeTaxi(int, int, int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(BeforeTakeoff(int*)), cabinWork, SLOT(BeforeTakeoff(int*))));
			TCabinWork->start();
			SendCommand(START_TIMER, 0, 0);
			Quit = FALSE;
			Connected = TRUE;			
			MainTimer->start(60);
			emit SendLog("Start timer");
			//data->Timer->start(60);
			//emit SendLog("Start timer");
			SendEvent(SAVE_VARS, 1);
		}
		else {
			emit SendLog("Not Connected");
			emit ButtonModify(ui->ConnectButton, "Unable to connect! Please verify the simulator`s run, and try again", "background: lightpink; color: darkred; border: 1px solid darkred");
		}
	}
	else {		
		for (int i = 0; i < ConnectConnectors->size(); i++) {
			disconnect(ConnectConnectors->at(i));
		}
		delete ConnectConnectors;
		data->Timer->stop();
		emit SendLog("Stop timer");
		MainTimer->stop();
		TCabinWork->exit();
		TCabinWork->wait();
		TData->exit();
		TData->wait();
		delete TCabinWork;
		delete cabinWork;
		delete TData;
		delete data;
		emit SendLog("Stop timer");
		Quit = TRUE;
		Disconnect();		
	}
}
void MainLogic::AddSidStarTrack(WayPointA* wayPoint, TPath* RWEnd, FIXX* fixx, DATA_RUNWAY* runway) {
	
	if (wayPoint->TypeName == "TRKUNTIL") {
		if (RWEnd == NULL) {
			AddWayPoint(runway->eLongitude, runway->eLatitude, wayPoint->ALT, "HEADINGUNTILALT", QString("(" + QString(std::to_string((DWORD)wayPoint->ALT).c_str()) + ")"), wayPoint->TRK, 0, 0.0, 0);
		}
		else {
			AddWayPoint(RWEnd->Lon, RWEnd->Lat, wayPoint->ALT, "HEADINGUNTILALT", QString("(" + QString(std::to_string((DWORD)wayPoint->ALT).c_str()) + ")"), wayPoint->TRK, 0, 0.0, 0);
		}
	}
	else if (wayPoint->TypeName == "TRKINTERCEPTRADIALTOFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TRKINTERCEPTRADIAL", QString(("(INTRAD) " + wayPoint->FIXName).c_str()), wayPoint->TRK, 0, 0.0, wayPoint->RADIAL);
	}
	else if (wayPoint->TypeName == "FIXATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0,0);
	}
	else if (wayPoint->TypeName == "FIXOVERFLYATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOW") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVESPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, wayPoint->SPEED, 0);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVESPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, wayPoint->SPEED, 0);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXATORBELOWATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIXSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "FIXOVERFLY") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIXFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, 0.0, 0);
	}
	else if (wayPoint->TypeName == "FIXFALTSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "FIXOVERFLYFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, 0.0, 0);
	}
	else if (wayPoint->TypeName == "RNW") {
		AddWayPoint(runway->sLongitude, runway->sLatitude, runway->alt, "GSWAY", QString(runway->Name.c_str()), 0, FIXALT, 0, 0);
		AddWayPoint(runway->eLongitude, runway->eLatitude, runway->alt, "RUNWAY", QString(runway->Name.c_str()), 0, FIXALT, 0, 0);
	}
	else if (wayPoint->TypeName == "FIXOVERFLYATORABOVESPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEEDLEGTIME") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXOVERFLY") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXOVERFLYFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXOVERFLYFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXOVERFLY") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, ALTORABOVE, wayPoint->SPEED, 0);
	}
	else if (wayPoint->TypeName == "TRKUNTILFROMFIXSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "HEADINGUNTILFROMDIST", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, wayPoint->SPEED, wayPoint->LegDist);
	}
	else if (wayPoint->TypeName == "TRKUNTILFROMFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "HEADINGUNTILFROMDIST", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, 0.0, wayPoint->LegDist, 0);
	}
	else {
		qDebug() << QString(wayPoint->TypeName.c_str());
	}
	/*if (wayPoint->SPEED != 0) {
		WayPoints->at(WayPoints->size() - 1).IndSpeed = wayPoint->SPEED;
	}*/
}

std::vector<TSTARS>* MainLogic::AddSTAR(SIDSTAR* sidstar, DATA_RUNWAY* runway, QString fix, QString SBSTAR) {
	std::vector<TSTARS>* STARs = new std::vector<TSTARS>();
	TSTARS STAR;
	std::string STARName = "";
	std::string APPName = "";
	std::string TRANSAPPName = "";
	std::string TRANSSTARName = "";
	std::vector<WayPointA>* wayPoint = new std::vector<WayPointA>();
	int i, j, j1, r, r2, t, t2;
	bool FindStar = false;
	bool FindAppr = false;
	bool FindTransAppr = false;
	bool FindTransStar = false;
	for (j = 0; j < sidstar->APPROACHES->size(); j++) {
		for (r = 0; r < sidstar->APPROACHES->at(j).Runways->size(); r++) {
			if (sidstar->APPROACHES->at(j).Runways->at(r) == runway->Name) {
				if (QString(sidstar->APPROACHES->at(j).WayPoints->at(0).FIXName.c_str()) == WayPoints->at(WayPoints->size() - 1).Ident) {
					t = -1;
					FindAppr = true;
					break;
				}
				else {
					if (sidstar->APPROACHES->at(j).TRANSITIONS != NULL) {
						for (t = 0; t < sidstar->APPROACHES->at(j).TRANSITIONS->size(); t++) {
							if (QString(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(0).FIXName.c_str()) == WayPoints->at(WayPoints->size() - 1).Ident) {
								FindAppr = true;
								FindTransAppr = true;
								break;
							}
						}
					}
				}
			}
		}
		if (FindAppr) {
			STAR.appr = j;
			STAR.APPROACH = sidstar->APPROACHES->at(j).Name;
			if (FindTransAppr) {
				STAR.transappr = t;
				STAR.APPTRANS = sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Name;
			}
			STAR.STAR = STAR.APPROACH;
			
			STAR.wayPoint = new std::vector<WayPointA>();
			if (FindTransAppr >= 0) {
				for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->size(); ii++) {
					STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(ii));
					STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
				}
			}
			for (int ii = 0; ii < sidstar->APPROACHES->at(j).WayPoints->size(); ii++) {
				STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).WayPoints->at(ii));
				STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = FIX;
			}
			STAR.RUNWAY = runway->Name;
			STARs->push_back(STAR);
			STAR = {};
			FindAppr = false;
			FindTransAppr = false;
			//break;
		}
	}
	if (!FindAppr) {
		for (i = 0; i < sidstar->STARS->size(); i++) {
			for (r2 = 0; r2 < sidstar->STARS->at(i).Runways->size(); r2++) {
				if (sidstar->STARS->at(i).Runways->at(r2) == runway->Name) {
					if (QString(sidstar->STARS->at(i).WayPoints->at(0).FIXName.c_str()) == WayPoints->at(WayPoints->size() - 1).Ident) {
						t2 = -1;
						for (j = 0; j < sidstar->APPROACHES->size(); j++) {
							for (r = 0; r < sidstar->APPROACHES->at(j).Runways->size(); r++) {
								if (sidstar->APPROACHES->at(j).Runways->at(r) == runway->Name) {
									std::string ss1 = sidstar->APPROACHES->at(j).WayPoints->at(0).FIXName;
									std::string ss2 = sidstar->STARS->at(i).WayPoints->at(sidstar->STARS->at(i).WayPoints->size() - 1).FIXName;
									if (sidstar->APPROACHES->at(j).WayPoints->at(0).FIXName ==
										sidstar->STARS->at(i).WayPoints->at(sidstar->STARS->at(i).WayPoints->size() - 1).FIXName) {
										t = -1;
										FindAppr = true;
										FindStar = true;
										break;
									}
									else {
										if (sidstar->APPROACHES->at(j).TRANSITIONS != NULL) {
											for (t = 0; t < sidstar->APPROACHES->at(j).TRANSITIONS->size(); t++) {
												if (sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(0).FIXName ==
													sidstar->STARS->at(i).WayPoints->at(sidstar->STARS->at(i).WayPoints->size() - 1).FIXName) {
													FindAppr = true;
													FindStar = true;
													FindTransAppr = true;
													break;
												}
											}
										}
									}
								}
							}
							if (FindAppr) {
								STAR.star = i;
								STAR.STAR = sidstar->STARS->at(i).Name;
								STAR.appr = j;
								STAR.APPROACH = sidstar->APPROACHES->at(j).Name;
								if (FindTransAppr) {
									STAR.transappr = t;
									STAR.APPTRANS = sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Name;
								}
								STAR.wayPoint = new std::vector<WayPointA>();
								for (int ii = 0; ii < sidstar->STARS->at(i).WayPoints->size() - 1; ii++) {
									STAR.wayPoint->push_back(sidstar->STARS->at(i).WayPoints->at(ii));
									STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = FIX;
								}
								if (FindTransAppr) {
									for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->size(); ii++) {
										STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(ii));
										STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
									}
								}
								for (int ii = 0; ii < sidstar->APPROACHES->at(j).WayPoints->size(); ii++) {
									STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).WayPoints->at(ii));
									STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = FIX;
								}
								STAR.RUNWAY = runway->Name;
								STARs->push_back(STAR);
								STAR = {};
								FindAppr = false;
								FindStar = false;
								FindTransAppr = false;
								//break;
							}
						}
						//break;
					}
					else {
						if (sidstar->STARS->at(i).TRANSITIONS != NULL) {
							for (t2 = 0; t2 < sidstar->STARS->at(i).TRANSITIONS->size(); t2++) {
								if (QString(sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->at(0).FIXName.c_str()) == WayPoints->at(WayPoints->size() - 1).Ident) {
									for (j1 = 0; j1 < sidstar->APPROACHES->size(); j1++) {
										for (r = 0; r < sidstar->APPROACHES->at(j1).Runways->size(); r++) {
											if (sidstar->APPROACHES->at(j1).Runways->at(r) == runway->Name) {
												if (sidstar->APPROACHES->at(j1).WayPoints->at(0).FIXName ==
													sidstar->STARS->at(i).WayPoints->at(sidstar->STARS->at(i).WayPoints->size() - 1).FIXName) {
													t = -1;
													FindAppr = true;
													FindStar = true;
													FindTransStar = true;
													j = j1;
													break;
												}
												else {
													if (sidstar->APPROACHES->at(j1).TRANSITIONS != NULL)
													{
														for (t = 0; t < sidstar->APPROACHES->at(j1).TRANSITIONS->size(); t++)

														{
															std::string ss1 = sidstar->APPROACHES->at(j1).TRANSITIONS->at(t).WayPoints->at(0).FIXName;
															std::string ss2 = sidstar->STARS->at(i).WayPoints->at(sidstar->STARS->at(i).WayPoints->size() - 1).FIXName;
															if (sidstar->APPROACHES->at(j1).TRANSITIONS != NULL)
															{
																if (sidstar->APPROACHES->at(j1).TRANSITIONS->at(t).WayPoints->at(0).FIXName ==
																	sidstar->STARS->at(i).WayPoints->at(sidstar->STARS->at(i).WayPoints->size() - 1).FIXName)
																{
																	FindAppr = true;
																	FindStar = true;
																	FindTransStar = true;
																	FindTransAppr = true;
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
											
											STAR.star = i;
											STAR.STAR = sidstar->STARS->at(i).Name;
											STAR.appr = j;
											STAR.APPROACH = sidstar->APPROACHES->at(j).Name;
											if (FindTransAppr) {
												STAR.transappr = t;
												STAR.APPTRANS = sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Name;
											}
											if (FindTransStar) {
												STAR.transstar = t2;
												STAR.STARTRANS = sidstar->STARS->at(i).TRANSITIONS->at(t2).Name;
											}
											STAR.wayPoint = new std::vector<WayPointA>();
											if (FindTransStar) {
												for (int ii = 0; ii < sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->size(); ii++) {
													STAR.wayPoint->push_back(sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->at(ii));
													STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
												}
											}
											for (int ii = 0; ii < sidstar->STARS->at(i).WayPoints->size() - 1; ii++) {
												STAR.wayPoint->push_back(sidstar->STARS->at(i).WayPoints->at(ii));
												STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = FIX;
											}
											if (FindTransAppr) {
												for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->size(); ii++) {
													STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(ii));
													STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
												}
											}
											for (int ii = 0; ii < sidstar->APPROACHES->at(j).WayPoints->size(); ii++) {
												STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).WayPoints->at(ii));
												STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = FIX;
											}
											STAR.RUNWAY = runway->Name;
											STARs->push_back(STAR);
											STAR = {};
											FindAppr = false;
											FindStar = false;
											FindTransAppr = false;
											FindTransStar = false;
											//break;
										}
									}
									//break;
								}
							}
						}
					}
				}
			}
			if (FindStar) {
				//break;
			}
		}
	}
	/*if (FindStar) {
		if (t2 >= 0) {
			for (int ii = 0; ii < sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->size(); ii++) {
				wayPoint->push_back(sidstar->STARS->at(i).TRANSITIONS->at(t2).WayPoints->at(ii));
				wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
			}
		}
		for (int ii = 0; ii < sidstar->STARS->at(i).WayPoints->size() - 1; ii++) {
			wayPoint->push_back(sidstar->STARS->at(i).WayPoints->at(ii));
			wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
		}
		STARName = sidstar->STARS->at(i).Name;
	}
	if (FindAppr) {
		if (t >= 0) {
			for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->size(); ii++) {
				wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).WayPoints->at(ii));
				wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
			}
		}
		AppPoint = wayPoint->size();
		for (int ii = 0; ii < sidstar->APPROACHES->at(j).WayPoints->size(); ii++) {
			wayPoint->push_back(sidstar->APPROACHES->at(j).WayPoints->at(ii));
			wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
		}
		STARName = STARName + " - " + sidstar->APPROACHES->at(j).Name;
	}
	else {*/
	if (STARs->size() == 0) {
		SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway->sLatitude, runway->sLongitude, runway->eHeading, 18.0);
		AddWayPoint(latlon.Longitude, latlon.Latitude, WayPoints->at(WayPoints->size() - 1).EAltitude + 2000.0, "FIX", "GS 18.0 " + QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		AddWayPoint(runway->sLongitude, runway->sLatitude, WayPoints->at(WayPoints->size() - 1).EAltitude + 2000.0, "WAYPOINT", "FINAL " + QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		AddWayPoint(runway->sLongitude, runway->sLatitude, WayPoints->at(WayPoints->size() - 1).EAltitude, "GSWAY", QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		AddWayPoint(runway->eLongitude, runway->eLatitude, WayPoints->at(WayPoints->size() - 1).EAltitude, "RUNWAY", QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		STARName = "GS 18.0 ";
	}
	/*for (int i = 0; i < wayPoint->size(); i++) {
		FIXX fixx = {};
		bool FindFIX = false;
		for (int fix = 0; fix < sidstar->FIXES->size(); fix++) {
			if (sidstar->FIXES->at(fix).Name == wayPoint->at(i).FIXName) {
				FindFIX = true;
				fixx = sidstar->FIXES->at(fix);
				break;
			}
		}
		if (i == AppPoint) {
			AppPoint = WayPoints->size()-1;
		}
		AddSidStarTrack(&wayPoint->at(i), NULL, &fixx, runway);
	}*/
	delete(wayPoint);
	wayPoint = NULL;
	//return QString(STARName.c_str());
	return STARs;
}

std::vector<TSIDS>* MainLogic::AddSID(SIDSTAR* sidstar, TPath* RWEnd, double RWHead, QString fix, QString SBSID) {
	
	std::vector<TSIDS>* SIDss = new std::vector<TSIDS>();
	std::string SIDName = "";
	std::string TRANSITIONName = "";
	
	int j, r, t;
	bool FindSid = false;
	bool FindTrans = false;
	for (j = 0; j < sidstar->SIDS->size(); j++) {
		std::vector<WayPointA>* wayPoint = new std::vector<WayPointA>();
		for (r = 0; r < sidstar->SIDS->at(j).Runways->size(); r++) {
			if (sidstar->SIDS->at(j).Runways->at(r) == RWEnd->name) {
				if (QString(sidstar->SIDS->at(j).WayPoints->at(sidstar->SIDS->at(j).WayPoints->size() - 1).FIXName.c_str()) == fix) {
					t = -1;
					FindSid = true;
					break;
				}
				else {
					if (sidstar->SIDS->at(j).TRANSITIONS != NULL) {
						for (t = 0; t < sidstar->SIDS->at(j).TRANSITIONS->size(); t++) {
							if (QString(sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->at(sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->size() - 1).FIXName.c_str()) == fix) {
								FindSid = true;
								FindTrans = true;
								break;
							}
						}
					}
				}
			}
		}
		if (FindSid) {
			for (int i = 0; i < sidstar->SIDS->at(j).WayPoints->size(); i++) {
				wayPoint->push_back(sidstar->SIDS->at(j).WayPoints->at(i));
				wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
			}
			if (t >= 0) {
				for (int i = 0; i < sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->size(); i++) {
					wayPoint->push_back(sidstar->SIDS->at(j).TRANSITIONS->at(t).WayPoints->at(i));
					wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
				}
			}
			SIDName = sidstar->SIDS->at(j).Name;
			if (FindTrans) {
				TRANSITIONName = sidstar->SIDS->at(j).TRANSITIONS->at(t).Name;
			}
			SIDss->push_back({ t, j, wayPoint, SIDName ,TRANSITIONName,RWEnd->name });
			FindSid = false;
			FindTrans = false;
			//break;
		}
	}
	if (SIDss->size() == 0) {
		SIDss->push_back({ -1, -1, NULL, "", "", RWEnd->name });
		AddWayPoint(RWEnd->Lon, RWEnd->Lat, 1750, "HEADINGUNTILALT", "(1750)", RWHead - GetData(MAGVAR), 0, 0.0, 0);
		AddWayPoint(0, 0, FlightCruise, "FIX", "", 0, 0, 0, 0, &ja[0].toObject());
	}
	else {
		
	}
	/*else {
		for (int i = 0; i < wayPoint->size(); i++) {
			FIXX fixx = {};
			bool FindFIX = false;
			for (int fix = 0; fix < sidstar->FIXES->size(); fix++) {
				if (sidstar->FIXES->at(fix).Name == wayPoint->at(i).FIXName) {
					FindFIX = true;
					fixx = sidstar->FIXES->at(fix);
					break;
				}
			}
			AddSidStarTrack(&wayPoint->at(i), RWEnd, &fixx);
		}
	}*/
	//delete(wayPoint);
	//wayPoint = NULL;
	return SIDss;
}

double MainLogic::AltPitchWithPos(double TargetValue) {
	double pitch = -GetData(PLANE_PITCH_DEGREES);
	alterPid lp = PPID;
	if (lp.FT) {
		lp.ePnm1 = 0.0;
		lp.eDfnm2 = lp.eDfnm1 = 0.0;
		lp.Unm1 = GetData(ELEVATOR_POSITION);
		lp.FT = false;
		TargetValue = pitch;
	}
	lp.Kp = pidPitch.pGain;
	lp.Ti = pidPitch.iGain;
	lp.Td = pidPitch.dGain;
	clock_t eTime = clock();
	double Ta = (double)(eTime - lp.time) / 1000.0;
	lp.time = eTime;
	if (Ta <= 0.00833333) {
		return -1.0;
	}
	double rn = TargetValue;
	double yn = pitch;
	lp.ePn = lp.b * rn - yn;
	double en = rn - yn;
	lp.eDfn = 0.0;
	if (lp.Td > 0) {
		double eDn = lp.g * rn - yn;
		double Tf = lp.a * lp.Td;
		lp.eDfn = lp.eDfnm1 / (Ta / Tf + 1) + eDn * (Ta / Tf) / (Ta / Tf + 1);
	}
	else {
		lp.eDfnm2 = lp.eDfnm1 = lp.eDfn = 0.0;
	}
	double DUn = 0.0;
	if (lp.Ti > 0.0) {
		DUn = lp.Kp * ((lp.ePn - lp.ePnm1) + ((Ta / lp.Ti) * en) + ((lp.Td / Ta) * (lp.eDfn - 2 * lp.eDfnm1 + lp.eDfnm2)));
	}
	if (DUn > (lp.uMax - lp.Unm1)) {
		DUn = (lp.uMax - lp.Unm1);
	}
	else if (DUn < (lp.uMin - lp.Unm1)) {
		DUn = (lp.uMin - lp.Unm1);
	}
	lp.eDfnm2 = lp.eDfnm1;
	lp.eDfnm1 = lp.eDfn;
	lp.ePnm1 = lp.ePn;
	if (DUn > 0.1) {
		DUn = 0.1;
	}
	else if (DUn < -0.1) {
		DUn = -0.1;
	}
	double Un = lp.Unm1 + DUn;
	lp.Unm1 = Un;
	lp.ePnm1 = lp.ePn;
	lp.eDfnm2 = lp.eDfnm1;
	lp.eDfnm1 = lp.eDfn;
	double intpParameter = Un;
	SendCommand(SET_ELEVATOR, intpParameter*10000, 20);
	//SimConnect_SetDataOnSimObject(hSimConnect, DEF_ELEVATOR, 0, 0, 0, sizeof(intpParameter), &intpParameter);
	pidPitch.Term = -Un * 30.0 * 0.5;
	pidPitch.val = pitch;
	pidPitch.inVal = -TargetValue;
	PPID = lp;
	return (Un);
}

double MainLogic::PBHeadWithWay(sWayPoint* Way, bool PB) {

	double EndHead = WayPoints->at(CurrentWay).EndHeadingTrue;
	double EangleRel = Utils::Constrain180(CurrentPos.EndHeadingTrue - EndHead);
	double a = Utils::GetFixDA(sin(EangleRel * M_PI / 180) * CurrentPos.Distance, EangleRel);
	double HeadingRel;
	EangleRel = a * 1000;
	EangleRel = Utils::AngleLimitS(EangleRel, 60);
	double EangleRel2 = EangleRel;
	EangleRel = Utils::Constrain180(EndHead + EangleRel + 180.0);
	CHAR s[256];
	
	HeadingRel = RudWithHead(EangleRel);
	sprintf(s, "EndHead: %.3f CurrentPos.EH: %.3f EangleRel1: %.3f a: %.3f HeadingRel: %.3f\n EangleRel: %.3f\n CurrAngle: %.3f", EndHead, CurrentPos.EndHeadingTrue, Utils::Constrain180(CurrentPos.EndHeadingTrue - EndHead), a, HeadingRel, EangleRel, Utils::Constrain180(GetData(PLANE_HEADING_DEGREES_TRUE)));
	SendText(QString(s), false);
	return HeadingRel;
}

double MainLogic::ManHeadWithWay(sWayPoint* Way) {
	double EndHead;
	if ((CurrentPos.Type == TYPE_PATHS[FIXHEADING])) {
		EndHead = WayPoints->at(CurrentWay).EndHeadingTrue + GetData(MAGVAR);
	}
	else if (CurrentPos.Type == TYPE_PATHS[FIX]) {
		EndHead = CurrentPos.EndHeadingTrue;
	}
	else if (CurrentPos.Type == TYPE_PATHS[TURNLEFTDIRECTFIX]) {
		EndHead = CurrentPos.EndHeadingTrue;
	}
	else if (CurrentPos.Type == TYPE_PATHS[TURNRIGHTDIRECTFIX]) {
		EndHead = CurrentPos.EndHeadingTrue;
	}
	else {
		EndHead = WayPoints->at(CurrentWay).EndHeadingTrue;
	}
	double EangleRel = Utils::Constrain180(CurrentPos.EndHeadingTrue - EndHead);
	double a = Utils::GetFixDA(sin(EangleRel * M_PI / 180) * CurrentPos.Distance, EangleRel);
	double HeadingRel;
	if ((GetData(SIM_ON_GROUND)== 1)) {
		EangleRel = a * 1000;
		if (GetData(GROUND_VELOCITY) < 2) {
			EangleRel = a * 50000;
		}
		if (GetData(GROUND_VELOCITY) < 5) {
			EangleRel = a * 10000;
		}
		if (GetData(GROUND_VELOCITY) < 7) {
			EangleRel = a * 3000;
		}
		double EangleRel1 = EangleRel;
		EangleRel = Utils::AngleLimitS(EangleRel, 60);
		EangleRel = Utils::Constrain180(EndHead + EangleRel);
		HeadingRel = RudWithHead(EangleRel);
	}
	else {
		if ((CurrentPos.Type == TYPE_PATHS[GSWAY]) || (CurrentPos.Type == TYPE_PATHS[RUNWAY])) {
			EangleRel = a * 500 / CurrentPos.Distance;
		}
		else {
			EangleRel = a * 10;
		}
		double EangleRel1 = EangleRel;
		EangleRel = Utils::AngleLimitS(EangleRel, 60);
		double AngleWind = GetData(AMBIENT_WIND_DIRECTION) - CurrentPos.HeadingTrue;
		double AngleDrift = asin(sin(AngleWind / 180 * M_PI) * GetData(AMBIENT_WIND_VELOCITY) / GetData(AIRSPEED_TRUE))* 180 / M_PI;
		EangleRel = Utils::Constrain180(EndHead + EangleRel + AngleDrift);
		int EangleRel2 = Utils::Constrain180(EangleRel - GetData(MAGVAR));
		//SendCommand(HDG_SEL, EangleRel2, 0);
		HeadingRel = BankWithHead(EangleRel);
	}
	return HeadingRel;
}

double MainLogic::RudWithHead(double Heading) {

	static double sHed = GetData(PLANE_HEADING_DEGREES_TRUE, "Degrees");
	double eHed = GetData(PLANE_HEADING_DEGREES_TRUE, "Degrees");
	double HeadingRel = Utils::Constrain180(Heading - GetData(PLANE_HEADING_DEGREES_TRUE, "Degrees"));

	double parameter = HeadingRel / 4; //-45.0 - +45.0
	double rud = GetData(RUDDER_PEDAL_POSITION) * 16383;
	if (GetData(GROUND_VELOCITY) > 5) {

		if (GetData(AIRSPEED_INDICATED) > 60) {
			rud = rud - (100 * (GetData(ROTATION_VELOCITY_BODY_Y, "degrees per second") - parameter) * (GetData(GROUND_VELOCITY) / 50));
		}
		else {
			rud = rud - (100 * (GetData(ROTATION_VELOCITY_BODY_Y, "degrees per second") - parameter) * (GetData(GROUND_VELOCITY) / 5));
		}
	}
	else {
		rud = rud - (500.0 * (GetData(ROTATION_VELOCITY_BODY_Y, "degrees per second") - parameter));
	}
	int intParameter = -Utils::Constrain(rud, -16383, 16383);
	if (Mode == PUSHBACK || Mode == STARTPUSHBACK) {
		intParameter = -intParameter;
		SendEvent(KEY_TUG_HEADING, (0xFFFFFFFF / 360) * Heading);
	}
	SendCommand(SET_RUDDER, intParameter, 0);

	sHed = GetData(PLANE_HEADING_DEGREES_TRUE, "Degrees");
	return HeadingRel;
}

double MainLogic::CalcToNewWay(bool changeWay) {
	double REangle = 0.045;
	DWORD Curr = CurrentWay;
	double SOG = data->GData.SIM_ON_GROUND;
	if ((CurrentWay + 1) < WayPoints->size()) {
		REangle = WayPoints->at(CurrentWay + 1).HeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue;
	}
	REangle = Utils::Constrain180(REangle);
	REangle = fabs(-REangle * 5 * GetData(AIRSPEED_INDICATED) / 100000);
	if (GetData(SIM_ON_GROUND) == 0) {
		if (REangle < 2.0) {
			REangle = 2.0;
		}
	}
	
	double EangleRel = CurrentPos.EndHeadingTrue - WayPoints->at(CurrentWay).EndHeadingTrue;
	EangleRel = Utils::Constrain180(EangleRel);
	if ((CurrentPos.Type == "HEADINGUNTILALT")) {
		if ((GetData(PLANE_ALTITUDE) > CurrentPos.EAltitude)) {
			if (WayPoints->at(CurrentWay+1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5)&&(GetData(AIRSPEED_INDICATED)>1)) { //TO DO
					SendCommand(PULL_SPD, 1, 0);
					SendCommand(SPD_SEL, WayPoints->at(CurrentWay + 1).IndSpeed, 0);
				}
				else {
					SendCommand(PUSH_SPD, 1, 0);
				}
			}
			else {
				SendCommand(PUSH_SPD, 1, 0);
			}
			Curr = Curr + 1;
		}
	}
	else if (CurrentPos.Type == "HEADINGUNTILFROMDIST") {
		if (CurrentPos.Distance > CurrentPos.Dist) {
			if (WayPoints->at(CurrentWay+1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (GetData(AIRSPEED_INDICATED) > 1)) { //TO DO
					SendCommand(PULL_SPD, 1, 0);
					SendCommand(SPD_SEL, WayPoints->at(CurrentWay + 1).IndSpeed, 0);
				}
				else {
					SendCommand(PUSH_SPD, 1, 0);
				}
			}
			else {
				SendCommand(PUSH_SPD, 1, 0);
			}
			Curr = Curr + 1;
		}
	}
	else if (CurrentPos.Type == "TRKINTERCEPTRADIAL") {
		static int fl;
		if (abs(abs(CurrentPos.EndHeadingTrue) - abs(WayPoints->at(CurrentWay).Dist)) < fl) {
			if (abs(abs(CurrentPos.EndHeadingTrue) - abs(WayPoints->at(CurrentWay).Dist)) < 1.0) {
				if (WayPoints->at(CurrentWay+1).IndSpeed > 0) {
					if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (GetData(AIRSPEED_INDICATED) > 1)) { //TO DO
						SendCommand(PULL_SPD, 1, 0);
						SendCommand(SPD_SEL, WayPoints->at(CurrentWay + 1).IndSpeed, 0);
					}
					else {
						SendCommand(PUSH_SPD, 1, 0);
					}
				}
				else {
					SendCommand(PUSH_SPD, 1, 0);
				}
				Curr = Curr + 1;
				fl = 0;
			}
			if ((CurrentPos.Distance < REangle)) {
				if (WayPoints->at(CurrentWay+1).IndSpeed > 0) {
					if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (GetData(AIRSPEED_INDICATED) > 1)) { //TO DO
						SendCommand(PULL_SPD, 1, 0);
						SendCommand(SPD_SEL, WayPoints->at(CurrentWay + 1).IndSpeed, 0);
					}
					else {
						SendCommand(PUSH_SPD, 1, 0);
					}
				}
				else {
					SendCommand(PUSH_SPD, 1, 0);
				}
				Curr = Curr + 1;
				fl = 0;
			}
		}
		else {
			if (WayPoints->at(CurrentWay+1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (GetData(AIRSPEED_INDICATED) > 1)) { //TO DO
					SendCommand(PULL_SPD, 1, 0);
					SendCommand(SPD_SEL, WayPoints->at(CurrentWay + 1).IndSpeed, 0);
				}
				else {
					SendCommand(PUSH_SPD, 1, 0);
				}
			}
			else {
				SendCommand(PUSH_SPD, 1, 0);
			}
			Curr = Curr + 1;
			fl = 0;
		}
		fl = abs(abs(CurrentPos.EndHeadingTrue) - abs(WayPoints->at(CurrentWay).Dist)) < fl;
	}
	else if ((CurrentPos.Type == "FIX") || (CurrentPos.Type == "TURNLEFTDIRECTFIX") || (CurrentPos.Type == "TURNRIGHTDIRECTFIX")) {
		if ((CurrentPos.Distance < REangle)) {
			if (WayPoints->at(CurrentWay+1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (GetData(AIRSPEED_INDICATED) > 1)) { //TO DO
					SendCommand(PULL_SPD, 1, 0);
					SendCommand(SPD_SEL, WayPoints->at(CurrentWay + 1).IndSpeed, 0);
				}
				else {
					SendCommand(PUSH_SPD, 1, 0);
				}
			}
			else {
				SendCommand(PUSH_SPD, 1, 0);
			}
			Curr = Curr + 1;
		}
	}
	else if (changeWay) {
		if ((EangleRel > 60) || (EangleRel < -60) || (CurrentPos.Distance < REangle)) {
			if (WayPoints->at(CurrentWay+1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (GetData(AIRSPEED_INDICATED) > 1)) { //TO DO
					SendCommand(PULL_SPD, 1, 0);
					SendCommand(SPD_SEL, WayPoints->at(CurrentWay + 1).IndSpeed, 0);
				}
				else {
					SendCommand(PUSH_SPD, 1, 0);
				}
			}
			else {
				SendCommand(PUSH_SPD, 1, 0);
			}
			Curr = Curr + 1;
		}
	}
	if ((Mode == TAKEOFF) && (GetData(INDICATED_ALTITUDE) > 500)) {
		CurrentWay = Curr;
	}
	else if ((Mode == CRUISE) && (SOG)) {
		CurrentWay = CurrentWay;
	}
	else if (CurrentPos.Type == "RUNWAY") {
		CurrentWay = CurrentWay;
	}
	else if (Mode != TAKEOFF) {
		CurrentWay = Curr;
	}
	return REangle;
}

void MainLogic::ManPitchWithFD(double NPitch) {
	static double NewPitch = GetData(ELEVATOR_POSITION);
	static int countFails = 0;
	if (countFails >= 5) {
		countFails = 0;
		SetDataPitch = true;
	}
	if (round(NewPitch * 1000000) == round(NewPitch * 1000000)) {
		SetDataPitch = true;
	}
	if (SetDataPitch) {
		static clock_t sTime = 0; //Начальное время
		clock_t eTime = clock();
		static double sPitch;
		static double sHorBar;
		double ePitch = GetData(PLANE_PITCH_DEGREES, "Degrees");
		double eHorBar = NPitch;
		double timeOff = eTime - sTime;
		if (timeOff == 0) {
			timeOff = 0.000001;
		}
		double pitchOff = ePitch - sPitch;
		double HorBarOff = eHorBar - sHorBar;
		double pitchA = (1000 / timeOff) * pitchOff;
		double HorBarA = (1000 / timeOff) * HorBarOff;
		double PitchRel = NPitch - ePitch;
		if (PitchRel > 180) {
			PitchRel = PitchRel - 360;
		}
		else if (PitchRel < -180) {
			PitchRel = PitchRel + 360;
		}
		double pparameter = (PitchRel / 10) + HorBarA * 1;
		double elev = NewPitch * 16383;
		double rel = ((pparameter - pitchA) * 500);
		if (rel < -500) {
			rel = -500;
		}
		else if (rel > 500) {
			rel = 500;
		}
		elev = elev + rel;
		if (elev > 16383) {
			elev = 16383;
		}
		else if (elev < -16383) {
			elev = -16383;
		}
		double intpParameter = elev / 16383;
		/*if ((intpParameter < -1.0) && (intpParameter > 1.0)) {
			intpParameter = 0.0;
		}*/
		/*SendText("elev: " + QString::number(elev, 'f', 3) + "\n" + 
			"NewPitch: " + QString::number(NPitch, 'f', 3) + "\n" +
			"Pitch: " + QString::number(ePitch, 'f', 3) + "\n"
			, false);*/
		SendCommand(SET_ELEVATOR, intpParameter * 16383, 0);
		sTime = clock();
		sPitch = ePitch;
		sHorBar = NPitch;
		NewPitch = intpParameter;
		SetDataPitch = false;
	}
	else {
		countFails++;
	}
}

void MainLogic::ManBankWithFD(double NNBank) {

	double NBank = NNBank;
	if (NBank > 30) {
		NBank = 30;
	}
	else if (NBank < -30) {
		NBank = -30;
	}
	static double NewBank = GetData(AILERON_POSITION);
	static int countFails = 0;
	//SendText("ailer: " + QString::number(GetData(YOKE_X_POSITION), 'f', 3) + "\n" + "NewBank: " + QString::number(NewBank, 'f', 3) + "\n", false);
	if (countFails >= 5) {
		countFails = 0; //AILERON_POSITION
		SetDataBank = true;
	}
	if (round(10000 * NewBank) == round(10000 * NewBank)) {
		SetDataBank = true;
	}
	if (SetDataBank) {
		static clock_t sTime = 0; //Начальное время
		clock_t eTime = clock();
		static double sBank;
		static double sVertBar; //BAR
		double eBank = GetData(PLANE_BANK_DEGREES, "Degrees");
		double eVertBar = NNBank; //BAR
		double timeOff = eTime - sTime;
		double bankOff = eBank - sBank;
		double VertBarOff = eVertBar - sVertBar; //BAR
		double bankA = (1000 / timeOff) * bankOff;
		double VertBarA = (1000 / timeOff) * VertBarOff; //BAR		
		double BankRel = NBank - /*(NBank / 15) - */eBank;
		if (BankRel > 180) {
			BankRel = BankRel - 360;
		}
		else if (BankRel < -180) {
			BankRel = BankRel + 360;
		}
		double pparameter = -(BankRel / 1);// -VertBarA;
		double ailer = NewBank * 16383;
		/*SendText("ailer: " + QString::number(ailer, 'f', 3) + "\n" +
			"NewBank: " + QString::number(NBank, 'f', 3) + "\n" +
			"Bank: " + QString::number(eBank, 'f', 3) + "\n"
			, false);*/
		double rel = ((pparameter - bankA) * 500);
		if (rel < -500) {
			rel = -500;
		}
		else if (rel > 500) {
			rel = 500;
		}
		ailer = ailer + rel;
		if (ailer > (16383 / 4.16666666666666667)) {
			ailer = 16383 / 4.16666666666666667;
		}
		else if (ailer < (-16383 / 4.16666666666666667)) {
			ailer = -16383 / 4.16666666666666667;
		}
		double intpParameter = ailer / 16383;// / 0.40000001;
		SendCommand(SET_AILERON, intpParameter * 16383, 0);
		sTime = clock();
		sBank = eBank;
		sVertBar = NNBank;
		NewBank = intpParameter;
		SetDataBank = false;
	}
	else
	{
		countFails++;
	}
}

double MainLogic::BankWithHead(double Heading) {
	static clock_t sTime = 0; //Начальное время
	clock_t eTime = clock();
	static double sHed = GetData(PLANE_HEADING_DEGREES_TRUE, "Degrees");
	double eHed = GetData(PLANE_HEADING_DEGREES_TRUE, "Degrees");
	double aHed = (eHed - sHed) / (double)((double)(eTime - sTime) / 1000);
	double HeadingRel = Heading - eHed;
	if (HeadingRel > 180) {
		HeadingRel = HeadingRel - 360;
	}
	else if (HeadingRel < -180) {
		HeadingRel = HeadingRel + 360;
	}
	double parameter = HeadingRel / 4;
	double rud = GetData(PLANE_BANK_DEGREES, "Degrees");
	rud = rud - (5 * (GetData(DELTA_HEADING_RATE, "degrees per second") - parameter));
	if ((HeadingRel > 60) || (HeadingRel < -60)) {
		if (CurrentPos.Type == "TURNRIGHTDIRECTFIX") {
			HeadingRel = Utils::GetFixDA(HeadingRel, 1);
		}
		else if (CurrentPos.Type == "TURNLEFTDIRECTFIX") {
			HeadingRel = Utils::GetFixDA(HeadingRel, -1);
		}
	}
	rud = HeadingRel * 2;
	if (rud > 30) {
		rud = 30;
	}
	else if (rud < -30) {
		rud = -30;
	}
	int intParameter = rud;
	AltBankWithPos(-intParameter);
	sTime = clock();
	sHed = GetData(PLANE_HEADING_DEGREES_TRUE, "Degrees");
	return HeadingRel;
}

double MainLogic::AltBankWithPos(double TargetValue) {
	double pTerm = 0, dTerm = 0, iTerm = 0;
	double error = TargetValue - GetData(PLANE_BANK_DEGREES, "Degrees");
	pTerm = pidBank.pGain * error;    // calculate the proportional term
	pidBank.iState += error;          // calculate the integral state with appropriate limiting
	if (pidBank.iState > pidBank.iMax) {
		pidBank.iState = pidBank.iMax;
	}
	else if (pidBank.iState < pidBank.iMin) {
		pidBank.iState = pidBank.iMin;
	}
	iTerm = pidBank.iGain * pidBank.iState;    // calculate the integral term
	dTerm = pidBank.dGain * (GetData(PLANE_BANK_DEGREES, "Degrees") - pidBank.dState);
	pidBank.dState = GetData(PLANE_BANK_DEGREES, "Degrees");
	double ailer = pTerm + iTerm - dTerm;
	if (ailer > (16383 / 4.16666666666666667)) {
		ailer = 16383 / 4.16666666666666667;
	}
	else if (ailer < (-16383 / 4.16666666666666667)) {
		ailer = -16383 / 4.16666666666666667;
	}
	double intpParameter = -ailer / 16383.0;
	//SimConnect_SetDataOnSimObject(hSimConnect, DEF_AILERON, 0, 0, 0, sizeof(intpParameter), &intpParameter);
	SendCommand(SET_AILERON, -intpParameter * 16383, 0);
	pidBank.Term = ailer / 16383.0 * 30.0 * 4.16666666666666667;
	pidBank.val = GetData(PLANE_BANK_DEGREES, "Degrees");
	pidBank.inVal = TargetValue;
	return (pTerm + iTerm - dTerm);
}