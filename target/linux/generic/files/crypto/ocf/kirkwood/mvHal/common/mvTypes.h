/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef __INCmvTypesh
#define __INCmvTypesh

/* Defines */

/* The following is a list of Marvell status    */
#define MV_ERROR		    (-1)
#define MV_OK			    (0x00)  /* Operation succeeded                   */
#define MV_FAIL			    (0x01)	/* Operation failed                      */
#define MV_BAD_VALUE        (0x02)  /* Illegal value (general)               */
#define MV_OUT_OF_RANGE     (0x03)  /* The value is out of range             */
#define MV_BAD_PARAM        (0x04)  /* Illegal parameter in function called  */
#define MV_BAD_PTR          (0x05)  /* Illegal pointer value                 */
#define MV_BAD_SIZE         (0x06)  /* Illegal size                          */
#define MV_BAD_STATE        (0x07)  /* Illegal state of state machine        */
#define MV_SET_ERROR        (0x08)  /* Set operation failed                  */
#define MV_GET_ERROR        (0x09)  /* Get operation failed                  */
#define MV_CREATE_ERROR     (0x0A)  /* Fail while creating an item           */
#define MV_NOT_FOUND        (0x0B)  /* Item not found                        */
#define MV_NO_MORE          (0x0C)  /* No more items found                   */
#define MV_NO_SUCH          (0x0D)  /* No such item                          */
#define MV_TIMEOUT          (0x0E)  /* Time Out                              */
#define MV_NO_CHANGE        (0x0F)  /* Parameter(s) is already in this value */
#define MV_NOT_SUPPORTED    (0x10)  /* This request is not support           */
#define MV_NOT_IMPLEMENTED  (0x11)  /* Request supported but not implemented */
#define MV_NOT_INITIALIZED  (0x12)  /* The item is not initialized           */
#define MV_NO_RESOURCE      (0x13)  /* Resource not available (memory ...)   */
#define MV_FULL             (0x14)  /* Item is full (Queue or table etc...)  */
#define MV_EMPTY            (0x15)  /* Item is empty (Queue or table etc...) */
#define MV_INIT_ERROR       (0x16)  /* Error occured while INIT process      */
#define MV_HW_ERROR         (0x17)  /* Hardware error                        */
#define MV_TX_ERROR         (0x18)  /* Transmit operation not succeeded      */
#define MV_RX_ERROR         (0x19)  /* Recieve operation not succeeded       */
#define MV_NOT_READY	    (0x1A)	/* The other side is not ready yet       */
#define MV_ALREADY_EXIST    (0x1B)  /* Tried to create existing item         */
#define MV_OUT_OF_CPU_MEM   (0x1C)  /* Cpu memory allocation failed.         */
#define MV_NOT_STARTED      (0x1D)  /* Not started yet         */
#define MV_BUSY             (0x1E)  /* Item is busy.                         */
#define MV_TERMINATE        (0x1F)  /* Item terminates it's work.            */
#define MV_NOT_ALIGNED      (0x20)  /* Wrong alignment                       */
#define MV_NOT_ALLOWED      (0x21)  /* Operation NOT allowed                 */
#define MV_WRITE_PROTECT    (0x22)  /* Write protected                       */


#define MV_INVALID  (int)(-1)

#define MV_FALSE	0
#define MV_TRUE     (!(MV_FALSE))


#ifndef NULL
#define NULL ((void*)0)
#endif


#ifndef MV_ASMLANGUAGE
/* typedefs */

typedef char  MV_8;
typedef unsigned char	MV_U8;

typedef int		MV_32;
typedef unsigned int	MV_U32;

typedef short		MV_16;
typedef unsigned short	MV_U16;

#ifdef MV_PPC64
typedef long		MV_64;
typedef unsigned long	MV_U64;
#else
typedef long long		MV_64;
typedef unsigned long long	MV_U64;
#endif

typedef long		MV_LONG;	/* 32/64 */
typedef unsigned long	MV_ULONG;	/* 32/64 */

typedef int     MV_STATUS;
typedef int     MV_BOOL;
typedef void    MV_VOID;
typedef float   MV_FLOAT;

typedef int 	(*MV_FUNCPTR) (void);	  /* ptr to function returning int   */
typedef void 	(*MV_VOIDFUNCPTR) (void); /* ptr to function returning void  */
typedef double 	(*MV_DBLFUNCPTR) (void);  /* ptr to function returning double*/
typedef float 	(*MV_FLTFUNCPTR) (void);  /* ptr to function returning float */

typedef MV_U32 MV_KHZ;
typedef MV_U32 MV_MHZ;
typedef MV_U32 MV_HZ;


/* This enumerator describes the set of commands that can be applied on   	*/
/* an engine (e.g. IDMA, XOR). Appling a comman depends on the current   	*/
/* status (see MV_STATE enumerator)                      					*/
/* Start can be applied only when status is IDLE                         */
/* Stop can be applied only when status is IDLE, ACTIVE or PAUSED        */
/* Pause can be applied only when status is ACTIVE                          */
/* Restart can be applied only when status is PAUSED                        */
typedef enum _mvCommand
{
    MV_START,              /* Start	*/
    MV_STOP,               /* Stop     */
    MV_PAUSE,              /* Pause    */
    MV_RESTART             /* Restart  */
} MV_COMMAND;

/* This enumerator describes the set of state conditions.					*/
/* Moving from one state to other is stricted.   							*/
typedef enum _mvState
{
    MV_IDLE,
    MV_ACTIVE,
    MV_PAUSED,
    MV_UNDEFINED_STATE
} MV_STATE;


/* This structure describes address space window. Window base can be        */
/* 64 bit, window size up to 4GB                                            */
typedef struct _mvAddrWin
{
    MV_U32      baseLow;    /* 32bit base low       */
    MV_U32      baseHigh;   /* 32bit base high      */
    MV_U32      size;       /* 32bit size           */
}MV_ADDR_WIN;

/* This binary enumerator describes protection attribute status             */
typedef enum _mvProtRight
{
    ALLOWED,        /* Protection attribute allowed                         */
    FORBIDDEN       /* Protection attribute forbidden                       */
}MV_PROT_RIGHT;

/* Unified struct for Rx and Tx packet operations. The user is required to 	*/
/* be familier only with Tx/Rx descriptor command status.               	*/
typedef struct _bufInfo
{
    MV_U32   cmdSts;        /* Tx/Rx command status                                     */
        MV_U16   byteCnt;       /* Size of valid data in the buffer     */
    MV_U16   bufSize;       /* Total size of the buffer             */
    MV_U8    *pBuff;            /* Pointer to Buffer                    */
    MV_U8    *pData;            /* Pointer to data in the Buffer        */
    MV_U32   userInfo1;         /* Tx/Rx attached user information 1    */
    MV_U32   userInfo2;         /* Tx/Rx attached user information 2    */
    struct _bufInfo *pNextBufInfo;  /* Next buffer in packet            */
} BUF_INFO;

/* This structure contains information describing one of buffers
 * (fragments) they are built Ethernet packet.
 */
typedef struct
{
     MV_U8*	    bufVirtPtr;
     MV_ULONG	bufPhysAddr;
     MV_U32   	bufSize;
     MV_U32     dataSize;
     MV_U32		memHandle;
	 MV_32      bufAddrShift;
} MV_BUF_INFO;

/* This structure contains information describing Ethernet packet.
 * The packet can be divided for few buffers (fragments)
 */
typedef struct
{
    MV_ULONG   	osInfo;
    MV_BUF_INFO *pFrags;
    MV_U32      status;
    MV_U16      pktSize;
    MV_U16      numFrags;
    MV_U32      ownerId;
    MV_U32      fragIP;
} MV_PKT_INFO;

#endif /* MV_ASMLANGUAGE */

#endif /* __INCmvTypesh */

