#ifndef _JUDY1_INCLUDED
#define _JUDY1_INCLUDED
// _________________
//
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

// @(#) $Revision: 4.76 $ $Source: /judy/src/Judy1/Judy1.h $

// ****************************************************************************
//          JUDY1 -- SMALL/LARGE AND/OR CLUSTERED/SPARSE BIT ARRAYS
//
//                                    -by-
//
//                             Douglas L. Baskins
//                             doug@sourcejudy.com
//
// Judy arrays are designed to be used instead of arrays.  The performance
// suggests the reason why Judy arrays are thought of as arrays, instead of
// trees.  They are remarkably memory efficient at all populations.
// Implemented as a hybrid digital tree (but really a state machine, see
// below), Judy arrays feature fast insert/retrievals, fast near neighbor
// searching, and contain a population tree for extremely fast ordinal related
// retrievals.
//
// CONVENTIONS:
//
// - The comments here refer to 32-bit [64-bit] systems.
//
// - BranchL, LeafL refer to linear branches and leaves (small populations),
//   except LeafL does not actually appear as such; rather, Leaf1..3 [Leaf1..7]
//   is used to represent leaf Index sizes, and LeafW refers to a Leaf with
//   full (long) word Indexes, which is also a type of linear leaf.  Note that
//   root-level LeafW (Leaf4 [Leaf8]) leaves are also called LEAFW.
//
// - BranchB, LeafB1 refer to bitmap branches and leaves (intermediate
//   populations).
//
// - BranchU refers to uncompressed branches.  An uncompressed branch has 256
//   JPs, some of which could be null.  Note:  All leaves are compressed (and
//   sorted), or else an expanse is full (FullPopu), so there is no LeafU
//   equivalent to BranchU.
//
// - "Popu" is short for "Population".
// - "Pop1" refers to actual population (base 1).
// - "Pop0" refers to Pop1 - 1 (base 0), the way populations are stored in data
//   structures.
//
// - Branches and Leaves are both named by the number of bytes in their Pop0
//   field.  In the case of Leaves, the same number applies to the Index sizes.
//
// - The representation of many numbers as hex is a relatively safe and
//   portable way to get desired bitpatterns as unsigned longs.
//
// - Some preprocessors cant handle single apostrophe characters within
//   #ifndef code, so here, use delete all instead.

#include "JudyPrivate.h"        // includes Judy.h in turn.
#include "JudyPrivateBranch.h"


// ****************************************************************************
// JUDY1 ROOT POINTER (JRP) AND JUDY1 POINTER (JP) TYPE FIELDS
// ****************************************************************************
//
// The following enum lists all possible JP Type fields. 

typedef enum            // uint8_t -- but C does not support this type of enum.
{

// JP NULL TYPES:
//
// There is a series of cJ1_JPNULL* Types because each one pre-records a
// different Index Size for when the first Index is inserted in the previously
// null JP.  They must start >= 8 (three bits).
//
// Note:  These Types must be in sequential order for doing relative
// calculations between them.

        cJ1_JPNULL1 = 1,
                                // Index Size 1[1] byte  when 1 Index inserted.
        cJ1_JPNULL2,            // Index Size 2[2] bytes when 1 Index inserted.
        cJ1_JPNULL3,            // Index Size 3[3] bytes when 1 Index inserted.

#ifndef JU_64BIT
#define cJ1_JPNULLMAX cJ1_JPNULL3
#else
        cJ1_JPNULL4,            // Index Size 4[4] bytes when 1 Index inserted.
        cJ1_JPNULL5,            // Index Size 5[5] bytes when 1 Index inserted.
        cJ1_JPNULL6,            // Index Size 6[6] bytes when 1 Index inserted.
        cJ1_JPNULL7,            // Index Size 7[7] bytes when 1 Index inserted.
#define cJ1_JPNULLMAX cJ1_JPNULL7
#endif


// JP BRANCH TYPES:
//
// Note:  There are no state-1 branches; only leaves reside at state 1.

// Linear branches:
//
// Note:  These Types must be in sequential order for doing relative
// calculations between them.

        cJ1_JPBRANCH_L2,        // 2[2] bytes Pop0, 1[5] bytes Dcd.
        cJ1_JPBRANCH_L3,        // 3[3] bytes Pop0, 0[4] bytes Dcd.

#ifdef JU_64BIT
        cJ1_JPBRANCH_L4,        //  [4] bytes Pop0,  [3] bytes Dcd.
        cJ1_JPBRANCH_L5,        //  [5] bytes Pop0,  [2] bytes Dcd.
        cJ1_JPBRANCH_L6,        //  [6] bytes Pop0,  [1] byte  Dcd.
        cJ1_JPBRANCH_L7,        //  [7] bytes Pop0,  [0] bytes Dcd.
#endif

        cJ1_JPBRANCH_L,         // note:  DcdPopO field not used.

// Bitmap branches:
//
// Note:  These Types must be in sequential order for doing relative
// calculations between them.

        cJ1_JPBRANCH_B2,        // 2[2] bytes Pop0, 1[5] bytes Dcd.
        cJ1_JPBRANCH_B3,        // 3[3] bytes Pop0, 0[4] bytes Dcd.

#ifdef JU_64BIT
        cJ1_JPBRANCH_B4,        //  [4] bytes Pop0,  [3] bytes Dcd.
        cJ1_JPBRANCH_B5,        //  [5] bytes Pop0,  [2] bytes Dcd.
        cJ1_JPBRANCH_B6,        //  [6] bytes Pop0,  [1] byte  Dcd.
        cJ1_JPBRANCH_B7,        //  [7] bytes Pop0,  [0] bytes Dcd.
#endif

        cJ1_JPBRANCH_B,         // note:  DcdPopO field not used.

// Uncompressed branches:
//
// Note:  These Types must be in sequential order for doing relative
// calculations between them.

        cJ1_JPBRANCH_U2,        // 2[2] bytes Pop0, 1[5] bytes Dcd.
        cJ1_JPBRANCH_U3,        // 3[3] bytes Pop0, 0[4] bytes Dcd.

#ifdef JU_64BIT
        cJ1_JPBRANCH_U4,        //  [4] bytes Pop0,  [3] bytes Dcd.
        cJ1_JPBRANCH_U5,        //  [5] bytes Pop0,  [2] bytes Dcd.
        cJ1_JPBRANCH_U6,        //  [6] bytes Pop0,  [1] byte  Dcd.
        cJ1_JPBRANCH_U7,        //  [7] bytes Pop0,  [0] bytes Dcd.
#endif

        cJ1_JPBRANCH_U,         // note:  DcdPopO field not used.


// JP LEAF TYPES:

// Linear leaves:
//
// Note:  These Types must be in sequential order for doing relative
// calculations between them.
//
// Note:  There is no cJ1_JPLEAF1 for 64-bit for a subtle reason.  An immediate
// JP can hold 15 1-byte Indexes, and a bitmap leaf would be used for 17
// Indexes, so rather than support a linear leaf for only the case of exactly
// 16 Indexes, a bitmap leaf is used in that case.  See also below regarding
// cJ1_LEAF1_MAXPOP1 on 64-bit systems.
//
// Note:  There is no full-word (4-byte [8-byte]) Index leaf under a JP because
// non-root-state leaves only occur under branches that decode at least one
// byte.  Full-word, root-state leaves are under a JRP, not a JP.  However, in
// the code a "fake" JP can be created temporarily above a root-state leaf.

#ifndef JU_64BIT // 32-bit only; see above.
        cJ1_JPLEAF1,            // 1    byte  Pop0, 2    bytes Dcd.
#endif

        cJ1_JPLEAF2,            // 2[2] bytes Pop0, 1[5] bytes Dcd.
        cJ1_JPLEAF3,            // 3[3] bytes Pop0, 0[4] bytes Dcd.

#ifdef JU_64BIT
        cJ1_JPLEAF4,            //  [4] bytes Pop0,  [3] bytes Dcd.
        cJ1_JPLEAF5,            //  [5] bytes Pop0,  [2] bytes Dcd.
        cJ1_JPLEAF6,            //  [6] bytes Pop0,  [1] byte  Dcd.
        cJ1_JPLEAF7,            //  [7] bytes Pop0,  [0] bytes Dcd.
#endif

// Bitmap leaf; Index Size == 1:
//
// Note:  These are currently only supported at state 1.  At other states the
// bitmap would grow from 256 to 256^2, 256^3, ... bits, which would not be
// efficient..

        cJ1_JPLEAF_B1,          // 1[1] byte Pop0, 2[6] bytes Dcd.


// Full population; Index Size == 1 virtual leaf:
//
// Note:  These are currently only supported at state 1.  At other states they
// could be used, but they would be rare and the savings are dubious.

        cJ1_JPFULLPOPU1,        // 1[1] byte Pop0, 2[6] bytes Dcd.

#ifdef notdef  // for future enhancements
        cJ1_JPFULLPOPU1m1,      // Full Population - 1 
        cJ1_JPFULLPOPU1m2,      // Full Population - 2 
        cJ1_JPFULLPOPU1m3,      // Full Population - 3 
        cJ1_JPFULLPOPU1m4,      // Full Population - 4 
        cJ1_JPFULLPOPU1m5,      // Full Population - 5 
        cJ1_JPFULLPOPU1m6,      // Full Population - 6 
        cJ1_JPFULLPOPU1m7,      // Full Population - 7 

#ifdef JU_64BIT
        cJ1_JPFULLPOPU1m8,      // Full Population - 8 
        cJ1_JPFULLPOPU1m9,      // Full Population - 9 
        cJ1_JPFULLPOPU1m10,     // Full Population - 10 
        cJ1_JPFULLPOPU1m11,     // Full Population - 11
        cJ1_JPFULLPOPU1m12,     // Full Population - 12
        cJ1_JPFULLPOPU1m13,     // Full Population - 13
        cJ1_JPFULLPOPU1m14,     // Full Population - 14
        cJ1_JPFULLPOPU1m15,     // Full Population - 15
#endif
#endif // notdef -- for future enhancements


// JP IMMEDIATES; leaves (Indexes) stored inside a JP:
//
// The second numeric suffix is the Pop1 for each type.  As the Index Size
// increases, the maximum possible population decreases.
//
// Note:  These Types must be in sequential order in each group (Index Size),
// and the groups in correct order too, for doing relative calculations between
// them.  For example, since these Types enumerate the Pop1 values (unlike
// other JP Types where there is a Pop0 value in the JP), the maximum Pop1 for
// each Index Size is computable.

        cJ1_JPIMMED_1_01,       // Index Size = 1, Pop1 = 1.
        cJ1_JPIMMED_2_01,       // Index Size = 2, Pop1 = 1.
        cJ1_JPIMMED_3_01,       // Index Size = 3, Pop1 = 1.
#ifdef JU_64BIT
        cJ1_JPIMMED_4_01,       // Index Size = 4, Pop1 = 1.
        cJ1_JPIMMED_5_01,       // Index Size = 5, Pop1 = 1.
        cJ1_JPIMMED_6_01,       // Index Size = 6, Pop1 = 1.
        cJ1_JPIMMED_7_01,       // Index Size = 7, Pop1 = 1.
#endif

        cJ1_JPIMMED_1_02,       // Index Size = 1, Pop1 = 2.
        cJ1_JPIMMED_1_03,       // Index Size = 1, Pop1 = 3.
        cJ1_JPIMMED_1_04,       // Index Size = 1, Pop1 = 4.
        cJ1_JPIMMED_1_05,       // Index Size = 1, Pop1 = 5.
        cJ1_JPIMMED_1_06,       // Index Size = 1, Pop1 = 6.
        cJ1_JPIMMED_1_07,       // Index Size = 1, Pop1 = 7.

#ifdef JU_64BIT
        cJ1_JPIMMED_1_08,       // Index Size = 1, Pop1 = 8.
        cJ1_JPIMMED_1_09,       // Index Size = 1, Pop1 = 9.
        cJ1_JPIMMED_1_10,       // Index Size = 1, Pop1 = 10.
        cJ1_JPIMMED_1_11,       // Index Size = 1, Pop1 = 11.
        cJ1_JPIMMED_1_12,       // Index Size = 1, Pop1 = 12.
        cJ1_JPIMMED_1_13,       // Index Size = 1, Pop1 = 13.
        cJ1_JPIMMED_1_14,       // Index Size = 1, Pop1 = 14.
        cJ1_JPIMMED_1_15,       // Index Size = 1, Pop1 = 15.
#endif

        cJ1_JPIMMED_2_02,       // Index Size = 2, Pop1 = 2.
        cJ1_JPIMMED_2_03,       // Index Size = 2, Pop1 = 3.

#ifdef JU_64BIT
        cJ1_JPIMMED_2_04,       // Index Size = 2, Pop1 = 4.
        cJ1_JPIMMED_2_05,       // Index Size = 2, Pop1 = 5.
        cJ1_JPIMMED_2_06,       // Index Size = 2, Pop1 = 6.
        cJ1_JPIMMED_2_07,       // Index Size = 2, Pop1 = 7.
#endif

        cJ1_JPIMMED_3_02,       // Index Size = 3, Pop1 = 2.

#ifdef JU_64BIT
        cJ1_JPIMMED_3_03,       // Index Size = 3, Pop1 = 3.
        cJ1_JPIMMED_3_04,       // Index Size = 3, Pop1 = 4.
        cJ1_JPIMMED_3_05,       // Index Size = 3, Pop1 = 5.

        cJ1_JPIMMED_4_02,       // Index Size = 4, Pop1 = 2.
        cJ1_JPIMMED_4_03,       // Index Size = 4, Pop1 = 3.

        cJ1_JPIMMED_5_02,       // Index Size = 5, Pop1 = 2.
        cJ1_JPIMMED_5_03,       // Index Size = 3, Pop1 = 3.

        cJ1_JPIMMED_6_02,       // Index Size = 6, Pop1 = 2.

        cJ1_JPIMMED_7_02,       // Index Size = 7, Pop1 = 2.
#endif

// This special Type is merely a sentinel for doing relative calculations.
// This value should not be used in switch statements (to avoid allocating code
// for it), which is also why it appears at the end of the enum list.

        cJ1_JPIMMED_CAP

} jp1_Type_t;


// RELATED VALUES:
//
// Index Size (state) for leaf JP, and JP type based on Index Size (state):

#ifndef JU_64BIT // 32-bit
#define J1_LEAFINDEXSIZE(jpType) ((jpType)    - cJ1_JPLEAF1 + 1)
#define J1_LEAFTYPE(IndexSize)   ((IndexSize) + cJ1_JPLEAF1 - 1)
#else
#define J1_LEAFINDEXSIZE(jpType) ((jpType)    - cJ1_JPLEAF2 + 2)
#define J1_LEAFTYPE(IndexSize)   ((IndexSize) + cJ1_JPLEAF2 - 2)
#endif


// ****************************************************************************
// JUDY1 POINTER (JP) -- RELATED MACROS AND CONSTANTS
// ****************************************************************************

// MAXIMUM POPULATIONS OF LINEAR LEAVES:
//
// Allow up to 2 cache lines per leaf, with N bytes per index.
//
// J_1_MAXB is the maximum number of bytes (sort of) to allocate per leaf.
// ALLOCSIZES is defined here, not there, for single-point control of these key
// definitions.  See JudyTables.c for "TERMINATOR".

#define J_1_MAXB   (sizeof(Word_t) * 32)
#define ALLOCSIZES { 3, 5, 7, 11, 15, 23, 32, 47, 64, TERMINATOR } // in words.
#define cJ1_LEAF1_MAXWORDS  5           // Leaf1 max alloc size in words.

// Under JRP (root-state leaves):
//
// Includes a count (Population) word.
//
// Under JP (non-root-state leaves), which have no count (Population) words:
//
// When a 1-byte index leaf grows above cJ1_LEAF1_MAXPOP1 Indexes (bytes),
// the memory chunk required grows to a size where a bitmap is just as
// efficient, so use a bitmap instead for all greater Populations, on both
// 32-bit and 64-bit systems.  However, on a 32-bit system this occurs upon
// going from 6 to 8 words (24 to 32 bytes) in the memory chunk, but on a
// 64-bit system this occurs upon going from 2 to 4 words (16 to 32 bytes).  It
// would be silly to go from a 15-Index Immediate JP to a 16-Index linear leaf
// to a 17-Index bitmap leaf, so just use a bitmap leaf for 16+ Indexes, which
// means set cJ1_LEAF1_MAXPOP1 to cJ1_IMMED1_MAXPOP1 (15) to cause the
// transition at that point.
//
// Note:  cJ1_LEAF1_MAXPOP1 is not used on 64-bit systems.

#ifndef JU_64BIT // 32-bit

#define cJ1_LEAF1_MAXPOP1    (cJ1_LEAF1_MAXWORDS * cJU_BYTESPERWORD)
#define cJ1_LEAF2_MAXPOP1    (J_1_MAXB / 2)
#define cJ1_LEAF3_MAXPOP1    (J_1_MAXB / 3)
#define cJ1_LEAFW_MAXPOP1    ((J_1_MAXB - cJU_BYTESPERWORD) / cJU_BYTESPERWORD)

#else // 64-bit

// #define cJ1_LEAF1_MAXPOP1                    // no LEAF1 in 64-bit.
#define cJ1_LEAF2_MAXPOP1    (J_1_MAXB / 2)
#define cJ1_LEAF3_MAXPOP1    (J_1_MAXB / 3)
#define cJ1_LEAF4_MAXPOP1    (J_1_MAXB / 4)
#define cJ1_LEAF5_MAXPOP1    (J_1_MAXB / 5)
#define cJ1_LEAF6_MAXPOP1    (J_1_MAXB / 6)
#define cJ1_LEAF7_MAXPOP1    (J_1_MAXB / 7)
#define cJ1_LEAFW_MAXPOP1    ((J_1_MAXB - cJU_BYTESPERWORD) / cJU_BYTESPERWORD)

#endif


// MAXIMUM POPULATIONS OF IMMEDIATE JPs:
//
// These specify the maximum Population of immediate JPs with various Index
// Sizes (== sizes of remaining undecoded Index bits).

#define cJ1_IMMED1_MAXPOP1  ((sizeof(jp_t) - 1) / 1)    // 7 [15].
#define cJ1_IMMED2_MAXPOP1  ((sizeof(jp_t) - 1) / 2)    // 3  [7].
#define cJ1_IMMED3_MAXPOP1  ((sizeof(jp_t) - 1) / 3)    // 2  [5].

#ifdef JU_64BIT
#define cJ1_IMMED4_MAXPOP1  ((sizeof(jp_t) - 1) / 4)    //    [3].
#define cJ1_IMMED5_MAXPOP1  ((sizeof(jp_t) - 1) / 5)    //    [3].
#define cJ1_IMMED6_MAXPOP1  ((sizeof(jp_t) - 1) / 6)    //    [2].
#define cJ1_IMMED7_MAXPOP1  ((sizeof(jp_t) - 1) / 7)    //    [2].
#endif


// ****************************************************************************
// JUDY1 BITMAP LEAF (J1LB) SUPPORT
// ****************************************************************************

#define J1_JLB_BITMAP(Pjlb,Subexp)  ((Pjlb)->j1lb_Bitmap[Subexp])

typedef struct J__UDY1_BITMAP_LEAF
{
        BITMAPL_t j1lb_Bitmap[cJU_NUMSUBEXPL];

} j1lb_t, * Pj1lb_t;


// ****************************************************************************
// MEMORY ALLOCATION SUPPORT
// ****************************************************************************

// ARRAY-GLOBAL INFORMATION:
//
// At the cost of an occasional additional cache fill, this object, which is
// pointed at by a JRP and in turn points to a JP_BRANCH*, carries array-global
// information about a Judy1 array that has sufficient population to amortize
// the cost.  The jpm_Pop0 field prevents having to add up the total population
// for the array in insert, delete, and count code.  The jpm_JP field prevents
// having to build a fake JP for entry to a state machine; however, the
// jp_DcdPopO field in jpm_JP, being one byte too small, is not used.
//
// Note:  Struct fields are ordered to keep "hot" data in the first 8 words
// (see left-margin comments) for machines with 8-word cache lines, and to keep
// sub-word fields together for efficient packing.

typedef struct J_UDY1_POPULATION_AND_MEMORY
{
/* 1 */ Word_t     jpm_Pop0;            // total population-1 in array.
/* 2 */ jp_t       jpm_JP;              // JP to first branch; see above.
/* 4 */ Word_t     jpm_LastUPop0;       // last jpm_Pop0 when convert to BranchU
// Note:  Field names match PJError_t for convenience in macros:
/* 7 */ char       je_Errno;            // one of the enums in Judy.h.
/* 7/8 */ int      je_ErrID;            // often an internal source line number.
/* 8/9 */ Word_t   jpm_TotalMemWords;   // words allocated in array.
} j1pm_t, *Pj1pm_t;


// TABLES FOR DETERMINING IF LEAVES HAVE ROOM TO GROW:
//
// These tables indicate if a given memory chunk can support growth of a given
// object into wasted (rounded-up) memory in the chunk.  This violates the
// hiddenness of the JudyMalloc code.
//
// Also define macros to hide the details in the code using these tables.

#ifndef JU_64BIT
extern const uint8_t j__1_Leaf1PopToWords[cJ1_LEAF1_MAXPOP1 + 1];
#endif
extern const uint8_t j__1_Leaf2PopToWords[cJ1_LEAF2_MAXPOP1 + 1];
extern const uint8_t j__1_Leaf3PopToWords[cJ1_LEAF3_MAXPOP1 + 1];
#ifdef JU_64BIT
extern const uint8_t j__1_Leaf4PopToWords[cJ1_LEAF4_MAXPOP1 + 1];
extern const uint8_t j__1_Leaf5PopToWords[cJ1_LEAF5_MAXPOP1 + 1];
extern const uint8_t j__1_Leaf6PopToWords[cJ1_LEAF6_MAXPOP1 + 1];
extern const uint8_t j__1_Leaf7PopToWords[cJ1_LEAF7_MAXPOP1 + 1];
#endif
extern const uint8_t j__1_LeafWPopToWords[cJ1_LEAFW_MAXPOP1 + 1];

// Check if increase of population will fit in same leaf:

#ifndef JU_64BIT
#define J1_LEAF1GROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAF1_MAXPOP1, j__1_Leaf1PopToWords)
#endif
#define J1_LEAF2GROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAF2_MAXPOP1, j__1_Leaf2PopToWords)
#define J1_LEAF3GROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAF3_MAXPOP1, j__1_Leaf3PopToWords)
#ifdef JU_64BIT
#define J1_LEAF4GROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAF4_MAXPOP1, j__1_Leaf4PopToWords)
#define J1_LEAF5GROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAF5_MAXPOP1, j__1_Leaf5PopToWords)
#define J1_LEAF6GROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAF6_MAXPOP1, j__1_Leaf6PopToWords)
#define J1_LEAF7GROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAF7_MAXPOP1, j__1_Leaf7PopToWords)
#endif
#define J1_LEAFWGROWINPLACE(Pop1) \
        J__U_GROWCK(Pop1, cJ1_LEAFW_MAXPOP1, j__1_LeafWPopToWords)

#ifndef JU_64BIT
#define J1_LEAF1POPTOWORDS(Pop1)  (j__1_Leaf1PopToWords[Pop1])
#endif
#define J1_LEAF2POPTOWORDS(Pop1)  (j__1_Leaf2PopToWords[Pop1])
#define J1_LEAF3POPTOWORDS(Pop1)  (j__1_Leaf3PopToWords[Pop1])
#ifdef JU_64BIT
#define J1_LEAF4POPTOWORDS(Pop1)  (j__1_Leaf4PopToWords[Pop1])
#define J1_LEAF5POPTOWORDS(Pop1)  (j__1_Leaf5PopToWords[Pop1])
#define J1_LEAF6POPTOWORDS(Pop1)  (j__1_Leaf6PopToWords[Pop1])
#define J1_LEAF7POPTOWORDS(Pop1)  (j__1_Leaf7PopToWords[Pop1])
#endif
#define J1_LEAFWPOPTOWORDS(Pop1)  (j__1_LeafWPopToWords[Pop1])


// FUNCTIONS TO ALLOCATE OBJECTS:

Pj1pm_t j__udy1AllocJ1PM(void);                         // constant size.

Pjbl_t  j__udy1AllocJBL(          Pj1pm_t);             // constant size.
Pjbb_t  j__udy1AllocJBB(          Pj1pm_t);             // constant size.
Pjp_t   j__udy1AllocJBBJP(Word_t, Pj1pm_t);
Pjbu_t  j__udy1AllocJBU(          Pj1pm_t);             // constant size.

#ifndef JU_64BIT
Pjll_t  j__udy1AllocJLL1( Word_t, Pj1pm_t);
#endif
Pjll_t  j__udy1AllocJLL2( Word_t, Pj1pm_t);
Pjll_t  j__udy1AllocJLL3( Word_t, Pj1pm_t);

#ifdef JU_64BIT
Pjll_t  j__udy1AllocJLL4( Word_t, Pj1pm_t);
Pjll_t  j__udy1AllocJLL5( Word_t, Pj1pm_t);
Pjll_t  j__udy1AllocJLL6( Word_t, Pj1pm_t);
Pjll_t  j__udy1AllocJLL7( Word_t, Pj1pm_t);
#endif

Pjlw_t  j__udy1AllocJLW(  Word_t         );             // no Pj1pm needed.
Pj1lb_t j__udy1AllocJLB1(         Pj1pm_t);             // constant size.


// FUNCTIONS TO FREE OBJECTS:

void    j__udy1FreeJ1PM( Pj1pm_t,        Pj1pm_t);      // constant size.

void    j__udy1FreeJBL(  Pjbl_t,         Pj1pm_t);      // constant size.
void    j__udy1FreeJBB(  Pjbb_t,         Pj1pm_t);      // constant size.
void    j__udy1FreeJBBJP(Pjp_t,  Word_t, Pj1pm_t);
void    j__udy1FreeJBU(  Pjbu_t,         Pj1pm_t);      // constant size.

#ifndef JU_64BIT
void    j__udy1FreeJLL1( Pjll_t, Word_t, Pj1pm_t);
#endif
void    j__udy1FreeJLL2( Pjll_t, Word_t, Pj1pm_t);
void    j__udy1FreeJLL3( Pjll_t, Word_t, Pj1pm_t);

#ifdef JU_64BIT
void    j__udy1FreeJLL4( Pjll_t, Word_t, Pj1pm_t);
void    j__udy1FreeJLL5( Pjll_t, Word_t, Pj1pm_t);
void    j__udy1FreeJLL6( Pjll_t, Word_t, Pj1pm_t);
void    j__udy1FreeJLL7( Pjll_t, Word_t, Pj1pm_t);
#endif

void    j__udy1FreeJLW(  Pjlw_t, Word_t, Pj1pm_t);
void    j__udy1FreeJLB1( Pj1lb_t,        Pj1pm_t);      // constant size.
void    j__udy1FreeSM(   Pjp_t,          Pj1pm_t);      // everything below Pjp.

#endif // ! _JUDY1_INCLUDED
