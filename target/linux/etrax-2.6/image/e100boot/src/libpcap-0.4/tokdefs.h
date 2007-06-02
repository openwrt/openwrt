/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DST = 258,
     SRC = 259,
     HOST = 260,
     GATEWAY = 261,
     NET = 262,
     MASK = 263,
     PORT = 264,
     LESS = 265,
     GREATER = 266,
     PROTO = 267,
     BYTE = 268,
     ARP = 269,
     RARP = 270,
     IP = 271,
     TCP = 272,
     UDP = 273,
     ICMP = 274,
     IGMP = 275,
     IGRP = 276,
     ATALK = 277,
     DECNET = 278,
     LAT = 279,
     SCA = 280,
     MOPRC = 281,
     MOPDL = 282,
     TK_BROADCAST = 283,
     TK_MULTICAST = 284,
     NUM = 285,
     INBOUND = 286,
     OUTBOUND = 287,
     LINK = 288,
     GEQ = 289,
     LEQ = 290,
     NEQ = 291,
     ID = 292,
     EID = 293,
     HID = 294,
     LSH = 295,
     RSH = 296,
     LEN = 297,
     AND = 298,
     OR = 299,
     UMINUS = 300
   };
#endif
/* Tokens.  */
#define DST 258
#define SRC 259
#define HOST 260
#define GATEWAY 261
#define NET 262
#define MASK 263
#define PORT 264
#define LESS 265
#define GREATER 266
#define PROTO 267
#define BYTE 268
#define ARP 269
#define RARP 270
#define IP 271
#define TCP 272
#define UDP 273
#define ICMP 274
#define IGMP 275
#define IGRP 276
#define ATALK 277
#define DECNET 278
#define LAT 279
#define SCA 280
#define MOPRC 281
#define MOPDL 282
#define TK_BROADCAST 283
#define TK_MULTICAST 284
#define NUM 285
#define INBOUND 286
#define OUTBOUND 287
#define LINK 288
#define GEQ 289
#define LEQ 290
#define NEQ 291
#define ID 292
#define EID 293
#define HID 294
#define LSH 295
#define RSH 296
#define LEN 297
#define AND 298
#define OR 299
#define UMINUS 300




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 82 "grammar.y"
{
	int i;
	bpf_u_int32 h;
	u_char *e;
	char *s;
	struct stmt *stmt;
	struct arth *a;
	struct {
		struct qual q;
		struct block *b;
	} blk;
	struct block *rblk;
}
/* Line 1489 of yacc.c.  */
#line 153 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE pcap_lval;

