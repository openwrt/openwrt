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

#ifndef LINUX_CFE_PRIVATE_H_
#define LINUX_CFE_PRIVATE_H_

#ifndef __ASSEMBLY__

/* Seal indicating CFE's presence, passed to the kernel. */
#define CFE_EPTSEAL		0x43464531

#define CFE_CMD_FW_GETINFO	0
#define CFE_CMD_FW_RESTART	1
#define CFE_CMD_FW_BOOT		2
#define CFE_CMD_FW_CPUCTL	3
#define CFE_CMD_FW_GETTIME      4
#define CFE_CMD_FW_MEMENUM	5
#define CFE_CMD_FW_FLUSHCACHE	6

#define CFE_CMD_DEV_GETHANDLE	9
#define CFE_CMD_DEV_ENUM	10
#define CFE_CMD_DEV_OPEN	11
#define CFE_CMD_DEV_INPSTAT	12
#define CFE_CMD_DEV_READ	13
#define CFE_CMD_DEV_WRITE	14
#define CFE_CMD_DEV_IOCTL	15
#define CFE_CMD_DEV_CLOSE	16
#define CFE_CMD_DEV_GETINFO	17

#define CFE_CMD_ENV_ENUM	20
#define CFE_CMD_ENV_GET		22
#define CFE_CMD_ENV_SET		23
#define CFE_CMD_ENV_DEL		24

#define CFE_CMD_MAX		32

#define CFE_CMD_VENDOR_USE	0x8000	/* codes above this are for customer use */

typedef u64 cfe_uint_t;
typedef s64 cfe_int_t;
typedef s64 cfe_ptr_t;

/* Cast a pointer from native to CFE-API pointer and back */
#define CFE_TO_PTR(p)		((void *)(unsigned long)(p))
#define PTR_TO_CFE(p)		((cfe_ptr_t)(unsigned long)(p))

struct cfe_iocb_buf {
	cfe_uint_t	offset;		/* offset on device (bytes) */
	cfe_ptr_t	ptr;		/* pointer to a buffer */
	cfe_uint_t	length;		/* length of this buffer */
	cfe_uint_t	retlen;		/* returned length (for read ops) */
	union {
		cfe_uint_t	ioctlcmd;	/* IOCTL command (used only for IOCTLs) */
		cfe_uint_t	devflags;	/* Returned device info flags */
	};
};

struct cfe_iocb_inpstat {
	cfe_uint_t	status;		/* 1 means input available */
};

struct cfe_iocb_envbuf {
	cfe_int_t	index;		/* 0-based enumeration index */
	cfe_ptr_t	name;		/* name string buffer */
	cfe_int_t	name_len;	/* size of name buffer */
	cfe_ptr_t	val;		/* value string buffer */
	cfe_int_t	val_len;	/* size of value string buffer */
};

struct cfe_iocb_cpuctl {
	cfe_uint_t	number;		/* cpu number to control */
	cfe_uint_t	command;	/* command to issue to CPU */
	cfe_uint_t	start_addr;	/* CPU start address */
	cfe_uint_t	gp;		/* starting GP value */
	cfe_uint_t	sp;		/* starting SP value */
	cfe_uint_t	a1;		/* starting A1 value */
};

struct cfe_iocb_time {
	cfe_int_t	ticks;		/* current time in ticks */
};

struct cfe_iocb_exitstat {
	cfe_int_t	status;
};

struct cfe_iocb_meminfo {
	cfe_int_t	index;		/* 0-based enumeration index */
	cfe_int_t	type;		/* type of memory block */
	cfe_uint_t	addr;		/* physical start address */
	cfe_uint_t	size;		/* block size */
};

struct cfe_iocb_fwinfo {
	cfe_int_t	version;	/* major, minor, eco version */
	cfe_int_t	totalmem;	/* total installed mem */
	cfe_int_t	flags;		/* various flags */
	cfe_int_t	boardid;	/* board ID */
	cfe_int_t	bootarea_va;	/* VA of boot area */
	cfe_int_t	bootarea_pa;	/* PA of boot area */
	cfe_int_t	bootarea_size;	/* size of boot area */
	cfe_int_t	reserved1;
	cfe_int_t	reserved2;
	cfe_int_t	reserved3;
};

/* CFE I/O Control Block */
struct cfe_iocb {
	cfe_uint_t	fcode;		/* IOCB function code */
	cfe_int_t	status;		/* return status */
	cfe_int_t	handle;		/* file/device handle */
	cfe_uint_t	flags;		/* flags for this IOCB */
	cfe_uint_t	psize;		/* size of parameter list */
	union {
		struct cfe_iocb_buf		buffer;		/* buffer parameters */
		struct cfe_iocb_inpstat		inpstat;	/* input status parameters */
		struct cfe_iocb_envbuf		envbuf;		/* environment function parameters */
		struct cfe_iocb_cpuctl		cpuctl;		/* CPU control parameters */
		struct cfe_iocb_time		time;		/* timer parameters */
		struct cfe_iocb_meminfo		meminfo;	/* memory arena info parameters */
		struct cfe_iocb_fwinfo		fwinfo;		/* firmware information */
		struct cfe_iocb_exitstat	exitstat;	/* Exit Status */
	};
};


#include <linux/init.h>

void __init cfe_setup(unsigned long fwarg0, unsigned long fwarg1,
		      unsigned long fwarg2, unsigned long fwarg3);

#else /* __ASSEMBLY__ */

	.macro	cfe_early_init
#ifdef CONFIG_CFE
		jal	cfe_setup
#endif
	.endm

#endif /* __ASSEMBLY__ */
#endif /* LINUX_CFE_PRIVATE_H_ */
