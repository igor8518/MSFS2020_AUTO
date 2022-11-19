#pragma once
#include "Structs.h"

#include "msfs2020_autoflight.h"
//#include <qdatetime.h>
//#include "msfs2020_autoflight.h"

//struct sWayPoint;

class Utils : public QObject
{
  Q_OBJECT
public:
  explicit Utils(MSFS2020_AutoFlight* mainLogic, HANDLE* hsimConnect, QObject* parent = Q_NULLPTR);
  
public slots:
  void SetText(QString s, bool simDisplay = false);
  static double DegToRad(double D);
  void AddLog(QString s);
  static double GetPathLength(std::vector<TPath>* p);
  static std::string Utils::Unpack(std::vector<double> values);
  static double GetRWAngle(std::vector<TPath>* p);
  static double GetRWLength(std::vector<TPath>* p);
  static SIMCONNECT_DATA_LATLONALT GetDALatLon(double lat, double lon, double ang, double dist);
  static void DOrtoKM(sWayPoint* dsheh);
  static double DToType(DWORD currWay, std::vector<sWayPoint>* wayPoints, QString type, sWayPoint* currPos, double headingRel);
  static double DToRunway(DWORD currWay, std::vector<sWayPoint>* wayPoints, sWayPoint* currPos);
  static double Constrain180(double angle);
  static double Constrain(double val, double min, double max);
  static double Constrain360(double angle);
  static double GetFixDA(double da, double angle);
  static double AngleLimitS(double angle, double limit);
  static double RunwayDictCalc(std::vector<sWayPoint>* runwayPaths, sWayPoint* CurrentLeg);
  //static double CalcToNewWay(bool changeWay, std::vector<sWayPoint>* wayPoints, sWayPoint* currentPos, int currentWay);
  //void DrawAirport(IAirportData* PAirportData, double Lat, double Lon, double Hed);
private:
  MSFS2020_AutoFlight* mainLogic;
  HANDLE* HSimConnect;
signals:
  void SendLog(QString s);
  void SendText(QString s, bool sendSim = false);
private:
};

