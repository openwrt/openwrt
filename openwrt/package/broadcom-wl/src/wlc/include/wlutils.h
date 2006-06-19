/*
 * Broadcom wireless network adapter utility functions
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: wlutils.h,v 1.1.1.12 2006/02/27 03:43:20 honor Exp $
 */

#ifndef _wlutils_h_
#define _wlutils_h_

#include <typedefs.h>
#include <wlioctl.h>

/*
 * Pass a wlioctl request to the specified interface.
 * @param	name	interface name
 * @param	cmd	WLC_GET_MAGIC <= cmd < WLC_LAST
 * @param	buf	buffer for passing in and/or receiving data
 * @param	len	length of buf
 * @return	>= 0 if successful or < 0 otherwise
 */
extern int wl_ioctl(char *name, int cmd, void *buf, int len);

/*
 * Probe the specified interface.
 * @param	name	interface name
 * @return	>= 0 if a Broadcom wireless device or < 0 otherwise
 */
extern int wl_probe(char *name);

extern int wl_iovar_set(char *ifname, char *iovar, void *param, int paramlen);
extern int wl_iovar_get(char *ifname, char *iovar, void *bufptr, int buflen);
/*
 * Set/Get named variable.
 * @param	ifname		interface name
 * @param	iovar		variable name
 * @param	param		input param value/buffer
 * @param	paramlen	input param value/buffer length
 * @param	bufptr		io buffer
 * @param	buflen		io buffer length
 * @param	val		val or val pointer for int routines
 * @return	success == 0, failure != 0
 */
/*
 * set named driver variable to int value
 * calling example: wl_iovar_setint(ifname, "arate", rate)
*/
static inline int
wl_iovar_setint(char *ifname, char *iovar, int val)
{
	return wl_iovar_set(ifname, iovar, &val, sizeof(val));
}

/*
 * get named driver variable to int value and return error indication
 * calling example: wl_iovar_getint(ifname, "arate", &rate)
 */
static inline int
wl_iovar_getint(char *ifname, char *iovar, int *val)
{
	return wl_iovar_get(ifname, iovar, val, sizeof(int));
}

/*
 * Set/Get named variable indexed by BSS Configuration
 * @param	ifname		interface name
 * @param	iovar		variable name
 * @param	bssidx		bsscfg index
 * @param	param		input param value/buffer
 * @param	paramlen	input param value/buffer length
 * @param	bufptr		io buffer
 * @param	buflen		io buffer length
 * @param	val		val or val pointer for int routines
 * @return	success == 0, failure != 0
 */
extern int wl_bssiovar_get(char *ifname, char *iovar, int bssidx, void *outbuf, int len);
extern int wl_bssiovar_set(char *ifname, char *iovar, int bssidx, void *param, int paramlen);
/*
 * set named & bss indexed driver variable to int value
 */
static inline int
wl_bssiovar_setint(char *ifname, char *iovar, int bssidx, int val)
{
	return wl_bssiovar_set(ifname, iovar, bssidx, &val, sizeof(int));
}

static inline int
wl_bssiovar_getint(char *ifname, char *iovar, int bssidx, int *val)
{
	return wl_bssiovar_get(ifname, iovar, bssidx, val, sizeof(int));
}

extern int wl_bssiovar_setint(char *ifname, char *iovar, int bssidx, int val);

#endif /* _wlutils_h_ */
