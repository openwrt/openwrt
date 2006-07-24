/*
 * wlc - Broadcom Wireless Driver Control Utility
 *
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <glob.h>
#include <ctype.h>

#include <typedefs.h>
#include <wlutils.h>
#include <proto/802.11.h>

#define VERSION "0.1"
#define BUFSIZE 8192
#define PTABLE_MAGIC 0xbadc0ded
#define PTABLE_SLT1 1
#define PTABLE_SLT2 2
#define PTABLE_ACKW 3
#define PTABLE_ADHM 4
#define PTABLE_END 0xffffffff

/* 
 * Copy each token in wordlist delimited by space into word 
 * Taken from Broadcom shutils.h
 */
#define foreach(word, wordlist, next) \
	for (next = &wordlist[strspn(wordlist, " ")], \
		 strncpy(word, next, sizeof(word)), \
		 word[strcspn(word, " ")] = '\0', \
		 word[sizeof(word) - 1] = '\0', \
		 next = strchr(next, ' '); \
		 strlen(word); \
		 next = next ? &next[strspn(next, " ")] : "", \
		 strncpy(word, next, sizeof(word)), \
		 word[strcspn(word, " ")] = '\0', \
		 word[sizeof(word) - 1] = '\0', \
		 next = strchr(next, ' '))

static char wlbuf[8192];
static char interface[16] = "wl0";
static unsigned long ptable[128];
static unsigned long kmem_offset = 0;
static int vif = 0, debug = 1, fromstdin = 0;

typedef enum {
	NONE =   0x00,

	/* types */
	PARAM_TYPE =    0x00f,
	INT =    0x001,
	STRING = 0x002,

	/* options */
	PARAM_OPTIONS = 0x0f0,
	NOARG =  0x010,

	/* modes */
	PARAM_MODE =    0xf00,
	GET =    0x100,
	SET =    0x200,
} wlc_param;

struct wlc_call {
	const char *name;
	wlc_param param;
	int (*handler)(wlc_param param, void *data, void *value);
	union {
		int num;
		char *str;
		void *ptr;
	} data;
	const char *desc;
};

/* can't use the system include because of the stupid broadcom header files */
extern struct ether_addr *ether_aton(const char *asc);
extern char *ether_ntoa(const struct ether_addr *addr);

/*
 * find the starting point of wl.o in memory
 * by reading /proc/ksyms
 */
static inline void wlc_get_mem_offset(void)
{
	FILE *f;
	char s[64];

	/* yes, i'm lazy ;) */
	f = popen("grep '\\[wl]' /proc/ksyms | sort", "r");
	if (fgets(s, 64, f) == 0)
		return;

	pclose(f);
	
	s[8] = 0;
	kmem_offset = strtoul(s, NULL, 16);

	/* sanity check */
	if (kmem_offset < 0xc0000000)
		kmem_offset = 0;
}


static int ptable_init(void)
{
	glob_t globbuf;
	struct stat statbuf;
	int fd;

	if (ptable[0] == PTABLE_MAGIC)
		return 0;
	
	glob("/lib/modules/2.4.*/wl.o.patch", 0, NULL, &globbuf);
	
	if (globbuf.gl_pathv[0] == NULL)
		return -1;
	
	if ((fd = open(globbuf.gl_pathv[0], O_RDONLY)) < 0)
		return -1;
	
	if (fstat(fd, &statbuf) < 0)
		goto failed;

	if (statbuf.st_size < 512)
		goto failed;

	if (read(fd, ptable, 512) < 512)
		goto failed;
	
	if (ptable[0] != PTABLE_MAGIC)
		goto failed;
	
	close(fd);

	wlc_get_mem_offset();
	if (kmem_offset == 0)
		return -1;
	
	return 0;
		
failed:
	close(fd);

	return -1;
}

static inline unsigned long wlc_kmem_read(unsigned long offset)
{
	int fd;
	unsigned long ret;

	if ((fd = open("/dev/kmem", O_RDONLY )) < 0)
		return -1;
	
	lseek(fd, 0x70000000, SEEK_SET);
	lseek(fd, (kmem_offset - 0x70000000) + offset, SEEK_CUR);
	read(fd, &ret, 4);
	close(fd);

	return ret;
}

static inline void wlc_kmem_write(unsigned long offset, unsigned long value)
{
	int fd;

	if ((fd = open("/dev/kmem", O_WRONLY )) < 0)
		return;
	
	lseek(fd, 0x70000000, SEEK_SET);
	lseek(fd, (kmem_offset - 0x70000000) + offset, SEEK_CUR);
	write(fd, &value, 4);
	close(fd);
}

static int wlc_patcher_getval(unsigned long key, unsigned long *val)
{
	unsigned long *pt = &ptable[1];
	unsigned long tmp;
	
	if (ptable_init() < 0) {
		fprintf(stderr, "Could not load the ptable\n");
		return -1;
	}

	while (*pt != PTABLE_END) {
		if (*pt == key) {
			tmp = wlc_kmem_read(pt[1]);

			if (tmp == pt[2])
				*val = 0xffffffff;
			else
				*val = tmp;
			
			return 0;
		}
		pt += 3;
	}
	
	return -1;
}

static int wlc_patcher_setval(unsigned long key, unsigned long val)
{
	unsigned long *pt = &ptable[1];
	
	if (ptable_init() < 0) {
		fprintf(stderr, "Could not load the ptable\n");
		return -1;
	}

	if (val != 0xffffffff)
		val = (pt[2] & ~(0xffff)) | (val & 0xffff);
	
	while (*pt != PTABLE_END) {
		if (*pt == key) {
			if (val == 0xffffffff) /* default */
				val = pt[2];

			wlc_kmem_write(pt[1], val);
		}
		pt += 3;
	}
	
	return 0;
}

static int wlc_slottime(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	int ret = 0;

	if ((param & PARAM_MODE) == SET) {
		wlc_patcher_setval(PTABLE_SLT1, *val);
		wlc_patcher_setval(PTABLE_SLT2, ((*val == -1) ? *val : *val + 510));
	} else if ((param & PARAM_MODE) == GET) {
		ret = wlc_patcher_getval(PTABLE_SLT1, (unsigned long *) val);
		if (*val != 0xffffffff)
			*val &= 0xffff;
	}

	return ret;
}

static int wlc_noack(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	int ret = 0;

	if ((param & PARAM_MODE) == SET) {
		wlc_patcher_setval(PTABLE_ACKW, ((*val) ? 1 : 0));
	} else if ((param & PARAM_MODE) == GET) {
		ret = wlc_patcher_getval(PTABLE_ACKW, (unsigned long *) val);
		*val &= 0xffff;
		*val = (*val ? 1 : 0);
	}

	return ret;
}

static int wlc_ibss_merge(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	int ret = 0;

	if ((param & PARAM_MODE) == SET) {
		/* overwrite the instruction with 'lui v0,0x0' - fake a return
		 * status of 0 for wlc_bcn_tsf_later */
		wlc_patcher_setval(PTABLE_ACKW, ((*val) ? -1 : 0x3c020000));
	} else if ((param & PARAM_MODE) == GET) {
		ret = wlc_patcher_getval(PTABLE_ACKW, (unsigned long *) val);
		*val = ((*val == -1) ? 1 : 0);
	}

	return ret;
}

static int wlc_ioctl(wlc_param param, void *data, void *value)
{
	unsigned int *var = ((unsigned int *) data);
	unsigned int ioc = *var;

	if (param & NOARG) {
		return wl_ioctl(interface, ioc, NULL, 0);
	}
	switch(param & PARAM_TYPE) {
		case INT:
			return wl_ioctl(interface, ((param & SET) ? (ioc) : (ioc >> 16)) & 0xffff, value, sizeof(int));
		case STRING:
			return wl_ioctl(interface, ((param & SET) ? (ioc) : (ioc >> 16)) & 0xffff, value, BUFSIZE);
	}	
	return 0;
}

static int wlc_iovar(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	char *iov = *((char **) data);
	int ret = 0;
	
	if (param & SET) {
		switch(param & PARAM_TYPE) {
			case INT:
				ret = wl_iovar_setint(interface, iov, *val);
		}
	}
	if (param & GET) {
		switch(param & PARAM_TYPE) {
			case INT:
				ret = wl_iovar_getint(interface, iov, val);
		}
	}

	return ret;
}

static int wlc_bssiovar(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	char *iov = *((char **) data);
	int ret = 0;
	
	if (param & SET) {
		switch(param & PARAM_TYPE) {
			case INT:
				ret = wl_bssiovar_setint(interface, iov, vif, *val);
		}
	}
	if (param & GET) {
		switch(param & PARAM_TYPE) {
			case INT:
				ret = wl_bssiovar_getint(interface, iov, vif, val);
		}
	}

	return ret;
}

static int wlc_vif_enabled(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	int buf[3];
	int ret = 0;
	
	sprintf((char *) buf, "bss");
	buf[1] = vif;
	if (param & SET) {
		buf[2] = (*val ? 1 : 0);
		ret = wl_ioctl(interface, WLC_SET_VAR, buf, sizeof(buf));
	} else if (param & GET) {
		ret = wl_ioctl(interface, WLC_GET_VAR, buf, sizeof(buf));
		*val = buf[0];
	}

	return ret;
}

static int wlc_ssid(wlc_param param, void *data, void *value)
{
	int ret = -1, ret2 = -1;
	char *dest = (char *) value;
	wlc_ssid_t ssid;
	
	if ((param & PARAM_MODE) == GET) {
		ret = wl_bssiovar_get(interface, "ssid", vif, &ssid, sizeof(ssid));

		if (ret)
			/* if we can't get the ssid through the bssiovar, try WLC_GET_SSID */
			ret = wl_ioctl(interface, WLC_GET_SSID, &ssid, sizeof(ssid));
		
		if (!ret) {
			memcpy(dest, ssid.SSID, ssid.SSID_len);
			dest[ssid.SSID_len] = 0;
		}
	} else if ((param & PARAM_MODE) == SET) {
		strncpy(ssid.SSID, value, 32);
		ssid.SSID_len = strlen(value);
		
		if (ssid.SSID_len > 32)
			ssid.SSID_len = 32;
		
		if (vif == 0) {
			/* for the main interface, also try the WLC_SET_SSID call */
			ret2 = wl_ioctl(interface, WLC_SET_SSID, &ssid, sizeof(ssid));
		}
		
		ret = wl_bssiovar_set(interface, "ssid", vif, &ssid, sizeof(ssid));
		ret = (!ret2 ? 0 : ret);
	}
	
	return ret;
}

static int wlc_int(wlc_param param, void *data, void *value)
{
	int *var = *((int **) data);
	int *val = (int *) value;

	if ((param & PARAM_MODE) == SET) {
		*var = *val;
	} else if ((param & PARAM_MODE) == GET) {
		*val = *var;
	}

	return 0;
}

static int wlc_flag(wlc_param param, void *data, void *value)
{
	int *var = *((int **) data);

	*var = 1;

	return 0;
}

static int wlc_string(wlc_param param, void *data, void *value)
{
	char *var = *((char **) data);
	
	if ((param & PARAM_MODE) == GET) {
		strcpy(value, var);
	}

	return 0;
}

static int wlc_afterburner(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	int ret = 0;

	if ((param & PARAM_MODE) == GET) {
		ret = wl_iovar_getint(interface, "afterburner", val);
	} else {
		wl_iovar_setint(interface, "wlfeatureflag", (*val ? 3 : 0));
		ret = wl_iovar_setint(interface, "afterburner", (*val ? 1 : 0));
		wl_iovar_setint(interface, "afterburner_override", *val);
	}

	return ret;
}

static int wlc_maclist(wlc_param param, void *data, void *value)
{
	unsigned int *var = ((unsigned int *) data);
	unsigned int ioc = *var;
	int limit = (sizeof(wlbuf) - 4) / sizeof(struct ether_addr);
	struct maclist *list = (struct maclist *) wlbuf;
	char *str = (char *) value;
	char astr[30], *p;
	struct ether_addr *addr;
	int isset = 0;
	int ret;

	if ((param & PARAM_MODE) == GET) {
		list->count = limit;
		ret = wl_ioctl(interface, (ioc >> 16) & 0xffff, wlbuf, sizeof(wlbuf));
		
		if (!ret) 
			while (list->count)
				str += sprintf(str, "%s%s", ((((char *) value) == str) ? "" : " "), ether_ntoa(&list->ea[list->count-- - 1]));
		
		return ret;
	} else {
		while (*str && isspace(*str))
			*str++;
		
		if (*str == '+') {
			str++;

			list->count = limit;
			if (wl_ioctl(interface, (ioc >> 16) & 0xffff, wlbuf, sizeof(wlbuf)) == 0)
				isset = 1;

			while (*str && isspace(*str))
				str++;
		}
		
		if (!isset)
			memset(wlbuf, 0, sizeof(wlbuf));
		
		foreach(astr, str, p) {
			if (list->count >= limit)
				break;
			
			if ((addr = ether_aton(astr)) != NULL)
				memcpy(&list->ea[list->count++], addr, sizeof(struct ether_addr));
		}

		return wl_ioctl(interface, ioc & 0xffff, wlbuf, sizeof(wlbuf));
	}
}

static int wlc_radio(wlc_param param, void *data, void *value)
{
	int *val = (int *) value;
	int ret;

	if ((param & PARAM_MODE) == GET) {
		ret = wl_ioctl(interface, WLC_GET_RADIO, val, sizeof(int));
		*val = ((*val & 1) ? 0 : 1);
	} else {
		*val = (1 << 16) | (*val ? 0 : 1); 
		ret = wl_ioctl(interface, WLC_SET_RADIO, val, sizeof(int));
	}

	return ret;
}

static int wlc_wsec_key(wlc_param param, void *null, void *value)
{
	wl_wsec_key_t wsec_key;
	unsigned char *index = value;
	unsigned char *key;
	unsigned char *data;
	unsigned char hex[3];
	
	if ((param & PARAM_MODE) != SET)
		return 0;

	memset(&wsec_key, 0, sizeof(wsec_key));
	if (index[0] == '=') {
		wsec_key.flags = WL_PRIMARY_KEY;
		index++;
	}
	
	if ((index[0] < '1') || (index[0] > '4') || (index[1] != ','))
		return -1;
	
	key = index + 2;
	if (strncmp(key, "d:", 2) == 0) { /* delete key */
	} else if (strncmp(key, "s:", 2) == 0) { /* ascii key */
		key += 2;
		wsec_key.len = strlen(key);

		if ((wsec_key.len != 5) && (wsec_key.len != 13))
			return -1;
		
		strcpy(wsec_key.data, key);
	} else { /* hex key */
		wsec_key.len = strlen(key);
		if ((wsec_key.len != 10) && (wsec_key.len != 26))
			return -1;
		
		wsec_key.len /= 2;
		data = wsec_key.data;
		hex[2] = 0;
		do {
			hex[0] = *(key++);
			hex[1] = *(key++);
			*(data++) = (unsigned char) strtoul(hex, NULL, 16);
		} while (*key != 0);
	}

	return wl_bssiovar_set(interface, "wsec_key", vif, &wsec_key, sizeof(wsec_key));
}

static inline int cw2ecw(int cw)
{
	int i;	
	for (cw++, i = 0; cw; i++) cw >>=1;
	return i - 1;
}

static int wlc_wme_ac(wlc_param param, void *data, void *value)
{
	char *type = *((char **) data);
	char *settings = (char *) value;
	char cmd[100], *p, *val;
	edcf_acparam_t params[AC_COUNT];
	int ret;
	int intval;
	int cur = -1;
	char *buf = wlbuf;

	if ((param & PARAM_MODE) != SET)
		return -1;
	
	memset(params, 0, sizeof(params));
	ret = wl_iovar_get(interface, type, params, sizeof(params));
	memset(buf, 0, BUFSIZE);
	strcpy(buf, type);
	buf += strlen(buf) + 1;
	
	foreach(cmd, settings, p) {
		val = strchr(cmd, '=');
		if (val == NULL) {
			if (strcmp(cmd, "be") == 0)
				cur = AC_BE;
			else if (strcmp(cmd, "bk") == 0)
				cur = AC_BK;
			else if (strcmp(cmd, "vi") == 0)
				cur = AC_VI;
			else if (strcmp(cmd, "vo") == 0)
				cur = AC_VO;
			else
				return -1;

			/* just in case */
			params[cur].ACI = (params[cur].ACI & (0x3 << 5)) | (cur << 5);
		} else {
			*(val++) = 0;
			
			intval = strtoul(val, NULL, 10);
			if (strcmp(cmd, "cwmin") == 0)
				params[cur].ECW = (params[cur].ECW & ~(0xf)) | cw2ecw(intval);
			else if (strcmp(cmd, "ecwmin") == 0)
				params[cur].ECW = (params[cur].ECW & ~(0xf)) | (intval & 0xf);
			else if (strcmp(cmd, "cwmax") == 0)
				params[cur].ECW = (params[cur].ECW & ~(0xf << 4)) | (cw2ecw(intval) << 4);
			else if (strcmp(cmd, "ecwmax") == 0)
				params[cur].ECW = (params[cur].ECW & ~(0xf << 4)) | ((intval & 0xf) << 4);
			else if (strcmp(cmd, "aifsn") == 0)
				params[cur].ACI = (params[cur].ACI & ~(0xf)) | (intval & 0xf);
			else if (strcmp(cmd, "txop") == 0)
				params[cur].TXOP = intval >> 5;
			else if (strcmp(cmd, "force") == 0)
				params[cur].ACI = (params[cur].ACI & ~(1 << 4)) | ((intval) ? (1 << 4) : 0);
			else return -1;
			
			memcpy(buf, &params[cur], sizeof(edcf_acparam_t));
			wl_ioctl(interface, WLC_SET_VAR, wlbuf, BUFSIZE);
		}
	}
	return ret;
}

static const struct wlc_call wlc_calls[] = {
	{
		.name = "version",
		.param = STRING|NOARG,
		.handler = wlc_string,
		.data.str = VERSION,
		.desc = "Version of this program"
	},
	{
		.name = "debug",
		.param = INT,
		.handler = wlc_int,
		.data.ptr = &debug,
		.desc = "wlc debug level"
	},
	{
		.name = "stdin",
		.param = NOARG,
		.handler = wlc_flag,
		.data.ptr = &fromstdin,
		.desc = "Accept input from stdin"
	},
	{
		.name = "up",
		.param = NOARG,
		.handler = wlc_ioctl,
		.data.num = WLC_UP,
		.desc = "Bring the interface up"
	},
	{
		.name = "down",
		.param = NOARG,
		.handler = wlc_ioctl,
		.data.num = WLC_DOWN,
		.desc = "Bring the interface down"
	},
	{
		.name = "radio",
		.param = INT,
		.handler = wlc_radio,
		.desc = "Radio enabled flag"
	},
	{
		.name = "ap",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_AP << 16) | WLC_SET_AP),
		.desc = "Access Point mode"
	},
	{
		.name = "mssid",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "mssid",
		.desc = "Multi-ssid mode"
	},
	{
		.name = "apsta",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "apsta",
		.desc = "AP+STA mode"
	},
	{
		.name = "infra",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_INFRA << 16) | WLC_SET_INFRA),
		.desc = "Infrastructure mode"
	},
	{
		.name = "wet",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_WET << 16) | WLC_SET_WET),
		.desc = "Wireless repeater mode",
	},
	{
		.name = "statimeout",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "sta_retry_time",
		.desc = "STA connection timeout"
	},
	{
		.name = "country",
		.param = STRING,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_COUNTRY << 16) | WLC_SET_COUNTRY),
		.desc = "Country code"
	},
	{
		.name = "channel",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_CHANNEL << 16) | WLC_SET_CHANNEL),
		.desc = "Channel",
	},
	{
		.name = "vif",
		.param = INT,
		.handler = wlc_int,
		.data.ptr = &vif,
		.desc = "Current vif index"
	},
	{
		.name = "enabled",
		.param = INT,
		.handler = wlc_vif_enabled,
		.desc = "vif enabled flag"
	},
	{
		.name = "ssid",
		.param = STRING,
		.handler = wlc_ssid,
		.desc = "Interface ESSID"
	},
	{
		.name = "closed",
		.param = INT,
		.handler = wlc_bssiovar,
		.data.str = "closednet",
		.desc = "Hidden ESSID flag"
	},
	{
		.name = "wsec",
		.param = INT,
		.handler = wlc_bssiovar,
		.data.str = "wsec",
		.desc = "Security mode flags"
	},
	{
		.name = "wepkey",
		.param = STRING,
		.handler = wlc_wsec_key,
		.desc = "Set/Remove WEP keys"
	},
	{
		.name = "wsec_restrict",
		.param = INT,
		.handler = wlc_bssiovar,
		.data.str = "wsec_restrict",
		.desc = "Drop unencrypted traffic"
	},
	{
		.name = "eap_restrict",
		.param = INT,
		.handler = wlc_bssiovar,
		.data.str = "eap_restrict",
		.desc = "Only allow 802.1X traffic until 802.1X authorized"
	},
	{
		.name = "wpa_auth",
		.param = INT,
		.handler = wlc_bssiovar,
		.data.str = "wpa_auth",
		.desc = "WPA authentication modes"
	},
	{
		.name = "ap_isolate",
		.param = INT,
		.handler = wlc_bssiovar,
		.data.str = "ap_isolate",
		.desc = "Isolate connected clients"
	},
	{
		.name = "supplicant",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "sup_wpa",
		.desc = "Built-in WPA supplicant"
	},
	{
		.name = "maxassoc",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "maxassoc",
		.desc = "Max. number of associated clients",
	},
	{
		.name = "wme",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "wme",
		.desc = "WME enabled"
	},
	{
		.name = "wme_ac_ap",
		.param = STRING,
		.handler = wlc_wme_ac,
		.data.str = "wme_ac_ap",
		.desc = "Set WME AC options for AP mode",
	},
	{
		.name = "wme_ac_sta",
		.param = STRING,
		.handler = wlc_wme_ac,
		.data.str = "wme_ac_sta",
		.desc = "Set WME AC options for STA mode",
	},
	{
		.name = "wme_noack",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "wme_noack",
		.desc = "WME ACK disable request",
	},
	{
		.name = "fragthresh",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "fragthresh",
		.desc = "Fragmentation threshold",
	},
	{
		.name = "rtsthresh",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "rtsthresh",
		.desc = "RTS threshold"
	},
	{
		.name = "rxant",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_ANTDIV << 16) | WLC_SET_ANTDIV),
		.desc = "Rx antenna selection"
	},
	{
		.name = "txant",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_TXANT << 16) | WLC_SET_TXANT),
		.desc = "Tx antenna selection"
	},
	{
		.name = "dtim",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_DTIMPRD << 16) | WLC_SET_DTIMPRD),
		.desc = "DTIM period",
	},
	{
		.name = "bcn",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_BCNPRD << 16) | WLC_SET_BCNPRD),
		.desc = "Beacon interval"
	},
	{
		.name = "frameburst",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_FAKEFRAG << 16) | WLC_SET_FAKEFRAG),
		.desc = "Framebursting"
	},
	{
		.name = "monitor",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_MONITOR << 16) | WLC_SET_MONITOR),
		.desc = "Monitor mode"
	},
	{
		.name = "passive",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_PASSIVE << 16) | WLC_SET_PASSIVE),
		.desc = "Passive mode"
	},
	{
		.name = "macfilter",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_MACMODE << 16) | WLC_SET_MACMODE),
		.desc = "MAC filter mode (0:disabled, 1:deny, 2:allow)"
	},
	{
		.name = "maclist",
		.param = STRING,
		.data.num = ((WLC_GET_MACLIST << 16) | WLC_SET_MACLIST),
		.handler = wlc_maclist,
		.desc = "MAC filter list"
	},
	{
		.name = "autowds",
		.param = INT,
		.handler = wlc_ioctl,
		.data.num = ((WLC_GET_LAZYWDS << 16) | WLC_SET_LAZYWDS),
		.desc = "Automatic WDS"
	},
	{
		.name = "wds",
		.param = STRING,
		.data.num = ((WLC_GET_WDSLIST << 16) | WLC_SET_WDSLIST),
		.handler = wlc_maclist,
		.desc = "WDS connection list"
	},
	{
		.name = "wdstimeout",
		.param = INT,
		.handler = wlc_iovar,
		.data.str = "wdstimeout",
		.desc = "WDS link detection timeout"
	},
	{
		.name = "afterburner",
		.param = INT,
		.handler = wlc_afterburner,
		.desc = "Broadcom Afterburner"
	},
	{
		.name = "slottime",
		.param = INT,
		.handler = wlc_slottime,
		.desc = "Slot time (-1 = auto)"
	},
	{
		.name = "txack",
		.param = INT,
		.handler = wlc_noack,
		.desc = "Tx ACK enabled flag"
	},
	{
		.name = "ibss_merge",
		.param = INT,
		.handler = wlc_ibss_merge,
		.desc = "Allow IBSS merge in Ad-Hoc mode"
	}
};
#define wlc_calls_size (sizeof(wlc_calls) / sizeof(struct wlc_call))

static void usage(char *cmd)
{
	int i;
	fprintf(stderr, "Usage: %s <command> [<argument> ...]\n"
					"\n"
					"Available commands:\n", cmd);
	for (i = 0; i < wlc_calls_size; i++) {
		fprintf(stderr, "\t%-16s\t%s\n", wlc_calls[i].name ?: "", wlc_calls[i].desc ?: "");
	}
	fprintf(stderr, "\n");
	exit(1);
}

static int do_command(const struct wlc_call *cmd, char *arg)
{
	static char buf[BUFSIZE];
	int set;
	int ret = 0;
	char *format, *end;
	int intval;

	if (debug >= 10) {
		fprintf(stderr, "do_command %-16s\t'%s'\n", cmd->name, arg);
	}
	
	if ((arg == NULL) && ((cmd->param & PARAM_TYPE) != NONE)) {
		set = 0;
		ret = cmd->handler(cmd->param | GET, (void *) &cmd->data, (void *) buf);
		if (ret == 0) {
			switch(cmd->param & PARAM_TYPE) {
				case INT:
					intval = *((int *) buf);
					
					if (intval > 65535)
						format = "0x%08x\n";
					else if (intval > 255)
						format = "0x%04x\n";
					else
						format = "%d\n";
					
					fprintf(stdout, format, intval);
					break;
				case STRING:
					fprintf(stdout, "%s\n", buf);
			}
		}
	} else { /* SET */
		set = 1;
		switch(cmd->param & PARAM_TYPE) {
			case INT:
				intval = strtoul(arg, &end, 10);
				if (end && !(*end)) {
					memcpy(buf, &intval, sizeof(intval));
				} else {
					fprintf(stderr, "%s: Invalid argument\n", cmd->name);
					return -1;
				}
				break;
			case STRING:
				strncpy(buf, arg, BUFSIZE);
				buf[BUFSIZE - 1] = 0;
		}

		ret = cmd->handler(cmd->param | SET, (void *) &cmd->data, (void *) buf);
	}
	
	if ((debug > 0) && (ret != 0)) 
		fprintf(stderr, "Command '%s %s' failed: %d\n", (set == 1 ? "set" : "get"), cmd->name, ret);
	
	return ret;
}

static struct wlc_call *find_cmd(char *name)
{
	int found = 0, i = 0;

	while (!found && (i < wlc_calls_size)) {
		if (strcmp(name, wlc_calls[i].name) == 0)
			found = 1;
		else
			i++;
	}

	return (struct wlc_call *) (found ? &wlc_calls[i] : NULL);
}

int main(int argc, char **argv)
{
	static char buf[BUFSIZE];
	char *s, *s2;
	char *cmd = argv[0];
	struct wlc_call *call;
	int ret = 0;

	if (argc < 2)
		usage(argv[0]);

	for(interface[2] = '0'; (interface[2] < '3') && (wl_probe(interface) != 0); interface[2]++);
	if (interface[2] == '3') {
		fprintf(stderr, "No Broadcom wl interface found!\n");
		return -1;
	}

	argv++;
	argc--;
	while ((argc > 0) && (argv[0] != NULL)) {
		if ((call = find_cmd(argv[0])) == NULL) {
			fprintf(stderr, "Invalid command: %s\n\n", argv[0]);
			usage(cmd);
		}
		if ((argc > 1) && (!(call->param & NOARG))) {
			ret = do_command(call, argv[1]);
			argv += 2;
			argc -= 2;
		} else {
			ret = do_command(call, NULL);
			argv++;
			argc--;
		}
	}

	while (fromstdin && !feof(stdin)) {
		*buf = 0;
		fgets(buf, BUFSIZE - 1, stdin);
		
		if (*buf == 0)
			continue;
		
		if ((s = strchr(buf, '\r')) != NULL)
			*s = 0;
		if ((s = strchr(buf, '\n')) != NULL)
			*s = 0;

		s = buf;
		while (isspace(*s))
			s++;

		if (!*s)
			continue;
	
		if ((s2 = strchr(buf, ' ')) != NULL)
			*(s2++) = 0;
		
		while (s2 && isspace(*s2))
			s2++;
		
		if ((call = find_cmd(buf)) == NULL) {
			fprintf(stderr, "Invalid command: %s\n", buf);
			ret = -1;
		} else
			ret = do_command(call, ((call->param & NOARG) ? NULL : s2));
	}

	return ret;
}
