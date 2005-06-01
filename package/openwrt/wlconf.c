/* 
 * no license, extracted from wag54gv2-AU_v1.00.39 GPL
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <shutils.h>
#include <wlutils.h>

/* phy types */
#define	PHY_TYPE_A		0
#define	PHY_TYPE_B		1
#define	PHY_TYPE_G		2
#define	PHY_TYPE_NULL		0xf

/* parts of an idcode: */
#define	IDCODE_MFG_MASK		0x00000fff
#define	IDCODE_MFG_SHIFT	0
#define	IDCODE_ID_MASK		0x0ffff000
#define	IDCODE_ID_SHIFT		12
#define	IDCODE_REV_MASK		0xf0000000
#define	IDCODE_REV_SHIFT	28

#define WL_IOCTL(name, cmd, buf, len) ((void) wl_ioctl((name), (cmd), (buf), (len)))

/* set WEP key */
static int
wlconf_set_wep_key(char *name, char *prefix, int i)
{
	wsec_key_t key;
	char wl_key[] = "wlXXXXXXXXXX_keyXXXXXXXXXX";
	char *keystr, hex[] = "XX";
	unsigned char *data = key.data;
	int ret = 0;

	memset(&key, 0, sizeof(key));
	key.index = i - 1;
	sprintf(wl_key, "%skey%d", prefix, i);
	keystr = nvram_safe_get(wl_key);

	switch (strlen(keystr)) {
	case WEP1_KEY_SIZE:
	case WEP128_KEY_SIZE:
		key.len = strlen(keystr);
		strcpy(key.data, keystr);
		break;
	case WEP1_KEY_HEX_SIZE:
	case WEP128_KEY_HEX_SIZE:
		key.len = strlen(keystr) / 2;
		while (*keystr) {
			strncpy(hex, keystr, 2);
			*data++ = (unsigned char) strtoul(hex, NULL, 16);
			keystr += 2;
		}
		break;
	default:
		key.len = 0;
		break;
	}

	/* Set current WEP key */
	if (key.len && i == atoi(nvram_safe_get(strcat_r(prefix, "key", wl_key))))
		key.flags = WSEC_PRIMARY_KEY;

	WL_IOCTL(name, WLC_SET_KEY, &key, sizeof(key));

	return ret;
}

extern struct nvram_tuple router_defaults[];

/* Keep this table in order */
static struct {
	int locale;
	char **names;
	char *abbr;
} countries[] = {
	{ WLC_WW,  ((char *[]) { "Worldwide", "WW", NULL }), "AU" },
	{ WLC_THA, ((char *[]) { "Thailand", "THA", NULL }), "TH" },
	{ WLC_ISR, ((char *[]) { "Israel", "ISR", NULL }), "IL" },
	{ WLC_JDN, ((char *[]) { "Jordan", "JDN", NULL }), "JO" },
	{ WLC_PRC, ((char *[]) { "China", "P.R. China", "PRC", NULL }), "CN" },
	{ WLC_JPN, ((char *[]) { "Japan", "JPN", NULL }), "JP" },
	{ WLC_FCC, ((char *[]) { "USA", "Canada", "ANZ", "New Zealand", "FCC", NULL }), "US" },
	{ WLC_EUR, ((char *[]) { "Europe", "EUR", NULL }), "DE" },
	{ WLC_USL, ((char *[]) { "USA Low", "USALow", "USL", NULL }), "US" },
	{ WLC_JPH, ((char *[]) { "Japan High", "JapanHigh", "JPH", NULL }), "JP" },
	{ WLC_ALL, ((char *[]) { "All", "AllTheChannels", NULL }), "All" },
};

/* validate/restore all per-interface related variables */
static void
wlconf_validate_all(char *prefix, bool restore)
{
	struct nvram_tuple *t;
	char tmp[100];
	char *v;
	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strcat_r(prefix, &t->name[3], tmp);
			if (!restore && nvram_get(tmp))
				continue;
			v = nvram_get(t->name);
			nvram_set(tmp, v ? v : t->value);
		}
	}
}

/* restore specific per-interface variable */
static void
wlconf_restore_var(char *prefix, char *name)
{
	struct nvram_tuple *t;
	char tmp[100];
	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3) && !strcmp(&t->name[3], name)) {
			nvram_set(strcat_r(prefix, name, tmp), t->value);
			break;
		}
	}
}

/* Set up wsec */
static int
wlconf_set_wsec(char *ifname, char *prefix)
{
	char tmp[100];
	int val;
	strcat_r(prefix, "wep", tmp);
	if (nvram_match(tmp, "wep") || nvram_match(tmp, "on") || nvram_match(tmp, "restricted"))
		val = WEP_ENABLED;
	else if (nvram_match(tmp, "tkip"))
		val = TKIP_ENABLED;
	else if (nvram_match(tmp, "aes"))
		val = AES_ENABLED;
	else if (nvram_match(tmp, "tkip+aes"))
		val = TKIP_ENABLED | AES_ENABLED;
	else
		val = 0;
	return wl_ioctl(ifname, WLC_SET_WSEC, &val, sizeof(val));
}

/*
* For debugging only
*/
#define WLCONF_DBG(fmt, arg...)

#if defined(linux)
#include <unistd.h>
static void
sleep_ms(const unsigned int ms)
{
	usleep(1000*ms);
}
#endif

/*
* The following condition(s) must be met when Auto Channel Selection 
* is enabled.
*  - the I/F is up (change radio channel requires it is up?)
*  - the AP must not be associated (setting SSID to empty should 
*    make sure it for us)
*/
static uint8
wlconf_auto_channel(char *name)
{
	int chosen = 0;
	wl_uint32_list_t request;
	int phytype;
	/* query the phy type */
	wl_ioctl(name, WLC_GET_PHYTYPE, &phytype, sizeof(phytype));
	request.count = 0;	/* let the ioctl decide */
	if (!wl_ioctl(name, WLC_START_CHANNEL_SEL, &request, sizeof(request))) {
		sleep_ms(phytype == PHY_TYPE_A ? 1000 : 750);
		while (wl_ioctl(name, WLC_GET_CHANNEL_SEL, &chosen, sizeof(chosen)))
			sleep_ms(100);
	}
	WLCONF_DBG("interface %s: channel selected %d\n", name, chosen);
	return chosen;
}

/* PHY type/BAND conversion */
#define WLCONF_PHYTYPE2BAND(phy)	((phy) == PHY_TYPE_A ? WLC_BAND_A : WLC_BAND_B)
/* PHY type conversion */
#define WLCONF_PHYTYPE2STR(phy)	((phy) == PHY_TYPE_A ? "a" : \
				 (phy) == PHY_TYPE_B ? "b" : "g")
#define WLCONF_STR2PHYTYPE(phy)	((phy) && (phy)[0] == 'a' ? PHY_TYPE_A : \
				 (phy) && (phy)[0] == 'b' ? PHY_TYPE_B : PHY_TYPE_G)
				 
/* configure the specified wireless interface */
int
wlconf(char *name)
{
	int restore_defaults, val, unit, phytype, gmode = 0, ret = 0;
	char tmp[100], prefix[] = "wlXXXXXXXXXX_";
	char var[80], *next, phy[] = "a", *str;
	unsigned char buf[WLC_IOCTL_MAXLEN];
	char *country;
	wlc_rev_info_t rev;
	channel_info_t ci;
	struct maclist *maclist;
	struct ether_addr *ea;
	wlc_ssid_t ssid;
	wl_rateset_t rs;
	unsigned int i;
	char eaddr[32];
	int ap, sta = 0, wet = 0;
	char country_code[4];

	/* Check interface (fail silently for non-wl interfaces) */
	if ((ret = wl_probe(name)))
		return ret;

	/* Get MAC address */
	(void) wl_hwaddr(name, buf);

	/* Get instance */
	WL_IOCTL(name, WLC_GET_INSTANCE, &unit, sizeof(unit));
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);

	/* Restore defaults if per-interface parameters do not exist */
	restore_defaults = !nvram_get(strcat_r(prefix, "ifname", tmp));
	wlconf_validate_all(prefix, restore_defaults);
	nvram_set(strcat_r(prefix, "ifname", tmp), name);
	nvram_set(strcat_r(prefix, "hwaddr", tmp), ether_etoa(buf, eaddr));
	snprintf(buf, sizeof(buf), "%d", unit);
	nvram_set(strcat_r(prefix, "unit", tmp), buf);

	/*
	* Nuke SSID first so that the AP won't be associated when WLC_UP.
	* This must be done here if Auto Channel Selection is enabled.
	*/
	WL_IOCTL(name, WLC_GET_UP, &val, sizeof(val));
	if (val) {
		/* Nuke SSID  */
		ssid.SSID_len = 0;
		ssid.SSID[0] = '\0';
		WL_IOCTL(name, WLC_SET_SSID, &ssid, sizeof(ssid));

		/* Bring the interface down */
		WL_IOCTL(name, WLC_DOWN, NULL, sizeof(val));
	}

	/* Set mode : AP, STA */
	ap = nvram_match(strcat_r(prefix, "mode", tmp), "ap");
	val = (ap + nvram_match(strcat_r(prefix, "mode", tmp), "wds")) ? 1 : 0;
	WL_IOCTL(name, WLC_SET_AP, &val, sizeof(val));

	/* Set STA specific parameters */
	if (!ap) {
		/* Set mode: WET */
		if ((wet = nvram_match(strcat_r(prefix, "mode", tmp), "wet")))
			WL_IOCTL(name, WLC_SET_WET, &wet, sizeof(wet));
		/* Set infra: BSS/IBSS */
		if (wet || (sta = nvram_match(strcat_r(prefix, "mode", tmp), "sta"))) {
			val = atoi(nvram_safe_get(strcat_r(prefix, "infra", tmp)));
			WL_IOCTL(name, WLC_SET_INFRA, &val, sizeof(val));
		}
	}

	/* Set network type */
	val = atoi(nvram_safe_get(strcat_r(prefix, "closed", tmp)));
	WL_IOCTL(name, WLC_SET_CLOSED, &val, sizeof(val));

	/* Set up the country code */
	(void) strcat_r(prefix, "country_code", tmp);
	country = nvram_get(tmp);
	if (country) {
		strncpy(country_code, country, sizeof(country_code));
		WL_IOCTL(name, WLC_SET_COUNTRY, country_code, strlen(country_code)+1);
	}
	else {
		/* If country_code doesn't exist, check for country to be backward compatible */
		(void) strcat_r(prefix, "country", tmp);
		country = nvram_safe_get(tmp);
		for (val = 0; val < ARRAYSIZE(countries); val++) {
			char **synonym;
			for (synonym = countries[val].names; *synonym; synonym++)
				if (!strcmp(country, *synonym))
					break;
			if (*synonym)
				break;
		}

		/* Get the default country code if undefined or invalid and set the NVRAM */
		if (val >= ARRAYSIZE(countries)) {
			WL_IOCTL(name, WLC_GET_COUNTRY, country_code, sizeof(country_code));
		}
		else {
			strncpy(country_code, countries[val].abbr, sizeof(country_code));
			WL_IOCTL(name, WLC_SET_COUNTRY, country_code, strlen(country_code)+1);
		}

		/* Add the new NVRAM variable */
		nvram_set("wl_country_code", country_code);
		(void) strcat_r(prefix, "country_code", tmp);
		nvram_set(tmp, country_code);
	}
	
	/* Set the MAC list */
	maclist = (struct maclist *) buf;
	maclist->count = 0;
	if (!nvram_match(strcat_r(prefix, "macmode", tmp), "disabled")) {
		ea = maclist->ea;
		foreach(var, nvram_safe_get(strcat_r(prefix, "maclist", tmp)), next) {
			if ((&ea[1])->octet > &buf[sizeof(buf)])
				break;
			if (ether_atoe(var, ea->octet)) {
				maclist->count++;
				ea++;
			}
		}
	}
	WL_IOCTL(name, WLC_SET_MACLIST, buf, sizeof(buf));

	/* Set the MAC list mode */
	(void) strcat_r(prefix, "macmode", tmp);
	if (nvram_match(tmp, "deny"))
		val = WLC_MACMODE_DENY;
	else if (nvram_match(tmp, "allow"))
		val = WLC_MACMODE_ALLOW;
	else
		val = WLC_MACMODE_DISABLED;
	WL_IOCTL(name, WLC_SET_MACMODE, &val, sizeof(val));

	/* Enable or disable the radio */
	val = nvram_match(strcat_r(prefix, "radio", tmp), "0");
	WL_IOCTL(name, WLC_SET_RADIO, &val, sizeof(val));

	/* Get supported phy types */
	WL_IOCTL(name, WLC_GET_PHYLIST, var, sizeof(var));
	nvram_set(strcat_r(prefix, "phytypes", tmp), var);

	/* Get radio IDs */
	*(next = buf) = '\0';
	for (i = 0; i < strlen(var); i++) {
		/* Switch to band */
		phy[0] = var[i];
		val = WLCONF_STR2PHYTYPE(phy);
		val = WLCONF_PHYTYPE2BAND(val);
		WL_IOCTL(name, WLC_SET_BAND, &val, sizeof(val));
		/* Get radio ID on this band */
		WL_IOCTL(name, WLC_GET_REVINFO, &rev, sizeof(rev));
		next += sprintf(next, "%sBCM%X", i ? " " : "",
				(rev.radiorev & IDCODE_ID_MASK) >> IDCODE_ID_SHIFT);
	}
	nvram_set(strcat_r(prefix, "radioids", tmp), buf);

	/* Set band */
	str = nvram_get(strcat_r(prefix, "phytype", tmp));
	val = WLCONF_STR2PHYTYPE(str);
	val = WLCONF_PHYTYPE2BAND(val);
	/* Check errors (card may have changed) */
	if (wl_ioctl(name, WLC_SET_BAND, &val, sizeof(val))) {
		/* default band to the first band in band list */
		phy[0] = var[0];
		val = WLCONF_STR2PHYTYPE(phy);
		val = WLCONF_PHYTYPE2BAND(val);
		WL_IOCTL(name, WLC_SET_BAND, &val, sizeof(val));
	}

	/* Get current core revision */
	WL_IOCTL(name, WLC_GET_REVINFO, &rev, sizeof(rev));
	snprintf(buf, sizeof(buf), "%d", rev.corerev);
	nvram_set(strcat_r(prefix, "corerev", tmp), buf);

	/* Get current phy type */
	WL_IOCTL(name, WLC_GET_PHYTYPE, &phytype, sizeof(phytype));
	snprintf(buf, sizeof(buf), "%s", WLCONF_PHYTYPE2STR(phytype));
	nvram_set(strcat_r(prefix, "phytype", tmp), buf);

	/* Set channel before setting gmode or rateset */
	/* Manual Channel Selection - when channel # is not 0 */
	val = atoi(nvram_safe_get(strcat_r(prefix, "channel", tmp)));
	if (val) {
		if (wl_ioctl(name, WLC_SET_CHANNEL, &val, sizeof(val))) {
			/* Use current channel (card may have changed) */
			WL_IOCTL(name, WLC_GET_CHANNEL, &ci, sizeof(ci));
			snprintf(buf, sizeof(buf), "%d", ci.target_channel);
			nvram_set(strcat_r(prefix, "channel", tmp), buf);
		}
	}
	
	/* Reset to hardware rateset (band may have changed) */
	WL_IOCTL(name, WLC_GET_RATESET, &rs, sizeof (wl_rateset_t));
	WL_IOCTL(name, WLC_SET_RATESET, &rs, sizeof (wl_rateset_t));

	/* Set gmode */
	if (phytype == PHY_TYPE_G) {

		/* Set gmode */

		gmode = atoi(nvram_safe_get(strcat_r(prefix, "gmode", tmp)));
		if (gmode == GMODE_AFTERBURNER) {
			if (wl_get_int(name, "abcap", &val) || !val) {
				gmode = GMODE_AUTO;
				snprintf(buf, sizeof(buf), "%d", gmode);
				nvram_set(tmp, buf);
			}
		}
		WL_IOCTL(name, WLC_SET_GMODE, &gmode, sizeof(gmode));

		/* Set gmode protection override and control algorithm */
		if (gmode != GMODE_AFTERBURNER) {
			int override = WLC_G_PROTECTION_OFF;
			int control = WLC_G_PROTECTION_CTL_OFF;
			strcat_r(prefix, "gmode_protection", tmp);
			if (nvram_match(tmp, "auto")) {
				override = WLC_G_PROTECTION_AUTO;
				control = WLC_G_PROTECTION_CTL_OVERLAP;
			}
			WL_IOCTL(name, WLC_SET_GMODE_PROTECTION_OVERRIDE, &override, sizeof(override));
			WL_IOCTL(name, WLC_SET_GMODE_PROTECTION_CONTROL, &control, sizeof(control));
		}
	}

	/* Get current rateset (gmode may have changed) */
	WL_IOCTL(name, WLC_GET_CURR_RATESET, &rs, sizeof (wl_rateset_t));

	strcat_r(prefix, "rateset", tmp);
	if (nvram_match(tmp, "all"))  {
		/* Make all rates basic */
		for (i = 0; i < rs.count; i++)
			rs.rates[i] |= 0x80;
	} else if (nvram_match(tmp, "12")) {
		/* Make 1 and 2 basic */
		for (i = 0; i < rs.count; i++) {
			if ((rs.rates[i] & 0x7f) == 2 || (rs.rates[i] & 0x7f) == 4)
				rs.rates[i] |= 0x80;
			else
				rs.rates[i] &= ~0x80;
		}
	}

	/* Set rateset */
	WL_IOCTL(name, WLC_SET_RATESET, &rs, sizeof (wl_rateset_t));

	/* Allow short preamble override for b cards */
	if (phytype == PHY_TYPE_B || gmode == 0) {
		strcat_r(prefix, "plcphdr", tmp);		
		if (nvram_match(tmp, "long"))
			val = WLC_PLCP_AUTO;
		else
			val = WLC_PLCP_SHORT;
		WL_IOCTL(name, WLC_SET_PLCPHDR, &val, sizeof(val));
	}

	/* Set rate in 500 Kbps units */
	val = atoi(nvram_safe_get(strcat_r(prefix, "rate", tmp))) / 500000;
	if (wl_ioctl(name, WLC_SET_RATE, &val, sizeof(val))) {
		/* Try default rate (card may have changed) */
		val = 0;
		WL_IOCTL(name, WLC_SET_RATE, &val, sizeof(val));
		snprintf(buf, sizeof(buf), "%d", val);
		nvram_set(strcat_r(prefix, "rate", tmp), buf);
	}

	/* Set fragmentation threshold */
	val = atoi(nvram_safe_get(strcat_r(prefix, "frag", tmp)));
	WL_IOCTL(name, WLC_SET_FRAG, &val, sizeof(val));

	/* Set RTS threshold */
	val = atoi(nvram_safe_get(strcat_r(prefix, "rts", tmp)));
	WL_IOCTL(name, WLC_SET_RTS, &val, sizeof(val));

	/* Set DTIM period */
	val = atoi(nvram_safe_get(strcat_r(prefix, "dtim", tmp)));
	WL_IOCTL(name, WLC_SET_DTIMPRD, &val, sizeof(val));

	/* Set beacon period */
	val = atoi(nvram_safe_get(strcat_r(prefix, "bcn", tmp)));
	WL_IOCTL(name, WLC_SET_BCNPRD, &val, sizeof(val));

	/* Set lazy WDS mode */
	val = atoi(nvram_safe_get(strcat_r(prefix, "lazywds", tmp)));
	WL_IOCTL(name, WLC_SET_LAZYWDS, &val, sizeof(val));

	/* Set the WDS list */
	maclist = (struct maclist *) buf;
	maclist->count = 0;
	ea = maclist->ea;
	foreach(var, nvram_safe_get(strcat_r(prefix, "wds", tmp)), next) {
		if (ea->octet > &buf[sizeof(buf)])
			break;
		ether_atoe(var, ea->octet);
		maclist->count++;
		ea++;
	}
	WL_IOCTL(name, WLC_SET_WDSLIST, buf, sizeof(buf));

	/* Set framebursting mode */
	val = nvram_match(strcat_r(prefix, "frameburst", tmp), "on");
	WL_IOCTL(name, WLC_SET_FAKEFRAG, &val, sizeof(val));
	
	/* Bring the interface back up */
	WL_IOCTL(name, WLC_UP, NULL, 0);

	/* Set antenna */
	val = atoi(nvram_safe_get(strcat_r(prefix, "antdiv", tmp)));
	WL_IOCTL(name, WLC_SET_ANTDIV, &val, sizeof(val));

	/* Auto Channel Selection - when channel # is 0 in AP mode */
	/*
	* The following condition(s) must be met in order for 
	* Auto Channel Selection to work.
	*  - the I/F must be up (change radio channel requires it is up?)
	*  - the AP must not be associated (setting SSID to empty should 
	*    make sure it for us)
	*/
	if (ap) {
		if (!(val = atoi(nvram_safe_get(strcat_r(prefix, "channel", tmp))))) {
			/* select a channel */
			val = wlconf_auto_channel(name);
			/* switch to the selected channel */
			WL_IOCTL(name, WLC_SET_CHANNEL, &val, sizeof(val));
			/* set the auto channel scan timer in the driver when in auto mode */
			val = 15;	/* 15 minutes for now */
			WL_IOCTL(name, WLC_SET_CS_SCAN_TIMER, &val, sizeof(val));
		}
		else {
			/* reset the channel scan timer in the driver when not in auto mode */
			val = 0;
			WL_IOCTL(name, WLC_SET_CS_SCAN_TIMER, &val, sizeof(val));
		}
	}

	/* Set WEP keys */
	for (i = 1; i <= WLC_MAX_DEFAULT_KEYS; i++)
		wlconf_set_wep_key(name, prefix, i);

	/* Set WSEC */
	/*
	* Need to check errors (card may have changed) and change to
	* defaults since the new chip may not support the requested 
	* encryptions after the card has been changed.
	*/
	if (wlconf_set_wsec(name, prefix)) {
		/* change nvram only, code below will pass them on */
		wlconf_restore_var(prefix, "auth_mode");
		wlconf_restore_var(prefix, "auth");
		/* reset wep to default */
		wlconf_restore_var(prefix, "wep");
		wlconf_set_wsec(name, prefix);
	}

	/* Set WPA authentication mode - radius/wpa/psk */
	strcat_r(prefix, "auth_mode", tmp);
	if (nvram_match(tmp, "radius"))
		val = WPA_AUTH_DISABLED;
	else if (nvram_match(tmp, "wpa"))
		val = WPA_AUTH_UNSPECIFIED;
	else if (nvram_match(tmp, "psk"))
		val = WPA_AUTH_PSK;
	else /* if (nvram_match(tmp, "disabled")) */
		val = WPA_AUTH_DISABLED;
	WL_IOCTL(name, WLC_SET_WPA_AUTH, &val, sizeof(val));
	
	/* Set non-WPA authentication mode - open/shared */
	val = atoi(nvram_safe_get(strcat_r(prefix, "auth", tmp)));
	WL_IOCTL(name, WLC_SET_AUTH, &val, sizeof(val));

	/* Set WEP restrict if WEP is not disabled */
	val = nvram_invmatch(strcat_r(prefix, "wep", tmp), "off");
	WL_IOCTL(name, WLC_SET_WEP_RESTRICT, &val, sizeof(val));

	/* Set SSID/Join network */
	if (ap | sta | wet) {
		strcat_r(prefix, "ssid", tmp);
		ssid.SSID_len = strlen(nvram_safe_get(tmp));
		if (ssid.SSID_len > sizeof(ssid.SSID))
			ssid.SSID_len = sizeof(ssid.SSID);
		strncpy(ssid.SSID, nvram_safe_get(tmp), ssid.SSID_len);
	} else {
		/* A zero length SSID turns off the AP */
		ssid.SSID_len = 0;
		ssid.SSID[0] = '\0';
	}		
	WL_IOCTL(name, WLC_SET_SSID, &ssid, sizeof(ssid));
	
	return 0;
}

int
wlconf_down(char *name)
{
	int val, ret = 0;
	unsigned char buf[WLC_IOCTL_MAXLEN];
	struct maclist *maclist;
	wlc_ssid_t ssid;

	/* Check interface (fail silently for non-wl interfaces) */
	if ((ret = wl_probe(name)))
		return ret;

	/* Bring down the interface */
	WL_IOCTL(name, WLC_GET_UP, &val, sizeof(val));
	if (val) {
		/* Nuke SSID  */
		ssid.SSID_len = 0;
		ssid.SSID[0] = '\0';
		WL_IOCTL(name, WLC_SET_SSID, &ssid, sizeof(ssid));

		/* Bring the interface down */
		WL_IOCTL(name, WLC_DOWN, NULL, sizeof(val));
	}

	/* Nuke the WDS list */
	maclist = (struct maclist *) buf;
	maclist->count = 0;
	WL_IOCTL(name, WLC_SET_WDSLIST, buf, sizeof(buf));

	return 0;
}

int
main(int argc, char *argv[])
{
	int ret = -1;
	
	/* Check parameters and branch based on action */
	if (argc == 3 && !strcmp(argv[2], "up"))
		ret = wlconf(argv[1]);
	else if (argc == 3 && !strcmp(argv[2], "down"))
		ret = wlconf_down(argv[1]);
	else {
		fprintf(stderr, "Usage: wlconf <ifname> up|down\n");
		return -1;
	}
	/* Check result */
	if (ret) {
		fprintf(stderr, "wlconf: %s failed (%d)\n", argv[1], ret);
		return ret;
	}
	return 0;
}
