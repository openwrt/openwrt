/*
 * epon_stubs.c -- definitions for the EPON symbols that the shared xpondrv.c
 * proc/debug handlers reference even though EPON is dropped from this GPON-only
 * build.  The vendor compiled xpondrv.c with EPON always present; here we
 * provide the referenced leaf symbols so econet-xpon.ko links.  All are inert:
 * the "epon ..." debug sub-commands and the max-discovery-gate proc are no-ops.
 * TODO EN7528: build the EPON sub-driver (one of the 5 helper modules) if EPON
 * support is ever wanted.
 */
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

#include "../inc/epon/epon.h"		/* epon_t */
#include "../inc/epon/epon_mpcp.h"
#include "../inc/epon/epon_reg.h"	/* PEPON_MAC_REGS */

/* EPON driver state struct + EPON MAC register window (unused in GPON build). */
epon_t eponDrv;
PEPON_MAC_REGS g_EPON_MAC_BASE;		/* NULL: EPON MAC regs never touched here */

/* EPON max-discovery-gate proc handlers (legacy read_proc/write_proc ABI). */
int max_dscv_gate_read_proc(char *page, char **start, off_t off,
			    int count, int *eof, void *data)
{
	if (eof)
		*eof = 1;
	return 0;
}

int max_dscv_gate_write_proc(struct file *file, const char *buffer,
			     unsigned long count, void *data)
{
	return count;
}

/* EPON sub-driver lifecycle/event hooks called from the shared xpondrv/xmcs
 * code paths (GPON/EPON auto-detect).  No-ops: EPON is not built. */
int  eponInit(void)            { return 0; }
void eponStop(void)            { }
void eponDetectPhyLosLof(void) { }
