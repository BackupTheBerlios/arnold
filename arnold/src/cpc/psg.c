/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "cpcdefs.h"
#include "cpc.h"
#include "psg.h"
#include "cpcglob.h"


#ifdef AY_OUTPUT
#include "dumpym.h"
#endif

/* stores current selected register */
static int		PSG_SelectedRegister;
/* stores current register data */
static int		PSG_Registers[16];
/* these are anded with the data when a read is done */
static int		PSG_ReadAndMask[16] =
{
	0x0ff,	/* channel A tone fine */
	0x00f,	/* channel A tone coarse */
	0x0ff,	/* channel B tone fine */
	0x00f,	/* channel B tone coarse */
	0x0ff,	/* channel C tone fine */
	0x00f,	/* channel C tone coarse */
	0x01f,	/* noise */
	0x07f,	/* mixer */
	0x01f,	/* volume A */
	0x01f,	/* volume B */
	0x01f,	/* volume C */
	0x0ff,	/* hardware envelope duration fine */
	0x0ff,	/* hardware envelope duration coarse */
	0x00f,	/* hardware envelope */
	0x0ff,	/* I/O port A */
	0x0ff	/* I/O port B */
};

void	PSG_Init(void)
{
	PSGPlay_Initialise();
}


/* reset PSG and put it into it's initial state */
void	PSG_Reset(void)
{
	int i;

	/* reset all registers to 0 - as per the Ay-3-8912 spec */
	for (i=0; i<15; i++)
	{
		PSG_RegisterSelect(i);
		PSG_WriteData(0);
	}

	/* when read, reports 0x0ff */
	PSG_Registers[15] = 0x0ff;

	PSGPlay_Reset();
}

unsigned int		PSG_ReadData(void)
{
	if (PSG_SelectedRegister!=14)
	{
		return PSG_Registers[PSG_SelectedRegister];
	}
	else
	{
		if ((PSG_Registers[7] & 0x040)==0)
			return Keyboard_Read();
		else
			return 0x0ff;
	}
}

void	PSG_WriteData(unsigned int Data )
{
	unsigned char RegisterData;

	/* and off bit's */
	RegisterData = (unsigned char)(Data & PSG_ReadAndMask[PSG_SelectedRegister]);

	/* write data to register */
	PSG_Registers[PSG_SelectedRegister] = RegisterData;

	/* when read, reports 0x0ff */
	PSG_Registers[15] = 0x0ff;

#ifdef AY_OUTPUT
	YMOutput_StoreRegData(PSG_SelectedRegister, RegisterData);
#endif

	/* write register for audio playback */
	PSGPlay_Write(PSG_SelectedRegister, RegisterData);
}


void	PSG_RegisterSelect(unsigned int Data)
{
	PSG_SelectedRegister = Data & 0x0f;
}

/* for debugging */
int		PSG_GetSelectedRegister(void)
{
	return PSG_SelectedRegister;
}

/* for debugging */
int		PSG_GetRegisterData(int RegisterIndex)
{
	RegisterIndex &= 0x0f;

	return PSG_Registers[RegisterIndex];
}
