/*
 *  $Id$
 *
 *  Generic PROM routines
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

#include <asm/bootinfo.h>

#include <prom/generic.h>

static int *_prom_argc;
static char **_prom_argv;
static char **_prom_envp;

char *generic_prom_getenv(char *envname)
{
	char **env;
	char *ret;

	ret = NULL;
	for (env = _prom_envp; *env != NULL; env++) {
		if (strcmp(envname, *env++) == 0) {
			ret = *env;
			break;
		}
	}

	return ret;
}

int generic_prom_present(void)
{
	_prom_argc = (int *)fw_arg0;
	_prom_argv = (char **)fw_arg1;
	_prom_envp = (char **)fw_arg2;

	return 1;
}
