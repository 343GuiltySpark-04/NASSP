/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Descent Propulsion System

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
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "papi.h"
#include "LEM.h"
#include "lm_dps.h"

// Descent Propulsion System
LEM_DPS::LEM_DPS(THRUSTER_HANDLE *dps) :
	dpsThruster(dps) {
	lem = NULL;
	thrustOn = 0;
	engPreValvesArm = 0;
	engArm = 0;
	HePress[0] = 0; HePress[1] = 0;
	thrustcommand = 0;
}

void LEM_DPS::Init(LEM *s) {
	lem = s;
	HePress[0] = 240;
	HePress[1] = 240;
}

void LEM_DPS::ThrottleActuator(double pos)
{
	if (engArm)
	{
		thrustcommand = pos;

		if (thrustcommand > 0.925)
		{
			thrustcommand = 0.925;
		}
		else if (thrustcommand < 0.1)
		{
			thrustcommand = 0.1;
		}
	}
	else
	{
		//Without power, the throttle will be fully open
		thrustcommand = 0.925;
	}
}

void LEM_DPS::TimeStep(double simt, double simdt) {
	if (lem == NULL) { return; }
	if (lem->stage > 1) { return; }

	if ((lem->SCS_DECA_PWR_CB.IsPowered() && lem->deca.GetK10()) || (lem->SCS_DES_ENG_OVRD_CB.IsPowered() && lem->scca3.GetK5()))
	{
		engPreValvesArm = true;
	}
	else
	{
		engPreValvesArm = false;
	}

	if (lem->SCS_DECA_PWR_CB.IsPowered() && (lem->deca.GetK1() || lem->deca.GetK23()))
	{
		engArm = true;
	}
	else
	{
		engArm = false;
	}

	if (lem->deca.GetThrustOn() || (lem->SCS_DES_ENG_OVRD_CB.IsPowered() && lem->scca3.GetK5()))
	{
		thrustOn = true;
	}
	else
	{
		thrustOn = false;
	}

	if (dpsThruster[0]) {

		//Set Thruster Resource
		if (engPreValvesArm)
		{
			lem->SetThrusterResource(dpsThruster[0], lem->ph_Dsc);
			lem->SetThrusterResource(dpsThruster[1], lem->ph_Dsc);
		}
		else
		{
			lem->SetThrusterResource(dpsThruster[0], NULL);
			lem->SetThrusterResource(dpsThruster[1], NULL);
		}

		//Engine Fire Command
		if (engPreValvesArm && thrustOn)
		{
			lem->SetThrusterLevel(dpsThruster[0], thrustcommand);
			lem->SetThrusterLevel(dpsThruster[1], thrustcommand);
		}
		else
		{
			lem->SetThrusterLevel(dpsThruster[0], 0.0);
			lem->SetThrusterLevel(dpsThruster[1], 0.0);
		}
	}

	// Do GDA time steps
	pitchGimbalActuator.Timestep(simt, simdt);
	rollGimbalActuator.Timestep(simt, simdt);

	VECTOR3 dpsvector;

	if (lem->stage < 2 && dpsThruster[0]) {
		// Directions X,Y,Z
		dpsvector.x = -rollGimbalActuator.GetPosition() * RAD; // Convert deg to rad
		dpsvector.z = pitchGimbalActuator.GetPosition() * RAD;
		dpsvector.y = 1;
		lem->SetThrusterDir(dpsThruster[0], dpsvector);
		lem->SetThrusterDir(dpsThruster[1], dpsvector);

		//sprintf(oapiDebugString(), "Start: %d, Stop: %d Lever: %f Throttle Cmd: %f thrustOn: %d thrustOff: %d", lem->ManualEngineStart.GetState(), lem->ManualEngineStop.GetState(), lem->ttca_throttle_pos_dig, thrustcommand, thrustOn, thrustOff);
		//sprintf(oapiDebugString(), "DPS %d rollc: %d, roll: %f� pitchc: %d, pitch: %f�", thrustOn, rollGimbalActuator.GetLGCPosition(), rollGimbalActuator.GetPosition(), pitchGimbalActuator.GetLGCPosition(), pitchGimbalActuator.GetPosition());
	}
}

void LEM_DPS::SystemTimestep(double simdt) {
	pitchGimbalActuator.SystemTimestep(simdt);
	rollGimbalActuator.SystemTimestep(simdt);
}

void LEM_DPS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "THRUSTON", (thrustOn ? 1 : 0));
	oapiWriteScenario_int(scn, "ENGPREVALVESARM", (engPreValvesArm ? 1 : 0));
	oapiWriteScenario_int(scn, "ENGARM", (engArm ? 1 : 0));
	oapiWriteLine(scn, end_str);
}

void LEM_DPS::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int i;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;

		if (!strnicmp(line, "THRUSTON", 8)) {
			sscanf(line + 8, "%d", &i);
			thrustOn = (i != 0);
		}
		else if (!strnicmp(line, "ENGPREVALVESARM", 15)) {
			sscanf(line + 15, "%d", &i);
			engPreValvesArm = (i != 0);
		}
		else if (!strnicmp(line, "ENGARM", 6)) {
			sscanf(line + 6, "%d", &i);
			engArm = (i != 0);
		}
	}
}

DPSGimbalActuator::DPSGimbalActuator() {

	position = 0;
	commandedPosition = 0;
	lgcPosition = 0;
	atcaPosition = 0;
	motorRunning = false;
	lem = 0;
	gimbalMotorSwitch = 0;
	motorSource = 0;
	gimbalfail = false;
}

DPSGimbalActuator::~DPSGimbalActuator() {
	// Nothing for now.
}

void DPSGimbalActuator::Init(LEM *s, AGCIOSwitch *m1Switch, e_object *m1Source) {

	lem = s;
	gimbalMotorSwitch = m1Switch;
	motorSource = m1Source;
}

void DPSGimbalActuator::Timestep(double simt, double simdt) {

	if (lem == NULL) { return; }

	// After staging
	if (lem->stage > 1) {
		position = 0;
		return;
	}

	//
	// Motors
	//

	if (motorRunning) {
		if (gimbalMotorSwitch->IsDown() || motorSource->Voltage() < SP_MIN_ACVOLTAGE) {
			motorRunning = false;
		}
	}
	else {
		if (gimbalMotorSwitch->IsUp() && motorSource->Voltage() > SP_MIN_ACVOLTAGE) {
			//if (motorStartSource->Voltage() > SP_MIN_ACVOLTAGE) {
			motorRunning = true;
			//	}
		}
	}

	//sprintf(oapiDebugString(), "Motor %d %f %d", gimbalMotorSwitch->IsUp(), motorSource->Voltage(), motorRunning);

	//
	// Process commanded position
	//

	if (lem->GuidContSwitch.IsUp())
	{
		commandedPosition = lgcPosition;
	}
	else {
		commandedPosition = atcaPosition;
	}

	if (IsSystemPowered() && motorRunning) {
		//position = commandedPosition; // Instant positioning
		GimbalTimestep(simdt);
	}

	// Only 6.0 degrees of travel allowed.
	if (position > 6.0) { position = 6.0; }
	if (position < -6.0) { position = -6.0; }

	//sprintf(oapiDebugString(), "position %.3f commandedPosition %d lgcPosition %d", position, commandedPosition, lgcPosition);
}

void DPSGimbalActuator::GimbalTimestep(double simdt)
{
	double LMR, dpos;

	LMR = 0.2;	//0.2�/s maximum gimbal speed

	dpos = (double)commandedPosition*LMR*simdt;

	position += dpos;
}

void DPSGimbalActuator::SystemTimestep(double simdt) {

	if (lem->stage > 1) return;

	if (IsSystemPowered() && motorRunning) {
		DrawSystemPower();
	}
}

bool DPSGimbalActuator::IsSystemPowered() {

	if (gimbalMotorSwitch->IsUp())
	{
		if (motorSource->Voltage() > SP_MIN_ACVOLTAGE)
		{
			return true;
		}
	}
	return false;
}


void DPSGimbalActuator::DrawSystemPower() {

	if (gimbalMotorSwitch->IsUp())
	{
		motorSource->DrawPower(17.5);	/// \todo apparently 35 Watts for both actuators
	}
}

void DPSGimbalActuator::ChangeLGCPosition(int pos) {

	lgcPosition = pos;
}

void DPSGimbalActuator::SaveState(FILEHANDLE scn) {

	// START_STRING is written in LEM
	papiWriteScenario_double(scn, "POSITION", position);
	oapiWriteScenario_int(scn, "COMMANDEDPOSITION", commandedPosition);
	oapiWriteScenario_int(scn, "LGCPOSITION", lgcPosition);
	oapiWriteScenario_int(scn, "ATCAPOSITION", atcaPosition);
	oapiWriteScenario_int(scn, "MOTORRUNNING", (motorRunning ? 1 : 0));

	oapiWriteLine(scn, "DPSGIMBALACTUATOR_END");
}

void DPSGimbalActuator::LoadState(FILEHANDLE scn) {

	char *line;
	int i;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "DPSGIMBALACTUATOR_END", sizeof("DPSGIMBALACTUATOR_END"))) {
			return;
		}

		if (!strnicmp(line, "POSITION", 8)) {
			sscanf(line + 8, "%lf", &position);
		}
		else if (!strnicmp(line, "COMMANDEDPOSITION", 17)) {
			sscanf(line + 17, "%d", &commandedPosition);
		}
		else if (!strnicmp(line, "LGCPOSITION", 11)) {
			sscanf(line + 11, "%d", &lgcPosition);
		}
		else if (!strnicmp(line, "ATCAPOSITION", 12)) {
			sscanf(line + 12, "%d", &atcaPosition);
		}
		else if (!strnicmp(line, "MOTORRUNNING", 13)) {
			sscanf(line + 13, "%d", &i);
			motorRunning = (i != 0);
		}
	}
}