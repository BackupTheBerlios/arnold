/*
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2003
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
 *
 *  Original implementation by Troels K.
 *  This implementation by Kev Thacker.
 *
 */
#include "cpc.h"
#include "autotype.h"
#include <string.h>

static AUTOTYPE AutoType;

/* this table converts from ASCII to CPC keyboard codes */
/* not exactly the same as converting from host keyboard
scan codes to CPC keys */

typedef struct
{
  int        nASCII;
  CPC_KEY_ID cpcKey1;
  CPC_KEY_ID cpcKey2;
} ASCII_TO_CPCKEY_MAP;

/* many thanks to Troels K for the original table */

/* this table is not complete */
static ASCII_TO_CPCKEY_MAP ASCIIToCPCMap[]=
{
	{ '1'       , CPC_KEY_1    , CPC_KEY_NULL } ,
	{ '!'       , CPC_KEY_1    , CPC_KEY_SHIFT} ,

	{ '2'       , CPC_KEY_2    , CPC_KEY_NULL } ,
	{ '"'       , CPC_KEY_2    , CPC_KEY_SHIFT} ,

	{ '3'       , CPC_KEY_3    , CPC_KEY_NULL } ,
	{ '#'       , CPC_KEY_3    , CPC_KEY_SHIFT} ,

	{ '4'       , CPC_KEY_4    , CPC_KEY_NULL } ,
	{ '$'       , CPC_KEY_4    , CPC_KEY_SHIFT} ,

	{ '5'       , CPC_KEY_5    , CPC_KEY_NULL } ,
	{ '%'       , CPC_KEY_5    , CPC_KEY_SHIFT} ,

	{ '6'       , CPC_KEY_6    , CPC_KEY_NULL } ,
	{ '&'       , CPC_KEY_6    , CPC_KEY_SHIFT} ,

	{ '7'       , CPC_KEY_7    , CPC_KEY_NULL } ,
	{ '\''      , CPC_KEY_7    , CPC_KEY_SHIFT} ,

	{ '8'       , CPC_KEY_8    , CPC_KEY_NULL } ,
	{ '('       , CPC_KEY_8    , CPC_KEY_SHIFT} ,

	{ '9'       , CPC_KEY_9    , CPC_KEY_NULL } ,
	{ ')'       , CPC_KEY_9    , CPC_KEY_SHIFT} ,

	{ '0'       , CPC_KEY_ZERO , CPC_KEY_NULL } ,
	{ '_'       , CPC_KEY_ZERO , CPC_KEY_SHIFT} ,

	{ '-'       , CPC_KEY_MINUS , CPC_KEY_NULL } ,
	{ '='       , CPC_KEY_MINUS , CPC_KEY_SHIFT} ,

	{ '^'       , CPC_KEY_HAT  , CPC_KEY_NULL } ,
	{ '�'       , CPC_KEY_HAT  , CPC_KEY_SHIFT} ,

	{ '\t'		,CPC_KEY_TAB  , CPC_KEY_NULL } ,

	{ 'q'       , CPC_KEY_Q    , CPC_KEY_NULL } ,
	{ 'Q'       , CPC_KEY_Q    , CPC_KEY_SHIFT} ,

	{ 'w'       , CPC_KEY_W    , CPC_KEY_NULL } ,
	{ 'W'       , CPC_KEY_W    , CPC_KEY_SHIFT} ,

	{ 'e'       , CPC_KEY_E    , CPC_KEY_NULL } ,
	{ 'E'       , CPC_KEY_E    , CPC_KEY_SHIFT} ,

	{ 'r'       , CPC_KEY_R    , CPC_KEY_NULL } ,
	{ 'R'       , CPC_KEY_R    , CPC_KEY_SHIFT} ,

	{ 't'       , CPC_KEY_T    , CPC_KEY_NULL } ,
	{ 'T'       , CPC_KEY_T    , CPC_KEY_SHIFT} ,

	{ 'y'       , CPC_KEY_Y    , CPC_KEY_NULL } ,
	{ 'Y'       , CPC_KEY_Y    , CPC_KEY_SHIFT} ,

	{ 'u'       , CPC_KEY_U    , CPC_KEY_NULL } ,
	{ 'U'       , CPC_KEY_U    , CPC_KEY_SHIFT} ,

	{ 'i'       , CPC_KEY_I    , CPC_KEY_NULL } ,
	{ 'I'       , CPC_KEY_I    , CPC_KEY_SHIFT} ,

	{ 'o'       , CPC_KEY_O    , CPC_KEY_NULL } ,
	{ 'O'       , CPC_KEY_O    , CPC_KEY_SHIFT} ,

	{ 'p'       , CPC_KEY_P    , CPC_KEY_NULL } ,
	{ 'P'       , CPC_KEY_P    , CPC_KEY_SHIFT} ,

	{ '@'       , CPC_KEY_AT   , CPC_KEY_NULL } ,
	{ '|'       , CPC_KEY_AT   , CPC_KEY_SHIFT } ,

	{ '['       , CPC_KEY_OPEN_SQUARE_BRACKET , CPC_KEY_NULL } ,
	{ '{'       , CPC_KEY_OPEN_SQUARE_BRACKET , CPC_KEY_SHIFT} ,

	{ '\n', CPC_KEY_RETURN , CPC_KEY_NULL } ,

	{ 'a'       , CPC_KEY_A    , CPC_KEY_NULL } ,
	{ 'A'       , CPC_KEY_A    , CPC_KEY_SHIFT} ,

	{ 's'       , CPC_KEY_S    , CPC_KEY_NULL } ,
	{ 'S'       , CPC_KEY_S    , CPC_KEY_SHIFT} ,

	{ 'd'       , CPC_KEY_D    , CPC_KEY_NULL } ,
	{ 'D'       , CPC_KEY_D    , CPC_KEY_SHIFT} ,

	{ 'f'       , CPC_KEY_F    , CPC_KEY_NULL } ,
	{ 'F'       , CPC_KEY_F    , CPC_KEY_SHIFT} ,

	{ 'g'       , CPC_KEY_G    , CPC_KEY_NULL } ,
	{ 'G'       , CPC_KEY_G    , CPC_KEY_SHIFT} ,

	{ 'h'       , CPC_KEY_H    , CPC_KEY_NULL } ,
	{ 'H'       , CPC_KEY_H    , CPC_KEY_SHIFT} ,

	{ 'j'       , CPC_KEY_J    , CPC_KEY_NULL } ,
	{ 'J'       , CPC_KEY_J    , CPC_KEY_SHIFT} ,

	{ 'k'       , CPC_KEY_K    , CPC_KEY_NULL } ,
	{ 'K'       , CPC_KEY_K    , CPC_KEY_SHIFT} ,

	{ 'l'       , CPC_KEY_L    , CPC_KEY_NULL } ,
	{ 'L'       , CPC_KEY_L    , CPC_KEY_SHIFT} ,

	{ ':'       , CPC_KEY_COLON , CPC_KEY_NULL } ,
	{ '*'       , CPC_KEY_COLON , CPC_KEY_SHIFT} ,

	{ ';'       , CPC_KEY_SEMICOLON , CPC_KEY_NULL } ,
	{ '+'       , CPC_KEY_SEMICOLON , CPC_KEY_SHIFT} ,

	{ ']'       , CPC_KEY_CLOSE_SQUARE_BRACKET , CPC_KEY_NULL } ,
	{ '}'       , CPC_KEY_CLOSE_SQUARE_BRACKET , CPC_KEY_SHIFT} ,

	{ '/'      , CPC_KEY_BACKSLASH , CPC_KEY_NULL } ,
	{ '`'      , CPC_KEY_BACKSLASH , CPC_KEY_SHIFT} ,

	{ 'z'       , CPC_KEY_Z    , CPC_KEY_NULL } ,
	{ 'Z'       , CPC_KEY_Z    , CPC_KEY_SHIFT} ,

	{ 'x'       , CPC_KEY_X    , CPC_KEY_NULL } ,
	{ 'X'       , CPC_KEY_X    , CPC_KEY_SHIFT} ,

	{ 'c'       , CPC_KEY_C    , CPC_KEY_NULL } ,
	{ 'C'       , CPC_KEY_C    , CPC_KEY_SHIFT} ,

	{ 'v'       , CPC_KEY_V    , CPC_KEY_NULL } ,
	{ 'V'       , CPC_KEY_V    , CPC_KEY_SHIFT} ,

	{ 'b'       , CPC_KEY_B    , CPC_KEY_NULL } ,
	{ 'B'       , CPC_KEY_B    , CPC_KEY_SHIFT} ,

	{ 'n'       , CPC_KEY_N    , CPC_KEY_NULL } ,
	{ 'N'       , CPC_KEY_N    , CPC_KEY_SHIFT} ,

	{ 'm'       , CPC_KEY_M    , CPC_KEY_NULL } ,
	{ 'M'       , CPC_KEY_M    , CPC_KEY_SHIFT} ,

	{ ','       , CPC_KEY_COMMA, CPC_KEY_NULL } ,
	{ '<'       , CPC_KEY_COMMA, CPC_KEY_SHIFT} ,

	{ '.'       , CPC_KEY_DOT  , CPC_KEY_NULL } ,
	{ '>'       , CPC_KEY_DOT  , CPC_KEY_SHIFT} ,

	{ '\\'      , CPC_KEY_FORWARD_SLASH , CPC_KEY_NULL } ,
	{ '?'       , CPC_KEY_BACKSLASH, CPC_KEY_SHIFT} ,

	{ ' ', CPC_KEY_SPACE, CPC_KEY_NULL } ,
};

#ifndef _countof
   #define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

void ASCII_to_CPC(int nASCII, BOOL bKeyDown)
{
   int i;
   ASCII_TO_CPCKEY_MAP *pMap = ASCIIToCPCMap;
   int nMap = _countof(ASCIIToCPCMap);

   for (i = 0; i < nMap; i++)
   {
      if (pMap->nASCII == nASCII)
      {
         if (bKeyDown)
         {
            if (pMap->cpcKey2 != CPC_KEY_NULL)
            {
               CPC_SetKey(pMap->cpcKey2);
            }
            CPC_SetKey(pMap->cpcKey1);
         }
         else
         {
            CPC_ClearKey(pMap->cpcKey1);
            if (pMap->cpcKey2!= CPC_KEY_NULL)
            {
               CPC_ClearKey(pMap->cpcKey2);
            }
         }
         break;
      }

	  pMap++;
   }
}


/* init the auto type functions */
void AutoType_Init()
{
	AutoType.nFlags = 0;
	AutoType.sString = NULL;
	AutoType.bFreeBuffer = FALSE;
	AutoType.nPos = 0;
	AutoType.nFrames = 0;
	AutoType.nCountRemaining = 0;
	AutoType.bResetCPC = 0;
}

BOOL AutoType_Active()
{
	/* if actively typing, or waiting for first keyboard scan
	before typing then auto-type is active */
	return ((AutoType.nFlags & (AUTOTYPE_ACTIVE|AUTOTYPE_WAITING))!=0);
}

void AutoType_Finish()
{
	if (AutoType.bFreeBuffer)
	{
		if (AutoType.sString!=NULL)
		{
			free(AutoType.sString);
			AutoType.sString = NULL;
		}
	}
}


/* set the string to auto type */
void AutoType_SetString(const char *sString, BOOL bFreeBuffer, BOOL bWaitInput, BOOL bResetCPC)
{
    AutoType.bResetCPC = bResetCPC;
	AutoType.bFreeBuffer = bFreeBuffer;
	AutoType.sString = sString;
	AutoType.ch = 0;
	AutoType.nPos = 0;
	AutoType.nFrames = 0;
	AutoType.nCountRemaining = strlen(sString);
	AutoType.nFlags&=~AUTOTYPE_ACTIVE_2;
	if (bWaitInput)
	{
	    if (bResetCPC)
	    {
            /* reset */
            CPC_Reset();
	    }

		/* wait for first keyboard */
		AutoType.nFlags|=AUTOTYPE_WAITING;
		AutoType.nFlags&=~AUTOTYPE_ACTIVE;
	}
	else
	{
		AutoType.nFlags |= AUTOTYPE_ACTIVE;
	}
}

/* execute this every emulated frame; even if it will be skipped */
void AutoType_Update(void)
{
	if ((AutoType.nFlags & AUTOTYPE_ACTIVE)==0)
	{
		if ((AutoType.nFlags & AUTOTYPE_WAITING)!=0)
		{
			if (Keyboard_HasBeenScanned())
			{
			    if (AutoType.bResetCPC)
			    {
			        /* to handle CPC+, we need to do a reset,
			        wait for keyboard to be scanned which is when the menu
			        appears, then we need to do a second reset using
			        MC START PROGRAM. Now we have got to BASIC and can
			        autotype.

			        This solution also works with standard CPC too */
			        if ((AutoType.nFlags & AUTOTYPE_ACTIVE_2)!=0)
			        {
                        /* auto-type is now active */
                        AutoType.nFlags |= AUTOTYPE_ACTIVE;
                        /* no longer waiting */
                        AutoType.nFlags &=~AUTOTYPE_WAITING;
			        }
			        else
			        {
			            extern char *Z80MemoryBase;

                        /* LD C,&ff */
                        Z80MemoryBase[0x04000] = 0x00e;
                        Z80MemoryBase[0x04001] = 0x0ff;
                        /* LD HL,&0 */
                        Z80MemoryBase[0x04002] = 0x021;
                        Z80MemoryBase[0x04003] = 0x000;
                        Z80MemoryBase[0x04004] = 0x000;
                        /* JP &BD16 - MC START PROGRAM */
                        Z80MemoryBase[0x04005] = 0x0c3;
                        Z80MemoryBase[0x04006] = 0x016;
                        Z80MemoryBase[0x04007] = 0x0bd;
                        /* start executing code */
                        Z80_SetReg(Z80_PC,0x04000);

			            Keyboard_ResetHasBeenScanned();
                        AutoType.nFlags |= AUTOTYPE_ACTIVE_2;

                    }
			    }
			    else
			    {
                    /* auto-type is now active */
                    AutoType.nFlags |= AUTOTYPE_ACTIVE;
                    /* no longer waiting */
                    AutoType.nFlags &=~AUTOTYPE_WAITING;
			    }
			}
		}
	}
	else
	{
		/* auto-type is active */

		/* delay frames? */
		if (AutoType.nFrames!=0)
		{
			AutoType.nFrames--;
			return;
		}

		/* NOTES:
			- if SHIFT or CONTROL is pressed, then they must be released
			for at least one whole frame for the CPC operating system to recognise them
			as released.

			- When the same key is pressed in sequence (e.g. press, release, press, release)
			then there must be at least two frames for the key to be recognised as released.
			The CPC operating system is trying to 'debounce' the key
		*/
		if (AutoType.nFlags & AUTOTYPE_RELEASE)
		{
			if (AutoType.nCountRemaining==0)
			{
				/* auto type is no longer active */
				AutoType.nFlags &=~AUTOTYPE_ACTIVE;
				AutoType_Finish();
			}

			AutoType.nFlags &=~AUTOTYPE_RELEASE;

			if (AutoType.ch!=1)
			{
				/* release the key */
				ASCII_to_CPC(AutoType.ch, FALSE);
			}

			/* number of frames for release to be acknowledged */
			AutoType.nFrames = 1;
		}
		else
		{
			char ch;

			/* get the current character */
			ch = AutoType.sString[AutoType.nPos];

			/* update position in string */
			AutoType.nPos++;

			/* update count */
			AutoType.nCountRemaining--;

			AutoType.ch = ch;

			if (ch==1)
			{
				AutoType.nFrames = 2;
			}
			else
			{
				/* number of frames for key to be acknowledged */
				AutoType.nFrames=1;

				ASCII_to_CPC(ch, TRUE);
			}

			AutoType.nFlags |= AUTOTYPE_RELEASE;
		}
	}
}

