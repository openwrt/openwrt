/*
 * Wireless Network Adapter configuration utility
 *
 * Copyright (C) 2005 Felix Fietkau <nbd@openwrt.org>
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

#include <stdio.h>
#include <unistd.h>
#include <iwlib.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <wlioctl.h>
#include <signal.h>

#define	ADD_VIF_RETRIES	5
// #define DEBUG

/*------------------------------------------------------------------*/
/*
 * Macro to handle errors when setting WE
 * Print a nice error message and exit...
 * We define them as macro so that "return" do the right thing.
 * The "do {...} while(0)" is a standard trick
 */
#define ERR_SET_EXT(rname, request) \
	fprintf(stderr, "Error for wireless request \"%s\" (%X) :\n", \
		rname, request)

#define ABORT_ARG_NUM(rname, request) \
	do { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    too few arguments.\n"); \
	} while(0)

#define ABORT_ARG_TYPE(rname, request, arg) \
	do { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    invalid argument \"%s\".\n", arg); \
	} while(0)

#define ABORT_ARG_SIZE(rname, request, max) \
	do { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    argument too big (max %d)\n", max); \
	} while(0)

/*------------------------------------------------------------------*/
/*
 * Wrapper to push some Wireless Parameter in the driver
 * Use standard wrapper and add pretty error message if fail...
 */
#define IW_SET_EXT_ERR(skfd, ifname, request, wrq, rname) \
	do { \
	if(iw_set_ext(skfd, ifname, request, wrq) < 0) { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    SET failed on device %-1.16s ; %s.\n", \
			ifname, strerror(errno)); \
	} } while(0)

/*------------------------------------------------------------------*/
/*
 * Wrapper to extract some Wireless Parameter out of the driver
 * Use standard wrapper and add pretty error message if fail...
 */
#define IW_GET_EXT_ERR(skfd, ifname, request, wrq, rname) \
	do { \
	if(iw_get_ext(skfd, ifname, request, wrq) < 0) { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    GET failed on device %-1.16s ; %s.\n", \
			ifname, strerror(errno)); \
	} } while(0)

static void set_wext_ssid(int skfd, char *ifname);

static char *prefix;
static char buffer[128];
static int wpa_enc = 0;

static char *wl_var(char *name)
{
	sprintf(buffer, "%s_%s", prefix, name);
	return buffer;
}

static char *vif_var(int vif, char *name)
{
	if (vif == 0)
		return wl_var(name);
	
	sprintf(buffer, "%s.%d_%s", prefix, vif, name);
	return buffer;
}

static int nvram_enabled(char *name)
{
	return (nvram_match(name, "1") || nvram_match(name, "on") || nvram_match(name, "enabled") || nvram_match(name, "true") || nvram_match(name, "yes") ? 1 : 0);
}

static int nvram_disabled(char *name)
{
	return (nvram_match(name, "0") || nvram_match(name, "off") || nvram_match(name, "disabled") || nvram_match(name, "false") || nvram_match(name, "no") ? 1 : 0);
}


/* Quarter dBm units to mW
 * Table starts at QDBM_OFFSET, so the first entry is mW for qdBm=153
 * Table is offset so the last entry is largest mW value that fits in
 * a uint16.
 */

#define QDBM_OFFSET 153
#define QDBM_TABLE_LEN 40

/* Smallest mW value that will round up to the first table entry, QDBM_OFFSET.
 * Value is ( mW(QDBM_OFFSET - 1) + mW(QDBM_OFFSET) ) / 2
 */
#define QDBM_TABLE_LOW_BOUND 6493

/* Largest mW value that will round down to the last table entry,
 * QDBM_OFFSET + QDBM_TABLE_LEN-1.
 * Value is ( mW(QDBM_OFFSET + QDBM_TABLE_LEN - 1) + mW(QDBM_OFFSET + QDBM_TABLE_LEN) ) / 2.
 */
#define QDBM_TABLE_HIGH_BOUND 64938

static const uint16 nqdBm_to_mW_map[QDBM_TABLE_LEN] = {
/* qdBm:        +0		+1		+2		+3		+4		+5		+6		+7	*/
/* 153: */      6683,	7079,	7499,	7943,	8414,	8913,	9441,	10000,
/* 161: */      10593,	11220,	11885,	12589,	13335,	14125,	14962,	15849,
/* 169: */      16788,	17783,	18836,	19953,	21135,	22387,	23714,	25119,
/* 177: */      26607,	28184,	29854,	31623,	33497,	35481,	37584,	39811,
/* 185: */      42170,	44668,	47315,	50119,	53088,	56234,	59566,	63096
};

unsigned char mw_to_qdbm(uint16 mw)
{
	char qdbm;
	int offset;
	uint mw_uint = mw;
	uint boundary;
	
	/* handle boundary case */
	if (mw_uint <= 1)
		return 0;
	
	offset = QDBM_OFFSET;
	
	/* move mw into the range of the table */
	while (mw_uint < QDBM_TABLE_LOW_BOUND) {
		mw_uint *= 10;
		offset -= 40;
	}

	for (qdbm = 0; qdbm < QDBM_TABLE_LEN-1; qdbm++) {
		boundary = nqdBm_to_mW_map[qdbm] + (nqdBm_to_mW_map[qdbm+1] - nqdBm_to_mW_map[qdbm])/2;
		if (mw_uint < boundary) break;
	}

	qdbm += (unsigned char)offset;

	return(qdbm);
}

static int bcom_ioctl(int skfd, char *ifname, int cmd, void *buf, int len)
{
	struct ifreq ifr;
	wl_ioctl_t ioc;
	int ret;
	
	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;
	
	ifr.ifr_data = (caddr_t) &ioc;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	ret = ioctl(skfd, SIOCDEVPRIVATE, &ifr);
	
#ifdef DEBUG
	if (ret < 0)
		fprintf(stderr, "IOCTL %d failed: %d\n", cmd, ret);
#endif

	return ret;
}

static int bcom_set_var(int skfd, char *ifname, char *var, void *val, int len)
{
	char buf[8192];
	int ret;
	
	if (strlen(var) + 1 > sizeof(buf) || len > sizeof(buf))
		return -1;

	bzero(buf, sizeof(buf));
	strcpy(buf, var);
	memcpy(&buf[strlen(var) + 1], val, len);
	
	ret = bcom_ioctl(skfd, ifname, WLC_SET_VAR, buf, sizeof(buf));

#ifdef DEBUG
	if (ret < 0)
		fprintf(stderr, "SET_VAR %s failed: %d\n", var, ret);
#endif
	
	return ret;
}

static int bcom_get_var(int skfd, char *ifname, char *var, void *buf, int len)
{
	int ret;
	
	if (strlen(var) + 1 > sizeof(buf) || len > sizeof(buf))
		return -1;

	bzero(buf, sizeof(buf));
	strcpy(buf, var);
	
	ret = bcom_ioctl(skfd, ifname, WLC_GET_VAR, buf, sizeof(buf));

#ifdef DEBUG
	if (ret < 0)
		fprintf(stderr, "GET_VAR %s failed: %d\n", var, ret);
#endif
	
	return ret;
}

static int bcom_set_bss_var(int skfd, char *ifname, int bss, char *var, void *val, int len)
{
	char buf[8192];
	int i = 0, ret;

	bzero(buf, sizeof(buf));
	if (strlen(var) + len + 8 > sizeof(buf) || len > sizeof(buf))
		return -1;
	
	// "bsscfg:<name>\x00" <bss> <data>
	i = sprintf(buf, "bsscfg:%s", var);
	buf[i++] = 0;
			
	memcpy(buf + i, &bss, sizeof(bss));
	i += sizeof(bss);
	
	memcpy(buf + i, val, len);
	i += len;
	
	ret = bcom_ioctl(skfd, ifname, WLC_SET_VAR, buf, i);

#ifdef DEBUG
	if (ret < 0)
		fprintf(stderr, "SET_BSS_VAR %s failed: %d\n", var, ret);
#endif

	return ret;
}

static int bcom_set_int(int skfd, char *ifname, char *var, int val)
{
	return bcom_set_var(skfd, ifname, var, &val, sizeof(val));
}

static int bcom_set_bss_int(int skfd, char *ifname, int bss, char *var, int val)
{
	return bcom_set_bss_var(skfd, ifname, bss, var, &val, sizeof(val));
}

static int is_new_bcom(int skfd, char *ifname)
{
	char buf[8192];
	
	bzero(buf, 8192);
	bcom_ioctl(skfd, ifname, WLC_DUMP, buf, 8192);

	if (strstr(buf, "3.90"))
		return 0;

	return 1;
}

static int bcom_get_wsec(int vif)
{
	int val; 

	if (nvram_match(vif_var(vif, "crypto"), "tkip"))
		val = TKIP_ENABLED;
	else if (nvram_match(vif_var(vif, "crypto"), "aes"))
		val = AES_ENABLED;
	else if (nvram_match(vif_var(vif, "crypto"), "tkip+aes") || nvram_match(vif_var(vif, "crypto"), "aes+tkip"))
		val = TKIP_ENABLED | AES_ENABLED;
	else
		val = 0;
	
	return val;
}

static int bcom_get_wauth(int vif)
{
	char *v, *next, var[80];
	int res = 0;
	
	if (!(v = nvram_get(vif_var(vif, "akm"))))
		v = nvram_safe_get(vif_var(vif, "auth_mode"));

	foreach(var, v, next) {
		if (strcmp(var, "psk") == 0)
			res |= WPA_AUTH_PSK;
		else if (strcmp(var, "psk2") == 0)
			res |= WPA2_AUTH_PSK;
		else if (strcmp(var, "wpa") == 0)
			res |= WPA_AUTH_UNSPECIFIED;
		else if (strcmp(var, "wpa2") == 0)
			res |= WPA2_AUTH_UNSPECIFIED;
	}

	return res;
}

static void stop_bcom(int skfd, char *ifname)
{
	int val = 0;
	wlc_ssid_t ssid;
	
	if (bcom_ioctl(skfd, ifname, WLC_GET_MAGIC, &val, sizeof(val)) < 0)
		return;
	
	ssid.SSID_len = 0;
	ssid.SSID[0] = 0;
	bcom_ioctl(skfd, ifname, WLC_SET_SSID, &ssid, sizeof(ssid));
	bcom_ioctl(skfd, ifname, WLC_DOWN, NULL, 0);

}

static void start_bcom(int skfd, char *ifname)
{
	int val = 0;
	
	if (bcom_ioctl(skfd, ifname, WLC_GET_MAGIC, &val, sizeof(val)) < 0)
		return;

	bcom_ioctl(skfd, ifname, WLC_UP, &val, sizeof(val));
}

static int setup_bcom_wds(int skfd, char *ifname)
{
	char buf[8192];
	char wbuf[80];
	char *v;
	int wds_enabled = 0;

	if (v = nvram_get(wl_var("wds"))) {
		struct maclist *wdslist = (struct maclist *) buf;
		struct ether_addr *addr = wdslist->ea;
		char *next;

		memset(buf, 0, 8192);
		foreach(wbuf, v, next) {
			if (ether_atoe(wbuf, addr->ether_addr_octet)) {
				wdslist->count++;
				addr++;
				wds_enabled = 1;
			}
		}
		bcom_ioctl(skfd, ifname, WLC_SET_WDSLIST, buf, sizeof(buf));
	}
	return wds_enabled;
}

static void set_wext_mode(skfd, ifname)
{
	struct iwreq wrq;
	int ap = 0, infra = 0, wet = 0;
	
	/* Set operation mode */
	ap = !nvram_match(wl_var("mode"), "sta") && !nvram_match(wl_var("mode"), "wet");
	infra = !nvram_disabled(wl_var("infra"));
	wet = !ap && nvram_match(wl_var("mode"), "wet");

	wrq.u.mode = (!infra ? IW_MODE_ADHOC : (ap ? IW_MODE_MASTER : (wet ? IW_MODE_REPEAT : IW_MODE_INFRA)));
	IW_SET_EXT_ERR(skfd, ifname, SIOCSIWMODE, &wrq, "Set Mode");
}


void start_watchdog(int skfd, char *ifname)
{
	FILE *f;
	char *v, *next;
	unsigned char buf[8192], buf2[8192], wbuf[80], *p, *tmp;
	int wds = 0, i, j, restart_wds;
	wlc_ssid_t ssid;

	if (fork())
		return;

	system("kill $(cat /var/run/wifi.pid) 2>&- >&-");
	f = fopen("/var/run/wifi.pid", "w");
	fprintf(f, "%d\n", getpid());
	fclose(f);
	
	v = nvram_safe_get(wl_var("wds"));
	memset(buf2, 0, 8192);
	p = buf2;
	foreach(wbuf, v, next) {
		if (ether_atoe(wbuf, p)) {
			p += 6;
			wds++;
		}
	}
	v = nvram_safe_get(wl_var("ssid"));
	ssid.SSID_len = strlen(v);
	strncpy(ssid.SSID, v, 32);
	
	for (;;) {
		sleep(5);

		/* client mode */
		bcom_ioctl(skfd, ifname, WLC_GET_AP, &i, sizeof(i));
		if (!i) {
			i = 0;
			if (bcom_ioctl(skfd, ifname, WLC_GET_BSSID, buf, 6) < 0) 
				i = 1;
			memcpy(buf + 6, "\x00\x00\x00\x00\x00\x00", 6);
			if (memcmp(buf, buf + 6, 6) == 0)
				i = 1;
			
			memset(buf, 0, 8192);
			strcpy(buf, "sta_info");
			bcom_ioctl(skfd, ifname, WLC_GET_BSSID, buf + strlen(buf) + 1, 6);
			if (bcom_ioctl(skfd, ifname, WLC_GET_VAR, buf, 8192) < 0) {
				i = 1;
			} else {
				sta_info_t *sta = (sta_info_t *) (buf + 4);
				if ((sta->flags & 0x18) != 0x18) 
					i = 1;
				if (sta->idle > 20)
					i = 1;
			}
			
			if (i) 
				bcom_ioctl(skfd, ifname, WLC_SET_SSID, &ssid, sizeof(ssid));
		}

		
		/* wds */
		p = buf2;
		restart_wds = 0;
		for (i = 0; i < wds; i++) {
			memset(buf, 0, 8192);
			strcpy(buf, "sta_info");
			memcpy(buf + strlen(buf) + 1, p, 6);
			if (bcom_ioctl(skfd, ifname, WLC_GET_VAR, buf, 8192) < 0) {
			} else {
				sta_info_t *sta = (sta_info_t *) (buf + 4);
				if (!(sta->flags & 0x40)) {
				} else {
					if (sta->idle > 120)
						restart_wds = 1;
				}
			}
			p += 6;
		}
		if (restart_wds)
			setup_bcom_wds(skfd, ifname);
	}
}

static void setup_bcom_vif_sec(int skfd, char *ifname, int vif)
{
	int val, wep, wsec, i;
	char *v;

	wsec = bcom_get_wsec(vif);
	if (wsec)
		val = bcom_get_wauth(vif);
	else
		val = 0;

	bcom_set_bss_int(skfd, ifname, vif, "wpa_auth", val);

	if (val) {
		if (WPA_AUTH_PSK | WPA2_AUTH_PSK) {
			v = nvram_safe_get(wl_var("wpa_psk"));
			if ((strlen(v) >= 8) && (strlen(v) < 63) && nvram_match(wl_var("mode"), "wet") && (vif == 0)) {
				/* Enable in-driver WPA supplicant */
				wsec_pmk_t pmk;
			
				pmk.key_len = (unsigned short) strlen(v);
				pmk.flags = WSEC_PASSPHRASE;
				strcpy(pmk.key, v);
				bcom_ioctl(skfd, ifname, WLC_SET_WSEC_PMK, &pmk, sizeof(pmk));
				bcom_set_int(skfd, ifname, "sup_wpa", 1);
			} else {
				bcom_set_int(skfd, ifname, "sup_wpa", 0);
			}
		}
		bcom_set_bss_int(skfd, ifname, vif, "eap_restrict", 1);
		bcom_set_bss_int(skfd, ifname, vif, "wsec", wsec);
		bcom_set_bss_int(skfd, ifname, vif, "wsec_restrict", 1);
	} else {
		bcom_set_bss_int(skfd, ifname, vif, "eap_restrict", 0);
		if (wep = nvram_enabled(vif_var(vif, "wep"))) {
			wep = atoi(nvram_safe_get(vif_var(vif, "key")));
			if ((wep >= 1) && (wep <= 4)) {
				for (i = 1; i < 4; i++) {
					wl_wsec_key_t k;
					char name[5] = "key0";
					unsigned char *kdata = k.data;
					unsigned char *kstr;

					bzero(&k, sizeof(k));
					name[3] += i;
					kstr = nvram_safe_get(vif_var(vif, name));
					k.len = strlen(kstr);
					if ((k.len == 10) || (k.len == 26)) {
						k.index = i - 1;
#ifdef DEBUG
						fprintf(stderr, "Adding WEP key %d to VIF %d: ", i, vif);
#endif
						k.len = 0;
						while (*kstr != 0) {
							strncpy(name, kstr, 2);
							name[2] = 0;
							*kdata = (unsigned char) strtoul(name, NULL, 16);
#ifdef DEBUG
							fprintf(stderr, "%02x", *kdata);
#endif
							kstr += 2;
							kdata++;
							k.len++;
						}
#ifdef DEBUG
						fprintf(stderr, "\n");
#endif
					} else {
						k.len = 0;
					}
					if ((k.len > 0) && (i == wep))
						k.flags = WL_PRIMARY_KEY;

					bcom_set_bss_var(skfd, ifname, vif, "wsec_key", &k, sizeof(k));
				}
				wep = 1;
				bcom_set_bss_int(skfd, ifname, vif, "wsec", WEP_ENABLED);
				bcom_set_bss_int(skfd, ifname, vif, "wsec_restrict", 1);
				bcom_set_bss_int(skfd, ifname, vif, "auth", nvram_enabled(vif_var(vif, "auth")));
			} else {
				wep = 0;
			}
		}
	}
	
	if (!wep && !val) {
		bcom_set_bss_int(skfd, ifname, vif, "wsec", 0);
		bcom_set_bss_int(skfd, ifname, vif, "wsec_restrict", 0);
	}
	
//	bcom_set_bss_int(skfd, ifname, vif, "auth", atoi(nvram_safe_get(vif_var(vif, "auth"))));
}

static void setup_bcom_vif(int skfd, char *ifname, int vif)
{
	int val, wep, wsec, i;
	char *s, *v;
	wlc_ssid_t ssid;
	
	s = nvram_safe_get(vif_var(vif, "ssid"));
	strncpy(ssid.SSID, s, sizeof(ssid.SSID));
	ssid.SSID_len = strlen(ssid.SSID);
	ssid.SSID_len = ((ssid.SSID_len > sizeof(ssid.SSID)) ? sizeof(ssid.SSID) : ssid.SSID_len);
	bcom_set_bss_var(skfd, ifname, vif, "ssid", &ssid, sizeof(ssid));
	
	val = nvram_enabled(vif_var(vif, "closed"));
	bcom_set_bss_int(skfd, ifname, vif, "closednet", val);

	val = nvram_enabled(wl_var("ap_isolate"));
	bcom_set_bss_int(skfd, ifname, vif, "ap_isolate", val);

}
	
static void start_bcom_vif(int skfd, char *ifname, int vif)
{
	int cfg[2];
	int i;
	
	cfg[0] = vif;
	cfg[1] = 1;
	for (i = 0; i < ADD_VIF_RETRIES; i++) {
		if (bcom_set_var(skfd, ifname, "bss" , cfg, sizeof(cfg)) == 0)
			break;
		usleep(1000 * 1000);
	}
}

static void setup_bcom_common(int skfd, char *ifname)
{
	int val = 0, ap;
	char buf[8192], wbuf[80], *v;

	nvram_set(wl_var("ifname"), ifname);

	/* Set Country */
	strncpy(buf, nvram_safe_get(wl_var("country_code")), 4);
	buf[3] = 0;
	bcom_ioctl(skfd, ifname, WLC_SET_COUNTRY, buf, 4);
	
	if (v = nvram_get(wl_var("txpwr"))) {
		val = atoi(v);
		val = mw_to_qdbm(val);
		bcom_set_int(skfd, ifname, "qtxpower", val);
	}
	
	/* Set other options */
	val = nvram_enabled(wl_var("lazywds"));
	bcom_ioctl(skfd, ifname, WLC_SET_LAZYWDS, &val, sizeof(val));

	if (v = nvram_get(wl_var("dtim"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_DTIMPRD, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("bcn"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_BCNPRD, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("antdiv"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_ANTDIV, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("txant"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_TXANT, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("maxassoc"))) {
		val = atoi(v);
		bcom_set_int(skfd, ifname, "maxassoc", val);
	}
	
	val = nvram_enabled(wl_var("frameburst"));
	bcom_ioctl(skfd, ifname, WLC_SET_FAKEFRAG, &val, sizeof(val));

	ap = !nvram_match(wl_var("mode"), "sta") && !nvram_match(wl_var("mode"), "wet");

	if (ap)
		val = setup_bcom_wds(skfd, ifname);

	if ((!ap || val) && is_new_bcom(skfd, ifname))
		start_watchdog(skfd, ifname);
	
	/* Set up afterburner, disabled it if WDS is enabled */
	if (val || nvram_enabled(wl_var("lazywds"))) {
		val = ABO_OFF;
	} else {
		val = ABO_AUTO;
		if (nvram_enabled(wl_var("afterburner")))
			val = ABO_ON;
		if (nvram_disabled(wl_var("afterburner")))
			val = ABO_OFF;
	}
	
	bcom_set_var(skfd, ifname, "afterburner_override", &val, sizeof(val));

	/* Set up MAC list */
	if (nvram_match(wl_var("macmode"), "allow"))
		val = WLC_MACMODE_ALLOW;
	else if (nvram_match(wl_var("macmode"), "deny"))
		val = WLC_MACMODE_DENY;
	else
		val = WLC_MACMODE_DISABLED;

	if ((val != WLC_MACMODE_DISABLED) && (v = nvram_get(wl_var("maclist")))) {
		struct maclist *mac_list;
		struct ether_addr *addr;
		char *next;
		
		memset(buf, 0, 8192);
		mac_list = (struct maclist *) buf;
		addr = mac_list->ea;
		
		foreach(wbuf, v, next) {
			if (ether_atoe(wbuf, addr->ether_addr_octet)) {
				mac_list->count++;
				addr++;
			}
		}
		bcom_ioctl(skfd, ifname, WLC_SET_MACLIST, buf, sizeof(buf));
	} else {
		val = WLC_MACMODE_DISABLED;
	}
	bcom_ioctl(skfd, ifname, WLC_SET_MACMODE, &val, sizeof(val));

	/* Set up G mode */
	bcom_ioctl(skfd, ifname, WLC_GET_PHYTYPE, &val, sizeof(val));
	if (val == 2) {
		int override = WLC_G_PROTECTION_OFF;
		int control = WLC_G_PROTECTION_CTL_OFF;
		
		if (v = nvram_get(wl_var("gmode"))) 
			val = atoi(v);
		else
			val = 1;

		if (val > 5)
			val = 1;

		bcom_ioctl(skfd, ifname, WLC_SET_GMODE, &val, sizeof(val));
		
		if (nvram_match(wl_var("gmode_protection"), "auto")) {
			override = WLC_G_PROTECTION_AUTO;
			control = WLC_G_PROTECTION_CTL_OVERLAP;
		}
		if (nvram_enabled(wl_var("gmode_protection"))) {
			override = WLC_G_PROTECTION_ON;
			control = WLC_G_PROTECTION_CTL_OVERLAP;
		}
		bcom_ioctl(skfd, ifname, WLC_SET_GMODE_PROTECTION_CONTROL, &override, sizeof(control));
		bcom_ioctl(skfd, ifname, WLC_SET_GMODE_PROTECTION_OVERRIDE, &override, sizeof(override));

		if (val = 0) {
			if (nvram_match(wl_var("plcphdr"), "long"))
				val = WLC_PLCP_AUTO;
			else
				val = WLC_PLCP_SHORT;

			bcom_ioctl(skfd, ifname, WLC_SET_PLCPHDR, &val, sizeof(val));
		}
	}
}

static void setup_bcom_new(int skfd, char *ifname)
{
	int val = 0, i;
	int iface[16], ifaces = 1;
	int ap, apsta, sta, wet;
	char *v;

	if (bcom_ioctl(skfd, ifname, WLC_GET_MAGIC, &val, sizeof(val)) < 0) 
		return;
	
	/* Clear all VIFs */
	iface[0] = 0;
	for (i = 0; i < 16; i++) {
		int cfg[2]; /* index, enabled */
		
		cfg[0] = i;
		cfg[1] = 0;
		
		bcom_set_var(skfd, ifname, "bss", cfg, sizeof(cfg));
		
		if ((i > 0) && nvram_enabled(vif_var(i, "enabled")) && (i == 0 || nvram_get(vif_var(i, "ssid")))) {
			iface[ifaces] = i;
			ifaces++;
		}
	}

	set_wext_mode(skfd, ifname);
	
	ap = nvram_match(wl_var("mode"), "ap") || nvram_match(wl_var("mode"), "apsta");
	apsta = nvram_match(wl_var("mode"), "apsta");
	sta = nvram_match(wl_var("mode"), "sta");

	bcom_set_int(skfd, ifname, "apsta", apsta);
	bcom_set_int(skfd, ifname, "mssid", (ifaces > 1));

	for (i = 0; i < (sta ? 0 : ifaces); i++) {
#ifdef DEBUG
		fprintf(stderr, "setup_bcom_vif(%d) start\n", iface[i]);
#endif
		setup_bcom_vif(skfd, ifname, iface[i]);
#ifdef DEBUG
		fprintf(stderr, "setup_bcom_vif(%d) end\n", iface[i]);
#endif
	}

	
	if ((val = atoi(nvram_safe_get(wl_var("rate")))) > 0) {
		val /= 500000;
		bcom_set_int(skfd, ifname, "bg_rate", val);
		bcom_set_int(skfd, ifname, "a_rate", val);
	}
	if (v = nvram_get(wl_var("rts"))) {
		val = atoi(v);
		bcom_set_int(skfd, ifname, "rtsthresh", val);
	}
	if (v = nvram_get(wl_var("frag"))) {
		val = atoi(v);
		bcom_set_int(skfd, ifname, "fragthresh", val);
	}

	val = (nvram_disabled(wl_var("radio")) ? (1 | (1 << 16)) : 0);
	bcom_ioctl(skfd, ifname, WLC_SET_RADIO, &val, sizeof(val));

	setup_bcom_common(skfd, ifname);
	start_bcom(skfd, ifname);

	val = atoi(nvram_safe_get(wl_var("channel")));
	if (val > 0)
		bcom_ioctl(skfd, ifname, WLC_SET_CHANNEL, &val, sizeof(val));
	
	val = (ap ? 15 : 0);
	bcom_ioctl(skfd, ifname, WLC_SET_CS_SCAN_TIMER, &val, sizeof(val));
	
	for (i = 0; i < (sta ? 0 : ifaces); i++) {
		setup_bcom_vif_sec(skfd, ifname, iface[i]);
	}

	for (i = 0; i < (sta ? 0 : ifaces); i++) {
		start_bcom_vif(skfd, ifname, iface[i]);
	}
}

static void setup_bcom_old(int skfd, char *ifname)
{
	int val = 0, i;
	char buf[8192];
	char wbuf[80];
	char *v;
	
	if (bcom_ioctl(skfd, ifname, WLC_GET_MAGIC, &val, sizeof(val)) < 0)
		return;
	
	setup_bcom_common(skfd, ifname);

	if ((val = atoi(nvram_safe_get(wl_var("rate")))) > 0) {
		val *= 2;
		bcom_ioctl(skfd, ifname, 13, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("frag"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_FRAG, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("rts"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_RTS, &val, sizeof(val));
	}
	
	val = nvram_enabled(wl_var("closed"));
	bcom_ioctl(skfd, ifname, WLC_SET_CLOSED, &val, sizeof(val));

	val = nvram_enabled(wl_var("ap_isolate"));
	bcom_set_int(skfd, ifname, "ap_isolate", val);

	start_bcom(skfd, ifname);
	set_wext_ssid(skfd, ifname);

	val = bcom_get_wauth(0);
	bcom_ioctl(skfd, ifname, WLC_SET_WPA_AUTH, &val, sizeof(val));

	if (val & (WPA_AUTH_PSK | WPA2_AUTH_PSK)) {
		v = nvram_safe_get(wl_var("wpa_psk"));
		if ((strlen(v) >= 8) && (strlen(v) < 63) && nvram_match(wl_var("mode"), "wet")) {
			/* Enable in-driver WPA supplicant */
			wsec_pmk_t pmk;
			
			pmk.key_len = (unsigned short) strlen(v);
			pmk.flags = WSEC_PASSPHRASE;
			strcpy(pmk.key, v);
			bcom_ioctl(skfd, ifname, WLC_SET_WSEC_PMK, &pmk, sizeof(pmk));
			bcom_set_int(skfd, ifname, "sup_wpa", 1);
		}
	}
	if (val) {
		val = 1;
		bcom_ioctl(skfd, ifname, WLC_SET_EAP_RESTRICT, &val, sizeof(val));
		val = bcom_get_wsec(0);
		bcom_ioctl(skfd, ifname, WLC_SET_WSEC, &val, sizeof(val));
	} else {
		val = 0;
		bcom_ioctl(skfd, ifname, WLC_SET_WSEC, &val, sizeof(val));
		bcom_ioctl(skfd, ifname, WLC_SET_EAP_RESTRICT, &val, sizeof(val));
		bcom_set_int(skfd, ifname, "sup_wpa", 0);
	}

	if (v = nvram_get(wl_var("auth"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_AUTH, &val, sizeof(val));
	}
}


static void set_wext_ssid(int skfd, char *ifname)
{
	char *buffer;
	char essid[IW_ESSID_MAX_SIZE + 1];
	struct iwreq wrq;

	buffer = nvram_get(wl_var("ssid"));
	
	if (!buffer || (strlen(buffer) > IW_ESSID_MAX_SIZE)) 
		buffer = "OpenWrt";

	wrq.u.essid.flags = 1;
	strcpy(essid, buffer);
	wrq.u.essid.pointer = (caddr_t) essid;
	wrq.u.essid.length = strlen(essid) + 1;
	IW_SET_EXT_ERR(skfd, ifname, SIOCSIWESSID, &wrq, "Set ESSID");
}

static void setup_wext_wep(int skfd, char *ifname)
{
	int i, keylen;
	struct iwreq wrq;
	char keystr[5];
	char *keyval;
	unsigned char key[IW_ENCODING_TOKEN_MAX];
	
	memset(&wrq, 0, sizeof(wrq));
	strcpy(keystr, "key1");
	for (i = 1; i <= 4; i++) {
		if (keyval = nvram_get(wl_var(keystr))) {
			keylen = iw_in_key(keyval, key);
			
			if (keylen > 0) {
				wrq.u.data.length = keylen;
				wrq.u.data.pointer = (caddr_t) key;
				wrq.u.data.flags = i;
				IW_SET_EXT_ERR(skfd, ifname, SIOCSIWENCODE, &wrq, "Set Encode");
			}
		}
		keystr[3]++;
	}
	
	memset(&wrq, 0, sizeof(wrq));
	i = atoi(nvram_safe_get(wl_var("key")));
	if (i > 0 && i < 4) {
		wrq.u.data.flags = i | IW_ENCODE_RESTRICTED;
		IW_SET_EXT_ERR(skfd, ifname, SIOCSIWENCODE, &wrq, "Set Encode");
	}
}

static void setup_wext(int skfd, char *ifname)
{
	char *buffer;
	struct iwreq wrq;

	/* Set channel */
	int channel = atoi(nvram_safe_get(wl_var("channel")));
	
	wrq.u.freq.m = -1;
	wrq.u.freq.e = 0;
	wrq.u.freq.flags = 0;

	if (channel > 0) {
		wrq.u.freq.flags = IW_FREQ_FIXED;
		wrq.u.freq.m = channel;
		IW_SET_EXT_ERR(skfd, ifname, SIOCSIWFREQ, &wrq, "Set Frequency");
	}

	/* Disable radio if wlX_radio is set and not enabled */
	wrq.u.txpower.disabled = nvram_disabled(wl_var("radio"));

	wrq.u.txpower.value = -1;
	wrq.u.txpower.fixed = 1;
	wrq.u.txpower.flags = IW_TXPOW_DBM;
	IW_SET_EXT_ERR(skfd, ifname, SIOCSIWTXPOW, &wrq, "Set Tx Power");

	/* Set up WEP */
	if (nvram_enabled(wl_var("wep")) && !wpa_enc)
		setup_wext_wep(skfd, ifname);
	
	/* Set ESSID */
	set_wext_ssid(skfd, ifname);

}

static int setup_interfaces(int skfd, char *ifname, char *args[], int count)
{
	struct iwreq wrq;
	int rc;
	
	/* Avoid "Unused parameter" warning */
	args = args; count = count;
	
	if(iw_get_ext(skfd, ifname, SIOCGIWNAME, &wrq) < 0)
		return 0;

	if (strncmp(ifname, "ath", 3) == 0) {
		set_wext_mode(skfd, ifname);
		setup_wext(skfd, ifname);
	} else {
		if (is_new_bcom(skfd, ifname)) {
#ifdef DEBUG
			fprintf(stderr, "New Broadcom driver detected.\n");
#endif
			stop_bcom(skfd, ifname);
#ifdef DEBUG
			fprintf(stderr, "Setup start.\n");
#endif
			setup_bcom_new(skfd, ifname);
#ifdef DEBUG
			fprintf(stderr, "Setup done.\n");
#endif
		} else {
#ifdef DEBUG
			fprintf(stderr, "Old Broadcom driver detected.\n");
#endif
			stop_bcom(skfd, ifname);
			set_wext_mode(skfd, ifname);
			setup_bcom_old(skfd, ifname);
			setup_wext(skfd, ifname);
		}
	}
	
	prefix[2]++;
}

int main(int argc, char **argv)
{
	int skfd;
	if((skfd = iw_sockets_open()) < 0) {
		perror("socket");
		exit(-1);
	}

	prefix = strdup("wl0");
	iw_enum_devices(skfd, &setup_interfaces, NULL, 0);
	
	return 0;
}
