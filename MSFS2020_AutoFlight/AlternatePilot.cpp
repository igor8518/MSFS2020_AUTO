/*
#include <Windows.h>
#include "PMDG_NGX.h"
namespace PMDG_TEST
{

	double AltBankWithPos(double TargetValue) {
		aircraftData* PaircraftData = &AircraftData;
		double pTerm =0, dTerm=0, iTerm=0;
		double error = TargetValue - PaircraftData->bank;
		pTerm = pidBank.pGain * error;    // calculate the proportional term
		pidBank.iState += error;          // calculate the integral state with appropriate limiting
    if (pidBank.iState > pidBank.iMax) {
      pidBank.iState = pidBank.iMax;
    }
    else if (pidBank.iState < pidBank.iMin) {
      pidBank.iState = pidBank.iMin;
    }
		iTerm = pidBank.iGain * pidBank.iState;    // calculate the integral term
		dTerm = pidBank.dGain * (PaircraftData->bank - pidBank.dState);
		pidBank.dState = PaircraftData->bank;
		double ailer = pTerm + iTerm - dTerm;
		if (ailer > (16383 / 4.16666666666666667)) {
			ailer = 16383 / 4.16666666666666667;
		}
		else if (ailer < (-16383 / 4.16666666666666667)) {
			ailer = -16383 / 4.16666666666666667;
		}
		double intpParameter = -ailer / 16383.0;
		//SimConnect_SetDataOnSimObject(hSimConnect, DEF_AILERON, 0, 0, 0, sizeof(intpParameter), &intpParameter);
		ExSimConnect_TransmitClientEvent(hSimConnect, 0, KEY_AXIS_AILERONS_SET, -intpParameter * 16383, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
		pidBank.Term = ailer / 16383.0*30.0 * 4.16666666666666667;
		pidBank.val = PaircraftData->bank;
		pidBank.inVal = TargetValue;
		return (pTerm + iTerm - dTerm);
	}




	double AltPitchWithPos(double TargetValue) {
		aircraftData* PaircraftData = &AircraftData;
		alterPid lp = PPID;
		if (lp.FT) {
			lp.ePnm1 = 0.0;
			lp.eDfnm2 = lp.eDfnm1 = 0.0;
			lp.Unm1 = PaircraftData->elev;
			lp.FT = false;
			TargetValue = PaircraftData->pitch;
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
		double yn = PaircraftData->pitch;
		lp.ePn = lp.b*rn - yn;
		double en = rn - yn;
		lp.eDfn = 0.0;
		if (lp.Td > 0) {
			double eDn = lp.g*rn - yn;
			double Tf = lp.a*lp.Td;
			lp.eDfn = lp.eDfnm1 / (Ta / Tf + 1) + eDn*(Ta / Tf) / (Ta / Tf + 1);
		}
		else {
			lp.eDfnm2 = lp.eDfnm1 = lp.eDfn = 0.0;
		}
		double DUn = 0.0;
		if (lp.Ti > 0.0) {
			DUn = lp.Kp * ((lp.ePn - lp.ePnm1) + ((Ta / lp.Ti)*en) + ((lp.Td / Ta)*(lp.eDfn - 2 * lp.eDfnm1 + lp.eDfnm2)));
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
		//SimConnect_SetDataOnSimObject(hSimConnect, DEF_ELEVATOR, 0, 0, 0, sizeof(intpParameter), &intpParameter);
		ExSimConnect_TransmitClientEvent(hSimConnect, 0, KEY_AXIS_ELEVATOR_SET, -intpParameter * 16383, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
		pidPitch.Term = -Un*30.0*0.5;
		pidPitch.val = PaircraftData->pitch;
		pidPitch.inVal = -TargetValue;
		PPID = lp;
		return (Un);
	}




	double AltVSpeedWithPitch(double TargetValue) {
		aircraftData* PaircraftData = &AircraftData;
		alterPid lp = VSPID;
		if (lp.FT) {
			lp.ePnm1 = 0.0;
			lp.eDfnm2 = lp.eDfnm1 = 0.0;
			lp.Unm1 = PaircraftData->pitch;
			lp.FT = false;
		}
		lp.Kp = pidVSpeed.pGain;
		lp.Ti = pidVSpeed.iGain;
		lp.Td = pidVSpeed.dGain;
		clock_t eTime = clock();
		double Ta = (double)(eTime - lp.time)/1000.0;
		lp.time = eTime;
		if (Ta <= 0.00833333) {
			return -1.0;
		}
		double rn = TargetValue;
		double yn = PaircraftData->VS*60;
		lp.ePn = lp.b*rn - yn;
		double en = rn - yn;
		lp.eDfn = 0.0;
		if (lp.Td > 0) {
			double eDn = lp.g*rn - yn;
			double Tf = lp.a*lp.Td;
			lp.eDfn = lp.eDfnm1 / (Ta / Tf + 1) + eDn*(Ta / Tf) / (Ta / Tf + 1);
		}
		else {
			lp.eDfnm2 = lp.eDfnm1 = lp.eDfn = 0.0;
		}
		double DUn = 0.0;
		if (lp.Ti > 0.0) {
			DUn = lp.Kp * ((lp.ePn - lp.ePnm1) + ((Ta / lp.Ti)*en) + ((lp.Td / Ta)*(lp.eDfn - 2 * lp.eDfnm1 + lp.eDfnm2)));
		}
		if (DUn > (lp.uMax - lp.Unm1)) {
			DUn = (lp.uMax - lp.Unm1);
		}
		else if (DUn < (lp.uMin - lp.Unm1)) {
			DUn = (lp.uMin - lp.Unm1);
		}
		if (DUn > 0.1) {
			DUn = 0.1;
		}
		else if (DUn < -0.1) {
			DUn = -0.1;
		}
		lp.eDfnm2 = lp.eDfnm1;
		lp.eDfnm1 = lp.eDfn;
		lp.ePnm1 = lp.ePn;
		double Un = lp.Unm1 + DUn;
		lp.Unm1 = Un;
		lp.ePnm1 = lp.ePn;
		lp.eDfnm2 = lp.eDfnm1;
		lp.eDfnm1 = lp.eDfn;
		double intpParameter = Un;
		AltPitchWithPos(intpParameter);
		pidVSpeed.Term = Un / 20 * 30;
		pidVSpeed.val = PaircraftData->VS*60.0/100;
		pidVSpeed.inVal = TargetValue;
		VSPID = lp;
		return (Un);
	}
}
*/