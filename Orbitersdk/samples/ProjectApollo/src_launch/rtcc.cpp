/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC)

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
//#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "rtcc.h"

RTCC::RTCC()
{
	mcc = NULL;
}

void RTCC::Init(MCC *ptr)
{
	mcc = ptr;
}

void RTCC::Calculation(int fcn, LPVOID &pad)
{
	switch (fcn) {
	case 1: // MISSION C PHASING BURN
	{
		LambertMan lambert;
		AP7ManPADOpt opt;
		double P30TIG;
		VECTOR3 dV_LVLH;

		AP7MNV * form = (AP7MNV *)pad;

		lambert.vessel = calcParams.src;
		lambert.GETbase = getGETBase();
		lambert.T1 = 3 * 3600 + 20 * 60;
		lambert.T2 = 26 * 3600 + 25 * 60;
		lambert.N = 15;
		lambert.axis = RTCC_LAMBERT_XAXIS;
		lambert.Offset = _V(76.5 * 1852, 0, 0);
		lambert.PhaseAngle = 0;
		lambert.target = calcParams.tgt;
		lambert.prograde = RTCC_LAMBERT_PROGRADE;
		lambert.impulsive = RTCC_IMPULSIVE;
		lambert.Perturbation = RTCC_LAMBERT_SPHERICAL;

		LambertTargeting(&lambert, dV_LVLH, P30TIG);

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.engopt = 2; //X- RCS Thrusters
		opt.HeadsUp = false;
		opt.sxtstardtime = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC();
		opt.navcheckGET = 0;

		AP7ManeuverPAD(&opt, *form);
	}
	break;
	case 2: // MISSION C CONTINGENCY DEORBIT (6-4) TARGETING
	{
		AP7MNV * form = (AP7MNV *)pad;

		double P30TIG, SVGET, latitude, longitude;
		VECTOR3 dV_LVLH, R0, V0;
		MATRIX3 REFSMMAT;
		EntryOpt entopt;
		AP7ManPADOpt opt;
		REFSMMATOpt refsopt;

		SVGET = 0;
		StateVectorCalc(calcParams.src, SVGET, R0, V0); //State vector for uplink

		entopt.vessel = calcParams.src;
		entopt.GETbase = getGETBase();
		entopt.impulsive = RTCC_NONIMPULSIVE;
		entopt.lng = -163.0*RAD;
		entopt.nominal = RTCC_ENTRY_NOMINAL;
		entopt.Range = 0;
		entopt.ReA = 0;
		entopt.TIGguess = 8 * 60 * 60 + 55 * 60;
		entopt.type = RTCC_ENTRY_DEORBIT;
		entopt.entrylongmanual = true;

		EntryTargeting(&entopt, dV_LVLH, P30TIG, latitude, longitude); //Target Load for uplink

		refsopt.vessel = calcParams.src;
		refsopt.GETbase = getGETBase();
		refsopt.dV_LVLH = dV_LVLH;
		refsopt.P30TIG = P30TIG;
		refsopt.REFSMMATdirect = true;
		refsopt.REFSMMATopt = 1;

		REFSMMAT = REFSMMATCalc(&refsopt); //REFSMMAT for uplink

		opt.vessel = calcParams.src;
		opt.GETbase = getGETBase();
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.engopt = 0;
		opt.HeadsUp = true;
		opt.sxtstardtime = -25 * 60;
		opt.REFSMMAT = REFSMMAT;
		opt.navcheckGET = 8 * 60 * 60 + 17 * 60;

		AP7ManeuverPAD(&opt, *form);
	}
	break;
	case 3: //MISSION C 2ND PHASING MANEUVER
	{
		AP7ManPADOpt opt;
		LambertMan lambert;
		double P30TIG;
		VECTOR3 dV_LVLH;

		AP7MNV * form = (AP7MNV *)pad;

		lambert.axis = RTCC_LAMBERT_XAXIS;
		lambert.GETbase = getGETBase();
		lambert.impulsive = RTCC_IMPULSIVE;
		lambert.N = 7;
		lambert.Offset = _V(76.5 * 1852, 0, 0);
		lambert.Perturbation = RTCC_LAMBERT_SPHERICAL;
		lambert.PhaseAngle = 0;
		lambert.prograde = RTCC_LAMBERT_PROGRADE;
		lambert.T1 = 15 * 60 * 60 + 52 * 60;
		lambert.T2 = 26 * 60 * 60 + 25 * 60;
		lambert.target = calcParams.tgt;
		lambert.vessel = calcParams.src;

		LambertTargeting(&lambert, dV_LVLH, P30TIG);

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.engopt = 1; //+X RCS Thrusters
		opt.HeadsUp = true;
		opt.sxtstardtime = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC();
		opt.navcheckGET = 0;

		AP7ManeuverPAD(&opt, *form);
	}
	break;
	case 4: //MISSION C NCC1 MANEUVER
	{
		LambertMan lambert;
		AP7ManPADOpt opt;
		double P30TIG;
		VECTOR3 dV_LVLH;

		AP7MNV * form = (AP7MNV *)pad;

		lambert.axis = RTCC_LAMBERT_MULTIAXIS;
		lambert.GETbase = getGETBase();
		lambert.impulsive = RTCC_NONIMPULSIVE;
		lambert.N = 1;
		lambert.Offset = _V(0, 0, 8 * 1852);
		lambert.Perturbation = RTCC_LAMBERT_PERTURBED;
		lambert.PhaseAngle = -1.32*RAD;
		lambert.prograde = RTCC_LAMBERT_PROGRADE;
		lambert.T1 = 26 * 60 * 60 + 25 * 60;
		lambert.T2 = 28 * 60 * 60;
		lambert.target = calcParams.tgt;
		lambert.vessel = calcParams.src;

		LambertTargeting(&lambert, dV_LVLH, P30TIG);

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.engopt = 0;
		opt.HeadsUp = true;
		opt.sxtstardtime = -30 * 60;
		opt.REFSMMAT = GetREFSMMATfromAGC();
		opt.navcheckGET = 25 * 60 * 60 + 42 * 60;

		AP7ManeuverPAD(&opt, *form);
	}
	break;
	case 5: //MISSION C NCC2 MANEUVER
	{
		LambertMan lambert;
		AP7ManPADOpt opt;
		double P30TIG;
		VECTOR3 dV_LVLH;

		AP7MNV * form = (AP7MNV *)pad;

		lambert.axis = RTCC_LAMBERT_MULTIAXIS;
		lambert.GETbase = getGETBase();
		lambert.impulsive = RTCC_IMPULSIVE;
		lambert.N = 0;
		lambert.Offset = _V(0, 0, 8 * 1852);
		lambert.Perturbation = RTCC_LAMBERT_PERTURBED;
		lambert.PhaseAngle = -1.32*RAD;
		lambert.prograde = RTCC_LAMBERT_PROGRADE;
		lambert.T1 = 27 * 60 * 60 + 30 * 60;
		lambert.T2 = 28 * 60 * 60;
		lambert.target = calcParams.tgt;
		lambert.vessel = calcParams.src;

		LambertTargeting(&lambert, dV_LVLH, P30TIG);

		opt.GETbase = getGETBase();
		opt.vessel = calcParams.src;
		opt.TIG = P30TIG;
		opt.dV_LVLH = dV_LVLH;
		opt.engopt = 1;
		opt.HeadsUp = false;
		opt.sxtstardtime = 0;
		opt.REFSMMAT = GetREFSMMATfromAGC();
		opt.navcheckGET = 0;

		AP7ManeuverPAD(&opt, *form);
	}
	break;
	case 30: //GENERIC STATE VECTOR UPDATE
	{
		double SVGET;
		VECTOR3 R0, V0;

		SVGET = 0;
		StateVectorCalc(calcParams.src, SVGET, R0, V0); //State vector for uplink
	}
	break;
	}
}

void RTCC::EntryTargeting(EntryOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude)
{
	Entry* entry;
	double SVMJD, GET;
	VECTOR3 RA0_orb, VA0_orb;
	bool stop;

	stop = false;
	
	opt->vessel->GetRelativePos(AGCGravityRef(opt->vessel), RA0_orb);
	opt->vessel->GetRelativeVel(AGCGravityRef(opt->vessel), VA0_orb);
	SVMJD = oapiGetSimMJD();
	GET = (SVMJD - opt->GETbase)*24.0*3600.0;

	entry = new Entry(opt->vessel, AGCGravityRef(opt->vessel), opt->GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->type, opt->Range, opt->nominal, opt->entrylongmanual);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	dV_LVLH = entry->Entry_DV;
	P30TIG = entry->EntryTIGcor;
	latitude = entry->EntryLatcor;
	longitude = entry->EntryLngcor;

	delete entry;

	if (opt->impulsive == RTCC_NONIMPULSIVE)
	{
		VECTOR3 Llambda, RA1_cor, VA1_cor, R0,V0,RA1,VA1,UX,UY,UZ,DV,i,j,k;
		double t_slip;
		MATRIX3 Rot, Q_Xx;
		
		Rot = OrbMech::J2000EclToBRCS(40222.525);

		R0 = mul(Rot, _V(RA0_orb.x, RA0_orb.z, RA0_orb.y));
		V0 = mul(Rot, _V(VA0_orb.x, VA0_orb.z, VA0_orb.y));

		OrbMech::oneclickcoast(R0, V0, SVMJD, P30TIG - GET, RA1, VA1, AGCGravityRef(opt->vessel), oapiGetObjectByName("Earth"));

		UY = unit(crossp(VA1, RA1));
		UZ = unit(-RA1);
		UX = crossp(UY, UZ);

		DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;

		OrbMech::impulsive(opt->vessel, RA1, VA1, AGCGravityRef(opt->vessel), opt->vessel->GetGroupThruster(THGROUP_MAIN, 0), DV, Llambda, t_slip);
		OrbMech::oneclickcoast(RA1, VA1, SVMJD + (P30TIG - GET) / 24.0 / 3600.0, t_slip, RA1_cor, VA1_cor, AGCGravityRef(opt->vessel), AGCGravityRef(opt->vessel));

		j = unit(crossp(VA1_cor, RA1_cor));
		k = unit(-RA1_cor);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG += t_slip;
	}
}

void RTCC::LambertTargeting(LambertMan *lambert, VECTOR3 &dV_LVLH, double &P30TIG)
{
	VECTOR3 RA0, VA0, RP0, VP0, RA0_orb, VA0_orb, RP0_orb, VP0_orb;
	VECTOR3 RA1, VA1, RP2, VP2;
	MATRIX3 Rot;
	double SVMJD,dt1,dt2,mu;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(lambert->vessel);

	lambert->vessel->GetRelativePos(gravref, RA0_orb);
	lambert->vessel->GetRelativeVel(gravref, VA0_orb);
	lambert->target->GetRelativePos(gravref, RP0_orb);
	lambert->target->GetRelativeVel(gravref, VP0_orb);
	SVMJD = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);
	mu = GGRAV*oapiGetMass(gravref);

	RA0 = mul(Rot, _V(RA0_orb.x, RA0_orb.z, RA0_orb.y));
	VA0 = mul(Rot, _V(VA0_orb.x, VA0_orb.z, VA0_orb.y));
	RP0 = mul(Rot, _V(RP0_orb.x, RP0_orb.z, RP0_orb.y));
	VP0 = mul(Rot, _V(VP0_orb.x, VP0_orb.z, VP0_orb.y));

	dt1 = lambert->T1 - (SVMJD - lambert->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = lambert->T2 - lambert->T1;

	if (lambert->Perturbation == 1)
	{
		OrbMech::oneclickcoast(RA0, VA0, SVMJD, dt1, RA1, VA1, gravref, gravref);
		OrbMech::oneclickcoast(RP0, VP0, SVMJD, dt1 + dt2, RP2, VP2, gravref, gravref);
	}
	else
	{
		OrbMech::rv_from_r0v0(RA0, VA0, dt1, RA1, VA1, mu);
		OrbMech::rv_from_r0v0(RP0, VP0, dt1 + dt2, RP2, VP2, mu);
	}

	VECTOR3 i, j, k, yvec,RP2off,VA1_apo;
	double angle;
	MATRIX3 Q_Xx2, Q_Xx;

	k = -RP2 / length(RP2);
	j = crossp(VP2, RP2) / length(RP2) / length(VP2);
	i = crossp(j, k);
	Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	
	RP2off = RP2 + tmul(Q_Xx2, _V(0.0, lambert->Offset.y, lambert->Offset.z));

	if (lambert->PhaseAngle != 0)
	{
		angle = lambert->PhaseAngle;
	}
	else
	{
		angle = lambert->Offset.x / length(RP2);
	}

	yvec = _V(Q_Xx2.m21, Q_Xx2.m22, Q_Xx2.m23);
	RP2off = OrbMech::RotateVector(yvec, -angle, RP2off);

	if (lambert->Perturbation == RTCC_LAMBERT_PERTURBED)
	{
		VA1_apo = OrbMech::Vinti(RA1, VA1, RP2off, SVMJD + dt1 / 24.0 / 3600.0, dt2, lambert->N, lambert->prograde, gravref); //Vinti Targeting: For non-spherical gravity
	}
	else
	{
		if (lambert->axis == RTCC_LAMBERT_MULTIAXIS)
		{
			VA1_apo = OrbMech::elegant_lambert(RA1, VA1, RP2off, dt2, lambert->N, lambert->prograde, mu);	//Lambert Targeting
		}
		else
		{
			OrbMech::xaxislambert(RA1, VA1, RP2off, dt2, lambert->N, lambert->prograde, mu, VA1_apo, lambert->Offset.z);	//Lambert Targeting
		}
	}

	if (lambert->impulsive == RTCC_IMPULSIVE)
	{
		j = unit(crossp(VA1, RA1));
		k = unit(-RA1);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z); 
		dV_LVLH = mul(Q_Xx, VA1_apo - VA1);
		P30TIG = lambert->T1;
	}
	else
	{
		VECTOR3 Llambda,RA1_cor,VA1_cor;
		double t_slip;
		

		OrbMech::impulsive(lambert->vessel, RA1, VA1, gravref, lambert->vessel->GetGroupThruster(THGROUP_MAIN, 0), VA1_apo - VA1, Llambda, t_slip);
		OrbMech::rv_from_r0v0(RA1, VA1, t_slip, RA1_cor, VA1_cor, mu);

		j = unit(crossp(VA1_cor, RA1_cor));
		k = unit(-RA1_cor);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG = lambert->T1 + t_slip;
	}

	if (lambert->axis == RTCC_LAMBERT_XAXIS)
	{
		dV_LVLH.y = 0.0;
	}
}

void RTCC::AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad)
{
	VECTOR3 DV_P, DV_C, V_G;
	double SVMJD, dt, mu, theta_T, t_go;
	VECTOR3 R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, X_B, DV, U_TD, R2B, V2B, Att;
	MATRIX3 Rot, M, M_R, M_RTM;
	double p_T, y_T, peri, apo, m1;
	double headsswitch;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(opt->vessel);

	mu = GGRAV*oapiGetMass(gravref);

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	dt = opt->TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
	V0B = mul(Rot, _V(V_A.x, V_A.z, V_A.y));

	OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);

	UY = unit(crossp(V1B, R1B));
	UZ = unit(-R1B);
	UX = crossp(UY, UZ);

	DV = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;

	pad.Weight = opt->vessel->GetMass() / 0.45359237;

	double v_e, F;
	if (opt->engopt == 0)
	{
		v_e = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
		F = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	pad.burntime = v_e / F *opt->vessel->GetMass()*(1.0 - exp(-length(opt->dV_LVLH) / v_e));

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	if (opt->engopt == 0)
	{
		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		if (length(DV_P) != 0.0)
		{
			theta_T = length(crossp(R1B, V1B))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
			DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*opt->dV_LVLH.y;
		}
		else
		{
			V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
		}

		U_TD = unit(V_G);

		OrbMech::poweredflight(opt->vessel, R1B, V1B, gravref, opt->vessel->GetGroupThruster(THGROUP_MAIN, 0), V_G, R2B, V2B, t_go);

		OrbMech::periapo(R2B, V2B, mu, apo, peri);
		pad.HA = apo - oapiGetSize(gravref);
		pad.HP = peri - oapiGetSize(gravref);

		p_T = -2.15*RAD;
		y_T = 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, R1B*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::transpose_matrix(M_R), M);

		m1 = opt->vessel->GetMass()*exp(-length(opt->dV_LVLH) / v_e);
		pad.Vc = length(opt->dV_LVLH)*cos(-2.15*RAD)*cos(0.95*RAD) - 60832.18 / m1;
	}
	else
	{
		OrbMech::periapo(R1B, V1B + DV, mu, apo, peri);
		pad.HA = apo - oapiGetSize(gravref);
		pad.HP = peri - oapiGetSize(gravref);

		V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
		X_B = unit(V_G);
		if (opt->engopt == 1)
		{
			UX = X_B;
		}
		else
		{
			UX = -X_B;
		}
		UY = unit(crossp(UX, R1B*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

		pad.Vc = length(opt->dV_LVLH);
	}
	Att = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	//GDCangles = OrbMech::backupgdcalignment(REFSMMAT, R1B, oapiGetSize(gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, opt->sxtstardtime, Rsxt, Vsxt, gravref, gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(round(Att.x*DEG)*RAD, round(Att.y*DEG)*RAD, round(Att.z*DEG)*RAD), Rsxt, oapiGetSize(gravref), pad.Star, pad.Trun, pad.Shaft);

	if (opt->navcheckGET != 0.0)
	{
		VECTOR3 Rnav, Vnav;
		double alt, lat, lng;
		OrbMech::oneclickcoast(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, opt->navcheckGET - opt->TIG, Rnav, Vnav, gravref, gravref);
		navcheck(Rnav, Vnav, opt->GETbase + opt->navcheckGET / 24.0 / 3600.0, gravref, lat, lng, alt);

		pad.NavChk = opt->navcheckGET;
		pad.lat = lat*DEG;
		pad.lng = lng*DEG;
		pad.alt = alt / 1852;
	}

	pad.Att = _V(OrbMech::imulimit(Att.x*DEG), OrbMech::imulimit(Att.y*DEG), OrbMech::imulimit(Att.z*DEG));

	pad.GETI = opt->TIG;
	pad.pTrim = 0;
	pad.yTrim = 0;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.Vc /= 0.3048;
	pad.Shaft *= DEG;
	pad.Trun *= DEG;
	pad.HA /= 1852.0;
	pad.HP /= 1852.0;
}

MATRIX3 RTCC::GetREFSMMATfromAGC()
{
	MATRIX3 REFSMMAT;
	char Buffer[100];
	int REFSMMAToct[20];

	if (mcc->cm->IsVirtualAGC() == FALSE)
	{
		
	}
	else
	{
		unsigned short REFSoct[20];
		REFSoct[2] = mcc->cm->agc.vagc.Erasable[0][01735];
		REFSoct[3] = mcc->cm->agc.vagc.Erasable[0][01736];
		REFSoct[4] = mcc->cm->agc.vagc.Erasable[0][01737];
		REFSoct[5] = mcc->cm->agc.vagc.Erasable[0][01740];
		REFSoct[6] = mcc->cm->agc.vagc.Erasable[0][01741];
		REFSoct[7] = mcc->cm->agc.vagc.Erasable[0][01742];
		REFSoct[8] = mcc->cm->agc.vagc.Erasable[0][01743];
		REFSoct[9] = mcc->cm->agc.vagc.Erasable[0][01744];
		REFSoct[10] = mcc->cm->agc.vagc.Erasable[0][01745];
		REFSoct[11] = mcc->cm->agc.vagc.Erasable[0][01746];
		REFSoct[12] = mcc->cm->agc.vagc.Erasable[0][01747];
		REFSoct[13] = mcc->cm->agc.vagc.Erasable[0][01750];
		REFSoct[14] = mcc->cm->agc.vagc.Erasable[0][01751];
		REFSoct[15] = mcc->cm->agc.vagc.Erasable[0][01752];
		REFSoct[16] = mcc->cm->agc.vagc.Erasable[0][01753];
		REFSoct[17] = mcc->cm->agc.vagc.Erasable[0][01754];
		REFSoct[18] = mcc->cm->agc.vagc.Erasable[0][01755];
		REFSoct[19] = mcc->cm->agc.vagc.Erasable[0][01756];
		for (int i = 2; i < 20; i++)
		{
			sprintf(Buffer, "%05o", REFSoct[i]);
			REFSMMAToct[i] = atoi(Buffer);
		}

		REFSMMAT.m11 = OrbMech::DecToDouble(REFSoct[2], REFSoct[3])*2.0;
		REFSMMAT.m12 = OrbMech::DecToDouble(REFSoct[4], REFSoct[5])*2.0;
		REFSMMAT.m13 = OrbMech::DecToDouble(REFSoct[6], REFSoct[7])*2.0;
		REFSMMAT.m21 = OrbMech::DecToDouble(REFSoct[8], REFSoct[9])*2.0;
		REFSMMAT.m22 = OrbMech::DecToDouble(REFSoct[10], REFSoct[11])*2.0;
		REFSMMAT.m23 = OrbMech::DecToDouble(REFSoct[12], REFSoct[13])*2.0;
		REFSMMAT.m31 = OrbMech::DecToDouble(REFSoct[14], REFSoct[15])*2.0;
		REFSMMAT.m32 = OrbMech::DecToDouble(REFSoct[16], REFSoct[17])*2.0;
		REFSMMAT.m33 = OrbMech::DecToDouble(REFSoct[18], REFSoct[19])*2.0;
	}
	return REFSMMAT;
}

void RTCC::navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt)
{
	//R and V in the BRCS

	MATRIX3 Rot, Rot2;
	VECTOR3 Requ, Recl, u;
	double sinl, a, b, gamma,r_0;

	a = 6378166;
	b = 6356784;

	Rot = OrbMech::J2000EclToBRCS(40222.525);
	Rot2 = OrbMech::GetRotationMatrix2(gravref, MJD);

	Recl = tmul(Rot, R);
	Recl = _V(Recl.x, Recl.z, Recl.y);

	Requ = tmul(Rot2, Recl);
	Requ = _V(Requ.x, Requ.z, Requ.y);

	u = unit(Requ);
	sinl = u.z;

	if (gravref == oapiGetObjectByName("Earth"))
	{
		gamma = b*b / a / a;
	}
	else
	{
		gamma = 1;
	}
	r_0 = oapiGetSize(gravref);

	lat = atan(u.z/(gamma*sqrt(u.x*u.x + u.y*u.y)));
	lng = atan2(u.y, u.x);
	alt = length(Requ) - r_0;
}

void RTCC::StateVectorCalc(VESSEL *vessel, double &SVGET, VECTOR3 &BRCSPos, VECTOR3 &BRCSVel)
{
	VECTOR3 R, V, R0B, V0B, R1B, V1B;
	MATRIX3 Rot;
	double SVMJD, dt, GETbase;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	SVMJD = oapiGetSimMJD();
	GETbase = getGETBase();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R.x, R.z, R.y));
	V0B = mul(Rot, _V(V.x, V.z, V.y));

	if (SVGET != 0.0)
	{
		dt = SVGET - (SVMJD - GETbase)*24.0*3600.0;
		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
	}
	else
	{
		SVGET = (SVMJD - GETbase)*24.0*3600.0;
		R1B = R0B;
		V1B = V0B;
	}

	//oapiGetPlanetObliquityMatrix(gravref, &obl);
	//convmat = mul(_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0),mul(transpose_matrix(obl),_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0)));

	BRCSPos = R1B;
	BRCSVel = V1B;
}

OBJHANDLE RTCC::AGCGravityRef(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 rsph;

	gravref = oapiGetObjectByName("Moon");
	vessel->GetRelativePos(gravref, rsph);
	if (length(rsph) > 64373760.0)
	{
		gravref = oapiGetObjectByName("Earth");
	}
	return gravref;
}

double RTCC::getGETBase()
{
	double GET, SVMJD;
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();
	return SVMJD - GET / 24.0 / 3600.0;
}

MATRIX3 RTCC::REFSMMATCalc(REFSMMATOpt *opt)
{
	double SVMJD, dt, mu;
	VECTOR3 R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, X_B;
	MATRIX3 Rot;
	VECTOR3 DV_P, DV_C, V_G, X_SM, Y_SM, Z_SM;
	double theta_T, t_go;
	OBJHANDLE hMoon, hEarth, gravref;

	gravref = AGCGravityRef(opt->vessel);

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R_A = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
	V_A = mul(Rot, _V(V_A.x, V_A.z, V_A.y));

	if (opt->REFSMMATdirect == false)
	{
		OrbMech::oneclickcoast(R_A, V_A, SVMJD, opt->P30TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0, R0B, V0B, gravref, opt->maneuverplanet);

		UY = unit(crossp(V0B, R0B));
		UZ = unit(-R0B);
		UX = crossp(UY, UZ);

		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		theta_T = length(crossp(R0B, V0B))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(R0B), 2.0) / 92100.0;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*opt->dV_LVLH.y;

		OrbMech::poweredflight(opt->vessel, R0B, V0B, opt->maneuverplanet, opt->vessel->GetGroupThruster(THGROUP_MAIN, 0), V_G, R0B, V0B, t_go);

		//DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
		//V0B = V0B + DV;
		SVMJD += (t_go + opt->P30TIG) / 24.0 / 60.0 / 60.0 - (SVMJD - opt->GETbase);
	}
	else
	{
		R0B = R_A;
		V0B = V_A;
	}

	if (opt->REFSMMATopt == 4)
	{
		//TODO: Nominal launchpad REFSMMATs for all missions
		/*if (mission == 7)
		{
		REFSMMAT = A7REFSMMAT;
		}
		else if (mission == 8)
		{
		REFSMMAT = A8REFSMMAT;
		}*/
		return _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	}
	else if (opt->REFSMMATopt == 5)
	{
		double LSMJD;
		VECTOR3 R_P, R_LS, H_C;
		MATRIX3 Rot2;

		LSMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat)));

		Rot2 = OrbMech::GetRotationMatrix2(oapiGetObjectByName("Moon"), LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));

		OrbMech::oneclickcoast(R0B, V0B, SVMJD, (LSMJD - SVMJD)*24.0*3600.0, R1B, V1B, gravref, hMoon);

		H_C = crossp(R1B, V1B);

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else if (opt->REFSMMATopt == 6)
	{
		double *MoonPos;
		double PTCMJD;
		VECTOR3 R_ME;

		MoonPos = new double[12];

		PTCMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		OBJHANDLE hMoon = oapiGetObjectByName("Moon");
		CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);

		cMoon->clbkEphemeris(PTCMJD, EPHEM_TRUEPOS, MoonPos);

		R_ME = -_V(MoonPos[0], MoonPos[1], MoonPos[2]);

		UX = unit(crossp(_V(0.0, 1.0, 0.0), unit(R_ME)));
		UX = mul(Rot, _V(UX.x, UX.z, UX.y));

		UZ = unit(mul(Rot, _V(0.0, 0.0, -1.0)));
		UY = crossp(UZ, UX);
		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else
	{
		mu = GGRAV*oapiGetMass(gravref);

		if (opt->REFSMMATopt == 2)
		{
			dt = opt->REFSMMATTime - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
			OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
		}
		else if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			dt = opt->P30TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
			OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
		}
		else
		{
			dt = OrbMech::time_radius_integ(R0B, V0B, SVMJD, oapiGetSize(hEarth) + 400000.0*0.3048, -1, hEarth, hEarth, R1B, V1B);
		}

		UY = unit(crossp(V1B, R1B));
		UZ = unit(-R1B);
		UX = crossp(UY, UZ);



		if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			MATRIX3 M, M_R, M_RTM;
			double p_T, y_T;

			DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
			if (length(DV_P) != 0.0)
			{
				theta_T = length(crossp(R1B, V1B))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
				DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
				V_G = DV_C + UY*opt->dV_LVLH.y;
			}
			else
			{
				V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
			}
			if (opt->REFSMMATopt == 0)
			{
				p_T = -2.15*RAD;
				X_B = unit(V_G);// tmul(REFSMMAT, dV_LVLH));
			}
			else
			{
				p_T = 2.15*RAD;
				X_B = -unit(V_G);// tmul(REFSMMAT, dV_LVLH));
			}
			UX = X_B;
			UY = unit(crossp(X_B, R1B));
			UZ = unit(crossp(X_B, crossp(X_B, R1B)));


			y_T = 0.95*RAD;

			M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
			M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
			M_RTM = mul(OrbMech::transpose_matrix(M_R), M);
			X_SM = mul(M_RTM, _V(1.0, 0.0, 0.0));
			Y_SM = mul(M_RTM, _V(0.0, 1.0, 0.0));
			Z_SM = mul(M_RTM, _V(0.0, 0.0, 1.0));
			return _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

			//IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(transpose_matrix(M), M_R));
			//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);
		}
		else
		{
			return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		}
		//
	}

	/*a = REFSMMAT;

	if (REFSMMATupl == 0)
	{
	if (vesseltype == CSM)
	{
	REFSMMAToct[1] = 306;
	}
	else
	{
	REFSMMAToct[1] = 3606;
	}
	}
	else
	{
	if (vesseltype == CSM)
	{
	REFSMMAToct[1] = 1735;
	}
	else
	{
	REFSMMAToct[1] = 1733;
	}
	}

	REFSMMAToct[0] = 24;
	REFSMMAToct[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	REFSMMAToct[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	REFSMMAToct[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	REFSMMAToct[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	REFSMMAToct[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	REFSMMAToct[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	REFSMMAToct[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	REFSMMAToct[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	REFSMMAToct[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	REFSMMAToct[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	REFSMMAToct[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	REFSMMAToct[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	REFSMMAToct[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	REFSMMAToct[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	REFSMMAToct[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	REFSMMAToct[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	REFSMMAToct[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	REFSMMAToct[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	REFSMMATcur = REFSMMATopt;*/
}