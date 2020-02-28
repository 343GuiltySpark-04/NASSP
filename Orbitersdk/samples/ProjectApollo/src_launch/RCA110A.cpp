/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  RCA-110A Saturn Ground Computer

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
#include "PadLCCInterface.h"
#include "LCCPadInterface.h"
#include "RCA110A.h"

RCA110A::RCA110A()
{
	mode = 0;
	other = NULL;
}

RCA110A::~RCA110A()
{
	Disconnect();
}

void RCA110A::Connect(RCA110A *o)
{
	if (o)
	{
		other = o;
		other->other = this;
	}
}

void RCA110A::Disconnect()
{
	if (other)
	{
		other->other = NULL;
		other = NULL;
	}
}

void RCA110A::SetInput(size_t n, bool val)
{
	inputdiscretes.set(n, val);
}

void RCA110A::SetOutput(size_t n, bool val)
{
	outputdiscretes.set(n, val);
}

bool RCA110A::GetInputSignal(size_t n) const
{
	return inputdiscretes.test(n);
}

bool RCA110A::GetOutputSignal(size_t n) const
{
	return outputdiscretes.test(n);
}

void RCA110A::SwitchMode(int m)
{
	if (mode >= 0 && mode <= 4)
	{
		mode = m;
	}
}

RCA110AL::RCA110AL(PadLCCInterface *l)
{
	lcc = l;
}

void RCA110AL::Timestep(double simdt)
{
	lcc->SLCCCheckDiscreteInput(this);

	if (other == NULL) return;

	size_t n;
	for (size_t i = 0;i < RCA110A_INPUT_LINES;i++)
	{
		//First 696 signals are set directly
		if (i <= 695)
		{
			other->SetOutput(i, GetInputSignal(i));
		}

		//These switch signals on or off
		if (i >= 1104 && GetInputSignal(i))
		{
			n = i / 2 + 504;
			other->SetOutput(n, ~(i % 2));
		}
	}
}

RCA110AM::RCA110AM(LCCPadInterface *m)
{
	pad = m;
}

void RCA110AM::Timestep(double simdt)
{
	pad->SLCCCheckDiscreteInput(this);

	if (other == NULL) return;

	for (size_t i = 0;i < RCA110A_OUTPUT_LINES;i++)
	{
		//For now all inputs are directly sent to the LCC computer
		other->SetOutput(i, GetInputSignal(i));
	}
}