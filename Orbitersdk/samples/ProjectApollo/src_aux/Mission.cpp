/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Mission File Handling

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

#include "Mission.h"
#include "OrbiterAPI.h"
#include <map>

std::map<std::string, mission::Mission*> pa_mission_hashmap;

DLLCLBK mission::Mission* paGetDefaultMission()
{
	return paGetMission("");
}

DLLCLBK mission::Mission* paGetMission(const std::string& mission_name)
{
	if (pa_mission_hashmap.find(mission_name) != pa_mission_hashmap.end())
	{
		oapiWriteLog("(Mission File Management) Found loaded mission file.");
		return pa_mission_hashmap[mission_name];
	}
	else {
		oapiWriteLog("(Mission File Management) Load mission file into shared memory.");
		mission::Mission* new_mission = new mission::Mission(mission_name);
		pa_mission_hashmap.insert(std::make_pair(mission_name, new_mission));
		return new_mission;
	}
}

void InitMissionManagementMemory()
{
	pa_mission_hashmap.clear();
}

void ClearMissionManagementMemory()
{
	std::map<std::string, mission::Mission*>::iterator iter
		= pa_mission_hashmap.begin();
	while (iter != pa_mission_hashmap.end())
	{
		if (iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
		iter++;
	}
}

namespace mission {

	Mission::Mission(const std::string& strMission)
	{
		SetDefaultValues();
		if (!strMission.empty())
		{
			if (!LoadMission(strMission))
			{
				oapiWriteLog("(Mission) failed creating mission from loaded file.");
			}
		}
	}

	Mission::~Mission()
	{
	}

	void Mission::SetDefaultValues()
	{
		iSMJCVersion = 1;
	}

	bool Mission::LoadMission(const int iMission)
	{
		return LoadMission("Apollo " + std::to_string(iMission));
	}

	bool Mission::LoadMission(const std::string& strMission)
	{
		char buffer[64];
		std::string filename;
		filename = "Missions\\ProjectApollo\\" + strMission + ".cfg";
		strFileName = strMission;

		sprintf_s(buffer, 64, "(Mission) Loading mission %s from file %s",
			strMission.c_str(), filename.c_str());
		oapiWriteLog(buffer);
		FILEHANDLE hFile = oapiOpenFile(filename.c_str(), FILE_IN);

		if (hFile == NULL)
		{
			oapiWriteLog("(Mission) ERROR: Can't open file.");
			return false;
		}

		if (oapiReadItem_string(hFile, "Name", buffer))
		{
			strMissionName = buffer;
		}
		oapiReadItem_int(hFile, "SMJCVersion", iSMJCVersion);

		oapiCloseFile(hFile, FILE_IN);
		return true;
	}

	int Mission::GetSMJCVersion() const
	{
		return iSMJCVersion;
	}
}