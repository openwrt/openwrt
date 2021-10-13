// @(#) $Revision: 4.20 $ $Source: /judy/test/manual/Judy1LTime.c $
//=======================================================================
//      This program measures the performance of a Judy1 and JudyL Array.
//                      -by- 
//      Douglas L. Baskins (8/2001)  doug@sourcejudy.com
//=======================================================================

#include <unistd.h>             // sbrk()
#include <stdlib.h>             // exit()
#include <stdio.h>              // printf()
#include <math.h>               // pow()
#include <sys/time.h>           // gettimeofday()
#include <Judy.h>               // for Judy macros J*()

#ifdef NOINLINE                 /* this is the 21st century? */
#define _INLINE_ static
#else
#define _INLINE_ inline
#endif


//=======================================================================
//      C O M P I L E D:
//=======================================================================
//
//      cc -static -O3 Judy1LTime.c -lJudy -lm
//
//  the -static is for a little better performace on some platforms
//
//  if optional high-resolution timers are desired:
//
//      cc -static -O3 -DJU_LINUX_IA32 Judy1LTime.c -lJudy -lm
//
//  and read below:
//  
//=======================================================================
//      T I M I N G   M A C R O S
//=======================================================================
// if your machine is one of the supported types in the following header
// file then uncomment this corresponding to what the header file says.
// This will give very high timing resolution.
//
// #define JU_xxxxxxx 1         // read timeit.h
// #define JU_LINUX_IA32 1      // I.E. IA32 Linux
//
#include "timeit.h"             // optional for high resolution times

double    DeltaUSec;            // Global for remembering delta times

#ifndef _TIMEIT_H

// Note: I have found some Linux systems (2.4.18-6mdk) have bugs in the 
// gettimeofday() routine.  Sometimes the difference of two consective calls 
// returns a negative ~2840 microseconds instead of 0 or 1.  If you use the 
// above #include "timeit.h" and compile with timeit.c and use 
// -DJU_LINUX_IA32, that problem will be eliminated.  This is because for 
// delta times less than .1 sec, the hardware free running timer is used 
// instead of gettimeofday().  I have found the negative time problem
// appears about 40-50 times per second with numerous gettimeofday() calls.
// You should just ignore negative times output.

#define TIMER_vars(T) struct timeval __TVBeg_##T, __TVEnd_##T

#define STARTTm(T) gettimeofday(&__TVBeg_##T, NULL)

#define ENDTm(D,T)                                                      \
{                                                                       \
    gettimeofday(&__TVEnd_##T, NULL);                                   \
    (D) = (double)(__TVEnd_##T.tv_sec  - __TVBeg_##T.tv_sec) * 1E6 +    \
         ((double)(__TVEnd_##T.tv_usec - __TVBeg_##T.tv_usec));         \
}

#endif // _TIMEIT_H

//=======================================================================
//      M E M O R Y   S I Z E   M A C R O S
//=======================================================================
//      Most mallocs have mallinfo()
//      However, the size is an int, so just about worthless in 64 bit
//      machines with more than 4Gb ram.  But needed on 32 bit machines 
//      that have more than a 1Gbyte of memory, because malloc stops
//      using sbrk() about at that time (runs out of heap -- use mmap()).

// un-define this if your malloc has mallinfo(); see above

#define NOMALLINFO 1

double    DeltaMem;

#ifndef NOMALLINFO

#include <malloc.h>             // mallinfo()

struct mallinfo malStart;

#define STARTmem malStart = mallinfo() /* works with some mallocs */
#define ENDmem                                                  \
{                                                               \
    struct mallinfo malEnd = mallinfo();                        \
/* strange little dance from signed to unsigned to double */    \
    unsigned int _un_int = malEnd.arena - malStart.arena;       \
    DeltaMem = _un_int;      /* to double */                    \
}

#else // MALLINFO

// this usually works for machines with less than 1-2Gb RAM.
// (it does not include memory ACQUIRED by mmap())

char     *malStart;

#define STARTmem (malStart = (char *)sbrk(0))
#define ENDmem                                                  \
{                                                               \
    char  *malEnd =  (char *)sbrk(0);                           \
    DeltaMem = malEnd - malStart;                               \
}

#endif // MALLINFO

//=======================================================================

// Common macro to handle a failure
#define FAILURE(STR, UL)                                                \
{                                                                       \
printf(         "Error: %s %lu, file='%s', 'function='%s', line %d\n",  \
        STR, UL, __FILE__, __FUNCTI0N__, __LINE__);                     \
fprintf(stderr, "Error: %s %lu, file='%s', 'function='%s', line %d\n",  \
        STR, UL, __FILE__, __FUNCTI0N__, __LINE__);                     \
        exit(1);                                                        \
}

// Interations without improvement
//  Minimum of 2 loops, maximum of 1000000
#define MINLOOPS 2
#define MAXLOOPS 1000000

// Maximum or 10 loops with no improvement
#define ICNT 10

// Structure to keep track of times
typedef struct MEASUREMENTS_STRUCT
{
    Word_t    ms_delta;
}
ms_t     , *Pms_t;

// Specify prototypes for each test routine
int       NextNumb(Word_t *PNumber, double *PDNumb, double DMult,
                   Word_t MaxN);

Word_t    TestJudyIns(void **J1, void **JL, Word_t Seed, Word_t Elems);

Word_t    TestJudyDup(void **J1, void **JL, Word_t Seed, Word_t Elems);

int       TestJudyDel(void **J1, void **JL, Word_t Seed, Word_t Elems);

Word_t    TestJudyGet(void *J1, void *JL, Word_t Seed, Word_t Elems);

int       TestJudyCount(void *J1, void *JL, Word_t LowIndex, Word_t Elems);

Word_t    TestJudyNext(void *J1, void *JL, Word_t LowIndex, Word_t Elems);

int       TestJudyPrev(void *J1, void *JL, Word_t HighIndex, Word_t Elems);

Word_t    TestJudyNextEmpty(void *J1, void *JL, Word_t LowIndex,
                            Word_t Elems);

Word_t    TestJudyPrevEmpty(void *J1, void *JL, Word_t HighIndex,
                            Word_t Elems);

//=======================================================================
// These are LFSF feedback taps for bitwidths of 10..64 sized numbers.
// Tested with Seed=0xc1fc to 35 billion numbers
//=======================================================================

Word_t    StartSeed = 0xc1fc;   // default beginning number
Word_t    FirstSeed;

Word_t    MagicList[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0..9
    0x27f,                      // 10
    0x27f,                      // 11
    0x27f,                      // 12
    0x27f,                      // 13
    0x27f,                      // 14
    0x27f,                      // 15
    0x1e71,                     // 16
    0xdc0b,                     // 17
    0xdc0b,                     // 18
    0xdc0b,                     // 19
    0xdc0b,                     // 20
    0xc4fb,                     // 21
    0xc4fb,                     // 22
    0xc4fb,                     // 23
    0x13aab,                    // 24 
    0x11ca3,                    // 25
    0x11ca3,                    // 26
    0x11ca3,                    // 27
    0x13aab,                    // 28
    0x11ca3,                    // 29
    0xc4fb,                     // 30
    0xc4fb,                     // 31
    0x13aab,                    // 32 
    0x14e73,                    // 33  
    0x145d7,                    // 34  
    0x145f9,                    // 35  following tested with Seed=0xc1fc to 35 billion numbers
    0x151ed,                    // 36 .. 41 
    0x151ed,                    // 37  
    0x151ed,                    // 38  
    0x151ed,                    // 39  
    0x151ed,                    // 40  
    0x146c3,                    // 41 .. 64 
    0x146c3,                    // 42  
    0x146c3,                    // 43  
    0x146c3,                    // 44  
    0x146c3,                    // 45  
    0x146c3,                    // 46  
    0x146c3,                    // 47  
    0x146c3,                    // 48  
    0x146c3,                    // 49  
    0x146c3,                    // 50  
    0x146c3,                    // 51  
    0x146c3,                    // 52  
    0x146c3,                    // 53  
    0x146c3,                    // 54  
    0x146c3,                    // 55  
    0x146c3,                    // 56  
    0x146c3,                    // 57  
    0x146c3,                    // 58  
    0x146c3,                    // 59  
    0x146c3,                    // 60  
    0x146c3,                    // 61  
    0x146c3,                    // 62  
    0x146c3,                    // 63  
    0x146c3                     // 64  
};

// Routine to "mirror" the input data word
static Word_t
Swizzle(Word_t word)
{
// BIT REVERSAL, Ron Gutman in Dr. Dobb's Journal, #316, Sept 2000, pp133-136
//

#ifdef __LP64__
    word = ((word & 0x00000000ffffffff) << 32) |
        ((word & 0xffffffff00000000) >> 32);
    word = ((word & 0x0000ffff0000ffff) << 16) |
        ((word & 0xffff0000ffff0000) >> 16);
    word = ((word & 0x00ff00ff00ff00ff) << 8) |
        ((word & 0xff00ff00ff00ff00) >> 8);
    word = ((word & 0x0f0f0f0f0f0f0f0f) << 4) |
        ((word & 0xf0f0f0f0f0f0f0f0) >> 4);
    word = ((word & 0x3333333333333333) << 2) |
        ((word & 0xcccccccccccccccc) >> 2);
    word = ((word & 0x5555555555555555) << 1) |
        ((word & 0xaaaaaaaaaaaaaaaa) >> 1);
#else // not __LP64__
    word = ((word & 0x0000ffff) << 16) | ((word & 0xffff0000) >> 16);
    word = ((word & 0x00ff00ff) << 8) | ((word & 0xff00ff00) >> 8);
    word = ((word & 0x0f0f0f0f) << 4) | ((word & 0xf0f0f0f0) >> 4);
    word = ((word & 0x33333333) << 2) | ((word & 0xcccccccc) >> 2);
    word = ((word & 0x55555555) << 1) | ((word & 0xaaaaaaaa) >> 1);
#endif // not __LP64__

    return (word);
}

double    DeltaUSec1 = 0.0;     // Global for measuring delta times
double    DeltaUSecL = 0.0;     // Global for measuring delta times

Word_t    J1Flag = 0;           // time Judy1
Word_t    JLFlag = 0;           // time JudyL
Word_t    dFlag = 0;            // time Judy1Unset JudyLDel
Word_t    vFlag = 0;            // time Searching 
Word_t    CFlag = 0;            // time Counting
Word_t    IFlag = 0;            // time duplicate inserts/sets
Word_t    DFlag = 0;            // bit reverse the data stream
Word_t    lFlag = 0;            // do not do multi-insert tests
Word_t    aFlag = 0;            // output active memory in array
Word_t    SkipN = 0;            // default == Random skip
Word_t    TValues = 100000;     // Maximum retrieve tests for timing
Word_t    nElms = 1000000;      // Max population of arrays
Word_t    ErrorFlag = 0;
Word_t    PtsPdec = 40;         // measurement points per decade

// Stuff for LFSR (pseudo random number generator)
Word_t    RandomBit = ~0UL / 2 + 1;
Word_t    BValue = sizeof(Word_t) * 8;
Word_t    Magic;

// for error routines -- notice misspelling, name conflicts with some compilers 
#undef __FUNCTI0N__
#define __FUNCTI0N__ "Random"

_INLINE_ Word_t                 // so INLINING compilers get to look at it.
Random(Word_t newseed)
{
    if (newseed & RandomBit)
    {
        newseed += newseed;
        newseed ^= Magic;
    }
    else
    {
        newseed += newseed;
    }
    newseed &= RandomBit * 2 - 1;
    if (newseed == FirstSeed)
        FAILURE("LFSR failed", newseed);
    return (newseed);
}

_INLINE_ Word_t                 // so INLINING compilers get to look at it.
GetNextIndex(Word_t Index)
{
    if (SkipN)
        Index += SkipN;
    else
        Index = Random(Index);

    return (Index);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "main"

int
main(int argc, char *argv[])
{
//  Names of Judy Arrays
    void     *J1 = NULL;        // Judy1
    void     *JL = NULL;        // JudyL

    TIMER_vars(tm1);            // declare timer variables
    Word_t    Count1, CountL;
    Word_t    Bytes;

    double    Mult;
    Pms_t     Pms;
    Word_t    Seed;
    Word_t    PtsPdec = 40;     // points per decade
    Word_t    Groups;           // Number of measurement groups
    Word_t    grp;
    Word_t    Pop1;
    Word_t    Meas;
    int       Col;

    int       c;
    extern char *optarg;

//============================================================
// PARSE INPUT PARAMETERS
//============================================================

    while ((c = getopt(argc, argv, "n:S:T:P:b:B:dDC1LvIla")) != -1)
    {
        switch (c)
        {
        case 'n':              // Max population of arrays
            nElms = strtoul(optarg, NULL, 0); // Size of Linear Array
            if (nElms == 0)
                FAILURE("No tests: -n", nElms);

//          Check if more than a trillion (64 bit only)
            if ((double)nElms > 1e12)
                FAILURE("Too many Indexes=", nElms);
            break;

        case 'S':              // Step Size, 0 == Random
            SkipN = strtoul(optarg, NULL, 0);
            break;

        case 'T':              // Maximum retrieve tests for timing 
            TValues = strtoul(optarg, NULL, 0);
            break;

        case 'P':              // measurement points per decade
            PtsPdec = strtoul(optarg, NULL, 0);
            break;

        case 'b':              // May not work past 35 bits if changed
            StartSeed = strtoul(optarg, NULL, 0);
            break;

        case 'B':              // expanse of data points (random only)
            BValue = strtoul(optarg, NULL, 0);
            if ((BValue > 64)
                ||
                (MagicList[BValue] == 0) || (BValue > (sizeof(Word_t) * 8)))
            {
                ErrorFlag++;
                printf("\nIllegal number of random bits of %lu !!!\n",
                       BValue);
            }
            break;

        case 'v':
            vFlag = 1;         // time Searching
            break;

        case '1':              // time Judy1
            J1Flag = 1;
            break;

        case 'L':              // time JudyL
            JLFlag = 1;
            break;

        case 'd':              // time Judy1Unset JudyLDel
            dFlag = 1;
            break;

        case 'D':              // bit reverse the data stream
            DFlag = 1;
            break;

        case 'C':              // time Counting
            CFlag = 1;
            break;

        case 'I':              // time duplicate insert/set
            IFlag = 1;
            break;

        case 'l':              // do not loop in tests
            lFlag = 1;
            break;

        case 'a':              // output active memory in Judy array
            aFlag = 1;
            break;

        default:
            ErrorFlag++;
            break;
        }
    }

    if (ErrorFlag)
    {
        printf("\n%s -n# -P# -S# -B# -T# -b # -DCpdI\n\n", argv[0]);
        printf("Where:\n");
        printf("-n <#>  number of indexes used in tests\n");
        printf("-P <#>  number measurement points per decade\n");
        printf("-S <#>  index skip amount, 0 = random\n");
        printf("-B <#>  # bits (10..%d) in random number generator\n",
               (int)sizeof(Word_t) * 8);
        printf("-L      time JudyL\n");
        printf("-1      time Judy1\n");
        printf("-I      time DUPLICATE Ins/Set times\n");
        printf("-C      time JudyCount tests\n");
        printf("-v      time Judy Search tests\n");
        printf("-d      time JudyDel/Unset instead of JudyFreeArray\n");
        printf("-l      do not loop on same Indexes\n");
        printf("-T <#>  max number indexes in read times - 0 == MAX\n");
        printf("\n");

        exit(1);
    }

//  If none, then both
    if (!JLFlag && !J1Flag)
        JLFlag = J1Flag = 1;

//  Set number of Random bits in LFSR
    RandomBit = 1UL << (BValue - 1);
    Magic = MagicList[BValue];

    if (nElms > ((RandomBit - 2) * 2))
    {
        printf
            ("# Number = -n%lu of Indexes reduced to max expanse of Random numbers\n",
             nElms);
        nElms = ((RandomBit - 2) * 2);
    }

    printf("# TITLE %s -n%lu -S%lu -T%lu -B%lu -P%lu",
           argv[0], nElms, SkipN, TValues, BValue, PtsPdec);
    if (J1Flag)
        printf(" -1");
    if (JLFlag)
        printf(" -L");
    if (DFlag)
        printf(" -D");
    if (dFlag)
        printf(" -d");
    if (CFlag)
        printf(" -C");
    if (IFlag)
        printf(" -I");
    if (lFlag)
        printf(" -l");
    if (aFlag)
        printf(" -a");
    printf("\n");

    if (sizeof(Word_t) == 8)
        printf("#%s 64 Bit version\n", argv[0]);
    else if (sizeof(Word_t) == 4)
        printf("#%s 32 Bit version\n", argv[0]);

    printf("# XLABEL Population\n");
    printf("# YLABEL Microseconds / Index\n");

//============================================================
// CALCULATE NUMBER OF MEASUREMENT GROUPS
//============================================================

//  Calculate Multiplier for number of points per decade
    Mult = pow(10.0, 1.0 / (double)PtsPdec);
    {
        double    sum;
        Word_t    numb, prevnumb;

//      Count number of measurements needed (10K max)
        sum = numb = 1;
        for (Groups = 2; Groups < 10000; Groups++)
        {
            if (NextNumb(&numb, &sum, Mult, nElms)) break;
        }

//      Get memory for measurements
        Pms = (Pms_t) calloc(Groups, sizeof(ms_t));

//      Now calculate number of Indexes for each measurement point
        numb = sum = 1;
        prevnumb = 0;
        for (grp = 0; grp < Groups; grp++)
        {
            Pms[grp].ms_delta = numb - prevnumb;
            prevnumb = numb;

            NextNumb(&numb, &sum, Mult, nElms);
        }
    }                           // Groups = number of sizes

//============================================================
// PRINT HEADER TO PERFORMANCE TIMERS
//============================================================

    printf("# COLHEAD 1 Population\n");
    printf("# COLHEAD 2 Measurments\n");
    printf("# COLHEAD 3 J1S\n");
    printf("# COLHEAD 4 JLI\n");
    printf("# COLHEAD 5 J1T\n");
    printf("# COLHEAD 6 JLG\n");

    Col = 7;
    if (IFlag)
    {
        printf("# COLHEAD %d J1S-dup\n", Col++);
        printf("# COLHEAD %d JLI-dup\n", Col++);
    }
    if (CFlag)
    {
        printf("# COLHEAD %d J1C\n", Col++);
        printf("# COLHEAD %d JLC\n", Col++);
    }
    if (vFlag)
    {
        printf("# COLHEAD %d J1N\n", Col++);
        printf("# COLHEAD %d JLN\n", Col++);
        printf("# COLHEAD %d J1P\n", Col++);
        printf("# COLHEAD %d JLP\n", Col++);
        printf("# COLHEAD %d J1NE\n", Col++);
        printf("# COLHEAD %d JLNE\n", Col++);
        printf("# COLHEAD %d J1PE\n", Col++);
        printf("# COLHEAD %d JLPE\n", Col++);
    }
    if (dFlag)
    {
        printf("# COLHEAD %d J1U\n", Col++);
        printf("# COLHEAD %d JLD\n", Col++);
    }
    printf("# COLHEAD %d J1MU/I\n", Col++);
    printf("# COLHEAD %d JLMU/I\n", Col++);
    if (aFlag)
    {
        printf("# COLHEAD %d J1MA/I\n", Col++);
        printf("# COLHEAD %d JLMA/I\n", Col++);
    }
    printf("# COLHEAD %d HEAP/I\n", Col++);

    printf("# %s\n", Judy1MallocSizes);
    printf("# %s\n", JudyLMallocSizes);

    printf("#     Pop1   Measmts    J1S    JLI    J1T    JLG");

    if (IFlag)
        printf(" dupJ1S dupJLI");

    if (CFlag)
        printf("    J1C    JLC");

    if (vFlag)
        printf("    J1N    JLN    J1P    JLP   J1NE   JLNE   J1PE   JLPE");

    if (dFlag)
        printf("    J1U    JLD");

    printf(" J1MU/I JLMU/I");
    if (aFlag)
    {
        printf(" J1MA/I JLMA/I");
    }

    printf(" HEAP/I");

    printf("\n");

//============================================================
// BEGIN TESTS AT EACH GROUP SIZE
//============================================================

//  Get the kicker to test the LFSR
    FirstSeed = Seed = StartSeed & (RandomBit * 2 - 1);

    STARTmem;
    for (Pop1 = grp = 0; grp < Groups; grp++)
    {
        Word_t    LowIndex, HighIndex;
        Word_t    Delta;
        Word_t    NewSeed;

        Delta = Pms[grp].ms_delta;

//      Test J1S, JLI
        NewSeed = TestJudyIns(&J1, &JL, Seed, Delta);

//      Accumulate the Total population of arrays
        Pop1 += Delta;
        Meas = Pop1;

//      Only test the maximum of TValues if not zero
        if (TValues)
            Meas = (Pop1 < TValues) ? Pop1 : TValues;

        printf("%10lu %9lu", Pop1, Meas);
        printf(" %6.3f", DeltaUSec1);
        printf(" %6.3f", DeltaUSecL);

//      Test J1T, JLG

        LowIndex = TestJudyGet(J1, JL, FirstSeed, Meas);
        printf(" %6.3f", DeltaUSec1);
        printf(" %6.3f", DeltaUSecL);

//      Test J1T, JLI - duplicates

        if (IFlag)
        {
            LowIndex = TestJudyDup(&J1, &JL, FirstSeed, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
        }
        if (CFlag)
        {
            TestJudyCount(J1, JL, LowIndex, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
        }
        if (vFlag)
        {
//          Test J1N, JLN
            HighIndex = TestJudyNext(J1, JL, LowIndex, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);

//          Test J1P, JLP
            TestJudyPrev(J1, JL, HighIndex, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);

//          Test J1NE, JLNE
            TestJudyNextEmpty(J1, JL, LowIndex, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);

//          Test J1PE, JLPE
            TestJudyPrevEmpty(J1, JL, HighIndex, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
        }

//      Test J1U, JLD
        if (dFlag)
        {
            TestJudyDel(&J1, &JL, FirstSeed, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);

//              Now put them back
            TestJudyIns(&J1, &JL, FirstSeed, Meas);
        }

//      Advance Index number set
        Seed = NewSeed;

//      Print the number of bytes used per Index
        printf(" %6.3f", (double)Judy1MemUsed(J1) / (double)Pop1);
        printf(" %6.3f", (double)JudyLMemUsed(JL) / (double)Pop1);
        if (aFlag)
        {
            printf(" %6.3f", (double)Judy1MemActive(J1) / (double)Pop1);
            printf(" %6.3f", (double)JudyLMemActive(JL) / (double)Pop1);
        }

        ENDmem;
        printf(" %6.3f", DeltaMem / (double)Pop1);
        printf("\n");
        fflush(NULL);           // assure data gets to file in case malloc fail
    }

    JLC(CountL, JL, 0, -1);     // get the counts
    J1C(Count1, J1, 0, -1);

    if (JLFlag && J1Flag)
    {
        if (CountL != Count1)
            FAILURE("Judy1/LCount not equal", Count1);
    }

    if (Count1)
    {
        STARTTm(tm1);
        J1FA(Bytes, J1);        // Free the Judy1 Array
        ENDTm(DeltaUSec1, tm1);
        DeltaUSec1 /= (double)Count1;

        printf("# Judy1FreeArray: %lu, %0.3f bytes/Index, %0.3f USec/Index\n",
               Count1, (double)Bytes / (double)Count1, DeltaUSec1);
    }

    if (CountL)
    {
        STARTTm(tm1);
        JLFA(Bytes, JL);        // Free the JudyL Array
        ENDTm(DeltaUSecL, tm1);
        DeltaUSecL /= (double)CountL;

        printf("# JudyLFreeArray: %lu, %0.3f bytes/Index, %0.3f USec/Index\n",
               CountL, (double)Bytes / (double)CountL, DeltaUSecL);
    }
    exit(0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyIns"

Word_t
TestJudyIns(void **J1, void **JL, Word_t Seed, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    TstIndex;
    Word_t    elm;
    Word_t   *PValue;
    Word_t    Seed1 = 0;
    int       Rc;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;

    if (Elements < 100)
        Loops = (MAXLOOPS / Elements) + MINLOOPS;
    else
        Loops = 1;

    if (lFlag)
        Loops = 1;

//  Judy1Set timings

    if (J1Flag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            if (lp != 0)
            {
                for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
                {
                    Seed1 = GetNextIndex(Seed1);

                    if (DFlag)
                        TstIndex = Swizzle(Seed1);
                    else
                        TstIndex = Seed1;

                    J1U(Rc, *J1, TstIndex);
                }
            }

            STARTTm(tm1);
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                J1S(Rc, *J1, TstIndex);
                if (Rc == 0)
                    FAILURE("Judy1Set failed - DUP Index at", elm);
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

//  JudyLIns timings

    if (JLFlag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            if (lp != 0)
            {
                for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
                {
                    Seed1 = GetNextIndex(Seed1);

                    if (DFlag)
                        TstIndex = Swizzle(Seed1);
                    else
                        TstIndex = Seed1;

                    JLD(Rc, *JL, TstIndex);
                }
            }

            STARTTm(tm1);
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                JLI(PValue, *JL, TstIndex);
                if (*PValue == TstIndex)
                    FAILURE("JudyLIns failed - DUP Index", TstIndex);

                *PValue = TstIndex; // save Index in Value
            }
            ENDTm(DeltaUSecL, tm1);
            DeltaUSecL /= Elements;

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
    }
    return (Seed1);             // New seed
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyDup"

Word_t
TestJudyDup(void **J1, void **JL, Word_t Seed, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    LowIndex = ~0UL;
    Word_t    TstIndex;
    Word_t    elm;
    Word_t   *PValue;
    Word_t    Seed1;
    int       Rc;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;

    if (J1Flag)
    {
        LowIndex = ~0UL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            STARTTm(tm1);
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;

                J1S(Rc, *J1, TstIndex);
                if (Rc != 0)
                    FAILURE("Judy1Test Rc != 0", (Word_t)Rc);
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

    icnt = ICNT;

    if (JLFlag)
    {
        LowIndex = ~0UL;
        for (DDel = 1e40, lp = 0; lp < Loops; lp++)
        {
            STARTTm(tm1);
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;

                JLI(PValue, *JL, TstIndex);
                if (PValue == (Word_t *)NULL)
                    FAILURE("JudyLGet ret PValue = NULL", 0L);
                if (*PValue != TstIndex)
                    FAILURE("JudyLGet ret wrong Value at", elm);
            }
            ENDTm(DeltaUSecL, tm1);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

    return (LowIndex);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyGet"

Word_t
TestJudyGet(void *J1, void *JL, Word_t Seed, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    LowIndex = ~0UL;
    Word_t    TstIndex;
    Word_t    elm;
    Word_t   *PValue;
    Word_t    Seed1;
    int       Rc;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;

    if (J1Flag)
    {
        LowIndex = ~0UL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            STARTTm(tm1);
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;

                J1T(Rc, J1, TstIndex);
                if (Rc != 1)
                    FAILURE("Judy1Test Rc != 1", (Word_t)Rc);
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

    icnt = ICNT;

    if (JLFlag)
    {
        LowIndex = ~0UL;
        for (DDel = 1e40, lp = 0; lp < Loops; lp++)
        {
            STARTTm(tm1);
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;

                JLG(PValue, JL, TstIndex);
                if (PValue == (Word_t *)NULL)
                    FAILURE("JudyLGet ret PValue = NULL", 0L);
                if (*PValue != TstIndex)
                    FAILURE("JudyLGet ret wrong Value at", elm);
            }
            ENDTm(DeltaUSecL, tm1);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

    return (LowIndex);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyCount"

int
TestJudyCount(void *J1, void *JL, Word_t LowIndex, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    elm;
    Word_t    Count1, CountL;
    Word_t    TstIndex = LowIndex;
    int       Rc;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;

    if (J1Flag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            TstIndex = LowIndex;
            STARTTm(tm1);
            for (elm = 0; elm < Elements; elm++)
            {
                J1C(Count1, J1, LowIndex, TstIndex);

                if (Count1 != (elm + 1))
                    FAILURE("J1C at", elm);

                J1N(Rc, J1, TstIndex);
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

    if (JLFlag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            TstIndex = LowIndex;
            STARTTm(tm1);
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t   *PValue;

                JLC(CountL, JL, LowIndex, TstIndex);

                if (CountL != (elm + 1))
                    FAILURE("JLC at", elm);

                JLN(PValue, JL, TstIndex);
            }
            ENDTm(DeltaUSecL, tm1);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

    return (0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyNext"

Word_t
TestJudyNext(void *J1, void *JL, Word_t LowIndex, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    Word_t    Jindex;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;

    if (J1Flag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            int       Rc;
            Jindex = LowIndex;

            STARTTm(tm1);
            J1F(Rc, J1, Jindex);

            for (elm = 0; elm < Elements; elm++)
            {
                if (Rc != 1)
                    FAILURE("Judy1Next Rc != 1 =", (Word_t)Rc);

                J1N(Rc, J1, Jindex); // Get next one
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

    if (JLFlag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t   *PValue;

//      Get an Index low enough for Elements
            Jindex = LowIndex;

            STARTTm(tm1);
            JLF(PValue, JL, Jindex);

            for (elm = 0; elm < Elements; elm++)
            {
                if (PValue == NULL)
                    FAILURE("JudyLNext ret NULL PValue at", elm);

                JLN(PValue, JL, Jindex); // Get next one
            }
            ENDTm(DeltaUSecL, tm1);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

//  perhaps a check should be done here -- if I knew what to expect.
    return (Jindex);            // return last one
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyPrev"

int
TestJudyPrev(void *J1, void *JL, Word_t HighIndex, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;

    if (J1Flag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    J1index = HighIndex;
            int       Rc;

            STARTTm(tm1);
            J1L(Rc, J1, J1index);

            for (elm = 0; elm < Elements; elm++)
            {
                if (Rc != 1)
                    FAILURE("Judy1Prev Rc != 1 =", (Word_t)Rc);

                J1P(Rc, J1, J1index); // Get previous one
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

    if (JLFlag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t   *PValue;
            Word_t    JLindex = HighIndex;

            STARTTm(tm1);
            JLL(PValue, JL, JLindex);

            for (elm = 0; elm < Elements; elm++)
            {
                if (PValue == NULL)
                    FAILURE("JudyLPrev ret NULL PValue at", elm);

                JLP(PValue, JL, JLindex); // Get previous one
            }
            ENDTm(DeltaUSecL, tm1);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

//  perhaps a check should be done here -- if I knew what to expect.
    return (0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyNextEmpty"

// Returns number of consecutive Indexes
Word_t
TestJudyNextEmpty(void *J1, void *JL, Word_t LowIndex, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    int       Rc;               // Return code

    Loops = (MAXLOOPS / Elements) + MINLOOPS;

    if (J1Flag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = LowIndex;

            STARTTm(tm1);
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    J1index;
                J1index = Seed1;

//          Find next Empty Index, J1index is modified by J1NE
                J1NE(Rc, J1, J1index); // Rc = Judy1NextEmpty(J1, &J1index,PJE0)

                if (Rc != 1)
                    FAILURE("Judy1NextEmpty Rcode != 1 =", (Word_t)Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

    if (JLFlag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = LowIndex;

            STARTTm(tm1);
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    JLindex;
                JLindex = Seed1;

//          Find next Empty Index, JLindex is modified by JLNE
                JLNE(Rc, JL, JLindex); // Rc = JudyLNextEmpty(JL, &JLindex,PJE0)

                if (Rc != 1)
                    FAILURE("JudyLNextEmpty Rcode != 1 =", (Word_t)Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSecL, tm1);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

    return (0);
}

// Routine to time and test JudyPrevEmpty routines

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyPrevEmpty"

Word_t
TestJudyPrevEmpty(void *J1, void *JL, Word_t HighIndex, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    int       Rc;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;

    if (J1Flag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = HighIndex;

            STARTTm(tm1);
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    J1index;
                J1index = Seed1;

                J1PE(Rc, J1, J1index); // Rc = Judy1PrevEmpty(J1, &J1index,PJE0)

                if (Rc != 1)
                    FAILURE("Judy1PrevEmpty Rc != 1 =", (Word_t)Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSec1, tm1);

            if (DDel > DeltaUSec1)
            {
                icnt = ICNT;
                DDel = DeltaUSec1;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSec1 = DDel / (double)Elements;
    }

    if (JLFlag)
    {
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = HighIndex;

            STARTTm(tm1);
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    JLindex;
                JLindex = Seed1;

//          Find next Empty Index, JLindex is modified by JLPE
                JLPE(Rc, JL, JLindex); // Rc = JudyLPrevEmpty(JL, &JLindex,PJE0)

                if (Rc != 1)
                    FAILURE("JudyLPrevEmpty Rcode != 1 =", (Word_t)Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSecL, tm1);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

    return (0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyDel"

int
TestJudyDel(void **J1, void **JL, Word_t Seed, Word_t Elements)
{
    TIMER_vars(tm1);            // declare timer variables
    Word_t    TstIndex;
    Word_t    elm;
    Word_t    Seed1;
    int       Rc;

    if (J1Flag)
    {
        STARTTm(tm1);
        for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
        {
            Seed1 = GetNextIndex(Seed1);

            if (DFlag)
                TstIndex = Swizzle(Seed1);
            else
                TstIndex = Seed1;

            J1U(Rc, *J1, TstIndex);
            if (Rc != 1)
                FAILURE("Judy1Unset ret Rcode != 1", (Word_t)Rc);
        }
        ENDTm(DeltaUSec1, tm1);
        DeltaUSec1 /= Elements;
    }

    STARTTm(tm1);
    if (JLFlag)
    {
        for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
        {
            Seed1 = GetNextIndex(Seed1);

            if (DFlag)
                TstIndex = Swizzle(Seed1);
            else
                TstIndex = Seed1;

            JLD(Rc, *JL, TstIndex);
            if (Rc != 1)
                FAILURE("JudyLDel ret Rcode != 1", (Word_t)Rc);
        }
        ENDTm(DeltaUSecL, tm1);
        DeltaUSecL /= Elements;
    }
    return (0);
}

// Routine to get next size of Indexes
int                             // return 1 if last number
NextNumb(Word_t *PNumber,       // pointer to returned next number
         double *PDNumb,        // Temp double of above
         double DMult,          // Multiplier
         Word_t MaxNumb)        // Max number to return
{
//  Save prev number
    double    PrevPDNumb = *PDNumb;
    double    DDiff;

//  Calc next number >= 1.0 beyond previous
    do {
        *PDNumb *= DMult;
        DDiff    = *PDNumb - PrevPDNumb;

    } while (DDiff < 0.5);

//  Return it in integer format
    if (DDiff < 100.0) *PNumber += (Word_t)(DDiff + 0.5);
    else               *PNumber = *PDNumb + 0.5;

//  Verify it did not exceed max number
    if (*PNumber >= MaxNumb)
    {
//      it did, so return max
        *PNumber = MaxNumb;
        return (1);             // flag it
    }
    return (0);                 // more available
}
