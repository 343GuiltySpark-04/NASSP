/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module Rendezvous Radar (Header)

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

///
/// \ingroup Connectors
/// \brief Message type to send to the CSM.
///
enum LM_RRmessageType{
	CW_RADAR_SIGNAL ///< Continuous Wave Radar Signal
};

class LEM_RR;

class LM_RRtoCSM_RRT_Connector : public Connector
{
public:
	LM_RRtoCSM_RRT_Connector(); //constructor
	~LM_RRtoCSM_RRT_Connector(); //descructor

	void SendRF(double freq, double XMITpow, double XMITgain, double Phase);
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	void SetRR(LEM_RR* lm_rr) { lemrr = lm_rr; };

protected:
	LEM_RR* lemrr; //pointer to the instance of the RR that's doing the sending
};

// Rendezvous Radar
class LEM_RR : public e_object {
public:
	LEM_RR();
	void Init(LEM *s, e_object *dc_src, e_object *ac_src, h_Radiator *ant, Boiler *anheat, Boiler *stbyanheat, h_HeatLoad *rreh, h_HeatLoad *secrreh, h_HeatLoad *rrh);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	void DefineAnimations(UINT idx);
	double GetAntennaTempF();
	double GetRadarTrunnionVel() { return -trunnionVel; };
	double GetRadarShaftVel() { return shaftVel; };
	double GetRadarTrunnionPos() { return -asin(sin(trunnionAngle)); }
	double GetRadarShaftPos() { return -asin(sin(shaftAngle)); }
	double GetRadarRange() { return range; };
	double GetRadarRate() { return rate; };
	double GetSignalStrength() { return SignalStrength * 4.0; }
	double GetShaftErrorSignal();
	double GetTrunnionErrorSignal();
	double GetTransmitterPower();
	double GetCSMGain(double theta, double phi, bool XPDRon); //returns the gain of the csm RRT system for returned power calculations
	double dBm2SignalStrength(double RecvdRRPower_dBm);

	bool IsPowered();
	bool IsDCPowered();
	bool IsACPowered();
	bool IsRadarDataGood() { return radarDataGood; };
	bool GetNoTrackSignal() { return NoTrackSignal; }

	virtual void ConnectRRToCSM(Connector *csmRRTconnector);

	LM_RRtoCSM_RRT_Connector* GetRR_to_RRT_Connector() { return &lm_rr_to_csm_connector; };

private:

	LEM * lem;					// Pointer at LEM
	h_Radiator *antenna;		// Antenna (loses heat into space)
	Boiler *antheater;			// Antenna Heater (puts heat back into antenna)
	Boiler *stbyantheater;		// Antenna Standby Heater (puts heat back into antenna)
	h_HeatLoad *rrheat;		// RR Heat Load
	h_HeatLoad *RREHeat;		// RRE Heat Load
	h_HeatLoad *RRESECHeat;		// RRE Heat Load Sec Loop
	e_object *dc_source;
	e_object *ac_source;
	double RangeLockTimer;
	double tstime;
	int    isTracking;
	bool   radarDataGood;
	bool NoTrackSignal;
	double trunnionAngle;
	double shaftAngle;
	double trunnionVel;
	double shaftVel;
	double range;
	double rate;
	double internalrange;
	double internalrangerate;
	int ruptSent;				// Rupt sent
	int scratch[2];             // Scratch data
	int mode;					//Mode I = false, Mode II = true
	double hpbw_factor;			//Beamwidth factor
	double SignalStrength;
	double SignalStrengthQuadrant[4];
	double AntennaFrequency;
	double XPDRpower;
	double AntennaGain;
	double AntennaWavelength;
	double AntennaPower;
	VECTOR3 U_RRL[4];
	bool AutoTrackEnabled;
	bool FrequencyLock;
	bool TrackingModeSwitch;
	bool RangeLock;
	double ShaftErrorSignal;
	double TrunnionErrorSignal;
	VECTOR3 GyroRates;
	// Animations
	UINT anim_RRPitch, anim_RRYaw;
	double rr_proc[2];
	double rr_proc_last[2];
	//connectors
	LM_RRtoCSM_RRT_Connector lm_rr_to_csm_connector;
};