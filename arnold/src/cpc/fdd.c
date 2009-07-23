#include "fdd.h"
#include "fdi.h"

/* the two drives */
static FDD drive[MAX_DRIVES];

FDD		*FDD_GetDrive(int DriveIndex)
{
	return &drive[DriveIndex];
}

int FDD_GetTracks(int DriveIndex)
{
	return drive[DriveIndex].NumberOfTracks;
}

BOOL FDD_GetDoubleSided(int DriveIndex)
{
	return ((drive[DriveIndex].Flags & FDD_FLAGS_DOUBLE_SIDED)!=0);
}

unsigned long FDD_GetFlags(int DriveIndex)
{
	return drive[DriveIndex].Flags;
}

void FDD_SetAlwaysWriteProtected(int DriveIndex, BOOL bWriteProtected)
{
	drive[DriveIndex].bAlwaysWriteProtected = bWriteProtected;
    FDD_RefreshWriteProtect(DriveIndex);
}


BOOL FDD_IsAlwaysWriteProtected(int DriveIndex)
{
	return (drive[DriveIndex].bAlwaysWriteProtected);
}

void FDD_RefreshWriteProtect(int DriveIndex)
{
	if (drive[DriveIndex].bAlwaysWriteProtected)
	{
		drive[DriveIndex].Flags|= FDD_FLAGS_WRITE_PROTECTED;
	}
	else
	{
	    if (drive[DriveIndex].Flags & FDD_FLAGS_DISK_PRESENT)
	    {
            /* determine based on dsk? */
            drive[DriveIndex].Flags&=~FDD_FLAGS_WRITE_PROTECTED;
        }
        else
        {
            /* write protected is set if disc is not in drive */
            drive[DriveIndex].Flags|=FDD_FLAGS_WRITE_PROTECTED;
        }
	}
}


void FDD_SetDoubleSided(int DriveIndex, BOOL bDoubleSided)
{
	if (bDoubleSided)
	{
		drive[DriveIndex].Flags|= FDD_FLAGS_DOUBLE_SIDED;
	}
	else
	{
		drive[DriveIndex].Flags&=~FDD_FLAGS_DOUBLE_SIDED;
	}
}

void FDD_SetTracks(int DriveIndex, int nTracks)
{
	drive[DriveIndex].NumberOfTracks = nTracks;
}

void FDD_SetSingleSided40Track(int DriveIndex)
{
	FDD_SetDoubleSided(DriveIndex,FALSE);
	FDD_SetTracks(DriveIndex,43);
}

void FDD_SetDoubleSided80Track(int DriveIndex)
{
	FDD_SetDoubleSided(DriveIndex, TRUE);
	FDD_SetTracks(DriveIndex,83);
}

BOOL FDD_IsSingleSided40Track(int DriveIndex)
{
	if (FDD_GetDoubleSided(DriveIndex))
		return FALSE;

	if ((FDD_GetTracks(DriveIndex)>=40) && (FDD_GetTracks(DriveIndex)<=45))
		return TRUE;

	return FALSE;
}


BOOL FDD_IsDoubleSided80Track(int DriveIndex)
{
	if (!FDD_GetDoubleSided(DriveIndex))
		return FALSE;

	if ((FDD_GetTracks(DriveIndex)>=80) && (FDD_GetTracks(DriveIndex)<=85))
		return TRUE;

	return FALSE;
}

void	FDD_InitialiseAll(void)
{
	int i;

	for (i=0; i<MAX_DRIVES; i++)
	{
		FDD *theDrive = &drive[i];

		FDD_Initialise(i);

		if (i==0)
		{
			FDD_SetSingleSided40Track(0);
		}
		else
		{
			FDD_SetDoubleSided80Track(i);
		}
	}
	/* by default enable drive 0 and drive 1 */
	FDD_Enable(0, TRUE);
	FDD_Enable(1, TRUE);
}


/* perform the actual step */
void	FDD_PerformStep(unsigned long DriveIndex, signed int StepDirection)
{
	FDD *theDrive;
	int CurrentTrack;
	int Flags;

	theDrive = FDD_GetDrive(DriveIndex);

	/* perform step */
	CurrentTrack = theDrive->CurrentTrack;
	CurrentTrack += StepDirection;

	/* range check head position */
	if (CurrentTrack<0)
	{
		CurrentTrack = 0;
	}
	else
	if (CurrentTrack>=theDrive->NumberOfTracks)
	{
		CurrentTrack = theDrive->NumberOfTracks-1;
	}

	Flags = theDrive->Flags;
	Flags &= ~FDD_FLAGS_HEAD_AT_TRACK_0;

	/* head at track 0? */
	if (CurrentTrack == 0)
	{
		/* yes */
		Flags |= FDD_FLAGS_HEAD_AT_TRACK_0;
	}

	theDrive->Flags = Flags;

	theDrive->CurrentTrack = CurrentTrack;

	theDrive->CurrentIDIndex = 0;
}


/* insert or remove a disk from a drive */
void	FDD_InsertDisk(int Drive,BOOL Status)
{
	FDD *drive = FDD_GetDrive(Drive);

	/* say disk is or isn't present */
	drive->Flags &=~FDD_FLAGS_DISK_PRESENT;

	if (Status)
	{
		drive->Flags |= FDD_FLAGS_DISK_PRESENT;
	}

	/* setup initial parameters for when a disk is present */
	drive->CurrentIDIndex = 0;

	/* refresh motor state */
	FDD_RefreshMotorState(Drive);

    /* refresh write protect status that is reported */
    FDD_RefreshWriteProtect(Drive);
}

BOOL	FDD_IsDiskPresent(int Drive)
{
	return FDD_GetDrive(Drive)->Flags & FDD_FLAGS_DISK_PRESENT;
}

int     FDD_GetPhysicalSide(int Drive)
{
    FDD *drive = FDD_GetDrive(Drive);

    return drive->PhysicalSide;
}

BOOL    FDD_IsEnabled(int Drive)
{
    FDD *drive = FDD_GetDrive(Drive);
    return ((drive->Flags & FDD_FLAGS_DRIVE_ENABLED)!=0);
}

void    FDD_Enable(int Drive, BOOL bState)
{
    FDD *drive = FDD_GetDrive(Drive);

    if (bState)
    {
        drive->Flags |= FDD_FLAGS_DRIVE_ENABLED;
    }
    else
    {
        drive->Flags &=~FDD_FLAGS_DRIVE_ENABLED;
    }
}

BOOL    FDD_GetDiskSideA(int Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

    return (drive->PhysicalSide==0);
}

/* turn disk in the drive */
void	FDD_TurnDisk(int Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

	drive->PhysicalSide^=0x01;
}

void	FDD_Initialise(int Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

    drive->CurrentTrack = 1;
    drive->CurrentIDIndex = 0;
    /* set default side */
	drive->PhysicalSide = 0;
    /* set flags */
	drive->Flags = 0;

	FDD_RefreshWriteProtect(Drive);
	FDD_RefreshMotorState(Drive);
}

/* the disc light comes on for a read/write operation only */
void	FDD_LED_SetState(unsigned long Drive, int LedState)
{
	FDD *drive = FDD_GetDrive(Drive);

	if (LedState)
	{
		drive->Flags |= FDD_FLAGS_LED_ON;
	}
	else
	{
		drive->Flags &=~FDD_FLAGS_LED_ON;
	}
}

int		FDD_LED_GetState(unsigned long Drive)
{
	FDD *drive = FDD_GetDrive(Drive);

	return ((drive->Flags & FDD_FLAGS_LED_ON)!=0);
}


void	FDD_RefreshMotorState(int Drive)
{
    /* really need a delay before drive becomes ready! */
	BOOL bReady = FALSE;
	FDD *drive = FDD_GetDrive(Drive);

	/* drive enabled? */
	if (drive->Flags & FDD_FLAGS_DRIVE_ENABLED)
	{
		/* disk present? */
		if (drive->Flags & FDD_FLAGS_DISK_PRESENT)
		{
			/* motor on? */
			if (FDI_GetMotorState())
				bReady = TRUE;
		}
	}

	if (bReady)
		drive->Flags |=FDD_FLAGS_DRIVE_READY;
	else
		drive->Flags &=~FDD_FLAGS_DRIVE_READY;
}

