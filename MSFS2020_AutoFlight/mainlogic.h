#pragma once


#include <ui_msfs2020_autoflight.h>
#include "Structs.h"
#include "WriteStream.h"
#include "SimData.h"
#include "msfs2020_autoflight.h"
#include "ExQThread.h"
#include "CabinWork.h"
#include "PlanesWork.h"
#include "Utils.h"

class QTableViewModel : public QAbstractListModel {
public:
    QTableViewModel(QObject* parrent = nullptr);
    int rowCount(const QModelIndex&) const;
    int columnCount(const QModelIndex& parrent) const;
    QVariant data(const QModelIndex& index, int role) const;
    void populate(std::vector<sWayPoint>* newValues);
private:
    std::vector<sWayPoint>* values;
};

template <typename T, typename U>
struct CompareByMember {
    // This is a pointer-to-member, it represents a member of class T
    // The data member has type U
    U T::* field;
    CompareByMember(U T::* f) : field(f) {}
    bool operator()(const T& lhs, const T& rhs) {
        return lhs.*field < rhs.*field;
    }
};
//#include "WriteStream.h"
class MainLogic : public QObject {
  Q_OBJECT;
  
public:
    SimData* data = NULL;
  MainLogic(PlanesWork* planesWork, MSFS2020_AutoFlight* mainOblect, QObject* parent = Q_NULLPTR);
  ~MainLogic();
  QTableViewModel* ModelTable;
  std::vector<QString>* LogArray = new std::vector<QString>();
  void Log(QString log);
  Utils* utils;
  HANDLE HSimConnect;
  DWORD Mode = 0;
  bool InTimer = false;
  DWORD CurrentWayIndex = 0;
  DWORD FlightPhase;
  bool STAR = false;
  BOOL Quit = FALSE;
  QString PLNFile;
  std::string* ParkN = NULL;
  double AddRadius = 0;
  std::string ParkType[14] = { "", "RAMP_GA", "RAMP_GA_SMALL", "RAMP_GA_MEDIUM", "RAMP_GA_LARGE", "RAMP_CARGO", "RAMP_MIL_CARGO", "RAMP_MIL_COMBAT", "GATE_SMALL", "GATE_MEDIUM", "GATE_HEAVY", "DOCK_GA", "FUEL", "VEHICLES" };
  std::string ParkName[38] = { "NONE", "PARKING", "N_PARKING", "NE_PARKING", "E_PARKING", "SE_PARKING", "S_PARKING", "SW_PARKING", "W_PARKING", "NW_PARKING", "GATE", "DOCK",
    "GATE_A", "GATE_B", "GATE_C", "GATE_D", "GATE_E", "GATE_F", "GATE_G", "GATE_H", "GATE_I", "GATE_J", "GATE_K", "GATE_L", "GATE_M", "GATE_N", "GATE_O", "GATE_P", "GATE_Q", "GATE_R", "GATE_S", "GATE_T",
    "GATE_U", "GATE_V", "GATE_W", "GATE_X", "GATE_Y", "GATE_Z" };
  double minDCommon = 10000;
  std::vector<sWayPoint>* Legs = NULL;
  //std::vector<sWayPoint>* QLegs = NULL;
  static void CALLBACK FDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* mainLogic);
  Ui::MSFS2020_AutoFlightClass* GetUi();
signals:
  void PlotConstraints(std::vector<sWayPoint>* Legs, int startIndex, int endIndex, int currentIndewx);
  void PlotPoints(std::vector<sWayPoint>* Legs, int startIndex, int endIndex);
  void PlotRealPath(double flyPoint, double planeAlt, double commonDistance);
  void PlotCircle(double dist, double alt, double common);
  void SelectRow(int Row);
  void SendText(QString s, bool sendSim);
  void SendLog(QString s);
  void ButtonModify(QPushButton* button, QString text, QString style);
  void StartButtonEnabled(bool enabled);
  void ConnectButtonEnabled(bool enabled);
  void SetDataReg(unsigned int index, double data);
  void SetDataSetReg(unsigned int index, double data);
  void CLPreliminaryCocpitPrep(int* Status);
  void SetModeSignal(DWORD mode);
  void CLCocpitPreparation(int* Status);
  void CLBeforeStart(int* Status);
  void CLEngineStart(int* Status);
  void CLAfterStart(int* Status);
  void CLBeforeTaxi(int flightLevel, int heading, int* Status);
  void CLBeforeTakeoff(int* Status);
  void CLAfterTakeoff(int* Status);
  void CLLanding(int* Status);
  void CLAfterLanding(int* Status);
  void CLParking(int* Status);
  void CabinReport();
  void ReplotGraphs();
  

  void RegisterVar(DWORD DefaultParameter, const char* unit = "");
  void GetDataSignal(DWORD sender, DWORD var, double* val, const char* unit = "");
  void GetDataSignalL(DWORD sender, DWORD var, double* val, const char* unit = "");
  void GetDataStringSignal(DWORD sender, DWORD var, std::string* val);
  void SetDataSignal(DWORD sender, DWORD var, double* val, const char* unit = "");
  void SetDataSignalL(DWORD sender, DWORD var, double* val, const char* unit = "");
  void SetGetDataSignal(DWORD sender, DWORD varSet, DWORD varGet, double* val, const char* unit = "");
  void SendEventSignal(DWORD sender, DWORD EventID, long dwData);

  void SendCommand(DWORD command, double parameter1, double parameter2);
private slots:
    void ChangeCurrentLegIndex(int currentLegIndex);
    //void on_ChangeRow(QItemSelection Row, QItemSelection prev);
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

  double GetAngleFront(double frontOut, bool nose);

  double PID(double dt, double err, double kp, double ki, double kd, double bi, double bd, double* prevErr, double* integral);

  void ManPitchWithFD(double NPitch);
  void ManBankWithFD(double NNBank);
  double BankWithHead(double Heading);
  double AltBankWithPos(double TargetValue);
  double GetDescentAngle();
  float GetAngleToDesc(float alt);
  double ManVSWithAngle(double GS);
  double GetVerticalSpeedForGlide(sWayPoint* Way, double GS, double TAlt, double BiasDist = 0);
  void SendDataPMDG(DWORD val);
  void StartStopSim();
  bool SetTimeOff(int IDREQ, int TimeOffset);
private:
    bool Approach = FALSE;
    double flyPoint;
    WriteStream* Flight = NULL;
    double CommonDistance = 0;
    double TraveledDistance = 0;
    double GRemainingDistance = 0;
    double lastlat = 0;
    double lastalt = 0;
    double lastlon = 0;
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
  int GraphNums = 0;
  double NPitchWork = 0;
  double AvgCounter = 0;
  double Pitch50 = 8518;
  double AvgMax = 0;
  double AvgPitch = 0;
  bool GetDataChanged = false;
  bool SetDataChanged = false;
  bool SetGetDataChanged = false;
  IAirportData* AirportData = NULL;
  std::vector<bool> DataChanged = std::vector<bool>(CVars, true);
  
  //double GetData(DWORD DefaultParameter, const char* unit = "");
  double GetDataL(DWORD DefaultParameter, const char* unit = "");
  double SetData(DWORD var, double val, const char* unit = "");
  double SetDataL(DWORD var, double val, const char* unit = "");
  std::string GetDataString(DWORD var);
  DWORD SendEvent(DWORD EventID, DWORD dwData);
  Ui::MSFS2020_AutoFlightClass* ui;
  
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
  sWayPoint CurrentLeg;
  double RudWithHead(double Heading);
  double CalcVelocity(double last, double current, double dt);
  double LimitVal(double val, double min, double max);
  std::string AtcRwy = "";

  void AddWayPoint(double lon, double lat, double alt, QString pointType, QString PointName, double heading, int fixAlt, double speed, double radial, double altLo, QJsonObject* fix = NULL);

  void ChangeFlightPhaseReport();

  sWayPoint GetCurrentLeg();

  double GetRemainingDistance();

  int GetOrigRunwayIndex();

  int GetDestRunwayIndex();
  

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
  int afterTakeoff = 0;
  int landing = 0;
  int afterLanding = 0;
  int parking = 0;
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

