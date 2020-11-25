/*
Copyright 2015-2018 Jo-Philipp Wich <jo@mein.io>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <netlink/socket.h>
#include <linux/rtnetlink.h>

#define LUCI_IP "luci.ip"
#define LUCI_IP_CIDR "luci.ip.cidr"

#define RTA_INT(x)	(*(int *)RTA_DATA(x))
#define RTA_U32(x)	(*(uint32_t *)RTA_DATA(x))

#define AF_BITS(f) \
	((f) == AF_INET ? 32 : \
		((f) == AF_INET6 ? 128 : \
			((f) == AF_PACKET ? 48 : 0)))

#define AF_BYTES(f) \
	((f) == AF_INET ? 4 : \
		((f) == AF_INET6 ? 16 : \
			((f) == AF_PACKET ? 6 : 0)))

static int hz = 0;
static struct nl_sock *sock = NULL;

typedef struct {
	union {
		struct in_addr v4;
		struct in6_addr v6;
		struct ether_addr mac;
		uint8_t u8[16];
	} addr;
	uint32_t scope;
	uint16_t family;
	int16_t bits;
} cidr_t;

struct dump_filter {
	bool get;
	int family;
	int iif;
	int oif;
	int type;
	int scope;
	int proto;
	int table;
	cidr_t gw;
	cidr_t from;
	cidr_t src;
	cidr_t dst;
	struct ether_addr mac;
	bool from_exact;
	bool dst_exact;
};

struct dump_state {
	int index;
	int pending;
	int callback;
	struct lua_State *L;
	struct dump_filter *filter;
};


static int _cidr_new(lua_State *L, int index, int family, bool mask);

static cidr_t *L_checkcidr (lua_State *L, int index, cidr_t *p)
{
	if (lua_type(L, index) == LUA_TUSERDATA)
		return luaL_checkudata(L, index, LUCI_IP_CIDR);

	if (_cidr_new(L, index, p ? p->family : 0, false))
		return lua_touserdata(L, -1);

	luaL_error(L, "Invalid operand");
	return NULL;
}

static bool parse_mac(const char *mac, struct ether_addr *ea)
{
	unsigned long int n;
	char *e, sep = 0;
	int i;

	for (i = 0; i < 6; i++)
	{
		if (i > 0)
		{
			if (sep == 0 && (mac[0] == ':' || mac[0] == '-'))
				sep = mac[0];

			if (sep == 0 || mac[0] != sep)
				return false;

			mac++;
		}

		n = strtoul(mac, &e, 16);

		if (n > 0xFF)
			return false;

		mac += (e - mac);
		ea->ether_addr_octet[i] = n;
	}

	if (mac[0] != 0)
		return false;

	return true;
}

static bool parse_mask(int family, const char *mask, int16_t *bits)
{
	char *e;
	union {
		struct in_addr v4;
		struct in6_addr v6;
		struct ether_addr mac;
		uint8_t u8[16];
	} m;

	if (family == AF_INET && inet_pton(AF_INET, mask, &m.v4))
	{
		for (*bits = 0, m.v4.s_addr = ntohl(m.v4.s_addr);
			 *bits < AF_BITS(AF_INET) && (m.v4.s_addr << *bits) & 0x80000000;
			 ++*bits);
	}
	else if ((family == AF_INET6 && inet_pton(AF_INET6, mask, &m.v6)) ||
	         (family == AF_PACKET && parse_mac(mask, &m.mac)))
	{
		for (*bits = 0;
			 *bits < AF_BITS(family) && (m.u8[*bits / 8] << (*bits % 8)) & 128;
			 ++*bits);
	}
	else
	{
		*bits = strtoul(mask, &e, 10);

		if (e == mask || *e != 0 || *bits > AF_BITS(family))
			return false;
	}

	return true;
}

static bool parse_cidr(const char *dest, cidr_t *pp)
{
	char *p, *s, buf[INET6_ADDRSTRLEN * 2 + 2];

	strncpy(buf, dest, sizeof(buf) - 1);

	p = strchr(buf, '/');

	if (p)
		*p++ = 0;

	s = strchr(buf, '%');

	if (s)
		*s++ = 0;

	if (inet_pton(AF_INET, buf, &pp->addr.v4))
		pp->family = AF_INET;
	else if (inet_pton(AF_INET6, buf, &pp->addr.v6))
		pp->family = AF_INET6;
	else if (parse_mac(buf, &pp->addr.mac))
		pp->family = AF_PACKET;
	else
		return false;

	if (s)
	{
		if (pp->family != AF_INET6)
			return false;

		if (!(pp->addr.v6.s6_addr[0] == 0xFE &&
		      pp->addr.v6.s6_addr[1] >= 0x80 &&
		      pp->addr.v6.s6_addr[2] <= 0xBF))
			return false;

		pp->scope = if_nametoindex(s);

		if (pp->scope == 0)
			return false;
	}
	else {
		pp->scope = 0;
	}

	if (p)
	{
		if (!parse_mask(pp->family, p, &pp->bits))
			return false;
	}
	else
	{
		pp->bits = AF_BITS(pp->family);
	}

	return true;
}

static int format_cidr(lua_State *L, cidr_t *p)
{
	char *s, buf[INET6_ADDRSTRLEN + 1 + IF_NAMESIZE + 4];

	if (p->family == AF_PACKET)
	{
		snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
		         p->addr.mac.ether_addr_octet[0],
		         p->addr.mac.ether_addr_octet[1],
		         p->addr.mac.ether_addr_octet[2],
		         p->addr.mac.ether_addr_octet[3],
		         p->addr.mac.ether_addr_octet[4],
		         p->addr.mac.ether_addr_octet[5]);

		if (p->bits < AF_BITS(AF_PACKET))
			lua_pushfstring(L, "%s/%d", buf, p->bits);
		else
			lua_pushstring(L, buf);
	}
	else
	{
		inet_ntop(p->family, &p->addr.v6, buf, sizeof(buf));

		s = buf + strlen(buf);

		if (p->scope != 0 && if_indextoname(p->scope, s + 1) != NULL) {
			*s++ = '%';
			s += strlen(s);
		}

		if (p->bits < AF_BITS(p->family))
			s += sprintf(s, "/%d", p->bits);

		lua_pushstring(L, buf);
	}

	return 1;
}

static int L_getint(lua_State *L, int index, const char *name)
{
	int rv = 0;

	lua_getfield(L, index, name);

	if (lua_type(L, -1) == LUA_TNUMBER)
		rv = lua_tonumber(L, -1);

	lua_pop(L, 1);

	return rv;
}

static const char * L_getstr(lua_State *L, int index, const char *name)
{
	const char *rv = NULL;

	lua_getfield(L, index, name);

	if (lua_type(L, -1) == LUA_TSTRING)
		rv = lua_tostring(L, -1);

	lua_pop(L, 1);

	return rv;
}

static void L_setint(struct lua_State *L, const char *name, uint32_t n)
{
	lua_pushinteger(L, n);
	lua_setfield(L, -2, name);
}

static void L_setbool(struct lua_State *L, const char *name, bool val)
{
	lua_pushboolean(L, val);
	lua_setfield(L, -2, name);
}

static void L_setaddr(struct lua_State *L, const char *name,
                      int family, void *addr, int bits)
{
	cidr_t *p;

	if (!addr)
		return;

	p = lua_newuserdata(L, sizeof(*p));

	if (!p)
		return;

	if (family == AF_INET)
	{
		p->family = AF_INET;
		p->bits = (bits < 0) ? AF_BITS(AF_INET) : bits;
		p->addr.v4 = *(struct in_addr *)addr;
		p->scope = 0;
	}
	else if (family == AF_INET6)
	{
		p->family = AF_INET6;
		p->bits = (bits < 0) ? AF_BITS(AF_INET6) : bits;
		p->addr.v6 = *(struct in6_addr *)addr;
		p->scope = 0;
	}
	else
	{
		p->family = AF_PACKET;
		p->bits = (bits < 0) ? AF_BITS(AF_PACKET) : bits;
		p->addr.mac = *(struct ether_addr *)addr;
		p->scope = 0;
	}

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	lua_setfield(L, -2, name);
}

static void L_setstr(struct lua_State *L, const char *name, const char *val)
{
	lua_pushstring(L, val);
	lua_setfield(L, -2, name);
}

static void L_setdev(struct lua_State *L, const char *name,
                     struct nlattr *attr)
{
	char buf[32];

	if (if_indextoname(RTA_INT(attr), buf))
		L_setstr(L, name, buf);
}

static int L_checkbits(lua_State *L, int index, cidr_t *p)
{
	int16_t s16;
	int bits;

	if (lua_gettop(L) < index || lua_isnil(L, index))
	{
		bits = p->bits;
	}
	else if (lua_type(L, index) == LUA_TNUMBER)
	{
		bits = lua_tointeger(L, index);

		if (bits < 0 || bits > AF_BITS(p->family))
			return luaL_error(L, "Invalid prefix size");
	}
	else if (lua_type(L, index) == LUA_TSTRING)
	{
		if (!parse_mask(p->family, lua_tostring(L, index), &s16))
			return luaL_error(L, "Invalid netmask format");

		bits = s16;
	}
	else
	{
		return luaL_error(L, "Invalid data type");
	}

	return bits;
}

static int _cidr_new(lua_State *L, int index, int family, bool mask)
{
	uint32_t n;
	const char *addr;
	cidr_t cidr = { }, *cidrp;

	if (lua_type(L, index) == LUA_TNUMBER)
	{
		n = htonl(lua_tointeger(L, index));

		if (family == AF_INET6)
		{
			cidr.family = AF_INET6;
			cidr.addr.v6.s6_addr[12] = n;
			cidr.addr.v6.s6_addr[13] = (n >> 8);
			cidr.addr.v6.s6_addr[14] = (n >> 16);
			cidr.addr.v6.s6_addr[15] = (n >> 24);
		}
		else if (family == AF_INET)
		{
			cidr.family = AF_INET;
			cidr.addr.v4.s_addr = n;
		}
		else
		{
			cidr.family = AF_PACKET;
			cidr.addr.mac.ether_addr_octet[2] = n;
			cidr.addr.mac.ether_addr_octet[3] = (n >> 8);
			cidr.addr.mac.ether_addr_octet[4] = (n >> 16);
			cidr.addr.mac.ether_addr_octet[5] = (n >> 24);
		}

		cidr.bits = AF_BITS(cidr.family);
	}
	else
	{
		addr = luaL_checkstring(L, index);

		if (!parse_cidr(addr, &cidr))
			return 0;

		if (family && cidr.family != family)
			return 0;

		if (mask)
			cidr.bits = L_checkbits(L, index + 1, &cidr);
	}

	if (!(cidrp = lua_newuserdata(L, sizeof(*cidrp))))
		return 0;

	*cidrp = cidr;
	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_new(lua_State *L)
{
	return _cidr_new(L, 1, 0, true);
}

static int cidr_ipv4(lua_State *L)
{
	return _cidr_new(L, 1, AF_INET, true);
}

static int cidr_ipv6(lua_State *L)
{
	return _cidr_new(L, 1, AF_INET6, true);
}

static int cidr_mac(lua_State *L)
{
	return _cidr_new(L, 1, AF_PACKET, true);
}

static int cidr_check(lua_State *L, int family)
{
	cidr_t cidr = { }, *cidrp;
	const char *addr;

	if (lua_type(L, 1) == LUA_TSTRING)
	{
		addr = lua_tostring(L, 1);

		if (addr && parse_cidr(addr, &cidr) && cidr.family == family)
			return format_cidr(L, &cidr);
	}
	else
	{
		cidrp = lua_touserdata(L, 1);

		if (cidrp == NULL)
			return 0;

		if (!lua_getmetatable(L, 1))
			return 0;

		lua_getfield(L, LUA_REGISTRYINDEX, LUCI_IP_CIDR);

		if (!lua_rawequal(L, -1, -2))
			cidrp = NULL;

		lua_pop(L, 2);

		if (cidrp != NULL && cidrp->family == family)
			return format_cidr(L, cidrp);
	}

	return 0;
}

static int cidr_checkip4(lua_State *L)
{
	return cidr_check(L, AF_INET);
}

static int cidr_checkip6(lua_State *L)
{
	return cidr_check(L, AF_INET6);
}

static int cidr_checkmac(lua_State *L)
{
	return cidr_check(L, AF_PACKET);
}

static int cidr_is4(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);

	lua_pushboolean(L, p->family == AF_INET);
	return 1;
}

static int cidr_is4rfc1918(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);
	uint32_t a = htonl(p->addr.v4.s_addr);

	lua_pushboolean(L, (p->family == AF_INET &&
	                    ((a >= 0x0A000000 && a <= 0x0AFFFFFF) ||
	                     (a >= 0xAC100000 && a <= 0xAC1FFFFF) ||
	                     (a >= 0xC0A80000 && a <= 0xC0A8FFFF))));

	return 1;
}

static int cidr_is4linklocal(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);
	uint32_t a = htonl(p->addr.v4.s_addr);

	lua_pushboolean(L, (p->family == AF_INET &&
	                    a >= 0xA9FE0000 &&
	                    a <= 0xA9FEFFFF));

	return 1;
}

static bool _is_mapped4(cidr_t *p)
{
	return (p->family == AF_INET6 &&
	        p->addr.v6.s6_addr[0] == 0 &&
	        p->addr.v6.s6_addr[1] == 0 &&
	        p->addr.v6.s6_addr[2] == 0 &&
	        p->addr.v6.s6_addr[3] == 0 &&
	        p->addr.v6.s6_addr[4] == 0 &&
	        p->addr.v6.s6_addr[5] == 0 &&
	        p->addr.v6.s6_addr[6] == 0 &&
	        p->addr.v6.s6_addr[7] == 0 &&
	        p->addr.v6.s6_addr[8] == 0 &&
	        p->addr.v6.s6_addr[9] == 0 &&
	        p->addr.v6.s6_addr[10] == 0xFF &&
	        p->addr.v6.s6_addr[11] == 0xFF);
}

static int cidr_is6mapped4(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);

	lua_pushboolean(L, _is_mapped4(p));
	return 1;
}

static int cidr_is6(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);

	lua_pushboolean(L, p->family == AF_INET6);
	return 1;
}

static int cidr_is6linklocal(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);

	lua_pushboolean(L, (p->family == AF_INET6 &&
	                    p->addr.v6.s6_addr[0] == 0xFE &&
	                    p->addr.v6.s6_addr[1] >= 0x80 &&
	                    p->addr.v6.s6_addr[1] <= 0xBF));

	return 1;
}

static int cidr_ismac(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);

	lua_pushboolean(L, p->family == AF_PACKET);
	return 1;
}

static int cidr_ismacmcast(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);

	lua_pushboolean(L, (p->family == AF_PACKET &&
	                    (p->addr.mac.ether_addr_octet[0] & 0x1)));

	return 1;
}

static int cidr_ismaclocal(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);

	lua_pushboolean(L, (p->family == AF_PACKET &&
	                    (p->addr.mac.ether_addr_octet[0] & 0x2)));

	return 1;
}

static int _cidr_cmp(lua_State *L)
{
	cidr_t *a = L_checkcidr(L, 1, NULL);
	cidr_t *b = L_checkcidr(L, 2, NULL);

	if (a->family != b->family)
		return (a->family - b->family);

	return memcmp(&a->addr.v6, &b->addr.v6, AF_BYTES(a->family));
}

static int cidr_lower(lua_State *L)
{
	lua_pushboolean(L, _cidr_cmp(L) < 0);
	return 1;
}

static int cidr_higher(lua_State *L)
{
	lua_pushboolean(L, _cidr_cmp(L) > 0);
	return 1;
}

static int cidr_equal(lua_State *L)
{
	lua_pushboolean(L, _cidr_cmp(L) == 0);
	return 1;
}

static int cidr_lower_equal(lua_State *L)
{
	lua_pushboolean(L, _cidr_cmp(L) <= 0);
	return 1;
}

static int cidr_prefix(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);
	int bits = L_checkbits(L, 2, p);

	p->bits = bits;
	lua_pushinteger(L, p->bits);
	return 1;
}

static void _apply_mask(cidr_t *p, int bits, bool inv)
{
	uint8_t b, i;

	if (bits <= 0)
	{
		memset(&p->addr.u8, inv * 0xFF, AF_BYTES(p->family));
	}
	else if (p->family == AF_INET && bits <= AF_BITS(AF_INET))
	{
		if (inv)
			p->addr.v4.s_addr |= ntohl((1 << (AF_BITS(AF_INET) - bits)) - 1);
		else
			p->addr.v4.s_addr &= ntohl(~((1 << (AF_BITS(AF_INET) - bits)) - 1));
	}
	else if (bits <= AF_BITS(p->family))
	{
		for (i = 0; i < AF_BYTES(p->family); i++)
		{
			b = (bits > 8) ? 8 : bits;
			if (inv)
				p->addr.u8[i] |= ~((uint8_t)(0xFF << (8 - b)));
			else
				p->addr.u8[i] &= (uint8_t)(0xFF << (8 - b));
			bits -= b;
		}
	}
}

static int cidr_network(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL), *p2;
	int bits = L_checkbits(L, 2, p1);

	if (!(p2 = lua_newuserdata(L, sizeof(*p2))))
		return 0;

	*p2 = *p1;
	p2->bits = AF_BITS(p1->family);
	_apply_mask(p2, bits, false);

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_host(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2 = lua_newuserdata(L, sizeof(*p2));

	if (!p2)
		return 0;

	*p2 = *p1;
	p2->bits = AF_BITS(p1->family);

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_mask(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL), *p2;
	int bits = L_checkbits(L, 2, p1);

	if (!(p2 = lua_newuserdata(L, sizeof(*p2))))
		return 0;

	p2->scope = 0;
	p2->bits = AF_BITS(p1->family);
	p2->family = p1->family;

	memset(&p2->addr.v6.s6_addr, 0xFF, sizeof(p2->addr.v6.s6_addr));
	_apply_mask(p2, bits, false);

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_broadcast(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2;
	int bits = L_checkbits(L, 2, p1);

	if (p1->family != AF_INET)
		return 0;

	if (!(p2 = lua_newuserdata(L, sizeof(*p2))))
		return 0;

	*p2 = *p1;
	p2->bits = AF_BITS(AF_INET);
	_apply_mask(p2, bits, true);

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_mapped4(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2;

	if (!_is_mapped4(p1))
		return 0;

	if (!(p2 = lua_newuserdata(L, sizeof(*p2))))
		return 0;

	p2->scope = 0;
	p2->family = AF_INET;
	p2->bits = (p1->bits > AF_BITS(AF_INET)) ? AF_BITS(AF_INET) : p1->bits;
	memcpy(&p2->addr.v4, p1->addr.v6.s6_addr + 12, sizeof(p2->addr.v4));

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_unscoped(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2;

	if (p1->family != AF_INET6)
		return 0;

	if (!(p2 = lua_newuserdata(L, sizeof(*p2))))
		return 0;

	*p2 = *p1;
	p2->scope = 0;

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_tolinklocal(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2;
	int i;

	if (p1->family != AF_PACKET)
		return 0;

	if (!(p2 = lua_newuserdata(L, sizeof(*p2))))
		return 0;

	p2->scope = p1->scope;
	p2->family = AF_INET6;
	p2->bits = AF_BITS(AF_INET6);
	p2->addr.u8[0] = 0xFE;
	p2->addr.u8[1] = 0x80;
	p2->addr.u8[8] = p1->addr.u8[0] ^ 0x02;
	p2->addr.u8[9] = p1->addr.u8[1];
	p2->addr.u8[10] = p1->addr.u8[2];
	p2->addr.u8[11] = 0xFF;
	p2->addr.u8[12] = 0xFE;
	p2->addr.u8[13] = p1->addr.u8[3];
	p2->addr.u8[14] = p1->addr.u8[4];
	p2->addr.u8[15] = p1->addr.u8[5];

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_tomac(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2;
	int i;

	if (p1->family != AF_INET6 ||
	    p1->addr.u8[0] != 0xFE ||
	    p1->addr.u8[1] != 0x80 ||
	    p1->addr.u8[2] != 0x00 ||
	    p1->addr.u8[3] != 0x00 ||
	    p1->addr.u8[4] != 0x00 ||
	    p1->addr.u8[5] != 0x00 ||
	    p1->addr.u8[6] != 0x00 ||
	    p1->addr.u8[7] != 0x00 ||
	    p1->addr.u8[11] != 0xFF ||
	    p1->addr.u8[12] != 0xFE)
	    return 0;

	if (!(p2 = lua_newuserdata(L, sizeof(*p2))))
		return 0;

	p2->scope = 0;
	p2->family = AF_PACKET;
	p2->bits = AF_BITS(AF_PACKET);
	p2->addr.u8[0] = p1->addr.u8[8] ^ 0x02;
	p2->addr.u8[1] = p1->addr.u8[9];
	p2->addr.u8[2] = p1->addr.u8[10];
	p2->addr.u8[3] = p1->addr.u8[13];
	p2->addr.u8[4] = p1->addr.u8[14];
	p2->addr.u8[5] = p1->addr.u8[15];

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_contains(lua_State *L)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2 = L_checkcidr(L, 2, NULL);
	cidr_t a = *p1, b = *p2;
	bool rv = false;

	if (p1->family == p2->family && p1->bits <= p2->bits)
	{
		_apply_mask(&a, p1->bits, false);
		_apply_mask(&b, p1->bits, false);

		rv = !memcmp(&a.addr.v6, &b.addr.v6, AF_BYTES(a.family));
	}

	lua_pushboolean(L, rv);
	return 1;
}

#define BYTE(a, i) \
	(a)->addr.u8[AF_BYTES((a)->family) - (i) - 1]

static int _cidr_add_sub(lua_State *L, bool add)
{
	cidr_t *p1 = L_checkcidr(L, 1, NULL);
	cidr_t *p2 = L_checkcidr(L, 2, p1);
	cidr_t r = *p1;
	bool inplace = lua_isboolean(L, 3) ? lua_toboolean(L, 3) : false;
	bool ok = true;
	uint8_t i, carry;
	uint32_t a, b;

	if (p1->family == p2->family)
	{
		if (p1->family == AF_INET)
		{
			a = ntohl(p1->addr.v4.s_addr);
			b = ntohl(p2->addr.v4.s_addr);

			/* would over/underflow */
			if ((add && (UINT_MAX - a) < b) || (!add && a < b))
			{
				r.addr.v4.s_addr = add * 0xFFFFFFFF;
				ok = false;
			}
			else
			{
				r.addr.v4.s_addr = add ? htonl(a + b) : htonl(a - b);
			}
		}
		else
		{
			for (i = 0, carry = 0; i < AF_BYTES(p1->family); i++)
			{
				if (add)
				{
					BYTE(&r, i) = BYTE(p1, i) + BYTE(p2, i) + carry;
					carry = (BYTE(p1, i) + BYTE(p2, i) + carry) / 256;
				}
				else
				{
					BYTE(&r, i) = (BYTE(p1, i) - BYTE(p2, i) - carry);
					carry = (BYTE(p1, i) < (BYTE(p2, i) + carry));
				}
			}

			/* would over/underflow */
			if (carry)
			{
				memset(&r.addr.u8, add * 0xFF, AF_BYTES(r.family));
				ok = false;
			}
		}
	}
	else
	{
		ok = false;
	}

	if (inplace)
	{
		*p1 = r;
		lua_pushboolean(L, ok);
		return 1;
	}

	if (!(p1 = lua_newuserdata(L, sizeof(*p1))))
		return 0;

	*p1 = r;

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_add(lua_State *L)
{
	return _cidr_add_sub(L, true);
}

static int cidr_sub(lua_State *L)
{
	return _cidr_add_sub(L, false);
}

static int cidr_minhost(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);
	cidr_t r = *p;
	uint8_t i, rest, carry;

	_apply_mask(&r, r.bits, false);

	if (r.family == AF_INET && r.bits < AF_BITS(AF_INET))
	{
		r.bits = AF_BITS(AF_INET);
		r.addr.v4.s_addr = htonl(ntohl(r.addr.v4.s_addr) + 1);
	}
	else if (r.bits < AF_BITS(r.family))
	{
		r.bits = AF_BITS(r.family);

		for (i = 0, carry = 1; i < AF_BYTES(r.family); i++)
		{
			rest = (BYTE(&r, i) + carry) > 255;
			BYTE(&r, i) += carry;
			carry = rest;
		}
	}

	if (!(p = lua_newuserdata(L, sizeof(*p))))
		return 0;

	*p = r;

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_maxhost(lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);
	cidr_t r = *p;

	_apply_mask(&r, r.bits, true);

	if (r.family == AF_INET && r.bits < AF_BITS(AF_INET))
	{
		r.bits = AF_BITS(AF_INET);
		r.addr.v4.s_addr = htonl(ntohl(r.addr.v4.s_addr) - 1);
	}
	else
	{
		r.bits = AF_BITS(r.family);
	}

	if (!(p = lua_newuserdata(L, sizeof(*p))))
		return 0;

	*p = r;

	luaL_getmetatable(L, LUCI_IP_CIDR);
	lua_setmetatable(L, -2);
	return 1;
}

static int cidr_gc (lua_State *L)
{
	return 0;
}

static int cidr_tostring (lua_State *L)
{
	cidr_t *p = L_checkcidr(L, 1, NULL);
	return format_cidr(L, p);
}

/*
 * route functions
 */

static bool diff_prefix(int family, void *addr, int bits, bool exact, cidr_t *p)
{
	uint8_t i, b, r, *a;
	uint32_t m;

	if (!p->family)
		return false;

	if (!addr || p->family != family || p->bits > bits)
		return true;

	if (family == AF_INET)
	{
		m = p->bits ? htonl(~((1 << (AF_BITS(AF_INET) - p->bits)) - 1)) : 0;

		if ((((struct in_addr *)addr)->s_addr & m) != (p->addr.v4.s_addr & m))
			return true;
	}
	else
	{
		for (i = 0, a = addr, r = p->bits; i < AF_BYTES(p->family); i++)
		{
			b = r ? (0xFF << (8 - ((r > 8) ? 8 : r))) : 0;

			if ((a[i] & b) != (p->addr.u8[i] & b))
				return true;

			r -= ((r > 8) ? 8 : r);
		}
	}

	return (exact && p->bits != bits);
}

static int cb_dump_route(struct nl_msg *msg, void *arg)
{
	struct dump_state *s = arg;
	struct dump_filter *f = s->filter;
	struct nlmsghdr *hdr = nlmsg_hdr(msg);
	struct rtmsg *rt = NLMSG_DATA(hdr);
	struct nlattr *tb[RTA_MAX+1];
	struct in6_addr *src, *dst, *gw, *from, def = { };
	int iif, oif, bitlen;
	uint32_t table;

	if (hdr->nlmsg_type != RTM_NEWROUTE ||
	    (rt->rtm_family != AF_INET && rt->rtm_family != AF_INET6))
		return NL_SKIP;

	nlmsg_parse(hdr, sizeof(*rt), tb, RTA_MAX, NULL);

	iif   = tb[RTA_IIF]     ? RTA_INT(tb[RTA_IIF])      : 0;
	oif   = tb[RTA_OIF]     ? RTA_INT(tb[RTA_OIF])      : 0;
	table = tb[RTA_TABLE]   ? RTA_U32(tb[RTA_TABLE])    : rt->rtm_table;
	from  = tb[RTA_SRC]     ? RTA_DATA(tb[RTA_SRC])     : NULL;
	src   = tb[RTA_PREFSRC] ? RTA_DATA(tb[RTA_PREFSRC]) : NULL;
	dst   = tb[RTA_DST]     ? RTA_DATA(tb[RTA_DST])     : &def;
	gw    = tb[RTA_GATEWAY] ? RTA_DATA(tb[RTA_GATEWAY]) : NULL;

	bitlen = AF_BITS(rt->rtm_family);

	if (!f->get) {
		if ((f->type   && rt->rtm_type     != f->type)   ||
		    (f->family && rt->rtm_family   != f->family) ||
		    (f->proto  && rt->rtm_protocol != f->proto)  ||
		    (f->scope  && rt->rtm_scope    != f->scope)  ||
		    (f->iif    && iif              != f->iif)    ||
		    (f->oif    && oif              != f->oif)    ||
		    (f->table  && table            != f->table)  ||
		    diff_prefix(rt->rtm_family, from, rt->rtm_src_len,
		                f->from_exact, &f->from)         ||
		    diff_prefix(rt->rtm_family, dst,  rt->rtm_dst_len,
		                f->dst_exact, &f->dst)           ||
		    diff_prefix(rt->rtm_family, gw,   bitlen,
		                false, &f->gw)                   ||
		    diff_prefix(rt->rtm_family, src,  bitlen,
		                false, &f->src))
			goto out;
	}

	if (s->callback)
		lua_pushvalue(s->L, 2);

	lua_newtable(s->L);

	L_setint(s->L, "type", rt->rtm_type);
	L_setint(s->L, "family", (rt->rtm_family == AF_INET) ? 4 : 6);

	L_setaddr(s->L, "dest", rt->rtm_family, dst, rt->rtm_dst_len);

	if (gw)
		L_setaddr(s->L, "gw", rt->rtm_family, gw, -1);

	if (from)
		L_setaddr(s->L, "from", rt->rtm_family, from, rt->rtm_src_len);

	if (iif)
		L_setdev(s->L, "iif", tb[RTA_IIF]);

	if (oif)
		L_setdev(s->L, "dev", tb[RTA_OIF]);

	L_setint(s->L, "table", table);
	L_setint(s->L, "proto", rt->rtm_protocol);
	L_setint(s->L, "scope", rt->rtm_scope);

	if (src)
		L_setaddr(s->L, "src", rt->rtm_family, src, -1);

	if (tb[RTA_PRIORITY])
		L_setint(s->L, "metric", RTA_U32(tb[RTA_PRIORITY]));

	if (rt->rtm_family == AF_INET6 && tb[RTA_CACHEINFO])
	{
		struct rta_cacheinfo *ci = RTA_DATA(tb[RTA_CACHEINFO]);

		if (ci->rta_expires)
		{
			if (ci->rta_expires)
				L_setint(s->L, "expires", ci->rta_expires / hz);

			if (ci->rta_error != 0)
				L_setint(s->L, "error", ci->rta_error);
		}
	}

	s->index++;

	if (s->callback)
		lua_call(s->L, 1, 0);
	else if (hdr->nlmsg_flags & NLM_F_MULTI)
		lua_rawseti(s->L, -2, s->index);

out:
	s->pending = !!(hdr->nlmsg_flags & NLM_F_MULTI);
	return NL_SKIP;
}

static int
cb_done(struct nl_msg *msg, void *arg)
{
	struct dump_state *s = arg;
	s->pending = 0;
	return NL_STOP;
}

static int
cb_error(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
	struct dump_state *s = arg;
	s->pending = 0;
	return NL_STOP;
}

static int _error(lua_State *L, int code, const char *msg)
{
	lua_pushnil(L);
	lua_pushnumber(L, code ? code : errno);
	lua_pushstring(L, msg ? msg : strerror(errno));

	return 3;
}

static int _route_dump(lua_State *L, struct dump_filter *filter)
{
	int flags = NLM_F_REQUEST;
	struct dump_state s = {
		.L = L,
		.pending = 1,
		.index = 0,
		.callback = lua_isfunction(L, 2),
		.filter = filter
	};

	if (!hz)
		hz = sysconf(_SC_CLK_TCK);

	if (!sock)
	{
		sock = nl_socket_alloc();
		if (!sock)
			return _error(L, -1, "Out of memory");

		if (nl_connect(sock, NETLINK_ROUTE))
			return _error(L, 0, NULL);
	}

	struct nl_msg *msg;
	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
	struct rtmsg rtm = {
		.rtm_family = filter->family,
		.rtm_dst_len = filter->dst.bits,
		.rtm_src_len = filter->src.bits
	};

	if (!filter->get)
		flags |= NLM_F_DUMP;

	msg = nlmsg_alloc_simple(RTM_GETROUTE, flags);
	if (!msg)
		goto out;

	nlmsg_append(msg, &rtm, sizeof(rtm), 0);

	if (filter->get) {
		nla_put(msg, RTA_DST, AF_BYTES(filter->dst.family),
		        &filter->dst.addr.v6);

		if (filter->src.family)
			nla_put(msg, RTA_SRC, AF_BYTES(filter->src.family),
			        &filter->src.addr.v6);
	}

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, cb_dump_route, &s);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, cb_done, &s);
	nl_cb_err(cb, NL_CB_CUSTOM, cb_error, &s);

	nl_send_auto_complete(sock, msg);

	if (!filter->get && !s.callback)
		lua_newtable(L);

	while (s.pending > 0)
		nl_recvmsgs(sock, cb);

	nlmsg_free(msg);

out:
	nl_cb_put(cb);

	if (s.callback)
		return 0;

	if (!filter->get)
		return 1;

	return (s.index > 0);
}

static int route_get(lua_State *L)
{
	struct dump_filter filter = { .get = true };
	const char *dest = luaL_checkstring(L, 1);
	const char *from = luaL_optstring(L, 2, NULL);

	if (!parse_cidr(dest, &filter.dst))
		return _error(L, -1, "Invalid destination");

	if (from && !parse_cidr(from, &filter.src))
		return _error(L, -1, "Invalid source");

	if (filter.src.family != 0 &&
	    filter.src.family != filter.dst.family)
		return _error(L, -1, "Different source/destination family");

	filter.family = filter.dst.family;

	return _route_dump(L, &filter);
}

static int route_dump(lua_State *L)
{
	const char *s;
	cidr_t p = { };
	struct dump_filter filter = { };

	if (lua_type(L, 1) == LUA_TTABLE)
	{
		filter.family = L_getint(L, 1, "family");

		if (filter.family == 4)
			filter.family = AF_INET;
		else if (filter.family == 6)
			filter.family = AF_INET6;
		else
			filter.family = 0;

		if ((s = L_getstr(L, 1, "iif")) != NULL)
			filter.iif = if_nametoindex(s);

		if ((s = L_getstr(L, 1, "oif")) != NULL)
			filter.oif = if_nametoindex(s);

		filter.type = L_getint(L, 1, "type");
		filter.scope = L_getint(L, 1, "scope");
		filter.proto = L_getint(L, 1, "proto");
		filter.table = L_getint(L, 1, "table");

		if ((s = L_getstr(L, 1, "gw")) != NULL && parse_cidr(s, &p))
			filter.gw = p;

		if ((s = L_getstr(L, 1, "from")) != NULL && parse_cidr(s, &p))
			filter.from = p;

		if ((s = L_getstr(L, 1, "src")) != NULL && parse_cidr(s, &p))
			filter.src = p;

		if ((s = L_getstr(L, 1, "dest")) != NULL && parse_cidr(s, &p))
			filter.dst = p;

		if ((s = L_getstr(L, 1, "from_exact")) != NULL && parse_cidr(s, &p))
			filter.from = p, filter.from_exact = true;

		if ((s = L_getstr(L, 1, "dest_exact")) != NULL && parse_cidr(s, &p))
			filter.dst = p, filter.dst_exact = true;
	}

	return _route_dump(L, &filter);
}


static bool diff_macaddr(struct ether_addr *mac1, struct ether_addr *mac2)
{
	struct ether_addr empty = { };

	if (!memcmp(mac2, &empty, sizeof(empty)))
		return false;

	if (!mac1 || memcmp(mac1, mac2, sizeof(empty)))
		return true;

	return false;
}

static int cb_dump_neigh(struct nl_msg *msg, void *arg)
{
	char buf[32];
	struct ether_addr *mac;
	struct in6_addr *dst;
	struct dump_state *s = arg;
	struct dump_filter *f = s->filter;
	struct nlmsghdr *hdr = nlmsg_hdr(msg);
	struct ndmsg *nd = NLMSG_DATA(hdr);
	struct nlattr *tb[NDA_MAX+1];
	int bitlen;

	if (hdr->nlmsg_type != RTM_NEWNEIGH ||
	    (nd->ndm_family != AF_INET && nd->ndm_family != AF_INET6))
		return NL_SKIP;

	nlmsg_parse(hdr, sizeof(*nd), tb, NDA_MAX, NULL);

	mac = tb[NDA_LLADDR] ? RTA_DATA(tb[NDA_LLADDR]) : NULL;
	dst = tb[NDA_DST]    ? RTA_DATA(tb[NDA_DST])    : NULL;

	bitlen = AF_BITS(nd->ndm_family);

	if ((f->family && nd->ndm_family  != f->family) ||
	    (f->iif    && nd->ndm_ifindex != f->iif) ||
		(f->type   && !(f->type & nd->ndm_state)) ||
	    diff_prefix(nd->ndm_family, dst, bitlen, false, &f->dst) ||
	    diff_macaddr(mac, &f->mac))
		goto out;

	if (s->callback)
		lua_pushvalue(s->L, 2);

	lua_newtable(s->L);

	L_setint(s->L, "family", (nd->ndm_family == AF_INET) ? 4 : 6);
	L_setstr(s->L, "dev", if_indextoname(nd->ndm_ifindex, buf));

	L_setbool(s->L, "router", (nd->ndm_flags & NTF_ROUTER));
	L_setbool(s->L, "proxy", (nd->ndm_flags & NTF_PROXY));

	L_setbool(s->L, "incomplete", (nd->ndm_state & NUD_INCOMPLETE));
	L_setbool(s->L, "reachable", (nd->ndm_state & NUD_REACHABLE));
	L_setbool(s->L, "stale", (nd->ndm_state & NUD_STALE));
	L_setbool(s->L, "delay", (nd->ndm_state & NUD_DELAY));
	L_setbool(s->L, "probe", (nd->ndm_state & NUD_PROBE));
	L_setbool(s->L, "failed", (nd->ndm_state & NUD_FAILED));
	L_setbool(s->L, "noarp", (nd->ndm_state & NUD_NOARP));
	L_setbool(s->L, "permanent", (nd->ndm_state & NUD_PERMANENT));

	if (dst)
		L_setaddr(s->L, "dest", nd->ndm_family, dst, -1);

	if (mac)
		L_setaddr(s->L, "mac", AF_PACKET, mac, -1);

	s->index++;

	if (s->callback)
		lua_call(s->L, 1, 0);
	else if (hdr->nlmsg_flags & NLM_F_MULTI)
		lua_rawseti(s->L, -2, s->index);

out:
	s->pending = !!(hdr->nlmsg_flags & NLM_F_MULTI);
	return NL_SKIP;
}

static int neighbor_dump(lua_State *L)
{
	cidr_t p = { };
	const char *s;
	struct ether_addr *mac;
	struct dump_filter filter = { .type = 0xFF & ~NUD_NOARP };
	struct dump_state st = {
		.callback = lua_isfunction(L, 2),
		.pending = 1,
		.filter = &filter,
		.L = L
	};

	if (lua_type(L, 1) == LUA_TTABLE)
	{
		filter.family = L_getint(L, 1, "family");

		if (filter.family == 4)
			filter.family = AF_INET;
		else if (filter.family == 6)
			filter.family = AF_INET6;
		else
			filter.family = 0;

		if ((s = L_getstr(L, 1, "dev")) != NULL)
			filter.iif = if_nametoindex(s);

		if ((s = L_getstr(L, 1, "dest")) != NULL && parse_cidr(s, &p))
			filter.dst = p;

		if ((s = L_getstr(L, 1, "mac")) != NULL &&
		    (mac = ether_aton(s)) != NULL)
			filter.mac = *mac;
	}

	if (!sock)
	{
		sock = nl_socket_alloc();
		if (!sock)
			return _error(L, -1, "Out of memory");

		if (nl_connect(sock, NETLINK_ROUTE))
			return _error(L, 0, NULL);
	}

	struct nl_msg *msg;
	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
	struct ndmsg ndm = {
		.ndm_family = filter.family
	};

	msg = nlmsg_alloc_simple(RTM_GETNEIGH, NLM_F_REQUEST | NLM_F_DUMP);
	if (!msg)
		goto out;

	nlmsg_append(msg, &ndm, sizeof(ndm), 0);

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, cb_dump_neigh, &st);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, cb_done, &st);
	nl_cb_err(cb, NL_CB_CUSTOM, cb_error, &st);

	nl_send_auto_complete(sock, msg);

	if (!st.callback)
		lua_newtable(L);

	while (st.pending > 0)
		nl_recvmsgs(sock, cb);

	nlmsg_free(msg);

out:
	nl_cb_put(cb);
	return (st.callback == 0);
}


static int cb_dump_link(struct nl_msg *msg, void *arg)
{
	char buf[48];
	struct dump_state *s = arg;
	struct nlmsghdr *hdr = nlmsg_hdr(msg);
	struct ifinfomsg *ifm = NLMSG_DATA(hdr);
	struct nlattr *tb[IFLA_MAX+1];
	int i, len;

	if (hdr->nlmsg_type != RTM_NEWLINK)
		return NL_SKIP;

	nlmsg_parse(hdr, sizeof(*ifm), tb, IFLA_MAX, NULL);

	L_setbool(s->L, "up", (ifm->ifi_flags & IFF_RUNNING));
	L_setint(s->L, "type", ifm->ifi_type);
	L_setstr(s->L, "name", if_indextoname(ifm->ifi_index, buf));

	if (tb[IFLA_MTU])
		L_setint(s->L, "mtu", RTA_U32(tb[IFLA_MTU]));

	if (tb[IFLA_TXQLEN])
		L_setint(s->L, "qlen", RTA_U32(tb[IFLA_TXQLEN]));

	if (tb[IFLA_MASTER])
		L_setdev(s->L, "master", tb[IFLA_MASTER]);

	if (tb[IFLA_ADDRESS] && nla_len(tb[IFLA_ADDRESS]) == AF_BYTES(AF_PACKET))
		L_setaddr(s->L, "mac", AF_PACKET, nla_get_string(tb[IFLA_ADDRESS]), -1);

	s->pending = 0;
	return NL_SKIP;
}

static int link_get(lua_State *L)
{
	const char *dev = luaL_checkstring(L, 1);
	struct dump_state st = {
		.pending = 1,
		.L = L
	};

	if (!sock)
	{
		sock = nl_socket_alloc();
		if (!sock)
			return _error(L, -1, "Out of memory");

		if (nl_connect(sock, NETLINK_ROUTE))
			return _error(L, 0, NULL);
	}

	struct nl_msg *msg = nlmsg_alloc_simple(RTM_GETLINK, NLM_F_REQUEST);
	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
	struct ifinfomsg ifm = { .ifi_index = if_nametoindex(dev) };

	if (!msg || !cb)
		return 0;

	nlmsg_append(msg, &ifm, sizeof(ifm), 0);

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, cb_dump_link, &st);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, cb_done, &st);
	nl_cb_err(cb, NL_CB_CUSTOM, cb_error, &st);

	lua_newtable(L);

	nl_send_auto_complete(sock, msg);

	while (st.pending > 0)
		nl_recvmsgs(sock, cb);

	nlmsg_free(msg);
	nl_cb_put(cb);

	return 1;
}


static const luaL_reg ip_methods[] = {
	{ "new",			cidr_new          },
	{ "IPv4",			cidr_ipv4         },
	{ "IPv6",			cidr_ipv6         },
	{ "MAC",			cidr_mac          },

	{ "checkip4",			cidr_checkip4     },
	{ "checkip6",			cidr_checkip6     },
	{ "checkmac",			cidr_checkmac     },

	{ "route",			route_get         },
	{ "routes",			route_dump        },

	{ "neighbors",		neighbor_dump     },

	{ "link",			link_get          },

	{ }
};

static const luaL_reg ip_cidr_methods[] = {
	{ "is4",			cidr_is4          },
	{ "is4rfc1918",		cidr_is4rfc1918   },
	{ "is4linklocal",	cidr_is4linklocal },
	{ "is6",			cidr_is6          },
	{ "is6linklocal",	cidr_is6linklocal },
	{ "is6mapped4",		cidr_is6mapped4   },
	{ "ismac",			cidr_ismac        },
	{ "ismaclocal",		cidr_ismaclocal   },
	{ "ismacmcast",		cidr_ismacmcast   },
	{ "lower",			cidr_lower        },
	{ "higher",			cidr_higher       },
	{ "equal",			cidr_equal        },
	{ "prefix",			cidr_prefix       },
	{ "network",		cidr_network      },
	{ "host",			cidr_host         },
	{ "mask",			cidr_mask         },
	{ "broadcast",		cidr_broadcast    },
	{ "mapped4",		cidr_mapped4      },
	{ "unscoped",		cidr_unscoped     },
	{ "tomac",			cidr_tomac        },
	{ "tolinklocal",	cidr_tolinklocal  },
	{ "contains",		cidr_contains     },
	{ "add",			cidr_add          },
	{ "sub",			cidr_sub          },
	{ "minhost",		cidr_minhost      },
	{ "maxhost",		cidr_maxhost      },
	{ "string",			cidr_tostring     },

	{ "__lt",			cidr_lower        },
	{ "__le",			cidr_lower_equal  },
	{ "__eq",			cidr_equal        },
	{ "__add",			cidr_add          },
	{ "__sub",			cidr_sub          },
	{ "__gc",			cidr_gc           },
	{ "__tostring",		cidr_tostring     },

	{ }
};

int luaopen_luci_ip(lua_State *L)
{
	luaL_register(L, LUCI_IP, ip_methods);

	luaL_newmetatable(L, LUCI_IP_CIDR);
	luaL_register(L, NULL, ip_cidr_methods);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	return 1;
}
