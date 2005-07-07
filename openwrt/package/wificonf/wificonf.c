/*
 * Wireless Network Adapter configuration utility
 *
 * Copyright (C) 2005 Felix Fietkau <nbd@vd-s.ath.cx>
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
		return; \
	} while(0)

#define ABORT_ARG_TYPE(rname, request, arg) \
	do { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    invalid argument \"%s\".\n", arg); \
		return; \
	} while(0)

#define ABORT_ARG_SIZE(rname, request, max) \
	do { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    argument too big (max %d)\n", max); \
		return; \
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
		return; \
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
		return; \
	} } while(0)

char *prefix;
char buffer[128];

char *wl_var(char *name)
{
	strcpy(buffer, prefix);
	strcat(buffer, name);
}

int nvram_enabled(char *name)
{
	return (nvram_match(name, "1") || nvram_match(name, "on") || nvram_match(name, "enabled") || nvram_match(name, "true") || nvram_match(name, "yes") ? 1 : 0);
}

int nvram_disabled(char *name)
{
	return (nvram_match(name, "0") || nvram_match(name, "off") || nvram_match(name, "disabled") || nvram_match(name, "false") || nvram_match(name, "no") ? 1 : 0);
}


int bcom_ioctl(int skfd, char *ifname, int cmd, void *buf, int len)
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
	if (ret < 0)
		fprintf(stderr, "bcom_ioctl [cmd=%d, buf=%08x, len=%d] failed: %d\n", cmd, buf, len, ret);

	return ret;
}

int bcom_set_val(int skfd, char *ifname, char *var, void *val, int len)
{
	char buf[8192];
	int ret;
	
	if (strlen(var) + 1 > sizeof(buf) || len > sizeof(buf))
		return -1;

	strcpy(buf, var);
	
	if ((ret = bcom_ioctl(skfd, ifname, WLC_GET_VAR, buf, sizeof(buf))))
		return ret;

	memcpy(val, buf, len);
	return 0;	
}

int bcom_set_int(int skfd, char *ifname, char *var, int val)
{
	return bcom_set_val(skfd, ifname, var, &val, sizeof(val));
}

void setup_bcom(int skfd, char *ifname)
{
	int val = 0;
	char buf[8192];
	char wbuf[80];
	char *v;
	
	if (bcom_ioctl(skfd, ifname, WLC_GET_MAGIC, &val, sizeof(val)) < 0)
		return;

	bcom_ioctl(skfd, ifname, WLC_DOWN, NULL, 0);
	
	if (nvram_match(wl_var("auth_mode"), "wpa") || nvram_match(wl_var("auth_mode"), "psk") || (nvram_get(wl_var("akm")) && !nvram_disabled(wl_var("akm")))) {
		/* Set up WPA */
		if (nvram_match(wl_var("crypto"), "tkip"))
			val = TKIP_ENABLED;
		else if (nvram_match(wl_var("crypto"), "aes"))
			val = AES_ENABLED;
		else if (nvram_match(wl_var("crypto"), "tkip+aes"))
			val = TKIP_ENABLED | AES_ENABLED;
		else
			val = 0;
		bcom_ioctl(skfd, ifname, WLC_SET_WSEC, &val, sizeof(val));

		if (val && nvram_get(wl_var("wpa_psk"))) {
			val = 1;
			bcom_ioctl(skfd, ifname, WLC_SET_EAP_RESTRICT, &val, sizeof(val));
		}
	}

	/* Set up afterburner */
	val = ABO_AUTO;
	if (nvram_enabled(wl_var("afterburner")))
		val = ABO_ON;
	if (nvram_disabled(wl_var("afterburner")))
		val = ABO_OFF;
	bcom_set_val(skfd, ifname, "afterburner_override", &val, sizeof(val));
	
	/* Set other options */
	if (v = nvram_get(wl_var("lazywds"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_LAZYWDS, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("frag"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_FRAG, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("dtim"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_DTIMPRD, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("bcn"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_BCNPRD, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("rts"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_RTS, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("antdiv"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_ANTDIV, &val, sizeof(val));
	}
	if (v = nvram_get(wl_var("txant"))) {
		val = atoi(v);
		bcom_ioctl(skfd, ifname, WLC_SET_TXANT, &val, sizeof(val));
	}
	
	val = nvram_enabled(wl_var("closed"));
	bcom_ioctl(skfd, ifname, WLC_SET_CLOSED, &val, sizeof(val));

	val = nvram_enabled(wl_var("ap_isolate"));
	bcom_set_int(skfd, ifname, "ap_isolate", val);

	val = nvram_enabled(wl_var("frameburst"));
	bcom_ioctl(skfd, ifname, WLC_SET_FAKEFRAG, &val, sizeof(val));

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

	if (v = nvram_get(wl_var("wds"))) {
		struct maclist *wdslist = (struct maclist *) buf;
		struct ether_addr *addr = wdslist->ea;
		char *next;

		memset(buf, 0, 8192);
		foreach(wbuf, v, next) {
			if (ether_atoe(wbuf, addr->ether_addr_octet)) {
				wdslist->count++;
				addr++;
			}
		}
		bcom_ioctl(skfd, ifname, WLC_SET_WDSLIST, buf, sizeof(buf));
	}
	
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

void set_wext_ssid(int skfd, char *ifname)
{
	char *buffer;
	struct iwreq wrq;

	if (buffer = nvram_get(wl_var("ssid"))) {
		if (strlen(buffer) > IW_ESSID_MAX_SIZE) {
			ABORT_ARG_SIZE("Set ESSID", SIOCSIWESSID, IW_ESSID_MAX_SIZE);
		} else {
			char essid[IW_ESSID_MAX_SIZE + 1];

			wrq.u.essid.flags = 1;
			strcpy(essid, buffer);
			wrq.u.essid.pointer = (caddr_t) essid;
			wrq.u.essid.length = strlen(essid) + 1;
			IW_SET_EXT_ERR(skfd, ifname, SIOCSIWESSID, &wrq, "Set ESSID");
		}
	}
}

void start_bcom(int skfd, char *ifname)
{
	int val = 0;
	
	if (bcom_ioctl(skfd, ifname, WLC_GET_MAGIC, &val, sizeof(val)) < 0)
		return;

	bcom_ioctl(skfd, ifname, WLC_UP, &val, sizeof(val));
	
	/* Need to re-set SSID after WLC_UP */
	set_wext_ssid(skfd, ifname);
}

void setup_wext_wep(int skfd, char *ifname)
{
	int i, keylen;
	struct iwreq wrq;
	char keystr[5];
	char *keyval;
	unsigned char key[IW_ENCODING_TOKEN_MAX];

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
	
	
	i = atoi(nvram_safe_get(wl_var("key")));
	if (i > 0 && i < 4) {
		wrq.u.data.flags = i | IW_ENCODE_RESTRICTED;
		IW_SET_EXT_ERR(skfd, ifname, SIOCSIWENCODE, &wrq, "Set Encode");
	}
}

void setup_wext(int skfd, char *ifname)
{
	char *buffer;
	struct iwreq wrq;

	/* Set ESSID */
	set_wext_ssid(skfd, ifname);
	
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
	
	/* Set operation mode */
	int ap = 0, infra = 0, wet = 0;
	
	ap = !nvram_match(wl_var("mode"), "sta");
	infra = !nvram_disabled(wl_var("infra"));
	wet = nvram_enabled(wl_var("wet"));

	wrq.u.mode = (!infra ? IW_MODE_ADHOC : (ap ? IW_MODE_MASTER : (wet ? IW_MODE_REPEAT : IW_MODE_INFRA)));
	IW_SET_EXT_ERR(skfd, ifname, SIOCSIWMODE, &wrq, "Set Mode");

	/* Disable radio if wlX_radio is set and not enabled */
	wrq.u.txpower.disabled = nvram_disabled(wl_var("radio"));

	wrq.u.txpower.value = -1;
	wrq.u.txpower.fixed = 1;
	wrq.u.txpower.flags = IW_TXPOW_MWATT;
	IW_SET_EXT_ERR(skfd, ifname, SIOCSIWTXPOW, &wrq, "Set Tx Power");

	/* Set up WEP */
	if (nvram_enabled(wl_var("wep")))
		setup_wext_wep(skfd, ifname);

}


static int setup_interfaces(int skfd, char *ifname, char *args[], int count)
{
	struct iwreq wrq;
	int rc;
	
	/* Avoid "Unused parameter" warning */
	args = args; count = count;
	
	if(iw_get_ext(skfd, ifname, SIOCGIWNAME, &wrq) < 0)
		return 0;

	setup_bcom(skfd, ifname);
	setup_wext(skfd, ifname);
	start_bcom(skfd, ifname);
	prefix[2]++;
}

int main(int argc, char **argv)
{
	int skfd;
	if((skfd = iw_sockets_open()) < 0) {
		perror("socket");
		exit(-1);
	}

	prefix = strdup("wl0_");
	iw_enum_devices(skfd, &setup_interfaces, NULL, 0);
	
	return 0;
}
