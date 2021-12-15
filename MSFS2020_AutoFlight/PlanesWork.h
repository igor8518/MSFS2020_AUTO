#pragma once
#include "Structs.h"

#include "SimData.h"
#include "SimData.h"
class PlanesWork : public QObject {
  Q_OBJECT

    Q_PROPERTY(bool sourceInThrottle READ sourceThrottle WRITE setSourceThrottle)
    Q_PROPERTY(double InThrottle READ inThrottle WRITE setInThrottle)
    Q_PROPERTY(double minInThrottle READ minInThrottle)
    Q_PROPERTY(double maxInThrottle READ maxInThrottle)

    Q_PROPERTY(bool sourceInAileron READ sourceAileron WRITE setSourceAileron)
    Q_PROPERTY(double InAileron READ inAileron WRITE setInAileron)
    Q_PROPERTY(double minInAileron READ minInAileron)
    Q_PROPERTY(double maxInAileron READ maxInAileron)

    Q_PROPERTY(bool sourceInElevator READ sourceElevator WRITE setSourceElevator)
    Q_PROPERTY(double InElevator READ inElevator WRITE setInElevator)
    Q_PROPERTY(double minInElevator READ minInElevator)
    Q_PROPERTY(double maxInElevator READ maxInElevator)

    Q_PROPERTY(bool sourceInRudder READ sourceRudder WRITE setSourceRudder)
    Q_PROPERTY(double InRudder READ inRudder WRITE setInRudder)
    Q_PROPERTY(double minInRudder READ minInRudder)
    Q_PROPERTY(double maxInRudder READ maxInRudder)

    Q_PROPERTY(bool sourceTrust READ sourceTrust WRITE setSourceTrust)
    Q_PROPERTY(double Trust READ Trust WRITE setTrust)
    Q_PROPERTY(double CurrTrust READ CurrTrust)
    Q_PROPERTY(double minTrust READ minTrust)
    Q_PROPERTY(double maxTrust READ maxTrust)

    Q_PROPERTY(bool sourceAccseleration READ sourceAccseleration WRITE setSourceAccseleration)
    Q_PROPERTY(double Accseleration READ Accseleration WRITE setAccseleration)
    Q_PROPERTY(double CurrAccseleration READ CurrAccseleration)
    Q_PROPERTY(double minAccseleration READ minAccseleration)
    Q_PROPERTY(double maxAccseleration READ maxAccseleration)

    Q_PROPERTY(bool sourceGSpeed READ sourceGSpeed WRITE setSourceGSpeed)
    Q_PROPERTY(double GSpeed READ GSpeed WRITE setGSpeed)
    Q_PROPERTY(double CurrGSpeed READ CurrGSpeed)
    Q_PROPERTY(double minGSpeed READ minGSpeed)
    Q_PROPERTY(double maxGSpeed READ maxGSpeed)

    Q_PROPERTY(double OutThrottle READ outThrottle)
    Q_PROPERTY(double minOutThrottle READ minInThrottle)
    Q_PROPERTY(double maxOutThrottle READ maxInThrottle)

    Q_PROPERTY(double OutAileron READ outAileron)
    Q_PROPERTY(double minOutAileron READ minInAileron)
    Q_PROPERTY(double maxOutTAileron READ maxInAileron)

    Q_PROPERTY(double OutElevator READ outElevator)
    Q_PROPERTY(double minOutElevator READ minInElevator)
    Q_PROPERTY(double maxOutElevator READ maxInElevator)

    Q_PROPERTY(double OutRudder READ outRudder)
    Q_PROPERTY(double minOutRudder READ minInRudder)
    Q_PROPERTY(double maxOutRudder READ maxInRudder)

    Q_PROPERTY(double PInThrottleOutThrottle READ PInThrottleOutThrottle WRITE setPInThrottleOutThrottle)
    Q_PROPERTY(double IInThrottleOutThrottle READ IInThrottleOutThrottle WRITE setIInThrottleOutThrottle)
    Q_PROPERTY(double DInThrottleOutThrottle READ DInThrottleOutThrottle WRITE setDInThrottleOutThrottle)

    Q_PROPERTY(double PInAileronOutAileron READ PInAileronOutAileron WRITE setPInAileronOutAileron)
    Q_PROPERTY(double IInAileronOutAileron READ IInAileronOutAileron WRITE setIInAileronOutAileron)
    Q_PROPERTY(double DInAileronOutAileron READ DInAileronOutAileron WRITE setDInAileronOutAileron)

    Q_PROPERTY(double PInElevatorOutElevator READ PInElevatorOutElevator WRITE setPInElevatorOutElevator)
    Q_PROPERTY(double IInElevatorOutElevator READ IInElevatorOutElevator WRITE setIInElevatorOutElevator)
    Q_PROPERTY(double DInElevatorOutElevator READ DInElevatorOutElevator WRITE setDInElevatorOutElevator)

    Q_PROPERTY(double PInRudderOutRudder READ PInRudderOutRudder WRITE setPInRudderOutRudder)
    Q_PROPERTY(double IInRudderOutRudder READ IInRudderOutRudder WRITE setIInRudderOutRudder)
    Q_PROPERTY(double DInRudderOutRudder READ DInRudderOutRudder WRITE setDInRudderOutRudder)

    Q_PROPERTY(double PTrustInThrottle READ PTrustInThrottle WRITE setPTrustInThrottle)
    Q_PROPERTY(double ITrustInThrottle READ ITrustInThrottle WRITE setITrustInThrottle)
    Q_PROPERTY(double DTrustInThrottle READ DTrustInThrottle WRITE setDTrustInThrottle)

    Q_PROPERTY(double PAccselerationTrust READ PAccselerationTrust WRITE setPAccselerationTrust)
    Q_PROPERTY(double IAccselerationTrust READ IAccselerationTrust WRITE setIAccselerationTrust)
    Q_PROPERTY(double DAccselerationTrust READ DAccselerationTrust WRITE setDAccselerationTrust)

    Q_PROPERTY(double PGSpeedAccseleration READ PGSpeedAccseleration WRITE setPGSpeedAccseleration)
    Q_PROPERTY(double IGSpeedAccseleration READ IGSpeedAccseleration WRITE setIGSpeedAccseleration)
    Q_PROPERTY(double DGSpeedAccseleration READ DGSpeedAccseleration WRITE setDGSpeedAccseleration)

    Q_PROPERTY(double PGSpeedTrust READ PGSpeedTrust WRITE setPGSpeedTrust)
    Q_PROPERTY(double IGSpeedTrust READ IGSpeedTrust WRITE setIGSpeedTrust)
    Q_PROPERTY(double DGSpeedTrust READ DGSpeedTrust WRITE setDGSpeedTrust)
    Q_PROPERTY(double DGSpeedTrust READ DGSpeedTrust WRITE setDGSpeedTrust)
public:
  SimData* DataT;
  QTimer* Timer;
  PlanesWork(HANDLE hSimConnect);
  double inThrottle() { return _inThrottle; };
  double minInThrottle() { return -3277; };
  double maxInThrottle() { return 16383; };

  double inAileron() { return _inAileron; };
  double minInAileron() { return -16383; };
  double maxInAileron() { return 16383; };

  double inElevator() { return _inElevator; };
  double minInElevator() { return -16383; };
  double maxInElevator() { return 16383; };

  double inRudder() { return _inRudder; };
  double minInRudder() { return -16383; };
  double maxInRudder() { return 16383; };

  double CurrTrust() { return _CurrTrust * 16383; };
  double CurrAccseleration() { return _CurrAccseleration / 10 * 16383; };
  double CurrGSpeed() { return _CurrGSpeed / 35 * 16383; };

  double Trust() { return _Trust*163.83; };
  double minTrust() { return 0; };
  double maxTrust() { return 16383; };

  double Accseleration() { return _Accseleration / 10 * 16383; };
  double minAccseleration() { return 0; };
  double maxAccseleration() { return 10 / 10 * 16383; };

  double GSpeed() { return _GSpeed / 35 * 16383; };
  double minGSpeed() { return 0; };
  double maxGSpeed() { return 30 / 35 * 16383; };


  double outThrottle() { return _outThrottle; };
  double minOutThrottle() { return -3277; };
  double maxOutThrottle() { return 16383; };

  double outAileron() { return _outAileron; };
  double minOutAileron() { return -16383; };
  double maxOutAileron() { return 16383; };

  double outElevator() { return _outElevator; };
  double minOutElevator() { return -16383; };
  double maxOutElevator() { return 16383; };

  double outRudder() { return _outRudder; };
  double minOutRudder() { return -16383; };
  double maxOutRudder() { return 16383; };

  void setInThrottle(double inThrottle) { _inThrottle = inThrottle; TTAS = 0; };
  bool sourceThrottle() { return _sourceThrottle; };
  void setSourceThrottle(bool sourceThrottle) { _sourceThrottle = sourceThrottle; };

  void setInAileron(double inAileron) { _inAileron = inAileron; ABVH = 13; };
  bool sourceAileron() { return _sourceAileron; };
  void setSourceAileron(bool sourceAileron) { _sourceAileron = sourceAileron; };

  void setInElevator(double inElevator) { _inElevator = inElevator; EPVA = 14; };
  bool sourceElevator() { return _sourceElevator; };
  void setSourceElevator(bool sourceElevator) { _sourceElevator = sourceElevator; };

  void setInRudder(double inRudder) { _inRudder = inRudder; RVH = 15; };
  bool sourceRudder() { return _sourceRudder; };
  void setSourceRudder(bool sourceRudder) { _sourceRudder = sourceRudder; };

  void setTrust(double Trust) { _Trust = Trust/163.83; TTAS = 1; };
  bool sourceTrust() { return _sourceTrust; };
  void setSourceTrust(bool sourceTrust) { _sourceTrust = sourceTrust; };

  void setAccseleration(double Accseleration) { _Accseleration = Accseleration / 16383 * 10; TTAS = 2; };
  bool sourceAccseleration() { return _sourceAccseleration; };
  void setSourceAccseleration(bool sourceAccseleration) { _sourceAccseleration = sourceAccseleration; };

  void setGSpeed(double GSpeed) { _GSpeed = GSpeed / 16383 * 35; TTAS = 3; };
  bool sourceGSpeed() { return _sourceGSpeed; };
  void setSourceGSpeed(bool sourceGSpeed) { _sourceGSpeed = sourceGSpeed; };

  void setPInThrottleOutThrottle(double PInThrottleOutThrottle) { _PInThrottleOutThrottle = PInThrottleOutThrottle; };
  double PInThrottleOutThrottle() { return _PInThrottleOutThrottle; };
  void setIInThrottleOutThrottle(double IInThrottleOutThrottle) { _IInThrottleOutThrottle = IInThrottleOutThrottle; };
  double IInThrottleOutThrottle() { return _IInThrottleOutThrottle; };
  void setDInThrottleOutThrottle(double DInThrottleOutThrottle) { _DInThrottleOutThrottle = DInThrottleOutThrottle; };
  double DInThrottleOutThrottle() { return _DInThrottleOutThrottle; };

  void setPTrustInThrottle(double PTrustInThrottle) { _PTrustInThrottle = PTrustInThrottle; };
  double PTrustInThrottle() { return _PTrustInThrottle; };
  void setITrustInThrottle(double ITrustInThrottle) { _ITrustInThrottle = ITrustInThrottle; };
  double ITrustInThrottle() { return _ITrustInThrottle; };
  void setDTrustInThrottle(double DTrustInThrottle) { _DTrustInThrottle = DTrustInThrottle; };
  double DTrustInThrottle() { return _DTrustInThrottle; };

  void setPInAileronOutAileron(double PInAileronOutAileron) { _PInAileronOutAileron = PInAileronOutAileron; };
  double PInAileronOutAileron() { return _PInAileronOutAileron; };
  void setIInAileronOutAileron(double IInAileronOutAileron) { _IInAileronOutAileron = IInAileronOutAileron; };
  double IInAileronOutAileron() { return _IInAileronOutAileron; };
  void setDInAileronOutAileron(double DInAileronOutAileron) { _DInAileronOutAileron = DInAileronOutAileron; };
  double DInAileronOutAileron() { return _DInAileronOutAileron; };

  void setPInElevatorOutElevator(double PInElevatorOutElevator) { _PInElevatorOutElevator = PInElevatorOutElevator; };
  double PInElevatorOutElevator() { return _PInElevatorOutElevator; };
  void setIInElevatorOutElevator(double IInElevatorOutElevator) { _IInElevatorOutElevator = IInElevatorOutElevator; };
  double IInElevatorOutElevator() { return _IInElevatorOutElevator; };
  void setDInElevatorOutElevator(double DInElevatorOutElevator) { _DInElevatorOutElevator = DInElevatorOutElevator; };
  double DInElevatorOutElevator() { return _DInElevatorOutElevator; };

  void setPInRudderOutRudder(double PInRudderOutRudder) { _PInRudderOutRudder = PInRudderOutRudder; };
  double PInRudderOutRudder() { return _PInRudderOutRudder; };
  void setIInRudderOutRudder(double IInRudderOutRudder) { _IInRudderOutRudder = IInRudderOutRudder; };
  double IInRudderOutRudder() { return _IInRudderOutRudder; };
  void setDInRudderOutRudder(double DInRudderOutRudder) { _DInRudderOutRudder = DInRudderOutRudder; };
  double DInRudderOutRudder() { return _DInRudderOutRudder; };

  void setPAccselerationTrust(double PAccselerationTrust) { _PAccselerationTrust = PAccselerationTrust; };
  double PAccselerationTrust() { return _PAccselerationTrust; };
  void setIAccselerationTrust(double IAccselerationTrust) { _IAccselerationTrust = IAccselerationTrust; };
  double IAccselerationTrust() { return _IAccselerationTrust; };
  void setDAccselerationTrust(double DAccselerationTrust) { _DAccselerationTrust = DAccselerationTrust; };
  double DAccselerationTrust() { return _DAccselerationTrust; };

  void setPGSpeedAccseleration(double PGSpeedAccseleration) { _PGSpeedAccseleration = PGSpeedAccseleration; };
  double PGSpeedAccseleration() { return _PGSpeedAccseleration; };
  void setIGSpeedAccseleration(double IGSpeedAccseleration) { _IGSpeedAccseleration = IGSpeedAccseleration; };
  double IGSpeedAccseleration() { return _IGSpeedAccseleration; };
  void setDGSpeedAccseleration(double DGSpeedAccseleration) { _DGSpeedAccseleration = DGSpeedAccseleration; };
  double DGSpeedAccseleration() { return _DGSpeedAccseleration; };

  void setPGSpeedTrust(double PGSpeedTrust) { _PGSpeedTrust = PGSpeedTrust; };
  double PGSpeedTrust() { return _PGSpeedTrust; };
  void setIGSpeedTrust(double IGSpeedTrust) { _IGSpeedTrust = IGSpeedTrust; };
  double IGSpeedTrust() { return _IGSpeedTrust; };
  void setDGSpeedTrust(double DGSpeedTrust) { _DGSpeedTrust = DGSpeedTrust; };
  double DGSpeedTrust() { return _DGSpeedTrust; };
  void setNGSpeedTrust(double NGSpeedTrust) { _NGSpeedTrust = NGSpeedTrust; };
  double NGSpeedTrust() { return _NGSpeedTrust; };

private:
    
  int dTh = -13130;
  int TTAS = 3;
  int ABVH = 13;
  int EPVA = 14;
  int RVH = 15;
  bool _sourceThrottle = true;
  double _inThrottle = 0;
  bool _sourceTrust = true;
  double _Trust = 0;
  double _CurrTrust = 0;
  bool _sourceGSpeed = true;
  double _GSpeed = 0;
  double _CurrGSpeed = 0;
  bool _sourceAccseleration = true;
  double _Accseleration = 0;
  double _CurrAccseleration = 0;
  double _outThrottle = 0;

  double _sourceAileron = true;
  double _sourceElevator = true;
  double _sourceRudder = true;

  double _inAileron = 0;
  double _outAileron = 0;
  double _inElevator = 0;
  double _outElevator = 0;
  double _inRudder = 0;
  double _outRudder = 0;

  double _PInThrottleOutThrottle = 0;
  double _IInThrottleOutThrottle = 4;
  double _DInThrottleOutThrottle = 0;

  double _PInAileronOutAileron = 0;
  double _IInAileronOutAileron = 4;
  double _DInAileronOutAileron = 0;

  double _PInElevatorOutElevator = 0;
  double _IInElevatorOutElevator = 4;
  double _DInElevatorOutElevator = 0;

  double _PInRudderOutRudder = 0;
  double _IInRudderOutRudder = 4;
  double _DInRudderOutRudder = 0;

  double _PTrustInThrottle = 0;
  double _ITrustInThrottle = 0;
  double _DTrustInThrottle = 0;

  double _PAccselerationTrust = 1;
  double _IAccselerationTrust = 0.3;
  double _DAccselerationTrust = 0;

  double _PGSpeedTrust = 10;
  double _IGSpeedTrust = 1;
  double _DGSpeedTrust = 0.3;
  double _NGSpeedTrust = 10;

  double _PGSpeedAccseleration = 2.5;
  double _IGSpeedAccseleration = 0.1;
  double _DGSpeedAccseleration = 0.5;

  HANDLE HSimConnect;
  
  bool GetDataChanged = false;
  bool SetDataChanged = false;
  bool SetGetDataChanged = false;
  bool SetSetDataChanged = false;
  

  


  

  

  void ThrottleSet(double pos, DWORD speed = 10);
  void AileronSet(double pos, DWORD speed);
  void ElevatorSet(double pos, DWORD speed);
  void RudderSet(double pos, DWORD speed);
  void TrustSet(double pos, DWORD speed);
  void AccselerationSet(double pos, DWORD speed);
  void throttleSet(DWORD position);
  void throttleLever(int position);
  void setThrust(FLOAT thrust);
  void GSpeedSet(double speed, double Tr = 0);
  double lagfilter(double n, double dt, double* time_constant, double* prev_in, double* prev_out);
  double iterate(double i, double rate, double dt, double* y_slope, double in_[], double out[]);
  double constrain(double val, double min, double max);
  double computePID(double input, double setpoint, double kp, double ki, double kd, double dt, double minOut, double maxOut, double* integral, double* prevErr);
  double newPID(double value, double setpoint, double kp, double ki, double kd, double n, double dt, double minOut, double maxOut, double y[], double e[]);
  double newPID(double input, double setpoint, double kp, double ki, double kd, double n, double dt, double minOut, double maxOut, double* y[3], double* e[3]);
  void RudderTrimSet(double pos, DWORD speed = 10);
  void PitchTrimSet(double deg, DWORD speed = 10);

  //double GetData(DWORD var, char* unit = "");
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



public slots:
  void SetDataChange(DWORD var, DWORD sender); //Direct
  void GetDataChange(DWORD var, DWORD sender); //Direct
  void SetGetDataChange(DWORD var1, DWORD var2, DWORD sender); //Direct
  void SetSetDataChange(DWORD var1, DWORD var2, DWORD sender); //Direct
  void TimerProc();
  void ReceiveCommand(DWORD command, double parameter1, double parameter2);
  void PushbackSpeedSet(double setSpeed, DWORD speed);
};

