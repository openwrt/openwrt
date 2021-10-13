// @(#) $Revision: 4.20 $ $Source: /judy/test/manual/Judy1LHTime.c $
//=======================================================================
//   This program measures the performance of a Judy1 and JudyL Array.
//                      -by- 
//   Author Douglas L. Baskins, Aug 2003.
//   Permission to use this code is freely granted, provided that this
//   statement is retained.
//   email - dougbaskins@yahoo.com
//=======================================================================

#include <unistd.h>                     // sbrk()
#include <stdlib.h>                     // exit()
#include <stdio.h>                      // printf(), setbuf()
#include <math.h>                       // pow()
#include <sys/time.h>                   // gettimeofday()
#include <sys/utsname.h>                // uname()

#include <Judy.h>                       // for Judy macros J*()
//#include <JudyHS.h>             // compiling with old Judy.h without JudyHS

#ifdef NOINLINE                         /* this is the 21st century? */
#define _INLINE_ static
#else
#define _INLINE_ inline
#endif

//=======================================================================
//   This program measures the performance of a Judy1, JudyL and 
//   limited to one size of string (sizeof Word_t) JudyHS Arrays.
//
// Compile: 
//
//   cc -O Judy1LHTime.c -lm -lJudy -o Judy1LHTime 
//           -or-
//   cc -O -DCPUMHZ=2400 Judy1LHTime.c -lm -lJudy -o Judy1LHTime

/* Notes:  
   1) Use '-DCPUMHZ=2400' in cc line if better clock resolution is desired
      and it compiles successfully.  The 2400 is the cpu MHz : 2400  -or-
      cat /proc/cpuinfo | grep "cpu MHz" in a Linux system.
   2)
*/

//=======================================================================
//      T I M I N G   M A C R O S
//=======================================================================

double    DeltaUSec;                    // Global for remembering delta times

#ifdef  CPUMHZ

//  For a 1.34 nS clock cycle processor (750Mhz)

#define CPUSPEED      (1.0 / (CPUMHZ))

//#define rdtscll(val)  __asm__ __volatile__("rdtsc" : "=A" (val))

#define rdtscl(low)  __asm__ __volatile__("rdtsc" : "=a" (low) : : "edx")

static inline uint32_t
get_cycles(void)
{
    uint32_t  ret = 0;
    rdtscl(ret);
    return (ret);
}

#define STARTTm 							\
{ 									\
    gettimeofday(&TVBeg__, NULL);					\
    GCBeg__ = get_cycles(); 						\
}

#define ENDTm(D) 							\
{ 									\
    uint32_t  GCEnd__;							\
    GCEnd__ = get_cycles();						\
    gettimeofday(&TVEnd__, NULL);                                   	\
    (D) = (double)(TVEnd__.tv_sec  - TVBeg__.tv_sec) * 1E6 +   		\
         ((double)(TVEnd__.tv_usec - TVBeg__.tv_usec));         	\
    if (D < 10000.0) D = (double)(GCEnd__ - GCBeg__) * CPUSPEED; 	\
}

#else  // ! CPUMHZ

#define STARTTm gettimeofday(&TVBeg__, NULL)

#define ENDTm(D)                                                      	\
{                                                                       \
    gettimeofday(&TVEnd__, NULL);                                   	\
    (D) = (double)(TVEnd__.tv_sec  - TVBeg__.tv_sec) * 1E6 +    	\
         ((double)(TVEnd__.tv_usec - TVBeg__.tv_usec));         	\
}

#endif // ! CPUMHZ

// ****************************************************************************
// J U D Y   M A L L O C
//
// Allocate RAM.  This is the single location in Judy code that calls
// malloc(3C).  Note:  JPM accounting occurs at a higher level.

enum
{
    JudyMal1,
    JudyMalL,
    JudyMalHS
} MalFlag;

static Word_t MalFreeCnt = 0;

Word_t    TotJudy1MemUsed = 0;
Word_t    TotJudyLMemUsed = 0;
Word_t    TotJudyHSMemUsed = 0;

Word_t
JudyMalloc(Word_t Words)
{
    Word_t    Addr;
    Word_t    IncWords;

    Addr = (Word_t)malloc(Words * sizeof(Word_t));

    if (Addr)
    {
        IncWords = Words;
        MalFreeCnt++;

//      Adjust for overhead of (dl)malloc
        if (Words % 1)
            IncWords += 1;
        else
            IncWords += 2;

        switch (MalFlag)
        {
        case JudyMal1:
            TotJudy1MemUsed += IncWords;
            break;
        case JudyMalL:
            TotJudyLMemUsed += IncWords;
            break;
        case JudyMalHS:
            TotJudyHSMemUsed += IncWords;
            break;
        }
    }
    return (Addr);
}                                       // JudyMalloc()

// ****************************************************************************
// J U D Y   F R E E

void
JudyFree(void *PWord, Word_t Words)
{
    Word_t    DecWords;

    if (Words == 0)
        printf("OOps JudyFree called with 0 words\n");

    free(PWord);
    DecWords = Words;
    MalFreeCnt++;

//  Adjust for overhead of (dl)malloc
    if (Words % 1)
        DecWords += 1;
    else
        DecWords += 2;

    switch (MalFlag)
    {
    case JudyMal1:
        TotJudy1MemUsed -= DecWords;
        break;
    case JudyMalL:
        TotJudyLMemUsed -= DecWords;
        break;
    case JudyMalHS:
        TotJudyHSMemUsed -= DecWords;
        break;
    }
}                                       // JudyFree()

// ****************************************************************************
// J U D Y   M A L L O C
//
// Higher-level "wrapper" for allocating objects that need not be in RAM,
// although at this time they are in fact only in RAM.  Later we hope that some
// entire subtrees (at a JPM or branch) can be "virtual", so their allocations
// and frees should go through this level.

Word_t
JudyMallocVirtual(Word_t Words)
{
    return (JudyMalloc(Words));

}                                       // JudyMallocVirtual()

// ****************************************************************************
// J U D Y   F R E E

void
JudyFreeVirtual(void *PWord, Word_t Words)
{
    JudyFree(PWord, Words);

}                                       // JudyFreeVirtual()

//=======================================================================

// Common macro to handle a failure
#define FAILURE(STR, UL)                                                \
{                                                                       \
printf(        "\nError: %s %lu, file='%s', 'function='%s', line %d\n", \
        STR, (Word_t)(UL), __FILE__, __FUNCTI0N__, __LINE__);           \
fprintf(stderr,"\nError: %s %lu, file='%s', 'function='%s', line %d\n", \
        STR, (Word_t)(UL), __FILE__, __FUNCTI0N__, __LINE__);           \
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
int       NextNumb(Word_t *PNumber, double *PDNumb, double DMult, Word_t MaxN);

Word_t    TestJudyIns(void **J1, void **JL, void **JH, Word_t Seed,
                      Word_t Elems);

Word_t    TestJudyDup(void **J1, void **JL, void **JH, Word_t Seed,
                      Word_t Elems);

int       TestJudyDel(void **J1, void **JL, void **JH, Word_t Seed,
                      Word_t Elems);

Word_t    TestJudyGet(void *J1, void *JL, void *JH, Word_t Seed, Word_t Elems);

int       TestJudy1Copy(void *J1, Word_t Elem);

int       TestJudyCount(void *J1, void *JL, Word_t LowIndex, Word_t Elems);

Word_t    TestJudyNext(void *J1, void *JL, Word_t LowIndex, Word_t Elems);

int       TestJudyPrev(void *J1, void *JL, Word_t HighIndex, Word_t Elems);

Word_t    TestJudyNextEmpty(void *J1, void *JL, Word_t LowIndex, Word_t Elems);

Word_t    TestJudyPrevEmpty(void *J1, void *JL, Word_t HighIndex, Word_t Elems);

//=======================================================================
// These are LFSF feedback taps for bitwidths of 10..64 sized numbers.
// Tested with Seed=0xc1fc to 35 billion numbers
//=======================================================================

Word_t    StartSeed = 0xc1fc;           // default beginning number
Word_t    FirstSeed;

Word_t    MagicList[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // 0..9
    0x27f,                              // 10
    0x27f,                              // 11
    0x27f,                              // 12
    0x27f,                              // 13
    0x27f,                              // 14
    0x27f,                              // 15
    0x1e71,                             // 16
    0xdc0b,                             // 17
    0xdc0b,                             // 18
    0xdc0b,                             // 19
    0xdc0b,                             // 20
    0xc4fb,                             // 21
    0xc4fb,                             // 22
    0xc4fb,                             // 23
    0x13aab,                            // 24 
    0x11ca3,                            // 25
    0x11ca3,                            // 26
    0x11ca3,                            // 27
    0x13aab,                            // 28
    0x11ca3,                            // 29
    0xc4fb,                             // 30
    0xc4fb,                             // 31
    0x13aab,                            // 32 
    0x14e73,                            // 33  
    0x145d7,                            // 34  
    0x145f9,                            // 35  following tested with Seed=0xc1fc to 35 billion numbers
    0x151ed,                            // 36 .. 41 
    0x151ed,                            // 37  
    0x151ed,                            // 38  
    0x151ed,                            // 39  
    0x151ed,                            // 40  
    0x146c3,                            // 41 .. 64 
    0x146c3,                            // 42  
    0x146c3,                            // 43  
    0x146c3,                            // 44  
    0x146c3,                            // 45  
    0x146c3,                            // 46  
    0x146c3,                            // 47  
    0x146c3,                            // 48  
    0x146c3,                            // 49  
    0x146c3,                            // 50  
    0x146c3,                            // 51  
    0x146c3,                            // 52  
    0x146c3,                            // 53  
    0x146c3,                            // 54  
    0x146c3,                            // 55  
    0x146c3,                            // 56  
    0x146c3,                            // 57  
    0x146c3,                            // 58  
    0x146c3,                            // 59  
    0x146c3,                            // 60  
    0x146c3,                            // 61  
    0x146c3,                            // 62  
    0x146c3,                            // 63  
    0x146c3                             // 64  
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
#else  // not __LP64__
    word = ((word & 0x0000ffff) << 16) | ((word & 0xffff0000) >> 16);
    word = ((word & 0x00ff00ff) << 8) | ((word & 0xff00ff00) >> 8);
    word = ((word & 0x0f0f0f0f) << 4) | ((word & 0xf0f0f0f0) >> 4);
    word = ((word & 0x33333333) << 2) | ((word & 0xcccccccc) >> 2);
    word = ((word & 0x55555555) << 1) | ((word & 0xaaaaaaaa) >> 1);
#endif // not __LP64__

    return (word);
}

double    DeltaUSec1 = 0.0;             // Global for measuring delta times
double    DeltaUSecL = 0.0;             // Global for measuring delta times
double    DeltaUSecHS = 0.0;            // Global for measuring delta times
double    DeltaMalFre1 = 0.0;           // Delta mallocs/frees per inserted index
double    DeltaMalFreL = 0.0;           // Delta mallocs/frees per inserted index
double    DeltaMalFreHS = 0.0;          // Delta mallocs/frees per inserted index
double    DeltaMalFre = 0.0;            // Delta mallocs/frees per inserted index

Word_t    J1Flag = 0;                   // time Judy1
Word_t    JLFlag = 0;                   // time JudyL
Word_t    JHFlag = 0;                   // time JudyHS
Word_t    dFlag = 0;                    // time Judy1Unset JudyLDel
Word_t    vFlag = 0;                    // time Searching 
Word_t    CFlag = 0;                    // time Counting
Word_t    cFlag = 0;                    // time Copy of Judy1 array
Word_t    IFlag = 0;                    // time duplicate inserts/sets
Word_t    DFlag = 0;                    // bit reverse the data stream
Word_t    lFlag = 0;                    // do not do multi-insert tests
Word_t    SkipN = 0;                    // default == Random skip
Word_t    TValues = 1000000;            // Maximum retrieve tests for timing
Word_t    nElms = 1000000;              // Max population of arrays
Word_t    ErrorFlag = 0;
Word_t    PtsPdec = 40;                 // measurement points per decade

// Stuff for LFSR (pseudo random number generator)
Word_t    RandomBit = ~0UL / 2 + 1;
Word_t    BValue = sizeof(Word_t) * 8;
Word_t    Magic;

// for error routines -- notice misspelling, name conflicts with some compilers 
#undef __FUNCTI0N__
#define __FUNCTI0N__ "Random"

_INLINE_ Word_t                         // so INLINING compilers get to look at it.
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

_INLINE_ Word_t                         // so INLINING compilers get to look at it.
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

struct timeval TVBeg__, TVEnd__;

#ifdef  CPUMHZ
uint32_t  GCBeg__;
#endif // CPUMHZ

int
main(int argc, char *argv[])
{
//  Names of Judy Arrays
    void     *J1 = NULL;                // Judy1
    void     *JL = NULL;                // JudyL
    void     *JH = NULL;                // JudyHS

    Word_t    Count1, CountL;           // , CountHS;
    Word_t    Bytes;

    double    Mult;
    Pms_t     Pms;
    Word_t    Seed;
    Word_t    PtsPdec = 40;             // points per decade
    Word_t    Groups;                   // Number of measurement groups
    Word_t    grp;
    Word_t    Pop1;
    Word_t    Meas;
    int       Col;

    int       c;
    extern char *optarg;

    setbuf(stdout, NULL);               // unbuffer output

//============================================================
// PARSE INPUT PARAMETERS
//============================================================

    while ((c = getopt(argc, argv, "n:S:T:P:b:B:dDcC1LHvIl")) != -1)
    {
        switch (c)
        {
        case 'n':                      // Max population of arrays
            nElms = strtoul(optarg, NULL, 0);   // Size of Linear Array
            if (nElms == 0)
                FAILURE("No tests: -n", nElms);
            break;

        case 'S':                      // Step Size, 0 == Random
            SkipN = strtoul(optarg, NULL, 0);
            break;

        case 'T':                      // Maximum retrieve tests for timing 
            TValues = strtoul(optarg, NULL, 0);
            break;

        case 'P':                      // measurement points per decade
            PtsPdec = strtoul(optarg, NULL, 0);
            break;

        case 'b':                      // May not work past 35 bits if changed
            StartSeed = strtoul(optarg, NULL, 0);
            break;

        case 'B':                      // expanse of data points (random only)
            BValue = strtoul(optarg, NULL, 0);
            if ((BValue > 64)
                || (MagicList[BValue] == 0) || (BValue > (sizeof(Word_t) * 8)))
            {
                ErrorFlag++;
                printf("\nIllegal number of random bits of %lu !!!\n", BValue);
            }
            break;

        case 'v':
            vFlag = 1;                  // time Searching
            break;

        case '1':                      // time Judy1
            J1Flag = 1;
            break;

        case 'L':                      // time JudyL
            JLFlag = 1;
            break;

        case 'H':                      // time JudyHS
            JHFlag = 1;
            break;

        case 'd':                      // time Judy1Unset JudyLDel
            dFlag = 1;
            break;

        case 'D':                      // bit reverse the data stream
            DFlag = 1;
            break;

        case 'c':                      // time Copy Judy1 array
            cFlag = 1;
            break;

        case 'C':                      // time Counting
            CFlag = 1;
            break;

        case 'I':                      // time duplicate insert/set
            IFlag = 1;
            break;

        case 'l':                      // do not loop in tests
            lFlag = 1;
            break;

        default:
            ErrorFlag++;
            break;
        }
    }

    if (ErrorFlag)
    {
        printf("\n%s -n# -P# -S# -B# -T# -dDcCpdI\n\n", argv[0]);
        printf("Where:\n");
        printf("-n <#>  number of indexes (1000000) used in tests\n");
        printf("-P <#>  number measurement points (40) per decade\n");
        printf("-S <#>  index skip amount, 0 = random\n");
        printf("-B <#>  # bits (10..%d) in random number generator\n",
               (int)sizeof(Word_t) * 8);
        printf("-L      time JudyL\n");
        printf("-1      time Judy1\n");
        printf("-H      time JudyHS\n");
        printf("-I      time DUPLICATE Ins/Set times\n");
        printf("-c      time copy Judy1 Array\n");
        printf("-C      time JudyCount tests\n");
        printf("-v      time Judy First/Last/Next/Prev tests\n");
        printf("-d      time JudyDel/Unset\n");
        printf("-l      do not loop on same Indexes\n");
        printf("-T <#>  max number indexes in read times - 0 == MAX\n");
        printf("\n");

        exit(1);
    }

//  If none, then all
    if (!JLFlag && !J1Flag && !JHFlag)
        JHFlag = JLFlag = J1Flag = 1;

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

//        case 'b':              // May not work past 35 bits if changed

    if (J1Flag)
        printf(" -1");
    if (JLFlag)
        printf(" -L");
    if (JHFlag)
        printf(" -H");
    if (DFlag)
        printf(" -D");
    if (dFlag)
        printf(" -d");
    if (cFlag)
        printf(" -c");
    if (CFlag)
        printf(" -C");
    if (IFlag)
        printf(" -I");
    if (lFlag)
        printf(" -l");
    if (vFlag)
        printf(" -v");
    printf("\n");

//  uname(2) strings describing the machine
    {
        struct utsname ubuf;            // for system name

        if (uname(&ubuf) == -1)
            printf("# Uname(2) failed\n");
        else
            printf("# %s %s %s %s %s\n", ubuf.sysname, ubuf.nodename,
                   ubuf.release, ubuf.version, ubuf.machine);
    }
    if (sizeof(Word_t) == 8)
        printf("# %s 64 Bit version\n", argv[0]);
    else if (sizeof(Word_t) == 4)
        printf("# %s 32 Bit version\n", argv[0]);

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
            if (NextNumb(&numb, &sum, Mult, nElms))
                break;
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
    }                                   // Groups = number of sizes

//============================================================
// PRINT HEADER TO PERFORMANCE TIMERS
//============================================================

    Col = 1;
    printf("# COLHEAD %d Population\n", Col++);
    printf("# COLHEAD %d Measurments\n", Col++);
    printf("# COLHEAD %d J1S  - Judy1Set\n", Col++);
    printf("# COLHEAD %d JLI  - JudyLIns\n", Col++);
    printf("# COLHEAD %d JHSI - JudyHSIns\n", Col++);
    printf("# COLHEAD %d J1T  - Judy1Test\n", Col++);
    printf("# COLHEAD %d JLG  - JudyLGet\n", Col++);
    printf("# COLHEAD %d JHSG - JudyHSGet\n", Col++);

    if (IFlag)
    {
        printf("# COLHEAD %d J1S-dup\n", Col++);
        printf("# COLHEAD %d JLI-dup\n", Col++);
        printf("# COLHEAD %d JHSI-dup\n", Col++);
    }
    if (cFlag)
    {
        printf("# COLHEAD %d Copy J1T->J1S\n", Col++);
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
        printf("# COLHEAD %d JHSD\n", Col++);
    }
    printf("# COLHEAD %d 1heap/I  - Judy1 heap memery per Index\n", Col++);
    printf("# COLHEAD %d Lheap/I  - JudyL heap memery per Index\n", Col++);
    printf("# COLHEAD %d HSheap/I - JudyHS heap memery per Index\n", Col++);
    printf("# COLHEAD %d MF1/I    - Judy1 malloc+free's per delta Indexes\n",
           Col++);
    printf("# COLHEAD %d MFL/I    - JudyL malloc+free's per delta Indexes\n",
           Col++);
    printf("# COLHEAD %d MFHS/I   - JudyHS malloc+free's per delta Indexes\n",
           Col++);

#ifdef  CPUMHZ
    printf("#\n# Compiled for processor speed of %d Mhz\n#\n", CPUMHZ);
#endif // CPUMHZ

    printf("# %s - Leaf sizes in Words\n", Judy1MallocSizes);
    printf("# %s - Leaf sizes in Words\n#\n", JudyLMallocSizes);

    printf("#     Pop1   Measmts    J1S    JLI   JHSI    J1T    JLG   JHSG");

    if (IFlag)
        printf(" dupJ1S dupJLI dupJHI");

    if (cFlag)
        printf(" CopyJ1");

    if (CFlag)
        printf("    J1C    JLC");

    if (vFlag)
        printf("    J1N    JLN    J1P    JLP   J1NE   JLNE   J1PE   JLPE");

    if (dFlag)
        printf("    J1U    JLD   JHSD");

    printf(" 1heap/I Lheap/I HSheap/I");

    printf(" MF1/I");
    printf(" MFL/I");
    printf(" MFHS/I");

    printf("\n");

//============================================================
// BEGIN TESTS AT EACH GROUP SIZE
//============================================================

//  Get the kicker to test the LFSR
    FirstSeed = Seed = StartSeed & (RandomBit * 2 - 1);

    for (Pop1 = grp = 0; grp < Groups; grp++)
    {
        Word_t    LowIndex;
        Word_t    Delta;
        Word_t    NewSeed;

        Delta = Pms[grp].ms_delta;

//      Test J1S, JLI
        NewSeed = TestJudyIns(&J1, &JL, &JH, Seed, Delta);

//      Accumulate the Total population of arrays
        Pop1 += Delta;
        Meas = Pop1;

//      Only test the maximum of TValues if not zero
        if (TValues)
            Meas = (Pop1 < TValues) ? Pop1 : TValues;

        printf("%10lu %9lu", Pop1, Meas);
        printf(" %6.3f", DeltaUSec1);
        printf(" %6.3f", DeltaUSecL);
        printf(" %6.3f", DeltaUSecHS);
        fflush(NULL);

//      Test J1T, JLG, JHSG

        LowIndex = TestJudyGet(J1, JL, JH, FirstSeed, Meas);
        printf(" %6.3f", DeltaUSec1);
        printf(" %6.3f", DeltaUSecL);
        printf(" %6.3f", DeltaUSecHS);
        fflush(NULL);

//      Test J1T, JLI, JHSI - duplicates

        if (IFlag)
        {
            LowIndex = TestJudyDup(&J1, &JL, &JH, FirstSeed, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
            printf(" %6.3f", DeltaUSecHS);
            fflush(NULL);
        }
        if (cFlag)
        {
            TestJudy1Copy(J1, Meas);
            printf(" %6.3f", DeltaUSec1);
            fflush(NULL);
        }
        if (CFlag)
        {
            TestJudyCount(J1, JL, LowIndex, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
            fflush(NULL);
        }
        if (vFlag)
        {
//          Test J1N, JLN
//          HighIndex = TestJudyNext(J1, JL, LowIndex, Meas);
            TestJudyNext(J1, JL, 0, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
            fflush(NULL);

//          Test J1P, JLP
            TestJudyPrev(J1, JL, ~0UL, Meas);
//          TestJudyPrev(J1, JL, HighIndex, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
            fflush(NULL);

//          Test J1NE, JLNE
            TestJudyNextEmpty(J1, JL, 0UL, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
            fflush(NULL);

//          Test J1PE, JLPE
            TestJudyPrevEmpty(J1, JL, ~0UL, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
            fflush(NULL);
        }

//      Test J1U, JLD, JHSD
        if (dFlag)
        {
            TestJudyDel(&J1, &JL, &JH, FirstSeed, Meas);
            printf(" %6.3f", DeltaUSec1);
            printf(" %6.3f", DeltaUSecL);
            printf(" %6.3f", DeltaUSecHS);
            fflush(NULL);

//          Now put them back
            TestJudyIns(&J1, &JL, &JH, FirstSeed, Meas);
        }

//      Advance Index number set
        Seed = NewSeed;

//      Print the number of bytes used per Index
        printf(" %7.3f",
               (double)TotJudy1MemUsed * sizeof(Word_t) / (double)Pop1);
        printf(" %7.3f",
               (double)TotJudyLMemUsed * sizeof(Word_t) / (double)Pop1);
        printf(" %8.3f",
               (double)TotJudyHSMemUsed * sizeof(Word_t) / (double)Pop1);

        printf(" %5.3f", DeltaMalFre1);
        printf(" %5.3f", DeltaMalFreL);
        printf(" %5.3f", DeltaMalFreHS);

        printf("\n");
        fflush(NULL);                   // assure data gets to file in case malloc fail
    }

#ifdef SKIPMACRO
    Count1 = Judy1Count(J1, 0, -1, PJE0);
    CountL = JudyLCount(JL, 0, -1, PJE0);
#else
    JLC(CountL, JL, 0, -1);             // get the counts
    J1C(Count1, J1, 0, -1);
#endif // SKIPMACRO

    if (JLFlag && J1Flag)
    {
        if (CountL != Count1)
            FAILURE("Judy1/LCount not equal", Count1);
    }

//    if (Count1)
    {
        Word_t    Cnt1 = Count1;
        if (Cnt1 == 0)
            Cnt1 = 1UL;

        STARTTm;

#ifdef SKIPMACRO
        Bytes = Judy1FreeArray(&J1, PJE0);
#else
        J1FA(Bytes, J1);                // Free the Judy1 Array
#endif // SKIPMACRO

        ENDTm(DeltaUSec1);
        DeltaUSec1 /= (double)Cnt1;

        printf("# Judy1FreeArray:  %lu, %0.3f bytes/Index, %0.3f USec/Index\n",
               Count1, (double)Bytes / (double)Cnt1, DeltaUSec1);
    }

//    if (CountL)
    {
        Word_t    CntL = CountL;
        if (CntL == 0)
            CntL = 1UL;

        STARTTm;

#ifdef SKIPMACRO
        Bytes = JudyLFreeArray(&JL, PJE0);
#else
        JLFA(Bytes, JL);                // Free the JudyL Array
#endif // SKIPMACRO

        ENDTm(DeltaUSecL);
        DeltaUSecL /= (double)CntL;

        printf("# JudyLFreeArray:  %lu, %0.3f bytes/Index, %0.3f USec/Index\n",
               CountL, (double)Bytes / (double)CntL, DeltaUSecL);
    }
//    if (CountHS)
    {
//        Word_t CntHS = CountHS;
//        if (CntHS == 0) CntHS = 1UL;

        STARTTm;

#ifdef SKIPMACRO
        Bytes = JudyHSFreeArray(&JH, PJE0);     // Free the JudyHS Array
#else
        JHSFA(Bytes, JH);               // Free the JudyHS Array
#endif // SKIPMACRO

        ENDTm(DeltaUSecHS);
        DeltaUSecHS /= (double)nElms;   // no Counts yet

        printf("# JudyHSFreeArray: %lu, %0.3f bytes/Index, %0.3f USec/Index\n",
               nElms, (double)Bytes / (double)nElms, DeltaUSecHS);
    }
    exit(0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyIns"

Word_t
TestJudyIns(void **J1, void **JL, void **JH, Word_t Seed, Word_t Elements)
{
    Word_t    TstIndex;
    Word_t    elm;
    Word_t   *PValue;
    Word_t    Seed1 = 0;
    int       Rc;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    Word_t    StartMallocs;

    if (Elements < 100)
        Loops = (MAXLOOPS / Elements) + MINLOOPS;
    else
        Loops = 1;

    if (lFlag)
        Loops = 1;

//  Judy1Set timings

    if (J1Flag)
    {
        MalFlag = JudyMal1;
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
#ifdef SKIPMACRO
                    Rc = Judy1Unset(J1, TstIndex, PJE0);
#else
                    J1U(Rc, *J1, TstIndex);
#endif // SKIPMACRO
                }
            }

            StartMallocs = MalFreeCnt;
            STARTTm;
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
#ifdef DIAG
                Rc = Judy1Test(*J1, TstIndex, PJE0);
                if (Rc != 1)
                    FAILURE("Judy1Test failed at", elm);
#endif // DIAG
            }
            ENDTm(DeltaUSec1);
            DeltaMalFre1 = (double)(MalFreeCnt - StartMallocs) / Elements;

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
        MalFlag = JudyMalL;
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
#ifdef SKIPMACRO
                    Rc = JudyLDel(JL, TstIndex, PJE0);
#else
                    JLD(Rc, *JL, TstIndex);
#endif // SKIPMACRO
                }
            }

            StartMallocs = MalFreeCnt;
            STARTTm;
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                JLI(PValue, *JL, TstIndex);
                if (*PValue == TstIndex)
                {
                    printf("Index = 0x%0lx, ", TstIndex);
                    FAILURE("JudyLIns failed - DUP Index", TstIndex);
                }
                *PValue = TstIndex;     // save Index in Value
#ifdef DIAG
                PValue = (PWord_t)JudyLGet(*JL, TstIndex, PJE0);
                if (PValue == NULL)
                {
                    printf("Index = 0x%0lx, ", TstIndex);
                    FAILURE("JudyLGet failed", TstIndex);
                }
#endif // DIAG
            }
            ENDTm(DeltaUSecL);
            DeltaUSecL /= Elements;
            DeltaMalFreL = (double)(MalFreeCnt - StartMallocs) / Elements;

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

//  JudyHSIns timings

    if (JHFlag)
    {
        MalFlag = JudyMalHS;
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

                    JHSD(Rc, *JH, &TstIndex, sizeof(Word_t));
                }
            }

            StartMallocs = MalFreeCnt;
            STARTTm;
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                JHSI(PValue, *JH, &TstIndex, sizeof(Word_t));
                if (*PValue == TstIndex)
                    FAILURE("JudyHSIns failed - DUP Index", TstIndex);

                *PValue = TstIndex;     // save Index in Value
            }
            ENDTm(DeltaUSecHS);
            DeltaUSecHS /= Elements;
            DeltaMalFreHS = (double)(MalFreeCnt - StartMallocs) / Elements;

            if (DDel > DeltaUSecHS)
            {
                icnt = ICNT;
                DDel = DeltaUSecHS;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
    }
    return (Seed1);                     // New seed
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyDup"

Word_t
TestJudyDup(void **J1, void **JL, void **JH, Word_t Seed, Word_t Elements)
{
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
    if (lFlag)
        Loops = 1;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        LowIndex = ~0UL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            STARTTm;
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
                    FAILURE("Judy1Test Rc != 0", Rc);
            }
            ENDTm(DeltaUSec1);

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
        MalFlag = JudyMalL;
        LowIndex = ~0UL;
        for (DDel = 1e40, lp = 0; lp < Loops; lp++)
        {
            STARTTm;
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
            ENDTm(DeltaUSecL);

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

    icnt = ICNT;

    if (JHFlag)
    {
        MalFlag = JudyMalHS;
        LowIndex = ~0UL;
        for (DDel = 1e40, lp = 0; lp < Loops; lp++)
        {
            STARTTm;
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;

                JHSI(PValue, *JH, &TstIndex, sizeof(Word_t));
                if (PValue == (Word_t *)NULL)
                    FAILURE("JudyHSGet ret PValue = NULL", 0L);
                if (*PValue != TstIndex)
                    FAILURE("JudyHSGet ret wrong Value at", elm);
            }
            ENDTm(DeltaUSecHS);

            if (DDel > DeltaUSecHS)
            {
                icnt = ICNT;
                DDel = DeltaUSecHS;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecHS = DDel / (double)Elements;
    }
    return (LowIndex);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyGet"

Word_t
TestJudyGet(void *J1, void *JL, void *JH, Word_t Seed, Word_t Elements)
{
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

    if (lFlag)
        Loops = 1;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        LowIndex = ~0UL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            STARTTm;
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;
#ifdef SKIPMACRO
                Rc = Judy1Test(J1, TstIndex, PJE0);
#else
                J1T(Rc, J1, TstIndex);
#endif // SKIPMACRO
                if (Rc != 1)
                {
                    printf
                        ("\nJudy1Test wrong Rc = %d, Index = 0x%lx, elm = %lu",
                         Rc, TstIndex, elm);
                    FAILURE("Judy1Test Rc != 1", Rc);
                }
            }
            ENDTm(DeltaUSec1);

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
        MalFlag = JudyMalL;
        LowIndex = ~0UL;
        for (DDel = 1e40, lp = 0; lp < Loops; lp++)
        {
            STARTTm;
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;
#ifdef SKIPMACRO
                PValue = (PWord_t)JudyLGet(JL, TstIndex, PJE0);
#else
                JLG(PValue, JL, TstIndex);
#endif // SKIPMACRO

                if (PValue == (Word_t *)NULL)
                {
                    printf("\nJudyGet Index = 0x%lx", TstIndex);
                    FAILURE("JudyLGet ret PValue = NULL", 0L);
                }
                if (*PValue != TstIndex)
                {
                    printf("JudyLGet returned Value=0x%lx, should be=0x%lx\n",
                           *PValue, TstIndex);
                    FAILURE("JudyLGet ret wrong Value at", elm);
                }
            }
            ENDTm(DeltaUSecL);

            if (DDel > DeltaUSecL)
            {
                icnt = ICNT;
                DDel = DeltaUSecL;
            }

            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecL = DDel / (double)Elements;
    }

    icnt = ICNT;

    if (JHFlag)
    {
        MalFlag = JudyMalHS;
        LowIndex = ~0UL;
        for (DDel = 1e40, lp = 0; lp < Loops; lp++)
        {
            STARTTm;
            for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
            {
                Seed1 = GetNextIndex(Seed1);

                if (DFlag)
                    TstIndex = Swizzle(Seed1);
                else
                    TstIndex = Seed1;

                if (TstIndex < LowIndex)
                    LowIndex = TstIndex;

                JHSG(PValue, JH, &TstIndex, sizeof(Word_t));
                if (PValue == (Word_t *)NULL)
                    FAILURE("JudyHSGet ret PValue = NULL", 0L);
                if (*PValue != TstIndex)
                    FAILURE("JudyHSGet ret wrong Value at", elm);
            }
            ENDTm(DeltaUSecHS);

            if (DDel > DeltaUSecHS)
            {
                icnt = ICNT;
                DDel = DeltaUSecHS;
            }
            else
            {
                if (--icnt == 0)
                    break;
            }
        }
        DeltaUSecHS = DDel / (double)Elements;
    }
    return (LowIndex);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudy1Copy"

int
TestJudy1Copy(void *J1, Word_t Elements)
{
    Pvoid_t   J1a;                      // Judy1 new array
    Word_t    elm = 0;
    Word_t    Bytes;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;
    if (lFlag)
        Loops = 1;

    MalFlag = JudyMal1;
    for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
    {
        Word_t    NextIndex;
        int       Rc;

        STARTTm;
        J1a = NULL;                     // Initialize To array

        NextIndex = 0;                  // Start at the beginning
        elm = 0;

#ifdef SKIPMACRO
        Rc = Judy1First(J1, &NextIndex, PJE0);
        while (Rc == 1)
        {
            if (elm++ == Elements) break;

            Rc = Judy1Set(&J1a, NextIndex, PJE0);
            if (Rc != 1)
                FAILURE("Judy1Set at", elm);
            Rc = Judy1Next(J1, &NextIndex, PJE0);
        }
#else
        J1F(Rc, J1, NextIndex);         // return first Index
        while (Rc == 1)
        {
            if (elm++ == Elements) break;

            J1S(Rc, J1a, NextIndex);
            if (Rc != 1)
                FAILURE("J1S at", elm);
            J1N(Rc, J1, NextIndex);
        }
#endif // SKIPMACRO

        ENDTm(DeltaUSec1);

#ifdef SKIPMACRO
        Bytes = Judy1FreeArray(&J1a, PJE0);
#else
        J1FA(Bytes, J1a);               // no need to keep it around
#endif // SKIPMACRO
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
    DeltaUSec1 = DDel / (double)elm;

    return (0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyCount"

int
TestJudyCount(void *J1, void *JL, Word_t LowIndex, Word_t Elements)
{
    Word_t    elm;
    Word_t    Count1, CountL;
    Word_t    TstIndex = LowIndex;
    int       Rc;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;
    if (lFlag)
        Loops = 1;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            TstIndex = LowIndex;
            STARTTm;
            for (elm = 0; elm < Elements; elm++)
            {
#ifdef SKIPMACRO
                Count1 = Judy1Count(J1, LowIndex, TstIndex, PJE0);
#else
                J1C(Count1, J1, LowIndex, TstIndex);
#endif // SKIPMACRO

                if (Count1 != (elm + 1))
                {
                    printf("Count1 = %lu, elm +1 = %lu\n", Count1, elm + 1);
                    FAILURE("J1C at", elm);
                }
#ifdef SKIPMACRO
                Rc = Judy1Next(J1, &TstIndex, PJE0);
#else
                J1N(Rc, J1, TstIndex);
#endif // SKIPMACRO

            }
            ENDTm(DeltaUSec1);

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
        MalFlag = JudyMalL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            TstIndex = LowIndex;
            STARTTm;
            for (elm = 0; elm < Elements; elm++)
            {
                PWord_t   PValue;
#ifdef SKIPMACRO
                CountL = JudyLCount(JL, LowIndex, TstIndex, PJE0);
#else
                JLC(CountL, JL, LowIndex, TstIndex);
#endif // SKIPMACRO
                if (CountL != (elm + 1))
                {
                    printf("CountL = %lu, elm +1 = %lu\n", CountL, elm + 1);
                    FAILURE("JLC at", elm);
                }
#ifdef SKIPMACRO
                PValue = (PWord_t)JudyLNext(JL, &TstIndex, PJE0);
#else
                JLN(PValue, JL, TstIndex);
#endif // SKIPMACRO
            }
            ENDTm(DeltaUSecL);

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
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    Word_t    JLindex;
    Word_t    J1index;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;
    if (lFlag)
        Loops = 1;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            int       Rc;

            J1index = LowIndex;

            STARTTm;
#ifdef SKIPMACRO
            Rc = Judy1First(J1, &J1index, PJE0);
#else
            J1F(Rc, J1, J1index);
#endif // SKIPMACRO
            for (elm = 0; elm < Elements; elm++)
            {
                if (Rc != 1)
                {
                    printf("\nElements = %lu, elm = %lu\n", Elements, elm);
                    FAILURE("Judy1Next Rc != 1 =", Rc);
                }
#ifdef SKIPMACRO
                Rc = Judy1Next(J1, &J1index, PJE0);
#else
                J1N(Rc, J1, J1index);   // Get next one
#endif // SKIPMACRO
            }
            ENDTm(DeltaUSec1);

            if ((TValues == 0) && (Rc != 0))
            {
                FAILURE("Judy1Next Rc != 0", Rc);
            }

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
        MalFlag = JudyMalL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t   *PValue;

//          Get an Index low enough for Elements
            JLindex = LowIndex;

            STARTTm;
            JLF(PValue, JL, JLindex);

            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    Prev;
                if (PValue == NULL)
                {
                    printf("\nElements = %lu, elm = %lu\n", Elements, elm);
                    FAILURE("JudyLNext ret NULL PValue at", elm);
                }
                if (*PValue != JLindex)
                {
                    printf("\n*PValue=0x%lx, JLindex=0x%lx\n", *PValue,
                           JLindex);
                    FAILURE("JudyLNext ret bad *PValue at", elm);
                }
                Prev = JLindex;
#ifdef SKIPMACRO
                PValue = (PWord_t)JudyLNext(JL, &JLindex, PJE0);
#else
                JLN(PValue, JL, JLindex);       // Get next one
#endif // SKIPMACRO
                if (JLindex == Prev)
                {
                    printf("OOPs, JLN did not advance 0x%lx\n", Prev);
                    FAILURE("JudyLNext ret did not advance", Prev);
                }
            }
            ENDTm(DeltaUSecL);

            if ((TValues == 0) && (PValue != NULL))
            {
                FAILURE("JudyLNext PValue != NULL", PValue);
            }

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
    if (JLFlag)
        return (JLindex);
    if (J1Flag)
        return (J1index);
    return (-1);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyPrev"

int
TestJudyPrev(void *J1, void *JL, Word_t HighIndex, Word_t Elements)
{
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    Word_t    J1index;
    Word_t    JLindex;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;
    if (lFlag)
        Loops = 1;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            J1index = HighIndex;
            int       Rc;

            STARTTm;
#ifdef SKIPMACRO
            Rc = Judy1Last(J1, &J1index, PJE0);
#else
            J1L(Rc, J1, J1index);
#endif // SKIPMACRO

            for (elm = 0; elm < Elements; elm++)
            {
                if (Rc != 1)
                    FAILURE("Judy1Prev Rc != 1, is: ", Rc);
#ifdef SKIPMACRO
                Rc = Judy1Prev(J1, &J1index, PJE0);
#else
                J1P(Rc, J1, J1index);   // Get previous one
#endif // SKIPMACRO

            }
            ENDTm(DeltaUSec1);

            if ((TValues == 0) && (Rc != 0))
            {
                FAILURE("Judy1Prev Rc != 0", Rc);
            }

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
        MalFlag = JudyMalL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t   *PValue;
            JLindex = HighIndex;

            STARTTm;
            JLL(PValue, JL, JLindex);

            for (elm = 0; elm < Elements; elm++)
            {
                if (PValue == NULL)
                {
                    printf("\nElements = %lu, elm = %lu\n", Elements, elm);
                    FAILURE("JudyLPrev ret NULL PValue at", elm);
                }
                if (*PValue != JLindex)
                    FAILURE("JudyLPrev ret bad *PValue at", elm);

                JLP(PValue, JL, JLindex);       // Get previous one
            }
            ENDTm(DeltaUSecL);

            if ((TValues == 0) && (PValue != NULL))
                FAILURE("JudyLPrev PValue != NULL", PValue);

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
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    int       Rc;                       // Return code

    Loops = (MAXLOOPS / Elements) + MINLOOPS;
    if (lFlag)
        Loops = 1;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = LowIndex;

            STARTTm;
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    J1index;
                J1index = Seed1;

#ifdef SKIPMACRO
                Rc = Judy1NextEmpty(J1, &J1index, PJE0);
#else
                J1NE(Rc, J1, J1index);
#endif // SKIPMACRO

                if (Rc != 1)
                    FAILURE("Judy1NextEmpty Rcode != 1 =", Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSec1);

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
        MalFlag = JudyMalL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = LowIndex;

            STARTTm;
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    JLindex;
                JLindex = Seed1;
#ifdef SKIPMACRO
                Rc = JudyLNextEmpty(JL, &JLindex, PJE0);
#else
                JLNE(Rc, JL, JLindex);
#endif // SKIPMACRO
                if (Rc != 1)
                    FAILURE("JudyLNextEmpty Rcode != 1 =", Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSecL);

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
    Word_t    elm;

    double    DDel;
    Word_t    icnt;
    Word_t    lp;
    Word_t    Loops;
    int       Rc;

    Loops = (MAXLOOPS / Elements) + MINLOOPS;
    if (lFlag)
        Loops = 1;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = HighIndex;

            STARTTm;
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    J1index;
                J1index = Seed1;

#ifdef SKIPMACRO
                Rc = Judy1PrevEmpty(J1, &J1index, PJE0);
#else
                J1PE(Rc, J1, J1index);
#endif // SKIPMACRO

                if (Rc != 1)
                    FAILURE("Judy1PrevEmpty Rc != 1 =", Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSec1);

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
        MalFlag = JudyMalL;
        for (DDel = 1e40, icnt = ICNT, lp = 0; lp < Loops; lp++)
        {
            Word_t    Seed1 = HighIndex;

            STARTTm;
            for (elm = 0; elm < Elements; elm++)
            {
                Word_t    JLindex;
                JLindex = Seed1;

                JLPE(Rc, JL, JLindex);  // Rc = JudyLPrevEmpty(JL, &JLindex,PJE0)

                if (Rc != 1)
                    FAILURE("JudyLPrevEmpty Rcode != 1 =", Rc);

                Seed1 = GetNextIndex(Seed1);
            }
            ENDTm(DeltaUSecL);

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
TestJudyDel(void **J1, void **JL, void **JH, Word_t Seed, Word_t Elements)
{
    Word_t    TstIndex;
    Word_t    elm;
    Word_t    Seed1;
    int       Rc;

    if (J1Flag)
    {
        MalFlag = JudyMal1;
        STARTTm;
        for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
        {
            Seed1 = GetNextIndex(Seed1);

            if (DFlag)
                TstIndex = Swizzle(Seed1);
            else
                TstIndex = Seed1;

#ifdef SKIPMACRO
            Rc = Judy1Unset(J1, TstIndex, PJE0);
#else
            J1U(Rc, *J1, TstIndex);
#endif // SKIPMACRO
            if (Rc != 1)
                FAILURE("Judy1Unset ret Rcode != 1", Rc);
        }
        ENDTm(DeltaUSec1);
        DeltaUSec1 /= Elements;
    }

    if (JLFlag)
    {
        MalFlag = JudyMalL;
        STARTTm;
        for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
        {
            Seed1 = GetNextIndex(Seed1);

            if (DFlag)
                TstIndex = Swizzle(Seed1);
            else
                TstIndex = Seed1;

#ifdef SKIPMACRO
            Rc = JudyLDel(JL, TstIndex, PJE0);
#else
            JLD(Rc, *JL, TstIndex);
#endif // SKIPMACRO
            if (Rc != 1)
                FAILURE("JudyLDel ret Rcode != 1", Rc);
        }
        ENDTm(DeltaUSecL);
        DeltaUSecL /= Elements;
    }

    if (JHFlag)
    {
        MalFlag = JudyMalHS;
        STARTTm;
        for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
        {
            Seed1 = GetNextIndex(Seed1);

            if (DFlag)
                TstIndex = Swizzle(Seed1);
            else
                TstIndex = Seed1;

            JHSD(Rc, *JH, &TstIndex, sizeof(Word_t));
            if (Rc != 1)
                FAILURE("JudyHSDel ret Rcode != 1", Rc);
        }
        ENDTm(DeltaUSecHS);
        DeltaUSecHS /= Elements;
    }
    return (0);
}

// Routine to get next size of Indexes
int                                     // return 1 if last number
NextNumb(Word_t *PNumber,               // pointer to returned next number
         double *PDNumb,                // Temp double of above
         double DMult,                  // Multiplier
         Word_t MaxNumb)                // Max number to return
{
//  Save prev number
    double    PrevPDNumb = *PDNumb;
    double    DDiff;

//  Calc next number >= 1.0 beyond previous
    do
    {
        *PDNumb *= DMult;
        DDiff = *PDNumb - PrevPDNumb;

    } while (DDiff < 0.5);

//  Return it in integer format
    if (DDiff < 100.0)
        *PNumber += (Word_t)(DDiff + 0.5);
    else
        *PNumber = *PDNumb + 0.5;

//  Verify it did not exceed max number
    if (*PNumber >= MaxNumb)
    {
//      it did, so return max
        *PNumber = MaxNumb;
        return (1);                     // flag it
    }
    return (0);                         // more available
}
