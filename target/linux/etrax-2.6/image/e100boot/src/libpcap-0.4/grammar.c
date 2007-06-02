/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse pcap_parse
#define yylex   pcap_lex
#define yyerror pcap_error
#define yylval  pcap_lval
#define yychar  pcap_char
#define yydebug pcap_debug
#define yynerrs pcap_nerrs


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




/* Copy the first part of user declarations.  */
#line 1 "grammar.y"

/*
 * Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996
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
 *
 */
#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/grammar.y,v 1.1 1999/08/26 10:05:22 johana Exp $ (LBL)";
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#if __STDC__
struct mbuf;
struct rtentry;
#endif

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <stdio.h>

#include "pcap-int.h"

#include "gencode.h"
#include <pcap-namedb.h>

#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#define QSET(q, p, d, a) (q).proto = (p),\
			 (q).dir = (d),\
			 (q).addr = (a)

int n_errors = 0;

static struct qual qerr = { Q_UNDEF, Q_UNDEF, Q_UNDEF, Q_UNDEF };

static void
yyerror(char *msg)
{
	++n_errors;
	bpf_error("%s", msg);
	/* NOTREACHED */
}

#ifndef YYBISON
int yyparse(void);

int
pcap_parse()
{
	return (yyparse());
}
#endif



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 187 of yacc.c.  */
#line 289 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 302 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   345

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  102
/* YYNRULES -- Number of states.  */
#define YYNSTATES  151

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   300

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,     2,     2,     2,     2,    47,     2,
      54,    53,    50,    48,     2,    49,     2,    51,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    60,     2,
      57,    56,    55,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    58,     2,    59,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    46,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    15,    19,    23,
      27,    29,    31,    33,    35,    39,    41,    45,    49,    51,
      53,    56,    58,    60,    62,    66,    70,    72,    74,    76,
      79,    83,    86,    89,    92,    95,    98,   102,   104,   108,
     112,   114,   116,   117,   119,   121,   125,   129,   133,   137,
     139,   141,   143,   145,   147,   149,   151,   153,   155,   157,
     159,   161,   163,   165,   167,   169,   171,   173,   175,   178,
     181,   184,   187,   192,   194,   196,   198,   200,   202,   204,
     206,   208,   210,   212,   217,   224,   228,   232,   236,   240,
     244,   248,   252,   256,   259,   263,   265,   267,   269,   271,
     273,   275,   277
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      62,     0,    -1,    63,    64,    -1,    63,    -1,    -1,    73,
      -1,    64,    65,    73,    -1,    64,    65,    67,    -1,    64,
      66,    73,    -1,    64,    66,    67,    -1,    43,    -1,    44,
      -1,    68,    -1,    87,    -1,    70,    71,    53,    -1,    37,
      -1,    39,    51,    30,    -1,    39,     8,    39,    -1,    39,
      -1,    38,    -1,    69,    67,    -1,    45,    -1,    54,    -1,
      68,    -1,    72,    65,    67,    -1,    72,    66,    67,    -1,
      87,    -1,    71,    -1,    75,    -1,    69,    73,    -1,    76,
      77,    78,    -1,    76,    77,    -1,    76,    78,    -1,    76,
      12,    -1,    76,    79,    -1,    74,    67,    -1,    70,    64,
      53,    -1,    80,    -1,    84,    82,    84,    -1,    84,    83,
      84,    -1,    81,    -1,    80,    -1,    -1,     4,    -1,     3,
      -1,     4,    44,     3,    -1,     3,    44,     4,    -1,     4,
      43,     3,    -1,     3,    43,     4,    -1,     5,    -1,     7,
      -1,     9,    -1,     6,    -1,    33,    -1,    16,    -1,    14,
      -1,    15,    -1,    17,    -1,    18,    -1,    19,    -1,    20,
      -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,    25,
      -1,    27,    -1,    26,    -1,    76,    28,    -1,    76,    29,
      -1,    10,    30,    -1,    11,    30,    -1,    13,    30,    86,
      30,    -1,    31,    -1,    32,    -1,    55,    -1,    34,    -1,
      56,    -1,    35,    -1,    57,    -1,    36,    -1,    87,    -1,
      85,    -1,    80,    58,    84,    59,    -1,    80,    58,    84,
      60,    30,    59,    -1,    84,    48,    84,    -1,    84,    49,
      84,    -1,    84,    50,    84,    -1,    84,    51,    84,    -1,
      84,    47,    84,    -1,    84,    46,    84,    -1,    84,    40,
      84,    -1,    84,    41,    84,    -1,    49,    84,    -1,    70,
      85,    53,    -1,    42,    -1,    47,    -1,    46,    -1,    57,
      -1,    55,    -1,    56,    -1,    30,    -1,    70,    87,    53,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   130,   130,   134,   136,   138,   139,   140,   141,   142,
     144,   146,   148,   149,   151,   153,   154,   156,   158,   170,
     171,   173,   175,   177,   178,   179,   181,   183,   185,   186,
     188,   189,   190,   191,   192,   194,   195,   196,   197,   199,
     201,   204,   205,   208,   209,   210,   211,   212,   213,   216,
     217,   218,   221,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   239,   240,
     241,   242,   243,   244,   245,   247,   248,   249,   251,   252,
     253,   255,   256,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   272,   273,   274,   275,
     276,   278,   279
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DST", "SRC", "HOST", "GATEWAY", "NET",
  "MASK", "PORT", "LESS", "GREATER", "PROTO", "BYTE", "ARP", "RARP", "IP",
  "TCP", "UDP", "ICMP", "IGMP", "IGRP", "ATALK", "DECNET", "LAT", "SCA",
  "MOPRC", "MOPDL", "TK_BROADCAST", "TK_MULTICAST", "NUM", "INBOUND",
  "OUTBOUND", "LINK", "GEQ", "LEQ", "NEQ", "ID", "EID", "HID", "LSH",
  "RSH", "LEN", "AND", "OR", "'!'", "'|'", "'&'", "'+'", "'-'", "'*'",
  "'/'", "UMINUS", "')'", "'('", "'>'", "'='", "'<'", "'['", "']'", "':'",
  "$accept", "prog", "null", "expr", "and", "or", "id", "nid", "not",
  "paren", "pid", "qid", "term", "head", "rterm", "pqual", "dqual",
  "aqual", "ndaqual", "pname", "other", "relop", "irelop", "arth", "narth",
  "byteop", "pnum", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,    33,   124,    38,    43,    45,
      42,    47,   300,    41,    40,    62,    61,    60,    91,    93,
      58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    61,    62,    62,    63,    64,    64,    64,    64,    64,
      65,    66,    67,    67,    67,    68,    68,    68,    68,    68,
      68,    69,    70,    71,    71,    71,    72,    72,    73,    73,
      74,    74,    74,    74,    74,    75,    75,    75,    75,    75,
      75,    76,    76,    77,    77,    77,    77,    77,    77,    78,
      78,    78,    79,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    81,    81,
      81,    81,    81,    81,    81,    82,    82,    82,    83,    83,
      83,    84,    84,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    86,    86,    86,    86,
      86,    87,    87
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     1,     3,     3,     3,     3,
       1,     1,     1,     1,     3,     1,     3,     3,     1,     1,
       2,     1,     1,     1,     3,     3,     1,     1,     1,     2,
       3,     2,     2,     2,     2,     2,     3,     1,     3,     3,
       1,     1,     0,     1,     1,     3,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     6,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,    42,     1,     0,     0,     0,    55,    56,    54,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    67,
      66,   101,    73,    74,    53,    95,    21,     0,    22,     2,
      42,    42,     5,     0,    28,     0,    41,    40,     0,    82,
      81,    70,    71,     0,     0,     0,    93,    10,    11,    42,
      42,    29,     0,    82,    81,    15,    19,    18,    35,    12,
       0,     0,    13,    44,    43,    49,    52,    50,    51,    33,
      68,    69,    31,    32,    34,     0,    76,    78,    80,     0,
       0,     0,     0,     0,     0,     0,     0,    75,    77,    79,
       0,     0,    97,    96,    99,   100,    98,     0,     0,     7,
      42,    42,     6,    81,     9,     8,    36,    94,   102,     0,
       0,    20,    23,     0,    27,     0,    26,     0,     0,     0,
       0,    30,     0,    91,    92,    90,    89,    85,    86,    87,
      88,    38,    39,    72,    81,    17,    16,     0,    14,     0,
       0,    48,    46,    47,    45,    83,     0,    24,    25,     0,
      84
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    52,    49,    50,   111,    59,    60,    44,
     114,   115,    32,    33,    34,    35,    72,    73,    74,    45,
      37,    90,    91,    38,    39,    97,    40
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -53
static const yytype_int16 yypact[] =
{
     -53,    10,   129,   -53,   -25,   -18,     0,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   254,   -53,    -1,
     213,   213,   -53,    50,   -53,   285,     8,   -53,   270,   -53,
     -53,   -53,   -53,   288,   254,   -33,   -53,   -53,   -53,   171,
     171,   -53,    -3,   -26,   -21,   -53,   -53,    -5,   -53,   -53,
      50,    50,   -53,    13,    53,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   210,   -53,   -53,   254,   -53,   -53,   -53,   254,
     254,   254,   254,   254,   254,   254,   254,   -53,   -53,   -53,
     254,   254,   -53,   -53,   -53,   -53,   -53,     9,   201,   -53,
     171,   171,   -53,    26,   -53,   -53,   -53,   -53,   -53,    21,
      23,   -53,   -53,   -17,   -15,    -1,   -21,    58,    61,    64,
      69,   -53,   117,    27,    27,    43,   282,   -36,   -36,   -53,
     -53,   201,   201,   -53,    57,   -53,   -53,   -21,   -53,    50,
      50,   -53,   -53,   -53,   -53,   -53,    52,   -53,   -53,    44,
     -53
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -53,   -53,   -53,   100,    -8,     2,   -31,   -52,     5,    -2,
     -53,   -53,   -29,   -53,   -53,   -53,   -53,    46,   -53,    14,
     -53,   -53,   -53,    41,   -20,   -53,   -27
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -38
static const yytype_int16 yytable[] =
{
      31,    51,    58,   109,    54,    41,    62,    30,   -37,   112,
       3,    53,    42,    21,    85,    86,    36,    54,    99,   104,
     102,   105,   103,   103,    53,    75,   -13,   107,    31,    31,
      43,    61,   108,    62,   116,    30,    30,    28,   138,   133,
      47,    48,    47,    48,    36,    36,   110,   101,   101,   112,
     106,   -37,   -37,   136,   100,   100,   117,   118,    61,   113,
     135,   -37,   141,    36,    36,   142,    75,   143,    46,   -13,
     -13,    51,   144,   103,   134,    83,    84,    85,    86,   -13,
      21,    53,   149,    79,    80,    98,   137,    55,    56,    57,
      82,    83,    84,    85,    86,    26,   119,   120,   101,    31,
     -26,   -26,    29,   150,    28,   100,   100,   139,   147,   148,
     108,   113,    62,    62,    36,    36,   122,   140,   121,     0,
     123,   124,   125,   126,   127,   128,   129,   130,     0,    -3,
       0,   131,   132,     0,     0,     0,     0,    61,    61,     4,
       5,     0,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    79,    80,    21,
      22,    23,    24,    81,    82,    83,    84,    85,    86,     0,
       0,    25,     0,     0,    26,     0,   145,   146,    27,     0,
       0,     4,     5,    28,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,     0,
       0,    21,    22,    23,    24,     0,     0,     0,    55,    56,
      57,     0,     0,    25,     0,    65,    26,    67,     0,    68,
      27,     0,     0,     4,     5,    28,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    79,    80,    21,    22,    23,    24,    81,    82,    83,
      84,    85,    86,     0,     0,    25,     0,     0,    26,     0,
       0,     0,    27,     0,     0,     0,     0,    28,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,     0,     0,    21,     0,     0,    24,    63,    64,
      65,    66,    67,     0,    68,     0,    25,    69,     0,     0,
       0,     0,     0,    27,    76,    77,    78,     0,    28,     0,
      79,    80,     0,    70,    71,     0,    81,    82,    83,    84,
      85,    86,    79,    80,     0,    87,    88,    89,     0,     0,
      83,    84,    85,    86,    92,    93,     0,     0,     0,     0,
       0,     0,     0,    94,    95,    96
};

static const yytype_int16 yycheck[] =
{
       2,    30,    33,     8,    31,    30,    33,     2,     0,    61,
       0,    31,    30,    30,    50,    51,     2,    44,    49,    50,
      49,    50,    49,    50,    44,    58,     0,    53,    30,    31,
      30,    33,    53,    60,    61,    30,    31,    54,    53,    30,
      43,    44,    43,    44,    30,    31,    51,    49,    50,   101,
      53,    43,    44,    30,    49,    50,    43,    44,    60,    61,
      39,    53,     4,    49,    50,     4,    58,     3,    27,    43,
      44,   100,     3,   100,   101,    48,    49,    50,    51,    53,
      30,   101,    30,    40,    41,    44,   113,    37,    38,    39,
      47,    48,    49,    50,    51,    45,    43,    44,   100,   101,
      43,    44,     2,    59,    54,   100,   101,   115,   139,   140,
      53,   113,   139,   140,   100,   101,    75,   115,    72,    -1,
      79,    80,    81,    82,    83,    84,    85,    86,    -1,     0,
      -1,    90,    91,    -1,    -1,    -1,    -1,   139,   140,    10,
      11,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    40,    41,    30,
      31,    32,    33,    46,    47,    48,    49,    50,    51,    -1,
      -1,    42,    -1,    -1,    45,    -1,    59,    60,    49,    -1,
      -1,    10,    11,    54,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    -1,
      -1,    30,    31,    32,    33,    -1,    -1,    -1,    37,    38,
      39,    -1,    -1,    42,    -1,     5,    45,     7,    -1,     9,
      49,    -1,    -1,    10,    11,    54,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    40,    41,    30,    31,    32,    33,    46,    47,    48,
      49,    50,    51,    -1,    -1,    42,    -1,    -1,    45,    -1,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    54,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    -1,    -1,    30,    -1,    -1,    33,     3,     4,
       5,     6,     7,    -1,     9,    -1,    42,    12,    -1,    -1,
      -1,    -1,    -1,    49,    34,    35,    36,    -1,    54,    -1,
      40,    41,    -1,    28,    29,    -1,    46,    47,    48,    49,
      50,    51,    40,    41,    -1,    55,    56,    57,    -1,    -1,
      48,    49,    50,    51,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    62,    63,     0,    10,    11,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    30,    31,    32,    33,    42,    45,    49,    54,    64,
      69,    70,    73,    74,    75,    76,    80,    81,    84,    85,
      87,    30,    30,    30,    70,    80,    84,    43,    44,    65,
      66,    73,    64,    85,    87,    37,    38,    39,    67,    68,
      69,    70,    87,     3,     4,     5,     6,     7,     9,    12,
      28,    29,    77,    78,    79,    58,    34,    35,    36,    40,
      41,    46,    47,    48,    49,    50,    51,    55,    56,    57,
      82,    83,    46,    47,    55,    56,    57,    86,    84,    67,
      69,    70,    73,    87,    67,    73,    53,    53,    53,     8,
      51,    67,    68,    70,    71,    72,    87,    43,    44,    43,
      44,    78,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    30,    87,    39,    30,    87,    53,    65,
      66,     4,     4,     3,     3,    59,    60,    67,    67,    30,
      59
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 131 "grammar.y"
    {
	finish_parse((yyvsp[(2) - (2)].blk).b);
}
    break;

  case 4:
#line 136 "grammar.y"
    { (yyval.blk).q = qerr; }
    break;

  case 6:
#line 139 "grammar.y"
    { gen_and((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 7:
#line 140 "grammar.y"
    { gen_and((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 8:
#line 141 "grammar.y"
    { gen_or((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 9:
#line 142 "grammar.y"
    { gen_or((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 10:
#line 144 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 11:
#line 146 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 13:
#line 149 "grammar.y"
    { (yyval.blk).b = gen_ncode(NULL, (bpf_u_int32)(yyvsp[(1) - (1)].i),
						   (yyval.blk).q = (yyvsp[(0) - (1)].blk).q); }
    break;

  case 14:
#line 151 "grammar.y"
    { (yyval.blk) = (yyvsp[(2) - (3)].blk); }
    break;

  case 15:
#line 153 "grammar.y"
    { (yyval.blk).b = gen_scode((yyvsp[(1) - (1)].s), (yyval.blk).q = (yyvsp[(0) - (1)].blk).q); }
    break;

  case 16:
#line 154 "grammar.y"
    { (yyval.blk).b = gen_mcode((yyvsp[(1) - (3)].s), NULL, (yyvsp[(3) - (3)].i),
				    (yyval.blk).q = (yyvsp[(0) - (3)].blk).q); }
    break;

  case 17:
#line 156 "grammar.y"
    { (yyval.blk).b = gen_mcode((yyvsp[(1) - (3)].s), (yyvsp[(3) - (3)].s), 0,
				    (yyval.blk).q = (yyvsp[(0) - (3)].blk).q); }
    break;

  case 18:
#line 158 "grammar.y"
    {
				  /* Decide how to parse HID based on proto */
				  (yyval.blk).q = (yyvsp[(0) - (1)].blk).q;
				  switch ((yyval.blk).q.proto) {
				  case Q_DECNET:
					(yyval.blk).b = gen_ncode((yyvsp[(1) - (1)].s), 0, (yyval.blk).q);
					break;
				  default:
					(yyval.blk).b = gen_ncode((yyvsp[(1) - (1)].s), 0, (yyval.blk).q);
					break;
				  }
				}
    break;

  case 19:
#line 170 "grammar.y"
    { (yyval.blk).b = gen_ecode((yyvsp[(1) - (1)].e), (yyval.blk).q = (yyvsp[(0) - (1)].blk).q); }
    break;

  case 20:
#line 171 "grammar.y"
    { gen_not((yyvsp[(2) - (2)].blk).b); (yyval.blk) = (yyvsp[(2) - (2)].blk); }
    break;

  case 21:
#line 173 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 22:
#line 175 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 24:
#line 178 "grammar.y"
    { gen_and((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 25:
#line 179 "grammar.y"
    { gen_or((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 26:
#line 181 "grammar.y"
    { (yyval.blk).b = gen_ncode(NULL, (bpf_u_int32)(yyvsp[(1) - (1)].i),
						   (yyval.blk).q = (yyvsp[(0) - (1)].blk).q); }
    break;

  case 29:
#line 186 "grammar.y"
    { gen_not((yyvsp[(2) - (2)].blk).b); (yyval.blk) = (yyvsp[(2) - (2)].blk); }
    break;

  case 30:
#line 188 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (3)].i), (yyvsp[(2) - (3)].i), (yyvsp[(3) - (3)].i)); }
    break;

  case 31:
#line 189 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].i), Q_DEFAULT); }
    break;

  case 32:
#line 190 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, (yyvsp[(2) - (2)].i)); }
    break;

  case 33:
#line 191 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, Q_PROTO); }
    break;

  case 34:
#line 192 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, (yyvsp[(2) - (2)].i)); }
    break;

  case 35:
#line 194 "grammar.y"
    { (yyval.blk) = (yyvsp[(2) - (2)].blk); }
    break;

  case 36:
#line 195 "grammar.y"
    { (yyval.blk).b = (yyvsp[(2) - (3)].blk).b; (yyval.blk).q = (yyvsp[(1) - (3)].blk).q; }
    break;

  case 37:
#line 196 "grammar.y"
    { (yyval.blk).b = gen_proto_abbrev((yyvsp[(1) - (1)].i)); (yyval.blk).q = qerr; }
    break;

  case 38:
#line 197 "grammar.y"
    { (yyval.blk).b = gen_relation((yyvsp[(2) - (3)].i), (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a), 0);
				  (yyval.blk).q = qerr; }
    break;

  case 39:
#line 199 "grammar.y"
    { (yyval.blk).b = gen_relation((yyvsp[(2) - (3)].i), (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a), 1);
				  (yyval.blk).q = qerr; }
    break;

  case 40:
#line 201 "grammar.y"
    { (yyval.blk).b = (yyvsp[(1) - (1)].rblk); (yyval.blk).q = qerr; }
    break;

  case 42:
#line 205 "grammar.y"
    { (yyval.i) = Q_DEFAULT; }
    break;

  case 43:
#line 208 "grammar.y"
    { (yyval.i) = Q_SRC; }
    break;

  case 44:
#line 209 "grammar.y"
    { (yyval.i) = Q_DST; }
    break;

  case 45:
#line 210 "grammar.y"
    { (yyval.i) = Q_OR; }
    break;

  case 46:
#line 211 "grammar.y"
    { (yyval.i) = Q_OR; }
    break;

  case 47:
#line 212 "grammar.y"
    { (yyval.i) = Q_AND; }
    break;

  case 48:
#line 213 "grammar.y"
    { (yyval.i) = Q_AND; }
    break;

  case 49:
#line 216 "grammar.y"
    { (yyval.i) = Q_HOST; }
    break;

  case 50:
#line 217 "grammar.y"
    { (yyval.i) = Q_NET; }
    break;

  case 51:
#line 218 "grammar.y"
    { (yyval.i) = Q_PORT; }
    break;

  case 52:
#line 221 "grammar.y"
    { (yyval.i) = Q_GATEWAY; }
    break;

  case 53:
#line 223 "grammar.y"
    { (yyval.i) = Q_LINK; }
    break;

  case 54:
#line 224 "grammar.y"
    { (yyval.i) = Q_IP; }
    break;

  case 55:
#line 225 "grammar.y"
    { (yyval.i) = Q_ARP; }
    break;

  case 56:
#line 226 "grammar.y"
    { (yyval.i) = Q_RARP; }
    break;

  case 57:
#line 227 "grammar.y"
    { (yyval.i) = Q_TCP; }
    break;

  case 58:
#line 228 "grammar.y"
    { (yyval.i) = Q_UDP; }
    break;

  case 59:
#line 229 "grammar.y"
    { (yyval.i) = Q_ICMP; }
    break;

  case 60:
#line 230 "grammar.y"
    { (yyval.i) = Q_IGMP; }
    break;

  case 61:
#line 231 "grammar.y"
    { (yyval.i) = Q_IGRP; }
    break;

  case 62:
#line 232 "grammar.y"
    { (yyval.i) = Q_ATALK; }
    break;

  case 63:
#line 233 "grammar.y"
    { (yyval.i) = Q_DECNET; }
    break;

  case 64:
#line 234 "grammar.y"
    { (yyval.i) = Q_LAT; }
    break;

  case 65:
#line 235 "grammar.y"
    { (yyval.i) = Q_SCA; }
    break;

  case 66:
#line 236 "grammar.y"
    { (yyval.i) = Q_MOPDL; }
    break;

  case 67:
#line 237 "grammar.y"
    { (yyval.i) = Q_MOPRC; }
    break;

  case 68:
#line 239 "grammar.y"
    { (yyval.rblk) = gen_broadcast((yyvsp[(1) - (2)].i)); }
    break;

  case 69:
#line 240 "grammar.y"
    { (yyval.rblk) = gen_multicast((yyvsp[(1) - (2)].i)); }
    break;

  case 70:
#line 241 "grammar.y"
    { (yyval.rblk) = gen_less((yyvsp[(2) - (2)].i)); }
    break;

  case 71:
#line 242 "grammar.y"
    { (yyval.rblk) = gen_greater((yyvsp[(2) - (2)].i)); }
    break;

  case 72:
#line 243 "grammar.y"
    { (yyval.rblk) = gen_byteop((yyvsp[(3) - (4)].i), (yyvsp[(2) - (4)].i), (yyvsp[(4) - (4)].i)); }
    break;

  case 73:
#line 244 "grammar.y"
    { (yyval.rblk) = gen_inbound(0); }
    break;

  case 74:
#line 245 "grammar.y"
    { (yyval.rblk) = gen_inbound(1); }
    break;

  case 75:
#line 247 "grammar.y"
    { (yyval.i) = BPF_JGT; }
    break;

  case 76:
#line 248 "grammar.y"
    { (yyval.i) = BPF_JGE; }
    break;

  case 77:
#line 249 "grammar.y"
    { (yyval.i) = BPF_JEQ; }
    break;

  case 78:
#line 251 "grammar.y"
    { (yyval.i) = BPF_JGT; }
    break;

  case 79:
#line 252 "grammar.y"
    { (yyval.i) = BPF_JGE; }
    break;

  case 80:
#line 253 "grammar.y"
    { (yyval.i) = BPF_JEQ; }
    break;

  case 81:
#line 255 "grammar.y"
    { (yyval.a) = gen_loadi((yyvsp[(1) - (1)].i)); }
    break;

  case 83:
#line 258 "grammar.y"
    { (yyval.a) = gen_load((yyvsp[(1) - (4)].i), (yyvsp[(3) - (4)].a), 1); }
    break;

  case 84:
#line 259 "grammar.y"
    { (yyval.a) = gen_load((yyvsp[(1) - (6)].i), (yyvsp[(3) - (6)].a), (yyvsp[(5) - (6)].i)); }
    break;

  case 85:
#line 260 "grammar.y"
    { (yyval.a) = gen_arth(BPF_ADD, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 86:
#line 261 "grammar.y"
    { (yyval.a) = gen_arth(BPF_SUB, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 87:
#line 262 "grammar.y"
    { (yyval.a) = gen_arth(BPF_MUL, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 88:
#line 263 "grammar.y"
    { (yyval.a) = gen_arth(BPF_DIV, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 89:
#line 264 "grammar.y"
    { (yyval.a) = gen_arth(BPF_AND, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 90:
#line 265 "grammar.y"
    { (yyval.a) = gen_arth(BPF_OR, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 91:
#line 266 "grammar.y"
    { (yyval.a) = gen_arth(BPF_LSH, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 92:
#line 267 "grammar.y"
    { (yyval.a) = gen_arth(BPF_RSH, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 93:
#line 268 "grammar.y"
    { (yyval.a) = gen_neg((yyvsp[(2) - (2)].a)); }
    break;

  case 94:
#line 269 "grammar.y"
    { (yyval.a) = (yyvsp[(2) - (3)].a); }
    break;

  case 95:
#line 270 "grammar.y"
    { (yyval.a) = gen_loadlen(); }
    break;

  case 96:
#line 272 "grammar.y"
    { (yyval.i) = '&'; }
    break;

  case 97:
#line 273 "grammar.y"
    { (yyval.i) = '|'; }
    break;

  case 98:
#line 274 "grammar.y"
    { (yyval.i) = '<'; }
    break;

  case 99:
#line 275 "grammar.y"
    { (yyval.i) = '>'; }
    break;

  case 100:
#line 276 "grammar.y"
    { (yyval.i) = '='; }
    break;

  case 102:
#line 279 "grammar.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;


/* Line 1267 of yacc.c.  */
#line 2165 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 281 "grammar.y"


