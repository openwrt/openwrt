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

#include <linux/init.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <asm/cfe.h>

#include "cfe_private.h"


static cfe_uint_t cfe_handle;
static int (*cfe_trampoline)(long handle, long iocb);


#include <linux/kernel.h>

void __init cfe_setup(unsigned long fwarg0, unsigned long fwarg1,
		      unsigned long fwarg2, unsigned long fwarg3)
{
	if (fwarg3 == 0x80300000) {
		/* WRT54G workaround */
		fwarg3 = CFE_EPTSEAL;
		fwarg2 = 0xBFC00500;
	}
	if (fwarg3 != CFE_EPTSEAL) {
		/* We are not booted from CFE */
		return;
	}
	if (fwarg1 == 0) {
		/* We are on the boot CPU */
		cfe_handle = (cfe_uint_t)fwarg0;
		cfe_trampoline = CFE_TO_PTR(fwarg2);
	}
}

int cfe_vprintk(const char *fmt, va_list args)
{
	static char buffer[1024];
	static DEFINE_SPINLOCK(lock);
	static const char pfx[] = "CFE-console: ";
	static const size_t pfx_len = sizeof(pfx) - 1;
	unsigned long flags;
	int len, cnt, pos;
	int handle;
	int res;

	if (!cfe_present())
		return -ENODEV;

	spin_lock_irqsave(&lock, flags);
	handle = cfe_getstdhandle(CFE_STDHANDLE_CONSOLE);
	if (CFE_ISERR(handle)) {
		len = -EIO;
		goto out;
	}
	strcpy(buffer, pfx);
	len = vscnprintf(buffer + pfx_len,
			 sizeof(buffer) - pfx_len - 2,
			 fmt, args);
	len += pfx_len;
	/* The CFE console requires CR-LF line-ends.
	 * Add a CR, if we only terminate lines with a LF.
	 * This does only fix CR-LF at the end of the string.
	 * So for multiple lines, use multiple cfe_vprintk calls.
	 */
	if (len > 1 &&
	    buffer[len - 1] == '\n' && buffer[len - 2] != '\r') {
		buffer[len - 1] = '\r';
		buffer[len] = '\n';
		len += 1;
	}
	cnt = len;
	pos = 0;
	while (cnt > 0) {
		res = cfe_write(handle, buffer + pos, len - pos);
		if (CFE_ISERR(res)) {
			len = -EIO;
			goto out;
		}
		cnt -= res;
		pos += res;
	}
out:
	spin_unlock_irqrestore(&lock, flags);

	return len;
}

int cfe_printk(const char *fmt, ...)
{
	va_list args;
	int res;

	va_start(args, fmt);
	res = cfe_vprintk(fmt, args);
	va_end(args);

	return res;
}

static int cfe_iocb_dispatch(struct cfe_iocb *iocb)
{
	if (!cfe_present())
		return CFE_ERR_UNSUPPORTED;
	return cfe_trampoline((long)cfe_handle, (long)iocb);
}

int cfe_present(void)
{
	return (cfe_trampoline != NULL);
}

int cfe_close(int handle)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_CLOSE;
	iocb.handle = handle;

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_cpu_start(int cpu, void (*fn)(void), long sp, long gp, long a1)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_FW_CPUCTL;
	iocb.psize = sizeof(struct cfe_iocb_cpuctl);
	iocb.cpuctl.number = cpu;
	iocb.cpuctl.command = CFE_CPU_CMD_START;
	iocb.cpuctl.gp = gp;
	iocb.cpuctl.sp = sp;
	iocb.cpuctl.a1 = a1;
	iocb.cpuctl.start_addr = (long)fn;

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_cpu_stop(int cpu)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_FW_CPUCTL;
	iocb.psize = sizeof(struct cfe_iocb_cpuctl);
	iocb.cpuctl.number = cpu;
	iocb.cpuctl.command = CFE_CPU_CMD_STOP;

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_enumenv(int idx, char *name, int namelen, char *val, int vallen)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_ENV_ENUM;
	iocb.psize = sizeof(struct cfe_iocb_envbuf);
	iocb.envbuf.index = idx;
	iocb.envbuf.name = PTR_TO_CFE(name);
	iocb.envbuf.name_len = namelen;
	iocb.envbuf.val = PTR_TO_CFE(val);
	iocb.envbuf.val_len = vallen;

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_enumdev(int idx, char *name, int namelen)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));

	iocb.fcode = CFE_CMD_DEV_ENUM;
	iocb.psize = sizeof(struct cfe_iocb_envbuf);
	iocb.envbuf.index = idx;
	iocb.envbuf.name = PTR_TO_CFE(name);
	iocb.envbuf.name_len = namelen;

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_enummem(int idx, int flags, u64 *start, u64 *length,
		u64 *type)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));

	iocb.fcode = CFE_CMD_FW_MEMENUM;
	iocb.flags = flags;
	iocb.psize = sizeof(struct cfe_iocb_meminfo);
	iocb.meminfo.index = idx;

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (!CFE_ISERR(iocb.status)) {
		*start = iocb.meminfo.addr;
		*length = iocb.meminfo.size;
		*type = iocb.meminfo.type;
	}

	return iocb.status;
}

int cfe_exit(int warm, int status)
{
	struct cfe_iocb iocb;
	int err;

printk("CFE REBOOT\n");
	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_FW_RESTART;
	if (warm)
		iocb.flags = CFE_FLG_WARMSTART;
	iocb.psize = sizeof(struct cfe_iocb_exitstat);
	iocb.exitstat.status = status;

printk("CALL\n");
	err = cfe_iocb_dispatch(&iocb);
printk("DONE\n");

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_flushcache(int flags)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_FW_FLUSHCACHE;
	iocb.flags = flags;

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_getdevinfo(char *name)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_GETINFO;
	iocb.psize = sizeof(struct cfe_iocb_buf);
	iocb.buffer.ptr = PTR_TO_CFE(name);
	iocb.buffer.length = strlen(name);

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return iocb.status;

	return iocb.buffer.devflags;
}

int cfe_getenv(char *name, char *dest, int destlen)
{
	struct cfe_iocb iocb;
	int err;

	dest[0] = '\0';
	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_ENV_GET;
	iocb.psize = sizeof(struct cfe_iocb_envbuf);
	iocb.envbuf.name = PTR_TO_CFE(name);
	iocb.envbuf.name_len = strlen(name);
	iocb.envbuf.val = PTR_TO_CFE(dest);
	iocb.envbuf.val_len = destlen;

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_getfwinfo(struct cfe_fwinfo *info)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_FW_GETINFO;
	iocb.psize = sizeof(struct cfe_iocb_fwinfo);

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return err;

	info->version = iocb.fwinfo.version;
	info->totalmem = iocb.fwinfo.totalmem;
	info->flags = iocb.fwinfo.flags;
	info->boardid = iocb.fwinfo.boardid;
	info->bootarea_va = iocb.fwinfo.bootarea_va;
	info->bootarea_pa = iocb.fwinfo.bootarea_pa;
	info->bootarea_size = iocb.fwinfo.bootarea_size;

	return iocb.status;
}

int cfe_getstdhandle(int handletype)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_GETHANDLE;
	iocb.flags = handletype;

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return iocb.status;

	return iocb.handle;
}

int cfe_getticks(s64 *ticks)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_FW_GETTIME;
	iocb.psize = sizeof(struct cfe_iocb_time);

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (!CFE_ISERR(iocb.status))
		*ticks = iocb.time.ticks;

	return iocb.status;
}

int cfe_inpstat(int handle)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_INPSTAT;
	iocb.handle = handle;
	iocb.psize = sizeof(struct cfe_iocb_inpstat);

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return iocb.status;

	return iocb.inpstat.status;
}

int cfe_ioctl(int handle, unsigned int ioctlnum,
	      unsigned char *buffer, int length,
	      int *retlen, u64 offset)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_IOCTL;
	iocb.handle = handle;
	iocb.psize = sizeof(struct cfe_iocb_buf);
	iocb.buffer.offset = offset;
	iocb.buffer.ioctlcmd = ioctlnum;
	iocb.buffer.ptr = PTR_TO_CFE(buffer);
	iocb.buffer.length = length;

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return iocb.status;
	if (retlen)
		*retlen = iocb.buffer.retlen;

	return iocb.status;
}

int cfe_open(char *name)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_OPEN;
	iocb.psize = sizeof(struct cfe_iocb_buf);
	iocb.buffer.ptr = PTR_TO_CFE(name);
	iocb.buffer.length = strlen(name);

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return iocb.status;

	return iocb.handle;
}

int cfe_read(int handle, unsigned char *buffer, int length)
{
	return cfe_readblk(handle, 0, buffer, length);
}

int cfe_readblk(int handle, s64 offset, unsigned char *buffer, int length)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_READ;
	iocb.handle = handle;
	iocb.psize = sizeof(struct cfe_iocb_buf);
	iocb.buffer.offset = offset;
	iocb.buffer.ptr = PTR_TO_CFE(buffer);
	iocb.buffer.length = length;

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return iocb.status;

	return iocb.buffer.retlen;
}

int cfe_setenv(char *name, char *val)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_ENV_SET;
	iocb.psize = sizeof(struct cfe_iocb_envbuf);
	iocb.envbuf.name = PTR_TO_CFE(name);
	iocb.envbuf.name_len = strlen(name);
	iocb.envbuf.val = PTR_TO_CFE(val);
	iocb.envbuf.val_len = strlen(val);

	err = cfe_iocb_dispatch(&iocb);

	return (CFE_ISERR(err)) ? err : iocb.status;
}

int cfe_write(int handle, unsigned char *buffer, int length)
{
	return cfe_writeblk(handle, 0, buffer, length);
}

int cfe_writeblk(int handle, s64 offset, unsigned char *buffer, int length)
{
	struct cfe_iocb iocb;
	int err;

	memset(&iocb, 0, sizeof(iocb));
	iocb.fcode = CFE_CMD_DEV_WRITE;
	iocb.handle = handle;
	iocb.psize = sizeof(struct cfe_iocb_buf);
	iocb.buffer.offset = offset;
	iocb.buffer.ptr = PTR_TO_CFE(buffer);
	iocb.buffer.length = length;

	err = cfe_iocb_dispatch(&iocb);
	if (CFE_ISERR(err))
		return err;
	if (CFE_ISERR(iocb.status))
		return iocb.status;

	return iocb.buffer.retlen;
}
