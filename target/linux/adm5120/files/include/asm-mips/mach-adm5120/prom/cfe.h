/*
 *  $Id$
 *
 *  Broadcom's CFE definitions
 *
 *  Copyright (C) 2006,2007 Gabor Juhos <juhosg at openwrt.org>
 *  Copyright (C) 2007 OpenWrt.org
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#ifndef _PROM_CFE_H_
#define _PROM_CFE_H_

extern int cfe_present(void) __init;
extern char *cfe_getenv(char *);

#endif /*_PROM_CFE_H_*/
