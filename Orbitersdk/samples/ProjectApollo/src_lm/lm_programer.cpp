/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Mission Programer

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
#include "LEM.h"
#include "lm_programer.h"
#include "papi.h"

LEM_Programer::LEM_Programer()
{
	lem = NULL;
}

void LEM_Programer::Init(LEM *l)
{
	lem = l;
}

void LEM_Programer::ProcessChannel10(ChannelValue val)
{
	if (!lem->HasProgramer) return;

	ChannelValue10LMP out_val;

	out_val.Value = val.to_ulong();

	if (out_val.Bits.a == 15)
	{
		switch (out_val.Bits.b)
		{
		case 4: //Pyro Master Arm On
			lem->EDMasterArm.SetState(1);
			break;
		case 5:	//Pyro Master Arm Off
			lem->EDMasterArm.SetState(0);
			break;
		case 6:	//RCS Press Fire
			lem->EDHePressRCS.SetState(1);
			break;
		case 7:	//RCS Press Fire Reset
			lem->EDHePressRCS.SetState(0);
			break;
		case 22: //Abort Stage Fire
			lem->AbortStageSwitch.SetState(0);
			break;
		case 23: //Abort Stage Fire Reset
			lem->AbortStageSwitch.SetState(1);
			break;
		case 60: //RCS ASC Feed A Open
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_UP);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 61:	//RCS ASC Feed A Open Reset
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 62:	//RCS ASC Feed B Open
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_UP);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 63: //RCS ASC Feed B Open Reset
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 76: //RCS ASC Feed A Closed
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_DOWN);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 77: //RCS ASC Feed A Closed Reset
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 78: //RCS ASC Feed B Closed
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_DOWN);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 79: //RCS ASC Feed B Closed Reset
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 90: //LM/SLA Sep Fire
			lem->LMSLASeparationFire();
			break;
		case 91: //LM/SLA Sep Fire Reset
			break;
		case 134: //APS Arm
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 135: //APS Arm Reset
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 150: //DPS Arm
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 151: //DPS Reset
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 172: //RCS Main A Closed
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 173: //RCS Main A Closed Reset
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 174: //RCS Main B Closed
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 175: //RCS Main B Closed Reset
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 188: //RCS Main A Open
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 189: //RCS Main A Open Reset
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 190: //RCS Main B Open
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 191: //RCS Main B Open Reset
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		default:
			break;
		}
	}
}