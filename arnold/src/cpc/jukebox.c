/* Arnold JukeBox */
/* fbe1, fbe2, fbe3, fbe0 */

/* fbe0 is read */
/* fbe2 is read */
/* fbe1 select cartridge */
/* fbe3 is write */

/* an array of cartridges */
/* first element is actually the jukebox cartridge itself */
#include "cpc.h"
#include "jukebox.h"
void Jukebox_SelectCartridge(int nCartridge);

static int Jukebox_CurrentCartridge;

/* bit 7: ?? */
/* bits 6..4 define time in seconds for play */
/* 0 = infinite, 1 = 8 seconds, 2 = 32 seconds, 3 = 16 seconds, 4 = 66 seconds */
/* bits 3..0 define number of plays */

/* seems bits 2..0 define something. delay? */
static int Jukebox_Control;

/* bits 7..4 define index of selected cartridge */
static int Jukebox_SelectedCartridge;

static BOOL Jukebox_EnableState;

typedef struct
{
	unsigned char *pFilename;
	unsigned char *pData;
	unsigned long nLength;
	BOOL bEnabled;
} Jukebox_Cartridge;

/* 12 cartridges */
static Jukebox_Cartridge Cartridges[14];

void Jukebox_Enable(BOOL bEnable)
{
	Jukebox_EnableState = bEnable;
}

BOOL Jukebox_IsEnabled()
{
	return Jukebox_EnableState;
}



void Jukebox_Reset()
{
	Jukebox_SelectCartridge(0);
}

void Jukebox_SetCartridgeEnable(int nCartridge, BOOL bEnable)
{
	if (nCartridge<=12)
	{
		Cartridges[nCartridge].bEnabled = bEnable;
	}
}

BOOL Jukebox_IsCartridgeEnabled(int nCartridge)
{
	return Cartridges[nCartridge].bEnabled;
}

const char *Jukebox_GetCartridgeFilename(int nCartridge)
{
	return Cartridges[nCartridge].pFilename;
}

/* up to 12 cartridges */
void Jukebox_Init()
{
	int i;

	for (i=0; i<14; i++)
	{
		Cartridges[i].pFilename = NULL;
		Cartridges[i].pData = NULL;
		Cartridges[i].nLength = 0;
	}
	Jukebox_SelectedCartridge = 0;

	Cartridges[0].bEnabled = TRUE;
	Jukebox_EnableState = FALSE;
}

void Jukebox_InsertSystemCartridge(const unsigned char *pCartridgeData, const unsigned long CartridgeLength)
{
	Cartridges[0].pData = pCartridgeData;
	Cartridges[0].nLength = CartridgeLength;
}

void Jukebox_Finish()
{
	int i;

	/* do not free system cartridge */
	for (i=1; i<=12; i++)
	{
		SetString(&Cartridges[i],NULL);

		if (Cartridges[i].pData!=NULL)
		{
			free(Cartridges[i].pData);
			Cartridges[i].pData = NULL;
		}
		Cartridges[i].nLength = 0;
	}
}

extern BOOL LoadFile(const char *pFilename, unsigned char **ppLocation, unsigned long *pLength);
void Jukebox_CartridgeInsert(int nCartridge, char *pFilename)
{
	LoadFile(pFilename, &Cartridges[nCartridge].pData, &Cartridges[nCartridge].nLength);
	SetString(&Cartridges[nCartridge].pFilename, pFilename);
}

void Jukebox_SelectCartridge(int nCartridge)
{

	// 0x0c0 what does that mean?
	//
	Jukebox_CurrentCartridge = nCartridge & 0x01f;

	if (
		/* cartridge index is valid? */
		(Jukebox_CurrentCartridge>12) ||
		/* cartridge data loaded? */
		(Cartridges[Jukebox_CurrentCartridge].pData==NULL)
		/* cartridge enabled in UI? */
		/* || (!Cartridges[Jukebox_CurrentCartridge].bEnabled)*/
		)
	{
		/* if no cartridge exists here then map empty memory */
		Cartridge_Remove();
	}
	else
	{
		/* insert the cartridge */
		Cartridge_Insert(Cartridges[Jukebox_CurrentCartridge].pData, Cartridges[Jukebox_CurrentCartridge].nLength);
	}


	/* refresh to get rom to work */
	ASIC_SetSecondaryRomMapping(ASIC_GetSecondaryRomMapping());

	ExpansionROM_RefreshTable();

	ROM_Select(ROM_GetSelectedROM());


}

void Jukebox_Write(Z80_WORD nAddr, Z80_BYTE nValue)
{
	switch (nAddr & 0x03)
	{
		case 0:
		case 2:
		case 3:
		{
			// 3 sets timer etc?
		}
		break;

		case 1:
		{
			Jukebox_SelectCartridge(nValue);
		}
		break;


		default:
			break;
	}
}


Z80_BYTE Jukebox_Read(Z80_WORD nAddr)
{
	switch (nAddr & 0x03)
	{
		case 0:
			// cpl and 3
			// or c
			return ~((1<<6));

		case 2:
			return Jukebox_SelectedCartridge;

		case 1:
		case 3:
			break;

		default:
			break;
	}

	return 0x0ff;
}

