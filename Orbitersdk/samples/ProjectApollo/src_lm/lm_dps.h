/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Descent Propulsion System (Header)

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

class LEM;
class AGCIOSwitch;
class e_object;

class DPSPropellantSource {
public:
	DPSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p);
protected:
	bool PrimaryHeRegulatorShutoffValveOpen;
	bool SecondaryHeRegulatorShutoffValveOpen;
	bool AmbientHeIsolValveOpen;
	bool SupercritHeIsolValveOpen;
};


class DPSGimbalActuator {

public:
	DPSGimbalActuator();
	virtual ~DPSGimbalActuator();

	void Init(LEM *s, AGCIOSwitch *m1Switch, e_object *m1Source);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	double GetPosition() { return position; }
	void ChangeLGCPosition(int pos);
	void ZeroLGCPosition() { lgcPosition = 0; }
	int GetLGCPosition() { return lgcPosition; }
	bool GimbalFail() { return gimbalfail; }

	void GimbalTimestep(double simdt);

protected:
	bool IsSystemPowered();
	void DrawSystemPower();

	double position;
	int commandedPosition;
	int lgcPosition;
	int atcaPosition;
	bool motorRunning;
	bool gimbalfail;

	LEM *lem;
	AGCIOSwitch *gimbalMotorSwitch;
	e_object *motorSource;
};

// Descent Engine
class LEM_DPS {
public:
	LEM_DPS(THRUSTER_HANDLE *dps);
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simt, double simdt);
	void SystemTimestep(double simdt);

	void ThrottleActuator(double manthrust, double autothrust);

	LEM *lem;					// Pointer at LEM
	double HePress[2];			// Helium pressure above and below the regulator
	bool thrustOn;				// Engine "On" Command
	bool engArm;				// Engine Arm Command
	bool engPreValvesArm;		// Engine Prevalves Arm Command
	double thrustcommand;		// DPS Thrust Command

	DPSGimbalActuator pitchGimbalActuator;
	DPSGimbalActuator rollGimbalActuator;

protected:

	THRUSTER_HANDLE *dpsThruster;

};