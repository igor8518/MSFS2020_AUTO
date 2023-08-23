#include "PlanesWork.h"

PlanesWork::PlanesWork(HANDLE hSimConnect) : HSimConnect(hSimConnect) {
	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(TimerProc()));
	DataT = NULL;
	//Timer->start(60);
}

void PlanesWork::TimerProc() {

	_CurrTrust = DataT->GData.ENG_N1_RPM1;
	_CurrAccseleration = DataT->GData.ACCELERATION_BODY_Z * 1.6878098571011956838728492271799;
	_CurrGSpeed = DataT->GData.GROUND_VELOCITY;
	GSpeedSet(0);
	//AccselerationSet(0, 10);
	TrustSet(0, 10);
	if (DataT->GData.INDICATED_ALTITUDE < 3000) {
		ThrottleSet(0, 10);
	}
	if ((DataT == NULL) || (!DataT->AllData.A32NX_AUTOPILOT_ACTIVE)) {
		AileronSet(0, 10);
		ElevatorSet(0, 10);
		RudderSet(0, 10);
	}
}

void PlanesWork::ReceiveCommand(DWORD command, double parameter1, double parameter2) {
	//if ((DataT == NULL) || (!DataT->AllData.A32NX_AUTOPILOT_ACTIVE)) {
		switch (command) {
		case SET_THROTTLE: {
			if (_sourceThrottle) {
				_inThrottle = parameter1;
				TTAS = SET_THROTTLE;
			}
			break;
		}
		case SET_TRUST: {
			if (_sourceTrust) {
				_Trust = parameter1;
				TTAS = SET_TRUST;
			}
			break;
		}
		case SET_ACCSELERATION: {
			if (_sourceAccseleration) {
				_Accseleration = parameter1;
				TTAS = SET_ACCSELERATION;
			}
			break;
		}
		case SET_GSPEED: {
			if (_sourceGSpeed) {
				_GSpeed = parameter1;
				TTAS = SET_GSPEED;
			}
			break;
		}
					   //if ((DataT == NULL) || (!DataT->AllData.A32NX_AUTOPILOT_ACTIVE)) {
		case SET_RUDDER_TRIM: {
			RudderTrimSet(parameter1, parameter2);
			break;
		}
		case SET_PITCH_TRIM: {
			PitchTrimSet(parameter1, parameter2);
			break;
		}
		case SET_PUSHBACK_SPEED: {
			PushbackSpeedSet(-parameter1, parameter2);
			break;
		}
		case START_TIMER: {
			Timer->start(60);
			break;
		}
		case SET_AILERON: {
			if (_sourceAileron) {
				_inAileron = parameter1;
				ABVH = SET_AILERON;
			}
			break;
		}
		case SET_ELEVATOR: {
			if (_sourceElevator) {
				_inElevator = parameter1;
				EPVA = SET_ELEVATOR;
			}
			break;
		}
		case SET_RUDDER: {
			if (_sourceRudder) {
				_inRudder = parameter1;
				RVH = SET_RUDDER;
			}
			break;
		}
		}
	
}

void PlanesWork::PushbackSpeedSet(double setSpeed, DWORD speed) { // Transfer to mainlogic and logarifmic set

	double startSpeed = DataT->GData.VELOCITY_BODY_Z;
	double adjustedSpeed = setSpeed;
	double speeds;
	for (int i = speed - 1; i > 0; i--)
	{
		speeds = adjustedSpeed + (startSpeed - adjustedSpeed) * i / speed;
		Sleep(1000 / speed);
		SetData(VELOCITY_BODY_Z, speeds);
	}
	speeds = adjustedSpeed;
	SetData(VELOCITY_BODY_Z, speeds);
}

double PlanesWork::PID(double dt, double err, double kp, double ki, double kd, double bi, double bd, double* prevErr, double* integral) {

	double P = err * kp;
	*integral = constrain(*integral + (err * dt * ki), -bi, bi);
	double I = *integral;
	double D = constrain(((err - *prevErr) / dt) * kd, -bd, bd);
	*prevErr = err;
	return P + I + D;
}

void PlanesWork::ThrottleSet(double pos, DWORD speed) { // Transfer to mainlogic and logarifmic set
	if (TTAS >= SET_THROTTLE) {
		static double  time_constant = 1.0;
		static double  prev_in = 0.0;
		static double  prev_out = 0.0;
		static QDateTime Start = QDateTime::currentDateTime();
		double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
		Start = QDateTime::currentDateTime();
		double CurrSpeed = DataT->GData.GROUND_VELOCITY;
		double SpeedRel = _GSpeed - CurrSpeed;
		static double integral = 0, prevErr = 0;
	
		static double throttle = _inThrottle;
		if (dt != 0) {
			//_outThrottle = computePID(throttle, _inThrottle, _PInThrottleOutThrottle, _IInThrottleOutThrottle, _DInThrottleOutThrottle, dt, -16383 - dTh, 16383 - dTh, &integral, &prevErr);
			_outThrottle += PID(dt, SpeedRel, _PInThrottleOutThrottle*5, _IInThrottleOutThrottle * 5, _DInThrottleOutThrottle * 5, _IBInThrottleOutThrottle, _DBInThrottleOutThrottle, &prevErr, &integral) * 1;
		}
		else {
			_outThrottle = _inThrottle;
		}
		throttle = _outThrottle;
		_outThrottle = _inThrottle;
		double initParameter;
		double brakeParameter;
		//_outThrottle = constrain(_outThrottle, -16383, 16383);
		//initParameter = constrain(_outThrottle, -16383 - dTh, 16383 - dTh); //-13130
		initParameter = constrain(_outThrottle, -14000, 16000); //-13130
		brakeParameter = constrain(-_outThrottle, 0, 16383);
		//_outThrottle = lagfilter(initParameter, dt, &time_constant, &prev_in, &prev_out);
		//emit SendEvent(KEY_THROTTLE_SET, int(_outThrottle));
		emit SendEvent(KEY_THROTTLE_SET, int(initParameter));
		//emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, brakeParameter * 2 - 16383);
		//emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, brakeParameter * 2 - 16383);
		//emit SendEvent(KEY_THROTTLE_SET, -2500);
		//emit SendEvent(KEY_TH, -16383);
		//SetDataL(A32NX_3D_THROTTLE_LEVER_POSITION_1, ((_outThrottle / 32768) + 0.5)*100);
		//SetDataL(A32NX_3D_THROTTLE_LEVER_POSITION_2, ((_outThrottle / 32768) + 0.5)*100);

	}
}

void PlanesWork::AileronSet(double pos, DWORD speed) { // Transfer to mainlogic and logarifmic set
	if (ABVH >= SET_AILERON) {
		static double integral = 0, prevErr = 0;
		static QDateTime Start = QDateTime::currentDateTime();
		qint64 y, z;
		double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
		//double dt = 0.2;
		Start = QDateTime::currentDateTime();
		static double aileron = _inAileron;
		if (dt != 0) {
			_outAileron= computePID(aileron, _inAileron, _PInAileronOutAileron, _IInAileronOutAileron, _DInAileronOutAileron, dt, -16383, 16383, &integral, &prevErr);
		}
		else {
			_outAileron = _inAileron;
		}
		aileron = _outAileron;
		//_outThrottle = _inThrottle;
		emit SendEvent(KEY_AXIS_AILERONS_SET, int(_outAileron));
		//SetDataL(A32NX_SIDESTICK_POSITION_X, _outAileron / 16383);
	}
}
void PlanesWork::ElevatorSet(double pos, DWORD speed) { // Transfer to mainlogic and logarifmic set
	if (EPVA >= SET_ELEVATOR) {
		static double integral = 0, prevErr = 0;
		static QDateTime Start = QDateTime::currentDateTime();
		qint64 y, z;
		double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
		//double dt = 0.2;
		Start = QDateTime::currentDateTime();
		static double elevator = _inElevator;
		if (dt == 0.05) {
			_outElevator = computePID(elevator, _inElevator, _PInElevatorOutElevator, _IInElevatorOutElevator, _DInElevatorOutElevator, dt, -16383, 16383, &integral, &prevErr);
		}
		else {
			_outElevator = _inElevator;
		}
		elevator = _outElevator;
		//_outThrottle = _inThrottle;
		emit SendEvent(KEY_AXIS_ELEVATOR_SET, int(_outElevator));
		//SetDataL(A32NX_SIDESTICK_POSITION_Y, _outElevator / 16383);
	}
}
void PlanesWork::RudderSet(double pos, DWORD speed) { // Transfer to mainlogic and logarifmic set
	if (RVH >= SET_RUDDER) {
		static double integral = 0, prevErr = 0;
		static QDateTime Start = QDateTime::currentDateTime();
		qint64 y, z;
		double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
		//double dt = 0.2;
		Start = QDateTime::currentDateTime();
		static double rudder = _inRudder;
		if (dt != 0) {
			if (DataT->GData.PLANE_ALT_ABOVE_GROUND < 500) {
				_outRudder = computePID(rudder, _inRudder, _PInRudderOutRudder, _IInRudderOutRudder, _DInRudderOutRudder, dt, -16383, 16383, &integral, &prevErr);
			}
			else {
				_outRudder = 0;
			}
		}
		else {
			//_outRudder = _inRudder;
		}
		rudder = _outRudder;
		//_outThrottle = _inThrottle;
		if (DataT->GData.INDICATED_ALTITUDE < 3000) {
			emit SendEvent(KEY_RUDDER_SET, int(_outRudder));
		}
	}
}
void PlanesWork::TrustSet(double pos, DWORD speed) { // Transfer to mainlogic and logarifmic set
	if (TTAS >= SET_TRUST) {
		
		/*static double integral = 0, prevErr = 0;
		static QDateTime Start = QDateTime::currentDateTime();
		qint64 y, z;
		double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
		//double dt = 0.2;
		Start = QDateTime::currentDateTime();
		static double trust = _CurrTrust;
		if (dt != 0) {
			_inThrottle = computePID(trust, _Trust, _PTrustInThrottle, _ITrustInThrottle, _DTrustInThrottle, dt, 0, 1, &integral, &prevErr) * 16383;
		}
		else {
			_inThrottle = 0;
		}
		trust = _CurrTrust;*/
		_inThrottle = _Trust * 163.83;
		if (_Trust < 0) {
			_inThrottle = 0;
		}
		
		//_outThrottle = _inThrottle;
		//emit SendEvent(KEY_THROTTLE_SET, int(_outThrottle));
	}
}
void PlanesWork::AccselerationSet(double pos, DWORD speed) { // Transfer to mainlogic and logarifmic set
	if (TTAS >= SET_ACCSELERATION) {
		static double integral = 0, prevErr = 0;
		static QDateTime Start = QDateTime::currentDateTime();
		qint64 y, z;
		double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
		//double dt = 0.2;
		Start = QDateTime::currentDateTime();
		static double Accseleration = _CurrAccseleration;
		if (dt != 0) {
			_Trust = computePID(Accseleration, _Accseleration, _PAccselerationTrust, _IAccselerationTrust, _DAccselerationTrust, dt, -20, 10, &integral, &prevErr) / 10;
			if (_Trust < 5.15) {
				emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, int((int((_Trust + 20.0) * -32767) - 16383) * 0.5));
				emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, int((int((_Trust + 20.0) * -32767) - 16383) * 0.5));
				if (_Trust < 0) {
					_Trust = 0;
				}
			}
			else {
				emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, -16383);
				emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, -16383);
			}
		}
		else {
			//_inThrottle = 0;
		}
	Accseleration = _CurrAccseleration;
		//_outThrottle = _inThrottle;
		//emit SendEvent(KEY_THROTTLE_SET, int(_outThrottle));
	}
}








void PlanesWork::throttleLever(int position) {
	_Trust = (position / 163.83 * 100);
	//_inThrottle = position * 100;
	//ThrottleSet(position * 100);
	/*throttle.L = (double)position;
	throttle.R = (double)position;
	SimConnect_SetDataOnSimObject(hSimConnect, DEF_THROTTLE, 0, 0, 0, sizeof(tthrottle), &throttle);*/
}




void PlanesWork::setThrust(FLOAT thrust) {
	double N1L = DataT->GData.TURB_ENG_N11;
	static clock_t startTime;
	static double throttle = 0;
	static double startThrust = N1L;
	clock_t endTime = clock() + 1;
	double endThrust = N1L;
	double timeOff = endTime - startTime;
	double thrustOff = endThrust - startThrust;
	double thrustA = (timeOff / 1000) * thrustOff;
	double dT = thrust - endThrust;
	double dA = (dT / 5) - thrustA;
	double dThrottle = dA;
	if (dThrottle < -5) {
		dThrottle = -5;
	}
	else if (dThrottle > 5) {
		dThrottle = 5;
	}
	throttle = throttle + dThrottle;
	bool thrustSet = FALSE;
	if (throttle < 0) {
		throttle = 0;
	}
	else if (throttle > 15) {
		throttle = 15;
	}
	throttleLever(throttle);
	thrustSet = TRUE;
	startTime = clock();
	startThrust = N1L;
}







void PlanesWork::GSpeedSet(double pos, double Tr) {
	if (TTAS >= SET_GSPEED) {
		double speed = 0;
		if (DataT->GData.ATC_CLEARED_TAXI == 1 || DataT->GData.ATC_CLEARED_TAKEOF == 1 || DataT->GData.ATC_CLEARED_LANDING == 1) {
			speed = _GSpeed;
		}
		else {
			speed = -5;
		}
		double brakes;
		double N1L = DataT->GData.TURB_ENG_N11;
		double GS = DataT->GData.GROUND_VELOCITY;
		static clock_t startTime;
		static double thrust = N1L;
		static double throttle = 0;
		static double startSpeed = GS;
		clock_t endTime = clock() + 1;
		double endSpeed = GS;
		double timeOff = endTime - startTime;
		double speedOff = endSpeed - startSpeed;
		double speedA = (timeOff / 1000) * speedOff;
		double dS = speed - endSpeed;
		double dA = (dS / 5) - speedA;
		double dThrust = dA;


		if (dThrust < -5) {
			dThrust = -5;
		}
		else if (dThrust > 5) {
			dThrust = 5;
		}
		if ((speedA < -0.3) && (speedA > -0.4)) {
			dThrust = dThrust + 2;
		}
		else if ((speedA > 0.3) && (speedA < 0.4)) {
			dThrust = dThrust - 2;
		}
		if (speedA < -0.4) {
			dThrust = dThrust + 5;
		}
		else if (speedA >= 0.4) {
			dThrust = dThrust - 5;
		}
		thrust = N1L + dThrust;
		bool thrustSet = FALSE;
		if (thrust < 21) {
			thrust = 21;
		}
		else if (thrust > 100) {
			thrust = 100;
		}
		if (_GSpeed <= 0) {
			_Trust = 0;
		}
		setThrust(thrust);
		thrustSet = TRUE;
		startTime = clock();
		startSpeed = GS;

		if (GS > (speed + 2)) {
			double brake = (GS - speed) / 70.0;
			setThrust(0);
			_Trust = 0;
			brakes = brake;
		}
		else if (GS < (speed)) {
			brakes = 0.0;
		}
		if (_GSpeed >= 0) {
			emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, brakes * 32768 - 16383);
			emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, brakes * 32768 - 16383);
		}
		else {
			emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, (brakes+0.05) * 32768 - 16383);
			emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, (brakes+0.05) * 32768 - 16383);
		}
	}
}
/*void PlanesWork::GSpeedSet(double pos, DWORD speed) { // Transfer to mainlogic and logarifmic set
	if (TTAS >= SET_GSPEED) {
		static double integral = 0, prevErr = 0;
		static double yy[] = { 0, 0, 0 };
		static double e[] = { 0, 0, 0 };

		
		static double y_slope = 0.0;
		static double in_[] = { 0, 0 };
		static double out[] = { 0, 0 };

		
		
		static QDateTime Start = QDateTime::currentDateTime();
		qint64 y, z;
		double dt = (double)Start.msecsTo(QDateTime::currentDateTime()) / 1000;
		//double dt = 0.2;
		Start = QDateTime::currentDateTime();
		static double GSpeed = _CurrGSpeed;
		if (dt != 0) {
			if (_GSpeed > 35) {
				//_Trust = newPID(GSpeed, _GSpeed, _PGSpeedTrust, _IGSpeedTrust, _DGSpeedTrust, _NGSpeedTrust, dt, 0, 100, yy, e);
				_Trust = computePID(GSpeed, _GSpeed, _PGSpeedTrust, _IGSpeedTrust, _DGSpeedTrust, dt, 0, 100, &integral, &prevErr);
			}
			else {
				//_Trust = newPID(GSpeed, _GSpeed, _PGSpeedTrust, _IGSpeedTrust, _DGSpeedTrust, _NGSpeedTrust, dt, -100, 60, yy, e);
				_Trust = computePID(GSpeed, _GSpeed, _PGSpeedTrust, _IGSpeedTrust, _DGSpeedTrust, dt, -20, 40, &integral, &prevErr);
			}
			//_Trust = iterate(_Trust, 10, dt, &y_slope, in_, out);
			if (_Trust < 0) {
				emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, int((int((_Trust + 20.0) * -32767) - 16383) * 0.5));
				emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, int((int((_Trust + 20.0) * -32767) - 16383) * 0.5));
				//emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, int((((_Trust + 0.0) * -327.67) - 16383)*0.5));
				//emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, int((((_Trust + 0.0) * -327.67) - 16383)*0.5));
				/*if (_Trust < 0) {
					_Trust = 0;
				}*/
			/*}
			else {
				emit SendEvent(KEY_AXIS_LEFT_BRAKE_SET, -16383);
				emit SendEvent(KEY_AXIS_RIGHT_BRAKE_SET, -16383);
			}
		}
		else {
			//_inThrottle = 0;
		}
		GSpeed = _CurrGSpeed;
		//_outThrottle = _inThrottle;
		//emit SendEvent(KEY_THROTTLE_SET, int(_outThrottle));
	}
}*/
double PlanesWork::lagfilter(double n, double dt, double* time_constant, double* prev_in, double* prev_out) {
	double sdt = dt * *time_constant;
	double sum0 = sdt + 2.0;

	double output = (n + *prev_in) * sdt / sum0 + (2.0 - sdt) / sum0 * *prev_out;
	*prev_in = n;
	*prev_out = output;

	return output;
}
double PlanesWork::iterate(double i, double rate, double dt, double* y_slope, double in_[], double out[]) {
	in_[0] = i;

	double slope = (in_[0] - out[1]) / dt;

	if (slope <= -rate) {
			*y_slope = -rate;
	}

		if ((-rate <= slope) && (slope <= rate)) {
				*y_slope = slope;
		}

			if (rate <= slope) {
					*y_slope = rate;
			}

		out[0] = out[1] + (dt * *y_slope);
		out[1] = out[0];
		in_[1] = in_[0];

		return out[0];
}
double PlanesWork::constrain(double val, double min, double max) {
	if (val > max) {
		val = max;
	} 
	else if (val < min) {
		val = min;
	}
	return val;
}

double PlanesWork::computePID(double input, double setpoint, double kp, double ki, double kd, double dt, double minOut, double maxOut, double* integral, double* prevErr) {
	double err = setpoint - input;
	
	*integral = constrain(*integral + (double)err * dt * ki, minOut, maxOut);
	double D = (err - *prevErr) / dt;
	*prevErr = err;
	return constrain(err * kp + *integral + D * kd, minOut, maxOut);
}

double PlanesWork::newPID(double value, double setpoint, double kp, double ki, double kd, double n, double dt, double minOut, double maxOut, double y[], double e[]) {
	double k = 2 / dt;
	double k2 = pow(k, 2.0);

	double b0 = k2 * kp
		+ k * ki
		+ ki * n
		+ k * kp * n
		+ k2 * kd * n;
	double b1 = 2.0 * ki * n - 2.0 * k2 * kp - 2.0 * k2 * kd * n;
	double b2 = k2 * kp - k * ki + ki * n - k * kp * n
		+ k2 * kd * n;
	double a0 = k2 + n * k;
	double a1 = -2.0 * k2;
	double a2 = k2 - k * n;

	e[2] = e[1];
	e[1] = e[0];
	e[0] = setpoint - value;
	y[2] = y[1];
	y[1] = y[0];
	y[0] = constrain(-a1 / a0 * y[1] - a2 / a0 * y[2] + b0 / a0 * e[0] + b1 / a0 * e[1] + b2 / a0 * e[2], minOut, maxOut);

	return  y[0];
}

void PlanesWork::RudderTrimSet(double deg, DWORD speed) { // Transfer to mainlogic and logarifmic set

	double startRudderTrim = DataT->GData.RUDDER_TRIM;
	double adjustedRudderTrim = deg;
	double RudderTrim = startRudderTrim;
	while (abs(RudderTrim - adjustedRudderTrim) > 0.1)
	{
		if ((RudderTrim - adjustedRudderTrim) / abs(RudderTrim - adjustedRudderTrim) > 0) {
			
			emit SendEvent(KEY_RUDDER_TRIM_LEFT, 0);
		}
		else {
			emit SendEvent(KEY_RUDDER_TRIM_RIGHT, 0);
		}
		Sleep(1000 / speed);
		RudderTrim = DataT->GData.RUDDER_TRIM;
	}
	RudderTrim = adjustedRudderTrim;
	emit SendEvent(KEY_RUDDER_TRIM_SET, int((RudderTrim)/ 20 * 16383));
}
void PlanesWork::PitchTrimSet(double deg, DWORD speed) { // Transfer to mainlogic and logarifmic set

	double startElevatorTrim = DataT->GData.ELEVATOR_TRIM_POSITION;
	double adjustedElevatorTrim = deg;
	double ElevatorTrim = startElevatorTrim;
	while (abs(ElevatorTrim - adjustedElevatorTrim) > 0.1)
	{
		if ((ElevatorTrim - adjustedElevatorTrim) / abs(ElevatorTrim - adjustedElevatorTrim) > 0) {

			emit SendEvent(KEY_ELEV_TRIM_DN, 0);
		}
		else {
			emit SendEvent(KEY_ELEV_TRIM_UP, 0);
		}
		Sleep(1000 / speed);
		ElevatorTrim = DataT->GData.ELEVATOR_TRIM_POSITION;
	}
	ElevatorTrim = adjustedElevatorTrim;
	emit SendEvent(KEY_ELEVATOR_TRIM_SET, int((ElevatorTrim + 4) / 17.5 * 16383));
}

/*double PlanesWork::GetData(DWORD var, const char* unit) {
	double lVar;
	emit GetDataSignal(PLANESWORK_ID, var, &lVar, unit);
	while (!GetDataChanged);
	GetDataChanged = false;
	return lVar;
}*/
double PlanesWork::GetDataL(DWORD var, const char* unit) {
	double lVar;
	emit GetDataSignalL(PLANESWORK_ID, var, &lVar, unit);
	while (!GetDataChanged);
	GetDataChanged = false;
	return lVar;
}

double PlanesWork::SetData(DWORD var, double val, const char* unit) {
	emit SetDataSignal(PLANESWORK_ID, var, &val, unit);
	while (!SetDataChanged);
	SetDataChanged = false;
	return  val;
}
double PlanesWork::SetDataL(DWORD var, double val, const char* unit) {
	emit SetDataSignalL(PLANESWORK_ID, var, &val, unit);
	while (!SetDataChanged);
	SetDataChanged = false;
	return  val;
}

/*double PlanesWork::SetGetData(DWORD var1, DWORD var2, double val, const char* unit) {
	double lVar;
	emit SetGetDataSignal(PLANESWORK_ID, var1, var2, &lVar, unit);
	while (!SetGetDataChanged);
	SetGetDataChanged = false;
	return  lVar;
}

double PlanesWork::SetSetData(DWORD var1, double val1, DWORD var2, double val2, char* unit1, char* unit2)
{
	emit SetSetDataSignal(PLANESWORK_ID, var1, &val1, var2, &val2, unit1, unit2);
	while (!SetSetDataChanged);
	SetSetDataChanged = false;
	return  val1;
}*/

DWORD PlanesWork::SendEvent(DWORD EventID, long dwData)
{
	SendEventSignal(PLANESWORK_ID, EventID, dwData);
	return 0;
	}
DWORD PlanesWork::SendEvent2(DWORD EventID, long dwData, DWORD var, double val, const char* unit)
{
	SendEventSignal2(PLANESWORK_ID, EventID, dwData, var, val, unit);
	return 0;
}


void PlanesWork::SetDataChange(DWORD var, DWORD sender) {
	if (sender == PLANESWORK_ID) {
		GetDataChanged = true;
	}
}
void PlanesWork::GetDataChange(DWORD var, DWORD sender) {
	if (sender == PLANESWORK_ID) {
		SetDataChanged = true;
	}
}
void PlanesWork::SetGetDataChange(DWORD var1, DWORD var2, DWORD sender) {
	if (sender == PLANESWORK_ID) {
		SetGetDataChanged = true;
	}
}

void PlanesWork::SetSetDataChange(DWORD var1, DWORD var2, DWORD sender)
{
	if (sender == PLANESWORK_ID) {
		SetSetDataChanged = true;
	}
}

