/*
 * ofw.c - Open Firmware client interface for 32-bit systems.
 * This code is intended to be portable to any 32-bit Open Firmware
 * implementation with a standard client interface that can be
 * called when Linux is running.
 *
 * Copyright (C) 2007  Mitch Bradley <wmb@firmworks.com>
 * Copyright (C) 2007  Andres Salomon <dilinger@debian.org>
 */

#include <stdarg.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <asm/ofw.h>


int (*call_firmware)(int *);

static DEFINE_SPINLOCK(prom_lock);

#define MAXARGS 20

/*
 * The return value from ofw() in all cases is 0 if the attempt to call the
 * function succeeded, <0 otherwise.  That return value is from the
 * gateway function only.  Any results from the called function are returned
 * via output argument pointers. 
 *
 * Here are call templates for all the standard OFW client services:
 *
 * ofw("test", 1, 1, namestr, &missing);
 * ofw("peer", 1, 1, phandle, &sibling_phandle);
 * ofw("child", 1, 1, phandle, &child_phandle);
 * ofw("parent", 1, 1, phandle, &parent_phandle);
 * ofw("instance_to_package", 1, 1, ihandle, &phandle);
 * ofw("getproplen", 2, 1, phandle, namestr, &proplen);
 * ofw("getprop", 4, 1, phandle, namestr, bufaddr, buflen, &size);
 * ofw("nextprop", 3, 1, phandle, previousstr, bufaddr, &flag);
 * ofw("setprop", 4, 1, phandle, namestr, bufaddr, len, &size);
 * ofw("canon", 3, 1, devspecstr, bufaddr, buflen, &length);
 * ofw("finddevice", 1, 1, devspecstr, &phandle);
 * ofw("instance-to-path", 3, 1, ihandle, bufaddr, buflen, &length);
 * ofw("package-to-path", 3, 1, phandle, bufaddr, buflen, &length);
 * ofw("call_method", numin, numout, in0, in1, ..., &out0, &out1, ...);
 * ofw("open", 1, 1, devspecstr, &ihandle);
 * ofw("close", 1, 0, ihandle);
 * ofw("read", 3, 1, ihandle, addr, len, &actual);
 * ofw("write", 3, 1, ihandle, addr, len, &actual);
 * ofw("seek", 3, 1, ihandle, pos_hi, pos_lo, &status);
 * ofw("claim", 3, 1, virtaddr, size, align, &baseaddr);
 * ofw("release", 2, 0, virtaddr, size);
 * ofw("boot", 1, 0, bootspecstr);
 * ofw("enter", 0, 0);
 * ofw("exit", 0, 0);
 * ofw("chain", 5, 0, virtaddr, size, entryaddr, argsaddr, len);
 * ofw("interpret", numin+1, numout+1, cmdstr, in0, ..., &catchres, &out0, ...);
 * ofw("set-callback", 1, 1, newfuncaddr, &oldfuncaddr);
 * ofw("set-symbol-lookup", 2, 0, symtovaladdr, valtosymaddr);
 * ofw("milliseconds", 0, 1, &ms);
 */

int ofw(char *name, int numargs, int numres, ...)
{
	va_list ap;
	int argarray[MAXARGS+3];
	int argnum = 3;
	int retval;
	int *intp;
	unsigned long flags;

	if (!call_firmware)
		return -1;
	if ((numargs + numres) > MAXARGS)
		return -1;	/* spit out an error? */

	argarray[0] = (int) name;
	argarray[1] = numargs;
	argarray[2] = numres;

	va_start(ap, numres);
	while (numargs) {
		argarray[argnum++] = va_arg(ap, int);
		numargs--;
	}

	spin_lock_irqsave(&prom_lock, flags);
	retval = call_firmware(argarray);
	spin_unlock_irqrestore(&prom_lock, flags);

	if (retval == 0) {
		while (numres) {
			intp = va_arg(ap, int *);
			*intp = argarray[argnum++];
			numres--;
		}
	}
	va_end(ap);
	return retval;
}
EXPORT_SYMBOL(ofw);
