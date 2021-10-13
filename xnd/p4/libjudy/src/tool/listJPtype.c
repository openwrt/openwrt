// Copyright (C) 2000 - 2002 Hewlett-Packard Company
//
// This program is free software; you can redistribute it and/or modify it
// under the term of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// _________________

// @(#) $Revision: 4.9 $ $Date: 2002/06/25 00:35:37 $
// @(#) $Source: /cvsroot/judy/tool/listJPtype.c,v $

// Print JP values.
//
// Compile with one of -DJUDY1 or -DJUDYL; defaults to JUDY1:
//
// IOPTS='-I ../src -I ../src/Judy1 -I ../src/JudyL -I ../src/JudyCommon'
// cc	    -Ae -DJUDY1		   $IOPTS listJPtype.c -o listJPtype1
// cc	    -Ae -DJUDYL		   $IOPTS listJPtype.c -o listJPtypeL
// cc +DD64 -Ae -DJUDY1 -DJU_64BIT $IOPTS listJPtype.c -o listJPtype1_64
// cc +DD64 -Ae -DJUDYL -DJU_64BIT $IOPTS listJPtype.c -o listJPtypeL_64

#ifndef	JUDYL
#ifndef	JUDY1
#define	JUDY1 1			// neither set => use default.
#endif
#endif

#ifdef JUDY1
#include "Judy1.h"
#else
#include "JudyL.h"
#endif

// Lists of all JP types; must agree with header files:

struct list {
	char *	name;
	int	value;
} *Plist, list[] = {

#ifdef JUDY1

    { "cJ1_JAPNULL",		cJ1_JAPNULL,		},
    { "cJ1_JAPLEAF",		cJ1_JAPLEAF,		},
    { "cJ1_JAPBRANCH",		cJ1_JAPBRANCH,		},
    { "cJ1_JPNULL1",		cJ1_JPNULL1,		},
    { "cJ1_JPNULL2",		cJ1_JPNULL2,		},
    { "cJ1_JPNULL3",		cJ1_JPNULL3,		},
#ifdef JU_64BIT
    { "cJ1_JPNULL4",		cJ1_JPNULL4,		},
    { "cJ1_JPNULL5",		cJ1_JPNULL5,		},
    { "cJ1_JPNULL6",		cJ1_JPNULL6,		},
    { "cJ1_JPNULL7",		cJ1_JPNULL7,		},
#endif
    { "cJ1_JPNULLMAX",		cJ1_JPNULLMAX,		},
    { "cJ1_JPBRANCH_L2",	cJ1_JPBRANCH_L2,	},
    { "cJ1_JPBRANCH_L3",	cJ1_JPBRANCH_L3,	},
#ifdef JU_64BIT
    { "cJ1_JPBRANCH_L4",	cJ1_JPBRANCH_L4,	},
    { "cJ1_JPBRANCH_L5",	cJ1_JPBRANCH_L5,	},
    { "cJ1_JPBRANCH_L6",	cJ1_JPBRANCH_L6,	},
    { "cJ1_JPBRANCH_L7",	cJ1_JPBRANCH_L7,	},
#endif
    { "cJ1_JPBRANCH_L",		cJ1_JPBRANCH_L,		},
    { "cJ1_JPBRANCH_B2",	cJ1_JPBRANCH_B2,	},
    { "cJ1_JPBRANCH_B3",	cJ1_JPBRANCH_B3,	},
#ifdef JU_64BIT
    { "cJ1_JPBRANCH_B4",	cJ1_JPBRANCH_B4,	},
    { "cJ1_JPBRANCH_B5",	cJ1_JPBRANCH_B5,	},
    { "cJ1_JPBRANCH_B6",	cJ1_JPBRANCH_B6,	},
    { "cJ1_JPBRANCH_B7",	cJ1_JPBRANCH_B7,	},
#endif
    { "cJ1_JPBRANCH_B",		cJ1_JPBRANCH_B,		},
    { "cJ1_JPBRANCH_U2",	cJ1_JPBRANCH_U2,	},
    { "cJ1_JPBRANCH_U3",	cJ1_JPBRANCH_U3,	},
#ifdef JU_64BIT
    { "cJ1_JPBRANCH_U4",	cJ1_JPBRANCH_U4,	},
    { "cJ1_JPBRANCH_U5",	cJ1_JPBRANCH_U5,	},
    { "cJ1_JPBRANCH_U6",	cJ1_JPBRANCH_U6,	},
    { "cJ1_JPBRANCH_U7",	cJ1_JPBRANCH_U7,	},
#endif
    { "cJ1_JPBRANCH_U",		cJ1_JPBRANCH_U,		},
#ifndef	JU_64BIT
    { "cJ1_JPLEAF1",		cJ1_JPLEAF1,		},
#endif
    { "cJ1_JPLEAF2",		cJ1_JPLEAF2,		},
    { "cJ1_JPLEAF3",		cJ1_JPLEAF3,		},
#ifdef JU_64BIT
    { "cJ1_JPLEAF4",		cJ1_JPLEAF4,		},
    { "cJ1_JPLEAF5",		cJ1_JPLEAF5,		},
    { "cJ1_JPLEAF6",		cJ1_JPLEAF6,		},
    { "cJ1_JPLEAF7",		cJ1_JPLEAF7,		},
#endif
    { "cJ1_JPLEAF_B1",		cJ1_JPLEAF_B1,		},
    { "cJ1_JPFULLPOPU1",	cJ1_JPFULLPOPU1,	},
    { "cJ1_JPIMMED_1_01",	cJ1_JPIMMED_1_01,	},
    { "cJ1_JPIMMED_2_01",	cJ1_JPIMMED_2_01,	},
    { "cJ1_JPIMMED_3_01",	cJ1_JPIMMED_3_01,	},
#ifdef JU_64BIT
    { "cJ1_JPIMMED_4_01",	cJ1_JPIMMED_4_01,	},
    { "cJ1_JPIMMED_5_01",	cJ1_JPIMMED_5_01,	},
    { "cJ1_JPIMMED_6_01",	cJ1_JPIMMED_6_01,	},
    { "cJ1_JPIMMED_7_01",	cJ1_JPIMMED_7_01,	},
#endif
    { "cJ1_JPIMMED_1_02",	cJ1_JPIMMED_1_02,	},
    { "cJ1_JPIMMED_1_03",	cJ1_JPIMMED_1_03,	},
    { "cJ1_JPIMMED_1_04",	cJ1_JPIMMED_1_04,	},
    { "cJ1_JPIMMED_1_05",	cJ1_JPIMMED_1_05,	},
    { "cJ1_JPIMMED_1_06",	cJ1_JPIMMED_1_06,	},
    { "cJ1_JPIMMED_1_07",	cJ1_JPIMMED_1_07,	},
#ifdef JU_64BIT
    { "cJ1_JPIMMED_1_08",	cJ1_JPIMMED_1_08,	},
    { "cJ1_JPIMMED_1_09",	cJ1_JPIMMED_1_09,	},
    { "cJ1_JPIMMED_1_10",	cJ1_JPIMMED_1_10,	},
    { "cJ1_JPIMMED_1_11",	cJ1_JPIMMED_1_11,	},
    { "cJ1_JPIMMED_1_12",	cJ1_JPIMMED_1_12,	},
    { "cJ1_JPIMMED_1_13",	cJ1_JPIMMED_1_13,	},
    { "cJ1_JPIMMED_1_14",	cJ1_JPIMMED_1_14,	},
    { "cJ1_JPIMMED_1_15",	cJ1_JPIMMED_1_15,	},
#endif
    { "cJ1_JPIMMED_2_02",	cJ1_JPIMMED_2_02,	},
    { "cJ1_JPIMMED_2_03",	cJ1_JPIMMED_2_03,	},
#ifdef JU_64BIT
    { "cJ1_JPIMMED_2_04",	cJ1_JPIMMED_2_04,	},
    { "cJ1_JPIMMED_2_05",	cJ1_JPIMMED_2_05,	},
    { "cJ1_JPIMMED_2_06",	cJ1_JPIMMED_2_06,	},
    { "cJ1_JPIMMED_2_07",	cJ1_JPIMMED_2_07,	},
#endif
    { "cJ1_JPIMMED_3_02",	cJ1_JPIMMED_3_02,	},
#ifdef JU_64BIT
    { "cJ1_JPIMMED_3_03",	cJ1_JPIMMED_3_03,	},
    { "cJ1_JPIMMED_3_04",	cJ1_JPIMMED_3_04,	},
    { "cJ1_JPIMMED_3_05",	cJ1_JPIMMED_3_05,	},
    { "cJ1_JPIMMED_4_02",	cJ1_JPIMMED_4_02,	},
    { "cJ1_JPIMMED_4_03",	cJ1_JPIMMED_4_03,	},
    { "cJ1_JPIMMED_5_02",	cJ1_JPIMMED_5_02,	},
    { "cJ1_JPIMMED_5_03",	cJ1_JPIMMED_5_03,	},
    { "cJ1_JPIMMED_6_02",	cJ1_JPIMMED_6_02,	},
    { "cJ1_JPIMMED_7_02",	cJ1_JPIMMED_7_02,	},
#endif
    { "cJ1_JPIMMED_CAP",	cJ1_JPIMMED_CAP,	},

#else // JUDYL -------------------------------------------------------------

    { "cJL_JAPNULL",		cJL_JAPNULL,		},
    { "cJL_JAPLEAF",		cJL_JAPLEAF,		},
    { "cJL_JAPLEAF_POPU2",	cJL_JAPLEAF_POPU2,	},
    { "cJL_JAPBRANCH",		cJL_JAPBRANCH,		},
    { "cJL_JAPINVALID",		cJL_JAPINVALID,		},
    { "cJL_JAPLEAF_POPU1",	cJL_JAPLEAF_POPU1,	},
    { "cJL_JPNULL1",		cJL_JPNULL1,		},
    { "cJL_JPNULL2",		cJL_JPNULL2,		},
    { "cJL_JPNULL3",		cJL_JPNULL3,		},
#ifdef JU_64BIT
    { "cJL_JPNULL4",		cJL_JPNULL4,		},
    { "cJL_JPNULL5",		cJL_JPNULL5,		},
    { "cJL_JPNULL6",		cJL_JPNULL6,		},
    { "cJL_JPNULL7",		cJL_JPNULL7,		},
#endif
    { "cJL_JPNULLMAX",		cJL_JPNULLMAX,		},
    { "cJL_JPBRANCH_L2",	cJL_JPBRANCH_L2,	},
    { "cJL_JPBRANCH_L3",	cJL_JPBRANCH_L3,	},
#ifdef JU_64BIT
    { "cJL_JPBRANCH_L4",	cJL_JPBRANCH_L4,	},
    { "cJL_JPBRANCH_L5",	cJL_JPBRANCH_L5,	},
    { "cJL_JPBRANCH_L6",	cJL_JPBRANCH_L6,	},
    { "cJL_JPBRANCH_L7",	cJL_JPBRANCH_L7,	},
#endif
    { "cJL_JPBRANCH_L",		cJL_JPBRANCH_L,		},
    { "cJL_JPBRANCH_B2",	cJL_JPBRANCH_B2,	},
    { "cJL_JPBRANCH_B3",	cJL_JPBRANCH_B3,	},
#ifdef JU_64BIT
    { "cJL_JPBRANCH_B4",	cJL_JPBRANCH_B4,	},
    { "cJL_JPBRANCH_B5",	cJL_JPBRANCH_B5,	},
    { "cJL_JPBRANCH_B6",	cJL_JPBRANCH_B6,	},
    { "cJL_JPBRANCH_B7",	cJL_JPBRANCH_B7,	},
#endif
    { "cJL_JPBRANCH_B",		cJL_JPBRANCH_B,		},
    { "cJL_JPBRANCH_U2",	cJL_JPBRANCH_U2,	},
    { "cJL_JPBRANCH_U3",	cJL_JPBRANCH_U3,	},
#ifdef JU_64BIT
    { "cJL_JPBRANCH_U4",	cJL_JPBRANCH_U4,	},
    { "cJL_JPBRANCH_U5",	cJL_JPBRANCH_U5,	},
    { "cJL_JPBRANCH_U6",	cJL_JPBRANCH_U6,	},
    { "cJL_JPBRANCH_U7",	cJL_JPBRANCH_U7,	},
#endif
    { "cJL_JPBRANCH_U",		cJL_JPBRANCH_U,		},
    { "cJL_JPLEAF1",		cJL_JPLEAF1,		},
    { "cJL_JPLEAF2",		cJL_JPLEAF2,		},
    { "cJL_JPLEAF3",		cJL_JPLEAF3,		},
#ifdef JU_64BIT
    { "cJL_JPLEAF4",		cJL_JPLEAF4,		},
    { "cJL_JPLEAF5",		cJL_JPLEAF5,		},
    { "cJL_JPLEAF6",		cJL_JPLEAF6,		},
    { "cJL_JPLEAF7",		cJL_JPLEAF7,		},
#endif
    { "cJL_JPLEAF_B1",		cJL_JPLEAF_B1,		},
    { "cJL_JPIMMED_1_01",	cJL_JPIMMED_1_01,	},
    { "cJL_JPIMMED_2_01",	cJL_JPIMMED_2_01,	},
    { "cJL_JPIMMED_3_01",	cJL_JPIMMED_3_01,	},
#ifdef JU_64BIT
    { "cJL_JPIMMED_4_01",	cJL_JPIMMED_4_01,	},
    { "cJL_JPIMMED_5_01",	cJL_JPIMMED_5_01,	},
    { "cJL_JPIMMED_6_01",	cJL_JPIMMED_6_01,	},
    { "cJL_JPIMMED_7_01",	cJL_JPIMMED_7_01,	},
#endif
    { "cJL_JPIMMED_1_02",	cJL_JPIMMED_1_02,	},
    { "cJL_JPIMMED_1_03",	cJL_JPIMMED_1_03,	},
#ifdef JU_64BIT
    { "cJL_JPIMMED_1_04",	cJL_JPIMMED_1_04,	},
    { "cJL_JPIMMED_1_05",	cJL_JPIMMED_1_05,	},
    { "cJL_JPIMMED_1_06",	cJL_JPIMMED_1_06,	},
    { "cJL_JPIMMED_1_07",	cJL_JPIMMED_1_07,	},
    { "cJL_JPIMMED_2_02",	cJL_JPIMMED_2_02,	},
    { "cJL_JPIMMED_2_03",	cJL_JPIMMED_2_03,	},
    { "cJL_JPIMMED_3_02",	cJL_JPIMMED_3_02,	},
#endif
    { "cJL_JPIMMED_CAP",	cJL_JPIMMED_CAP,	},

#endif // JUDYL

    { (char *) 0,		0,			},
};

FUNCTION main()
{
	for (Plist = list; (Plist -> name) != (char *) 0; ++Plist)
	    (void) printf ("%-20s %2d\n", Plist -> name, Plist -> value);

} // main
