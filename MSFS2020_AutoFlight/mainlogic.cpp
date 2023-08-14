//------------------------------------------------------------------------------
//
//  PMDG 737NGX external connection sample 
// 
//------------------------------------------------------------------------------
#include "mainlogic.h"


void MainLogic::ChangeCurrentLegIndex(int currentLegIndex) {
	CurrentWayIndex = currentLegIndex;
}

double MainLogic::GetDataL(DWORD var, const char* unit) {
	double lVar;
	emit GetDataSignalL(MAINLOGIC_ID, var, &lVar, unit);
	while (!GetDataChanged);
	GetDataChanged = false;
	return lVar;
}

double MainLogic::SetData(DWORD var, double val, const char* unit) {
	emit SetDataSignal(MAINLOGIC_ID, var, &val, unit);
	while (!SetDataChanged);
	SetDataChanged = false;
	return  val;
}

double MainLogic::SetDataL(DWORD var, double val, const char* unit) {
	emit SetDataSignalL(MAINLOGIC_ID, var, &val, unit);
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
	ObjectConnectors->push_back(connect(this, SIGNAL(StartButtonEnabled(bool)), mainObject, SLOT(StartButtonEnabled(bool))));
	ObjectConnectors->push_back(connect(this, SIGNAL(ConnectButtonEnabled(bool)), mainObject, SLOT(ConnectButtonEnabled(bool))));
	emit SendLog("READY");
	Mode = START;
	ObjectConnectors->push_back(connect(MainTimer, SIGNAL(timeout()), this, SLOT(TimerProc())));
	ObjectConnectors->push_back(connect(this, SIGNAL(PlotConstraints(std::vector<sWayPoint>*, int, int, int)), mainObject, SLOT(PlotConstraints(std::vector<sWayPoint>*, int, int, int)),Qt::QueuedConnection));
	ObjectConnectors->push_back(connect(this, SIGNAL(PlotPoints(std::vector<sWayPoint>*, int, int)), mainObject, SLOT(PlotPoints(std::vector<sWayPoint>*, int, int)), Qt::QueuedConnection));
	ObjectConnectors->push_back(connect(this, SIGNAL(PlotRealPath(double, double, double)), mainObject, SLOT(PlotRealPath(double, double, double)), Qt::QueuedConnection));
	ObjectConnectors->push_back(connect(this, SIGNAL(PlotCircle(double, double, double)), mainObject, SLOT(PlotCircle(double, double, double)), Qt::QueuedConnection));
	ObjectConnectors->push_back(connect(this, SIGNAL(SelectRow(int)), mainObject, SLOT(SelectRow(int)), Qt::QueuedConnection));
	//ObjectConnectors->push_back(connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(on_ChangeRow(QItemSelection, QItemSelection))));
	
	ModelTable = new QTableViewModel();
	ui->tableView->setModel(ModelTable);
	MainTimer->start(60);
	emit SendLog("Start main timer");
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

void MainLogic::Log(QString log)
{
	if (std::find(LogArray->begin(), LogArray->end(), log) == LogArray->end()) {
		emit SendLog(log);
		LogArray->push_back(log);
	}
	
	

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

void MainLogic::AddWayPoint(double lon, double lat, double altHi, QString pointType, QString PointName, double heading, int fixAlt, double speed, double radial, double altLo, QJsonObject* fix) {
	QAction act;
	;
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
	if ((point == 0) && (Legs->size() == 0)) {
		sw.Lon = lon;
		sw.Lat = lat;
		sw.SAltitudeHi = altHi;
		sw.SAltitudeLo = altLo;
		if (fix != NULL) {
			sw.AirWay = fix->value("via_airway").toString();
			sw.Ident = fix->value("ident").toString();
			sw.Name = fix->value("name").toString();
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
			if (altHi != 0) {
				sw.SAltitudeHi = altHi;
			}
			else {
				sw.SAltitudeHi = fix->value("altitude_feet").toString().toDouble();
			}
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
		Legs->push_back(sw);
		//QLegs->push_back(sw);
		
		point = 1;
	}
	else if ((point == 1) && (Legs->size() == 1)) {

		Legs->at(Legs->size() - 1).ELon = lon;
		Legs->at(Legs->size() - 1).ELat = lat;
		Legs->at(Legs->size() - 1).EAltitudeHi = altHi;
		Legs->at(Legs->size() - 1).EAltitudeLo = altLo;
		Legs->at(Legs->size() - 1).Type = pointType;
		Legs->at(Legs->size() - 1).Name = PointName;
		Legs->at(Legs->size() - 1).Ident = PointName;
		Legs->at(Legs->size() - 1).FixAlt = fixAlt;
		Legs->at(Legs->size() - 1).IndSpeed = speed;
		
		if (fix != NULL) {
			Legs->at(Legs->size() - 1).AirWay = fix->value("via_airway").toString();
			Legs->at(Legs->size() - 1).Ident = fix->value("ident").toString();
			Legs->at(Legs->size() - 1).Name = fix->value("name").toString();
			//Legs->at(Legs->size() - 1).Type = fix->value("type").toString();
			Legs->at(Legs->size() - 1).Type = pointType;
			Legs->at(Legs->size() - 1).ELat = fix->value("pos_lat").toString().toDouble();
			Legs->at(Legs->size() - 1).ELon = fix->value("pos_long").toString().toDouble();
			Legs->at(Legs->size() - 1).Stage = fix->value("stage").toString();
			Legs->at(Legs->size() - 1).SidStar = (bool)fix->value("is_sid_star").toString().toInt();
			Legs->at(Legs->size() - 1).Distance = fix->value("distance").toString().toDouble();
			Legs->at(Legs->size() - 1).TrackTrue = fix->value("track_true").toString().toDouble();
			Legs->at(Legs->size() - 1).TrackMag = fix->value("track_mag").toString().toDouble();
			Legs->at(Legs->size() - 1).HeadingTrue = fix->value("heading_true").toString().toDouble();
			Legs->at(Legs->size() - 1).HeadingMag = fix->value("heading_mag").toString().toDouble();
			if (altHi != 0) {
				Legs->at(Legs->size() - 1).EAltitudeHi = altHi;
			}
			else {
				Legs->at(Legs->size() - 1).EAltitudeHi = fix->value("altitude_feet").toString().toDouble();
			}
			//Legs->at(Legs->size() - 1).IndSpeed = fix->value("ind_airspeed").toString().toDouble();
			Legs->at(Legs->size() - 1).IndSpeed = speed;
			Legs->at(Legs->size() - 1).TrueSpeed = fix->value("true_airspeed").toString().toDouble();
			Legs->at(Legs->size() - 1).Mach = fix->value("mach").toString().toDouble();
			Legs->at(Legs->size() - 1).MachThousandths = fix->value("mach_thousandths").toString().toDouble();
			Legs->at(Legs->size() - 1).WindComponent = fix->value("wind_component").toString().toDouble();
			Legs->at(Legs->size() - 1).GroundSpeed = fix->value("groundspeed").toString().toDouble();
			Legs->at(Legs->size() - 1).TimeLeg = fix->value("time_leg").toString().toDouble();
			Legs->at(Legs->size() - 1).TimeTotal = fix->value("time_total").toString().toDouble();
			Legs->at(Legs->size() - 1).FuelFlow = fix->value("fuel_flow").toString().toDouble();
			Legs->at(Legs->size() - 1).FuelLeg = fix->value("fuel_leg").toString().toDouble();
			Legs->at(Legs->size() - 1).FuelTotalUsed = fix->value("fuel_totalused").toString().toDouble();
			Legs->at(Legs->size() - 1).FuelMinOnboard = fix->value("fuel_min_onboard").toString().toDouble();
			Legs->at(Legs->size() - 1).FuelPlanOnboard = fix->value("fuel_plan_onboard").toString().toDouble();
			Legs->at(Legs->size() - 1).OAT = fix->value("oat").toString().toDouble();
			Legs->at(Legs->size() - 1).OATIsaDev = fix->value("oat_isa_dev").toString().toDouble();
			Legs->at(Legs->size() - 1).WindDir = fix->value("wind_dir").toString().toDouble();
			Legs->at(Legs->size() - 1).WindSpeed = fix->value("wind_spd").toString().toDouble();
			Legs->at(Legs->size() - 1).Shear = fix->value("shear").toString().toDouble();
			Legs->at(Legs->size() - 1).TropoPause = fix->value("tropopause_feet").toString().toDouble();
			Legs->at(Legs->size() - 1).GroundHeight = fix->value("ground_height").toString().toDouble();
			Legs->at(Legs->size() - 1).Mora = fix->value("mora").toString().toDouble();
			Legs->at(Legs->size() - 1).Fir = fix->value("fir").toString();
			Legs->at(Legs->size() - 1).FirUnits = fix->value("fir_units").toString();
			Legs->at(Legs->size() - 1).FirValidLevels = fix->value("fir_valid_levels").toString();
		}
		Utils::DOrtoKM(&Legs->at(Legs->size() - 1));
		Legs->at(Legs->size() - 1).RealDistance = 0;
		Legs->at(Legs->size() - 1).CommonDistance = Legs->at(Legs->size() - 1).Distance;
		if ((pointType == "FIXHEADING") || (pointType == "HEADINGUNTILALT")) {
			Legs->at(Legs->size() - 1).H = heading;
		}
		if (pointType == "HEADINGUNTILFROMDIST") {
			Legs->at(Legs->size() - 1).Dist = radial;
			Legs->at(Legs->size() - 1).H = heading;
		}
		if (pointType == "TRKINTERCEPTRADIAL") {
			Legs->at(Legs->size() - 1).Dist = radial;
			Legs->at(Legs->size() - 1).H = heading;
		}
		if (pointType == "RUNWAY") {
			Legs->at(Legs->size() - 1).Dist = radial;
		}
		if (pointType == "TAXIWAY") {
			Legs->at(Legs->size() - 1).Dist = radial;
		}
		//QLegs->at(QLegs->size() - 1) = Legs->at(Legs->size() - 1);
		//CountWays++;
		point = 0;
	}
	else if (Legs->size() > 0) {
		
		sw.Lon = Legs->at(Legs->size() - 1).ELon;
		sw.Lat = Legs->at(Legs->size() - 1).ELat;
		sw.SAltitudeHi = Legs->at(Legs->size() - 1).EAltitudeHi;
		sw.ELon = lon;
		sw.ELat = lat;
		sw.EAltitudeHi = altHi;
		sw.EAltitudeLo = altLo;
		sw.Type = pointType;
		sw.Name = PointName;
		sw.Ident = PointName;
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
			if (altHi != 0) {
				sw.EAltitudeHi = altHi;
			}
			else {
				sw.EAltitudeHi = fix->value("altitude_feet").toString().toDouble();
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
		sw.RealDistance = 0;
		sw.CommonDistance = Legs->at(Legs->size() - 1).CommonDistance + sw.Distance;
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
		if (pointType == "RUNWAY") {
			sw.Dist = radial;
		}
		if (pointType == "TAXIWAY") {
			sw.Dist = radial;
		}
		Legs->push_back(sw);
		//QLegs->push_back(sw);
		//CountWays++;
	}
	ModelTable->populate(Legs);
}

void MainLogic::ChangeFlightPhaseReport() {
	if (FlightPhase != data->AllData.A32NX_FMGC_FLIGHT_PHASE) {
		emit CabinReport();
		FlightPhase = data->AllData.A32NX_FMGC_FLIGHT_PHASE;
	}
}

sWayPoint MainLogic::GetCurrentLeg() {
	sWayPoint cL =  Legs->at(CurrentWayIndex);
	cL.Lat = data->GData.PLANE_LATITUDE;
	cL.Lon = data->GData.PLANE_LONGITUDE;
	Utils::DOrtoKM(&cL);
	return cL;
}

int MainLogic::GetOrigRunwayIndex() {
	int cI = 0;
	while ((Legs->at(cI).Type != "RUNWAY") && (cI < (Legs->size() - 1))) {
		cI++;
	}
	if (cI >= (Legs->size() - 1)) {
		return -1;
	}
	else {
		return cI;
	}
}

int MainLogic::GetDestRunwayIndex() {
	int cI = 0;
	while ((Legs->at(cI).Type != "RUNWAY") && (cI < (Legs->size() - 1))) {
		cI++;
	}
	cI++;
	while ((Legs->at(cI).Type != "RUNWAY") && (cI < (Legs->size() - 1))) {
		cI++;
	}
	if (cI >= (Legs->size() - 1)) {
		return -1;
	}
	else {
		return cI;
	}
}

double MainLogic::GetRemainingDistance() {
	if (CurrentWayIndex > 0) {
		int cI = CurrentWayIndex;
		double rD = CurrentLeg.Distance;
		double tD = 0;
		while ((Legs->at(cI).Type != "RUNWAY") && (cI < (Legs->size() - 1))) {
			rD += Legs->at(cI + 1).Distance;
			cI++;
		}
		cI = CurrentWayIndex - 1;
		/*while ((Legs->at(cI).Type != "RUNWAY") && (cI >= 1)) {
			tD += Legs->at(cI - 1).RealDistance + Legs->at(cI - 1).Distance;
			cI--;
		}
		CurrentLeg.RealDistance = TraveledDistance - tD;
		Legs->at(CurrentWayIndex).RealDistance = CurrentLeg.RealDistance;
		Legs->at(CurrentWayIndex).Distance = CurrentLeg.Distance;*/

		CurrentLeg.CommonDistance = TraveledDistance + CurrentLeg.Distance;
		Legs->at(CurrentWayIndex).CommonDistance = TraveledDistance + Legs->at(CurrentWayIndex).Distance;
		Legs->at(CurrentWayIndex).RealDistance = TraveledDistance - Legs->at(CurrentWayIndex - 1).CommonDistance;
		//Legs->at(CurrentWayIndex).Distance = CurrentLeg.Distance;
		for (int i = CurrentWayIndex + 1; i < Legs->size(); i++) {
			Legs->at(i).CommonDistance = Legs->at(i - 1).RealDistance + Legs->at(i - 1).CommonDistance + Legs->at(i - 1).Distance;
		}
		//ModelTable->populate(Legs);
		return rD;
	}
	else {
		return 0.0;
	}
}

//Main timer loop

VOID MainLogic::TimerProc()
{
	//emit CLLanding(&landing);
	if ((Legs != NULL) && (Legs->size() > 0)) {
		//emit PlotPoints(Legs, GetOrigRunwayIndex(), GetDestRunwayIndex());
		emit PlotConstraints(Legs, GetOrigRunwayIndex(), GetDestRunwayIndex(), CurrentWayIndex);
		emit SelectRow(CurrentWayIndex);
	}
	
	
	static double lastFlyPoint = 0;
	if (Legs && Legs->size() > 0) {
		GetDescentAngle();
		if (!Connected) {
			//emit PlotCircle(TraveledDistance, data->GData.PLANE_ALTITUDE);
			CommonDistance = Legs->at(GetDestRunwayIndex()).CommonDistance;
			emit PlotCircle(ui->dist->value(), ui->Alt->value(), CommonDistance);
		}
	}
	
	if ((Mode == TAKEOFF || Mode == CRUISE || Mode == DESCENT || Mode == PREPREPARE) /*&& (!data->GData.SIM_ON_GROUND)*/) {
		sWayPoint currDis;
		double RealDistance = 0;
		for (int i = GetOrigRunwayIndex(); i <= GetDestRunwayIndex(); i++) {
			Legs->at(i).CommonDistance = Legs->at(i).Distance + Legs->at(i).RealDistance + RealDistance;
			RealDistance = Legs->at(i).CommonDistance;
		}
		currDis.Lat = lastlat;
		currDis.Lon = lastlon;
		if ((lastlat == 0) && (lastlon == 0)) {
			currDis.Lat = data->GData.PLANE_LATITUDE;
			currDis.Lon = data->GData.PLANE_LONGITUDE;
			for (int i = GetOrigRunwayIndex(); i < CurrentWayIndex; i++) {
				TraveledDistance = TraveledDistance + Legs->at(i).CommonDistance;
			}
			currDis.ELat = Legs->at(CurrentWayIndex).Lat;
			currDis.ELon = Legs->at(CurrentWayIndex).Lon;
			Utils::DOrtoKM(&currDis);
			TraveledDistance = TraveledDistance + currDis.Distance;
			CurrentLeg.RealDistance = CurrentLeg.RealDistance + currDis.Distance;
			//CurrentLeg.RealDistance = CurrentLeg.RealDistance + currDis.Distance;

		}
		currDis.ELat = data->GData.PLANE_LATITUDE;
		currDis.ELon = data->GData.PLANE_LONGITUDE;
		currDis.EAltitudeHi = data->GData.PLANE_ALTITUDE;
		Utils::DOrtoKM(&currDis);
		CommonDistance = TraveledDistance + GRemainingDistance;
		if (currDis.Distance > 0.1) { // at most add point every 100 m
			TraveledDistance = TraveledDistance + currDis.Distance;
			lastlat = currDis.ELat;
			lastlon = currDis.ELon;
			currDis.Lat = CurrentLeg.ELat;
			currDis.Lon = CurrentLeg.ELon;
			Utils::DOrtoKM(&currDis);
			CurrentLeg.Distance = currDis.Distance;
			//Legs->at(CurrentWayIndex).Distance = currDis.Distance;
			double ang = atan((currDis.EAltitudeHi - lastalt) / 3280.84 / currDis.Distance) * 180.0 / M_PI;
			lastalt = currDis.EAltitudeHi;
			flyPoint = TraveledDistance;
			emit PlotRealPath(flyPoint, data->GData.INDICATED_ALTITUDE, CommonDistance);
			if (Flight != NULL) {
				Flight->WriteLn(std::to_string(TraveledDistance) + ";"
					+ std::to_string(data->GData.PLANE_ALTITUDE) + ";"
					+ std::to_string(data->GData.VERTICAL_SPEED) + ";"
					+ std::to_string(data->GData.TOTAL_WEIGHT) + ";"
					+ std::to_string(data->AllData.A32NX_SPEEDS_MANAGED_PFD) + ";"
					+ std::to_string(data->GData.AIRSPEED_INDICATED) + ";"
					+ std::to_string(data->AllData.XMLVAR_AirSpeedIsInMach) + ";"
					+ std::to_string(data->AllData.A32NX_FMGC_FLIGHT_PHASE) + ";"
					+ std::to_string(data->GData.TURB_ENG_N11) + ";"
					+ std::to_string(ang) + ";"
					+ "\n");
			}
		}
		
	}
	if (Mode == START) {
		if (InTimer) {
			return;
		}
		InTimer = true;
		Log("START");
		//SendCommand(SET_THROTTLE, 13130, 0);
		if (Legs == NULL) {
			Legs = new std::vector<sWayPoint>();

			mgr = new QNetworkAccessManager(this);
			//const QUrl url(QStringLiteral("http://www.simbrief.com/api/xml.fetcher.php?json=1&username=SamFrieeman"));
			const QUrl url(QStringLiteral("http://www.simbrief.com/api/xml.fetcher.php?json=1&username=igor8518"));
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
		else if (Legs->size() > 0) {
			Mode = PLANREADY;
		}

	}
	if (Mode == PLANREADY) {
		Log("PLANREADY");
		QJsonObject root = document.object();
		FlightCruise = root["general"].toObject()["initial_altitude"].toString().toDouble();
		MaximumAltitude = FlightCruise;
		ja = root["navlog"].toObject()["fix"].toArray();


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
							//PLNFile = getenv("TEMP") + QString("\\") + PLNFile;
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
		if (Connected) {
			Mode = PREPARE;
			if (data->GData.SIM_ON_GROUND != 1) {
				SendCommand(GEAR_SET, 0, 0);
				SendCommand(SET_THROTTLE, 4000, 0);
				SendCommand(PARKBRAKE_SET, 0, 0);
				Mode = FILLWAY;	
			}
			InTimer = false;
		}
		else {
			//Sleep(30000);
			Mode = FILLWAY;
			
		}
	}
	if (Mode == FILLWAY) {
		Log("FILLWAY");
		if (AirportData == NULL) {
			//if (data->GData.PLANE_LATITUDE != 0.0 && data->GData.PLANE_LONGITUDE != 0.0) {
			AirportData = ::GetAirportData();
			
			
			while (1) {
				if (AirportData->GetProgress() == 100) {
					break;
				}
			}
			//}
		}
		else {
			QJsonObject root = document.object();

			SendLog("Setup original airport");
			AirportList OrigApt = AirportData->GetAirportByIcao(root["origin"].toObject()["icao_code"].toString().toStdString());

			if (OrigApt.ICAO == "RJCI") {
				SendLog("Original airport not found! EXIT.");
				AddWayPoint(0, 0, FlightCruise, "FIX", "", 0, CRUISEALT, 0, 0,0.0, &ja[0].toObject());
				return;
			}

			QJsonObject fix;
			std::vector<TSIDS>* OrigSids;
			sWayPoint HeadTakeOff;
			TPath RunwayWaysOrig1;
			if (Connected) {
				Legs->clear();
				//TO DO Only tests
				DWORD nearTaxiwayPointIndex = 325;
				if (data->GData.SIM_ON_GROUND) {
					nearTaxiwayPointIndex = AirportData->GetNearTaxiwayPoint(data->GData.PLANE_LATITUDE, data->GData.PLANE_LONGITUDE);
				}
				////////
				AirportData->SetHSim(HSimConnect);
				data->AirportData = AirportData;
				std::vector<RunwayPaths>* RP = AirportData->GetRunways();
				AirportData->GetRunwayStart(0, 0, 0);

				std::vector<std::vector<TPath>>* RunwayWaysOrig = new std::vector<std::vector<TPath>>();
				std::vector<TPath> P;
				int countWays = 0;
				for (int i = 1; i <= RP->size(); i++) {
					P = AirportData->GetPath1(nearTaxiwayPointIndex, data->GData.PLANE_HEADING_DEGREES_TRUE, i);
					if (P.size() > 1) {
						RP->at(i - 1).RunwayName1 = P[P.size() - 1].name;
						RP->at(i - 1).R1Dist = Utils::GetPathLength(&P);
						RP->at(i - 1).Deg1 = Utils::GetRWAngle(&P);
						RunwayWaysOrig->push_back(P);
						countWays++;
					}

					P = AirportData->GetPath1(nearTaxiwayPointIndex, data->GData.PLANE_HEADING_DEGREES_TRUE, -i);
					if (P.size() > 1) {
						RP->at(i - 1).RunwayName2 = P[P.size() - 1].name;
						RP->at(i - 1).R2Dist = Utils::GetPathLength(&P);
						RP->at(i - 1).Deg2 = Utils::GetRWAngle(&P);
						RunwayWaysOrig->push_back(P);
						RP->at(i - 1).Lenght = Utils::GetRWLength(&P);
						countWays++;
					}
				}
				if (countWays > 0) {
					std::string RWFromSB = root["origin"].toObject()["plan_rwy"].toString().toStdString();
					for (int RC = 0; RC < RunwayWaysOrig->size(); RC++) {
						if (RunwayWaysOrig->at(RC).at(RunwayWaysOrig->at(RC).size() - 1).name == RWFromSB) {
							rrr = RC;
							break;
						}
					}



					RunwayWaysOrig1 = RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1);
					DeparturePath = RunwayWaysOrig->at(rrr);
					if (data->GData.SIM_ON_GROUND) {
						for (int i = 0; i < RunwayWaysOrig->at(rrr).size() - 1; i++) {
							if (RunwayWaysOrig->at(rrr).at(i).InRunway == 0xff) {
								AddWayPoint(RunwayWaysOrig->at(rrr).at(i).Lon, RunwayWaysOrig->at(rrr).at(i).Lat, 0.0, TYPE_PATHS[RunwayWaysOrig->at(rrr)[i].Type], QString(RunwayWaysOrig->at(rrr).at(i).name.c_str()), 0, 0, 0, -1.0, 0.0);
							}
							else {
								AddWayPoint(RunwayWaysOrig->at(rrr).at(i).Lon, RunwayWaysOrig->at(rrr).at(i).Lat, 0.0, TYPE_PATHS[RunwayWaysOrig->at(rrr)[i].Type], QString(RunwayWaysOrig->at(rrr).at(i).name.c_str()), 0, 0, 0, 0, 0.0);
							}
						}


						AddWayPoint(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lon, RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lat, 5555, "RUNWAY", QString(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).name.c_str()), 0, 0, 0, 0, 4444);
						for (int i = Legs->size() - 2; i > 0; i--) {
							if ((Legs->at(i).Dist != -1.0) || (abs(Utils::Constrain180(Legs->at(i).EndHeadingTrue - Legs->at(Legs->size() - 1).EndHeadingTrue)) < 90)) {
								Legs->at(i + 1).Dist = 2.0;
								break;
							}
						}
						//SendCommand(PULL_HDG, 1, 0);
						//SendCommand(HDG_SEL, Legs->at(Legs->size() - 1).HeadingMag, 10);
					}
					else {
						AddWayPoint(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).Lon, RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).Lat, 0, "CLIMB", QString(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).name.c_str()), 0, 0, 0, 0, 0.0);
						AddWayPoint(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).Lon, RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).Lat, 0, "CLIMB", QString(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).name.c_str()), 0, 0, 0, 0, 0.0);

						AddWayPoint(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lon, RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lat, 5555, "RUNWAY", QString(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).name.c_str()), 0, 0, 0, 0, 4444);
						CurrentWayIndex = 1;

					}
					SIDPoint = Legs->size() - 2;
					SimBriefSID = " ";
					FixSID = " ";
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
					HeadTakeOff;
					HeadTakeOff.Lat = RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).Lat;
					HeadTakeOff.Lon = RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 2).Lon;
					HeadTakeOff.SAltitudeHi = 0.0;
					HeadTakeOff.ELat = RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lat;
					HeadTakeOff.ELon = RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lon;
					HeadTakeOff.EAltitudeHi = 0.0;
					Utils::DOrtoKM(&HeadTakeOff);

					OrigSidStar = AirportData->GetAirport()->sidstar;
					OrigSids = AddSID(OrigSidStar, &RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1), HeadTakeOff.EndHeadingTrue, FixSID, SimBriefSID);
					if (OrigSids->size() == 0) {
						OrigSids = AddSID(OrigSidStar, &RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1), HeadTakeOff.EndHeadingTrue, FixSID + "NB", SimBriefSID);
					}
					if (OrigSids->size() == 0) {
						OrigSids->push_back({ -1, -1, NULL, "", "", RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).name });
						AddWayPoint(RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lon, RunwayWaysOrig->at(rrr).at(RunwayWaysOrig->at(rrr).size() - 1).Lat, 1750, "HEADINGUNTILALT", "(1750)", HeadTakeOff.EndHeadingTrue - data->GData.MAGVAR, 0, 0.0, 0, 1750);
						AddWayPoint(0, 0, FlightCruise, "FIX", "", 0, 0, 0, 0, 0.0, &ja[0].toObject());
					}
					for (int i = 0; i < RunwayWaysOrig->size(); i = i + 1) {
						RunWaysPathsOrig.push_back({ "", "", "", 0, RunwayWaysOrig->at(i).at(RunwayWaysOrig->at(i).size() - 2).Lat, RunwayWaysOrig->at(i).at(RunwayWaysOrig->at(i).size() - 2).Lon,
							RunwayWaysOrig->at(i).at(RunwayWaysOrig->at(i).size() - 1).Lat, RunwayWaysOrig->at(i).at(RunwayWaysOrig->at(i).size() - 1).Lon
							});
						Utils::DOrtoKM(&RunWaysPathsOrig.at(i));
					}
				}
				else {
					MessageBoxW(0, L"NO WAYS", L"ERROR", MB_OK);
					Mode = DONE;
					exit;
				}
			}
			//Connected
			else {
				AirportData->SetHSim(NULL);
				std::string RWFromSB = root["origin"].toObject()["plan_rwy"].toString().toStdString();
				std::vector<RunwayPaths>* RP = AirportData->GetRunways();
				AirportData->GetRunwayStart(0, 0, 0);
				for (int i = 0; i < RP->size(); i++) {
					DATA_RUNWAY rwy;
					rwy = AirportData->GetStartCoord(i + 1);

					RunWaysPathsDest.push_back({ "", "", "", 0, rwy.sLatitude, rwy.sLongitude,
						rwy.eLatitude, rwy.eLongitude,
						});
					Utils::DOrtoKM(&RunWaysPathsDest.at(i));


				}
				AirportData->GetRunwayStart(0, 0, 0);
				int FindRWYIndex = 0;
				for (int i = 1; i <= RP->size(); i++) {
					if (RWFromSB == AirportData->GetStartCoord(i).Name) {
						FindRWYIndex = i;
						break;
					}
					if (RWFromSB == AirportData->GetStartCoord(-i).Name) {
						FindRWYIndex = -i;
						break;
					}
				}
				DATA_RUNWAY runway;
				if (FindRWYIndex == 0) {
					runway = AirportData->GetStartCoord(1);
				}
				else {
					runway = AirportData->GetStartCoord(FindRWYIndex);
				}
				//runway.alt = AirportData->GetAirport()->PAirportInformation->Alt / 1000 * 3.28;
				fix = ja[0].toObject();
				//AddWayPoint(0, 0, 0, "WAYPOINT", "", 0, FIXALT, 0, 0, &fix);
				AddWayPoint(runway.sLongitude, runway.sLatitude, runway.alt, "AIRPORT", QString(runway.Name.c_str()), 0, FIXALT, 0.0, 0, runway.alt);
				AddWayPoint(runway.sLongitude, runway.sLatitude, runway.alt, "AIRPORT", QString(runway.Name.c_str()), 0, FIXALT, 0.0, 0, runway.alt);
				AddWayPoint(runway.eLongitude, runway.eLatitude, runway.alt, "RUNWAY", QString(runway.Name.c_str()), 0, FIXALT, 0.0, 0, runway.alt);
				
				SIDPoint = Legs->size() - 1;
				SimBriefSID = " ";
				FixSID = " ";
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
				HeadTakeOff;
				HeadTakeOff.Lat = runway.sLatitude;
				HeadTakeOff.Lon = runway.sLongitude;
				HeadTakeOff.SAltitudeHi = 0.0;
				HeadTakeOff.ELat = runway.eLatitude;
				HeadTakeOff.ELon = runway.eLongitude;
				HeadTakeOff.EAltitudeHi = 0.0;
				Utils::DOrtoKM(&HeadTakeOff);
				RunwayWaysOrig1.name = runway.Name;
				RunwayWaysOrig1.Lat = runway.eLatitude;
				RunwayWaysOrig1.Lon = runway.eLongitude;

				OrigSidStar = AirportData->GetAirport()->sidstar;
				OrigSids = AddSID(OrigSidStar, &RunwayWaysOrig1, HeadTakeOff.EndHeadingTrue, FixSID, SimBriefSID);
				if (OrigSids->size() == 0) {
					OrigSids = AddSID(OrigSidStar, &RunwayWaysOrig1, HeadTakeOff.EndHeadingTrue, FixSID + "NB", SimBriefSID);
				}
				if (OrigSids->size() == 0) {
					OrigSids->push_back({ -1, -1, NULL, "", "", RunwayWaysOrig1.name });
					AddWayPoint(RunwayWaysOrig1.Lon, RunwayWaysOrig1.Lat, 1750, "HEADINGUNTILALT", "(1750)", 0.0, 0, 0.0, 0,1750);
					AddWayPoint(0, 0, FlightCruise, "FIX", "", 0, 0, 0, 0,0.0, &ja[0].toObject());
				}
			} //Connected
			Departure = AirportData->GetAirport();

			
			if (OrigSids->size() > 0) {
				Legs->at(SIDPoint).Name = QString(OrigSids->at(0).SID.c_str());
				//QLegs->at(SIDPoint).Name = QString(OrigSids->at(0).SID.c_str());
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
							AddSidStarTrack(&OrigSids->at(i).wayPoint->at(j), &RunwayWaysOrig1, &fixx);
						}
						Legs->at(SIDPoint).Name = QString(OrigSids->at(i).SID.c_str());
						//QLegs->at(SIDPoint).Name = QString(OrigSids->at(i).SID.c_str());
						SendLog("Start runway " + QString(OrigSids->at(i).RUNWAY.c_str()));
						SendLog("SID " + QString(OrigSids->at(i).SID.c_str()));
						SendLog("Transition SID " + QString(OrigSids->at(i).TRANSITION.c_str()));
						break;
					}
				}
			}
			RunwayHeading = HeadTakeOff.EndHeadingTrue;
			Legs->at(0).Ident = root["origin"].toObject()["icao_code"].toString();
			Legs->at(0).Name = root["origin"].toObject()["name"].toString();
			Legs->at(0).IndSpeed = 0.0;
			//QLegs->at(0).Ident = root["origin"].toObject()["icao_code"].toString();
			//QLegs->at(0).Name = root["origin"].toObject()["name"].toString();
			//QLegs->at(0).IndSpeed = 0.0;
			RoutePoint = Legs->size();
			for (int sr = SIDPoint; sr < RoutePoint; sr++) {
				for (int sp = sr; sp < RoutePoint; sp++) {
					if (Legs->at(sr).IndSpeed == 0) {
						if (Legs->at(sp).IndSpeed != 0) {
							Legs->at(sr).IndSpeed = Legs->at(sp).IndSpeed;
							//QLegs->at(sr).IndSpeed = Legs->at(sp).IndSpeed;
							break;
						}
					}
				}
			}
			
			SendLog("Setup route");
			sWayPoint tWP;
			bool sid = false;
			int CrzAlt = 0;
			int toc = -1;
			int tod = -1;
			for (int j = 1; j < ja.count() - 1; j++) {
				if (ja[j].toObject().value("ident").toString() == "TOC") {
					toc = j;
				}
				if (ja[j].toObject().value("ident").toString() == "TOD") {
					tod = j;
				}
			}
			for (int i = 1; i < ja.count() - 1; i++) {
				fix = ja[i].toObject();
				if (ja[i+1].toObject().value("is_sid_star").toString() == "0") {
					if (ja[i - 1].toObject().value("is_sid_star").toString() == "0") {
						if (i < toc) {
							CrzAlt = ja[toc].toObject().value("altitude_feet").toString().toDouble();
						}
						else if (i > tod) {
							CrzAlt = ja[tod].toObject().value("altitude_feet").toString().toDouble();
						}
						else {
							CrzAlt = ja[i].toObject().value("altitude_feet").toString().toDouble();;
						}	
						AddWayPoint(0, 0, CrzAlt,/*root["general"].toObject()["initial_altitude"].toString().toDouble(),*/ "WAYPOINT", "", 0, CRUISEALT, 0, 0, 0.0, &fix);
					}
				}
			}

			SendLog("Setup destination airport");
			SimBriefSTAR = " ";
			FixSTAR = " ";


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
			fix = ja[ja.count() - 1].toObject();
			std::string RWDest = root["destination"].toObject()["plan_rwy"].toString().toStdString();
			STARPoint = Legs->size();
			AirportList DestApt = AirportData->GetAirportByIcao(root["destination"].toObject()["icao_code"].toString().toStdString());
			DestSidStar = AirportData->GetAirport()->sidstar;
			fix = ja[ja.count() - 1].toObject();
			std::vector<RunwayPaths>* RP = AirportData->GetRunways();
			AirportData->GetRunwayStart(0, 0, 0);

			for (int i = 0; i < RP->size(); i++) {
				DATA_RUNWAY rwy;
				rwy = AirportData->GetStartCoord(i + 1);

				RunWaysPathsDest.push_back({ "", "", "", 0, rwy.sLatitude, rwy.sLongitude,
					rwy.eLatitude, rwy.eLongitude,
					});
				Utils::DOrtoKM(&RunWaysPathsDest.at(i));


			}
			AirportData->GetRunwayStart(0, 0, 0);
			int FindRWYIndex = 0;
			for (int i = 1; i <= RP->size(); i++) {
				if (RWDest == AirportData->GetStartCoord(i).Name) {
					FindRWYIndex = i;
					break;
				}
				if (RWDest == AirportData->GetStartCoord(-i).Name) {
					FindRWYIndex = -i;
					break;
				}
			}
			DATA_RUNWAY runway;
			if (FindRWYIndex == 0) {
				runway = AirportData->GetStartCoord(1);
			}
			else {
				runway = AirportData->GetStartCoord(FindRWYIndex);
			}
			//runway.alt = AirportData->GetAirport()->PAirportInformation->Alt / 1000 * 3.28;
			SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway.sLatitude, runway.sLongitude, runway.eHeading, 18.0);
			STARPoint = Legs->size();
			std::vector<TSTARS>* OrigStars = AddSTAR(DestSidStar, &runway, FixSTAR, SimBriefSTAR);
			if (OrigStars->size() == 0) {
				OrigStars = AddSTAR(DestSidStar, &runway, FixSTAR + "NB", SimBriefSTAR);
			}
			int Star = 0;
			bool FindStar = false;
			for (int i = 0; i < OrigStars->size(); i++) {
				if (QString(OrigStars->at(i).STAR.c_str()) == SimBriefSTAR) {
					FindStar = true;
					Star = i;
					break;
				}
			}
			if (OrigStars->size() > 0) {
				
				SendLog("Transition STAR " + QString((OrigStars->at(Star).STARTRANS).c_str()));
				SendLog("STAR " + QString((OrigStars->at(Star).STAR).c_str()));
				SendLog("Transition approach " + QString((OrigStars->at(Star).APPTRANS).c_str()));
				SendLog("Approach " + QString((OrigStars->at(Star).APPROACH).c_str()));
				SendLog("Stop runway " + QString((OrigStars->at(Star).RUNWAY).c_str()));

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
						AppPoint = Legs->size() - 1;
					}
					AddSidStarTrack(&OrigStars->at(Star).wayPoint->at(i), NULL, &fixx, &runway);
				}
				Legs->at(STARPoint).Name = QString((OrigStars->at(Star).STAR + " -> " + OrigStars->at(Star).APPROACH).c_str());
			}
			else {
				SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway.sLatitude, runway.sLongitude, runway.eHeading, 18.0);
				AddWayPoint(latlon.Longitude, latlon.Latitude, runway.alt + 2000.0, "FIX", "GlideAngle 18.0 " + QString(runway.Name.c_str()), 0, FIXALT, 0.0, 0, runway.alt + 2000.0);
				AddWayPoint(runway.sLongitude, runway.sLatitude, runway.alt + 2000.0, "WAYPOINT", "FINAL " + QString(runway.Name.c_str()), 0, FIXALT, 0.0, 0, runway.alt + 2000.0);
				AddWayPoint(runway.sLongitude, runway.sLatitude, runway.alt, "GSWAY", QString(runway.Name.c_str()), 0, FIXALT, 0.0, 0, runway.alt);
				AddWayPoint(runway.eLongitude, runway.eLatitude, runway.alt, "RUNWAY", QString(runway.Name.c_str()), 0, FIXALT, 0.0, 0, runway.alt);
				Legs->at(STARPoint).Name = QString(" -> ");
			}
			AddWayPoint(0, 0, runway.alt, "AIRPORT", "", 0, FIXALT, 0, 0, runway.alt, &fix);
			Legs->at(Legs->size() - 1).SAltitudeHi = runway.alt;
			Legs->at(Legs->size() - 1).EAltitudeHi = runway.alt;
			//QLegs->at(Legs->size() - 1).SAltitudeHi = AirportData->GetAirport()->PAirportInformation->Alt / 1000 * 3.28;
			//QLegs->at(Legs->size() - 1).EAltitudeHi = AirportData->GetAirport()->PAirportInformation->Alt / 1000 * 3.28;
			Legs->at(Legs->size() - 1).SAltitudeLo = runway.alt;
			Legs->at(Legs->size() - 1).EAltitudeLo = runway.alt;
			//QLegs->at(Legs->size() - 1).SAltitudeLo = AirportData->GetAirport()->PAirportInformation->Alt / 1000 * 3.28;
			//QLegs->at(Legs->size() - 1).EAltitudeLo = AirportData->GetAirport()->PAirportInformation->Alt / 1000 * 3.28;
			
			//QLegs->at(STARPoint).Name = QString((OrigStars->at(Star).STAR + " -> " + OrigStars->at(Star).APPROACH).c_str());
			STAR = true;
			Destination = AirportData->GetAirport();
			if (!Connected) {
				ModelTable->populate(Legs);
				//delete Legs;
				//Legs = NULL;
				InTimer = false;
				Mode = -1;
				ConnectButtonEnabled(true);
				//Sleep(300000);
				//emit SendLog("Refresh flightplan: " + root["origin"].toObject()["icao_code"].toString() + " -> " + root["destination"].toObject()["icao_code"].toString());
				//QLegs->clear();
				return;
			}
			

			Mode = PREPREPARE;
			TCabinWork->start();
			ConnectButtonEnabled(true);
			StartButtonEnabled(true);
			if (data->GData.SIM_ON_GROUND != 1) {
				SendCommand(GEAR_SET, 0, 0);
				SendCommand(SET_THROTTLE, 4000, 0);
				SendCommand(PARKBRAKE_SET, 0, 0);
				Mode = TAKEOFF;
			}
			//Mode = TAKEOFF;
			//Mode = CRUISE;
			//SendCommand(HDG_SEL, 180, 0);
			//Mode = ENGINESTART;
			CurrentLeg = Legs->at(CurrentWayIndex);
			CurrentLeg.Lat = data->GData.PLANE_LATITUDE;
			CurrentLeg.Lon = data->GData.PLANE_LONGITUDE;
			Utils::DOrtoKM(&CurrentLeg);
		}
	}
	if (Mode == PREPREPARE) {

		CurrentLeg = GetCurrentLeg();
		double RemainingDistance = GetRemainingDistance();
		GRemainingDistance = RemainingDistance;
		double DToChange = CalcToNewWay(true);
		CHAR s[512];
		SendText("MODE: AUTO Current: " + QString::number(CurrentLeg.Distance, 'f', 3) + " EFIS: " + " km Total: " + QString::number(RemainingDistance, 'f', 3) + " km \nCurrent way: " + QString::number(CurrentWayIndex) + " Type: " + CurrentLeg.Type + " Total ways: " + QString::number(Legs->size()) + " D for head: " + QString::number(DToChange, 'f', 3), false);

	}
	if (Mode == PREPARE) {
		SendCommand(START_TIMER, 0, 0);
		int rrrrr = sizeof(UINT64);
		std::string LIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_1 });
		std::string RIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_1 });
		if (data->AllData.A32NX_INITFLIGHT_STATE >= 8) {
			Log("Start boarding");
		}
		if (data->AllData.A32NX_INITFLIGHT_STATE > 8) {
			Log("Boarding completed");
		}
		Log("Prepare");
		if (preliminaryCocpitPrep == 0) {
			Log("Start preliminary");
			if (PLNFile != "") {
				QByteArray ba = (getenv("TEMP") + QString("\\") + PLNFile).toLocal8Bit();
				const char* PLNChar = ba.data();
				
				emit CLPreliminaryCocpitPrep(&preliminaryCocpitPrep);
				//emit SetModeSignal(PREPARE);
				SimConnect_FlightPlanLoad(HSimConnect, PLNChar);
			}
		}
			if (preliminaryCocpitPrep == 2) {
				Log("End preliminary");
			if (cocpitPreparation == 0) {
				Log("Start cocpit prepare");
				emit CLCocpitPreparation(&cocpitPreparation);
				//while (data->GData.ATC_RUNWAY_SELECTED != 1);
				//AtcRwy = std::string(data->GData.ATC_RUNWAY_AIRPORT_NAME);
				
			}
			if (SIDPoint == 0) {
				//SendCommand(SET_THROTTLE, 9000, 0);
				Mode = FILLWAY;
			}
			else {
				if (cocpitPreparation == 2) {
					Log("End cocpit prepare");
					if (beforeStart == 0) {
						Log("Start before start");
						emit CLBeforeStart(&beforeStart);
							Mode = STARTPUSHBACK;
							//Mode = CRUISE;
					}
				}
			}
		}
		
		

	}
	if (Mode == PUSHBACK || Mode == STARTPUSHBACK) {

		//DrawAirport(PAirportData, PaircraftData->lat, PaircraftData->lon, PaircraftData->hed);
		Log("Start pushback");
		if (beforeStart == 2) {
			Log("End before start");
		}
			CurrentLeg = Legs->at(CurrentWayIndex);
			if (CurrentLeg.Type == TYPE_PATHS[11]) {
				if (Mode == STARTPUSHBACK) {
					/*if (data->GData.PUSHBACK_AVAILABLE);
					//SendEvent(KEY_PARKING_BRAKES, 1);
					SendEvent(KEY_PUSHBACK_SET, 1);
					while (data->GData.GROUND_VELOCITY < 0.02);
					SetData(PUSHBACK_WAIT, 1);
					Sleep(60000);
					//SendEvent(KEY_PARKING_BRAKES, 0);
					SendCommand(PARKBRAKE_SET, 0, 0);
					SetData(PUSHBACK_WAIT, 0);*/
					if (data->AllData.FSDT_GSX_DEPARTURE_STATE == 5) {
						SetDataL(FSDT_GSX_MENU_CHOICE, 1);
						while (data->GData.GROUND_VELOCITY < 0.02) {
							data->AllData.FSDT_GSX_MENU_CHOICE = 1;
							while (data->AllData.FSDT_GSX_MENU_CHOICE != -2);
							SendCommand(PARKBRAKE_SET, 0, 0);
							Sleep(2000);
						}
						Mode = PUSHBACK;
						Log("Start pushback");
					}
					
				}
				//emit SendCommand(SET_PUSHBACK_SPEED, 5, 10);
				//SetData(GROUND_VELOCITY, 5);
				CurrentLeg.Lat = data->GData.PLANE_LATITUDE;
				CurrentLeg.Lon = data->GData.PLANE_LONGITUDE;
				Utils::DOrtoKM(&CurrentLeg);
				double DCommon = Utils::DToType(CurrentWayIndex, Legs, "RUNWAY", &CurrentLeg, 0);

				double REangle; //Subtraction from start and end points heading on ortodrome
				if ((CurrentWayIndex + 1) >= Legs->size()) {
					REangle = (Legs->at(0).HeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue);
				}
				else {
					REangle = Legs->at(CurrentWayIndex + 1).HeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue;
				}

				REangle = Utils::Constrain180(REangle); //Угол между текущей и следующей линиями маршрута

				double DToHed = 0.014486872954506089683063561200355 + 0.001; // (tan(REangle) * 8.364);
				double EangleRel = CurrentLeg.EndHeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue; //Отклонение текущего угла от линии маршрута

				EangleRel = Utils::Constrain180(EangleRel);

				if ((abs(EangleRel) > 60) || (CurrentLeg.Distance < abs(DToHed))) {
					CurrentWayIndex = CurrentWayIndex + 1;  //Если отклонение текущего угла от линии маршрута больше 60* или расстояние до точки маршрута меньше REangle*0.0002, то расчитываем следующюю линию маршрута

				}
				else {
					double HeadingRel = PBHeadWithWay(&Legs->at(CurrentWayIndex), true);
				}
				
				if (data->GData.GROUND_VELOCITY < 0.05) {
					SendCommand(PARKBRAKE_SET, 1, 0);
				}
				if ((data->AllData.FSDT_GSX_DEPARTURE_STATE == 6) || 
					data->AllData.FSDT_GSX_DEPARTURE_STATE == 6) {
					SendCommand(PARKBRAKE_SET, 1, 0);
					Mode = ENGINESTART;
					Log("End pushback");
				}
			}
			else {
				if (Mode == PUSHBACK) {
					
					/*SetData(PUSHBACK_WAIT, 1);
					//emit SendCommand(SET_PUSHBACK_SPEED, 0, 10);
					//SetData(GROUND_VELOCITY, 0);
					SendEvent(KEY_PUSHBACK_SET, 0);
					while (data->GData.GROUND_VELOCITY > 0.005) {
						SetData(PUSHBACK_WAIT, 1);
						SetData(GROUND_VELOCITY, 0);
					};
					SetData(PUSHBACK_WAIT, 1);*/
					//SendEvent(KEY_PARKING_BRAKES, 1);
					
					//SendEvent(KEY_PUSHBACK_SET, 1);
				}
				if (data->GData.GROUND_VELOCITY < 1.5) {
					SendCommand(PARKBRAKE_SET, 1, 0);
				}
				if (data->AllData.FSDT_GSX_DEPARTURE_STATE == 6) {
					SendCommand(PARKBRAKE_SET, 1, 0);
					Mode = ENGINESTART;
					Log("End pushback");
				}
				Sleep(30000);
				//brakes = { 0.0,0.0 };
				//SimConnect_SetDataOnSimObject(hSimConnect, DEF_BRAKE, 0, 0, 0, sizeof(brakes), &brakes.LBrake);
			}
	}	
	if (Mode == ENGINESTART) {
		
		if (beforeStart == 2) {
			Log("End before start");
			if (engineStart == 0) {
				Log("Starting engines");
				emit CLEngineStart(&engineStart);
			}

		}
		if (engineStart == 2) {
			Log("Engines started");
			if (afterStart == 0) {
				Log("Start afterstart");
				emit CLAfterStart(&afterStart);
			}
		}
		if (afterStart == 2) {
			Log("End afterstart");
			if (beforeTaxi == 0) {
				Log("Start before taxi");
				emit CLBeforeTaxi(FlightCruise, RunwayHeading, &beforeTaxi);
			}
		}
		double V2 = data->AllData.AIRLINER_V2_SPEED;
		
		if ((beforeTaxi == 2)&&(V2 != 0)) {
			Log("End before taxi");
			SendCommand(SET_GSPEED, -1, 0);
			Sleep(5000);
			Mode = TAXIOUT;
			Log("Taxiout");
			Taxiway = CurrentWayIndex;
			SendCommand(PARKBRAKE_SET, 0, 0);
		}
	}
	if ((Mode == TAXIOUT) || (Mode == TAXIIN)) {
		std::string LIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_1 });
		std::string RIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_1 });
		double GAlt = data->GData.PLANE_ALT_ABOVE_GROUND - 9;
		CurrentLeg = Legs->at(CurrentWayIndex);
		CurrentLeg.Lat = data->GData.PLANE_LATITUDE;
		CurrentLeg.Lon = data->GData.PLANE_LONGITUDE;
		Utils::DOrtoKM(&CurrentLeg);
		double AngleFront = GetAngleFront(0.04, false);
		double NoseFront = GetAngleFront(0.01, false);
		double HeadingRel = ManHeadWithWay(&Legs->at(CurrentWayIndex));
		//double HeadingRel = RudWithHead(data->GData.PLANE_HEADING_DEGREES_TRUE + NoseFront);
		double DCommon = Utils::DToType(CurrentWayIndex, Legs, "RUNWAY", &CurrentLeg, Utils::Constrain180(CurrentLeg.EndHeadingTrue-data->GData.PLANE_HEADING_DEGREES_TRUE));
		
		SetDataL(AI_ANGLE_HEAD, AngleFront);
		SetDataL(AI_ANGLE_HEAD, NoseFront);
		double DToRunway = 1;
		if (Mode == TAXIOUT) {
			DToRunway = Utils::DToRunway(CurrentWayIndex, Legs, &CurrentLeg);
		}
		if (Mode == TAXIOUT) {
			SendText("MODE: TAXI_OUT To " + Legs->at(SIDPoint + 1).Name + " - " + Legs->at(SIDPoint).Name + " Current: " + QString::number(CurrentLeg.Distance, 'f', 3) + " GroundAltitude: " + QString::number(GAlt, 'f', 3) + " km Total: " + QString::number(AngleFront, 'f', 3) + " km \n \
			Speed " + QString::number(speed) + "; Current way: " + QString::number(CurrentWayIndex) + " EFIS: " + QString(LIdent.c_str()) + " Name: " + CurrentLeg.Name + " Type: " + CurrentLeg.Type + " Total ways: " + QString::number(Legs->size()), false);
		}
		else {
			SendText("MODE: TAXI_IN To " + Legs->at(SIDPoint + 1).Name + " - " + QString(ParkN->c_str()) + " Current: " + QString::number(CurrentLeg.Distance, 'f', 3) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \n \
			Speed " + QString::number(speed) + "; Current way: " + QString::number(CurrentWayIndex) + " EFIS: " + QString(LIdent.c_str()) + " Name: " + CurrentLeg.Name + " Type: " + CurrentLeg.Type + " Total ways: " + QString::number(Legs->size()), false);
		}
		double kDToHed = 0.00004;
		double REangle;
		if ((CurrentWayIndex + 1) <= Legs->size()) {
			REangle = Utils::Constrain180(Legs->at(CurrentWayIndex).HeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue);
		}
		else {
			REangle = Utils::Constrain180(Legs->at(CurrentWayIndex + 1).HeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue);
		}
		double DToHed = abs(REangle) * data->GData.GROUND_VELOCITY * kDToHed;
		if (DToHed < 0.025) {
			DToHed = 0.025;
		}
		if ((CurrentWayIndex + 1) >= Legs->size()) {
			DToHed = 0.001;
		}
		double EangleRel = Utils::Constrain180(CurrentLeg.HeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue);

		double a = Utils::GetFixDA(sin(EangleRel * M_PI / 180) * CurrentLeg.Distance, EangleRel);
		if (Mode == TAXIIN) {
			DCommon = DCommon + AddRadius;
		}
		if (Mode == TAXIOUT) {
			double RWDist = Utils::RunwayDictCalc(&RunWaysPathsOrig, &CurrentLeg);
			if (RWDist < 0.1) {
				SendCommand(LIGHTSTROBE_SET, 1, 0);
				if (DToRunway < 0.15) {
					if (beforeTakeoff == 0) {
						emit CLBeforeTakeoff(&beforeTakeoff);
					}
				}
			}
			else {
				SendCommand(LIGHTSTROBE_SET, 0, 0);
			}
		}
		else {
			if (afterLanding == 0) {
				emit CLAfterLanding(&afterLanding);
			}
			double RWDist = Utils::RunwayDictCalc(&RunWaysPathsDest, &CurrentLeg);
			if (RWDist < 0.1) {
				SendCommand(LIGHTSTROBE_SET, 1, 0);
			}
			else {
				SendCommand(LIGHTSTROBE_SET, 0, 0);
				SendCommand(FLAPS_SET, 0, 0);
				SendCommand(LIGHTLANDING_SET, 0, 0);
				if (((CurrentWayIndex + 1) > Legs->size()) && (DCommon > 0.1)) {
					SendCommand(LIGHTTAXI_SET, 1, 0);
					SendCommand(LIGHTRUNWAY_SET, 1, 0);
				}
			}
		}



		
		if (minDCommon > CurrentLeg.Distance) {
				minDCommon = CurrentLeg.Distance;
		}
		if (((CurrentWayIndex + 1) >= Legs->size()) && (DCommon < 0.003) && ((minDCommon + 0.001) < DCommon)) {
			SendCommand(SET_GSPEED, -5, 0);
			double brake = 0.5; // (PaircraftData->GlideAngle - speed) / 70.0;

			//if (data->GData.GROUND_VELOCITY < 0.1) {
				//SendCommand(SET_THROTTLE, 0, 0);
				//SendCommand(PARKBRAKE_SET, 1, 0);

				SendText("DONE!", true);
				Mode = DONE;
			//}
		}
		if ((abs(EangleRel) > 60) || (CurrentLeg.Distance < DToHed)) {
			if (((CurrentWayIndex + 1) >= Legs->size()) && (DCommon < 0.003) && ((minDCommon + 0.001) < DCommon)) {
				SendCommand(SET_GSPEED, -5, 0);

				//if (data->GData.GROUND_VELOCITY < 0.1) {
					//SendCommand(SET_THROTTLE, 0, 0);
					//SendCommand(PARKBRAKE_SET, 1, 0);

					SendText("DONE!", true);
					Mode = DONE;
					//mode = START;
					//CurrentWayIndex = CurrentWayIndex + 1;
					if (CurrentWayIndex + 1 < Legs->size()) {
						CurrentWayIndex = CurrentWayIndex + 1;
					}
				//}
			}
			else {
				if (CurrentWayIndex + 1 < Legs->size()) {
					CurrentWayIndex = CurrentWayIndex + 1;
					if ((Mode == TAXIOUT) && (Legs->at(CurrentWayIndex).Dist == 2.0)) {
						CurrentWayIndex = SIDPoint + 1;
					}
				}
			}
		}
		else {
			if ((abs(HeadingRel) < 2.0) && (abs(a) < 0.002) && (CurrentLeg.Distance > 0.200)) {
				if (((CurrentWayIndex + 1) != Legs->size()) && ((Legs->at(CurrentWayIndex).Type == "RUNWAY"))) {
					speed = -1;
					SendCommand(LIGHTLANDING_SET, 1, 0);
					SendCommand(LIGHTTAXI_SET, 2, 0);
					if (data->GData.GROUND_VELOCITY < 1) {

						SendCommand(SET_TRUST, 40, 0);
						while ((data->GData.ENG_N1_RPM1 < 0.4) && (data->GData.ENG_N1_RPM2 < 0.4));
						//speed = 180;
						Flight = new WriteStream(AirportData->RootSim + "\\Flight.csv");
						lastlon = CurrentLeg.Lon;
						lastlat = CurrentLeg.Lat;
						Mode = TAKEOFF;
						
						//CurrentWayIndex = Taxiway;

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
				if ((abs(AngleFront) > 60) || (CurrentLeg.Distance < (DToHed + 0.30))) {
					speed = 20;
					//speed = 5;
					if (abs(AngleFront) > 10) {
						speed = 15;
						//speed = 5;
					}
					if (abs(AngleFront) > 30) {
						speed = 12;
						//speed = 5;
					}
					if (abs(AngleFront) > 50) {
						speed = 10;
						//speed = 5;
					}
					if (abs(AngleFront) > 100) {
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
						double brake = 0.5; // (PaircraftData->GlideAngle - speed) / 70.0;

						//if (data->GData.GROUND_VELOCITY < 0.1) {
							//SendCommand(SET_THROTTLE, 0, 0);
							//SendCommand(PARKBRAKE_SET, 1, 0);

							SendText("DONE!", true);
							Mode = DONE;
							//mode = START;
							//CurrentWayIndex = CurrentWayIndex + 1;
							if (CurrentWayIndex + 1 < Legs->size()) {
								CurrentWayIndex = CurrentWayIndex + 1;
							}
						//}
					}
					if ((CurrentWayIndex + 1) >= Legs->size()) {
						SendCommand(LIGHTTAXI_SET, 0, 0);
						SendCommand(LIGHTRUNWAY_SET, 0, 0);
						speed = 3;
					}
					if (DCommon < 0.100) {
						SendCommand(LIGHTTAXI_SET, 0, 0);
						SendCommand(LIGHTRUNWAY_SET, 0, 0);
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
			if (data->GData.GROUND_VELOCITY < 1) {
				if (parking == 0) {
					emit CLParking(&parking);
				}
				SendCommand(PARKBRAKE_SET, 1, 0);
				SendCommand(SET_THROTTLE, 0, 0);
			}
			
		}
	}
	if (Mode == TAKEOFF) {
		std::string LIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_1 });
		std::string RIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_1 });
		//SendCommand(SET_THROTTLE, 4000, 0);
		SendCommand(SET_ATHR, 1, 0);
		double NPitch = (data->AllData.A32NX_FLIGHT_DIRECTOR_PITCH + data->GData.PLANE_PITCH_DEGREES);
		double NBank = data->AllData.A32NX_FLIGHT_DIRECTOR_BANK;
		//FlightDirectorPitch = -15;
		double ISpeed = data->GData.AIRSPEED_INDICATED;
		double SOG = data->GData.SIM_ON_GROUND;
		double VSpeed = data->GData.VERTICAL_SPEED;
		double Alt = data->GData.PLANE_ALTITUDE;
		double Lat = data->GData.PLANE_LATITUDE;
		double Lon = data->GData.PLANE_LONGITUDE;
		double IAlt = data->GData.INDICATED_ALTITUDE;
		double GAlt = data->GData.PLANE_ALT_ABOVE_GROUND - 9;

		double V1 = (data->AllData.AIRLINER_V1_SPEED);
		double VR = (data->AllData.AIRLINER_VR_SPEED);
		double V2 = (data->AllData.AIRLINER_V2_SPEED);
		double VF = (data->AllData.A32NX_SPEEDS_F);
		double VS = (data->AllData.A32NX_SPEEDS_S);
		double VGD = (data->AllData.A32NX_SPEEDS_GD);

		double RT = data->AllData.AIRLINER_THR_RED_ALT;

		

		if (data->GData.ENG_N1_RPM1 > 0.40) {
			if (!TOGA) {
				//setThrust(100);
				SendEvent(KEY_AXIS_LEFT_BRAKE_SET, -16383);
				SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, -16383);

				//TO DO From weight and runway lenght calc engine mode
				SendCommand(SET_THROTTLE, 9000, 0); // FLEX
				//SendCommand(SET_THROTTLE, 16000, 0); // TO GA
				TOGA = TRUE;
			}
		}

		if (ISpeed > 230) {
			
			//FlightDirectorPitch = data->GData.AUTOPILOT_FLIGHT_DIRECTOR_PITCH;
		}
		else if (ISpeed > VS) {
			SendCommand(FLAPS_SET, 0.0, 0);

			//FlightDirectorPitch = data->GData.AUTOPILOT_FLIGHT_DIRECTOR_PITCH;

		}
		else if (ISpeed > VF) {
			SendCommand(FLAPS_SET, 1.0, 0);
			//FlightDirectorPitch = data->GData.AUTOPILOT_FLIGHT_DIRECTOR_PITCH;
		}

		if ((SOG== FALSE)) {
			if (NPitch < 10) {
				//FlightDirectorPitch = data->GData.AUTOPILOT_FLIGHT_DIRECTOR_PITCH;
			}
			else {

				//FlightDirectorPitch = data->GData.AUTOPILOT_FLIGHT_DIRECTOR_PITCH;
				//FlightDirectorPitch = -15;

			}
			SendCommand(SET_RUDDER, 0, 0);
		}
		//ManPitchWithFD(FlightDirectorPitch);
		if (ISpeed > VR) {
			
				if (SOG == 1) {
					ManPitchWithFD(-15);
				}
				if (data->GData.AUTOPILOT_FLIGHT_DIRECTOR_ACTIVE1) {
					if ((GAlt < 500) && (SOG == 0)) {
						ManPitchWithFD(NPitch);
					}
					else {
						ManPitchWithFD(-15);
					}
				}
				else if (SOG == 0) {
					ManPitchWithFD(-10);
				}
			if ((VSpeed > 1000) && (SOG == FALSE) && (GAlt > 100)) {
				double intpParameter = 0.0;
				if (ISpeed > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
					
				}
				//TOGA = FALSE;
				//CurrentWayIndex = CurrentWayIndex + 1;
				//PPID.FT = true;
				//Mode = CLIMB;
			}
			if (GAlt > 500) {
				if (!data->AllData.A32NX_AUTOPILOT_ACTIVE) {
					SendEvent(A32NX_FCU_AP_1_PUSH, 1);
				}
			}

			if (TOGA) {
				if (data->AllData.A32NX_AUTOTHRUST_MODE_MESSAGE) {
					SendCommand(SET_THROTTLE, 4000, 0);
				}
				if (data->AllData.A32NX_FMGC_FLIGHT_PHASE == 2) {
					
					SendCommand(SET_THROTTLE, 4000, 0);
					TOGA = false;
					Mode = CRUISE;
					if (afterTakeoff == 0) {
						emit CLAfterTakeoff(&afterTakeoff);
					}
				}
			}
			
			//ManPitchWithFD(data->GData.FlightDirectorPitch;
			
			

		}
		else if ((ISpeed > V1) && (ISpeed <= VR)) {
			//ManPitchWithFD(0);
			SendCommand(SET_ELEVATOR, -2500, 0);

		}
		else {
			SendCommand(SET_ELEVATOR, 3000, 20);
		}
		if (GAlt > 500) {

			//ManBankWithFD(FlightDirectorBank);
			//BankWithHead(CurrentLeg.HeadingMag);
		}
		else {
			ManBankWithFD(-1);
		}
		CurrentLeg = Legs->at(CurrentWayIndex);
		CurrentLeg.Lat = Lat;
		CurrentLeg.Lon = Lon;
		Utils::DOrtoKM(&CurrentLeg);
		GetRemainingDistance();
		double DCommon = Utils::DToType(CurrentWayIndex, Legs, "GSWAY", &CurrentLeg, 0);
		GRemainingDistance = DCommon;

		double DToChange = CalcToNewWay(true);
		CHAR s[512];
		SendText("MODE: TAKEOFF Current: " + QString::number(CurrentLeg.Distance, 'f', 3) + " EFIS: " + QString(LIdent.c_str()) + " km Total: " + QString::number(DCommon, 'f', 3) + " km \nCurrent way: " + QString::number(CurrentWayIndex) + " Type: " + CurrentLeg.Type + " Total ways: " + QString::number(Legs->size()) + " D for head: " + QString::number(DToChange, 'f', 3), false);
		if (SOG == 1) {
			double HeadingRel = ManHeadWithWay(&Legs->at(CurrentWayIndex));
			SendCommand(SET_AILERON, 0, 20);
		}
		else {
			if (GAlt > 500) {
				//double HeadingRel = BankWithHead(Utils::Constrain180(CurrentLeg.HeadingTrue));
				ManHeadWithWay(&Legs->at(CurrentWayIndex));
			}
			else {
				AltBankWithPos(0);
				SendCommand(SET_RUDDER, 0, 20);
				//double HeadingRel = BankWithHead(Utils::Constrain180(CurrentLeg.HeadingTrue));
			}
		}
	}
	if ((Mode == CRUISE)||(Mode == DESCENT)) {
		QJsonObject root = document.object();
		double FlightDirectorPitch = (data->AllData.A32NX_FLIGHT_DIRECTOR_PITCH + data->GData.PLANE_PITCH_DEGREES);
		double FlightDirectorBank = (data->AllData.A32NX_FLIGHT_DIRECTOR_BANK);
		double Pitch = data->GData.PLANE_PITCH_DEGREES;
		double IAS = data->GData.AIRSPEED_INDICATED;
		double GAS = data->GData.GROUND_VELOCITY;
		double TAS = data->GData.AIRSPEED_TRUE;
		double SimOnGround = data->GData.SIM_ON_GROUND;
		double VerticalSpeed = data->GData.VERTICAL_SPEED;
		double IndicatedAltitude = data->GData.INDICATED_ALTITUDE;
		double WindDir = data->GData.AMBIENT_WIND_DIRECTION;
		double WindSpeed = data->GData.AMBIENT_WIND_VELOCITY;
		double MagVar = data->GData.MAGVAR;
		double GroundAltitude = data->GData.PLANE_ALT_ABOVE_GROUND - 9;
		double TrueHeading = data->GData.PLANE_HEADING_DEGREES_TRUE;
		double FSpeed = (data->AllData.A32NX_SPEEDS_F);
		double SSpeed = (data->AllData.A32NX_SPEEDS_S);
		double GDSpeed = (data->AllData.A32NX_SPEEDS_GD);
		double Lat = data->GData.PLANE_LATITUDE;
		double Lon = data->GData.PLANE_LONGITUDE;
		double FlightPhase = data->AllData.A32NX_FMGC_FLIGHT_PHASE;
		double TD = data->AllData.A32NX_PFD_MSG_TD_REACHED;
		double VerticalSpeedForGlide;
		std::string LIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_L_TO_WPT_IDENT_1 });
		std::string RIdent = Utils::Unpack({ data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_0, data->AllData.A32NX_EFIS_R_TO_WPT_IDENT_1 });
		ChangeFlightPhaseReport();

		CurrentLeg = GetCurrentLeg();

		double RemainingDistance = GetRemainingDistance();
		
		//test 
		if (RemainingDistance < 215) {
			//SendCommand(PUSH_ALT_TEST, 1, 0);
		}
		///////
		GRemainingDistance = RemainingDistance;



		double AngleToDesc = GetDescentAngle();

		// Set approach phase
		if ((RemainingDistance < 30) && (!Approach) && (FlightPhase != 5)) {
			SetDataL(A32NX_APPROACH_STATE, 1); //Verify
			SendCommand(PUSH_SPD, 1, 0);
		}
		
		// Working flaps and gears
		if ((FlightPhase >= 4)) {
			if (IAS < FSpeed + 5) {
				SendCommand(FLAPS_SET, 4.0, 0);
				SendCommand(LIGHTLANDING_SET, 1, 20);	
				SendCommand(GEAR_SET, 1.0, 0);
			}
			else if (IAS < SSpeed - 15) {
				
				SendCommand(FLAPS_SET, 3.0, 0);
			}
			else if (IAS < SSpeed - 10) {
				SendCommand(GEAR_SET, 1.0, 0);
			}
			else if (IAS < SSpeed + 5) {
				SendCommand(FLAPS_SET, 2.0, 0);
				SendCommand(GEAR_SET, 0.0, 0);
			}
			else if (IAS < GDSpeed + 5) {
					SendCommand(LIGHTLANDING_SET, 0, 20);
					SendCommand(FLAPS_SET, 1.0, 0);
			}
		}
		else {
			if (IAS > SSpeed) {
				SendCommand(FLAPS_SET, 0.0, 0);
				SendCommand(LIGHTLANDING_SET, 0, 20);
				if (IAS > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			else if (IAS > FSpeed) {
				SendCommand(FLAPS_SET, 1.0, 0);
				if (IAS > 140) {
					SendCommand(GEAR_SET, 0.0, 0);
				}
			}
			if (GroundAltitude > 500) {
				if (!data->AllData.A32NX_AUTOPILOT_ACTIVE) {
					//SendEvent(A32NX_FCU_AP_1_PUSH, 0);
				}
			}
		}

		// Gear up if positive climb
		if (VerticalSpeed > 1000) {
			SendCommand(GEAR_SET, 0.0, 0);
		}

		// Trans altHi/level works TO DO 10000 Ft
		if (IndicatedAltitude > 10500) {
			SendCommand(LIGHTTAXI_SET, 0, 0);
			SendCommand(LIGHTRUNWAY_SET, 0, 0);
			SendCommand(BARO_MODE, 2, 0);
			SendCommand(SEATBELT_SET, 0.0, 0);
		}
		else if (IndicatedAltitude < 9500) {
			SendCommand(LIGHTTAXI_SET, 2, 0);
			SendCommand(LIGHTRUNWAY_SET, 1, 0);
			SendCommand(BARO_MODE, 1, 0);
			SendCommand(SEATBELT_SET, 1.0, 0);
		}

		// Lower limit flight level
		if (FlightCruise < 1000) {
			FlightCruise = 1000;
		}

		double AngleToFixAlt = 0;// = atan((IndicatedAltitude - CurrentLeg.FinalAlt) / 3280.84 / CurrentLeg.DistToAlt) * 180.0 / M_PI;
		

		if (RemainingDistance > 17) {
			int cI = CurrentWayIndex;
			while ((Legs->at(cI).Type != "RUNWAY") && (cI < (Legs->size() - 1))) {				
				cI++;
			}
			int II = CurrentWayIndex+1;
			QString Id = QString(LIdent.c_str());
			while ((Legs->at(II).Type != "RUNWAY") && (II < (Legs->size() - 1))) {
				
				if (Legs->at(II).Name == Id) {
					CurrentWayIndex = II;
					CurrentLeg = GetCurrentLeg();
					AngleToDesc = GetDescentAngle();
					break;
				}
				II++;
			}
			if (RemainingDistance < 20) {
				CurrentWayIndex = cI;
				CurrentLeg = GetCurrentLeg();
				AngleToDesc = GetDescentAngle();
				if (data->AllData.A32NX_AUTOPILOT_1_ACTIVE) {
					SendEvent(A32NX_FCU_AP_1_PUSH, 0);
				}
			}
			else {
				if (TD == 1) {
					SendCommand(PUSH_ALT_TEST, 0, 0); // Future working
					Mode = DESCENT;
				}
			}
			
			if ((Mode == DESCENT) && ((IndicatedAltitude - Legs->at(cI).SAltitudeHi) < 3000)) {
				//CurrentWayIndex = cI;
			}
			// Create vertical profile //TO DO Big work
			if ((IndicatedAltitude > 10000) && (CurrentLeg.FinalAlt < 10000)) {
				VerticalSpeedForGlide = GetVerticalSpeedForGlide(&Legs->at(CurrentWayIndex), 3.0, CurrentLeg.FinalAlt - 500, -20);
				AngleToFixAlt = atan(((IndicatedAltitude - CurrentLeg.FinalAlt - 500) * 0.0003048) / (CurrentLeg.DistToAlt + 0.05 - 20)) * 180.0 / M_PI;
			}
			else if (IndicatedAltitude > 10000) {
				VerticalSpeedForGlide = GetVerticalSpeedForGlide(&Legs->at(CurrentWayIndex), 3.0, CurrentLeg.FinalAlt - 500, -10);
				AngleToFixAlt = atan(((IndicatedAltitude - CurrentLeg.FinalAlt - 500) * 0.0003048) / (CurrentLeg.DistToAlt + 0.05 - 5)) * 180.0 / M_PI;
			}
			else {
				if (FlightPhase < 5) {
					VerticalSpeedForGlide = GetVerticalSpeedForGlide(&Legs->at(CurrentWayIndex), 2.7, CurrentLeg.FinalAlt - 500, -10);
					AngleToFixAlt = atan(((IndicatedAltitude - CurrentLeg.FinalAlt - 500) * 0.0003048) / (CurrentLeg.DistToAlt + 0.05 - 5)) * 180.0 / M_PI;
				}
				else {
					VerticalSpeedForGlide = GetVerticalSpeedForGlide(&Legs->at(CurrentWayIndex), 2.7, CurrentLeg.FinalAlt - 500, 0);
					AngleToFixAlt = atan(((IndicatedAltitude - CurrentLeg.FinalAlt - 500) * 0.0003048) / (CurrentLeg.DistToAlt + 0.05)) * 180.0 / M_PI;
				}
			}
			
			if (FlightCruise > (IndicatedAltitude - 100)) {
				SendCommand(ALT_SEL, FlightCruise, 20);
				if (FlightCruise != MaximumAltitude) {
					SendCommand(PULL_ALT, 0, 0);
				}
				else {
					//SendCommand(PUSH_ALT, 0, 0);
					SendCommand(PULL_ALT, 0, 0);
				}
			}
			if (FlightCruise < (IndicatedAltitude - 100)) {
				//SendCommand(VS_SEL, 0, 20);
			}
			
			if ((AngleToDesc >= 122.6/*GetAngleToDesc(IndicatedAltitude) */) || (Mode == DESCENT)) {
				Mode = DESCENT;
				SendCommand(ALT_SEL, FlightCruise, 20);
				SendCommand(ALT_TEST, 2000, 20); // Future working
				if (FlightCruise > (IndicatedAltitude - 100)) {
					if (FlightCruise != MaximumAltitude) {
						SendCommand(PULL_ALT, 0, 0);
					}
					else {
						//SendCommand(PUSH_ALT, 0, 0);
						SendCommand(PULL_ALT, 0, 0);
					}
				}
				else {

					

					if ((IndicatedAltitude - 100) > FlightCruise) {
						if ((IAS - 10) < data->AllData.A32NX_SPEEDS_MANAGED_PFD) {
							if (((IndicatedAltitude - FlightCruise) < 1000) && (VerticalSpeedForGlide < -1000)) {
								SendCommand(VS_SEL, int((FlightCruise - IndicatedAltitude) / 100) * 100, 20);
							}
							else {
								if (IAS > 200) {
									if (VerticalSpeedForGlide < -2500) {
										SendCommand(VS_SEL, -2500, 20);
									}
									else {
										SendCommand(VS_SEL, int(VerticalSpeedForGlide / 100) * 100/* - 200*/, 20);
									}
								}
								else {
									if (VerticalSpeedForGlide < -2000) {
										SendCommand(VS_SEL, -2000, 20);
									}
									else {
										SendCommand(VS_SEL, int(VerticalSpeedForGlide / 100) * 100/* - 200*/, 20);
									}
								}
							}
						}
						else {
							if (IAS > 200) {
								if (VerticalSpeedForGlide < -1500 + (IAS - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) {
									if ((-1500 + (IAS - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) > -500) {
										SendCommand(VS_SEL, -500, 20);
									}
									else {
										SendCommand(VS_SEL, int((-1500 + (IAS - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) / 100) * 100, 20);
									}
								}
								else {
									SendCommand(VS_SEL, int(VerticalSpeedForGlide / 100) * 100, 20);
								}
							}
							else {
								if (VerticalSpeedForGlide < -1000 + (IAS - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) {
									if ((-1000 + (IAS - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) > -500) {
										SendCommand(VS_SEL, -500, 20);
									}
									else {
										SendCommand(VS_SEL, int((-1000 + (IAS - data->AllData.A32NX_SPEEDS_MANAGED_PFD) * 50) / 100) * 100, 20);
									}
								}
								else {
									SendCommand(VS_SEL, int(VerticalSpeedForGlide / 100) * 100, 20);
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
			if (landing == 0) {
				emit CLLanding(&landing);
			}
			//LANDING
			SendCommand(AUTOBRAKES_SET, 1, 0);
			VerticalSpeedForGlide = GetVerticalSpeedForGlide(&Legs->at(CurrentWayIndex), 3.0, Legs->at(CurrentWayIndex).EAltitudeHi);
			AngleToFixAlt = atan(((IndicatedAltitude - CurrentLeg.FinalAlt) * 0.0003048) / (CurrentLeg.DistToAlt - Legs->at(CurrentWayIndex).Distance + Legs->at(CurrentWayIndex).Dist+ 0.25)) * 180.0 / M_PI;
			//VerticalSpeedForGlide = ManVSWithAngle(3);
			
			if (SimOnGround) {
				if (GAS < 15) {
					SendCommand(SET_THROTTLE, 0, 0);
					SendCommand(SET_GSPEED, 10, 0);
					SendCommand(SPOILER_SET, 0, 0);
					//SendCommand(LIGHTSTROBE_SET, 0, 0);
					//SendCommand(FLAPS_SET, 0, 0);

					AirportList ApList = AirportData->GetAirportByIcao(root["destination"].toObject()["icao_code"].toString().toStdString());
					DWORD indexPath = AirportData->GetNearTaxiwayPath(Lat, Lon, TrueHeading);
					TaxiwayParks* Park = new TaxiwayParks();
					std::vector<TPath> Path = AirportData->GetPathGate(indexPath, TrueHeading, 15.0, Park);
					DWORD PI = Park->TaxiParkInfo;
					AddRadius = (Park->Radius - 13) / 1000;
					DWORD PIi = (PI >> 12) & 0xfff;
					DWORD PIt = (PI >> 8) & 0xf;
					DWORD PIn = PI & 0x1f;
					ParkN = new std::string(ParkType[PIt] + " " + std::to_string(PIi) + " " + ParkName[PIn]);
					Legs->resize(CurrentWayIndex + 1);
					//CountWays = CurrentWayIndex + 1;
					AddWayPoint(Lon, Lat, 0.0, "RUNWAY", "Go out", TrueHeading, 0, 0, 0,0.0);
					for (int i = 0; i < Path.size() - 1; i++) {
						AddWayPoint(Path.at(i).Lon, Path.at(i).Lat, 0.0, TYPE_PATHS[Path[i].Type], QString(Path.at(i).name.c_str()), 0, 0, 0, 0,0.0);
					}
					AddWayPoint(Path.at(Path.size() - 1).Lon, Path.at(Path.size() - 1).Lat, 0, "GATE", QString(Path.at(Path.size() - 1).name.c_str()),0, 0, 0, 0,0.0);
					CurrentWayIndex = CurrentWayIndex + 2;
					if (Flight) {
						delete Flight;
						Flight = NULL;
					}
					Mode = TAXIIN;
				}
				else if (IAS > 60) {
					SendCommand(SET_THROTTLE, -14000, 0);
					SendCommand(SET_AILERON, 0, 0);
					SendCommand(SET_ELEVATOR, 0, 0);
				}
				else if (IAS > 40) {
					SendCommand(SET_THROTTLE, -3500, 0);
				}
				
				else {
					SendCommand(SET_THROTTLE, 0, 0);
					SendCommand(SET_GSPEED, -1, 0);
				}
			}
			else if (GroundAltitude < 20) {
				SendCommand(SET_THROTTLE, 0, 0);
			}
			else {
				SendCommand(ALT_SEL, 3000, 20);
				SendCommand(VS_SEL, int(VerticalSpeedForGlide / 100) * 100, 20);
				//SendCommand(PULL_VS, 0, 0);
				SendCommand(SPOILER_SET, -1, 0);
				SendCommand(PARKBRAKE_SET, 0, 0);
			}
		}
		
		double HeadingRel;
		
		if (SimOnGround == 0) {
			firstLand = true;
			DTForLanding++;
			/*if (GroundAltitude > 30) {
				NPitchWork = FlightDirectorPitch;
				if (IndicatedAltitude < 1000) {
					AvgCounter++;
					AvgMax = AvgMax + NPitchWork;
					AvgPitch = AvgMax / AvgCounter;
				}
			}
			else {		
				NPitchWork = AvgPitch;
			}
			if (GroundAltitude < 10) {
				ManPitchWithFD(NPitchWork-2);
				//AltPitchWithPos(NPitchWork - 2);
			}
			else {
				
				ManPitchWithFD(NPitchWork);
				//AltPitchWithPos(NPitchWork);
			}*/
			if ((GroundAltitude < 50) && Pitch50 == 8518) {
				Pitch50 = Pitch;
			}
			if (GroundAltitude > 100) {
				ManPitchWithFD(FlightDirectorPitch);
			}
			else if (GroundAltitude > 30) {
				if ((DTForLanding % 10) == 0) {
					emit SendLog("Alt: " + QString::number(GroundAltitude) + "; FDP: " + QString::number(FlightDirectorPitch) + "; Pitch: " + QString::number(Pitch) + ";VS: " + QString::number(VerticalSpeed));
				}
				//ManPitchWithFD(Pitch50);
				ManPitchWithFD(-2.5);
			}
			else if (GroundAltitude > 20) {
				if ((DTForLanding % 10) == 0) {
					emit SendLog("Alt: " + QString::number(GroundAltitude) + "; FDP: " + QString::number(FlightDirectorPitch) + "; Pitch: " + QString::number(Pitch) + ";VS: " + QString::number(VerticalSpeed));
				}
				//ManPitchWithFD(Pitch50);
				ManPitchWithFD(-3.0);
			}
			else {
				//ManPitchWithFD(Pitch50 - 2.0);
				if ((DTForLanding % 10) == 0) {
					emit SendLog("Alt: " + QString::number(GroundAltitude) + "; FDP: " + QString::number(FlightDirectorPitch) + "; Pitch: " + QString::number(Pitch) + ";VS: " + QString::number(VerticalSpeed));
				}
				ManPitchWithFD(-4.0);
			}
			
			
			//AltPitchWithPos(-FlightDirectorPitch);
			HeadingRel = ManHeadWithWay(&Legs->at(CurrentWayIndex));

		}
		else {
			if (firstLand) {
				emit SendLog("VS: " + QString::number(VerticalSpeed) + "; Pitch: " + QString::number(Pitch) + "; IAS: " + QString::number(IAS));
				firstLand = false;
			}
			if (GAS < 100) {
				SendCommand(SET_ELEVATOR, 0, 0);
				SendCommand(SET_AILERON, 0, 0);
			}
			else {
				ManPitchWithFD(0);
			}
			
			//AltPitchWithPos(0);
			//AltPitchWithPos(-15);
			HeadingRel = ManHeadWithWay(&Legs->at(CurrentWayIndex));
		}
		
		double DToChange = CalcToNewWay();

		SendText("MODE: CRUISE \"" + Legs->at(STARPoint).Name + "\" Current: " + QString::number(CurrentLeg.Distance, 'f', 3) + " GroundAltitude: " + QString::number(GroundAltitude, 'f', 3) + " km Total: " + QString::number(RemainingDistance, 'f', 3) + " km \nCurrent way: " + QString::number(CurrentWayIndex) + " Type: " + CurrentLeg.Type + " EFIS: " + QString(LIdent.c_str()) + " Name: \"" + CurrentLeg.Name + "\" Total ways: " + QString::number(Legs->size()) + " D for head: " + QString::number(DToChange, 'f', 3) + " \n SSpeed: " + QString::number(VerticalSpeedForGlide) + " D to altHi: " + QString::number(CurrentLeg.DistToAlt, 'f', 3) + " Next altHi: " + QString::number(CurrentLeg.FinalAlt, 'f', 3) + " \n ATD: " + QString::number(AngleToDesc, 'f', 3) + " Current angle: " + QString::number(GetAngleToDesc(IndicatedAltitude), 'f', 3) + " Angle to next alt: " + QString::number(AngleToFixAlt, 'f', 3), false);

	}
		
	if (Quit) {
		Connect();
	}
	
}

double MainLogic::CalcToNewWay(bool changeWay) {
	double REangle = 0.045;
	DWORD Curr = CurrentWayIndex;
	double SOG = data->GData.SIM_ON_GROUND;
	if ((CurrentWayIndex + 1) < Legs->size()) {
		REangle = Legs->at(CurrentWayIndex + 1).HeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue;
	}
	REangle = Utils::Constrain180(REangle);
	REangle = fabs(-REangle * 5 * data->GData.AIRSPEED_INDICATED / 100000);
	if (data->GData.SIM_ON_GROUND == 0) {
		if (REangle < 2.0) {
			REangle = 2.0;
		}
	}

	double EangleRel = CurrentLeg.EndHeadingTrue - Legs->at(CurrentWayIndex).EndHeadingTrue;
	EangleRel = Utils::Constrain180(EangleRel);
	if ((CurrentLeg.Type == "HEADINGUNTILALT")) {
		if ((data->GData.INDICATED_ALTITUDE > CurrentLeg.EAltitudeHi)) {
			if (Legs->at(CurrentWayIndex + 1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (data->GData.AIRSPEED_INDICATED > 1)) { //TO DO
					//SendCommand(PULL_SPD, 1, 0);
					//SendCommand(SPD_SEL, Legs->at(CurrentWayIndex + 1).IndSpeed, 0);
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
	else if (CurrentLeg.Type == "HEADINGUNTILFROMDIST") {
		if (CurrentLeg.Distance > CurrentLeg.Dist) {
			if (Legs->at(CurrentWayIndex + 1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (data->GData.AIRSPEED_INDICATED > 1)) { //TO DO
					//SendCommand(PULL_SPD, 1, 0);
					//SendCommand(SPD_SEL, Legs->at(CurrentWayIndex + 1).IndSpeed, 0);
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
	else if (CurrentLeg.Type == "TRKINTERCEPTRADIAL") {
		static int fl;
		if (abs(abs(CurrentLeg.EndHeadingTrue) - abs(Legs->at(CurrentWayIndex).Dist)) < fl) {
			if (abs(abs(CurrentLeg.EndHeadingTrue) - abs(Legs->at(CurrentWayIndex).Dist)) < 1.0) {
				if (Legs->at(CurrentWayIndex + 1).IndSpeed > 0) {
					if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (data->GData.AIRSPEED_INDICATED > 1)) { //TO DO
						//SendCommand(PULL_SPD, 1, 0);
						//SendCommand(SPD_SEL, Legs->at(CurrentWayIndex + 1).IndSpeed, 0);
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
			if ((CurrentLeg.Distance < REangle)) {
				if (Legs->at(CurrentWayIndex + 1).IndSpeed > 0) {
					if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (data->GData.AIRSPEED_INDICATED > 1)) { //TO DO
						//SendCommand(PULL_SPD, 1, 0);
						//SendCommand(SPD_SEL, Legs->at(CurrentWayIndex + 1).IndSpeed, 0);
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
			if (Legs->at(CurrentWayIndex + 1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (data->GData.AIRSPEED_INDICATED > 1)) { //TO DO
					//SendCommand(PULL_SPD, 1, 0);
					//SendCommand(SPD_SEL, Legs->at(CurrentWayIndex + 1).IndSpeed, 0);
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
		fl = abs(abs(CurrentLeg.EndHeadingTrue) - abs(Legs->at(CurrentWayIndex).Dist)) < fl;
	}
	else if ((CurrentLeg.Type == "FIX") || (CurrentLeg.Type == "TURNLEFTDIRECTFIX") || (CurrentLeg.Type == "TURNRIGHTDIRECTFIX")) {
		if ((CurrentLeg.Distance < REangle)) {
			if (Legs->at(CurrentWayIndex + 1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (data->GData.AIRSPEED_INDICATED > 1)) { //TO DO
					//SendCommand(PULL_SPD, 1, 0);
					//SendCommand(SPD_SEL, Legs->at(CurrentWayIndex + 1).IndSpeed, 0);
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
		if ((EangleRel > 60) || (EangleRel < -60) || (CurrentLeg.Distance < REangle)) {
			if (Legs->at(CurrentWayIndex + 1).IndSpeed > 0) {
				if ((data->AllData.A32NX_FMGC_FLIGHT_PHASE < 5) && (data->GData.AIRSPEED_INDICATED > 1)) { //TO DO
					//SendCommand(PULL_SPD, 1, 0);
					//SendCommand(SPD_SEL, Legs->at(CurrentWayIndex + 1).IndSpeed, 0);
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
	if ((Mode == TAKEOFF) && (data->GData.INDICATED_ALTITUDE > 500)) {
		CurrentWayIndex = Curr;
		if ((Mode == TAXIOUT) && (Legs->at(CurrentWayIndex).Dist == 2.0)) {
			CurrentWayIndex = SIDPoint + 1;
		}
	}
	else if (((Mode == CRUISE) || (Mode == DESCENT)) && (SOG)) {
		CurrentWayIndex = CurrentWayIndex;
	}
	else if (CurrentLeg.Type == "RUNWAY") {
		CurrentWayIndex = CurrentWayIndex;
	}
	else if ((Mode == TAXIOUT) && (Legs->at(CurrentWayIndex).Dist == 2.0)) {
		CurrentWayIndex = SIDPoint + 1;
	}
	else if (Mode != TAKEOFF) {
		CurrentWayIndex = Curr;
		if ((Mode == TAXIOUT) && (Legs->at(CurrentWayIndex).Dist == 2.0)) {
			CurrentWayIndex = SIDPoint + 1;
		}
	}
	
	return REangle;
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

double MainLogic::GetDescentAngle() {
	
	double MaxAlt = MaximumAltitude;
	if (!Connected) {
		double Dist = ui->dist->value();
		for (int i = 0; i < Legs->size(); i++) {
			if (Legs->at(i).CommonDistance > Dist) {
				CurrentWayIndex = i-1;
				CurrentLeg = Legs->at(i-1);
				CurrentLeg.RealDistance = Dist - CurrentLeg.CommonDistance;
				CurrentLeg.Distance = CurrentLeg.Distance - CurrentLeg.RealDistance;
				break;
			}
		}
	}
	if (CurrentWayIndex == 0) {
		return 0;
	}
	int wi = CurrentWayIndex;
	double IndicatedAltitude;
	if (Connected) {
		IndicatedAltitude = data->GData.INDICATED_ALTITUDE;
	}
	else {
		IndicatedAltitude = ui->Alt->value();
	}
	int PrewionsFixIndex = -1;
	double DistanceToPrewionsAltitude;

	while ((Legs->at(wi).FixAlt != CRUISEALT) && (wi < (Legs->size()))) {
		wi++;
		if (Legs->size() == wi) {
			break;
		}
	}
	if (wi != Legs->size()) {
		MaximumAltitude = Legs->at(wi).EAltitudeHi;
	}
	double D = 0;;
	if (wi != 0) {
		CurrentLeg.EAltitudeHi = MaxAlt;
		CurrentLeg.DistToAlt = D;
		CurrentLeg.FinalAlt = MaxAlt;
		int fi = wi;
		//int PrewionsFixIndex = -1;
		for (fi = wi - 1; fi >= 0; fi--) {
			double GeometricAngle;
			DistanceToPrewionsAltitude = 0.0;
			if (Legs->at(fi).FixAlt == FIXALT) {
				if (PrewionsFixIndex == -1) {
					PrewionsFixIndex = fi;
				}
				else {

					for (int icw = fi; icw < PrewionsFixIndex; icw++) {
						DistanceToPrewionsAltitude += Legs->at(icw).Distance;
					}
					GeometricAngle = atan((Legs->at(fi).EAltitudeHi - Legs->at(PrewionsFixIndex).EAltitudeHi) / 3280.84 / DistanceToPrewionsAltitude) * 180.0 / M_PI;
					for (int icw = fi; icw < PrewionsFixIndex; icw++) {
						Legs->at(icw).GeometricAngle = GeometricAngle;
					}
					PrewionsFixIndex = fi;
				}
			}
			if (Legs->at(fi).Type == "RUNWAY") {
				for (int icw = fi; icw < PrewionsFixIndex; icw++) {
					Legs->at(icw).GeometricAngle = 0.0;
				}
			}
		}
		fi = wi;
		double angle = 0.0;
		double doubleAlt = 0.0;
		PrewionsFixIndex = -1;
		for (fi = wi - 1; fi >= CurrentWayIndex; fi--) {
			//Legs->at(fi).GeometricAngle = Legs->at(fi + 1).GeometricAngle;
			if ((MaxAlt > Legs->at(fi).EAltitudeHi) && ((Legs->at(fi).FixAlt == ALTORABOVE) || (Legs->at(fi).FixAlt == ALTORBELOW) || (Legs->at(fi).FixAlt == FIXALT))) {
				MaxAlt = Legs->at(fi).EAltitudeHi;
				D = 0.0;
				DistanceToPrewionsAltitude = 0.0;
				int icw;
				for (int icw = fi; icw > CurrentWayIndex; icw--) {
					D += Legs->at(icw).Distance;
				}
				D += CurrentLeg.Distance;
				for (int icw = fi; icw < PrewionsFixIndex; icw++) {
					DistanceToPrewionsAltitude += Legs->at(icw).Distance;
				}

				double Tangle = atan((IndicatedAltitude - Legs->at(fi).EAltitudeHi) / 3280.84 / D) * 180.0 / M_PI;
				double GeometricAngle = 0;
				if (PrewionsFixIndex >= 0) {
					GeometricAngle = atan((Legs->at(fi).EAltitudeHi - Legs->at(PrewionsFixIndex).EAltitudeHi) / 3280.84 / DistanceToPrewionsAltitude) * 180.0 / M_PI;
				}
		
				switch (int(Legs->at(fi).FixAlt)) {
				case FIXALT: {
					angle = Tangle;
					CurrentLeg.FinalAlt = Legs->at(fi).EAltitudeHi;
					doubleAlt = Legs->at(fi).EAltitudeHi;
					CurrentLeg.DistToAlt = D;
					PrewionsFixIndex = fi;
					Legs->at(fi).GeometricAngle = GeometricAngle;
					break;
				}
				case ALTORBELOW: {
					if (angle < Tangle) {
						angle = Tangle;
						CurrentLeg.FinalAlt = Legs->at(fi).EAltitudeHi;
						//if (doubleAlt >= Legs->at(fi).EAltitudeHi) {
						doubleAlt = Legs->at(fi).EAltitudeHi;
						CurrentLeg.DistToAlt = D;
						PrewionsFixIndex = fi;
						Legs->at(fi).GeometricAngle = GeometricAngle;
						//}
					}
					break;
				}
				case ALTORABOVE: {
					if (angle > Tangle) {
						angle = Tangle;
						CurrentLeg.FinalAlt = Legs->at(fi).EAltitudeHi;
						//if (doubleAlt <= Legs->at(fi).EAltitudeHi) {
						doubleAlt = Legs->at(fi).EAltitudeHi;
						CurrentLeg.DistToAlt = D;
						PrewionsFixIndex = fi;
						Legs->at(fi).GeometricAngle = GeometricAngle;
						//}
					}
					break;
				}
				default: {
					break;
				}
				}
			}

		}
	}
	else {
		MaxAlt = CurrentLeg.EAltitudeHi;
		D += CurrentLeg.Distance;
	}

	FlightCruise = (int(MaxAlt / 100.0)) * 100;
	if ((wi != Legs->size()) && (wi != CurrentWayIndex)) {
		
		double Tangle = atan((IndicatedAltitude - MaxAlt) / 3280.84 / D) * 180.0 / M_PI;

		return Tangle;
	}

	//DESCENT
	wi = CurrentWayIndex;
	while (((Legs->at(wi).FixAlt != FIXALT)) && (wi < (Legs->size() - 1))) {
		wi++;
	}
	if (wi == Legs->size()) {
		return -1.0;
	}
	double angle = 0.0;
	double doubleAlt = 0.0;
	int fi = wi;
	for (fi = wi; fi >= CurrentWayIndex; fi--) {
		double D = 0.0;
		int icw;
		for (int icw = fi; icw > CurrentWayIndex; icw--) {
			D += Legs->at(icw).Distance;
		}
		D += CurrentLeg.Distance;

		double Tangle = atan((IndicatedAltitude - Legs->at(fi).EAltitudeHi) / 3280.84 / D) * 180.0 / M_PI;
		/*if (doubleAlt < Legs->at(fi).EAltitudeHi) {
			doubleAlt = Legs->at(fi).EAltitudeHi;
		}*/
		switch (int(Legs->at(fi).FixAlt)) {
		case FIXALT: {
			angle = Tangle;
			CurrentLeg.FinalAlt = Legs->at(fi).EAltitudeHi;
			doubleAlt = Legs->at(fi).EAltitudeHi;
			CurrentLeg.DistToAlt = D;
			break;
		}
		case ALTORBELOW: {
			if (angle < Tangle) {
				angle = Tangle;
				CurrentLeg.FinalAlt = Legs->at(fi).EAltitudeHi;
				//if (doubleAlt >= Legs->at(fi).EAltitudeHi) {
				doubleAlt = Legs->at(fi).EAltitudeHi;
				CurrentLeg.DistToAlt = D;
				//}
			}
			break;
		}
		case ALTORABOVE: {
			if (angle > Tangle) {
				angle = Tangle;
				CurrentLeg.FinalAlt = Legs->at(fi).EAltitudeLo;
				//if (doubleAlt <= Legs->at(fi).EAltitudeHi) {
				doubleAlt = Legs->at(fi).EAltitudeHi;
				CurrentLeg.DistToAlt = D;
				//}
			}
			break;
		}
		default: {
			break;
		}
		}
	}
	if (angle > GetAngleToDesc(IndicatedAltitude) || (Mode == DESCENT)) {
		CurrentLeg.EAltitudeHi = doubleAlt;
		FlightCruise = (int(doubleAlt / 100.0)) * 100;
	}
	return angle;
	//}
	return 0;
}

double MainLogic::ManVSWithAngle(double GS) {

	double TToTarget = (CurrentLeg.DistToAlt) / data->GData.GROUND_VELOCITY / 1.852;
	double CurrentNeedAlt = tan(GS / 180.0 * M_PI) * CurrentLeg.DistToAlt * 3280.84 + CurrentLeg.EAltitudeHi;
	double VSWay = -(CurrentNeedAlt - CurrentLeg.EAltitudeHi)/**0.0003048*/ / TToTarget / 60/**54.681*/;
	int VS = (VSWay);// +(CurrentNeedAlt - PaircraftData->altHi) * 10);
	if (VS > 0) {
		VS = 0;
	}
	return VS;
}

double MainLogic::GetVerticalSpeedForGlide(sWayPoint* Leg, double GlideAngle, double TargetAlt, double BiasDistance) {
	double GS = data->GData.GROUND_VELOCITY;
	double IndicatedAltitude = data->GData.INDICATED_ALTITUDE;
	double PlaneAltitude = data->GData.PLANE_ALTITUDE;
	double GroundAltitude = data->GData.PLANE_ALT_ABOVE_GROUND - 9;
	double TimeToNextWay;
	double CurrentNeedAlt;

	if (Leg->Type == "RUNWAY") {
		TimeToNextWay = ((CurrentLeg.Distance - Leg->Distance + Leg->Dist + 0.25) / 1.852) / GS; //Hour
		CurrentNeedAlt = (((tan(GlideAngle / 180 * M_PI)) * (CurrentLeg.Distance - Leg->Distance + Leg->Dist + 0.25)) * 3280.84) + Leg->EAltitudeHi; //Feet
	}
	else {
		TimeToNextWay = ((CurrentLeg.DistToAlt + BiasDistance + 0.05) / 1.852) / GS;
		CurrentNeedAlt = (((tan(GlideAngle / 180 * M_PI)) * (CurrentLeg.DistToAlt + BiasDistance + 0.05)) * 3280.84) + TargetAlt;
	}

	double VSWay = -(((CurrentNeedAlt - Leg->EAltitudeHi) * 0.0003048) /*Km*/ / TimeToNextWay) * 54.681; // Feet per Minute
	int NeedVerticalSpeed = (VSWay + (CurrentNeedAlt - IndicatedAltitude) * 5);

	if (NeedVerticalSpeed > 0) {
		NeedVerticalSpeed = 0;
	}

	if ((IndicatedAltitude - Legs->at(Legs->size() - 1).EAltitudeHi) < -NeedVerticalSpeed / 6) {
	//if ((PlaneAltitude - Legs->at(Legs->size() - 1).EAltitudeHi) < -NeedVerticalSpeed / 6) {
		if ((CurrentLeg.Distance - Leg->Distance - 0.1) < 0) {
			NeedVerticalSpeed = -GroundAltitude * 5;
		}
	}
	if (GroundAltitude < 40) {
		NeedVerticalSpeed = 0;
	}
	return NeedVerticalSpeed;
}

void MainLogic::SendDataPMDG(DWORD val) {
	if (Connected) {
		SendCommand(SEND_COMMAND_PMDG, val, 0);
	}
}

void MainLogic::StartStopSim() {
	if (Mode == PREPREPARE) {
		cabinWork->Work = true;
		Mode = PREPARE;
	}
	if (MainTimer->isActive()) {
		MainTimer->stop();
	}
	else {
		MainTimer->start();
	}
	/*if (Mode == TAXIOUT) {
		data->Timer->stop();
		//MainTimer->stop();
	}*/
}

// Main connect method

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
			int sz = sizeof(sAllData);
			hr = (SimConnect_AddToClientDataDefinition(HSimConnect, A32NX_ALLDATA_DEFINITION, 0, sizeof(sAllData)));
			//hr = (SimConnect_RequestClientData(HSimConnect, A32NX_ALLDATA_ID, REQ_ALL_DATA, A32NX_ALLDATA_DEFINITION,
//				SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT, 0, 0, 0));


			SUCC(SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_TEXT));
			SUCC(SimConnect_RequestSystemState(HSimConnect, REQ_AIR_PATH, "AircraftLoaded"));
			hr = (SimConnect_SubscribeToSystemEvent(HSimConnect, EVENT_SIM_START, "SimStart"));

			hr = (SimConnect_MapClientEventToSimEvent(HSimConnect, EXTERNAL_SYSTEM_SET, "EXTERNAL_SYSTEM_SET"));
			hr = (SimConnect_MapClientEventToSimEvent(HSimConnect, EXTERNAL_SYSTEM_TOGGLE, "EXTERNAL_SYSTEM_TOGGLE"));

			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_REGISTER_VARIABLE_QUERY, "WASM.RegisterVariableQuery");
			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_REGISTER_VARIABLE_RESPONSE, "WASM.RegisterVariableResponse");
			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_GET_VARIABLE_QUERY, "WASM.GetVariableQuery");
			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_GET_VARIABLE_RESPONSE, "WASM.GetVariableResponse");
			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_SET_VARIABLE_QUERY, "WASM.SetVariableQuery");
			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_SET_VARIABLE_RESPONSE, "WASM.SetVariableResponse");
			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_EXEC_CODE_QUERY, "WASM.ExecCodeQuery");
			//hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_EXEC_CODE_RESPONSE, "WASM.ExecCodeResponse");
			hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_MENU_OPEN, "WASM.MenuOpen");
			hr = SimConnect_MapClientEventToSimEvent(HSimConnect, EVENT_MENU_CHOISE, "WASM.MenuChoice");

			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_REGISTER_VARIABLE_QUERY, TRUE);
			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_REGISTER_VARIABLE_RESPONSE, FALSE);
			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_GET_VARIABLE_QUERY, TRUE);
			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_GET_VARIABLE_RESPONSE, FALSE);
			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_SET_VARIABLE_QUERY, TRUE);
			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_SET_VARIABLE_RESPONSE, FALSE);
			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_EXEC_CODE_QUERY, TRUE);
			//hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_EXEC_CODE_RESPONSE, FALSE);
			hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_MENU_OPEN, FALSE);
			hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EVENT_MENU_CHOISE, FALSE);

			hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EXTERNAL_SYSTEM_SET);
			hr = SimConnect_AddClientEventToNotificationGroup(HSimConnect, 0, EXTERNAL_SYSTEM_TOGGLE);
			hr = SimConnect_SetNotificationGroupPriority(HSimConnect, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST);

			hr = (SimConnect_SubscribeToSystemEvent(HSimConnect, EVENT_6HZ, "6Hz"));
			SUCC(SimConnect_AddToDataDefinition(HSimConnect, DEF_AIRCRAFT_USER, "Title", NULL, SIMCONNECT_DATATYPE_STRING256));
			//SUCC(SimConnect_MapClientEventToSimEvent(HSimConnect, A32NX_APU_FLAP_OPEN_PERCENTAGE, "Autoflight.A32NX_APU_FLAP_OPEN_PERCENTAGE"));


			




			data = new SimData(HSimConnect);
			
			planesWork->DataT = data;

			TData = new ExQThread("data", utils);
			cabinWork = new CabinWork(HSimConnect);
			TCabinWork = new ExQThread("cabinWork", utils);
			cabinWork->DataT = data;
			data->moveToThread(TData);
			cabinWork->moveToThread(TCabinWork);
			
			
			TData->start();
			
			ConnectConnectors = new std::vector<QMetaObject::Connection>();
			//ConnectConnectors->push_back(connect(this, SIGNAL(GetDataSignal(DWORD, DWORD, double*, const char*)), data, SLOT(GetData(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(GetDataSignalL(DWORD, DWORD, double*, const char*)), data, SLOT(GetDataL(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(GetDataStringSignal(DWORD, DWORD, std::string*)), data, SLOT(GetDataString(DWORD, DWORD, std::string*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SetDataSignal(DWORD, DWORD, double*, const char*)), data, SLOT(SetData(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SetDataSignalL(DWORD, DWORD, double*, const char*)), data, SLOT(SetDataL(DWORD, DWORD, double*, const char*))));
			//ConnectConnectors->push_back(connect(this, SIGNAL(SetGetDataSignal(DWORD, DWORD, DWORD, double*, const char*)), data, SLOT(SetGetData(DWORD, DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SendEventSignal(DWORD, DWORD, long)), data, SLOT(SendEvent(DWORD, DWORD, long))));

			//ConnectConnectors->push_back(connect(cabinWork, SIGNAL(GetDataSignal(DWORD, DWORD, double*, const char*)), data, SLOT(GetData(DWORD, DWORD, double*, const char*))));
			//ConnectConnectors->push_back(connect(cabinWork, SIGNAL(GetDataSignalL(DWORD, DWORD, double*, const char*)), data, SLOT(GetDataL(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetDataSignal(DWORD, DWORD, double*, const char*)), data, SLOT(SetData(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetDataSignalL(DWORD, DWORD, double*, const char*)), data, SLOT(SetDataL(DWORD, DWORD, double*, const char*))));
			//ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetGetDataSignal(DWORD, DWORD, DWORD, double*, const char*)), data, SLOT(SetGetData(DWORD, DWORD, DWORD, double*, const char*))));
			//ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SetSetDataSignal(DWORD, DWORD, double*, DWORD, double*, const char*, const char*)), data, SLOT(SetSetData(DWORD, DWORD, double*, DWORD, double*, const char*, const char*))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendEventSignal(DWORD, DWORD, long)), data, SLOT(SendEvent(DWORD, DWORD, long))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendEventSignal2(DWORD, DWORD, long, DWORD, double, const char*)), data, SLOT(SendEvent2(DWORD, DWORD, long, DWORD, double, const char*))));

			//ConnectConnectors->push_back(connect(planesWork, SIGNAL(GetDataSignal(DWORD, DWORD, double*, const char*)), data, SLOT(GetData(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(GetDataSignalL(DWORD, DWORD, double*, const char*)), data, SLOT(GetDataL(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetDataSignal(DWORD, DWORD, double*, const char*)), data, SLOT(SetData(DWORD, DWORD, double*, const char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetDataSignalL(DWORD, DWORD, double*, const char*)), data, SLOT(SetDataL(DWORD, DWORD, double*, const char*))));
			//ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetGetDataSignal(DWORD, DWORD, DWORD, double*, const char*)), data, SLOT(SetGetData(DWORD, DWORD, DWORD, double*, const char*))));
			//ConnectConnectors->push_back(connect(planesWork, SIGNAL(SetSetDataSignal(DWORD, DWORD, double*, DWORD, double*, const char*, const char*)), data, SLOT(SetSetData(DWORD, DWORD, double*, DWORD, double*, const char*, const char*))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendEventSignal(DWORD, DWORD, long)), data, SLOT(SendEvent(DWORD, DWORD, long))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendEventSignal2(DWORD, DWORD, long, DWORD, double, const char*)), data, SLOT(SendEvent2(DWORD, DWORD, long, DWORD, double, const char*))));


			ConnectConnectors->push_back(connect(data, SIGNAL(GetDataChange(DWORD, DWORD)), this, SLOT(GetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetDataChange(DWORD, DWORD)), this, SLOT(SetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetGetDataChange(DWORD, DWORD)), this, SLOT(SetGetDataChange(DWORD, DWORD)), Qt::DirectConnection));

			//ConnectConnectors->push_back(connect(data, SIGNAL(GetDataChange(DWORD, DWORD)), cabinWork, SLOT(SetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetDataChange(DWORD, DWORD)), cabinWork, SLOT(SetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			//ConnectConnectors->push_back(connect(data, SIGNAL(SetGetDataChange(DWORD, DWORD, DWORD)), cabinWork, SLOT(SetGetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));
			//ConnectConnectors->push_back(connect(data, SIGNAL(SetSetDataChange(DWORD, DWORD, DWORD)), cabinWork, SLOT(SetSetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));

			ConnectConnectors->push_back(connect(data, SIGNAL(GetDataChange(DWORD, DWORD)), planesWork, SLOT(SetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetDataChange(DWORD, DWORD)), planesWork, SLOT(GetDataChange(DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetGetDataChange(DWORD, DWORD, DWORD)), planesWork, SLOT(SetGetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));
			ConnectConnectors->push_back(connect(data, SIGNAL(SetSetDataChange(DWORD, DWORD, DWORD)), planesWork, SLOT(SetSetDataChange(DWORD, DWORD, DWORD)), Qt::DirectConnection));


			

			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendText(QString, bool)), utils, SLOT(SetText(QString, bool))));
			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendLog(QString)), utils, SLOT(AddLog(QString))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendText(QString, bool)), utils, SLOT(SetText(QString, bool))));
			ConnectConnectors->push_back(connect(planesWork, SIGNAL(SendLog(QString)), utils, SLOT(AddLog(QString))));
			ConnectConnectors->push_back(connect(data, SIGNAL(SendText(QString, bool)), utils, SLOT(SetText(QString, bool))));
			ConnectConnectors->push_back(connect(data, SIGNAL(SendLog(QString)), utils, SLOT(AddLog(QString))));


			ConnectConnectors->push_back(connect(cabinWork, SIGNAL(SendCommand(DWORD, double, double)), planesWork, SLOT(ReceiveCommand(DWORD, double, double))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SendCommand(DWORD, double, double)), planesWork, SLOT(ReceiveCommand(DWORD, double, double))));
			ConnectConnectors->push_back(connect(this, SIGNAL(SendCommand(DWORD, double, double)), cabinWork, SLOT(ReceiveCommand(DWORD, double, double))));

			
			ConnectConnectors->push_back(connect(this, SIGNAL(CLPreliminaryCocpitPrep(int*)), cabinWork, SLOT(CLPreliminaryCocpitPrep(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLCocpitPreparation(int*)), cabinWork, SLOT(CLCocpitPreparation(int*)))); 
			ConnectConnectors->push_back(connect(this, SIGNAL(CLBeforeStart(int*)), cabinWork, SLOT(CLBeforeStart(int*)))); 
			ConnectConnectors->push_back(connect(this, SIGNAL(CLEngineStart(int*)), cabinWork, SLOT(CLEngineStart(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLAfterStart(int*)), cabinWork, SLOT(CLAfterStart(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLBeforeTaxi(int, int, int*)), cabinWork, SLOT(CLBeforeTaxi(int, int, int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLBeforeTakeoff(int*)), cabinWork, SLOT(CLBeforeTakeoff(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLAfterTakeoff(int*)), cabinWork, SLOT(CLAfterTakeoff(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLLanding(int*)), cabinWork, SLOT(CLLanding(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLAfterLanding(int*)), cabinWork, SLOT(CLAfterLanding(int*))));
			ConnectConnectors->push_back(connect(this, SIGNAL(CLParking(int*)), cabinWork, SLOT(CLParking(int*))));

			ConnectConnectors->push_back(connect(this, SIGNAL(CabinReport()), cabinWork, SLOT(CabinReport())));
			
			Quit = FALSE;
			Connected = TRUE;			
			Mode = START;
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

// SID/STAR Methods

void MainLogic::AddSidStarTrack(WayPointA* wayPoint, TPath* RWEnd, FIXX* fixx, DATA_RUNWAY* runway) {
	
	if (wayPoint->TypeName == "TRKUNTIL") {
		if (RWEnd == NULL) {
			AddWayPoint(runway->eLongitude, runway->eLatitude, wayPoint->ALT, "HEADINGUNTILALT", QString("(" + QString(std::to_string((DWORD)wayPoint->ALT).c_str()) + ")"), wayPoint->TRK, 0, 0.0, 0, wayPoint->ALT);
		}
		else {
			AddWayPoint(RWEnd->Lon, RWEnd->Lat, wayPoint->ALT, "HEADINGUNTILALT", QString("(" + QString(std::to_string((DWORD)wayPoint->ALT).c_str()) + ")"), wayPoint->TRK, 0, 0.0, 0, wayPoint->ALT);
		}
	}
	else if (wayPoint->TypeName == "TRKINTERCEPTRADIALTOFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TRKINTERCEPTRADIAL", QString(("(INTRAD) " + wayPoint->FIXName).c_str()), wayPoint->TRK, 0, 0.0, wayPoint->RADIAL, 0.0);
	}
	else if (wayPoint->TypeName == "FIXATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0,0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIXOVERFLYATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIXATORBELOW") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0,0.0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, wayPoint->SPEED, 0,0.0);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVE") {
		//
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0, wayPoint->ALT2);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVE") {
		//
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0, wayPoint->ALT2);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVESPEED") {
		//
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, wayPoint->SPEED, 0, wayPoint->ALT2);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIXATORBELOWATORABOVESPEED") {
		//
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, wayPoint->SPEED, 0, wayPoint->ALT2);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXATORBELOWATORABOVE") {
		//
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, 0.0, 0, wayPoint->ALT2);
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT2, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORBELOW, 0.0, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, 0.0, 0,0.0);
	}
	else if (wayPoint->TypeName == "FIXSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, wayPoint->SPEED, 0,0.0);
	}
	else if (wayPoint->TypeName == "FIXOVERFLY") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, 0.0, 0,0.0);
	}
	else if (wayPoint->TypeName == "FIXFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, 0.0, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIXFALTSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "FIXOVERFLYFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, 0.0, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "RNW") {
		AddWayPoint(runway->sLongitude, runway->sLatitude, runway->alt, "GSWAY", QString(runway->Name.c_str()), 0, FIXALT, 0, runway->Dev, runway->alt);
		AddWayPoint(runway->eLongitude, runway->eLatitude, runway->alt, "RUNWAY", QString(runway->Name.c_str()), 0, FIXALT, 0, runway->Dev, runway->alt);
	}
	else if (wayPoint->TypeName == "FIXOVERFLYATORABOVESPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, ALTORABOVE, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEEDLEGTIME") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "HOLDATFIXRIGHTTURNINBOUNDCOURSEALTSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, TYPE_PATHS[wayPoint->TYPE], QString(wayPoint->FIXName.c_str()), wayPoint->TRK, FIXALT, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0,0.0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0,0.0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXOVERFLY") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0,0.0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXOVERFLYFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXOVERFLYFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXOVERFLY") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, 0, wayPoint->SPEED, 0,0.0);
	}
	else if (wayPoint->TypeName == "TURNRIGHTDIRECTFIXFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNRIGHTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXFALT") {
		AddWayPoint(fixx->Lon, fixx->Lat, wayPoint->ALT, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, FIXALT, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "TURNLEFTDIRECTFIXATORABOVE") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "TURNLEFTDIRECTFIX", QString(wayPoint->FIXName.c_str()), 0, ALTORABOVE, wayPoint->SPEED, 0, wayPoint->ALT);
	}
	else if (wayPoint->TypeName == "TRKUNTILFROMFIXSPEED") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "HEADINGUNTILFROMDIST", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, wayPoint->SPEED, wayPoint->LegDist, 0.0);
	}
	else if (wayPoint->TypeName == "TRKUNTILFROMFIX") {
		AddWayPoint(fixx->Lon, fixx->Lat, 0.0, "HEADINGUNTILFROMDIST", QString(wayPoint->FIXName.c_str()), wayPoint->TRK, 0, 0.0, wayPoint->LegDist, 0.0);
	}
	else {
		qDebug() << QString(wayPoint->TypeName.c_str());
	}
	/*if (wayPoint->SPEED != 0) {
		Legs->at(Legs->size() - 1).IndSpeed = wayPoint->SPEED;
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
	std::vector<std::string> AppTypes = { "ILS", "GLS", "LOC", "NDB", "VOR", "RNV" };
	for (int i = 0; i < sidstar->APPROACHES->size(); i++) {
		for (int k = 0; k < AppTypes.size(); k++) {
			if (sidstar->APPROACHES->at(i).Name.find(AppTypes[k]) != std::string::npos) {
				sidstar->APPROACHES->at(i).Priority = k;
			}
		}
	}
	std::sort(sidstar->APPROACHES->begin(), sidstar->APPROACHES->end(),
		CompareByMember<Points, int>(&Points::Priority));

	for (j = 0; j < sidstar->APPROACHES->size(); j++) {
		for (r = 0; r < sidstar->APPROACHES->at(j).Runways->size(); r++) {
			if (sidstar->APPROACHES->at(j).Runways->at(r) == runway->Name) {
				if (QString(sidstar->APPROACHES->at(j).Legs->at(0).FIXName.c_str()) ==fix) {
					t = -1;
					FindAppr = true;
					break;
				}
				else {
					if (sidstar->APPROACHES->at(j).TRANSITIONS != NULL) {
						for (t = 0; t < sidstar->APPROACHES->at(j).TRANSITIONS->size(); t++) {
							if (QString(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->at(0).FIXName.c_str()) ==fix) {
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
			if (FindTransAppr) {
				for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->size(); ii++) {
					STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->at(ii));
					STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
				}
			}
			for (int ii = 0; ii < sidstar->APPROACHES->at(j).Legs->size(); ii++) {
				STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).Legs->at(ii));
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
					if (QString(sidstar->STARS->at(i).Legs->at(0).FIXName.c_str()) ==fix) {
						t2 = -1;
						for (j = 0; j < sidstar->APPROACHES->size(); j++) {
							for (r = 0; r < sidstar->APPROACHES->at(j).Runways->size(); r++) {
								if (sidstar->APPROACHES->at(j).Runways->at(r) == runway->Name) {
									std::string ss1 = sidstar->APPROACHES->at(j).Legs->at(0).FIXName;
									std::string ss2 = sidstar->STARS->at(i).Legs->at(sidstar->STARS->at(i).Legs->size() - 1).FIXName;
									if (sidstar->APPROACHES->at(j).Legs->at(0).FIXName ==
										sidstar->STARS->at(i).Legs->at(sidstar->STARS->at(i).Legs->size() - 1).FIXName) {
										t = -1;
										FindAppr = true;
										FindStar = true;
										break;
									}
									else {
										if (sidstar->APPROACHES->at(j).TRANSITIONS != NULL) {
											for (t = 0; t < sidstar->APPROACHES->at(j).TRANSITIONS->size(); t++) {
												if (sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->at(0).FIXName ==
													sidstar->STARS->at(i).Legs->at(sidstar->STARS->at(i).Legs->size() - 1).FIXName) {
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
								for (int ii = 0; ii < sidstar->STARS->at(i).Legs->size() - 1; ii++) {
									STAR.wayPoint->push_back(sidstar->STARS->at(i).Legs->at(ii));
									STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = FIX;
								}
								if (FindTransAppr) {
									for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->size(); ii++) {
										STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->at(ii));
										STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
									}
								}
								for (int ii = 0; ii < sidstar->APPROACHES->at(j).Legs->size(); ii++) {
									STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).Legs->at(ii));
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
								if (QString(sidstar->STARS->at(i).TRANSITIONS->at(t2).Legs->at(0).FIXName.c_str()) ==fix) {
									for (j1 = 0; j1 < sidstar->APPROACHES->size(); j1++) {
										for (r = 0; r < sidstar->APPROACHES->at(j1).Runways->size(); r++) {
											if (sidstar->APPROACHES->at(j1).Runways->at(r) == runway->Name) {
												if (sidstar->APPROACHES->at(j1).Legs->at(0).FIXName ==
													sidstar->STARS->at(i).Legs->at(sidstar->STARS->at(i).Legs->size() - 1).FIXName) {
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
															std::string ss1 = sidstar->APPROACHES->at(j1).TRANSITIONS->at(t).Legs->at(0).FIXName;
															std::string ss2 = sidstar->STARS->at(i).Legs->at(sidstar->STARS->at(i).Legs->size() - 1).FIXName;
															if (sidstar->APPROACHES->at(j1).TRANSITIONS != NULL)
															{
																if (sidstar->APPROACHES->at(j1).TRANSITIONS->at(t).Legs->at(0).FIXName ==
																	sidstar->STARS->at(i).Legs->at(sidstar->STARS->at(i).Legs->size() - 1).FIXName)
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
												for (int ii = 0; ii < sidstar->STARS->at(i).TRANSITIONS->at(t2).Legs->size(); ii++) {
													STAR.wayPoint->push_back(sidstar->STARS->at(i).TRANSITIONS->at(t2).Legs->at(ii));
													STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
												}
											}
											for (int ii = 0; ii < sidstar->STARS->at(i).Legs->size() - 1; ii++) {
												STAR.wayPoint->push_back(sidstar->STARS->at(i).Legs->at(ii));
												STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = FIX;
											}
											if (FindTransAppr) {
												for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->size(); ii++) {
													STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->at(ii));
													STAR.wayPoint->at(STAR.wayPoint->size() - 1).TYPE = WAYPOINT;
												}
											}
											for (int ii = 0; ii < sidstar->APPROACHES->at(j).Legs->size(); ii++) {
												STAR.wayPoint->push_back(sidstar->APPROACHES->at(j).Legs->at(ii));
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
			for (int ii = 0; ii < sidstar->STARS->at(i).TRANSITIONS->at(t2).Legs->size(); ii++) {
				wayPoint->push_back(sidstar->STARS->at(i).TRANSITIONS->at(t2).Legs->at(ii));
				wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
			}
		}
		for (int ii = 0; ii < sidstar->STARS->at(i).Legs->size() - 1; ii++) {
			wayPoint->push_back(sidstar->STARS->at(i).Legs->at(ii));
			wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
		}
		STARName = sidstar->STARS->at(i).Name;
	}
	if (FindAppr) {
		if (t >= 0) {
			for (int ii = 0; ii < sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->size(); ii++) {
				wayPoint->push_back(sidstar->APPROACHES->at(j).TRANSITIONS->at(t).Legs->at(ii));
				wayPoint->at(wayPoint->size() - 1).TYPE = WAYPOINT;
			}
		}
		AppPoint = wayPoint->size();
		for (int ii = 0; ii < sidstar->APPROACHES->at(j).Legs->size(); ii++) {
			wayPoint->push_back(sidstar->APPROACHES->at(j).Legs->at(ii));
			wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
		}
		STARName = STARName + " - " + sidstar->APPROACHES->at(j).Name;
	}
	else {*/
	/*/if (STARs->size() == 0) {
		SIMCONNECT_DATA_LATLONALT latlon = Utils::GetDALatLon(runway->sLatitude, runway->sLongitude, runway->eHeading, 18.0);
		AddWayPoint(latlon.Longitude, latlon.Latitude, Legs->at(Legs->size() - 1).EAltitudeHi + 2000.0, "FIX", "GlideAngle 18.0 " + QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		AddWayPoint(runway->sLongitude, runway->sLatitude, Legs->at(Legs->size() - 1).EAltitudeHi + 2000.0, "WAYPOINT", "FINAL " + QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		AddWayPoint(runway->sLongitude, runway->sLatitude, Legs->at(Legs->size() - 1).EAltitudeHi, "GSWAY", QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		AddWayPoint(runway->eLongitude, runway->eLatitude, Legs->at(Legs->size() - 1).EAltitudeHi, "RUNWAY", QString(runway->Name.c_str()), 0, FIXALT, 0.0, 0);
		STARName = "GlideAngle 18.0 ";
	}*/
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
			AppPoint = Legs->size()-1;
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
				if (QString(sidstar->SIDS->at(j).Legs->at(sidstar->SIDS->at(j).Legs->size() - 1).FIXName.c_str()) == fix) {
					t = -1;
					FindSid = true;
					break;
				}
				else {
					if (sidstar->SIDS->at(j).TRANSITIONS != NULL) {
						for (t = 0; t < sidstar->SIDS->at(j).TRANSITIONS->size(); t++) {
							if (QString(sidstar->SIDS->at(j).TRANSITIONS->at(t).Legs->at(sidstar->SIDS->at(j).TRANSITIONS->at(t).Legs->size() - 1).FIXName.c_str()) == fix) {
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
			for (int i = 0; i < sidstar->SIDS->at(j).Legs->size(); i++) {
				wayPoint->push_back(sidstar->SIDS->at(j).Legs->at(i));
				wayPoint->at(wayPoint->size() - 1).TYPE = FIX;
			}
			if (t >= 0) {
				for (int i = 0; i < sidstar->SIDS->at(j).TRANSITIONS->at(t).Legs->size(); i++) {
					wayPoint->push_back(sidstar->SIDS->at(j).TRANSITIONS->at(t).Legs->at(i));
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
	return SIDss;
}

// Autoflight Methods

double MainLogic::AltPitchWithPos(double TargetValue) {
	double pitch = -data->GData.PLANE_PITCH_DEGREES;
	alterPid lp = PPID;
	if (lp.FT) {
		lp.ePnm1 = 0.0;
		lp.eDfnm2 = lp.eDfnm1 = 0.0;
		lp.Unm1 = data->GData.ELEVATOR_POSITION;
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

	double EndHead = Legs->at(CurrentWayIndex).EndHeadingTrue;
	double EangleRel = Utils::Constrain180(CurrentLeg.EndHeadingTrue - EndHead);
	double a = Utils::GetFixDA(sin(EangleRel * M_PI / 180) * CurrentLeg.Distance, EangleRel);
	double HeadingRel;
	EangleRel = a * 1000;
	EangleRel = Utils::AngleLimitS(EangleRel, 60);
	double EangleRel2 = EangleRel;
	EangleRel = Utils::Constrain180(EndHead + EangleRel + 180.0);
	CHAR s[256];
	
	HeadingRel = RudWithHead(EangleRel);
	sprintf(s, "EndHead: %.3f CurrentLeg.EH: %.3f EangleRel1: %.3f a: %.3f HeadingRel: %.3f\n EangleRel: %.3f\n CurrAngle: %.3f", EndHead, CurrentLeg.EndHeadingTrue, Utils::Constrain180(CurrentLeg.EndHeadingTrue - EndHead), a, HeadingRel, EangleRel, Utils::Constrain180(data->GData.PLANE_HEADING_DEGREES_TRUE));
	SendText(QString(s), false);
	return HeadingRel;
}

double MainLogic::ManHeadWithWay(sWayPoint* Way) {
	double EndHead;
	if (CurrentWayIndex > 1750) {
		CurrentWayIndex = 98;
	}
	if ((CurrentLeg.Type == TYPE_PATHS[FIXHEADING])) {
		EndHead = Legs->at(CurrentWayIndex).EndHeadingTrue + data->GData.MAGVAR;
	}
	else if (CurrentLeg.Type == TYPE_PATHS[FIX]) {
		EndHead = CurrentLeg.EndHeadingTrue;
	}
	else if (CurrentLeg.Type == TYPE_PATHS[TURNLEFTDIRECTFIX]) {
		EndHead = CurrentLeg.EndHeadingTrue;
	}
	else if (CurrentLeg.Type == TYPE_PATHS[TURNRIGHTDIRECTFIX]) {
		EndHead = CurrentLeg.EndHeadingTrue;
	}
	else {
		EndHead = Legs->at(CurrentWayIndex).EndHeadingTrue;
	}
	double EangleRel = Utils::Constrain180(CurrentLeg.EndHeadingTrue - EndHead);
	double a = Utils::GetFixDA(sin(EangleRel * M_PI / 180) * CurrentLeg.Distance, EangleRel);
	double HeadingRel;
	if ((data->GData.SIM_ON_GROUND== 1)) {
		EangleRel = a * 500; //500
		if (data->GData.GROUND_VELOCITY < 10) {
			EangleRel = a * 2000; //2000
		}
		if (data->GData.GROUND_VELOCITY < 7) {
			EangleRel = a * 3000; //6000
		}
		if (data->GData.GROUND_VELOCITY < 5) {
			EangleRel = a * 3000; //20000
		}
		if (data->GData.GROUND_VELOCITY < 2) {
			EangleRel = a * 6000;
		}
		
		
		
		double EangleRel1 = EangleRel;
		EangleRel = Utils::AngleLimitS(EangleRel, 40);
		EangleRel = Utils::Constrain180(EndHead + EangleRel);
		HeadingRel = RudWithHead(EangleRel);
	}
	else {
		if ((CurrentLeg.Type == TYPE_PATHS[GSWAY]) || (CurrentLeg.Type == TYPE_PATHS[RUNWAY])) {
			EangleRel = a * 500 / CurrentLeg.Distance;
		}
		else {
			EangleRel = a * 10;
		}
		double EangleRel1 = EangleRel;
		EangleRel = Utils::AngleLimitS(EangleRel, 60);
		double AngleWind = data->GData.AMBIENT_WIND_DIRECTION - CurrentLeg.HeadingTrue;
		double AngleDrift = asin(sin(AngleWind / 180 * M_PI) * data->GData.AMBIENT_WIND_VELOCITY / data->GData.AIRSPEED_TRUE)* 180 / M_PI;
		EangleRel = Utils::Constrain180(EndHead + EangleRel + AngleDrift);
		int EangleRel2 = Utils::Constrain180(EangleRel - data->GData.MAGVAR);
		//SendCommand(HDG_SEL, EangleRel2, 0);
		HeadingRel = BankWithHead(EangleRel);
	}
	return HeadingRel;
}

double MainLogic::GetAngleFront(double frontOut, bool nose = false) {
	double D = CurrentLeg.Distance;
	int way = CurrentWayIndex;
	SIMCONNECT_DATA_LATLONALT noselatlon;
	SIMCONNECT_DATA_LATLONALT latlon;
	double dDist;
	while (D < frontOut) {
		if (way >= Legs->size()) {
			break;
		}
		D += Legs->at(way).Distance;
		way++;
	}
	if (way != CurrentWayIndex) {
		way--;
		D -= Legs->at(way).Distance;
		dDist = frontOut - D;

		latlon = Utils::GetDALatLon(Legs->at(way).Lat, Legs->at(way).Lon, Legs->at(way).EndHeadingTrue, dDist);
	}
	else {
		D -= CurrentLeg.Distance;
		dDist = frontOut - D;

		latlon = Utils::GetDALatLon(CurrentLeg.Lat, CurrentLeg.Lon, CurrentLeg.EndHeadingTrue, dDist);
	}
	if (nose) {

	}
	noselatlon = Utils::GetDALatLon(CurrentLeg.Lat, CurrentLeg.Lon, data->GData.PLANE_HEADING_DEGREES_TRUE, 0.0084);
	sWayPoint front = CurrentLeg;
	front.ELat = latlon.Latitude;
	front.ELon = latlon.Longitude;
	Utils::DOrtoKM(&front);

	double angleFront = Utils::Constrain180(front.HeadingTrue - data->GData.PLANE_HEADING_DEGREES_TRUE);
	return angleFront;
}

double MainLogic::PID(double dt, double err, double kp, double ki, double kd, double bi, double bd, double* prevErr, double* integral) {

	double P = err * kp;
	*integral = Utils::Constrain(*integral + (err * dt * ki), -bi, bi);
	double I = *integral;
	double D = Utils::Constrain(((err - *prevErr) / dt) * kd, -bd, bd);
	*prevErr = err;
	return P + I + D;
}

double MainLogic::RudWithHead(double Heading) {
	double VelV = data->GData.ROTATION_VELOCITY_BODY_Y;
	VelV = VelV / 1;
	
	static double sHed = data->GData.PLANE_HEADING_DEGREES_TRUE;
	double eHed = data->GData.PLANE_HEADING_DEGREES_TRUE;
	double HeadingRel = Utils::Constrain180(Heading - data->GData.PLANE_HEADING_DEGREES_TRUE);

	static double prevErr = 0;
	static double integral = 0;
	static QDateTime Start = QDateTime::currentDateTime();
	double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
	Start = QDateTime::currentDateTime();
	//computePID(dt, HeadingRel, 0.15, 0.001, 0.001, 1.0, 1.0, &prevErr, &integral);

	double parameter = HeadingRel / 4; //-45.0 - +45.0
	double rud = data->GData.RUDDER_PEDAL_POSITION * 16383;
	double rudPID = 0;
	int intParameter;
	if (!data->GData.SIM_ON_GROUND) {
		integral = 0;
		//rudPID = 0;
	}
	else {
		if (data->GData.GROUND_VELOCITY < 0.5) {
			rudPID = 0;
		}
		else if (data->GData.GROUND_VELOCITY < 3) {
			rudPID = PID(dt, HeadingRel, data->AllData.AI_RUDDER_P / (data->GData.GROUND_VELOCITY / 4), data->AllData.AI_RUDDER_I, data->AllData.AI_RUDDER_D, data->AllData.AI_RUDDER_IB, data->AllData.AI_RUDDER_ID, &prevErr, &integral) * data->AllData.AI_RUDDER_K; //0.1,0.001,0.05,1.0,1.0
		}
		else {
			rudPID = PID(dt, HeadingRel, data->AllData.AI_RUDDER_P / 2, data->AllData.AI_RUDDER_I, data->AllData.AI_RUDDER_D, data->AllData.AI_RUDDER_IB, data->AllData.AI_RUDDER_ID, &prevErr, &integral) * data->AllData.AI_RUDDER_K; //0.1,0.001,0.05,1.0,1.0
		}
	}
	if (data->GData.GROUND_VELOCITY > 5) {

		if (data->GData.AIRSPEED_INDICATED > 60) {
			rud = rud - (100 * (VelV - parameter) * (data->GData.GROUND_VELOCITY / 5)); //50
			//rud = computePID(dt, HeadingRel, 0.15, 0.001, 0.001, 1.0, 1.0, &prevErr, &integral) * 16383;
			//intParameter = -Utils::Constrain(rud, -3000, 3000);
			intParameter = -Utils::Constrain(rud, -16383, 16383);
		}
		else {
			if (data->GData.AIRSPEED_INDICATED > 20) {
				rud = rud - (100 * (VelV - parameter) * (data->GData.GROUND_VELOCITY / 2));
				//rud = computePID(dt, HeadingRel, 0.15, 0.001, 0.001, 1.0, 1.0, &prevErr, &integral) * 16383;
				//intParameter = -Utils::Constrain(rud, -13100, 13100);
				intParameter = -Utils::Constrain(rud, -16383, 16383);
			} 
			else {
				rud = rud - (100 * (VelV - parameter) * (data->GData.GROUND_VELOCITY / 2));
				//rud = computePID(dt, HeadingRel, 0.0, 0.0, 0.1, 1.0, 1.0, &prevErr, &integral) * 16383;
				//intParameter = -Utils::Constrain(rud, -15290, 15290);
				intParameter = -Utils::Constrain(rud, -16383, 16383);
			}
		}
	}
	else {
		rud = rud - (500 * (VelV - parameter) * (data->GData.GROUND_VELOCITY / 2));
		//rud = computePID(dt, HeadingRel, 0.15, 0.001, 0.001, 1.0, 1.0, &prevErr, &integral)*16383;
		intParameter = -Utils::Constrain(rud, -16383, 16383);
	}
	if (Mode == TAKEOFF || Mode == LANDING) {
		intParameter = -Utils::Constrain(rud, -16383, 16383);
	}
	else {
		//rudPID = 0;
		intParameter = -Utils::Constrain(rudPID, -16383, 16383);
	}
	if (Mode == PUSHBACK || Mode == STARTPUSHBACK) {
		intParameter = -intParameter;
		SendEvent(KEY_TUG_HEADING, (0xFFFFFFFF / 360) * Heading);
	}
	SendCommand(SET_RUDDER, intParameter, 0);

	sHed = data->GData.PLANE_HEADING_DEGREES_TRUE;
	return HeadingRel;
}

double MainLogic::CalcVelocity(double last, double current, double dt) {
	return (1000 / dt) * (current - last);
}

double MainLogic::LimitVal(double val, double min, double max = 8518) {
	if (max == 8518) {
		if (val < -min) {
			val = -min;
		}
		else if (val > min) {
			val = min;
		}
	}
	else {
		if (val < min) {
			val = min;
		}
		else if (val > max) {
			val = max;
		}
	}
	return val;
}

void MainLogic::ManPitchWithFD(double RequiredPitch) {
	//if (!data->AllData.A32NX_AUTOPILOT_ACTIVE) {
		//static double StickYPos = data->GData.YOKE_Y_POSITION;

	static double prevErr = 0;
	static double integral = 0;

	double Coefficient = 15;
	if (Mode == TAKEOFF) {
		Coefficient = 70;
	}
	if (Mode == CRUISE) {
		Coefficient = 30;
	}
	if (landing != 0) {
		Coefficient = 50;
	}
	if ((Mode == DESCENT) && ((data->GData.PLANE_ALT_ABOVE_GROUND - 9) < 100) && (data->GData.SIM_ON_GROUND == 0)) {
		Coefficient = 70;
	} 
	
	static double StickYPos = data->AllData.A32NX_SIDESTICK_POSITION_Y * 16383;
	static int countFails = 5;
	if (countFails >= 0) { //5
		countFails = 0;
		SetDataPitch = true;
	}

	/*if (round(StickYPos * 1000000) == round(StickYPos * 1000000)) {
		SetDataPitch = true;
	}*/
	if (SetDataPitch) {
		static clock_t sTime = 0; //Start time
		clock_t cTime = clock(); //Current time
		static double LastPitch = 0;
		static double LastRequiredPich = 0;
		double CurrentPitch = data->GData.PLANE_PITCH_DEGREES;
		//double RequiredPitch_L = RequiredPitch;
		double DeltaTime = cTime - sTime; //Delta time from last iterration
		
		//ui->delta->setText(QString::number(DeltaTime, 'f', 3));
		//Safe NaN
		if (DeltaTime == 0) {
			DeltaTime = 0.000001;
		}
		double dt = DeltaTime;
		double PichVelocity = CalcVelocity(LastPitch, CurrentPitch, DeltaTime);
		double RequiredPitchVelocity = CalcVelocity(LastRequiredPich, RequiredPitch, DeltaTime);

		double PitchRelative = RequiredPitch - CurrentPitch; //Relative pitch

		PitchRelative = Utils::Constrain180(PitchRelative);

		double NeededChangeStickYPos = ((PitchRelative / 10 + RequiredPitchVelocity - PichVelocity) * 500);

		NeededChangeStickYPos = LimitVal(NeededChangeStickYPos, Coefficient);

		double NewStickYPos = StickYPos + NeededChangeStickYPos;

		//Test PID
		//NewStickYPos = PID(dt, PitchRelative, 1000.0, 0.1, 5.0, 100, 100.0, &prevErr, &integral); //0.1,0.001,0.05,1.0,1.0
		NewStickYPos = PID(dt, PitchRelative, data->AllData.AI_PITCH_P, data->AllData.AI_PITCH_I, data->AllData.AI_PITCH_D, data->AllData.AI_PITCH_IB, data->AllData.AI_PITCH_ID, &prevErr, &integral) * data->AllData.AI_PITCH_K; //0.1,0.001,0.05,1.0,1.0
		//////////

		NewStickYPos = LimitVal(NewStickYPos, 16383);

		if (!data->AllData.A32NX_AUTOPILOT_ACTIVE) {
			SendCommand(SET_ELEVATOR, NewStickYPos, 0);
		}
		else {
			SendCommand(SET_ELEVATOR, 0, 0);
			StickYPos = 0;
			NewStickYPos = 0;
		}
		sTime = clock();
		StickYPos = NewStickYPos;
		LastPitch = CurrentPitch;
		LastRequiredPich = RequiredPitch;
		SetDataPitch = true;
	}
	else {
		countFails++;
	}
	//}
	//else {
	//	SendCommand(SET_ELEVATOR, 0, 0);
	//}
}

void MainLogic::ManBankWithFD(double NNBank) {
	
		double NBank = NNBank;
		if (NBank > 30) {
			NBank = 30;
		}
		else if (NBank < -30) {
			NBank = -30;
		}
		static double NewBank = data->AllData.A32NX_SIDESTICK_POSITION_X * 16383;
		static int countFails = 0;
		//SendText("ailer: " + QString::number(data->GData.YOKE_X_POSITION, 'f', 3) + "\n" + "NewBank: " + QString::number(NewBank, 'f', 3) + "\n", false);
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
			double eBank = data->GData.PLANE_BANK_DEGREES;
			double eVertBar = NNBank; //BAR
			double timeOff = eTime - sTime;
			double bankOff = eBank - sBank;
			double VertBarOff = eVertBar - sVertBar; //BAR
			double bankA = (1000 / timeOff) * bankOff;
			double VertBarA = (1000 / timeOff) * VertBarOff; //BAR		
			double BankRel = NBank - /*(FlightDirectorBank / 15) - */eBank;
			if (BankRel > 180) {
				BankRel = BankRel - 360;
			}
			else if (BankRel < -180) {
				BankRel = BankRel + 360;
			}
			double pparameter = -(BankRel / 1);// -VertBarA;
			double ailer = NewBank * 16383;
			/*SendText("ailer: " + QString::number(ailer, 'f', 3) + "\n" +
				"NewBank: " + QString::number(FlightDirectorBank, 'f', 3) + "\n" +
				"Bank: " + QString::number(eBank, 'f', 3) + "\n"
				, false);*/
			double rel = ((pparameter - bankA) * 500);
			if (rel < -10) {
				rel = -10;
			}
			else if (rel > 10) {
				rel = 10;
			}
			ailer = ailer + rel;
			if (ailer > (16383 / 4.16666666666666667)) {
				ailer = 16383 / 4.16666666666666667;
			}
			else if (ailer < (-16383 / 4.16666666666666667)) {
				ailer = -16383 / 4.16666666666666667;
			}
			double intpParameter = ailer / 16383;// / 0.40000001;
			if (!data->AllData.A32NX_AUTOPILOT_ACTIVE) {
				SendCommand(SET_AILERON, intpParameter * 16383, 0);
			}
			else {
				SendCommand(SET_AILERON, 0, 0);
			}
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
	static double sHed = data->GData.PLANE_HEADING_DEGREES_TRUE;
	double eHed = data->GData.PLANE_HEADING_DEGREES_TRUE;
	double aHed = (eHed - sHed) / (double)((double)(eTime - sTime) / 1000);
	double HeadingRel = Heading - eHed;
	if (HeadingRel > 180) {
		HeadingRel = HeadingRel - 360;
	}
	else if (HeadingRel < -180) {
		HeadingRel = HeadingRel + 360;
	}
	double parameter = HeadingRel / 4;
	double rud = data->GData.PLANE_BANK_DEGREES;
	rud = rud - (5 * (data->GData.DELTA_HEADING_RATE - parameter));
	if ((HeadingRel > 60) || (HeadingRel < -60)) {
		if (CurrentLeg.Type == "TURNRIGHTDIRECTFIX") {
			HeadingRel = Utils::GetFixDA(HeadingRel, 1);
		}
		else if (CurrentLeg.Type == "TURNLEFTDIRECTFIX") {
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
	sHed = data->GData.PLANE_HEADING_DEGREES_TRUE;
	return HeadingRel;
}

double MainLogic::AltBankWithPos(double TargetValue) {
	double pTerm = 0, dTerm = 0, iTerm = 0;
	double error = TargetValue - data->GData.PLANE_BANK_DEGREES;
	pTerm = pidBank.pGain * error;    // calculate the proportional term
	pidBank.iState += error;          // calculate the integral state with appropriate limiting
	if (pidBank.iState > pidBank.iMax) {
		pidBank.iState = pidBank.iMax;
	}
	else if (pidBank.iState < pidBank.iMin) {
		pidBank.iState = pidBank.iMin;
	}
	iTerm = pidBank.iGain * pidBank.iState;    // calculate the integral term
	dTerm = pidBank.dGain * (data->GData.PLANE_BANK_DEGREES - pidBank.dState);
	pidBank.dState = data->GData.PLANE_BANK_DEGREES;
	double ailer = pTerm + iTerm - dTerm;
	if (ailer > (16383 / 4.16666666666666667)) {
		ailer = 16383 / 4.16666666666666667;
	}
	else if (ailer < (-16383 / 4.16666666666666667)) {
		ailer = -16383 / 4.16666666666666667;
	}
	double intpParameter = -ailer / 16383.0;
	//SimConnect_SetDataOnSimObject(hSimConnect, DEF_AILERON, 0, 0, 0, sizeof(intpParameter), &intpParameter);
	if (!data->AllData.A32NX_AUTOPILOT_ACTIVE) {
		SendCommand(SET_AILERON, -intpParameter * 16383, 0);
	}
	else {
		SendCommand(SET_AILERON, 0, 0);
	}
	pidBank.Term = ailer / 16383.0 * 30.0 * 4.16666666666666667;
	pidBank.val = data->GData.PLANE_BANK_DEGREES;
	pidBank.inVal = TargetValue;
	return (pTerm + iTerm - dTerm);
}

// Table Methods

QTableViewModel::QTableViewModel(QObject* parrent)
	:QAbstractListModel(parrent)
{
	values = new std::vector<sWayPoint>();
}

int QTableViewModel::rowCount(const QModelIndex&) const
{
	return values->size();
}

int QTableViewModel::columnCount(const QModelIndex& parrent) const
{
	return 9;
}

QVariant QTableViewModel::data(const QModelIndex& index, int role) const
{
	QVariant value;
	switch (role) {
	case Qt::DisplayRole: { //string 
		switch (index.column()) {
		case 0: {
			value = this->values->at(index.row()).Ident;
			break;
		}
		case 1: {
			value = this->values->at(index.row()).Name;
			break;
		}
		case 2: {
			value = this->values->at(index.row()).EAltitudeLo;
			break;
		}
		case 3: {
			value = this->values->at(index.row()).EAltitudeHi;
			break;
		}
		case 4: {
			value = this->values->at(index.row()).IndSpeed;
			break;
		}
		case 5: {
			value = this->values->at(index.row()).Distance;
			break;
		}
		case 6: {
			value = this->values->at(index.row()).RealDistance;
			break;
		}
		case 8: {
			value = this->values->at(index.row()).Type;
			break;
		}
		case 7: {
			value = this->values->at(index.row()).CommonDistance;
			break;
		}
		}
		break;
	}
	case Qt::UserRole: { //data
		value = this->values->at(index.row()).Ident;
		break;
	}
	default:
		break;
	}
	return value;;
}

void QTableViewModel::populate(std::vector<sWayPoint>* newValues)
{
	if (values->size() > 0) {
		this->beginRemoveRows(QModelIndex(), 0, this->values->size()-1);
		this->endRemoveRows();
	}
	if ((newValues != NULL) && (newValues->size() > 0)) {
		int idx = newValues->size() - 1;
		this->beginInsertRows(QModelIndex(), 0, idx);
		this->values = newValues;
		endInsertRows();
	}
}
