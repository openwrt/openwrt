/*
 * Broadcom Common Firmware Environment (CFE) support
 *
 * Copyright 2000, 2001, 2002
 * Broadcom Corporation. All rights reserved.
 *
 * Copyright (C) 2006 Michael Buesch
 *
 * Original Authors:  Mitch Lichtenberg, Chris Demetriou
 *
 * This software is furnished under license and may be used and copied only
 * in accordance with the following terms and conditions.  Subject to these
 * conditions, you may download, copy, install, use, modify and distribute
 * modified or unmodified copies of this software in source and/or binary
 * form. No title or ownership is transferred hereby.
 *
 * 1) Any source code used, modified or distributed must reproduce and
 *    retain this copyright notice and list of conditions as they appear in
 *    the source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of
 *    Broadcom Corporation.  The "Broadcom Corporation" name may not be
 *    used to endorse or promote products derived from this software
 *    without the prior written permission of Broadcom Corporation.
 *
 * 3) THIS SOFTWARE IS PROVIDED "AS-IS" AND ANY EXPRESS OR IMPLIED
 *    WARRANTIES, INCLUDING BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
 *    NON-INFRINGEMENT ARE DISCLAIMED. IN NO EVENT SHALL BROADCOM BE LIABLE
 *    FOR ANY DAMAGES WHATSOEVER, AND IN PARTICULAR, BROADCOM SHALL NOT BE
 *    LIABLE FOR DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *    OR OTHERWISE), EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LINUX_CFE_API_H_
#define LINUX_CFE_API_H_

#include <linux/types.h>


#define CFE_MI_RESERVED		0		/* memory is reserved, do not use */
#define CFE_MI_AVAILABLE	1		/* memory is available */

#define CFE_FLG_WARMSTART	0x00000001
#define CFE_FLG_FULL_ARENA	0x00000001
#define CFE_FLG_ENV_PERMANENT	0x00000001

#define CFE_CPU_CMD_START	1
#define CFE_CPU_CMD_STOP	0

#define CFE_STDHANDLE_CONSOLE	0

#define CFE_DEV_NETWORK 	1
#define CFE_DEV_DISK		2
#define CFE_DEV_FLASH		3
#define CFE_DEV_SERIAL		4
#define CFE_DEV_CPU		5
#define CFE_DEV_NVRAM		6
#define CFE_DEV_CLOCK           7
#define CFE_DEV_OTHER		8
#define CFE_DEV_MASK		0x0F

#define CFE_CACHE_FLUSH_D	1
#define CFE_CACHE_INVAL_I	2
#define CFE_CACHE_INVAL_D	4
#define CFE_CACHE_INVAL_L2	8

#define CFE_FWI_64BIT		0x00000001
#define CFE_FWI_32BIT		0x00000002
#define CFE_FWI_RELOC		0x00000004
#define CFE_FWI_UNCACHED	0x00000008
#define CFE_FWI_MULTICPU	0x00000010
#define CFE_FWI_FUNCSIM		0x00000020
#define CFE_FWI_RTLSIM		0x00000040

struct cfe_fwinfo {
	s64 version;		/* major, minor, eco version */
	s64 totalmem;		/* total installed mem */
	s64 flags;		/* various flags */
	s64 boardid;		/* board ID */
	s64 bootarea_va;	/* VA of boot area */
	s64 bootarea_pa;	/* PA of boot area */
	s64 bootarea_size;	/* size of boot area */
};


/* The public CFE API */

int cfe_present(void);	/* Check if we booted from CFE. Returns bool */

int cfe_getticks(s64 *ticks);
int cfe_close(int handle);
int cfe_cpu_start(int cpu, void (*fn)(void), long sp, long gp, long a1);
int cfe_cpu_stop(int cpu);
int cfe_enumenv(int idx, char *name, int namelen, char *val, int vallen);
int cfe_enumdev(int idx, char *name, int namelen);
int cfe_enummem(int idx, int flags, u64 *start, u64 *length,
		u64 *type);
int cfe_exit(int warm, int status);
int cfe_flushcache(int flags);
int cfe_getdevinfo(char *name);
int cfe_getenv(char *name, char *dest, int destlen);
int cfe_getfwinfo(struct cfe_fwinfo *info);
int cfe_getstdhandle(int handletype);
int cfe_inpstat(int handle);
int cfe_ioctl(int handle, unsigned int ioctlnum, unsigned char *buffer,
	      int length, int *retlen, u64 offset);
int cfe_open(char *name);
int cfe_read(int handle, unsigned char *buffer, int length);
int cfe_readblk(int handle, s64 offset, unsigned char *buffer, int length);
int cfe_setenv(char *name, char *val);
int cfe_write(int handle, unsigned char *buffer, int length);
int cfe_writeblk(int handle, s64 offset, unsigned char *buffer,
		 int length);


/* High level API */

/* Print some information to CFE's console (most likely serial line) */
int cfe_printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int cfe_vprintk(const char *fmt, va_list args);



/* Error codes returned by the low API functions */

#define CFE_ISERR(errcode)	(errcode < 0)

#define CFE_OK			 0
#define CFE_ERR                 -1	/* generic error */
#define CFE_ERR_INV_COMMAND	-2
#define CFE_ERR_EOF		-3
#define CFE_ERR_IOERR		-4
#define CFE_ERR_NOMEM		-5
#define CFE_ERR_DEVNOTFOUND	-6
#define CFE_ERR_DEVOPEN		-7
#define CFE_ERR_INV_PARAM	-8
#define CFE_ERR_ENVNOTFOUND	-9
#define CFE_ERR_ENVREADONLY	-10

#define CFE_ERR_NOTELF		-11
#define CFE_ERR_NOT32BIT 	-12
#define CFE_ERR_WRONGENDIAN 	-13
#define CFE_ERR_BADELFVERS 	-14
#define CFE_ERR_NOTMIPS 	-15
#define CFE_ERR_BADELFFMT 	-16
#define CFE_ERR_BADADDR 	-17

#define CFE_ERR_FILENOTFOUND	-18
#define CFE_ERR_UNSUPPORTED	-19

#define CFE_ERR_HOSTUNKNOWN	-20

#define CFE_ERR_TIMEOUT		-21

#define CFE_ERR_PROTOCOLERR	-22

#define CFE_ERR_NETDOWN		-23
#define CFE_ERR_NONAMESERVER	-24

#define CFE_ERR_NOHANDLES	-25
#define CFE_ERR_ALREADYBOUND	-26

#define CFE_ERR_CANNOTSET	-27
#define CFE_ERR_NOMORE		-28
#define CFE_ERR_BADFILESYS	-29
#define CFE_ERR_FSNOTAVAIL	-30

#define CFE_ERR_INVBOOTBLOCK	-31
#define CFE_ERR_WRONGDEVTYPE	-32
#define CFE_ERR_BBCHECKSUM	-33
#define CFE_ERR_BOOTPROGCHKSUM	-34

#define CFE_ERR_LDRNOTAVAIL	-35

#define CFE_ERR_NOTREADY	-36

#define CFE_ERR_GETMEM          -37
#define CFE_ERR_SETMEM          -38

#define CFE_ERR_NOTCONN		-39
#define CFE_ERR_ADDRINUSE	-40


#endif /* LINUX_CFE_API_H_ */
