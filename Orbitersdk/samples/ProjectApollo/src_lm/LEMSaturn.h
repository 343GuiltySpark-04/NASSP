/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

LEM Saturn (Header)

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

class LEMSaturn : public LEM {
	friend class LVDC1B;
public:
	LEMSaturn(OBJHANDLE hObj, int fmodel);
	virtual ~LEMSaturn();

protected:

	void ClearThrusters();
	void CalculateStageMass();
	void SetFirstStage();
	void SetFirstStageMeshes(double offset);
	void SetFirstStageEngines();
	void SetSecondStage();
	void SetSecondStageMeshes(double offset);
	void SetSecondStageEngines();

	void SetNosecapMesh();

	int lemsat_stage;

	bool NosecapAttached;

	double ISP_FIRST_VAC, ISP_FIRST_SL, ISP_SECOND_VAC, ISP_SECOND_SL;
	double THRUST_FIRST_VAC, THRUST_SECOND_VAC;

	double SI_EmptyMass, SI_FuelMass;
	double SIVB_EmptyMass, SIVB_FuelMass;
	double LM_EmptyMass, LM_FuelMass;
	double SI_Mass, SIVB_Mass, LM_Mass;
	double Stage1Mass, Stage2Mass;

	double Offset1st;
	double TCPO;
	double STG1O;

	MESHHANDLE hStage1Mesh;
	MESHHANDLE hStage2Mesh;
	MESHHANDLE hInterstageMesh;
	MESHHANDLE hStageSLA1Mesh;
	MESHHANDLE hStageSLA2Mesh;
	MESHHANDLE hStageSLA3Mesh;
	MESHHANDLE hStageSLA4Mesh;

	// Mesh indexes
	int nosecapidx;
	int meshLM_1;

	//
	// Surfaces.
	//
	SURFHANDLE J2Tex;

	THRUSTER_HANDLE th_1st[8], th_3rd[1], th_3rd_lox, th_ver[3];
	THGROUP_HANDLE thg_1st, thg_3rd, thg_ver;

	PROPELLANT_HANDLE ph_1st, ph_3rd, ph_ullage3;

	IU* iu;
	SIBSystems *sib;
	SIVBSystems *sivb;
};