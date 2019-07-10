/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Emergency Detection System 602A5 (Header)

Project Apollo is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Project Apollo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Project Apollo; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

See http://nassp.sourceforge.net/license/ for more details.

**************************************************************************/

#pragma once

class IU;

class EDS
{
public:
	EDS(IU *iu);
	virtual ~EDS() {}
	virtual void Timestep(double simdt);
	void SetPlatformFailureParameters(bool PlatFail, double PlatFailTime);
	void SetLiftoffCircuitAFailure() { LiftoffCircuitAFailure = true; }
	void SetLiftoffCircuitBFailure() { LiftoffCircuitBFailure = true; }

	virtual void SaveState(FILEHANDLE scn, char *start_str, char *end_str) = 0;
	virtual void LoadState(FILEHANDLE scn, char *end_str) = 0;

	//Switch Selector Functions
	void SetSIVBEngineOutIndicationA(bool set) { SIVBEngineOutIndicationA = set; }
	void SetSIVBEngineOutIndicationB(bool set) { SIVBEngineOutIndicationB = set; }
	void SetLVEnginesCutoffEnable1() { if (LVEnginesCutoffCommand2 == false) { LVEnginesCutoffEnable1 = true; } }
	void SetSIVBEngineCutoffDisabled() { SIVBEngineCutoffDisabled = true; }
	void SetSIVBRestartAlert(bool set) { SIVBRestartAlert = set; }
	void EnableSCControl() { SCControlEnableRelay = true; }
	void DisableSCControl() { SCControlEnableRelay = false; }
	void EnableCommandSystem() { IUCommandSystemEnable = true; }

	//GSE Reset Buses
	virtual void ResetBus1();
	virtual void ResetBus2();

	//To spacecraft
	bool GetLiftoffCircuitA() { return !LiftoffCircuitAFailure && LiftoffA; }
	bool GetLiftoffCircuitB() { return !LiftoffCircuitBFailure && LiftoffB; }
	bool GetEDSAbort(int n);
	virtual double GetLVTankPressure(int n);
	bool GetAbortLightSignal() { return AbortLightSignal; }

	//To LVDA
	bool GetIULiftoff() { return LiftoffRelay; }
	virtual bool GetSIISIVBSepSeqStart() { return false; }

	//To FCC
	bool GetSCControl();

	//To Control Distributor
	bool GetIUCommandSystemEnable();

	//GSE
	bool GetLiftoffEnableA() { return EDSLiftoffEnableA; }
	bool GetLiftoffEnableB() { return EDSLiftoffEnableB; }
	bool GetAutoAbort() { return AutoAbortBus; }
	virtual bool GetAllSIEnginesRunning();
	virtual bool IsEDSUnsafe();
	bool GetLVEnginesCutoffFromSC1() { return LVEnginesCutoffFromSC1; }
	bool GetLVEnginesCutoffFromSC2() { return LVEnginesCutoffFromSC2; }
	bool GetLVEnginesCutoffFromSC3() { return LVEnginesCutoffFromSC3; }
	bool GetExcessiveRateIndication() { return ExcessivePitchYawRateIndication || ExcessiveRollRateIndication; }

	void SetEDSLiftoffEnableA() { EDSLiftoffEnableA = true; }
	void SetEDSLiftoffEnableB() { EDSLiftoffEnableB = true; }
	void LiftoffEnableReset() { EDSLiftoffEnableA = false; EDSLiftoffEnableB = false; }
protected:
	IU* iu;

	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);

	bool LVEnginesCutoffVote();

	//Buses:

	//+6D91-93
	bool IUEDSBusPowered;
	//+6D95
	bool AutoAbortBus;

	//Relays:
	
	//A4K1
	bool SCControlEnableRelay;
	//A4K6, A10K3, K55-1, K55-2
	bool LVAttRefFail;
	//A6K1, A7K1, A8K3 (K43-1 - K43-3)
	bool TwoEngOutAutoAbortDeactivate;
	//K46-K48 (K46-1 - K46-3)
	bool ExcessRatesAutoAbortDeactivatePY;
	//K52-K54 (K47-1 - K71-3)
	bool ExcessRatesAutoAbortDeactivateR;
	//K19 (K19-1)
	bool LVEnginesCutoffEnable1;
	//K29 (K19-2)
	bool LVEnginesCutoffEnable2;
	//K6 (K66)
	bool SIVBEngineOutIndicationA;
	//K91 (K167)
	bool SIVBEngineOutIndicationB;
	//K2 (K90)
	bool EDSLiftoffInhibitA;
	//K3 (K91)
	bool EDSLiftoffInhibitB;
	//K4 (K4)
	bool LiftoffRelay;
	//K40 (K20-1)
	bool LVEnginesCutoffFromSC1;
	//K41-1/2 (K20-2/4)
	bool LVEnginesCutoffFromSC2;
	//K42 (K20-3)
	bool LVEnginesCutoffFromSC3;
	//K51 (K51)
	bool SIVBEngineThrustMonitorA;
	//K63 (K134)
	bool SIVBEngineThrustMonitorB;
	//K65 (K92)
	bool EDSLiftoffEnableA;
	//K66 (K93)
	bool EDSLiftoffEnableB;
	//K76 (K232)
	bool SIVBRestartAlert;
	//K77-1/2
	bool IUCommandSystemEnable;
	//K9-1/2, (K9-1/2)
	bool LVEnginesCutoffCommand1;
	//K98, K99 (K230, K231)
	bool LVEnginesCutoffCommand2;
	//K86-1/2 (K10-1/2)
	bool LVEnginesCutoffCommand3;
	//K89 (K171)
	bool GSEEngineThrustIndicationEnableA;
	//K90 (K172)
	bool GSEEngineThrustIndicationEnableB;
	//K49 (K69)
	bool RangeSafetyDestructArmedAFromSIVB;
	//K50 (K70)
	bool RangeSafetyDestructArmedBFromSIVB;
	//A8K1 (K71)
	bool PadAbortRequest;
	//A5K3 (K60), A6K3 (K61), A10K1 (K220)
	bool ExcessiveRollRateIndication;
	//A5K2 (K62), A7K3 (K63), A5K1 (K64), A8K2 (K65), A4K5 (K221), A9K5 (K222)
	bool ExcessivePitchYawRateIndication;
	//A9K3 (K59)
	bool SIAllEnginesOKA;
	//A4K4 (K219)
	bool SIAllEnginesOKB;
	bool SIVBEngineCutoffDisabled;

	//Signals
	bool LiftoffA;
	bool LiftoffB;
	bool EDSAbortSignal1;
	bool EDSAbortSignal2;
	bool EDSAbortSignal3;
	bool AbortLightSignal;
	int LVRateAutoSwitch;
	int TwoEngineOutAutoSwitch;
	int Stage;
	VECTOR3 AttRate;
	bool EDSBus1Powered, EDSBus2Powered, EDSBus3Powered;
	bool BECOA, BECOB;

	//Other
	bool SIEDSCutoff;
	bool SIVBEDSCutoff;

	//Common Saturn Failures
	bool PlatformFailure;
	double PlatformFailureTime;
	bool LiftoffCircuitAFailure;
	bool LiftoffCircuitBFailure;
};

class EDS1B : public EDS
{
public:
	EDS1B(IU *iu);
	void Timestep(double simdt);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
protected:

	bool SIThrustNotOK[8];
	//Temporary variables, not relays
	bool ThrustOKSignal[8];
};

class EDSSV : public EDS
{
public:
	EDSSV(IU *iu);
	void Timestep(double simdt);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetSIIEngineOutIndicationA() { SIIEngineOutIndicationA = true; }
	void SetSIIEngineOutIndicationB() { SIIEngineOutIndicationB = true; }
	void SetUllageThrustIndicate(bool set) { UllageThrustIndicate = set; }

	double GetLVTankPressure(int n);
	bool GetSIISIVBSepSeqStart() { return SIISIVBSepSeqStart; }

	//GSE Reset Buses
	void ResetBus1();
	void ResetBus2();
protected:
	//K21-1-5, K22-1-5
	bool SIThrustNotOK[5];

	// A11K1-5 (K81-K85)
	bool SIIEngineThrustMonitorA[5];

	// A12K1-5 (K151-K156)
	bool SIIEngineThrustMonitorB[5];

	//Temporary variables, not relays
	bool ThrustOKSignal[5];

	//K69 (K223), K70 (K224)
	bool SIISIVBNotSeparated;

	//K78-1 (K173)
	bool SIIEngineOutIndicationA;
	//K88-2 (K174)
	bool SIIEngineOutIndicationB;
	//A9K6, A10K2
	bool SIISIVBSepSeqStart;
	//K26 (K233)
	bool UllageThrustIndicate;

	bool SIIEDSCutoff;

private:
	const int SIIEngInd[5] = { 1,3,0,2,4 };
};