#pragma once
#include "Structs.h"
#include "SimData.h"
#include "Utils.h"
//

//class SimData;


class CabinWork : public QObject {
  Q_OBJECT
public:
  CabinWork(HANDLE hSimConnect/*, cVars* vars*/);
  SimData* DataT;
  
private:
  HANDLE HSimConnect;
  double Flaps = 0;
  double Gears = 1;
  double Spoilers = 0;
  double LightStrobe = 0;
  double LightLanding = 0;
  double LightTaxi = 0;
  double LightRunway = 0;
  double ParkBrake = 1;
  double BaroM = 1;
  double BaroS;

  double SpeedMode = 1;
  double Speed = 100;
  double HeadingMode = 1;
  double Heading = 0;

  double PrevAlt;
  double AltMode = 0;
  double Alt = 100;
  double VS = 0;
  
  bool GetDataChanged = false;
  bool SetDataChanged = false;
  bool SetGetDataChanged = false;
  bool SetSetDataChanged = false;

  QTimer* Timer;

  void SetATHR(DWORD onoff);
  void EngineModeSelector(DWORD mode);
  void EngineMasterSwitches(DWORD num, DWORD mode);
  void Wiper(DWORD num, DWORD mode);
  void BatterySwitch(DWORD num, DWORD onoff);
  void ExternalPower(DWORD onoff);
  void APUSet(DWORD onoff);
  void APUOff();
  void FuelPump(DWORD num, DWORD onoff);
  void APUFireTest();
  void APUBleedSet(DWORD onoff);
  void CabinLight(DWORD mode);
  void ParkingBrakeSet(DWORD onoff);
  void FlapsSet(DWORD pos);
  long IncDec16384_100(double curr, bool dir);
  void SpoilersSet(double pos);
  void ProbeWindowHeat(DWORD onoff);
  void XBleedSet(DWORD mode);
  void EvacCapt(DWORD mode);
  void ADIRS(DWORD num, DWORD mode);
  void NavLight(DWORD onoff);
  void NoSmokeSign(DWORD mode);
  void EmerExitSign(DWORD mode);
  void SeatBeltSign(DWORD mode);
  void LandingElevation(double pos);
  void PackFlow(DWORD mode);
  void EngFireTest(DWORD num);
  void TrustRedAccAlt(double red, double acc);
  void FlapRem(DWORD pos);
  void SetCruise(DWORD alt);
  void FloodLightOver(double flood);
  void BaroMode(DWORD mode);
  void BarometricRef();
  void FDSet(DWORD num, DWORD onoff);
  void NDMode(DWORD num, DWORD mode);
  void NDRange(DWORD num, DWORD range);
  void ADFVOR1(DWORD num, DWORD mode);
  void ADFVOR2(DWORD num, DWORD mode);
  void SpeedMachMode(DWORD mode);
  void HdgVsTrkFpa(DWORD mode);
  void AltSet(DWORD alt);
  void SetAltitudeIncrement(DWORD alt);
  void AltInitial(DWORD alt);
  void CrewSupply(DWORD offon);
  void ASandNS(DWORD offon);
  void WxrSys(DWORD mode);
  void WxrWs(DWORD onoff);
  void WxrMode(DWORD mode);
  void IrsAligned();
  void EngineIdleParameters(DWORD num);
  char* GetTitleCDU();
  void GearSet(DWORD pos);
  double GetData(DWORD var, char* unit = "");
  double GetDataL(DWORD var, char* unit = "");
  double SetData(DWORD var, double val, char* unit = "");
  double SetDataL(DWORD var, double val, char* unit = "");
  double SetGetData(DWORD varSet, DWORD varGet, double val, char* unit = "");
  double SetSetData(DWORD var1, double val1, DWORD var2, double val2, char* unit1 = "", char* unit2 = "");
  DWORD SendEvent(DWORD EventID, long dwData);

  DWORD SendEvent2(DWORD EventID, long dwData, DWORD var, double val, char* unit = "");

signals:
  void SendText(QString s, bool sendSim);
  void SendLog(QString s);
  void GetDataSignal(DWORD sender, DWORD var, double* val, char* unit = "");
  void GetDataSignalL(DWORD sender, DWORD var, double* val, char* unit = "");
  void SetDataSignal(DWORD sender, DWORD var, double* val, char* unit = "");
  void SetDataSignalL(DWORD sender, DWORD var, double* val, char* unit = "");
  void SetGetDataSignal(DWORD sender, DWORD varSet, DWORD varGet, double* val, char* unit = "");
  void SendEventSignal(DWORD sender, DWORD EventID, long dwData);
  void SendEventSignal2(DWORD sender, DWORD EventID, long dwData, DWORD var, double val, char* unit = "");
  void SetSetDataSignal(DWORD sender, DWORD var1, double* val1, DWORD var2, double* val2, char* unit1 = "", char* unit2 = "");

  void SendCommand(DWORD command, double parameter1, double parameter2);

public slots:
  void TimerProc();
  void SetDataChange(DWORD var, DWORD sender); //Direct
  void GetDataChange(DWORD var, DWORD sender); //Direct
  void SetGetDataChange(DWORD var1, DWORD var2, DWORD sender); //Direct
  void SetSetDataChange(DWORD var1, DWORD var2, DWORD sender); //Direct
  void ReceiveCommand(DWORD command, double parameter1, double parameter2);
  
  void PreliminaryCocpitPrep(int* Status);
 
  

  void CocpitPreparation(int* Status);
  void WindowsAndDoors(DWORD num, DWORD openclose);
  void BeaconLigts(DWORD onoff);
  void BeforeStart(int* Status);
  
  void EngineStart(int* Status);
  void AfterTakeoff(int* Status);
  void Landing(int* Status);
  void AfterLanding(int* Status);
  void Parking(int* Status);
  
  void EngineAntiIce(DWORD num, DWORD onoff);
  void WingAntiIce(DWORD onoff);
  void AfterStart(int* Status);
  void NoseLight(DWORD pos);
  void RunwayLight(DWORD onoff);
  void LandingLight(DWORD mode);
  void StrobeLight(DWORD onoff);
  void SetAutoBrakes(DWORD num);
  void HdgSel(double heading);
  void SpeedSel(DWORD speed);
  void VSSel(double VS);
  void BeforeTaxi(int flightLevel, int heading, int* Status);
  void CabinReport();
  void TOConfig();
  void TcasMode(DWORD mode);
  void TcasTraffic(DWORD mode);
  void Pack(DWORD num, DWORD onoff);
  void BeforeTakeoff(int* Status);
};

