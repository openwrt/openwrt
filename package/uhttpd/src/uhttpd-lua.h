/*
 * uhttpd - Tiny single-threaded httpd - Lua header
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _UHTTPD_LUA_

#include <math.h>  /* floor() */
#include <errno.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define UH_LUA_CALLBACK		"handle_request"

#define UH_LUA_ERR_TIMEOUT -1
#define UH_LUA_ERR_TOOBIG  -2
#define UH_LUA_ERR_PARAM   -3


lua_State * uh_lua_init();

void uh_lua_request(
	struct client *cl, struct http_request *req, lua_State *L
);

void uh_lua_close(lua_State *L);

#endif
