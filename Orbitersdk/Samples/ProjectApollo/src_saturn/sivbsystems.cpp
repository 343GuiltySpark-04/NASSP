/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IVB Systems

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

#include "Orbitersdk.h"

#include "soundlib.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "sivbsystems.h"

SIVBSystems::SIVBSystems(VESSEL *v, THRUSTER_HANDLE &j2) :
	j2engine(j2) {

	vessel = v;

	FirstBurnRelay = false;
	SecondBurnRelay = false;
	EngineStart = false;
	LVDCEngineStopRelay = false;
	EDSEngineStop = false;
	EngineReady = false;
	EnginePower = false;
	PropellantDepletionSensors = false;
	CutoffInhibitRelay = false;
	RSSEngineStop = false;
	ThrustOKRelay = false;

	ThrustTimer = 0.0;
}

void SIVBSystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SIVBSYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "FIRSTBURNRELAY", FirstBurnRelay);
	papiWriteScenario_bool(scn, "SECONDBURNRELAY", SecondBurnRelay);
	papiWriteScenario_bool(scn, "ENGINESTART", EngineStart);
	papiWriteScenario_bool(scn, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
	papiWriteScenario_bool(scn, "EDSENGINESTOP", EDSEngineStop);
	papiWriteScenario_bool(scn, "RSSENGINESTOP", RSSEngineStop);
	papiWriteScenario_bool(scn, "ENGINESTOP", EngineStop);
	papiWriteScenario_bool(scn, "ENGINEREADY", EngineReady);
	papiWriteScenario_bool(scn, "CUTOFFINHIBITRELAY", CutoffInhibitRelay);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	papiWriteScenario_double(scn, "THRUSTLEVEL", ThrustLevel);

	oapiWriteLine(scn, SIVBSYSTEMS_END_STRING);
}

void SIVBSystems::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SIVBSYSTEMS_END_STRING, sizeof(SIVBSYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "FIRSTBURNRELAY", FirstBurnRelay);
		papiReadScenario_bool(line, "SECONDBURNRELAY", SecondBurnRelay);
		papiReadScenario_bool(line, "ENGINESTART", EngineStart);
		papiReadScenario_bool(line, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
		papiReadScenario_bool(line, "EDSENGINESTOP", EDSEngineStop);
		papiReadScenario_bool(line, "RSSENGINESTOP", RSSEngineStop);
		papiReadScenario_bool(line, "ENGINESTOP", EngineStop);
		papiReadScenario_bool(line, "ENGINEREADY", EngineReady);
		papiReadScenario_bool(line, "CUTOFFINHIBITRELAY", CutoffInhibitRelay);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
		papiReadScenario_double(line, "THRUSTLEVEL", ThrustLevel);

	}
}

void SIVBSystems::Timestep(double simdt)
{
	if (j2engine == NULL) return;

	//Thrust OK switch
	bool ThrustOK = vessel->GetThrusterLevel(j2engine) > 0.65;

	//TBD: Propellant Depletion

	//Engine Control Power Switch (EDS no. 1, range safety no. 1)
	if ((!EDSCutoffDisabled && EDSEngineStop) || RSSEngineStop)
	{
		AftPowerDisableRelay = true;
	}
	else
	{
		AftPowerDisableRelay = false;
	}

	//Engine Power
	if (!AftPowerDisableRelay)
	{
		EnginePower = true;
	}
	else
	{
		EnginePower = false;
	}

	if (EnginePower && ThrustOK)
	{
		ThrustOKRelay = true;
	}
	else
	{
		ThrustOKRelay = false;
	}

	if (ThrustOKRelay)
	{
		CutoffInhibitRelay = true;
	}

	//Engine Cutoff Bus (switch selector, range safety system no. 2, EDS no. 2, propellant depletion sensors)
	if (LVDCEngineStopRelay || EDSEngineStop || RSSEngineStop || PropellantDepletionSensors)
	{
		EngineCutoffBus = true;
	}
	else
	{
		EngineCutoffBus = false;
	}

	if ((!ThrustOKRelay && CutoffInhibitRelay) || EngineCutoffBus)
	{
		EngineStop = true;
	}

	if (EngineStop)
	{
		if (EngineReady == true)
		{
			ThrustTimer = 0.0;
			EngineStart = false;
			EngineReady = false;
			CutoffInhibitRelay = false;
		}

		ThrustTimer += simdt;

		if (ThrustLevel > 0.0)
		{
			// Cutoff transient thrust
			if (ThrustTimer < 2.0) {
				if (ThrustTimer < 0.25) {
					// 95% of thrust dies in the first .25 second
					ThrustLevel = 1.0 - (ThrustTimer*3.3048);
					vessel->SetThrusterLevel(j2engine, ThrustLevel);
				}
				else {
					if (ThrustTimer < 1.5) {
						// The remainder dies over the next 1.25 second
						ThrustLevel = 0.1738 - ((ThrustTimer - 0.25)*0.1390);
						vessel->SetThrusterLevel(j2engine, ThrustLevel);
					}
					else {
						// Engine is completely shut down at 1.5 second
						ThrustLevel = 0.0;
						vessel->SetThrusterLevel(j2engine, ThrustLevel);
					}
				}
			}
		}
		else
		{
			EngineStop = false;
		}
	}
	else if ((EngineReady && EngineStart) || ThrustLevel > 0.0)
	{
		ThrustTimer += simdt;

		if (SecondBurnRelay)
		{
			//Second Burn
			if (ThrustTimer >= 8.6 && ThrustTimer < 11.4) {
				ThrustLevel = (ThrustTimer - 8.6)*0.53;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
			}
			else if (ThrustTimer > 11.4 && ThrustLevel < 1.0)
			{
				ThrustLevel = 1.0;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
			}
		}
		else
		{
			//First Burn
			if (ThrustTimer >= 3.0 && ThrustTimer < 5.8) {
				ThrustLevel = (ThrustTimer - 3.0)*0.36;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
			}
			else if (ThrustTimer > 5.8 && ThrustLevel < 1.0)
			{
				ThrustLevel = 1.0;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
			}
		}
	}
	else if (ThrustTimer > 0.0)
	{
		ThrustTimer = 0.0;
	}

	//sprintf(oapiDebugString(), "First %d Second %d Start %d Stop %d Ready %d Level %f Timer %f", FirstBurnRelay, SecondBurnRelay, EngineStart, EngineStop, EngineReady, ThrustLevel, ThrustTimer);
}