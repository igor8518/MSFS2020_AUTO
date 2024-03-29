#pragma once
#include "Structs.h"
#include "SimData.h"
#include "Utils.h"


enum GSServices {
	REQUEST_DEBOARDING,
	REQUEST_CATERING_SERVICE,
	REQUEST_REFUELING,
	REQUEST_BOARDING,
	REQUEST_DEPARTING,
	OPERATE_JETWAYS,
	OPERATE_STAIRS,
};

class CabinWork : public QObject {
  Q_OBJECT
public:
  CabinWork(HANDLE hSimConnect);
  SimData* DataT;
  bool Work = false;
private:
  QTimer* Timer;
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

  double SpeedMode = 1;
  double Speed = 100;
  double HeadingMode = 1;
  double Heading = 0;
  double AltMode = 0;
  double Alt = 100;
  double AltTest = 0;
  double VS = 0;
  
  bool SetDataChanged = false;

  

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
  void FlapRem(DWORD pos);
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
  void GearSet(DWORD pos);
  void WindowsAndDoors(DWORD num, DWORD openclose);
  void BeaconLigts(DWORD onoff);
  void EngineAntiIce(DWORD num, DWORD onoff);
  void WingAntiIce(DWORD onoff);
  void NoseLight(DWORD pos);
  void RunwayLight(DWORD onoff);
  void LandingLight(DWORD mode);
  void StrobeLight(DWORD onoff);
  void SetAutoBrakes(DWORD num);
  void HdgSel(double heading);
  void SpeedSel(DWORD speed);
  void VSSel(double VS);
  void CabinReport();
  void TOConfig();
  void TcasMode(DWORD mode);
  void TcasTraffic(DWORD mode);
  void AltRptg(DWORD onoff);
  void TransponderMode(DWORD mode);
  void Pack(DWORD num, DWORD onoff);

  double SetData(DWORD var, double val, const char* unit = "");
  double SetDataL(DWORD var, double val, const char* unit = "");
  DWORD SendEvent(DWORD EventID, long dwData);
  DWORD SendEvent2(DWORD EventID, long dwData, DWORD var, double val, const char* unit = "");

signals:
  void SendText(QString s, bool sendSim);
  void SendLog(QString s);
  void SetDataSignal(DWORD sender, DWORD var, double* val, const char* unit = "");
  void SetDataSignalL(DWORD sender, DWORD var, double* val, const char* unit = "");
  void SendEventSignal(DWORD sender, DWORD EventID, long dwData);
  void SendEventSignal2(DWORD sender, DWORD EventID, long dwData, DWORD var, double val, const char* unit = "");
  void SendCommand(DWORD command, double parameter1, double parameter2);

public slots:
  void TimerProc();
  void SetDataChange(DWORD var, DWORD sender); //Direct
  void ReceiveCommand(DWORD command, double parameter1, double parameter2);
  HRESULT GSXServicesRequest(GSServices sr);
  void CLPreliminaryCocpitPrep(int* Status);
  void CLCocpitPreparation(int* Status);
  void CLBeforeStart(int* Status);
  void CLEngineStart(int* Status);
  void CLAfterTakeoff(int* Status);
  void CLLanding(int* Status);
  void CLAfterLanding(int* Status);
  void CLParking(int* Status);
  void SetTimer(DWORD mode);

  void SetCabinLight(DWORD value);

  void CLAfterStart(int* Status);
  void CLBeforeTaxi(int flightLevel, int heading, int* Status);
  void CLBeforeTakeoff(int* Status);
};

