#include "fdi.h"
#include "fdd.h"
#include "diskimage/diskimg.h"

/* floppy disc interface sits between the fdc and the
fdd */

static FDI fdi;

/* ABBA drive switch; use to swap drives */
static unsigned long FDI_DriveSwitch = 0;
/* side switch; used to swap sides */
static unsigned long FDI_SideSwitch = 0;
/* disc motor output from floppy disc interface */
static int FDI_MotorState;
static unsigned long FDI_DriveMask = 0x01;

/* swap drives */
void FDI_SwapDrives(void)
{
	FDI_DriveSwitch^=0x01;
}

/*-----------------------------------------------------------------------*/
/* get state of motor output from floppy disc interface */
BOOL FDI_GetMotorState(void)
{
	if ((FDI_MotorState & 0x01)==0x01)
	{
		return TRUE;
	}

	return FALSE;
}

/*-----------------------------------------------------------------------*/
/* set state of motor output from floppy disc interface */
void	FDI_SetMotorState(int MotorStatus)
{
	int i;

	FDI_MotorState = MotorStatus & 0x01;

	for (i=0; i<4; i++)
	{
		FDD_RefreshMotorState(i);
	}

}

BOOL FDI_Get4Drives()
{
	return (FDI_DriveMask==0x03);
}

void FDI_Set4Drives(BOOL bFourDrives)
{
	if (bFourDrives)
	{
		FDI_DriveMask = 0x03;
        FDD_Enable(2, TRUE);
        FDD_Enable(3, TRUE);

	}
	else
	{
		FDI_DriveMask = 0x01;
        FDD_Enable(2, FALSE);
        FDD_Enable(3, FALSE);
	}
}

BOOL FDI_GetSwapDrives(void)
{
	return (FDI_DriveSwitch!=0);
}

BOOL FDI_GetSwapSides(void)
{
	return (FDI_SideSwitch!=0);
}

/*-----------------------------------------------------------------------*/
/* floppy disc interface set drive */
void FDI_SetPhysicalDrive(unsigned long Value)
{
	/* map fdc drive output to physical drive selects */
	fdi.PhysicalDrive = Value & FDI_DriveMask;
	fdi.PhysicalDrive = fdi.PhysicalDrive ^ FDI_DriveSwitch;

	fdi.drive = FDD_GetDrive(fdi.PhysicalDrive);
}

void FDI_SetCurrentFDDLEDState(BOOL fState)
{
	FDD_LED_SetState(fdi.PhysicalDrive, fState);
}

unsigned long FDI_GetDriveFlags()
{
	return FDD_GetFlags(fdi.PhysicalDrive);
}

void FDI_SwapSides()
{
	FDI_SideSwitch^=1;
}

/*-----------------------------------------------------------------------*/

/* update physical side selection based on side output from fdc and
	side switch selection */
void FDI_SetPhysicalSide(unsigned long Value)
{
	/* double sided? */
	if (fdi.drive->Flags & FDD_FLAGS_DOUBLE_SIDED)
	{
		/* side can be changed */
		/* the value is effected by side output from FDC,
		Side Switch AND if the disc has been turned */

		fdi.PhysicalSide = Value^FDI_SideSwitch^FDD_GetPhysicalSide(fdi.PhysicalDrive);
	}
	else
	{
		/* side can't be changed */
		/* but allow the disc to be turned over like a 3" disc can be */
		fdi.PhysicalSide = FDD_GetPhysicalSide(fdi.PhysicalDrive);
	}
	fdi.drive->Flags &=~FDD_FLAGS_DRIVE_INDEX;

}
/*-----------------------------------------------------------------------*/
unsigned long FDI_GetSelectedDriveFlags(void)
{
	return FDD_GetFlags(fdi.PhysicalDrive);
}
/*-----------------------------------------------------------------------*/
void FDI_PerformStep(unsigned long DriveIndex, int Direction)
{
	FDI_SetPhysicalDrive(DriveIndex);

	FDD_PerformStep(fdi.PhysicalDrive, Direction);

	fdi.drive->CurrentIDIndex = 0;
	fdi.drive->Flags &=~FDD_FLAGS_DRIVE_INDEX;
}

unsigned long	FDI_GetNextID(CHRN *pCHRN)
{
	unsigned long SPT;

	SPT = DiskImage_GetSectorsPerTrack(
					fdi.PhysicalDrive,
					fdi.drive->CurrentTrack,
					fdi.PhysicalSide);

	if ((SPT==0) || (fdi.Density==0))
	{
		/* if there are no sectors on this track, we see a index pulse */
		return 1;
	}

	/* was index flag set last time around? */
	if ((fdi.drive->Flags & FDD_FLAGS_DRIVE_INDEX)==0)
	{
		/* no */

		/* increment */
		fdi.drive->CurrentIDIndex++;

		/* beyond last sector of this track? */
		if (fdi.drive->CurrentIDIndex==SPT)
		{
			/* yes, reset id */

			fdi.drive->CurrentIDIndex = 0;

			/* set index flag */
			fdi.drive->Flags |= FDD_FLAGS_DRIVE_INDEX;
			/* return index flag state */
			return 1;
		}
	}
	else
	{
		/* clear index flag, don't increment counter this time */
		fdi.drive->Flags &=~FDD_FLAGS_DRIVE_INDEX;
	}


	/* get the id */
	DiskImage_GetID(
		fdi.PhysicalDrive,
		fdi.drive->CurrentTrack,
		fdi.PhysicalSide,
		fdi.drive->CurrentIDIndex,pCHRN);

	return 0;
}


void	FDI_ReadSector(char *pBuffer)
{
	/* get sector data */
	DiskImage_GetSector(
		fdi.PhysicalDrive,
		fdi.drive->CurrentTrack,
		fdi.PhysicalSide,
		fdi.drive->CurrentIDIndex,pBuffer);
}

void	FDI_WriteSector(char *pBuffer, int Mark)
{
	DiskImage_PutSector(
		fdi.PhysicalDrive,
		fdi.drive->CurrentTrack,
		fdi.PhysicalSide,
		fdi.drive->CurrentIDIndex,pBuffer,Mark);
}

void	FDI_EmptyTrack(void)
{
	DiskImage_EmptyTrack(
		fdi.PhysicalDrive,
		fdi.drive->CurrentTrack,
		fdi.PhysicalSide
		);
}

void	FDI_AddSector(CHRN *pCHRN, int N, int Filler)
{
	DiskImage_AddSector(fdi.PhysicalDrive,
		fdi.drive->CurrentTrack,
		fdi.PhysicalSide, pCHRN,N, Filler);
}

void	FDI_SetDensity(int State)
{
	if (State!=0)
	{
		fdi.Density = 1;
	}
	else
	{
		fdi.Density = 0;
	}
}
