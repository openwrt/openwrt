/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse zconfparse
#define yylex   zconflex
#define yyerror zconferror
#define yylval  zconflval
#define yychar  zconfchar
#define yydebug zconfdebug
#define yynerrs zconfnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_MAINMENU = 258,
     T_MENU = 259,
     T_ENDMENU = 260,
     T_SOURCE = 261,
     T_CHOICE = 262,
     T_ENDCHOICE = 263,
     T_COMMENT = 264,
     T_CONFIG = 265,
     T_MENUCONFIG = 266,
     T_HELP = 267,
     T_HELPTEXT = 268,
     T_IF = 269,
     T_ENDIF = 270,
     T_DEPENDS = 271,
     T_REQUIRES = 272,
     T_OPTIONAL = 273,
     T_PROMPT = 274,
     T_TYPE = 275,
     T_DEFAULT = 276,
     T_DESELECT = 277,
     T_SELECT = 278,
     T_RANGE = 279,
     T_ON = 280,
     T_RESET = 281,
     T_WORD = 282,
     T_WORD_QUOTE = 283,
     T_UNEQUAL = 284,
     T_CLOSE_PAREN = 285,
     T_OPEN_PAREN = 286,
     T_EOL = 287,
     T_OR = 288,
     T_AND = 289,
     T_EQUAL = 290,
     T_NOT = 291
   };
#endif
#define T_MAINMENU 258
#define T_MENU 259
#define T_ENDMENU 260
#define T_SOURCE 261
#define T_CHOICE 262
#define T_ENDCHOICE 263
#define T_COMMENT 264
#define T_CONFIG 265
#define T_MENUCONFIG 266
#define T_HELP 267
#define T_HELPTEXT 268
#define T_IF 269
#define T_ENDIF 270
#define T_DEPENDS 271
#define T_REQUIRES 272
#define T_OPTIONAL 273
#define T_PROMPT 274
#define T_TYPE 275
#define T_DEFAULT 276
#define T_DESELECT 277
#define T_SELECT 278
#define T_RANGE 279
#define T_ON 280
#define T_RESET 281
#define T_WORD 282
#define T_WORD_QUOTE 283
#define T_UNEQUAL 284
#define T_CLOSE_PAREN 285
#define T_OPEN_PAREN 286
#define T_EOL 287
#define T_OR 288
#define T_AND 289
#define T_EQUAL 290
#define T_NOT 291




/* Copy the first part of user declarations.  */


/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LKC_DIRECT_LINK
#include "lkc.h"

#include "zconf.hash.c"

#define printd(mask, fmt...) if (cdebug & (mask)) printf(fmt)

#define PRINTD		0x0001
#define DEBUG_PARSE	0x0002

int cdebug = PRINTD;

extern int zconflex(void);
static void zconfprint(const char *err, ...);
static void zconf_error(const char *err, ...);
static void zconferror(const char *err);
static bool zconf_endtoken(struct kconf_id *id, int starttoken, int endtoken);

struct symbol *symbol_hash[SYMBOL_HASHSIZE];

static struct menu *current_menu, *current_entry;

#define YYDEBUG 0
#if YYDEBUG
#define YYERROR_VERBOSE
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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
	char *string;
	struct file *file;
	struct symbol *symbol;
	struct expr *expr;
	struct menu *menu;
	struct kconf_id *id;
} YYSTYPE;
/* Line 191 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */


#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
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
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   285

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  37
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  42
/* YYNRULES -- Number of rules. */
#define YYNRULES  108
/* YYNRULES -- Number of states. */
#define YYNSTATES  184

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   291

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      35,    36
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    12,    15,    20,    23,
      28,    33,    37,    39,    41,    43,    45,    47,    49,    51,
      53,    55,    57,    59,    61,    63,    65,    67,    71,    74,
      78,    81,    85,    88,    89,    92,    95,    98,   101,   104,
     108,   113,   118,   123,   128,   134,   137,   140,   142,   146,
     147,   150,   153,   156,   159,   162,   167,   171,   174,   178,
     183,   184,   187,   191,   193,   197,   198,   201,   204,   207,
     211,   214,   216,   220,   221,   224,   227,   230,   234,   238,
     241,   244,   247,   248,   251,   254,   257,   262,   266,   270,
     271,   274,   276,   278,   281,   284,   287,   289,   292,   293,
     296,   298,   302,   306,   310,   313,   317,   321,   323
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      38,     0,    -1,    39,    -1,    -1,    39,    41,    -1,    39,
      52,    -1,    39,    63,    -1,    39,     3,    73,    75,    -1,
      39,    74,    -1,    39,    27,     1,    32,    -1,    39,    40,
       1,    32,    -1,    39,     1,    32,    -1,    16,    -1,    19,
      -1,    20,    -1,    22,    -1,    23,    -1,    18,    -1,    24,
      -1,    21,    -1,    26,    -1,    32,    -1,    58,    -1,    67,
      -1,    44,    -1,    46,    -1,    65,    -1,    27,     1,    32,
      -1,     1,    32,    -1,    10,    27,    32,    -1,    43,    47,
      -1,    11,    27,    32,    -1,    45,    47,    -1,    -1,    47,
      48,    -1,    47,    71,    -1,    47,    69,    -1,    47,    42,
      -1,    47,    32,    -1,    20,    72,    32,    -1,    19,    73,
      76,    32,    -1,    21,    77,    76,    32,    -1,    22,    27,
      76,    32,    -1,    23,    27,    76,    32,    -1,    24,    78,
      78,    76,    32,    -1,     7,    32,    -1,    49,    53,    -1,
      74,    -1,    50,    55,    51,    -1,    -1,    53,    54,    -1,
      53,    71,    -1,    53,    69,    -1,    53,    32,    -1,    53,
      42,    -1,    19,    73,    76,    32,    -1,    20,    72,    32,
      -1,    18,    32,    -1,    26,    76,    32,    -1,    21,    27,
      76,    32,    -1,    -1,    55,    41,    -1,    14,    77,    75,
      -1,    74,    -1,    56,    59,    57,    -1,    -1,    59,    41,
      -1,    59,    63,    -1,    59,    52,    -1,     4,    73,    32,
      -1,    60,    70,    -1,    74,    -1,    61,    64,    62,    -1,
      -1,    64,    41,    -1,    64,    63,    -1,    64,    52,    -1,
       6,    73,    32,    -1,     9,    73,    32,    -1,    66,    70,
      -1,    12,    32,    -1,    68,    13,    -1,    -1,    70,    71,
      -1,    70,    32,    -1,    70,    42,    -1,    16,    25,    77,
      32,    -1,    16,    77,    32,    -1,    17,    77,    32,    -1,
      -1,    73,    76,    -1,    27,    -1,    28,    -1,     5,    32,
      -1,     8,    32,    -1,    15,    32,    -1,    32,    -1,    75,
      32,    -1,    -1,    14,    77,    -1,    78,    -1,    78,    35,
      78,    -1,    78,    29,    78,    -1,    31,    77,    30,    -1,
      36,    77,    -1,    77,    33,    77,    -1,    77,    34,    77,
      -1,    27,    -1,    28,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   105,   105,   107,   109,   110,   111,   112,   113,   114,
     115,   119,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   127,   128,   129,   130,   131,   132,   136,   137,   143,
     151,   157,   165,   175,   177,   178,   179,   180,   181,   184,
     192,   198,   208,   214,   220,   228,   237,   242,   250,   253,
     255,   256,   257,   258,   259,   262,   268,   279,   285,   290,
     300,   302,   307,   315,   323,   326,   328,   329,   330,   335,
     342,   347,   355,   358,   360,   361,   362,   365,   373,   380,
     387,   393,   400,   402,   403,   404,   407,   412,   417,   425,
     427,   432,   433,   436,   437,   438,   442,   443,   446,   447,
     450,   451,   452,   453,   454,   455,   456,   459,   460
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_MAINMENU", "T_MENU", "T_ENDMENU",
  "T_SOURCE", "T_CHOICE", "T_ENDCHOICE", "T_COMMENT", "T_CONFIG",
  "T_MENUCONFIG", "T_HELP", "T_HELPTEXT", "T_IF", "T_ENDIF", "T_DEPENDS",
  "T_REQUIRES", "T_OPTIONAL", "T_PROMPT", "T_TYPE", "T_DEFAULT",
  "T_DESELECT", "T_SELECT", "T_RANGE", "T_ON", "T_RESET", "T_WORD",
  "T_WORD_QUOTE", "T_UNEQUAL", "T_CLOSE_PAREN", "T_OPEN_PAREN", "T_EOL",
  "T_OR", "T_AND", "T_EQUAL", "T_NOT", "$accept", "input", "stmt_list",
  "option_name", "common_stmt", "option_error", "config_entry_start",
  "config_stmt", "menuconfig_entry_start", "menuconfig_stmt",
  "config_option_list", "config_option", "choice", "choice_entry",
  "choice_end", "choice_stmt", "choice_option_list", "choice_option",
  "choice_block", "if_entry", "if_end", "if_stmt", "if_block", "menu",
  "menu_entry", "menu_end", "menu_stmt", "menu_block", "source_stmt",
  "comment", "comment_stmt", "help_start", "help", "depends_list",
  "depends", "prompt_stmt_opt", "prompt", "end", "nl", "if_expr", "expr",
  "symbol", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    37,    38,    39,    39,    39,    39,    39,    39,    39,
      39,    39,    40,    40,    40,    40,    40,    40,    40,    40,
      40,    41,    41,    41,    41,    41,    41,    42,    42,    43,
      44,    45,    46,    47,    47,    47,    47,    47,    47,    48,
      48,    48,    48,    48,    48,    49,    50,    51,    52,    53,
      53,    53,    53,    53,    53,    54,    54,    54,    54,    54,
      55,    55,    56,    57,    58,    59,    59,    59,    59,    60,
      61,    62,    63,    64,    64,    64,    64,    65,    66,    67,
      68,    69,    70,    70,    70,    70,    71,    71,    71,    72,
      72,    73,    73,    74,    74,    74,    75,    75,    76,    76,
      77,    77,    77,    77,    77,    77,    77,    78,    78
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     4,     2,     4,
       4,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     3,
       2,     3,     2,     0,     2,     2,     2,     2,     2,     3,
       4,     4,     4,     4,     5,     2,     2,     1,     3,     0,
       2,     2,     2,     2,     2,     4,     3,     2,     3,     4,
       0,     2,     3,     1,     3,     0,     2,     2,     2,     3,
       2,     1,     3,     0,     2,     2,     2,     3,     3,     2,
       2,     2,     0,     2,     2,     2,     4,     3,     3,     0,
       2,     1,     1,     2,     2,     2,     1,     2,     0,     2,
       1,     3,     3,     3,     2,     3,     3,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    12,    17,    13,    14,
      19,    15,    16,    18,    20,     0,    21,     0,     4,    33,
      24,    33,    25,    49,    60,     5,    65,    22,    82,    73,
       6,    26,    82,    23,     8,    11,    91,    92,     0,     0,
      93,     0,    45,    94,     0,     0,     0,   107,   108,     0,
       0,     0,   100,    95,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    96,     7,    69,    77,    78,    29,
      31,     0,   104,     0,     0,    62,     0,     0,     9,    10,
       0,     0,     0,     0,     0,    89,     0,     0,     0,     0,
       0,    38,    37,    34,     0,    36,    35,     0,     0,    89,
       0,    98,    53,    54,    50,    52,    51,    61,    48,    47,
      66,    68,    64,    67,    63,    84,    85,    83,    74,    76,
      72,    75,    71,    97,   103,   105,   106,   102,   101,    28,
      80,     0,     0,     0,    98,     0,    98,    98,    98,    98,
       0,     0,    81,    57,    98,     0,    98,     0,     0,     0,
      87,    88,     0,    39,    90,     0,     0,     0,    98,    27,
       0,    56,     0,    99,    58,    86,    40,    41,    42,    43,
       0,    55,    59,    44
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,     2,    27,    28,   102,    29,    30,    31,    32,
      66,   103,    33,    34,   118,    35,    68,   114,    69,    36,
     122,    37,    70,    38,    39,   130,    40,    72,    41,    42,
      43,   104,   105,    71,   106,   145,   146,    44,    75,   158,
      61,    62
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -80
static const short int yypact[] =
{
     -80,     2,   164,   -80,   -22,   105,   105,    -6,   105,     0,
       7,   105,    17,    28,    70,    35,   -80,   -80,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,    69,   -80,    78,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,    62,    68,
     -80,    85,   -80,   -80,    97,   127,   144,   -80,   -80,    70,
      70,   188,    -8,   -80,   149,   163,    42,   104,   192,    67,
     221,     8,   221,   134,   -80,   167,   -80,   -80,   -80,   -80,
     -80,    50,   -80,    70,    70,   167,   119,   119,   -80,   -80,
     173,   184,    60,    70,   105,   105,    70,    65,   150,   119,
     232,   -80,   -80,   -80,   210,   -80,   -80,   202,   105,   105,
     231,   224,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,   226,   -80,   -80,   -80,   -80,
     -80,    70,   211,   218,   224,   230,   224,    -3,   224,   224,
     119,   233,   -80,   -80,   224,   234,   224,    70,   235,   222,
     -80,   -80,   236,   -80,   -80,   237,   238,   239,   224,   -80,
     240,   -80,   241,   129,   -80,   -80,   -80,   -80,   -80,   -80,
     242,   -80,   -80,   -80
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
     -80,   -80,   -80,   -80,   -36,    22,   -80,   -80,   -80,   -80,
     244,   -80,   -80,   -80,   -80,   176,   -80,   -80,   -80,   -80,
     -80,   -80,   -80,   -80,   -80,   -80,   187,   -80,   -80,   -80,
     -80,   -80,   195,   243,   121,   155,    -5,   145,   215,    93,
     -55,   -79
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -80
static const short int yytable[] =
{
      48,    49,     3,    51,    81,    82,    54,   137,   138,    90,
      45,   157,   -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,
     150,    86,   -70,   -70,    92,    93,    50,    87,   135,   136,
      83,    84,    52,   117,   120,   100,   128,   142,   143,    53,
     125,   147,   -30,    90,    55,   -30,   -30,   -30,   -30,   -30,
     -30,   -30,   -30,   -30,    91,    56,   -30,   -30,    92,    93,
     -30,    94,    95,    96,    97,    98,    99,    63,   -30,   100,
      64,   168,     7,     8,   101,    10,    11,    12,    13,    65,
     134,    14,    15,    83,    84,   141,   159,    57,    58,   144,
     113,    59,   148,   126,    74,   126,    60,    57,    58,    26,
      76,    59,   173,   154,   -32,    90,    60,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,   -32,    91,    77,   -32,   -32,
      92,    93,   -32,    94,    95,    96,    97,    98,    99,    78,
     -32,   100,    46,    47,   -79,    90,   101,   -79,   -79,   -79,
     -79,   -79,   -79,   -79,   -79,   -79,    57,    58,   -79,   -79,
      92,    93,   -79,   -79,   -79,   -79,   -79,   -79,   -79,    79,
     -79,   100,    83,    84,    -2,     4,   125,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    80,   149,    14,    15,
      16,    88,    17,    18,    19,    20,    21,    22,    23,   116,
      24,    25,   127,    90,   127,    89,    26,   -46,   -46,   133,
     -46,   -46,   -46,   -46,    91,   139,   -46,   -46,    92,    93,
     107,   108,   109,   110,   119,   124,   140,   132,   111,   100,
      74,    83,    84,   152,   112,     6,     7,     8,     9,    10,
      11,    12,    13,   151,   153,    14,    15,   162,   157,   164,
     165,   166,   167,   160,    83,    84,   121,   170,   129,   172,
     161,    83,    84,    26,   175,    83,    84,   123,   156,   131,
      84,   180,   163,   115,   155,   169,   171,   174,   176,   177,
     178,   179,   181,   182,   183,    67,    85,     0,     0,     0,
       0,     0,     0,     0,     0,    73
};

static const short int yycheck[] =
{
       5,     6,     0,     8,    59,    60,    11,    86,    87,     1,
      32,    14,     4,     5,     6,     7,     8,     9,    10,    11,
      99,    29,    14,    15,    16,    17,    32,    35,    83,    84,
      33,    34,    32,    69,    70,    27,    72,    92,    93,    32,
      32,    96,     0,     1,    27,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    27,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    32,    26,    27,
       1,   150,     5,     6,    32,     8,     9,    10,    11,     1,
      30,    14,    15,    33,    34,    25,   141,    27,    28,    94,
      68,    31,    27,    71,    32,    73,    36,    27,    28,    32,
      32,    31,   157,   108,     0,     1,    36,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    32,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    32,
      26,    27,    27,    28,     0,     1,    32,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    27,    28,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    32,
      26,    27,    33,    34,     0,     1,    32,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    32,    27,    14,    15,
      16,    32,    18,    19,    20,    21,    22,    23,    24,    68,
      26,    27,    71,     1,    73,    32,    32,     5,     6,    32,
       8,     9,    10,    11,    12,    32,    14,    15,    16,    17,
      18,    19,    20,    21,    69,    70,    32,    72,    26,    27,
      32,    33,    34,    13,    32,     4,     5,     6,     7,     8,
       9,    10,    11,     1,    32,    14,    15,   144,    14,   146,
     147,   148,   149,    32,    33,    34,    70,   154,    72,   156,
      32,    33,    34,    32,    32,    33,    34,    70,    27,    72,
      34,   168,    32,    68,   109,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    31,    61,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    38,    39,     0,     1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    14,    15,    16,    18,    19,    20,
      21,    22,    23,    24,    26,    27,    32,    40,    41,    43,
      44,    45,    46,    49,    50,    52,    56,    58,    60,    61,
      63,    65,    66,    67,    74,    32,    27,    28,    73,    73,
      32,    73,    32,    32,    73,    27,    27,    27,    28,    31,
      36,    77,    78,    32,     1,     1,    47,    47,    53,    55,
      59,    70,    64,    70,    32,    75,    32,    32,    32,    32,
      32,    77,    77,    33,    34,    75,    29,    35,    32,    32,
       1,    12,    16,    17,    19,    20,    21,    22,    23,    24,
      27,    32,    42,    48,    68,    69,    71,    18,    19,    20,
      21,    26,    32,    42,    54,    69,    71,    41,    51,    74,
      41,    52,    57,    63,    74,    32,    42,    71,    41,    52,
      62,    63,    74,    32,    30,    77,    77,    78,    78,    32,
      32,    25,    77,    77,    73,    72,    73,    77,    27,    27,
      78,     1,    13,    32,    73,    72,    27,    14,    76,    77,
      32,    32,    76,    32,    76,    76,    76,    76,    78,    32,
      76,    32,    76,    77,    32,    32,    32,    32,    32,    32,
      76,    32,    32,    32
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {
      case 50: /* choice_entry */

        {
	fprintf(stderr, "%s:%d: missing end statement for this entry\n",
		yyvaluep->menu->file->name, yyvaluep->menu->lineno);
	if (current_menu == yyvaluep->menu)
		menu_end_menu();
};

        break;
      case 56: /* if_entry */

        {
	fprintf(stderr, "%s:%d: missing end statement for this entry\n",
		yyvaluep->menu->file->name, yyvaluep->menu->lineno);
	if (current_menu == yyvaluep->menu)
		menu_end_menu();
};

        break;
      case 61: /* menu_entry */

        {
	fprintf(stderr, "%s:%d: missing end statement for this entry\n",
		yyvaluep->menu->file->name, yyvaluep->menu->lineno);
	if (current_menu == yyvaluep->menu)
		menu_end_menu();
};

        break;

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
        case 8:

    { zconf_error("unexpected end statement"); ;}
    break;

  case 9:

    { zconf_error("unknown statement \"%s\"", yyvsp[-2].string); ;}
    break;

  case 10:

    {
	zconf_error("unexpected option \"%s\"", kconf_id_strings + yyvsp[-2].id->name);
;}
    break;

  case 11:

    { zconf_error("invalid statement"); ;}
    break;

  case 27:

    { zconf_error("unknown option \"%s\"", yyvsp[-2].string); ;}
    break;

  case 28:

    { zconf_error("invalid option"); ;}
    break;

  case 29:

    {
	struct symbol *sym = sym_lookup(yyvsp[-1].string, 0);
	sym->flags |= SYMBOL_OPTIONAL;
	menu_add_entry(sym);
	printd(DEBUG_PARSE, "%s:%d:config %s\n", zconf_curname(), zconf_lineno(), yyvsp[-1].string);
;}
    break;

  case 30:

    {
	menu_end_entry();
	printd(DEBUG_PARSE, "%s:%d:endconfig\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 31:

    {
	struct symbol *sym = sym_lookup(yyvsp[-1].string, 0);
	sym->flags |= SYMBOL_OPTIONAL;
	menu_add_entry(sym);
	printd(DEBUG_PARSE, "%s:%d:menuconfig %s\n", zconf_curname(), zconf_lineno(), yyvsp[-1].string);
;}
    break;

  case 32:

    {
	if (current_entry->prompt)
		current_entry->prompt->type = P_MENU;
	else
		zconfprint("warning: menuconfig statement without prompt");
	menu_end_entry();
	printd(DEBUG_PARSE, "%s:%d:endconfig\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 39:

    {
	menu_set_type(yyvsp[-2].id->stype);
	printd(DEBUG_PARSE, "%s:%d:type(%u)\n",
		zconf_curname(), zconf_lineno(),
		yyvsp[-2].id->stype);
;}
    break;

  case 40:

    {
	menu_add_prompt(P_PROMPT, yyvsp[-2].string, yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:prompt\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 41:

    {
	menu_add_expr(P_DEFAULT, yyvsp[-2].expr, yyvsp[-1].expr);
	if (yyvsp[-3].id->stype != S_UNKNOWN)
		menu_set_type(yyvsp[-3].id->stype);
	printd(DEBUG_PARSE, "%s:%d:default(%u)\n",
		zconf_curname(), zconf_lineno(),
		yyvsp[-3].id->stype);
;}
    break;

  case 42:

    {
	menu_add_symbol(P_DESELECT, sym_lookup(yyvsp[-2].string, 0), yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:deselect\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 43:

    {
	menu_add_symbol(P_SELECT, sym_lookup(yyvsp[-2].string, 0), yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:select\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 44:

    {
	menu_add_expr(P_RANGE, expr_alloc_comp(E_RANGE,yyvsp[-3].symbol, yyvsp[-2].symbol), yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:range\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 45:

    {
	struct symbol *sym = sym_lookup(NULL, 0);
	sym->flags |= SYMBOL_CHOICE;
	menu_add_entry(sym);
	menu_add_expr(P_CHOICE, NULL, NULL);
	printd(DEBUG_PARSE, "%s:%d:choice\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 46:

    {
	yyval.menu = menu_add_menu();
;}
    break;

  case 47:

    {
	if (zconf_endtoken(yyvsp[0].id, T_CHOICE, T_ENDCHOICE)) {
		menu_end_menu();
		printd(DEBUG_PARSE, "%s:%d:endchoice\n", zconf_curname(), zconf_lineno());
	}
;}
    break;

  case 55:

    {
	menu_add_prompt(P_PROMPT, yyvsp[-2].string, yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:prompt\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 56:

    {
	if (yyvsp[-2].id->stype == S_BOOLEAN || yyvsp[-2].id->stype == S_TRISTATE) {
		menu_set_type(yyvsp[-2].id->stype);
		printd(DEBUG_PARSE, "%s:%d:type(%u)\n",
			zconf_curname(), zconf_lineno(),
			yyvsp[-2].id->stype);
	} else
		YYERROR;
;}
    break;

  case 57:

    {
	current_entry->sym->flags |= SYMBOL_OPTIONAL;
	printd(DEBUG_PARSE, "%s:%d:optional\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 58:

    {
	menu_add_prop(P_RESET, NULL, NULL, yyvsp[-1].expr);
;}
    break;

  case 59:

    {
	if (yyvsp[-3].id->stype == S_UNKNOWN) {
		menu_add_symbol(P_DEFAULT, sym_lookup(yyvsp[-2].string, 0), yyvsp[-1].expr);
		printd(DEBUG_PARSE, "%s:%d:default\n",
			zconf_curname(), zconf_lineno());
	} else
		YYERROR;
;}
    break;

  case 62:

    {
	printd(DEBUG_PARSE, "%s:%d:if\n", zconf_curname(), zconf_lineno());
	menu_add_entry(NULL);
	menu_add_dep(yyvsp[-1].expr);
	yyval.menu = menu_add_menu();
;}
    break;

  case 63:

    {
	if (zconf_endtoken(yyvsp[0].id, T_IF, T_ENDIF)) {
		menu_end_menu();
		printd(DEBUG_PARSE, "%s:%d:endif\n", zconf_curname(), zconf_lineno());
	}
;}
    break;

  case 69:

    {
	menu_add_entry(NULL);
	menu_add_prompt(P_MENU, yyvsp[-1].string, NULL);
	printd(DEBUG_PARSE, "%s:%d:menu\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 70:

    {
	yyval.menu = menu_add_menu();
;}
    break;

  case 71:

    {
	if (zconf_endtoken(yyvsp[0].id, T_MENU, T_ENDMENU)) {
		menu_end_menu();
		printd(DEBUG_PARSE, "%s:%d:endmenu\n", zconf_curname(), zconf_lineno());
	}
;}
    break;

  case 77:

    {
	printd(DEBUG_PARSE, "%s:%d:source %s\n", zconf_curname(), zconf_lineno(), yyvsp[-1].string);
	zconf_nextfile(yyvsp[-1].string);
;}
    break;

  case 78:

    {
	menu_add_entry(NULL);
	menu_add_prompt(P_COMMENT, yyvsp[-1].string, NULL);
	printd(DEBUG_PARSE, "%s:%d:comment\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 79:

    {
	menu_end_entry();
;}
    break;

  case 80:

    {
	printd(DEBUG_PARSE, "%s:%d:help\n", zconf_curname(), zconf_lineno());
	zconf_starthelp();
;}
    break;

  case 81:

    {
	current_entry->sym->help = yyvsp[0].string;
;}
    break;

  case 86:

    {
	menu_add_dep(yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:depends on\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 87:

    {
	menu_add_dep(yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:depends\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 88:

    {
	menu_add_dep(yyvsp[-1].expr);
	printd(DEBUG_PARSE, "%s:%d:requires\n", zconf_curname(), zconf_lineno());
;}
    break;

  case 90:

    {
	menu_add_prompt(P_PROMPT, yyvsp[-1].string, yyvsp[0].expr);
;}
    break;

  case 93:

    { yyval.id = yyvsp[-1].id; ;}
    break;

  case 94:

    { yyval.id = yyvsp[-1].id; ;}
    break;

  case 95:

    { yyval.id = yyvsp[-1].id; ;}
    break;

  case 98:

    { yyval.expr = NULL; ;}
    break;

  case 99:

    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 100:

    { yyval.expr = expr_alloc_symbol(yyvsp[0].symbol); ;}
    break;

  case 101:

    { yyval.expr = expr_alloc_comp(E_EQUAL, yyvsp[-2].symbol, yyvsp[0].symbol); ;}
    break;

  case 102:

    { yyval.expr = expr_alloc_comp(E_UNEQUAL, yyvsp[-2].symbol, yyvsp[0].symbol); ;}
    break;

  case 103:

    { yyval.expr = yyvsp[-1].expr; ;}
    break;

  case 104:

    { yyval.expr = expr_alloc_one(E_NOT, yyvsp[0].expr); ;}
    break;

  case 105:

    { yyval.expr = expr_alloc_two(E_OR, yyvsp[-2].expr, yyvsp[0].expr); ;}
    break;

  case 106:

    { yyval.expr = expr_alloc_two(E_AND, yyvsp[-2].expr, yyvsp[0].expr); ;}
    break;

  case 107:

    { yyval.symbol = sym_lookup(yyvsp[0].string, 0); free(yyvsp[0].string); ;}
    break;

  case 108:

    { yyval.symbol = sym_lookup(yyvsp[0].string, 1); free(yyvsp[0].string); ;}
    break;


    }

/* Line 1010 of yacc.c.  */


  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}





void conf_parse(const char *name)
{
	struct symbol *sym;
	int i;

	zconf_initscan(name);

	sym_init();
	menu_init();
	modules_sym = sym_lookup("MODULES", 0);
	rootmenu.prompt = menu_add_prompt(P_MENU, "OpenWrt Configuration", NULL);

#if YYDEBUG
	if (getenv("ZCONF_DEBUG"))
		zconfdebug = 1;
#endif
	zconfparse();
	if (zconfnerrs)
		exit(1);
	menu_finalize(&rootmenu);
	for_all_symbols(i, sym) {
		sym_check_deps(sym);
        }

	sym_change_count = 1;
}

const char *zconf_tokenname(int token)
{
	switch (token) {
	case T_MENU:		return "menu";
	case T_ENDMENU:		return "endmenu";
	case T_CHOICE:		return "choice";
	case T_ENDCHOICE:	return "endchoice";
	case T_IF:		return "if";
	case T_ENDIF:		return "endif";
	case T_DEPENDS:		return "depends";
	}
	return "<token>";
}

static bool zconf_endtoken(struct kconf_id *id, int starttoken, int endtoken)
{
	if (id->token != endtoken) {
		zconf_error("unexpected '%s' within %s block",
			kconf_id_strings + id->name, zconf_tokenname(starttoken));
		zconfnerrs++;
		return false;
	}
	if (current_menu->file != current_file) {
		zconf_error("'%s' in different file than '%s'",
			kconf_id_strings + id->name, zconf_tokenname(starttoken));
		fprintf(stderr, "%s:%d: location of the '%s'\n",
			current_menu->file->name, current_menu->lineno,
			zconf_tokenname(starttoken));
		zconfnerrs++;
		return false;
	}
	return true;
}

static void zconfprint(const char *err, ...)
{
	va_list ap;

	fprintf(stderr, "%s:%d: ", zconf_curname(), zconf_lineno());
	va_start(ap, err);
	vfprintf(stderr, err, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

static void zconf_error(const char *err, ...)
{
	va_list ap;

	zconfnerrs++;
	fprintf(stderr, "%s:%d: ", zconf_curname(), zconf_lineno());
	va_start(ap, err);
	vfprintf(stderr, err, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

static void zconferror(const char *err)
{
#if YYDEBUG
	fprintf(stderr, "%s:%d: %s\n", zconf_curname(), zconf_lineno() + 1, err);
#endif
}

void print_quoted_string(FILE *out, const char *str)
{
	const char *p;
	int len;

	putc('"', out);
	while ((p = strchr(str, '"'))) {
		len = p - str;
		if (len)
			fprintf(out, "%.*s", len, str);
		fputs("\\\"", out);
		str = p + 1;
	}
	fputs(str, out);
	putc('"', out);
}

void print_symbol(FILE *out, struct menu *menu)
{
	struct symbol *sym = menu->sym;
	struct property *prop;

	if (sym_is_choice(sym))
		fprintf(out, "choice\n");
	else
		fprintf(out, "config %s\n", sym->name);
	switch (sym->type) {
	case S_BOOLEAN:
		fputs("  boolean\n", out);
		break;
	case S_TRISTATE:
		fputs("  tristate\n", out);
		break;
	case S_STRING:
		fputs("  string\n", out);
		break;
	case S_INT:
		fputs("  integer\n", out);
		break;
	case S_HEX:
		fputs("  hex\n", out);
		break;
	default:
		fputs("  ???\n", out);
		break;
	}
	for (prop = sym->prop; prop; prop = prop->next) {
		if (prop->menu != menu)
			continue;
		switch (prop->type) {
		case P_PROMPT:
			fputs("  prompt ", out);
			print_quoted_string(out, prop->text);
			if (!expr_is_yes(prop->visible.expr)) {
				fputs(" if ", out);
				expr_fprint(prop->visible.expr, out);
			}
			fputc('\n', out);
			break;
		case P_DEFAULT:
			fputs( "  default ", out);
			expr_fprint(prop->expr, out);
			if (!expr_is_yes(prop->visible.expr)) {
				fputs(" if ", out);
				expr_fprint(prop->visible.expr, out);
			}
			fputc('\n', out);
			break;
		case P_CHOICE:
			fputs("  #choice value\n", out);
			break;
		default:
			fprintf(out, "  unknown prop %d!\n", prop->type);
			break;
		}
	}
	if (sym->help) {
		int len = strlen(sym->help);
		while (sym->help[--len] == '\n')
			sym->help[len] = 0;
		fprintf(out, "  help\n%s\n", sym->help);
	}
	fputc('\n', out);
}

void zconfdump(FILE *out)
{
	struct property *prop;
	struct symbol *sym;
	struct menu *menu;

	menu = rootmenu.list;
	while (menu) {
		if ((sym = menu->sym))
			print_symbol(out, menu);
		else if ((prop = menu->prompt)) {
			switch (prop->type) {
			case P_COMMENT:
				fputs("\ncomment ", out);
				print_quoted_string(out, prop->text);
				fputs("\n", out);
				break;
			case P_MENU:
				fputs("\nmenu ", out);
				print_quoted_string(out, prop->text);
				fputs("\n", out);
				break;
			default:
				;
			}
			if (!expr_is_yes(prop->visible.expr)) {
				fputs("  depends ", out);
				expr_fprint(prop->visible.expr, out);
				fputc('\n', out);
			}
			fputs("\n", out);
		}

		if (menu->list)
			menu = menu->list;
		else if (menu->next)
			menu = menu->next;
		else while ((menu = menu->parent)) {
			if (menu->prompt && menu->prompt->type == P_MENU)
				fputs("\nendmenu\n", out);
			if (menu->next) {
				menu = menu->next;
				break;
			}
		}
	}
}

#include "lex.zconf.c"
#include "util.c"
#include "confdata.c"
#include "expr.c"
#include "symbol.c"
#include "menu.c"


