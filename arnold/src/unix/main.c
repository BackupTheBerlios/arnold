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
#include "../cpc/arnold.h"
#include "../cpc/cpc.h"
#include "../cpc/host.h"
#include "../cpc/dirstuff.h"
#include "../ifacegen/ifacegen.h"
#include "configfile.h"
#include "gtkui.h"
#include "roms.h"

#ifdef HAVE_SDL
#include <SDL.h>
#endif

#include "display.h"
#include <string.h>
#include <unistd.h>

/* FIXME: Move declarations to header file */
extern void	Host_InitDriveLEDIndicator();
extern void	Host_FreeDriveLEDIndicator();

/* Forward declarations */
void init_main();
char *getLocalIfNull(char *s);

/* main start for Arnold CPC emulator for linux */
int main(int argc, char *argv[])
{
	/* print welcome message */
	printf("Arnold Emulator (c) Kevin Thacker\n");
	printf("Linux Port maintained by Andreas Micklei\n");
	roms_init();
	//printrom();

	if (!CPCEmulation_CheckEndianness())
	{
		printf("Program Compiled with wrong Endian settings.\n");
		exit(1);
	}
	
//	/* check display */
//	if (!XWindows_CheckDisplay())
//	{
//		printf("Failed to open display. Or display depth is  8-bit\n");
//		exit(-1);
//	}
	
	/* display ok, continue with emulation */

	DirStuff_Initialise();

	loadConfigfile();

	init_main(argc, argv);

	GenericInterface_Finish();

	DirStuff_Finish();

	exit(0);

	return 0;	/* Never reached */
}

void init_main(int argc, char *argv[]) {
	char LocalDirectory[1024];
	//char ProgramDirectory[1024]="";
	//char *dataDirectory;
	char *romDirectory;

	romDirectory = getRomDirectory();
	if (romDirectory == NULL) {
		/*char *local;
		local = getLocalIfNull(romDirectory);
		romDirectory = malloc(strlen(local)+5+1);
		sprintf(romDirectory,"%s/roms",local);*/
		romDirectory = BUILTIN;
	}

	sprintf(LocalDirectory,"%s/amsdose/",romDirectory);
	SetDirectoryForLocation(EMULATOR_ROM_CPCAMSDOS_DIR, LocalDirectory);
	/* fprintf(stderr,"%s\n",LocalDirectory); */

	sprintf(LocalDirectory,"%s/cpc464e/",romDirectory);
	SetDirectoryForLocation(EMULATOR_ROM_CPC464_DIR, LocalDirectory);
	/* fprintf(stderr,"%s\n",LocalDirectory); */

	sprintf(LocalDirectory,"%s/cpc664e/",romDirectory);
	SetDirectoryForLocation(EMULATOR_ROM_CPC664_DIR, LocalDirectory);
	/* fprintf(stderr,"%s\n",LocalDirectory); */

	sprintf(LocalDirectory,"%s/cpc6128e/",romDirectory);
	SetDirectoryForLocation(EMULATOR_ROM_CPC6128_DIR, LocalDirectory);
	/* fprintf(stderr,"%s\n",LocalDirectory); */

	sprintf(LocalDirectory,"%s/cpcplus/",romDirectory);
	SetDirectoryForLocation(EMULATOR_ROM_CPCPLUS_DIR, LocalDirectory);
	/* fprintf(stderr,"%s\n",LocalDirectory); */

	sprintf(LocalDirectory,"%s/kcc/",romDirectory);
	SetDirectoryForLocation(EMULATOR_ROM_KCCOMPACT_DIR, LocalDirectory);	
	/* fprintf(stderr,"%s\n",LocalDirectory); */


	GenericInterface_Initialise();

	if (CPCEmulation_Initialise()) {
		//chdir(dataDirectory);		/* FIXME: What is this? */
		chdir(getDiskDirectory());	/* FIXME: What is this? */

		CPC_SetCPCType(CPC_TYPE_CPC6128);

		if (argc!=0)
		{

			int argindex = 0;

			do
			{
				if (strcmp("-tape",argv[argindex])==0)
				{
					if ((argindex+1)<argc)
					{
						if
(!TapeImage_Insert(argv[argindex+1]))
{
	printf("Failed to open tape image %s.\n",argv[argindex+1]);
}
					}
				}

				if (strcmp("-drivea",argv[argindex])==0)
				{
					if ((argindex+1)<argc)
					{

if (!GenericInterface_InsertDiskImage(0, argv[argindex+1]))
{
	printf("Failed to open disk image %s.\n",argv[argindex+1]);
} 
					}
				}

				if (strcmp("-driveb",argv[argindex])==0)	
				{
					if ((argindex+1)<argc)
					{

if (!GenericInterface_InsertDiskImage(1, argv[argindex+1]))
{	
	printf("Failed to open disk image %s.\n",argv[argindex+1]);
}
					}
				}

				if (strcmp("-cart",argv[argindex])==0)
				{
					if ((argindex+1)<argc)
					{

if (!GenericInterface_InsertCartridge(argv[argindex+1]))
{
	printf("Failed to insert cartridge %s\n", argv[argindex+1]);
}
					}
				}
				
				if (strcmp("-frameskip",argv[argindex])==0)
				{
					int frameskip;

					frameskip =  atoi(argv[argindex+1]);

					CPC_SetFrameSkip(frameskip);
				}

				if (strcmp("-crtctype",argv[argindex])==0)
				{
					int crtc;
				
					crtc = atoi(argv[argindex+1]);
				
					CRTC_SetType(crtc);
				}
			
				if (strcmp("-cpctype", argv[argindex])==0)
				{
					int cpctype;

					cpctype = atoi(argv[argindex+1]);

					switch (cpctype)
					{
						case 0:
						{	
				CPC_SetCPCType(CPC_TYPE_CPC464);
				}
				break;
				case 1:
				{
				CPC_SetCPCType(CPC_TYPE_CPC664);
				}
				break;
				case 2:
				{
				CPC_SetCPCType(CPC_TYPE_CPC6128);
				}
				break;
				case 3:
				{
				CPC_SetCPCType(CPC_TYPE_464PLUS);
				}
				break;
				case 4:
				{
				CPC_SetCPCType(CPC_TYPE_6128PLUS);
				}
				break;

				case 5:
				{
				CPC_SetCPCType(CPC_TYPE_KCCOMPACT);
				}
				break;

				default:
				{
				CPC_SetCPCType(CPC_TYPE_CPC6128);
				}
				break;

				}
				}

				if (strcmp("-snapshot",argv[argindex])==0)
				{
if (!Snapshot_Load(argv[argindex+1]))
{	
	printf("Failed to open snapshot %s.\n",argv[argindex+1]);
}

	

				}

				if (strcmp("-help",argv[argindex])==0)
				{
					printf("Switches supported:\n");
					printf("-drivea <string> = specify disk image to insert into drive A\n");
					printf("-driveb <string> = specify disk image to insert into drive B\n");
					printf("-cart <string> = specify CPC+ cartridge to insert\n");				
					printf("-frameskip <integer> = specify frame skip (0-5)\n");
					printf("-crtctype <integer> = specify crtc type (0,1,2,3,4)\n");
					printf("-tape <string> = specify tape image\n");
					printf("-cpctype <integer> = specify CPC type (0=CPC464, 1=CPC664, 2=CPC6128, 3=CPC464+, 4=CPC6128+\n");
					printf("-snapshort <string> = specify snapshot to load\n");

				}
				argindex++;
			}
			while (argindex<argc);
		}

#ifdef HAVE_GTK
		fprintf(stderr, "Initializing GTK+\n");
		gtkui_init(argc, argv);
#endif

#ifdef HAVE_SDL
		fprintf(stderr, "Initializing SDL\n");
		if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_TIMER
			|SDL_INIT_JOYSTICK) < 0 ) {
			fprintf(stderr, "SDL could not be initialized: %s\n", SDL_GetError());
			exit(1);
		}
		atexit(SDL_Quit);
#endif

		Host_InitDriveLEDIndicator();

		Render_SetDisplayWindowed();

		printf("Initialised CPC Emulation Core...\n");

		CPC_SetAudioActive(TRUE);

		printf("Initialised Audio...\n");

		/* Enter GTK+ event loop when GTK+ is compiled in. Use own main loop
		 * otherwise. */
#ifdef HAVE_GTK
		gtkui_run();
#else
	    CPCEmulation_Run();
#endif

	    CPCEmulation_Finish();

		Host_FreeDriveLEDIndicator();
	}
}

char *getLocalIfNull(char *s) {
	static char *localDirectory = NULL;
	if (s == NULL) {
		if (localDirectory == NULL) {
			localDirectory = malloc(1024);	// FIXME: Check -1
			getcwd(localDirectory, 1024);
		}
		return localDirectory;
	} else {
		return s;
	}
}
