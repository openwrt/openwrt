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

// @(#) $Revision: 4.13 $ $Source: /judy/src/JudyCommon/JudyPrintJP.c $
//
// JudyPrintJP() debugging/tracing function for Judy1 or JudyL code.
// The caller should #include this file, with its static function (replicated
// in each compilation unit), in another *.c file, and compile with one of
// -DJUDY1 or -DJUDYL.
//
// The caller can set j__udyIndex and/or j__udyPopulation non-zero to have
// those values reported, and also to control trace-enabling (see below).
//
// Tracing is disabled by default unless one or both of two env parameters is
// set (regardless of value).  If either value is set but null or evaluates to
// zero, tracing is immediately enabled.  To disable tracing until a particular
// j__udy*Index value is seen, set STARTINDEX=<hex-index> in the env.  To
// disable it until a particular j__udy*Population value is seen, set
// STARTPOP=<decimal-population> in the env.  Once either condition is met,
// tracing "latches on".
//
// Example:
//
//      STARTPOP=0              // immediate tracing.
//      STARTINDEX=f35430a8     // not until one of these is met.
//      STARTPOP=1000000
//
// Note:  Trace-enabling does nothing unless the caller sets the appropriate
// global variable non-zero.

#if (! (defined(JUDY1) || defined(JUDYL)))
#error:  One of -DJUDY1 or -DJUDYL must be specified.
#endif

#include <stdlib.h>             // for getenv() and strtoul().


// GLOBALS FROM CALLER:
//
// Note:  This storage is declared once in each compilation unit that includes
// this file, but the linker should merge all cases into single locations, but
// ONLY if these are uninitialized, so ASSUME they are 0 to start.

Word_t j__udyIndex;             // current Index itself, optional from caller.
Word_t j__udyPopulation;        // Indexes in array, optional from caller.

// Other globals:

static Word_t startindex = 0;           // see usage below.
static Word_t startpop   = 0;
static bool_t enabled    = FALSE;       // by default, unless env params set.

// Shorthand for announcing JP addresses, Desc (in context), and JP types:
//
// Note:  Width is at least one blank wider than any JP type name, and the line
// is left unfinished.
//
// Note:  Use a format for address printing compatible with other tracing
// facilities; in particular, %x not %lx, to truncate the "noisy" high part on
// 64-bit systems.

#define JPTYPE(Type)  printf("0x%lx %s %-17s", (Word_t) Pjp, Desc, Type)

// Shorthands for announcing expanse populations from DcdPopO fields:

#define POP0 printf("Pop1 = 0 ")
#define POP1 printf("Pop1 = %ld ", (Word_t) ((JU_JPDCDPOP0(Pjp) &            0xff) + 1))
#define POP2 printf("Pop1 = %ld ", (Word_t) ((JU_JPDCDPOP0(Pjp) &          0xffff) + 1))
#define POP3 printf("Pop1 = %ld ", (Word_t) ((JU_JPDCDPOP0(Pjp) &        0xffffff) + 1))
#ifdef JU_64BIT
#define POP4 printf("Pop1 = %ld ", (Word_t) ((JU_JPDCDPOP0(Pjp) &       0xffffffff) + 1))
#define POP5 printf("Pop1 = %ld ", (Word_t) ((JU_JPDCDPOP0(Pjp) &     0xffffffffff) + 1))
#define POP6 printf("Pop1 = %ld ", (Word_t) ((JU_JPDCDPOP0(Pjp) &   0xffffffffffff) + 1))
#define POP7 printf("Pop1 = %ld ", (Word_t) ((JU_JPDCDPOP0(Pjp) & 0xffffffffffffff) + 1))
#endif

// Shorthands for announcing populations of Immeds:
//
// Note:  Line up the small populations that often occur together, but beyond
// that, dont worry about it because populations can get arbitrarily large.

#define POP_1   printf("Pop1 =  1 ")
#define POP_2   printf("Pop1 =  2 ")
#define POP_3   printf("Pop1 =  3 ")
#define POP_4   printf("Pop1 =  4 ")
#define POP_5   printf("Pop1 =  5 ")
#define POP_6   printf("Pop1 =  6 ")
#define POP_7   printf("Pop1 =  7 ")
#define POP_8   printf("Pop1 =  8 ")
#define POP_9   printf("Pop1 =  8 ")
#define POP_10  printf("Pop1 = 10 ")
#define POP_11  printf("Pop1 = 11 ")
#define POP_12  printf("Pop1 = 12 ")
#define POP_13  printf("Pop1 = 13 ")
#define POP_14  printf("Pop1 = 14 ")
#define POP_15  printf("Pop1 = 15 ")

// Shorthands for other announcements:

#define NUMJPSL printf("NumJPs = %d ", P_JBL(Pjp->jp_Addr)->jbl_NumJPs)
#define OOPS    printf("-- OOPS, invalid Type\n"); exit(1)

// This is harder to compute:

#define NUMJPSB                                                         \
        {                                                               \
            Pjbb_t Pjbb = P_JBB(Pjp->jp_Addr);                          \
            Word_t subexp;                                              \
            int    numJPs = 0;                                          \
                                                                        \
            for (subexp = 0; subexp < cJU_NUMSUBEXPB; ++subexp)         \
                numJPs += j__udyCountBitsB(JU_JBB_BITMAP(Pjbb, subexp));\
                                                                        \
            printf("NumJPs = %d ", numJPs);                             \
        }


// ****************************************************************************
// J U D Y   P R I N T   J P
//
// Dump information about a JP, at least its address, type, population, and
// number of JPs, as appropriate.  Error out upon any unexpected JP type.
//
// TBD:  Dump more detailed information about the JP?

FUNCTION static void JudyPrintJP(
        Pjp_t  Pjp,             // JP to describe.
        char * Desc,            // brief description of caller, such as "i".
        int    Line)            // callers source line number.
{
static  bool_t checked = FALSE; // set upon first entry and check for params.
        char * value;           // for getenv().


// CHECK FOR EXTERNAL ENABLING:
//
// If a parameter is set, report the value, even if it is null or otherwise
// evaluates to zero, in which case enable tracing immediately; otherwise wait
// for the value to be hit.

#define GETENV(Name,Value,Base)                                 \
        if ((value = getenv (Name)) != (char *) NULL)           \
        {                                                       \
            (Value) = strtoul (value, (char **) NULL, Base);    \
            enabled |= ((Value) == 0);  /* see above */         \
                                                                \
            (void) printf ("JudyPrintJP(\"%s\"): $%s = %lu\n",  \
                           Desc, Name, Value);                  \
        }

        if (! checked)  // only check once.
        {
            checked = TRUE;

            GETENV ("STARTINDEX", startindex, 16);
            GETENV ("STARTPOP",   startpop,   10);

            (void) printf ("JudyPrintJP(\"%s\"): Tracing present %s\n", Desc,
                           enabled ? "and immediately enabled" :
                           (startindex || startpop) ?
                           "but disabled until start condition met" :
                           "but not enabled by env parameter");
        }

        if (! enabled)  // check repeatedly until latched enabled:
        {
            if (startindex && (startindex == j__udyIndex))
            {
                 (void) printf ("=== TRACING ENABLED (\"%s\"), "
                                "startindex = 0x%lx\n", Desc, startindex);
                 enabled = TRUE;
            }
            else if (startpop && (startpop == j__udyPopulation))
            {
                 (void) printf ("=== TRACING ENABLED (\"%s\"), "
                                "startpop = %lu\n", Desc, startpop);
                 enabled = TRUE;
            }
            else
            {
                return;         // print nothing this time.
            }
        }


// SWITCH ON JP TYPE:

        switch (JU_JPTYPE(Pjp))
        {

// Note:  The following COULD be merged more tightly between Judy1 and JudyL,
// but we decided that the output should say cJ1*/cJL*, not cJU*, to be more
// specific.

#ifdef JUDY1
        case cJ1_JPNULL1:       JPTYPE("cJ1_JPNULL1"); POP0;            break;
        case cJ1_JPNULL2:       JPTYPE("cJ1_JPNULL2"); POP0;            break;
        case cJ1_JPNULL3:       JPTYPE("cJ1_JPNULL3"); POP0;            break;
#ifdef JU_64BIT
        case cJ1_JPNULL4:       JPTYPE("cJ1_JPNULL4"); POP0;            break;
        case cJ1_JPNULL5:       JPTYPE("cJ1_JPNULL5"); POP0;            break;
        case cJ1_JPNULL6:       JPTYPE("cJ1_JPNULL6"); POP0;            break;
        case cJ1_JPNULL7:       JPTYPE("cJ1_JPNULL7"); POP0;            break;
#endif

        case cJ1_JPBRANCH_L2:   JPTYPE("cJ1_JPBRANCH_L2"); POP2;NUMJPSL;break;
        case cJ1_JPBRANCH_L3:   JPTYPE("cJ1_JPBRANCH_L3"); POP3;NUMJPSL;break;
#ifdef JU_64BIT
        case cJ1_JPBRANCH_L4:   JPTYPE("cJ1_JPBRANCH_L4"); POP4;NUMJPSL;break;
        case cJ1_JPBRANCH_L5:   JPTYPE("cJ1_JPBRANCH_L5"); POP5;NUMJPSL;break;
        case cJ1_JPBRANCH_L6:   JPTYPE("cJ1_JPBRANCH_L6"); POP6;NUMJPSL;break;
        case cJ1_JPBRANCH_L7:   JPTYPE("cJ1_JPBRANCH_L7"); POP7;NUMJPSL;break;
#endif
        case cJ1_JPBRANCH_L:    JPTYPE("cJ1_JPBRANCH_L");       NUMJPSL;break;

        case cJ1_JPBRANCH_B2:   JPTYPE("cJ1_JPBRANCH_B2"); POP2;NUMJPSB;break;
        case cJ1_JPBRANCH_B3:   JPTYPE("cJ1_JPBRANCH_B3"); POP3;NUMJPSB;break;
#ifdef JU_64BIT
        case cJ1_JPBRANCH_B4:   JPTYPE("cJ1_JPBRANCH_B4"); POP4;NUMJPSB;break;
        case cJ1_JPBRANCH_B5:   JPTYPE("cJ1_JPBRANCH_B5"); POP5;NUMJPSB;break;
        case cJ1_JPBRANCH_B6:   JPTYPE("cJ1_JPBRANCH_B6"); POP6;NUMJPSB;break;
        case cJ1_JPBRANCH_B7:   JPTYPE("cJ1_JPBRANCH_B7"); POP7;NUMJPSB;break;
#endif
        case cJ1_JPBRANCH_B:    JPTYPE("cJ1_JPBRANCH_B");       NUMJPSB;break;

        case cJ1_JPBRANCH_U2:   JPTYPE("cJ1_JPBRANCH_U2"); POP2;        break;
        case cJ1_JPBRANCH_U3:   JPTYPE("cJ1_JPBRANCH_U3"); POP3;        break;
#ifdef JU_64BIT
        case cJ1_JPBRANCH_U4:   JPTYPE("cJ1_JPBRANCH_U4"); POP4;        break;
        case cJ1_JPBRANCH_U5:   JPTYPE("cJ1_JPBRANCH_U5"); POP5;        break;
        case cJ1_JPBRANCH_U6:   JPTYPE("cJ1_JPBRANCH_U6"); POP6;        break;
        case cJ1_JPBRANCH_U7:   JPTYPE("cJ1_JPBRANCH_U7"); POP7;        break;
#endif
        case cJ1_JPBRANCH_U:    JPTYPE("cJ1_JPBRANCH_U");               break;

#ifndef JU_64BIT
        case cJ1_JPLEAF1:       JPTYPE("cJ1_JPLEAF1"); POP1;            break;
#endif
        case cJ1_JPLEAF2:       JPTYPE("cJ1_JPLEAF2"); POP2;            break;
        case cJ1_JPLEAF3:       JPTYPE("cJ1_JPLEAF3"); POP3;            break;
#ifdef JU_64BIT
        case cJ1_JPLEAF4:       JPTYPE("cJ1_JPLEAF4"); POP4;            break;
        case cJ1_JPLEAF5:       JPTYPE("cJ1_JPLEAF5"); POP5;            break;
        case cJ1_JPLEAF6:       JPTYPE("cJ1_JPLEAF6"); POP6;            break;
        case cJ1_JPLEAF7:       JPTYPE("cJ1_JPLEAF7"); POP7;            break;
#endif

        case cJ1_JPLEAF_B1:     JPTYPE("cJ1_JPLEAF_B1");    POP1;       break;
        case cJ1_JPFULLPOPU1:   JPTYPE("cJ1_JPFULLPOPU1");  POP1;       break;

        case cJ1_JPIMMED_1_01:  JPTYPE("cJ1_JPIMMED_1_01"); POP_1;      break;
        case cJ1_JPIMMED_2_01:  JPTYPE("cJ1_JPIMMED_2_01"); POP_1;      break;
        case cJ1_JPIMMED_3_01:  JPTYPE("cJ1_JPIMMED_3_01"); POP_1;      break;
#ifdef JU_64BIT
        case cJ1_JPIMMED_4_01:  JPTYPE("cJ1_JPIMMED_4_01"); POP_1;      break;
        case cJ1_JPIMMED_5_01:  JPTYPE("cJ1_JPIMMED_5_01"); POP_1;      break;
        case cJ1_JPIMMED_6_01:  JPTYPE("cJ1_JPIMMED_6_01"); POP_1;      break;
        case cJ1_JPIMMED_7_01:  JPTYPE("cJ1_JPIMMED_7_01"); POP_1;      break;
#endif

        case cJ1_JPIMMED_1_02:  JPTYPE("cJ1_JPIMMED_1_02"); POP_2;      break;
        case cJ1_JPIMMED_1_03:  JPTYPE("cJ1_JPIMMED_1_03"); POP_3;      break;
        case cJ1_JPIMMED_1_04:  JPTYPE("cJ1_JPIMMED_1_04"); POP_4;      break;
        case cJ1_JPIMMED_1_05:  JPTYPE("cJ1_JPIMMED_1_05"); POP_5;      break;
        case cJ1_JPIMMED_1_06:  JPTYPE("cJ1_JPIMMED_1_06"); POP_6;      break;
        case cJ1_JPIMMED_1_07:  JPTYPE("cJ1_JPIMMED_1_07"); POP_7;      break;
#ifdef JU_64BIT
        case cJ1_JPIMMED_1_08:  JPTYPE("cJ1_JPIMMED_1_08"); POP_8;      break;
        case cJ1_JPIMMED_1_09:  JPTYPE("cJ1_JPIMMED_1_09"); POP_9;      break;
        case cJ1_JPIMMED_1_10:  JPTYPE("cJ1_JPIMMED_1_10"); POP_10;     break;
        case cJ1_JPIMMED_1_11:  JPTYPE("cJ1_JPIMMED_1_11"); POP_11;     break;
        case cJ1_JPIMMED_1_12:  JPTYPE("cJ1_JPIMMED_1_12"); POP_12;     break;
        case cJ1_JPIMMED_1_13:  JPTYPE("cJ1_JPIMMED_1_13"); POP_13;     break;
        case cJ1_JPIMMED_1_14:  JPTYPE("cJ1_JPIMMED_1_14"); POP_14;     break;
        case cJ1_JPIMMED_1_15:  JPTYPE("cJ1_JPIMMED_1_15"); POP_15;     break;
#endif
        case cJ1_JPIMMED_2_02:  JPTYPE("cJ1_JPIMMED_2_02"); POP_2;      break;
        case cJ1_JPIMMED_2_03:  JPTYPE("cJ1_JPIMMED_2_03"); POP_3;      break;
#ifdef JU_64BIT
        case cJ1_JPIMMED_2_04:  JPTYPE("cJ1_JPIMMED_2_04"); POP_4;      break;
        case cJ1_JPIMMED_2_05:  JPTYPE("cJ1_JPIMMED_2_05"); POP_5;      break;
        case cJ1_JPIMMED_2_06:  JPTYPE("cJ1_JPIMMED_2_06"); POP_6;      break;
        case cJ1_JPIMMED_2_07:  JPTYPE("cJ1_JPIMMED_2_07"); POP_7;      break;
#endif

        case cJ1_JPIMMED_3_02:  JPTYPE("cJ1_JPIMMED_3_02"); POP_2;      break;
#ifdef JU_64BIT
        case cJ1_JPIMMED_3_03:  JPTYPE("cJ1_JPIMMED_3_03"); POP_3;      break;
        case cJ1_JPIMMED_3_04:  JPTYPE("cJ1_JPIMMED_3_04"); POP_4;      break;
        case cJ1_JPIMMED_3_05:  JPTYPE("cJ1_JPIMMED_3_05"); POP_5;      break;
        case cJ1_JPIMMED_4_02:  JPTYPE("cJ1_JPIMMED_4_02"); POP_2;      break;
        case cJ1_JPIMMED_4_03:  JPTYPE("cJ1_JPIMMED_4_03"); POP_3;      break;
        case cJ1_JPIMMED_5_02:  JPTYPE("cJ1_JPIMMED_5_02"); POP_2;      break;
        case cJ1_JPIMMED_5_03:  JPTYPE("cJ1_JPIMMED_5_03"); POP_3;      break;
        case cJ1_JPIMMED_6_02:  JPTYPE("cJ1_JPIMMED_6_02"); POP_2;      break;
        case cJ1_JPIMMED_7_02:  JPTYPE("cJ1_JPIMMED_7_02"); POP_2;      break;
#endif
        case cJ1_JPIMMED_CAP:   JPTYPE("cJ1_JPIMMED_CAP");              OOPS;

#else // JUDYL ===============================================================

        case cJL_JPNULL1:       JPTYPE("cJL_JPNULL1"); POP0;            break;
        case cJL_JPNULL2:       JPTYPE("cJL_JPNULL2"); POP0;            break;
        case cJL_JPNULL3:       JPTYPE("cJL_JPNULL3"); POP0;            break;
#ifdef JU_64BIT
        case cJL_JPNULL4:       JPTYPE("cJL_JPNULL4"); POP0;            break;
        case cJL_JPNULL5:       JPTYPE("cJL_JPNULL5"); POP0;            break;
        case cJL_JPNULL6:       JPTYPE("cJL_JPNULL6"); POP0;            break;
        case cJL_JPNULL7:       JPTYPE("cJL_JPNULL7"); POP0;            break;
#endif

        case cJL_JPBRANCH_L2:   JPTYPE("cJL_JPBRANCH_L2"); POP2;NUMJPSL;break;
        case cJL_JPBRANCH_L3:   JPTYPE("cJL_JPBRANCH_L3"); POP3;NUMJPSL;break;
#ifdef JU_64BIT
        case cJL_JPBRANCH_L4:   JPTYPE("cJL_JPBRANCH_L4"); POP4;NUMJPSL;break;
        case cJL_JPBRANCH_L5:   JPTYPE("cJL_JPBRANCH_L5"); POP5;NUMJPSL;break;
        case cJL_JPBRANCH_L6:   JPTYPE("cJL_JPBRANCH_L6"); POP6;NUMJPSL;break;
        case cJL_JPBRANCH_L7:   JPTYPE("cJL_JPBRANCH_L7"); POP7;NUMJPSL;break;
#endif
        case cJL_JPBRANCH_L:    JPTYPE("cJL_JPBRANCH_L");       NUMJPSL;break;

        case cJL_JPBRANCH_B2:   JPTYPE("cJL_JPBRANCH_B2"); POP2;NUMJPSB;break;
        case cJL_JPBRANCH_B3:   JPTYPE("cJL_JPBRANCH_B3"); POP3;NUMJPSB;break;
#ifdef JU_64BIT
        case cJL_JPBRANCH_B4:   JPTYPE("cJL_JPBRANCH_B4"); POP4;NUMJPSB;break;
        case cJL_JPBRANCH_B5:   JPTYPE("cJL_JPBRANCH_B5"); POP5;NUMJPSB;break;
        case cJL_JPBRANCH_B6:   JPTYPE("cJL_JPBRANCH_B6"); POP6;NUMJPSB;break;
        case cJL_JPBRANCH_B7:   JPTYPE("cJL_JPBRANCH_B7"); POP7;NUMJPSB;break;
#endif
        case cJL_JPBRANCH_B:    JPTYPE("cJL_JPBRANCH_B");       NUMJPSB;break;

        case cJL_JPBRANCH_U2:   JPTYPE("cJL_JPBRANCH_U2"); POP2;        break;
        case cJL_JPBRANCH_U3:   JPTYPE("cJL_JPBRANCH_U3"); POP3;        break;
#ifdef JU_64BIT
        case cJL_JPBRANCH_U4:   JPTYPE("cJL_JPBRANCH_U4"); POP4;        break;
        case cJL_JPBRANCH_U5:   JPTYPE("cJL_JPBRANCH_U5"); POP5;        break;
        case cJL_JPBRANCH_U6:   JPTYPE("cJL_JPBRANCH_U6"); POP6;        break;
        case cJL_JPBRANCH_U7:   JPTYPE("cJL_JPBRANCH_U7"); POP7;        break;
#endif
        case cJL_JPBRANCH_U:    JPTYPE("cJL_JPBRANCH_U");               break;

        case cJL_JPLEAF1:       JPTYPE("cJL_JPLEAF1"); POP1;            break;
        case cJL_JPLEAF2:       JPTYPE("cJL_JPLEAF2"); POP2;            break;
        case cJL_JPLEAF3:       JPTYPE("cJL_JPLEAF3"); POP3;            break;
#ifdef JU_64BIT
        case cJL_JPLEAF4:       JPTYPE("cJL_JPLEAF4"); POP4;            break;
        case cJL_JPLEAF5:       JPTYPE("cJL_JPLEAF5"); POP5;            break;
        case cJL_JPLEAF6:       JPTYPE("cJL_JPLEAF6"); POP6;            break;
        case cJL_JPLEAF7:       JPTYPE("cJL_JPLEAF7"); POP7;            break;
#endif

        case cJL_JPLEAF_B1:     JPTYPE("cJL_JPLEAF_B1"); POP1;  break;

        case cJL_JPIMMED_1_01:  JPTYPE("cJL_JPIMMED_1_01"); POP_1;      break;
        case cJL_JPIMMED_2_01:  JPTYPE("cJL_JPIMMED_2_01"); POP_1;      break;
        case cJL_JPIMMED_3_01:  JPTYPE("cJL_JPIMMED_3_01"); POP_1;      break;
#ifdef JU_64BIT
        case cJL_JPIMMED_4_01:  JPTYPE("cJL_JPIMMED_4_01"); POP_1;      break;
        case cJL_JPIMMED_5_01:  JPTYPE("cJL_JPIMMED_5_01"); POP_1;      break;
        case cJL_JPIMMED_6_01:  JPTYPE("cJL_JPIMMED_6_01"); POP_1;      break;
        case cJL_JPIMMED_7_01:  JPTYPE("cJL_JPIMMED_7_01"); POP_1;      break;
#endif

        case cJL_JPIMMED_1_02:  JPTYPE("cJL_JPIMMED_1_02"); POP_2;      break;
        case cJL_JPIMMED_1_03:  JPTYPE("cJL_JPIMMED_1_03"); POP_3;      break;
#ifdef JU_64BIT
        case cJL_JPIMMED_1_04:  JPTYPE("cJL_JPIMMED_1_04"); POP_4;      break;
        case cJL_JPIMMED_1_05:  JPTYPE("cJL_JPIMMED_1_05"); POP_5;      break;
        case cJL_JPIMMED_1_06:  JPTYPE("cJL_JPIMMED_1_06"); POP_6;      break;
        case cJL_JPIMMED_1_07:  JPTYPE("cJL_JPIMMED_1_07"); POP_7;      break;
        case cJL_JPIMMED_2_02:  JPTYPE("cJL_JPIMMED_2_02"); POP_2;      break;
        case cJL_JPIMMED_2_03:  JPTYPE("cJL_JPIMMED_2_03"); POP_3;      break;
        case cJL_JPIMMED_3_02:  JPTYPE("cJL_JPIMMED_3_02"); POP_2;      break;
#endif
        case cJL_JPIMMED_CAP:   JPTYPE("cJL_JPIMMED_CAP");      OOPS;

#endif // JUDYL

        default:  printf("Unknown Type = %d", JU_JPTYPE(Pjp));          OOPS;
        }

        if (j__udyIndex)        printf("Index = 0x%lx", j__udyIndex);
        if (j__udyPopulation)   printf("Pop = %lu",     j__udyPopulation);

        printf("line = %d\n", Line);

} // JudyPrintJP()
