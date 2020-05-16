/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LEM virtual cockpit code

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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LM_VC_Resource.h"
#include "Mission.h"

#include "LEM.h"

void LEM::JostleViewpoint(double amount)

{
	double j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetz += j;

	if (ViewOffsetx > 0.10)
		ViewOffsetx = 0.10;
	if (ViewOffsetx < -0.10)
		ViewOffsetx = -0.10;

	if (ViewOffsety > 0.10)
		ViewOffsety = 0.10;
	if (ViewOffsety < -0.10)
		ViewOffsety = -0.10;

	if (ViewOffsetz > 0.05)
		ViewOffsetz = 0.05;
	if (ViewOffsetz < -0.05)
		ViewOffsetz = -0.05;

	SetView();
}

void LEM::SetView() {

	VECTOR3 v;
	//
	// Set camera offset
	//
	if (InVC) {
		switch (viewpos) {
		case LMVIEW_CDR:
			if (stage == 2) {
				v = _V(-0.55, -0.07, 1.35);
			}
			else {
				v = _V(-0.55, 1.68, 1.35);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;

		case LMVIEW_LMP:
			if (stage == 2) {
				v = _V(0.55, -0.07, 1.35);
			}
			else {
				v = _V(0.55, 1.68, 1.35);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;
		}

		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;

		SetCameraOffset(v);

	} else {

		if(InPanel)
		{
			switch(PanelId)
			{
				case LMPANEL_MAIN:
					if (stage == 2) {
						SetCameraOffset(_V(0, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(0, 1.90, 1.26));
					}
					break;
				case LMPANEL_RIGHTWINDOW:
					if (stage == 2) {
						SetCameraOffset(_V(0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_LEFTWINDOW:
					if (stage == 2) {
						SetCameraOffset(_V(-0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(-0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_LPDWINDOW:
					if (stage == 2) {
						v = _V(-0.61, -0.125, 1.39);
					}
					else {
						v = _V(-0.61, 1.625, 1.39);
					}
					v.x += ViewOffsetx;
					v.y += ViewOffsety;
					v.z += ViewOffsetz;

					SetCameraOffset(v);
					break;
				case LMPANEL_RNDZWINDOW:
					if (stage == 2) {
						SetCameraOffset(_V(-0.598, 0.15, 1.106));
					}
					else {
						SetCameraOffset(_V(-0.598, 1.90, 1.106));
					}
					break;
				case LMPANEL_LEFTPANEL:
					if (stage == 2) {
						SetCameraOffset(_V(-0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(-0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_AOTVIEW:
					if (stage == 2) {
						SetCameraOffset(_V(0, 1.13, 1.26));
					}
					else {
						SetCameraOffset(_V(0, 2.88, 1.26));
					}
					break;
				case LMPANEL_AOTZOOM:
					if (stage == 2) {
						SetCameraOffset(_V(0, 1.13, 1.26));
					}
					else {
						SetCameraOffset(_V(0, 2.88, 1.26));
					}
					break;
				case LMPANEL_DOCKVIEW:
					if (stage == 2) {
						SetCameraOffset(_V(-0.598, 0.15, 1.106));
					}
					else {
						SetCameraOffset(_V(-0.598, 1.90, 1.106));
					}
					break;
				
				case LMPANEL_LEFTZOOM:
					if (stage == 2) {
						SetCameraOffset(_V(-0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(-0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_UPPERHATCH:
					if (stage == 2) {
						SetCameraOffset(_V(0, -0.55, 0));
					}
					else {
						SetCameraOffset(_V(0, 1.20, 0));
					}
					SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
					oapiCameraSetCockpitDir(180 * RAD, 0);
					break;
				case LMPANEL_FWDHATCH:
					if (stage == 2) {
						SetCameraOffset(_V(0, -1.4, 1.5));
					}
					else {
						SetCameraOffset(_V(0, 0.35, 1.5));
					}
					break;
			}
		}
		else
			SetCameraOffset (_V(0, 0, 0));
	}

	//
	// Change FOV for the LPD window and AOT zoom
	//
	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
	   // if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1080.0, 1.0)));
	}
	else if (InPanel && PanelId == LMPANEL_AOTZOOM) {
		// if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1050.0, 1.0)));
	}
    else {
		if(InFOV == false) {
			oapiCameraSetAperture(SaveFOV);
			InFOV = true;
		}
	}
}

bool LEM::clbkLoadVC (int id)
{
	LoadVC();

	switch (id) {
	case 0:
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		return true;

	default:
		return false;
	}
}
void LEM::LoadVC()
{
	VECTOR3 ofs;
	if (stage > 1) { ofs = _V(0.00, -0.76, 0.00); }
	else { ofs = _V(0.00, 0.99, 0.00); }

	RegisterActiveAreas(ofs);
}

void LEM::RegisterActiveAreas(VECTOR3 ofs)
{
	int i = 0;

	//
	// Release all surfaces
	//
	ReleaseSurfacesVC();

	SURFHANDLE MainPanelTex = oapiGetTextureHandle(hLMVC, 4);

	// Panel 1
	oapiVCRegisterArea(AID_VC_LM_CWS_LEFT, _R(238, 27, 559, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_LM_CWS_RIGHT, _R(1075, 27, 1375, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);

	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(60, 259, 202, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(276, 259, 357, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);

	oapiVCRegisterArea(AID_VC_RANGE_TAPE, _R(431, 633, 475, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_RATE_TAPE, _R(482, 633, 517, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);

	oapiVCRegisterArea(AID_VC_XPOINTERCDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_XPOINTERLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 3
	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + ofs, 0.012);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P3_01 + i, PANEL_REDRAW_MOUSE, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_RDR_SIG_STR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_RR_NOTRACK, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 4

	for (i = 0; i < P4_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P4_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P4_01 + i, P4_PUSHB_POS[i] + ofs, 0.008);
	}

	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(309, 1520, 414, 1696), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS,  _R(165, 1525, 267, 1694), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);

	MainPanelVC.ClearSwitches();
	// Panel 3
	MainPanelVC.AddSwitch(&EngGimbalEnableSwitch, AID_VC_SWITCH_P3_01, &anim_P3switch[0]);
	MainPanelVC.AddSwitch(&EngineDescentCommandOverrideSwitch, AID_VC_SWITCH_P3_02, &anim_P3switch[1]);
	MainPanelVC.AddSwitch(&LandingAntSwitch, AID_VC_SWITCH_P3_03, &anim_P3switch[2]);
	MainPanelVC.AddSwitch(&RadarTestSwitch, AID_VC_SWITCH_P3_04, &anim_P3switch[3]);
	MainPanelVC.AddSwitch(&SlewRateSwitch, AID_VC_SWITCH_P3_05, &anim_P3switch[4]);
	MainPanelVC.AddSwitch(&DeadBandSwitch, AID_VC_SWITCH_P3_06, &anim_P3switch[5]);
	MainPanelVC.AddSwitch(&GyroTestLeftSwitch, AID_VC_SWITCH_P3_07, &anim_P3switch[6]);
	MainPanelVC.AddSwitch(&GyroTestRightSwitch, AID_VC_SWITCH_P3_08, &anim_P3switch[7]);
	MainPanelVC.AddSwitch(&RollSwitch, AID_VC_SWITCH_P3_09, &anim_P3switch[8]);
	MainPanelVC.AddSwitch(&PitchSwitch, AID_VC_SWITCH_P3_10, &anim_P3switch[9]);
	MainPanelVC.AddSwitch(&YawSwitch, AID_VC_SWITCH_P3_11, &anim_P3switch[10]);
	MainPanelVC.AddSwitch(&ModeControlPGNSSwitch, AID_VC_SWITCH_P3_12, &anim_P3switch[11]);
	MainPanelVC.AddSwitch(&ModeControlAGSSwitch, AID_VC_SWITCH_P3_13, &anim_P3switch[12]);
	MainPanelVC.AddSwitch(&IMUCageSwitch, AID_VC_SWITCH_P3_14, &anim_P3switch[13]);
	MainPanelVC.AddSwitch(&EventTimerCtlSwitch, AID_VC_SWITCH_P3_15, &anim_P3switch[14]);
	MainPanelVC.AddSwitch(&EventTimerStartSwitch, AID_VC_SWITCH_P3_16, &anim_P3switch[15]);
	MainPanelVC.AddSwitch(&EventTimerMinuteSwitch, AID_VC_SWITCH_P3_17, &anim_P3switch[16]);
	MainPanelVC.AddSwitch(&EventTimerSecondSwitch, AID_VC_SWITCH_P3_18, &anim_P3switch[17]);
	MainPanelVC.AddSwitch(&RCSSysQuad1Switch, AID_VC_SWITCH_P3_19, &anim_P3switch[18]);
	MainPanelVC.AddSwitch(&RCSSysQuad4Switch, AID_VC_SWITCH_P3_20, &anim_P3switch[19]);
	MainPanelVC.AddSwitch(&RCSSysQuad2Switch, AID_VC_SWITCH_P3_21, &anim_P3switch[20]);
	MainPanelVC.AddSwitch(&RCSSysQuad3Switch, AID_VC_SWITCH_P3_22, &anim_P3switch[21]);
	MainPanelVC.AddSwitch(&SidePanelsSwitch, AID_VC_SWITCH_P3_23, &anim_P3switch[22]);
	MainPanelVC.AddSwitch(&FloodSwitch, AID_VC_SWITCH_P3_24, &anim_P3switch[23]);
	MainPanelVC.AddSwitch(&RightXPointerSwitch, AID_VC_SWITCH_P3_25, &anim_P3switch[24]);
	MainPanelVC.AddSwitch(&ExteriorLTGSwitch, AID_VC_SWITCH_P3_26, &anim_P3switch[25]);

	MainPanelVC.AddSwitch(&TestMonitorRotary, AID_VC_ROT_P3_01, &anim_P3_Rot[0]);
	MainPanelVC.AddSwitch(&RendezvousRadarRotary, AID_VC_ROT_P3_02, &anim_P3_Rot[1]);
	MainPanelVC.AddSwitch(&TempMonitorRotary, AID_VC_ROT_P3_03, &anim_P3_Rot[2]);
	MainPanelVC.AddSwitch(&LampToneTestRotary, AID_VC_ROT_P3_04, &anim_P3_Rot[3]);
	MainPanelVC.AddSwitch(&FloodRotary, AID_VC_ROT_P3_05, &anim_P3_Rot[4]);

	//Panel 4
	MainPanelVC.AddSwitch(&DskySwitchVerb, AID_VC_PUSHB_P4_01, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P4_02, NULL);
	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P4_03, NULL);
	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P4_04, NULL);
	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P4_05, NULL);
	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P4_06, NULL);
	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P4_07, NULL);
	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P4_08, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P4_09, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P4_10, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P4_11, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P4_12, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P4_13, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P4_14, NULL);
	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P4_15, NULL);
	MainPanelVC.AddSwitch(&DskySwitchProg, AID_VC_PUSHB_P4_16, NULL);
	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P4_17, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P4_18, NULL);
	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P4_19, NULL);

	//
	// Initialize surfaces and switches
	//
	InitPanelVC();
}

bool LEM::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
{
	return MainPanelVC.VCMouseEvent(id, event, p);
}

bool LEM::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	switch (id) {

	case AID_VC_LM_CWS_LEFT:
		CWEA.RedrawLeft(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_LM_CWS_RIGHT:
		CWEA.RedrawRight(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP]);
		return true;

	case AID_VC_DSKY_LIGHTS:
		if (pMission->GetLMDSKYVersion() == 3)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, true);
		}
		else if (pMission->GetLMDSKYVersion() == 2)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS]);
		}
		else
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, false);
		}
		return true;

	case AID_VC_RDR_SIG_STR:
		SetAnimation(anim_Needle_Radar, (RadarSignalStrengthMeter.GetDisplayValue() / 6.67) + 0.125);
		return true;

	case AID_VC_RANGE_TAPE:
		RadarTape.RenderRangeVC(surf, srf[SRF_VC_RADAR_TAPE], srf[SRF_VC_RADAR_TAPE2]);
		return true;

	case AID_VC_RATE_TAPE:
		RadarTape.RenderRateVC(surf, srf[SRF_VC_RADAR_TAPE]);
		return true;

	case AID_VC_XPOINTERCDR:
		RedrawPanel_XPointerVC(&crossPointerLeft, anim_xpointerx_cdr, anim_xpointery_cdr);
		return true;

	case AID_VC_XPOINTERLMP:
		RedrawPanel_XPointerVC(&crossPointerRight, anim_xpointerx_lmp, anim_xpointery_lmp);
		return true;

	case AID_VC_FDAI_LEFT:

		VECTOR3 attitude;
		VECTOR3 errors;
		//VECTOR3 rates;
		int no_att = 0;

		if (AttitudeMonSwitch.IsUp())	//PGNS
		{
			attitude = gasta.GetTotalAttitude();
		}
		else							//AGS
		{
			attitude = aea.GetTotalAttitude();
		}

		if (RateErrorMonSwitch.GetState() == 1)
		{
			if (RR.IsPowered()) {
				if (ShiftTruSwitch.IsUp())
				{
					errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
					errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
					errors.x = 0.0;
				}
				else
				{
					errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
					errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
					errors.x = 0.0;
				}
			}
			else
			{
				errors = _V(0, 0, 0);
			}
		}
		else
		{
			if (AttitudeMonSwitch.IsUp())	//PGNS
			{
				errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
			}
			else							//AGS
			{
				VECTOR3 aeaerror = aea.GetAttitudeError();
				errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

				if (DeadBandSwitch.IsUp())
				{
					errors *= 15.0 / 14.4;
				}
				else
				{
					errors *= 15.0 / 1.7;
				}
			}
		}

		// ORDEAL
		if (!no_att) {
			attitude.y += ordeal.GetFDAI1PitchAngle();
			if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
		}

		if (RateScaleSwitch.IsUp())
		{
			//rates = rga.GetRates() / (25.0*RAD);
		}
		else
		{
			//rates = rga.GetRates() / (5.0*RAD);
		}

		if (AttitudeMonSwitch.IsUp())	//PGNS
		{
			fdai_proc[0] = 1.0 - attitude.x / PI2;
			fdai_proc[1] = attitude.y / PI2;
			fdai_proc[2] = attitude.z / PI2;
		}
		else							//AGS
		{
			fdai_proc[0] = -attitude.x / PI2;
			fdai_proc[1] = attitude.y / PI2;
			fdai_proc[2] = attitude.z / PI2;
		}
		if (fdai_proc[0] < 0) fdai_proc[0] += 1.0;
		if (fdai_proc[1] < 0) fdai_proc[1] += 1.0;
		if (fdai_proc[2] < 0) fdai_proc[2] += 1.0;
		if (fdai_proc[0] - fdai_proc_last[0] != 0.0) SetAnimation(anim_fdai_yaw, fdai_proc[2]);
		if (fdai_proc[1] - fdai_proc_last[1] != 0.0) SetAnimation(anim_fdai_roll, fdai_proc[0]);
		if (fdai_proc[2] - fdai_proc_last[2] != 0.0) SetAnimation(anim_fdai_pitch, fdai_proc[1]);
		fdai_proc_last[0] = fdai_proc[0];
		fdai_proc_last[1] = fdai_proc[1];
		fdai_proc_last[2] = fdai_proc[2];

		sprintf(oapiDebugString(),"LEM: LGC-ERR: %d %d %d",atca.lgc_err_x,atca.lgc_err_y,atca.lgc_err_z);

		SetAnimation(anim_fdai_rollerror, (errors.x + 384) / 768);
		SetAnimation(anim_fdai_pitcherror, (-errors.y + 384) / 768);
		SetAnimation(anim_fdai_yawerror, (errors.z + 384) / 768);

		return true;

	/*case AID_VC_RR_NOTRACK:
		if (lca.GetAnnunVoltage() > 2.25 && (RR.GetNoTrackSignal() || LampToneTestRotary.GetState() == 6)) { // The AC side is only needed for the transmitter
			SetCompLight(LM_COMPLIGHT_1, true); // Light On
		} else {
			SetCompLight(LM_COMPLIGHT_1, false); // Light Off
		}
		return true;*/
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
}

void LEM::DeleteVCAnimations()
{
	int i = 0;

	for (i = 0; i < P3_SWITCHCOUNT; i++) delete mgt_P3switch[i];

	for (i = 0; i < P3_ROTCOUNT; i++) delete mgt_P3Rot[i];
}

void LEM::InitVCAnimations()
{
	UINT mesh = vcidx;
	int i = 0;

	// Define panel 3 animations
	static UINT meshgroup_P3switches[P3_SWITCHCOUNT], meshgroup_P3Rots[P3_ROTCOUNT];
	for (int i = 0; i < P3_SWITCHCOUNT; i++)
	{
		meshgroup_P3switches[i] = VC_GRP_Sw_P3_01 + i;

		mgt_P3switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3switches[i], 1, P3_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P3switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P3switch[i], 0.0f, 1.0f, mgt_P3switch[i]);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		meshgroup_P3Rots[i] = VC_GRP_Rot_P3_01 + i;

		mgt_P3Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3Rots[i], 1, P3_ROT_POS[i], P3_ROT_AXIS, (float)(RAD * 360));
		anim_P3_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P3_Rot[i], 0.0f, 1.0f, mgt_P3Rot[i]);
	}

	// Radar strength meter
	static UINT meshgroup_Needle_Radar = VC_GRP_Needle_Radar;
	static MGROUP_ROTATE mgt_Needle_Radar(mesh, &meshgroup_Needle_Radar, 1, _V(-0.264142, 0.235721, 1.57832), P3_ROT_AXIS, (float)(RAD * 360));
	anim_Needle_Radar = CreateAnimation(0.5);
	AddAnimationComponent(anim_Needle_Radar, 0.0f, 1.0f, &mgt_Needle_Radar);

	// CDR & LMP X-pointer
	const VECTOR3 xvector = { 0.00, 0.075*cos(P1_TILT), 0.075*sin(P1_TILT) };
	const VECTOR3 yvector = { 0.075, 0, 0 };
	static UINT meshgroup_XpointerX_cdr = VC_GRP_XpointerX_cdr, meshgroup_XpointerX_lmp = VC_GRP_XpointerX_lmp;
	static UINT meshgroup_XpointerY_cdr = VC_GRP_XpointerY_cdr, meshgroup_XpointerY_lmp = VC_GRP_XpointerY_lmp;
	static MGROUP_TRANSLATE mgt_xpointerx_cdr(mesh, &meshgroup_XpointerX_cdr, 1, xvector);
	static MGROUP_TRANSLATE mgt_xpointery_cdr(mesh, &meshgroup_XpointerY_cdr, 1, yvector);
	static MGROUP_TRANSLATE mgt_xpointerx_lmp(mesh, &meshgroup_XpointerX_lmp, 1, xvector);
	static MGROUP_TRANSLATE mgt_xpointery_lmp(mesh, &meshgroup_XpointerY_lmp, 1, yvector);
	anim_xpointerx_cdr = CreateAnimation(0.5);
	anim_xpointery_cdr = CreateAnimation(0.5);
	anim_xpointerx_lmp = CreateAnimation(0.5);
	anim_xpointery_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_xpointerx_cdr, 0.0f, 1.0f, &mgt_xpointerx_cdr);
	AddAnimationComponent(anim_xpointery_cdr, 0.0f, 1.0f, &mgt_xpointery_cdr);
	AddAnimationComponent(anim_xpointerx_lmp, 0.0f, 1.0f, &mgt_xpointerx_lmp);
	AddAnimationComponent(anim_xpointery_lmp, 0.0f, 1.0f, &mgt_xpointery_lmp);

	//FDAI

	ANIMATIONCOMPONENT_HANDLE	ach_FDAI_roll, ach_FDAI_pitch, ach_FDAI_yaw;
	const VECTOR3	FDAI_PIVOT = { -0.297851, 0.525802, 1.70639 }; // Pivot Point

	static UINT meshgroup_Fdai1 = { VC_GRP_FDAIBall1 };
	static UINT meshgroup_Fdai2 = { VC_GRP_FDAIBall };
	static UINT meshgroup_Fdai3 = { VC_GRP_FDAIBall2 };

	const VECTOR3 rollaxis = { -0.00, sin(7.95581 * RAD), -cos(7.95581 * RAD) };
	const VECTOR3 yawvaxis = { -0.00, sin(97.95581 * RAD), -cos(97.95581 * RAD) };

	static MGROUP_ROTATE mgt_FDAI_Roll(mesh, &meshgroup_Fdai1, 1, FDAI_PIVOT, rollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAI_Pitch(mesh, &meshgroup_Fdai2, 1, FDAI_PIVOT, _V(-1,0,0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAI_Yaw(mesh, &meshgroup_Fdai3, 1, FDAI_PIVOT, yawvaxis, (float)(RAD * 360));
	anim_fdai_roll = CreateAnimation(0.0);
	anim_fdai_pitch = CreateAnimation(0.0);
	anim_fdai_yaw = CreateAnimation(0.0);
	ach_FDAI_roll = AddAnimationComponent(anim_fdai_roll, 0.0f, 1.0f, &mgt_FDAI_Roll);
	ach_FDAI_yaw = AddAnimationComponent(anim_fdai_yaw, 0.0f, 1.0f, &mgt_FDAI_Yaw, ach_FDAI_roll);
	ach_FDAI_pitch = AddAnimationComponent(anim_fdai_pitch, 0.0f, 1.0f, &mgt_FDAI_Pitch, ach_FDAI_yaw);
	//Anything but 0.0-1.0 will do
	fdai_proc_last[0] = 2.0;
	fdai_proc_last[1] = 2.0;
	fdai_proc_last[2] = 2.0;

	// FDAI error needles
	const VECTOR3 needlexvector = { 0.00, 0.05*cos(P1_TILT), 0.05*sin(P1_TILT) };
	const VECTOR3 needleyvector = { 0.05, 0, 0 };

	static UINT meshgroup_RollError = VC_GRP_FDAI_rollerror;
	static UINT meshgroup_PitchError = VC_GRP_FDAI_pitcherror;
	static UINT meshgroup_YawError = VC_GRP_FDAI_yawerror;

	static MGROUP_TRANSLATE mgt_rollerror(mesh, &meshgroup_RollError, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror(mesh, &meshgroup_PitchError, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror(mesh, &meshgroup_YawError, 1, needleyvector);
	anim_fdai_rollerror = CreateAnimation(0.5);
	anim_fdai_pitcherror = CreateAnimation(0.5);
	anim_fdai_yawerror = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdai_rollerror, 0.0f, 1.0f, &mgt_rollerror);
	AddAnimationComponent(anim_fdai_pitcherror, 0.0f, 1.0f, &mgt_pitcherror);
	AddAnimationComponent(anim_fdai_yawerror, 0.0f, 1.0f, &mgt_yawerror);
}

void LEM::SetCompLight(int m, bool state) {

	if (!vcmesh)
		return;

	int complightmat = VC_NMAT - 9;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, complightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 0.878f;
		mat->emissive.b = 0.506f;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0;
		mat->emissive.g = 0;
		mat->emissive.b = 0;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, complightmat + m, mat);
}

