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
#include "../cpc/host.h"
#include "display.h"
#include "gtkui.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

static GRAPHICS_BUFFER_INFO BufferInfo;
static GRAPHICS_BUFFER_COLOUR_FORMAT BufferColourFormat;
static SOUND_PLAYBACK_FORMAT SoundFormat;

#ifdef HAVE_SDL
//const int audio_NumberOfChannels = 2;
const int audio_NumberOfChannels = 1;
//const int audio_BitsPerSample = 16;
const int audio_BitsPerSample = 8;
const int audio_Frequency = 44100;
//const int audio_Frequency = 22050;
//const int audio_bufsize = 131072;
//const int audio_bufsize = 16384;
const int audio_bufsize = 4096;
//const int audio_bufsize = 1024;
//const int audio_callbacksize = 4096;
const int audio_callbacksize = 1024;
SDL_AudioSpec audioSpec;
BOOL audio_open = FALSE;
Uint8 *audio_chunk;
//Uint32 chunk_len;
Uint32 audio_len;
Uint8 *audio_pos;
Uint8 *audio_rec;
#endif

BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth)
{
	int DispType;

#ifdef HAVE_SDL
	if (Type == DISPLAY_TYPE_WINDOWED)
	{
		sdl_SetDisplayWindowed(Width, Height, Depth);
	}
	else
	{
		sdl_SetDisplayFullscreen(Width, Height, Depth);
	}
#else
	if (Type == DISPLAY_TYPE_WINDOWED)
	{
		XWindows_SetDisplayWindowed(Width, Height, Depth);
	}
	else
	{
		XWindows_SetDisplayWindowed(Width, Height, Depth);
	}
#endif

	return TRUE;
}


BOOL	Host_LockGraphicsBuffer(void);	
GRAPHICS_BUFFER_INFO	*Host_GetGraphicsBufferInfo(void);
void	Host_UnlockGraphicsBuffer(void);
void	Host_SetPaletteEntry(int, unsigned char, unsigned char, unsigned char);
BOOL	Host_SetDisplay(int Type, int Width, int Height, int Depth);


GRAPHICS_BUFFER_COLOUR_FORMAT *Host_GetGraphicsBufferColourFormat()
{
#ifdef HAVE_SDL
	sdl_GetGraphicsBufferColourFormat(&BufferColourFormat);
#else
	XWindows_GetGraphicsBufferColourFormat(&BufferColourFormat);
#endif

	return &BufferColourFormat;
}

GRAPHICS_BUFFER_INFO *Host_GetGraphicsBufferInfo()
{
#ifdef HAVE_SDL
	sdl_GetGraphicsBufferInfo(&BufferInfo);
#else
	XWindows_GetGraphicsBufferInfo(&BufferInfo);
#endif

	return &BufferInfo;
}

BOOL	Host_LockGraphicsBuffer(void)
{
	//printf("buffer lock\r\n");
#ifdef HAVE_SDL
	return sdl_LockGraphicsBuffer();
#else
	return TRUE;
#endif
}


void	Host_UnlockGraphicsBuffer(void)
{
	//printf("buffer unlock\r\n");
#ifdef HAVE_SDL
	sdl_UnlockGraphicsBuffer();
#endif
}

void	Host_SwapGraphicsBuffers(void)
{
#ifdef HAVE_SDL
	sdl_SwapGraphicsBuffers();
#else
	XWindows_SwapGraphicsBuffers();
#endif
}

void	Host_SetPaletteEntry(int Index, unsigned char R, unsigned char G, unsigned char B)
{
}

void	Host_WriteDataToSoundBuffer(unsigned char *pData, unsigned long Length)
{
	fprintf(stderr,".\n");
}		

#ifdef HAVE_SDL
BOOL	Host_open_audio(SDL_AudioSpec *audioSpec) {
	BOOL status;
	SDL_AudioSpec desired;
	memcpy(&desired, audioSpec, sizeof(SDL_AudioSpec));
	audio_open = TRUE;
	status = SDL_OpenAudio(&desired, audioSpec);
	if ( status < 0 ){
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		return FALSE;
	}
	if (audio_chunk != NULL) free(audio_chunk);
	audio_chunk = malloc(audio_bufsize);
	if (audio_chunk == NULL) {
		fprintf(stderr,"Error: Unable to allocate %i bytes of memory\n",
			audio_bufsize);
		exit(1);
	}
	memset(audio_chunk, 0, audio_bufsize);
	audio_pos = audio_chunk;
	audio_rec = audio_chunk;
	audio_len = audio_bufsize;
	SDL_PauseAudio(0);
	return TRUE;
}

void	Host_close_audio(void) {
	SDL_CloseAudio();
	audio_open = FALSE;
}

void	fill_audio(void *userdata, Uint8 *stream, int len) {
	static int debugcounter = 0;
	//static SDL_AudioSpec wav_spec;
	//static Uint32 wav_length;
	//static Uint8 *wav_buffer = NULL;
	//static Uint8 *p = NULL;
	int i, j;
	int remain;
	//fprintf(stderr,"fill_audio()\n");
	//fprintf(stderr,".%x",len);
	//fprintf(stderr,".%x:%x:%x",(debugcounter+=len),len,
		//audio_pos-audio_chunk);
#if 0
	if ( wav_buffer == NULL ) {
		SDL_LoadWAV("test.wav", &wav_spec, &wav_buffer, &wav_length);
		p = wav_buffer;
	}
	memcpy(stream, p, len);
	//if (debugcounter < 0x5000) {
	if (debugcounter < wav_spec.samples*16*2*2) {
		fprintf(stderr,"_");
		p += len;
	}
#endif
	if ( audio_pos + len < audio_chunk + audio_bufsize ) {
		memcpy(stream, audio_pos, len);
		audio_pos += len;
		//fprintf(stderr,",");
	} else {
		remain = (audio_chunk + audio_bufsize) - audio_pos;
		memcpy(stream, audio_pos, remain);
		memcpy(stream + remain, audio_chunk, len - remain);
		audio_pos = audio_chunk + len - remain;
		//fprintf(stderr,"'");
	}

	//memset(stream, 0, len);

#if 0
	/* Only play if we have data left */
	if ( audio_len == 0 ) {
		//return;
		audio_pos = audio_chunk;
		audio_len = audio_bufsize;
		fprintf(stderr,",");
	}
	/* Mix as much data as possible */
	len = ( len > audio_len ? audio_len : len );
	//SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	memcpy(stream, audio_pos, len);
	audio_pos += len;
	audio_len -= len;
#endif
}
#endif

BOOL	Host_AudioPlaybackPossible(void)
{
#ifdef HAVE_SDL
	fprintf(stderr,"Host_AudioPlaybackPossible(void)\n");
	if (audio_open) return TRUE;
	audioSpec.freq = audio_Frequency;
	if (audio_BitsPerSample == 16) {
		audioSpec.format = AUDIO_S16;
	} else {
		audioSpec.format = AUDIO_S8;
	}
	audioSpec.channels = audio_NumberOfChannels;
	//audioSpec.samples = audio_BitsPerSample;
	//audioSpec.samples = 3520;
	//audioSpec.samples = audio_bufsize/4;	//FIXME, only 16 bit Stereo
	//audioSpec.samples = audio_callbacksize/4;//FIXME, only 16 bit Stereo
	audioSpec.samples = audio_callbacksize;
	audioSpec.callback = fill_audio;
	audioSpec.userdata = NULL;
	return Host_open_audio(&audioSpec);
#else
	return FALSE;
#endif
}

SOUND_PLAYBACK_FORMAT *Host_GetSoundPlaybackFormat(void)
{
#ifdef HAVE_SDL
	fprintf(stderr,"Host_GetSoundPlaybackFormat(void)\n");
	if (!audio_open) Host_AudioPlaybackPossible();
	SoundFormat.NumberOfChannels = audioSpec.channels;
	//SoundFormat.BitsPerSample = audioSpec.samples;
	if (audioSpec.format == AUDIO_S8) {
		SoundFormat.BitsPerSample = 8;
	} else {
		SoundFormat.BitsPerSample = 16;
	}
	SoundFormat.Frequency = audioSpec.freq;
#endif
	return &SoundFormat;
}

BOOL XWindows_ProcessSystemEvents();


BOOL	Host_ProcessSystemEvents(void)
{	
	/* Always break out of main loop when using GTK+, because GTK+ has it's
	 * own event loop. */
#ifdef HAVE_GTK
#ifdef HAVE_SDL
	sdl_ProcessSystemEvents();
#else
	XWindows_ProcessSystemEvents();
#endif
	return TRUE;	/* always break */
#else
	return XWindows_ProcessSystemEvents();
#endif
}

/* copied from /usr/include/linux/kd.h */
#define KDGETLED	0x4B31	/* return current led state */
#define KDSETLED	0x4B32	/* set led state [lights, not flags] */
#define LED_SCR		0x01	/* scroll lock led */
#define LED_CAP		0x04	/* caps lock led */
#define LED_NUM		0x02	/* num lock led */

char	*fn_console = "/dev/console";
int		fd_console;		/* File descriptor for console tty */
long	led_save;

void	Host_InitDriveLEDIndicator()
{
		fd_console = open(fn_console, O_RDONLY);
		if(fd_console < 0) {
			fprintf(stderr, "Could not open %s: ", fn_console);
			perror(NULL);
			fprintf(stderr, "Keyboard LED flashing will be disabled.\n");
		}
		ioctl(fd_console,KDGETLED,&led_save);
}

void	Host_FreeDriveLEDIndicator()
{
		if (fd_console < 0) return;

		ioctl(fd_console,KDSETLED,led_save);
		close(fd_console);
}

void	Host_DoDriveLEDIndicator(int Drive, BOOL State)
{
		long led;
		long indicator;

		if (fd_console < 0) return;

		led = ioctl(fd_console,KDGETLED,&led);
		indicator = (Drive == 0) ? LED_SCR : LED_CAP;
		if(State) led = led | indicator;
		else led = led & ~indicator;
		ioctl(fd_console,KDSETLED,led);
}


void	Host_SetDirectory(char *Directory)
{
	chdir(Directory);
}

void	Host_Throttle(void)
{
#ifdef HAVE_SDL
	sdl_Throttle();
#endif
}

BOOL	Host_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{	
	static int debugcounter = 0;
	int remain;
	//fprintf(stderr,"Host_LockAudioBuffer %i %i %i %i %i\n",
		//*pBlock1, *pBlock1Size, *pBlock2, *pBlock2Size, AudioBufferSize);
	SDL_LockAudio();
	//fprintf(stderr,"<");
	/*if (chunk_len != AudioBufferSize) {
		fprintf(stderr,"?");
		chunk_len = AudioBufferSize;
		//chunk_len = audio_bufsize;
		if (audio_chunk != NULL) free(audio_chunk);
		audio_chunk = malloc(chunk_len);
		if (audio_chunk == NULL) {
			fprintf(stderr,"Error: Unable to allocate %i bytes of memory\n",
				chunk_len);
			exit(1);
		}
		memset(audio_chunk, 0, chunk_len);
		*pBlock1 = audio_chunk;
		*pBlock1Size = chunk_len;
		*pBlock2 = NULL;
		*pBlock2Size = 0;
		audio_pos = audio_chunk;
		audio_len = chunk_len;
	} else {
	}*/
	//memset(audio_chunk, 0, chunk_len);
	/**pBlock1 = audio_chunk;
	*pBlock1Size = audio_bufsize;
	*pBlock2 = NULL;
	*pBlock2Size = 0;
	audio_pos = audio_chunk;
	audio_len = audio_bufsize;*/
	/*if((audio_rec - audio_chunk) + AudioBufferSize < audio_bufsize) {
		fprintf(stderr,"-");
		*pBlock1 = audio_rec;
		*pBlock1Size = AudioBufferSize;
		*pBlock2 = NULL;
		*pBlock2Size = 0;
		audio_rec += AudioBufferSize;
	} else {
		fprintf(stderr,"+");
		*pBlock1 = audio_rec;
		*pBlock1Size = audio_bufsize - (audio_rec - audio_chunk);
		*pBlock2 = audio_chunk;
		*pBlock2Size = audio_bufsize;	// FIXME
		audio_rec = audio_chunk;	// FIXME
	}*/
	remain = audio_bufsize - (audio_rec - audio_chunk);
	//fprintf(stderr,"%i %i",remain,audio_rec - audio_chunk);
	//fprintf(stderr,"%x %x", AudioBufferSize, audio_rec - audio_chunk);
	//fprintf(stderr,"%x", (debugcounter+=AudioBufferSize));
	//fprintf(stderr,"%x:%x:%x", (debugcounter+=AudioBufferSize),
		//AudioBufferSize,audio_rec - audio_chunk);
	if(remain > AudioBufferSize) {
		//fprintf(stderr,"-");
		*pBlock1 = audio_rec;
		*pBlock1Size = AudioBufferSize;
		*pBlock2 = NULL;
		*pBlock2Size = 0;
		audio_rec += AudioBufferSize;
	} else {
		//fprintf(stderr,"+");
		*pBlock1 = audio_rec;
		*pBlock1Size = remain;
		*pBlock2 = audio_chunk;
		*pBlock2Size = AudioBufferSize - remain;
		audio_rec = audio_chunk + *pBlock2Size;
		//sleep(10);
	}
	return TRUE;
}

void	Host_UnLockAudioBuffer(void)
{
	//fprintf(stderr,">");
	SDL_UnlockAudio();
	//fprintf(stderr,"Host_UnLockAudioBuffer\n");
}

unsigned long	Host_GetCurrentTimeInMilliseconds(void)
{
	struct timeval tv;
	
	gettimeofday(&tv,NULL);
	return 1000*tv.tv_sec+tv.tv_usec/1000;
}
