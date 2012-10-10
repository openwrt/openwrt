/*
 * iwinfo - Wireless Information Library - Lua Headers
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

#ifndef __IWINFO_LUALUB_H_
#define __IWINFO_LUALIB_H_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "iwinfo.h"
#include "iwinfo/wext_scan.h"


#define IWINFO_META			"iwinfo"
#define IWINFO_WEXT_META	"iwinfo.wext"

#ifdef USE_WL
#define IWINFO_WL_META		"iwinfo.wl"
#endif

#ifdef USE_MADWIFI
#define IWINFO_MADWIFI_META	"iwinfo.madwifi"
#endif

#ifdef USE_NL80211
#define IWINFO_NL80211_META	"iwinfo.nl80211"
#endif


#define LUA_REG(type,op) \
	{ #op, iwinfo_L_##type##_##op }

#define LUA_WRAP_INT(type,op) 							\
	static int iwinfo_L_##type##_##op(lua_State *L)		\
	{													\
		const char *ifname = luaL_checkstring(L, 1);	\
		int rv;											\
		if( !type##_get_##op(ifname, &rv) )				\
			lua_pushnumber(L, rv);						\
		else											\
			lua_pushnil(L);								\
		return 1;										\
	}

#define LUA_WRAP_STRING(type,op) 						\
	static int iwinfo_L_##type##_##op(lua_State *L)		\
	{													\
		const char *ifname = luaL_checkstring(L, 1);	\
		char rv[IWINFO_BUFSIZE];						\
		memset(rv, 0, IWINFO_BUFSIZE);					\
		if( !type##_get_##op(ifname, rv) )				\
			lua_pushstring(L, rv);						\
		else											\
			lua_pushnil(L);								\
		return 1;										\
	}

#define LUA_WRAP_STRUCT(type,op)						\
	static int iwinfo_L_##type##_##op(lua_State *L)		\
	{													\
		return iwinfo_L_##op(L, type##_get_##op);		\
	}

#endif
