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
#ifndef __CPC_GLOBAL_HEADER_INCLUDED__
#define __CPC_GLOBAL_HEADER_INCLUDED__

#include "cpcdefs.h"

#ifndef _BOOL
#define _BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

#ifndef NULL
#define	NULL	0
#endif

#ifndef _BYTE
#define _BYTE
typedef unsigned char EMU_BYTE;
#endif

#ifndef _WORD
#define _WORD
typedef unsigned short EMU_WORD;
#endif

#define USE_INLINE

#ifdef USE_INLINE
#define INLINE __inline
#else
#define INLINE
#endif

#endif

