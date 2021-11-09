#include "Utils.h"


Utils::Utils(MSFS2020_AutoFlight* mainLogic, HANDLE* hsimConnect, QObject* parent) : QObject(parent) {
	this->mainLogic = mainLogic;
	this->HSimConnect = hsimConnect;
	connect(this, SIGNAL(SendText(QString)), ((MSFS2020_AutoFlight*)mainLogic)->GetUi()->label, SLOT(setText(QString)));
	connect(this, SIGNAL(SendLog(QString)), mainLogic, SLOT(AppendListView(QString)));
}

void Utils::SetText(QString s, bool simDisplay)
{
	QString metar = "Test metar";
		if (simDisplay) {
			QString sss = s + "\n" + metar;
			std::string my_qstring = sss.toStdString();
			char* ctext = (char*)my_qstring.c_str();
			SimConnect_Text(*HSimConnect, SIMCONNECT_TEXT_TYPE_PRINT_RED, 1, EVENT_TEXT, sss.length() + 1, ctext);
		}
	emit SendText(s);
	return;
}

double Utils::DegToRad(double D)
{
	if (0 > D || D > 360)
	{
		// Придумайте что делать в случае ошибки
	}
	return D / 180 * 3.14;
}

void Utils::AddLog(QString s)
{
	emit SendLog(QDateTime::currentDateTime().toString("dd.MM.yyyyThh:mm:ss.zzz") + " - " + s);
	return;
}

double Utils::GetPathLength(std::vector<TPath>* p) {
	double D = 0;
	for (int i = 1; i < p->size() - 1; i++) {
		sWayPoint dsheh = { 0 };
		dsheh.Lat = p->at(i - 1).Lat;
		dsheh.Lon = p->at(i - 1).Lon;
		dsheh.Altitude = 0;
		dsheh.ELat = p->at(i).Lat;
		dsheh.ELon = p->at(i).Lon;
		dsheh.EAltitude = 0;
		DOrtoKM(&dsheh);
		D = D + dsheh.Distance;
	}
	return D;
}

double Utils::GetRWAngle(std::vector<TPath>* p) {
	double D = 0;


	sWayPoint dsheh = { 0 };
	dsheh.Lat = p->at(p->size() - 2).Lat;
	dsheh.Lon = p->at(p->size() - 2).Lon;
	dsheh.Altitude = 0;
	dsheh.ELat = p->at(p->size() - 1).Lat;
	dsheh.ELon = p->at(p->size() - 1).Lon;
	dsheh.EAltitude = 0;
	DOrtoKM(&dsheh);
	D = dsheh.HeadingTrue;

	return D;
}
double Utils::GetRWLength(std::vector<TPath>* p) {
	double D = 0;
	sWayPoint dsheh = { 0 };
	dsheh.Lat = p->at(p->size() - 2).Lat;
	dsheh.Lon = p->at(p->size() - 2).Lon;
	dsheh.Altitude = 0;
	dsheh.ELat = p->at(p->size() - 1).Lat;
	dsheh.ELon = p->at(p->size() - 1).Lon;
	dsheh.EAltitude = 0;
	DOrtoKM(&dsheh);
	D = dsheh.Distance;
	return D;
}



SIMCONNECT_DATA_LATLONALT Utils::GetDALatLon(double lat, double lon, double ang, double dist) {
	dist = dist / 111.12;
	SIMCONNECT_DATA_LATLONALT latlon;
	double newlat = sin(lat * M_PI / 180) * cos(dist * M_PI / 180) + cos(lat * M_PI / 180) * sin(dist * M_PI / 180) * cos(ang * M_PI / 180);
	newlat = asin(newlat) * 180 / M_PI;
	double newlon = sin(dist * M_PI / 180) * sin(ang * M_PI / 180) / (cos(lat * M_PI / 180) * cos(dist * M_PI / 180) - sin(lat * M_PI / 180) * sin(dist * M_PI / 180) * cos(ang * M_PI / 180));
	newlon = lon + ((atan(newlon) * 180 / M_PI));
	latlon.Latitude = newlat;
	latlon.Longitude = newlon;
	return latlon;
}

void Utils::DOrtoKM(sWayPoint* dsheh) {
	double SLatitude = DegToRad(dsheh->Lat);
	double SLongitude = DegToRad(dsheh->Lon);
	double ELatitude = DegToRad(dsheh->ELat);
	double ELongitude = DegToRad(dsheh->ELon);
	double dO = (acos(sin(SLatitude) * sin(ELatitude) + cos(SLatitude) * cos(ELatitude) * cos(ELongitude - SLongitude))) * 180 / M_PI * 111.12;
	if ((SLatitude == ELatitude) && (SLongitude == ELongitude)) {
		dsheh->Distance = 0;
	}
	else {
		dsheh->Distance = dO;
	}
	dsheh->HeadingTrue = M_PI_2 - atan(cos(SLatitude) * tan(ELatitude) / sin(ELongitude - SLongitude) - sin(SLatitude) / tan(ELongitude - SLongitude));
	dsheh->EndHeadingTrue = M_PI_2 - atan(sin(ELatitude) / tan(ELongitude - SLongitude) - cos(ELatitude) * tan(SLatitude) / sin(ELongitude - SLongitude));
	dsheh->HeadingTrue = dsheh->HeadingTrue * 180 / M_PI;
	dsheh->EndHeadingTrue = dsheh->EndHeadingTrue * 180 / M_PI;
	if ((dsheh->ELon - dsheh->Lon) == 0) {
		dsheh->HeadingTrue = 0;
		dsheh->EndHeadingTrue = 0;
	}
	if (((dsheh->ELon - dsheh->Lon) < 0)) {
		dsheh->HeadingTrue = dsheh->HeadingTrue + 180;
		dsheh->EndHeadingTrue = dsheh->EndHeadingTrue + 180;
	}
	else if (((dsheh->ELon - dsheh->Lat) == 0) && ((dsheh->ELon - dsheh->Lon) < 0)) {
		dsheh->HeadingTrue = dsheh->HeadingTrue + 180;
		dsheh->EndHeadingTrue = dsheh->EndHeadingTrue + 180;
	}
	else if (((dsheh->ELon - dsheh->Lon) == 0) && ((dsheh->ELon - dsheh->Lat) < 0)) {
		dsheh->HeadingTrue = dsheh->HeadingTrue + 180;
		dsheh->EndHeadingTrue = dsheh->EndHeadingTrue + 180;
	}
	if (dsheh->HeadingTrue > 360) {
		dsheh->HeadingTrue = dsheh->HeadingTrue - 360;
	}
	if (dsheh->HeadingTrue < -360) {
		dsheh->HeadingTrue = dsheh->HeadingTrue + 360;
	}
	if (dsheh->EndHeadingTrue > 360) {
		dsheh->EndHeadingTrue = dsheh->EndHeadingTrue - 360;
	}
	if (dsheh->EndHeadingTrue < -360) {
		dsheh->EndHeadingTrue = dsheh->EndHeadingTrue + 360;
	}
}

double Utils::DToType(DWORD currWay, std::vector<sWayPoint>* wayPoints, QString type, sWayPoint* currPos, double headingRel) {
	double dist = currPos->Distance;
	while ((wayPoints->at(currWay).Type != type) && (currWay < (wayPoints->size() - 1))) {
		dist += wayPoints->at(currWay + 1).Distance;
		currWay++;
	}
	if ((currPos->Type == "GATE") && (abs(headingRel) > 160)) {
		dist = -dist;
	}
	return dist;
}

double Utils::DToRunway(DWORD currWay, std::vector<sWayPoint>* wayPoints, sWayPoint* currPos) {
	double dist = currPos->Distance;
	while ((wayPoints->at(currWay + 1).Type != "RUNWAY") && ((currWay + 1) < (wayPoints->size() - 1))) {
		dist += wayPoints->at(currWay + 1).Distance;
		currWay++;
	}
	return dist;
}

double Utils::Constrain180(double angle) {
	angle = Constrain360(angle + 180.0);
		return angle - 180.0;
}

double Utils::Constrain(double val, double min, double max) {
	if (val > max) {
		return max;
	}
	else if (val < min) {
		return min;
	}
	return val;
}

double Utils::Constrain360(double angle) {
	angle = fmod(angle, 360.0);
	while (angle < 0.0) {
		angle += 360.0;
	}
	return angle;
}

double Utils::GetFixDA(double da, double angle) {
	if (((angle < 0) && (da > 0)) || ((angle > 0) && (da < 0))) {
		return -da;
	}
	return da;
}

double Utils::AngleLimitS(double angle, double limit) {
	if (angle > limit) {
		return limit;
	}
	else if (angle < -limit) {
		return -limit;
	}
	return angle;
}
double Utils::RunwayDictCalc(std::vector<sWayPoint>* runwayPaths, sWayPoint* CurrentPos) {
	sWayPoint Runway;
	double Dist = 99;
	Runway.Lat = CurrentPos->Lat;
	Runway.Lon = CurrentPos->Lon;
	for (int i = 0; i < runwayPaths->size(); i++) {
		Runway.ELat = runwayPaths->at(i).ELat;
		Runway.ELon = runwayPaths->at(i).ELon;
		DOrtoKM(&Runway);
		double RunwayRel = Constrain180(Runway.EndHeadingTrue - runwayPaths->at(i).EndHeadingTrue);
		double RunwayDist = abs(GetFixDA(sin(RunwayRel * M_PI / 180) * Runway.Distance, RunwayRel));
		if (RunwayDist < Dist) {
			Dist = RunwayDist;
		}
	}
	if (Dist == 99) {
		Dist = 0;
	}
	return Dist;
}



/*void Utils::DrawAirport(IAirportData* PAirportData, double Lat, double Lon, double Hed) {
	if (aDC != NULL) {
		RECT rc;
		HWND PDraw = (HWND)MainForm::mainForm->AirportImage->Handle.ToPointer();
		GetClientRect(PDraw, &rc);
		MainForm::mainForm->AirportImage->Refresh();
		SelectObject(aDC, hPenSolid2Grey);
		double MinLat = 360.0, MinLon = 360.0, MaxLat = -360.0, MaxLon = -360.0;
		IAirport* Air = PAirportData->GetAirport();
		for (int i = 0; i < Air->PTaxiwayPoints->size(); i++) {
			if (SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(i).Lat) < MinLat) {
				MinLat = SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(i).Lat);
			}
			if (SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(i).Lat) > MaxLat) {
				MaxLat = SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(i).Lat);
			}
			if (SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(i).Lon) < MinLon) {
				MinLon = SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(i).Lon);
			}
			if (SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(i).Lon) > MaxLon) {
				MaxLon = SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(i).Lon);
			}
		}
		for (int i = 0; i < Air->PTaxiwayParks->size(); i++) {
			if (SIMMATH::DecodeLat(Air->PTaxiwayParks->at(i).Lat) < MinLat) {
				MinLat = SIMMATH::DecodeLat(Air->PTaxiwayParks->at(i).Lat);
			}
			if (SIMMATH::DecodeLat(Air->PTaxiwayParks->at(i).Lat) > MaxLat) {
				MaxLat = SIMMATH::DecodeLat(Air->PTaxiwayParks->at(i).Lat);
			}
			if (SIMMATH::DecodeLon(Air->PTaxiwayParks->at(i).Lon) < MinLon) {
				MinLon = SIMMATH::DecodeLon(Air->PTaxiwayParks->at(i).Lon);
			}
			if (SIMMATH::DecodeLon(Air->PTaxiwayParks->at(i).Lon) > MaxLon) {
				MaxLon = SIMMATH::DecodeLon(Air->PTaxiwayParks->at(i).Lon);
			}
		}
		double dLat = MaxLat - MinLat;
		double dLon = MaxLon - MinLon;
		double rcRate = (rc.right - 10.0) / (rc.bottom - 10.0);
		double AirRate = dLon / dLat;
		double drawRate;
		if (AirRate <= rcRate) {
			drawRate = (rc.bottom - 10.0) / dLat;
		}
		else {
			drawRate = (rc.right - 10.0) / dLon;
		}
		for (int i = 0; i < Air->PTaxiwayPaths->size(); i++) {
			if ((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x1) {
				SelectObject(aDC, hPenSolid2Blue);
			}
			if ((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x2) {
				SelectObject(aDC, hPenSolid2Grey);
			}
			if ((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x3) {
				SelectObject(aDC, hPenSolidRed);
			}
			if ((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x4) {
				SelectObject(aDC, hPenSolid2Green);
			}
			SelectObject(aDC, hPenSolid2Grey);
			if (((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x1) || ((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x2) || ((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x4)) {
				MoveToEx(aDC,
					((SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(Air->PTaxiwayPaths->at(i).IndexStartPoint).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0),
					(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(Air->PTaxiwayPaths->at(i).IndexStartPoint).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)), NULL);
				LineTo(aDC,
					((SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(Air->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0),
					(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(Air->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)));
			}
			if ((Air->PTaxiwayPaths->at(i).Type & 0xf) == 0x3) {
				MoveToEx(aDC,
					((SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(Air->PTaxiwayPaths->at(i).IndexStartPoint).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0),
					(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(Air->PTaxiwayPaths->at(i).IndexStartPoint).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)), NULL);
				LineTo(aDC,
					((SIMMATH::DecodeLon(Air->PTaxiwayParks->at(Air->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0),
					(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayParks->at(Air->PTaxiwayPaths->at(i).IndexEndPoint & 0xfff).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)));
			}
			SelectObject(aDC, hPenSolid2Green);
			for (int i = 0; i < PAirportData->ReturnPath->size() - 1; i++) {
				MoveToEx(aDC,
					(((PAirportData->ReturnPath->at(i).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0),
					(rc.bottom - (((PAirportData->ReturnPath->at(i).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)), NULL);
				LineTo(aDC,
					(((PAirportData->ReturnPath->at(i + 1).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0),
					(rc.bottom - (((PAirportData->ReturnPath->at(i + 1).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)));
			}
		}
		double INF = 1000000000.0;
		double dMin = INF, dMax = 0.0;
		for (int i = 0; i < PAirportData->d->size() - 1; i++) {
			if ((PAirportData->d->at(i) < INF) && (PAirportData->d->at(i) > 0.0)) {
				if (PAirportData->d->at(i) < dMin) {
					dMin = PAirportData->d->at(i);
				}
				if (PAirportData->d->at(i) > dMax) {
					dMax = PAirportData->d->at(i);
				}
			}
		}
		double dD = (1279.0 / (dMax - dMin));
		for (int i = 0; i < PAirportData->d->size() - 2; i++) {
			if ((PAirportData->d->at(i) > 0.0) && (PAirportData->d->at(i) < INF)) {
				int dRel = (PAirportData->d->at(i) - dMin) * dD;
				if (i == 700) {
					std::cout << 1;
				}
				int R, G, B;
				if ((dRel <= 255) && (dRel >= 0)) {
					R = 0;
					G = 0;
					B = dRel;
				}
				else if ((dRel <= 511) && (dRel >= 256)) {
					R = 0;
					G = (dRel - 256);
					B = 255;
				}
				else if ((dRel <= 767) && (dRel >= 512)) {
					R = 0;
					G = 255;
					B = 255 - (dRel - 512);
				}
				else if ((dRel <= 1023) && (dRel >= 768)) {
					R = dRel - 768;
					G = 255;
					B = 0;
				}
				else if ((dRel <= 1279) && (dRel >= 1024)) {
					R = 255;
					G = 255 - (dRel - 1024);
					B = 0;
				}
				else {
					std::cout << 1;
				}
				HPEN hPen = CreatePen(PS_SOLID, 2, RGB(R, G, B));
				SelectObject(aDC, hPen);
				if (i < Air->PTaxiwayPoints->size()) {
					Ellipse(aDC,
						((SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(i).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0) - 2,
						(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(i).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)) - 2,
						((SIMMATH::DecodeLon(Air->PTaxiwayPoints->at(i).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0) + 2,
						(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayPoints->at(i).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)) + 2);
				}
				else {
					Ellipse(aDC,
						((SIMMATH::DecodeLon(Air->PTaxiwayParks->at(i - Air->PTaxiwayPoints->size()).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0) - 2,
						(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayParks->at(i - Air->PTaxiwayPoints->size()).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)) - 2,
						((SIMMATH::DecodeLon(Air->PTaxiwayParks->at(i - Air->PTaxiwayPoints->size()).Lon) - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0) + 2,
						(rc.bottom - ((SIMMATH::DecodeLat(Air->PTaxiwayParks->at(i - Air->PTaxiwayPoints->size()).Lat) - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)) + 2);
				}
				DeleteObject(hPen);
			}
		}
		SelectObject(aDC, hPenSolidRed);
		Ellipse(aDC,
			((Lon - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0) - 4,
			(rc.bottom - ((Lat - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)) - 4,
			((Lon - MinLon) * drawRate) + (((dLon * drawRate) - rc.right) / 2.0) + 4,
			(rc.bottom - ((Lat - MinLat) * drawRate) + (((dLat * drawRate) - rc.bottom) / 2.0)) + 4);
	}
}*/