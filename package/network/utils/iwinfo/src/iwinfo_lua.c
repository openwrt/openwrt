/*
 * iwinfo - Wireless Information Library - Lua Bindings
 *
 *   Copyright (C) 2009 Jo-Philipp Wich <xm@subsignal.org>
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 */

#include "iwinfo/lua.h"


/* Determine type */
static int iwinfo_L_type(lua_State *L)
{
	const char *ifname = luaL_checkstring(L, 1);
	const char *type = iwinfo_type(ifname);

	if (type)
		lua_pushstring(L, type);
	else
		lua_pushnil(L);

	return 1;
}

/* Shutdown backends */
static int iwinfo_L__gc(lua_State *L)
{
	iwinfo_finish();
	return 0;
}

/*
 * Build a short textual description of the crypto info
 */

static char * iwinfo_crypto_print_ciphers(int ciphers)
{
	static char str[128] = { 0 };
	char *pos = str;

	if (ciphers & IWINFO_CIPHER_WEP40)
		pos += sprintf(pos, "WEP-40, ");

	if (ciphers & IWINFO_CIPHER_WEP104)
		pos += sprintf(pos, "WEP-104, ");

	if (ciphers & IWINFO_CIPHER_TKIP)
		pos += sprintf(pos, "TKIP, ");

	if (ciphers & IWINFO_CIPHER_CCMP)
		pos += sprintf(pos, "CCMP, ");

	if (ciphers & IWINFO_CIPHER_WRAP)
		pos += sprintf(pos, "WRAP, ");

	if (ciphers & IWINFO_CIPHER_AESOCB)
		pos += sprintf(pos, "AES-OCB, ");

	if (ciphers & IWINFO_CIPHER_CKIP)
		pos += sprintf(pos, "CKIP, ");

	if (!ciphers || (ciphers & IWINFO_CIPHER_NONE))
		pos += sprintf(pos, "NONE, ");

	*(pos - 2) = 0;

	return str;
}

static char * iwinfo_crypto_print_suites(int suites)
{
	static char str[64] = { 0 };
	char *pos = str;

	if (suites & IWINFO_KMGMT_PSK)
		pos += sprintf(pos, "PSK/");

	if (suites & IWINFO_KMGMT_8021x)
		pos += sprintf(pos, "802.1X/");

	if (!suites || (suites & IWINFO_KMGMT_NONE))
		pos += sprintf(pos, "NONE/");

	*(pos - 1) = 0;

	return str;
}

static char * iwinfo_crypto_desc(struct iwinfo_crypto_entry *c)
{
	static char desc[512] = { 0 };

	if (c)
	{
		if (c->enabled)
		{
			/* WEP */
			if (c->auth_algs && !c->wpa_version)
			{
				if ((c->auth_algs & IWINFO_AUTH_OPEN) &&
				    (c->auth_algs & IWINFO_AUTH_SHARED))
				{
					sprintf(desc, "WEP Open/Shared (%s)",
						iwinfo_crypto_print_ciphers(c->pair_ciphers));
				}
				else if (c->auth_algs & IWINFO_AUTH_OPEN)
				{
					sprintf(desc, "WEP Open System (%s)",
						iwinfo_crypto_print_ciphers(c->pair_ciphers));
				}
				else if (c->auth_algs & IWINFO_AUTH_SHARED)
				{
					sprintf(desc, "WEP Shared Auth (%s)",
						iwinfo_crypto_print_ciphers(c->pair_ciphers));
				}
			}

			/* WPA */
			else if (c->wpa_version)
			{
				switch (c->wpa_version) {
					case 3:
						sprintf(desc, "mixed WPA/WPA2 %s (%s)",
							iwinfo_crypto_print_suites(c->auth_suites),
							iwinfo_crypto_print_ciphers(
								c->pair_ciphers & c->group_ciphers));
						break;

					case 2:
						sprintf(desc, "WPA2 %s (%s)",
							iwinfo_crypto_print_suites(c->auth_suites),
							iwinfo_crypto_print_ciphers(
								c->pair_ciphers & c->group_ciphers));
						break;

					case 1:
						sprintf(desc, "WPA %s (%s)",
							iwinfo_crypto_print_suites(c->auth_suites),
							iwinfo_crypto_print_ciphers(
								c->pair_ciphers & c->group_ciphers));
						break;
				}
			}
			else
			{
				sprintf(desc, "None");
			}
		}
		else
		{
			sprintf(desc, "None");
		}
	}
	else
	{
		sprintf(desc, "Unknown");
	}

	return desc;
}

/* Build Lua table from crypto data */
static void iwinfo_L_cryptotable(lua_State *L, struct iwinfo_crypto_entry *c)
{
	int i, j;

	lua_newtable(L);

	lua_pushboolean(L, c->enabled);
	lua_setfield(L, -2, "enabled");

	lua_pushstring(L, iwinfo_crypto_desc(c));
	lua_setfield(L, -2, "description");

	lua_pushboolean(L, (c->enabled && !c->wpa_version));
	lua_setfield(L, -2, "wep");

	lua_pushinteger(L, c->wpa_version);
	lua_setfield(L, -2, "wpa");

	lua_newtable(L);
	for (i = 0, j = 1; i < 8; i++)
	{
		if (c->pair_ciphers & (1 << i))
		{
			lua_pushstring(L, IWINFO_CIPHER_NAMES[i]);
			lua_rawseti(L, -2, j++);
		}
	}
	lua_setfield(L, -2, "pair_ciphers");

	lua_newtable(L);
	for (i = 0, j = 1; i < 8; i++)
	{
		if (c->group_ciphers & (1 << i))
		{
			lua_pushstring(L, IWINFO_CIPHER_NAMES[i]);
			lua_rawseti(L, -2, j++);
		}
	}
	lua_setfield(L, -2, "group_ciphers");

	lua_newtable(L);
	for (i = 0, j = 1; i < 8; i++)
	{
		if (c->auth_suites & (1 << i))
		{
			lua_pushstring(L, IWINFO_KMGMT_NAMES[i]);
			lua_rawseti(L, -2, j++);
		}
	}
	lua_setfield(L, -2, "auth_suites");

	lua_newtable(L);
	for (i = 0, j = 1; i < 8; i++)
	{
		if (c->auth_algs & (1 << i))
		{
			lua_pushstring(L, IWINFO_AUTH_NAMES[i]);
			lua_rawseti(L, -2, j++);
		}
	}
	lua_setfield(L, -2, "auth_algs");
}


/* Wrapper for mode */
static int iwinfo_L_mode(lua_State *L, int (*func)(const char *, int *))
{
	int mode;
	const char *ifname = luaL_checkstring(L, 1);

	if ((*func)(ifname, &mode))
		mode = IWINFO_OPMODE_UNKNOWN;

	lua_pushstring(L, IWINFO_OPMODE_NAMES[mode]);
	return 1;
}

/* Wrapper for assoclist */
static int iwinfo_L_assoclist(lua_State *L, int (*func)(const char *, char *, int *))
{
	int i, len;
	char rv[IWINFO_BUFSIZE];
	char macstr[18];
	const char *ifname = luaL_checkstring(L, 1);
	struct iwinfo_assoclist_entry *e;

	lua_newtable(L);
	memset(rv, 0, sizeof(rv));

	if (!(*func)(ifname, rv, &len))
	{
		for (i = 0; i < len; i += sizeof(struct iwinfo_assoclist_entry))
		{
			e = (struct iwinfo_assoclist_entry *) &rv[i];

			sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X",
				e->mac[0], e->mac[1], e->mac[2],
				e->mac[3], e->mac[4], e->mac[5]);

			lua_newtable(L);

			lua_pushnumber(L, e->signal);
			lua_setfield(L, -2, "signal");

			lua_pushnumber(L, e->noise);
			lua_setfield(L, -2, "noise");

			lua_pushnumber(L, e->inactive);
			lua_setfield(L, -2, "inactive");

			lua_pushnumber(L, e->rx_packets);
			lua_setfield(L, -2, "rx_packets");

			lua_pushnumber(L, e->tx_packets);
			lua_setfield(L, -2, "tx_packets");

			lua_pushnumber(L, e->rx_rate.rate);
			lua_setfield(L, -2, "rx_rate");

			lua_pushnumber(L, e->tx_rate.rate);
			lua_setfield(L, -2, "tx_rate");

			if (e->rx_rate.mcs >= 0)
			{
				lua_pushnumber(L, e->rx_rate.mcs);
				lua_setfield(L, -2, "rx_mcs");

				lua_pushboolean(L, e->rx_rate.is_40mhz);
				lua_setfield(L, -2, "rx_40mhz");

				lua_pushboolean(L, e->rx_rate.is_short_gi);
				lua_setfield(L, -2, "rx_short_gi");
			}

			if (e->tx_rate.mcs >= 0)
			{
				lua_pushnumber(L, e->tx_rate.mcs);
				lua_setfield(L, -2, "tx_mcs");

				lua_pushboolean(L, e->tx_rate.is_40mhz);
				lua_setfield(L, -2, "tx_40mhz");

				lua_pushboolean(L, e->tx_rate.is_short_gi);
				lua_setfield(L, -2, "tx_short_gi");
			}

			lua_setfield(L, -2, macstr);
		}
	}

	return 1;
}

/* Wrapper for tx power list */
static int iwinfo_L_txpwrlist(lua_State *L, int (*func)(const char *, char *, int *))
{
	int i, x, len;
	char rv[IWINFO_BUFSIZE];
	const char *ifname = luaL_checkstring(L, 1);
	struct iwinfo_txpwrlist_entry *e;

	memset(rv, 0, sizeof(rv));

	if (!(*func)(ifname, rv, &len))
	{
		lua_newtable(L);

		for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_txpwrlist_entry), x++)
		{
			e = (struct iwinfo_txpwrlist_entry *) &rv[i];

			lua_newtable(L);

			lua_pushnumber(L, e->mw);
			lua_setfield(L, -2, "mw");

			lua_pushnumber(L, e->dbm);
			lua_setfield(L, -2, "dbm");

			lua_rawseti(L, -2, x);
		}

		return 1;
	}

	return 0;
}

/* Wrapper for scan list */
static int iwinfo_L_scanlist(lua_State *L, int (*func)(const char *, char *, int *))
{
	int i, x, len;
	char rv[IWINFO_BUFSIZE];
	char macstr[18];
	const char *ifname = luaL_checkstring(L, 1);
	struct iwinfo_scanlist_entry *e;

	lua_newtable(L);
	memset(rv, 0, sizeof(rv));

	if (!(*func)(ifname, rv, &len))
	{
		for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_scanlist_entry), x++)
		{
			e = (struct iwinfo_scanlist_entry *) &rv[i];

			lua_newtable(L);

			/* BSSID */
			sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X",
				e->mac[0], e->mac[1], e->mac[2],
				e->mac[3], e->mac[4], e->mac[5]);

			lua_pushstring(L, macstr);
			lua_setfield(L, -2, "bssid");

			/* ESSID */
			if (e->ssid[0])
			{
				lua_pushstring(L, (char *) e->ssid);
				lua_setfield(L, -2, "ssid");
			}

			/* Channel */
			lua_pushinteger(L, e->channel);
			lua_setfield(L, -2, "channel");

			/* Mode */
			lua_pushstring(L, IWINFO_OPMODE_NAMES[e->mode]);
			lua_setfield(L, -2, "mode");

			/* Quality, Signal */
			lua_pushinteger(L, e->quality);
			lua_setfield(L, -2, "quality");

			lua_pushinteger(L, e->quality_max);
			lua_setfield(L, -2, "quality_max");

			lua_pushnumber(L, (e->signal - 0x100));
			lua_setfield(L, -2, "signal");

			/* Crypto */
			iwinfo_L_cryptotable(L, &e->crypto);
			lua_setfield(L, -2, "encryption");

			lua_rawseti(L, -2, x);
		}
	}

	return 1;
}

/* Wrapper for frequency list */
static int iwinfo_L_freqlist(lua_State *L, int (*func)(const char *, char *, int *))
{
	int i, x, len;
	char rv[IWINFO_BUFSIZE];
	const char *ifname = luaL_checkstring(L, 1);
	struct iwinfo_freqlist_entry *e;

	lua_newtable(L);
	memset(rv, 0, sizeof(rv));

	if (!(*func)(ifname, rv, &len))
	{
		for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_freqlist_entry), x++)
		{
			e = (struct iwinfo_freqlist_entry *) &rv[i];

			lua_newtable(L);

			/* MHz */
			lua_pushinteger(L, e->mhz);
			lua_setfield(L, -2, "mhz");

			/* Channel */
			lua_pushinteger(L, e->channel);
			lua_setfield(L, -2, "channel");

			/* Restricted (DFS/TPC/Radar) */
			lua_pushboolean(L, e->restricted);
			lua_setfield(L, -2, "restricted");

			lua_rawseti(L, -2, x);
		}
	}

	return 1;
}

/* Wrapper for crypto settings */
static int iwinfo_L_encryption(lua_State *L, int (*func)(const char *, char *))
{
	const char *ifname = luaL_checkstring(L, 1);
	struct iwinfo_crypto_entry c = { 0 };

	if (!(*func)(ifname, (char *)&c))
	{
		iwinfo_L_cryptotable(L, &c);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

/* Wrapper for hwmode list */
static int iwinfo_L_hwmodelist(lua_State *L, int (*func)(const char *, int *))
{
	const char *ifname = luaL_checkstring(L, 1);
	int hwmodes = 0;

	if (!(*func)(ifname, &hwmodes))
	{
		lua_newtable(L);

		lua_pushboolean(L, hwmodes & IWINFO_80211_A);
		lua_setfield(L, -2, "a");

		lua_pushboolean(L, hwmodes & IWINFO_80211_B);
		lua_setfield(L, -2, "b");

		lua_pushboolean(L, hwmodes & IWINFO_80211_G);
		lua_setfield(L, -2, "g");

		lua_pushboolean(L, hwmodes & IWINFO_80211_N);
		lua_setfield(L, -2, "n");

		return 1;
	}

	lua_pushnil(L);
	return 1;
}

/* Wrapper for mbssid_support */
static int iwinfo_L_mbssid_support(lua_State *L, int (*func)(const char *, int *))
{
	const char *ifname = luaL_checkstring(L, 1);
	int support = 0;

	if (!(*func)(ifname, &support))
	{
		lua_pushboolean(L, support);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

/* Wrapper for hardware_id */
static int iwinfo_L_hardware_id(lua_State *L, int (*func)(const char *, char *))
{
	const char *ifname = luaL_checkstring(L, 1);
	struct iwinfo_hardware_id ids;

	if (!(*func)(ifname, (char *)&ids))
	{
		lua_newtable(L);

		lua_pushnumber(L, ids.vendor_id);
		lua_setfield(L, -2, "vendor_id");

		lua_pushnumber(L, ids.device_id);
		lua_setfield(L, -2, "device_id");

		lua_pushnumber(L, ids.subsystem_vendor_id);
		lua_setfield(L, -2, "subsystem_vendor_id");

		lua_pushnumber(L, ids.subsystem_device_id);
		lua_setfield(L, -2, "subsystem_device_id");
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

/* Wrapper for country list */
static char * iwinfo_L_country_lookup(char *buf, int len, int iso3166)
{
	int i;
	struct iwinfo_country_entry *c;

	for (i = 0; i < len; i += sizeof(struct iwinfo_country_entry))
	{
		c = (struct iwinfo_country_entry *) &buf[i];

		if (c->iso3166 == iso3166)
			return c->ccode;
	}

	return NULL;
}

static int iwinfo_L_countrylist(lua_State *L, int (*func)(const char *, char *, int *))
{
	int len, i, j;
	char rv[IWINFO_BUFSIZE], alpha2[3];
	char *ccode;
	const char *ifname = luaL_checkstring(L, 1);
	const struct iwinfo_iso3166_label *l;

	lua_newtable(L);
	memset(rv, 0, sizeof(rv));

	if (!(*func)(ifname, rv, &len))
	{
		for (l = IWINFO_ISO3166_NAMES, j = 1; l->iso3166; l++)
		{
			if ((ccode = iwinfo_L_country_lookup(rv, len, l->iso3166)) != NULL)
			{
				sprintf(alpha2, "%c%c",
					(l->iso3166 / 256), (l->iso3166 % 256));

				lua_newtable(L);

				lua_pushstring(L, alpha2);
				lua_setfield(L, -2, "alpha2");

				lua_pushstring(L, ccode);
				lua_setfield(L, -2, "ccode");

				lua_pushstring(L, l->name);
				lua_setfield(L, -2, "name");

				lua_rawseti(L, -2, j++);
			}
		}
	}

	return 1;
}


#ifdef USE_WL
/* Broadcom */
LUA_WRAP_INT(wl,channel)
LUA_WRAP_INT(wl,frequency)
LUA_WRAP_INT(wl,frequency_offset)
LUA_WRAP_INT(wl,txpower)
LUA_WRAP_INT(wl,txpower_offset)
LUA_WRAP_INT(wl,bitrate)
LUA_WRAP_INT(wl,signal)
LUA_WRAP_INT(wl,noise)
LUA_WRAP_INT(wl,quality)
LUA_WRAP_INT(wl,quality_max)
LUA_WRAP_STRING(wl,ssid)
LUA_WRAP_STRING(wl,bssid)
LUA_WRAP_STRING(wl,country)
LUA_WRAP_STRING(wl,hardware_name)
LUA_WRAP_STRING(wl,phyname)
LUA_WRAP_STRUCT(wl,mode)
LUA_WRAP_STRUCT(wl,assoclist)
LUA_WRAP_STRUCT(wl,txpwrlist)
LUA_WRAP_STRUCT(wl,scanlist)
LUA_WRAP_STRUCT(wl,freqlist)
LUA_WRAP_STRUCT(wl,countrylist)
LUA_WRAP_STRUCT(wl,hwmodelist)
LUA_WRAP_STRUCT(wl,encryption)
LUA_WRAP_STRUCT(wl,mbssid_support)
LUA_WRAP_STRUCT(wl,hardware_id)
#endif

#ifdef USE_MADWIFI
/* Madwifi */
LUA_WRAP_INT_OP(madwifi,channel)
LUA_WRAP_INT_OP(madwifi,frequency)
LUA_WRAP_INT_OP(madwifi,frequency_offset)
LUA_WRAP_INT_OP(madwifi,txpower)
LUA_WRAP_INT_OP(madwifi,txpower_offset)
LUA_WRAP_INT_OP(madwifi,bitrate)
LUA_WRAP_INT_OP(madwifi,signal)
LUA_WRAP_INT_OP(madwifi,noise)
LUA_WRAP_INT_OP(madwifi,quality)
LUA_WRAP_INT_OP(madwifi,quality_max)
LUA_WRAP_STRING_OP(madwifi,ssid)
LUA_WRAP_STRING_OP(madwifi,bssid)
LUA_WRAP_STRING_OP(madwifi,country)
LUA_WRAP_STRING_OP(madwifi,hardware_name)
LUA_WRAP_STRING_OP(madwifi,phyname)
LUA_WRAP_STRUCT_OP(madwifi,mode)
LUA_WRAP_STRUCT_OP(madwifi,assoclist)
LUA_WRAP_STRUCT_OP(madwifi,txpwrlist)
LUA_WRAP_STRUCT_OP(madwifi,scanlist)
LUA_WRAP_STRUCT_OP(madwifi,freqlist)
LUA_WRAP_STRUCT_OP(madwifi,countrylist)
LUA_WRAP_STRUCT_OP(madwifi,hwmodelist)
LUA_WRAP_STRUCT_OP(madwifi,encryption)
LUA_WRAP_STRUCT_OP(madwifi,mbssid_support)
LUA_WRAP_STRUCT_OP(madwifi,hardware_id)
#endif

#ifdef USE_NL80211
/* NL80211 */
LUA_WRAP_INT_OP(nl80211,channel)
LUA_WRAP_INT_OP(nl80211,frequency)
LUA_WRAP_INT_OP(nl80211,frequency_offset)
LUA_WRAP_INT_OP(nl80211,txpower)
LUA_WRAP_INT_OP(nl80211,txpower_offset)
LUA_WRAP_INT_OP(nl80211,bitrate)
LUA_WRAP_INT_OP(nl80211,signal)
LUA_WRAP_INT_OP(nl80211,noise)
LUA_WRAP_INT_OP(nl80211,quality)
LUA_WRAP_INT_OP(nl80211,quality_max)
LUA_WRAP_STRING_OP(nl80211,ssid)
LUA_WRAP_STRING_OP(nl80211,bssid)
LUA_WRAP_STRING_OP(nl80211,country)
LUA_WRAP_STRING_OP(nl80211,hardware_name)
LUA_WRAP_STRING_OP(nl80211,phyname)
LUA_WRAP_STRUCT_OP(nl80211,mode)
LUA_WRAP_STRUCT_OP(nl80211,assoclist)
LUA_WRAP_STRUCT_OP(nl80211,txpwrlist)
LUA_WRAP_STRUCT_OP(nl80211,scanlist)
LUA_WRAP_STRUCT_OP(nl80211,freqlist)
LUA_WRAP_STRUCT_OP(nl80211,countrylist)
LUA_WRAP_STRUCT_OP(nl80211,hwmodelist)
LUA_WRAP_STRUCT_OP(nl80211,encryption)
LUA_WRAP_STRUCT_OP(nl80211,mbssid_support)
LUA_WRAP_STRUCT_OP(nl80211,hardware_id)
#endif

/* Wext */
LUA_WRAP_INT_OP(wext,channel)
LUA_WRAP_INT_OP(wext,frequency)
LUA_WRAP_INT_OP(wext,frequency_offset)
LUA_WRAP_INT_OP(wext,txpower)
LUA_WRAP_INT_OP(wext,txpower_offset)
LUA_WRAP_INT_OP(wext,bitrate)
LUA_WRAP_INT_OP(wext,signal)
LUA_WRAP_INT_OP(wext,noise)
LUA_WRAP_INT_OP(wext,quality)
LUA_WRAP_INT_OP(wext,quality_max)
LUA_WRAP_STRING_OP(wext,ssid)
LUA_WRAP_STRING_OP(wext,bssid)
LUA_WRAP_STRING_OP(wext,country)
LUA_WRAP_STRING_OP(wext,hardware_name)
LUA_WRAP_STRING_OP(wext,phyname)
LUA_WRAP_STRUCT_OP(wext,mode)
LUA_WRAP_STRUCT_OP(wext,assoclist)
LUA_WRAP_STRUCT_OP(wext,txpwrlist)
LUA_WRAP_STRUCT_OP(wext,scanlist)
LUA_WRAP_STRUCT_OP(wext,freqlist)
LUA_WRAP_STRUCT_OP(wext,countrylist)
LUA_WRAP_STRUCT_OP(wext,hwmodelist)
LUA_WRAP_STRUCT_OP(wext,encryption)
LUA_WRAP_STRUCT_OP(wext,mbssid_support)
LUA_WRAP_STRUCT_OP(wext,hardware_id)

#ifdef USE_WL
/* Broadcom table */
static const luaL_reg R_wl[] = {
	LUA_REG(wl,channel),
	LUA_REG(wl,frequency),
	LUA_REG(wl,frequency_offset),
	LUA_REG(wl,txpower),
	LUA_REG(wl,txpower_offset),
	LUA_REG(wl,bitrate),
	LUA_REG(wl,signal),
	LUA_REG(wl,noise),
	LUA_REG(wl,quality),
	LUA_REG(wl,quality_max),
	LUA_REG(wl,mode),
	LUA_REG(wl,ssid),
	LUA_REG(wl,bssid),
	LUA_REG(wl,country),
	LUA_REG(wl,assoclist),
	LUA_REG(wl,txpwrlist),
	LUA_REG(wl,scanlist),
	LUA_REG(wl,freqlist),
	LUA_REG(wl,countrylist),
	LUA_REG(wl,hwmodelist),
	LUA_REG(wl,encryption),
	LUA_REG(wl,mbssid_support),
	LUA_REG(wl,hardware_id),
	LUA_REG(wl,hardware_name),
	LUA_REG(wl,phyname),
	{ NULL, NULL }
};
#endif

#ifdef USE_MADWIFI
/* Madwifi table */
static const luaL_reg R_madwifi[] = {
	LUA_REG(madwifi,channel),
	LUA_REG(madwifi,frequency),
	LUA_REG(madwifi,frequency_offset),
	LUA_REG(madwifi,txpower),
	LUA_REG(madwifi,txpower_offset),
	LUA_REG(madwifi,bitrate),
	LUA_REG(madwifi,signal),
	LUA_REG(madwifi,noise),
	LUA_REG(madwifi,quality),
	LUA_REG(madwifi,quality_max),
	LUA_REG(madwifi,mode),
	LUA_REG(madwifi,ssid),
	LUA_REG(madwifi,bssid),
	LUA_REG(madwifi,country),
	LUA_REG(madwifi,assoclist),
	LUA_REG(madwifi,txpwrlist),
	LUA_REG(madwifi,scanlist),
	LUA_REG(madwifi,freqlist),
	LUA_REG(madwifi,countrylist),
	LUA_REG(madwifi,hwmodelist),
	LUA_REG(madwifi,encryption),
	LUA_REG(madwifi,mbssid_support),
	LUA_REG(madwifi,hardware_id),
	LUA_REG(madwifi,hardware_name),
	LUA_REG(madwifi,phyname),
	{ NULL, NULL }
};
#endif

#ifdef USE_NL80211
/* NL80211 table */
static const luaL_reg R_nl80211[] = {
	LUA_REG(nl80211,channel),
	LUA_REG(nl80211,frequency),
	LUA_REG(nl80211,frequency_offset),
	LUA_REG(nl80211,txpower),
	LUA_REG(nl80211,txpower_offset),
	LUA_REG(nl80211,bitrate),
	LUA_REG(nl80211,signal),
	LUA_REG(nl80211,noise),
	LUA_REG(nl80211,quality),
	LUA_REG(nl80211,quality_max),
	LUA_REG(nl80211,mode),
	LUA_REG(nl80211,ssid),
	LUA_REG(nl80211,bssid),
	LUA_REG(nl80211,country),
	LUA_REG(nl80211,assoclist),
	LUA_REG(nl80211,txpwrlist),
	LUA_REG(nl80211,scanlist),
	LUA_REG(nl80211,freqlist),
	LUA_REG(nl80211,countrylist),
	LUA_REG(nl80211,hwmodelist),
	LUA_REG(nl80211,encryption),
	LUA_REG(nl80211,mbssid_support),
	LUA_REG(nl80211,hardware_id),
	LUA_REG(nl80211,hardware_name),
	LUA_REG(nl80211,phyname),
	{ NULL, NULL }
};
#endif

/* Wext table */
static const luaL_reg R_wext[] = {
	LUA_REG(wext,channel),
	LUA_REG(wext,frequency),
	LUA_REG(wext,frequency_offset),
	LUA_REG(wext,txpower),
	LUA_REG(wext,txpower_offset),
	LUA_REG(wext,bitrate),
	LUA_REG(wext,signal),
	LUA_REG(wext,noise),
	LUA_REG(wext,quality),
	LUA_REG(wext,quality_max),
	LUA_REG(wext,mode),
	LUA_REG(wext,ssid),
	LUA_REG(wext,bssid),
	LUA_REG(wext,country),
	LUA_REG(wext,assoclist),
	LUA_REG(wext,txpwrlist),
	LUA_REG(wext,scanlist),
	LUA_REG(wext,freqlist),
	LUA_REG(wext,countrylist),
	LUA_REG(wext,hwmodelist),
	LUA_REG(wext,encryption),
	LUA_REG(wext,mbssid_support),
	LUA_REG(wext,hardware_id),
	LUA_REG(wext,hardware_name),
	LUA_REG(wext,phyname),
	{ NULL, NULL }
};

/* Common */
static const luaL_reg R_common[] = {
	{ "type", iwinfo_L_type },
	{ "__gc", iwinfo_L__gc  },
	{ NULL, NULL }
};


LUALIB_API int luaopen_iwinfo(lua_State *L) {
	luaL_register(L, IWINFO_META, R_common);

#ifdef USE_WL
	luaL_newmetatable(L, IWINFO_WL_META);
	luaL_register(L, NULL, R_common);
	luaL_register(L, NULL, R_wl);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_setfield(L, -2, "wl");
#endif

#ifdef USE_MADWIFI
	luaL_newmetatable(L, IWINFO_MADWIFI_META);
	luaL_register(L, NULL, R_common);
	luaL_register(L, NULL, R_madwifi);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_setfield(L, -2, "madwifi");
#endif

#ifdef USE_NL80211
	luaL_newmetatable(L, IWINFO_NL80211_META);
	luaL_register(L, NULL, R_common);
	luaL_register(L, NULL, R_nl80211);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_setfield(L, -2, "nl80211");
#endif

	luaL_newmetatable(L, IWINFO_WEXT_META);
	luaL_register(L, NULL, R_common);
	luaL_register(L, NULL, R_wext);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_setfield(L, -2, "wext");

	return 1;
}
