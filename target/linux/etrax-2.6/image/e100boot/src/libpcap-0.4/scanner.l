%{
/*
 * Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/scanner.l,v 1.1 1999/08/26 10:05:27 johana Exp $ (LBL)";
#endif

#include <sys/types.h>
#include <sys/time.h>

#include <ctype.h>
#include <unistd.h>

#include "pcap-int.h"

#include "gencode.h"
#include <pcap-namedb.h>
#include "tokdefs.h"

#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

static int stoi(char *);
static inline int xdtoi(int);

#ifdef FLEX_SCANNER
#define YY_NO_UNPUT
#undef YY_INPUT
#define YY_INPUT(buf, result, max)\
 {\
	char *src = in_buffer;\
	int i;\
\
	if (*src == 0)\
		result = YY_NULL;\
	else {\
		for (i = 0; *src && i < max; ++i)\
			buf[i] = *src++;\
		in_buffer += i;\
		result = i;\
	}\
 }
#else
#undef getc
#define getc(fp)  (*in_buffer == 0 ? EOF : *in_buffer++)
#endif

#define yylval pcap_lval
extern YYSTYPE yylval;

static char *in_buffer;

%}

N		([0-9]+|(0X|0x)[0-9A-Fa-f]+)
B		([0-9A-Fa-f][0-9A-Fa-f]?)

%a 3000

%%
dst		return DST;
src		return SRC;

link|ether|ppp|slip  return LINK;
fddi		return LINK;
arp		return ARP;
rarp		return RARP;
ip		return IP;
tcp		return TCP;
udp		return UDP;
icmp		return ICMP;
igmp		return IGMP;
igrp		return IGRP;

atalk		return ATALK;
decnet		return DECNET;
lat		return LAT;
sca		return SCA;
moprc		return MOPRC;
mopdl		return MOPDL;

host		return HOST;
net		return NET;
mask		return MASK;
port		return PORT;
proto		return PROTO;

gateway		return GATEWAY;

less		return LESS;
greater		return GREATER;
byte		return BYTE;
broadcast	return TK_BROADCAST;
multicast	return TK_MULTICAST;

and|"&&"	return AND;
or|"||"		return OR;
not		return '!';

len|length	return LEN;
inbound		return INBOUND;
outbound	return OUTBOUND;

[ \n\t]			;
[+\-*/:\[\]!<>()&|=]	return yytext[0];
">="			return GEQ;
"<="			return LEQ;
"!="			return NEQ;
"=="			return '=';
"<<"			return LSH;
">>"			return RSH;
{N}			{ yylval.i = stoi((char *)yytext); return NUM; }
({N}\.{N})|({N}\.{N}\.{N})|({N}\.{N}\.{N}\.{N})	{
			yylval.s = sdup((char *)yytext); return HID; }
{B}:{B}:{B}:{B}:{B}:{B} { yylval.e = pcap_ether_aton((char *)yytext);
			  return EID; }
{B}:+({B}:+)+		{ bpf_error("bogus ethernet address %s", yytext); }
[A-Za-z0-9][-_.A-Za-z0-9]*[.A-Za-z0-9] {
			 yylval.s = sdup((char *)yytext); return ID; }
"\\"[^ !()\n\t]+	{ yylval.s = sdup((char *)yytext + 1); return ID; }
[^ \[\]\t\n\-_.A-Za-z0-9!<>()&|=]+i {
			bpf_error("illegal token: %s\n", yytext); }
.			{ bpf_error("illegal char '%c'", *yytext); }
%%
void
lex_init(buf)
	char *buf;
{
	in_buffer = buf;
}

/*
 * Also define a yywrap.  Note that if we're using flex, it will
 * define a macro to map this identifier to pcap_wrap.
 */
int
yywrap()
{
	return 1;
}

/* Hex digit to integer. */
static inline int
xdtoi(c)
	register int c;
{
	if (isdigit(c))
		return c - '0';
	else if (islower(c))
		return c - 'a' + 10;
	else
		return c - 'A' + 10;
}

/*
 * Convert string to integer.  Just like atoi(), but checks for
 * preceding 0x or 0 and uses hex or octal instead of decimal.
 */
static int
stoi(s)
	char *s;
{
	int base = 10;
	int n = 0;

	if (*s == '0') {
		if (s[1] == 'x' || s[1] == 'X') {
			s += 2;
			base = 16;
		}
		else {
			base = 8;
			s += 1;
		}
	}
	while (*s)
		n = n * base + xdtoi(*s++);

	return n;
}

