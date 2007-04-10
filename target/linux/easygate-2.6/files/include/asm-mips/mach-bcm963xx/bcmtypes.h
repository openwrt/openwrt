/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/

//
// bcmtypes.h - misc useful typedefs
//
#ifndef BCMTYPES_H
#define BCMTYPES_H

// These are also defined in typedefs.h in the application area, so I need to
// protect against re-definition.

#ifndef _TYPEDEFS_H_
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef signed char     int8;
typedef signed short    int16;
typedef signed long     int32;
#endif

typedef unsigned char   byte;
// typedef unsigned long   sem_t;

typedef unsigned long   HANDLE,*PULONG,DWORD,*PDWORD;
typedef signed long     LONG,*PLONG;

typedef unsigned int    *PUINT;
typedef signed int      INT;

typedef unsigned short  *PUSHORT;
typedef signed short    SHORT,*PSHORT;
typedef unsigned short  WORD,*PWORD;

typedef unsigned char   *PUCHAR;
typedef signed char     *PCHAR;

typedef void            *PVOID;

typedef unsigned char   BOOLEAN, *PBOOL, *PBOOLEAN;

typedef unsigned char   BYTE,*PBYTE;

//#ifndef __GNUC__
//The following has been defined in Vxworks internally: vxTypesOld.h
//redefine under vxworks will cause error
typedef signed int      *PINT;

typedef signed char     INT8;
typedef signed short    INT16;
typedef signed long     INT32;

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned int    UINT;
typedef unsigned long   ULONG;

typedef void            VOID;
typedef unsigned char   BOOL;

//#endif  /* __GNUC__ */


// These are also defined in typedefs.h in the application area, so I need to
// protect against re-definition.
#ifndef TYPEDEFS_H

// Maximum and minimum values for a signed 16 bit integer.
#define MAX_INT16 32767
#define MIN_INT16 -32768

// Useful for true/false return values.  This uses the
// Taligent notation (k for constant).
typedef enum
{
    kFalse = 0,
    kTrue = 1
} Bool;

#endif

/* macros to protect against unaligned accesses */

#if 0
/* first arg is an address, second is a value */
#define PUT16( a, d ) { 		\
  *((byte *)a) = (byte)((d)>>8); 	\
  *(((byte *)a)+1) = (byte)(d); 	\
}

#define PUT32( a, d ) { 		\
  *((byte *)a) = (byte)((d)>>24); 	\
  *(((byte *)a)+1) = (byte)((d)>>16); 	\
  *(((byte *)a)+2) = (byte)((d)>>8); 	\
  *(((byte *)a)+3) = (byte)(d); 	\
}

/* first arg is an address, returns a value */
#define GET16( a ) ( 			\
  (*((byte *)a) << 8) |			\
  (*(((byte *)a)+1))	 		\
)

#define GET32( a ) ( 			\
  (*((byte *)a) << 24)     |		\
  (*(((byte *)a)+1) << 16) | 		\
  (*(((byte *)a)+2) << 8)  | 		\
  (*(((byte *)a)+3))	 		\
)
#endif

#ifndef YES
#define YES 1
#endif

#ifndef NO
#define NO  0
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define READ32(addr)        (*(volatile UINT32 *)((ULONG)&addr))
#define READ16(addr)        (*(volatile UINT16 *)((ULONG)&addr))
#define READ8(addr)         (*(volatile UINT8  *)((ULONG)&addr))

#endif
