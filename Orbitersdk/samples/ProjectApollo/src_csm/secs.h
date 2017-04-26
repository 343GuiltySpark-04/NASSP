/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Sequential Events Controller simulation.

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

  **************************** Revision History ****************************/

#if !defined(_PA_SECS_H)
#define _PA_SECS_H

class Saturn;
class FloatBag;

class SECSTimer{

public:
	SECSTimer(double delay);

	virtual void Timestep(double simdt);
	virtual void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	virtual void LoadState(FILEHANDLE scn, char *end_str);
	void SetTime(double t);
	double GetTime();

	void Reset();
	void SetRunning(bool run) { Running = run; };
	bool IsRunning() { return Running; };
	void SetContact(bool cont) { Contact = cont; };
	bool ContactClosed() { return Contact; };

protected:
	double seconds;
	double delay;

	bool Running;
	bool Contact;
};

class RestartableSECSTimer : public SECSTimer
{
public:
	RestartableSECSTimer(double delay);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetStart(bool st) { Start = st; };

protected:
	bool Start;
};

//Reaction Control System Controller
class RCSC
{
public:
	RCSC();
	void Timestep(double simdt);

	void ControlVessel(Saturn *v);

	void SetDeadFaceA(bool df) { MESCDeadfaceA = df; };
	void SetDeadFaceB(bool df) { MESCDeadfaceB = df; };

	//Propellant Dump and Purge Disable Timer
	RestartableSECSTimer TD1, TD2;

	//CM RCS Propellant Dump Delay
	RestartableSECSTimer TD3, TD4;

	//Purge Delay
	SECSTimer TD5, TD6;

protected:

	void TimerTimestep(double simdt);

	//Relays
	bool OxidizerDumpA;
	bool OxidizerDumpB;
	bool InterconnectAndPropellantBurnA;
	bool InterconnectAndPropellantBurnB;
	bool FuelAndOxidBypassPurgeA;
	bool FuelAndOxidBypassPurgeB;
	bool RSCSCMSMTransferA;
	bool RSCSCMSMTransferB;

	bool MESCDeadfaceA;
	bool MESCDeadfaceB;

	Saturn *Sat;
};

class MESC
{
public:
	MESC();
	void Init(Saturn *v, DCbus *LogicBus, DCbus *PyroBus, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch *RCSLogicCB, MissionTimer *MT, EventTimer *ET);
	void Timestep(double simdt);

	void Liftoff();
	bool GetCSMLVSeparateRelay() { return CSMLVSeparateRelay; };
	bool GetCMSMSeparateRelay() { return CMSMSeparateRelay; };
	bool GetCMSMDeadFace() { return CMSMDeadFace; };
	bool FireUllage() { return MESCLogicBus() && RCSLogicCircuitBraker->IsPowered() && UllageRelay; };

	void LoadState(FILEHANDLE scn, char *end_str);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
protected:

	void TimerTimestep(double simdt);

	//Source 11
	bool SequentialLogicBus();
	//Source 12
	bool MESCLogicBus();
	//Source 15
	bool SequentialArmBus();

	bool MESCLogicArm;
	bool BoosterCutoffAbortStartRelay;
	bool LETPhysicalSeperationMonitor;
	bool LESAbortRelay;
	bool AutoAbortEnableRelay;
	bool CMSMDeadFace;
	bool CMSMSeparateRelay;
	bool PyroCutout;
	bool CMRCSPress;
	bool CanardDeploy;
	bool UllageRelay;
	bool CSMLVSeparateRelay;

	//Abort Start Delay
	SECSTimer TD1;
	//CM/SM Seperate Delay
	SECSTimer TD3;
	//Canard Deploy Delay
	SECSTimer TD5;
	//CSM/LM Separation Delay
	SECSTimer TD11;

	Saturn *Sat;

	DCbus *SECSLogicBus;
	DCbus *SECSPyroBus;
	CircuitBrakerSwitch *SECSArmBreaker;
	CircuitBrakerSwitch *RCSLogicCircuitBraker;
	MissionTimer *MissionTimerDisplay;
	EventTimer *EventTimerDisplay;
};

///
/// This class simulates the Sequential Events Control System in the CM.
/// \ingroup InternalSystems
/// \brief SECS simulation.
///
class SECS { 

public:
	SECS();
	virtual ~SECS();

	void ControlVessel(Saturn *v);
	void Timestep(double simt, double simdt);

	void LiftoffA();
	void LiftoffB();

	void TD1_GSEReset();

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

protected:
	bool IsLogicPoweredAndArmedA();
	bool IsLogicPoweredAndArmedB();
	
	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;

	bool PyroBusAMotor;
	bool PyroBusBMotor;

	MESC MESCA;
	MESC MESCB;
	RCSC rcsc;

	Saturn *Sat;
};

///
/// This class simulates the Earth Landing System in the CM.
/// \ingroup InternalSystems
/// \brief ELS simulation.
///
class ELS { 

public:
	ELS();
	virtual ~ELS();

	void ControlVessel(Saturn *v);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);
	double *GetDyeMarkerLevelRef() { return &DyeMarkerLevel; }


protected:
	double NewFloatBagSize(double size, ThreePosSwitch *sw, CircuitBrakerSwitch *cb, double simdt);

	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;

	double FloatBag1Size;
	double FloatBag2Size;
	double FloatBag3Size;
	double DyeMarkerLevel;
	double DyeMarkerTime;

	Saturn *Sat;
	FloatBag *FloatBagVessel;
};

//
// Strings for state saving.
//

#define SECS_START_STRING		"SECS_BEGIN"
#define SECS_END_STRING			"SECS_END"

#define ELS_START_STRING		"ELS_BEGIN"
#define ELS_END_STRING			"ELS_END"

#endif // _PA_SECS_H
