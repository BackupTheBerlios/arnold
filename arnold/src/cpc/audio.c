#include "cpc.h"
#include "psgplay.h"
#include "host.h"

static unsigned char *pAudioBufferBase;
static unsigned char *pAudioBufferPtr;

/* sample rate */
static unsigned long SampleRate;
/* bits per sample */
static int BitsPerSample;
/* no of channels */
static int NoOfChannels;

static FIXED_POINT16 fSamplesPerNop;
static FIXED_POINT16 fPSGEventsPerNop;

static FIXED_POINT16 fSample;

void    Audio_Init(int newFrequency, int newBitsPerSample, int newNoOfChannels)
{
	float SamplesPerScreen;
	int AudioBufferSize;
	int BytesPerSample = ((newBitsPerSample*newNoOfChannels)+7)>>3;
	float ScreenRefreshFrequency;
	float SamplesPerNop;
//	float PSGEventsPerSample;
	float PSGEventsPerNOP;
    float NopsPerSecond;
	SampleRate = newFrequency;
    BitsPerSample = newBitsPerSample;
    NoOfChannels = newNoOfChannels;

    ScreenRefreshFrequency = 50.08f;

    NopsPerSecond = NOPS_PER_FRAME * ScreenRefreshFrequency;
    /* no of samples per screen refresh */
    SamplesPerScreen = (float)SampleRate/(float)ScreenRefreshFrequency;

    /* calculate size of sample buffer; allowing for an extra half screen time just in case */
    AudioBufferSize = (int)(SamplesPerScreen * BytesPerSample * 1.5f);

    /* allocate memory for sample buffer */
    pAudioBufferBase  = (unsigned char *)malloc(AudioBufferSize);
	pAudioBufferPtr = pAudioBufferBase;

    /* samples per nop */
    SamplesPerNop = (float)SamplesPerScreen/(float)NOPS_PER_FRAME;
//	PSGEventsPerSample = (float)(PSG_CLOCK_FREQUENCY>>3)/(float)SampleRate;

	fSamplesPerNop.FixedPoint.L = (int)(SamplesPerNop*65536.0f);

	PSGEventsPerNOP = (float)(PSG_CLOCK_FREQUENCY/(float)(NopsPerSecond*8));
	fPSGEventsPerNop.FixedPoint.L = (int)(PSGEventsPerNOP*65536.0f);

	fSample.FixedPoint.L = 0;
	pAudioBufferPtr[0] = 0;


}

unsigned char *Audio_Write8BitMono(unsigned char *pPtr)
{
	unsigned char *pSamplePtr = pPtr;



	return pSamplePtr;
}

unsigned char *Audio_Write8BitStereo(unsigned char *pPtr)
{


}

unsigned char *Audio_Write16BitMono(unsigned char *pPtr)
{
	unsigned short *pSamplePtr = (unsigned short *)pPtr;



	return (unsigned char *)pSamplePtr;

}

unsigned char *Audio_Write16BitStereo(unsigned char *pPtr)
{
	unsigned short *pSamplePtr = (unsigned short *)pPtr;

	return (unsigned char *)pSamplePtr;
}




void CPC_Stereo_Mixer(PSG_OUTPUT *PSG_Output, unsigned char *LeftVolume, unsigned char *RightVolume)
{
    /* according to the specifications B channel is mixed with A
    and C, but not exactly half */

    /* make this a lookup? */
    unsigned char MiddleChannel = (PSG_Output->B*10)/22;

    /* resulting could be as much as 64 */
    *LeftVolume = (PSG_Output->A + MiddleChannel)/2;
    *RightVolume = (PSG_Output->C + MiddleChannel)/2;
}

void CPC_Mono_Mixer(PSG_OUTPUT *PSG_Output, unsigned char *LeftVolume, unsigned char *RightVolume)
{
    unsigned char Volume = (PSG_Output->A + PSG_Output->B + PSG_Output->C)/3;
    *LeftVolume = Volume;
    *RightVolume = Volume;
}


void Audio_Update(int NopCycles)
{
	// this is the current position through the current sample
	const unsigned long PrevFraction = fSample.FixedPoint.W.Fraction;
	// this is the amount of time until this sample ends.
	const unsigned long PrevFractionRemaining = (0x010000-PrevFraction);
    unsigned char LeftChannel, RightChannel;

	// current volume set
//	const unsigned char DigiblasterVolume = Printer_GetDataByte();	//Digiblaster;	//sCPC_GetTapeVolume();


	// number of samples we are generating this time
	FIXED_POINT16 fSamples;
	FIXED_POINT16 fPSGEvents;
	PSG_OUTPUT		PSGOutput;
	unsigned char FinalVolume;

	fSamples.FixedPoint.L = (fSamplesPerNop.FixedPoint.L * NopCycles);
	fPSGEvents.FixedPoint.L = (fPSGEventsPerNop.FixedPoint.L * NopCycles);

	//PSG_InitialiseToneUpdates(&fPSGEvents);

	/* update channels and get raw volumes */
	PSG_UpdateChannels(&PSGOutput, &fPSGEvents);

    /* mix it according to mono output on expansion port */
	CPC_Mono_Mixer(&PSGOutput,&LeftChannel, &RightChannel) ;

    /* calc final volume */
    FinalVolume = LeftChannel<<3;

	// add this sample's value weighting it accordingly.
	if (fSamples.FixedPoint.L>=PrevFractionRemaining)
	{
		int nSample;

		*pAudioBufferPtr += ((FinalVolume*PrevFractionRemaining)>>16);
		++pAudioBufferPtr;

		fSamples.FixedPoint.L-=PrevFractionRemaining;

		for (nSample = 0; nSample<(fSamples.FixedPoint.W.Int); ++nSample)
		{
			*pAudioBufferPtr = FinalVolume;
			++pAudioBufferPtr;
		}
		*pAudioBufferPtr = 0;
		if ((fSamples.FixedPoint.W.Fraction)!=0)
		{
			*pAudioBufferPtr += ((FinalVolume*fSamples.FixedPoint.W.Fraction)>>16);
		}
		fSample.FixedPoint.L=fSamples.FixedPoint.W.Fraction;
	}
	else
	{
		*pAudioBufferPtr += ((FinalVolume*fSamples.FixedPoint.L)>>16);

		// add on and continue
		fSample.FixedPoint.L += fSamples.FixedPoint.L;
		fSample.FixedPoint.W.Int = 0;
	}
}



void Audio_Commit()
{
	/* calc length in bytes of complete number of samples */
	unsigned long Length = pAudioBufferPtr-pAudioBufferBase;

	if (Length!=0)
	{
		unsigned char *pAudio1;
		unsigned char *pAudio2;
		unsigned long AudioBlock1Size;
		unsigned long AudioBlock2Size;

		/* commit to dx */
		if (Host_LockAudioBuffer(&pAudio1, &AudioBlock1Size, &pAudio2, &AudioBlock2Size, Length))
		{
			memcpy(pAudio1, pAudioBufferBase , AudioBlock1Size);

			if (pAudio2!=NULL)
			{
				memcpy(pAudio2, pAudioBufferBase+AudioBlock1Size, AudioBlock2Size);
			}

			/* unlock */
			Host_UnLockAudioBuffer();
		}
	}

//	/* remember remainder of sample we got to for next audio write */
//	fSample.FixedPoint.W.Int = 0;

	/* now copy sample we are building to front of buffer ready for next time */
	{
		const int BytesPerSample = (BitsPerSample*NoOfChannels)>>3;
		// copy current sample being built.
		memcpy(pAudioBufferBase, pAudioBufferPtr, BytesPerSample);
	}
	pAudioBufferPtr = pAudioBufferBase;
}

void Audio_Finish()
{
	free(pAudioBufferBase);
}
