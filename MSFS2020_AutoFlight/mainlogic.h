#pragma once

#include "headers.h"
#include "msfs2020_autoflight.h"
#include <qtimer.h>
#include <qeventloop.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QtNetwork/qnetworkreply>
#include <QUrl>
#include <QFile>
#include <cmath>
class MainLogic : public QObject {
  Q_OBJECT;
  
public:
  MainLogic(PlanesWork* planesWork, MSFS2020_AutoFlight* mainOblect, QObject* parent = Q_NULLPTR);
  ~MainLogic();
  Utils* utils;
  HANDLE HSimConnect;
  DWORD Mode = 0;
  DWORD CurrentWay = 0;
  bool STAR = false;
  BOOL Quit = FALSE;
  QString PLNFile;
  std::string* ParkN = NULL;
  std::string ParkType[14] = { "", "RAMP_GA", "RAMP_GA_SMALL", "RAMP_GA_MEDIUM", "RAMP_GA_LARGE", "RAMP_CARGO", "RAMP_MIL_CARGO", "RAMP_MIL_COMBAT", "GATE_SMALL", "GATE_MEDIUM", "GATE_HEAVY", "DOCK_GA", "FUEL", "VEHICLES" };
  std::string ParkName[38] = { "NONE", "PARKING", "N_PARKING", "NE_PARKING", "E_PARKING", "SE_PARKING", "S_PARKING", "SW_PARKING", "W_PARKING", "NW_PARKING", "GATE", "DOCK",
    "GATE_A", "GATE_B", "GATE_C", "GATE_D", "GATE_E", "GATE_F", "GATE_G", "GATE_H", "GATE_I", "GATE_J", "GATE_K", "GATE_L", "GATE_M", "GATE_N", "GATE_O", "GATE_P", "GATE_Q", "GATE_R", "GATE_S", "GATE_T",
    "GATE_U", "GATE_V", "GATE_W", "GATE_X", "GATE_Y", "GATE_Z" };
  double minDCommon = 10000;
  std::vector<sWayPoint>* WayPoints = NULL;
  static void CALLBACK FDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* mainLogic);
  Ui::MSFS2020_AutoFlightClass* GetUi();
signals:
  void SendText(QString s, bool sendSim);
  void SendLog(QString s);
  void ButtonModify(QPushButton* button, QString text, QString style);
  void SetDataReg(unsigned int index, double data);
  void SetDataSetReg(unsigned int index, double data);
  void PreliminaryCocpitPrep(int* Status);
  void SetModeSignal(DWORD mode);
  void CocpitPreparation(int* Status);
  void BeforeStart(int* Status);
  void EngineStart(int* Status);
  void AfterStart(int* Status);
  void BeforeTaxi(int flightLevel, int heading, int* Status);
  void BeforeTakeoff(int* Status);
  

  void RegisterVar(DWORD DefaultParameter, char* unit = "");
  void GetDataSignal(DWORD sender, DWORD var, double* val, char* unit = "");
  void GetDataSignalL(DWORD sender, DWORD var, double* val, char* unit = "");
  void GetDataStringSignal(DWORD sender, DWORD var, std::string* val);
  void SetDataSignal(DWORD sender, DWORD var, double* val, char* unit = "");
  void SetGetDataSignal(DWORD sender, DWORD varSet, DWORD varGet, double* val, char* unit = "");
  void SendEventSignal(DWORD sender, DWORD EventID, long dwData);

  void SendCommand(DWORD command, double parameter1, double parameter2);
private slots:
  void TimerProc();
  void Connect();
  void Disconnect();
  void SetMode(DWORD mode);
  double CalcToNewWay(bool changeWay = true);
  void GetDataChange(DWORD var, DWORD sender);
  void SetDataChange(DWORD var, DWORD sender);
  void SetGetDataChange(DWORD var, DWORD sender);
  std::vector<TSIDS>* AddSID(SIDSTAR* sidstar, TPath* RWEnd, double RWHead, QString fix, QString SBSID = "");
  std::vector<TSTARS>* AddSTAR(SIDSTAR* sidstar, DATA_RUNWAY* runway, QString fix, QString SBSTAR= "");
  void AddSidStarTrack(WayPointA* wayPoint, TPath* RWEnd, FIXX* fixx, DATA_RUNWAY* runway = NULL);
  double AltPitchWithPos(double TargetValue);
  double PBHeadWithWay(sWayPoint* Way, bool PB = false);
  double ManHeadWithWay(sWayPoint* Way);
  void ManPitchWithFD(double NPitch);
  void ManBankWithFD(double NNBank);
  double BankWithHead(double Heading);
  double AltBankWithPos(double TargetValue);
  double GetDescentAngle(std::vector<sWayPoint>* Way);
  float GetAngleToDesc(float alt);
  double ManVSWithAngle(double GS);
  double ManVSWithGlide(sWayPoint* Way, double GS, double TAlt, double BiasDist = 0);
  bool SetTimeOff(int IDREQ, int TimeOffset);
private:
  bool abortLanding = false;
  bool abortLanding2 = false;
  bool flare = false;
  alterPid VSPID = { -0.0008, 0.0, 0, 0.1, 1.0, 0.0, 1.5, 0.001, 0.0, 0.0, 0.0, 0, -15.0, 15.0, true };
  alterPid PPID = { -0.2, 0.0, 0, 0.1, 1.0, 0.0, 2.0, 0.001, 0.0, 0.0, 0.0, 0, -1.0, 1.0, true };
  SPid pidBank = { 0.0, 0.0, 1500.0, 15.0, 5.0, 0.0, 0.0, 0.0, 0.0 };
  SPid pidPitch = { 0.0, 0.0, PPID.Kp, PPID.Ti, PPID.Td, 0.0, 0.0, 0.0, 0.0 };
  SPid pidVSpeed = { 0.0, 0.0, VSPID.Kp, VSPID.Ti, VSPID.Td, 0.0, 0.0, 0.0, 0.0 };
  SPid* SelectedPid;
  bool SetDataBank = false;
  bool TOGA = false;
  std::vector<sWayPoint> RunWaysPathsOrig;
  std::vector<sWayPoint> RunWaysPathsDest;
  DWORD Taxiway = 0;
  double speed = 0;
  bool GetDataChanged = false;
  bool SetDataChanged = false;
  bool SetGetDataChanged = false;
  IAirportData* AirportData = NULL;
  std::vector<bool> DataChanged = std::vector<bool>(CVars, true);
  double GetData(DWORD DefaultParameter, char* unit = "");
  double GetDataL(DWORD DefaultParameter, char* unit = "");
  double SetData(DWORD var, double val, char* unit = "");
  std::string GetDataString(DWORD var);
  DWORD SendEvent(DWORD EventID, DWORD dwData);
  Ui::MSFS2020_AutoFlightClass* ui;
  SimData* data;
  QTimer* MainTimer;
  double MaximumAltitude;
  CabinWork* cabinWork;
  PlanesWork* planesWork = NULL;
  ExQThread* TUtils;
  ExQThread* TData;
  ExQThread* TCabinWork;
  ExQThread* TPlanesWork;
  QColor DefColorBgrnd;
  QColor DefColorTxt;
  UINT Timer = 0;
  BOOL Connected = FALSE;
  std::vector<QMetaObject::Connection>* ObjectConnectors;
  std::vector<QMetaObject::Connection>* ConnectConnectors;
  sExportData ExportData;
  //sAllData AllData;
  A32NX_Control Control;
  
  QJsonDocument document;
  QNetworkAccessManager* mgr;
  QJsonArray ja;
  sWayPoint CurrentPos;
  double RudWithHead(double Heading);
  std::string AtcRwy = "";

  void AddWayPoint(double lon, double lat, double alt, QString pointType, QString PointName, double heading, int fixAlt, double speed, double radial, QJsonObject* fix = NULL);


  QString SimBriefSID = "";
  QString SimBriefSTAR = "";
  QString FixSID = "";
  QString FixSTAR = "";

  SIDSTAR* OrigSidStar = NULL;
  SIDSTAR* DestSidStar = NULL;
  SIDSTAR* AltSidStar = NULL;

  int preliminaryCocpitPrep = 0;
  int cocpitPreparation = 0;
  int beforeStart = 0;
  int engineStart = 0;
  int afterStart = 0;
  int beforeTaxi = 0;
  int beforeTakeoff = 0;
  bool SetDataPitch; //?

  int RoutePoint = 0;
  int RunwayHeading = 0;
  double FlightCruise = 0;

  int rrr = -1;
  int SIDPoint = 0;
  int AppPoint = 0;
  int STARPoint = 0;

  QString TYPE_PATHS[22] = {
  "TAXIWAY",
  "RUNWAY",
  "WAYPOINT",
  "FIXHEADING",
  "HEADINGUNTILALT",
  "HEADINGUNTILRADIALTOFIX",
  "FIXATORABOVE",
  "TURNLEFTDIRECTFIX",
  "TURNRIGHTDIRECTFIX",
  "TURNRIGHTDIRECTFIXOVERFLY",
  "TURNLEFTDIRECTFIXOVERFLY",
  "TURNRIGHTDIRECTFIXOVERFLYATORABOVE",
  "TURNLEFTDIRECTFIXOVERFLYATORABOVE",
  "FIX",
  "TRKINTERCEPTRADIAL",
  "TRKINTERCEPTRADIALTOFIXATORABOVE",
  "FIXOVERFLY",
  "FIXOVERFLYATORABOVE",
  "HDGVECTORS",
  "HEADINGUNTILFROMDIST",
  "GSWAY",
  "GATE",
  };
};
