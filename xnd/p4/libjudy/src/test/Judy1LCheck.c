// @(#) $Revision: 4.15 $ $Source: /judy/test/manual/Judy1LCheck.c $
//      This program tests the accuracy of a Judy1 with a JudyL Array.
//                      -by- 
//      Douglas L. Baskins (8/2001)  doug@sourcejudy.com

#ifndef JU_WIN_IA32
#include <unistd.h>		// unavailable on Win32, and no getopt().
#endif

#include <stdlib.h>		// calloc()
#include <math.h>               // pow()
#include <stdio.h>		// printf()

#include <Judy.h>

// Common macro to handle a failure
#define FAILURE(STR, UL)						\
{									\
printf(         "Error: %s %lu, file='%s', 'function='%s', line %d\n",	\
	STR, UL, __FILE__, __FUNCTI0N__, __LINE__); 			\
fprintf(stderr, "Error: %s %lu, file='%s', 'function='%s', line %d\n",	\
	STR, UL, __FILE__, __FUNCTI0N__, __LINE__); 			\
	exit(1);							\
}

// Structure to keep track of times
typedef struct MEASUREMENTS_STRUCT
{
    Word_t ms_delta;
}
ms_t, *Pms_t;

// Specify prototypes for each test routine
int
NextNumb(Word_t * PNumber, double *PDNumb, double DMult, Word_t MaxNumb);

Word_t TestJudyIns(void **J1, void **JL, Word_t Seed, Word_t Elements);

Word_t TestJudyDup(void **J1, void **JL, Word_t Seed, Word_t Elements);

int TestJudyDel(void **J1, void **JL, Word_t Seed, Word_t Elements);

Word_t TestJudyGet(void *J1, void *JL, Word_t Seed, Word_t Elements);

int TestJudyCount(void *J1, void *JL, Word_t LowIndex, Word_t Elements);

Word_t TestJudyNext(void *J1, void *JL, Word_t LowIndex, Word_t Elements);

int TestJudyPrev(void *J1, void *JL, Word_t HighIndex, Word_t Elements);

int
TestJudyNextEmpty(void *J1, void *JL, Word_t LowIndex, Word_t Elements);

int
TestJudyPrevEmpty(void *J1, void *JL, Word_t HighIndex, Word_t Elements);

Word_t MagicList[] = 
{
    0,0,0,0,0,0,0,0,0,0, // 0..9
    0x27f,      // 10
    0x27f,      // 11
    0x27f,      // 12
    0x27f,      // 13
    0x27f,      // 14
    0x27f,      // 15
    0x1e71,     // 16
    0xdc0b,     // 17
    0xdc0b,     // 18
    0xdc0b,     // 19
    0xdc0b,     // 20
    0xc4fb,     // 21
    0xc4fb,     // 22
    0xc4fb,     // 23
    0x13aab,    // 24 
    0x11ca3,    // 25
    0x11ca3,    // 26
    0x11ca3,    // 27
    0x13aab,    // 28
    0x11ca3,    // 29
    0xc4fb,     // 30
    0xc4fb,     // 31
    0x13aab,    // 32 
    0x14e73,    // 33  
    0x145d7,    // 34  
    0x145f9,    // 35  following tested with Seed=0xc1fc to 35Gig numbers
    0x151ed,    // 36 .. 41 
    0x151ed,    // 37  
    0x151ed,    // 38  
    0x151ed,    // 39  
    0x151ed,    // 40  
    0x146c3,    // 41 .. 64 
    0x146c3,    // 42  
    0x146c3,    // 43  
    0x146c3,    // 44  
    0x146c3,    // 45  
    0x146c3,    // 46  
    0x146c3,    // 47  
    0x146c3,    // 48  
    0x146c3,    // 49  
    0x146c3,    // 50  
    0x146c3,    // 51  
    0x146c3,    // 52  
    0x146c3,    // 53  
    0x146c3,    // 54  
    0x146c3,    // 55  
    0x146c3,    // 56  
    0x146c3,    // 57  
    0x146c3,    // 58  
    0x146c3,    // 59  
    0x146c3,    // 60  
    0x146c3,    // 61  
    0x146c3,    // 62  
    0x146c3,    // 63  
    0x146c3     // 64  
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
#else // __LP64__
    word = ((word & 0x0000ffff) << 16) | ((word & 0xffff0000) >> 16);
    word = ((word & 0x00ff00ff) << 8) | ((word & 0xff00ff00) >> 8);
    word = ((word & 0x0f0f0f0f) << 4) | ((word & 0xf0f0f0f0) >> 4);
    word = ((word & 0x33333333) << 2) | ((word & 0xcccccccc) >> 2);
    word = ((word & 0x55555555) << 1) | ((word & 0xaaaaaaaa) >> 1);
#endif // __LP64__

    return(word);
}

Word_t dFlag = 1;
Word_t pFlag = 0;
Word_t CFlag = 0;
Word_t DFlag = 0;
Word_t SkipN = 0;		// default == Random skip
Word_t nElms = 1000000;	// Default = 1M
Word_t ErrorFlag = 0;
Word_t TotalIns = 0;
Word_t TotalPop = 0;
Word_t TotalDel = 0;

// Stuff for LFSR (pseudo random number generator)
Word_t RandomBit = ~0UL / 2 + 1;
Word_t BValue    = sizeof(Word_t) * 8;
Word_t Magic;
Word_t StartSeed = 0xc1fc;	// default beginning number
Word_t FirstSeed;

#undef __FUNCTI0N__
#define __FUNCTI0N__ "Random"

static Word_t			// Placed here so INLINING compilers get to look at it.
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
    {
    	printf("End of LFSR, Total Population = %lu\n", TotalPop);
    	exit(0);
    }
    return(newseed);
}

static Word_t			// Placed here so INLINING compilers get to look at it.
GetNextIndex(Word_t Index)
{
    if (SkipN)
	Index += SkipN;
    else
	Index = Random(Index);

    return(Index);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "main"

int
main(int argc, char *argv[])
{
//  Names of Judy Arrays
    void *J1 = NULL;		// Judy1
    void *JL = NULL;		// JudyL

    double Mult;
    Pms_t Pms;
    Word_t Seed;
    Word_t PtsPdec = 10;	// points per decade
    Word_t Groups;		// Number of measurement groups
    Word_t grp;

    int  c;
    extern char *optarg;

//////////////////////////////////////////////////////////////
// PARSE INPUT PARAMETERS
//////////////////////////////////////////////////////////////

    while ((c = getopt(argc, argv, "n:S:P:b:L:B:pdDC")) != -1)
    {
	switch (c)
	{
	case 'n':		// Number of elements
	    nElms = strtoul(optarg, NULL, 0);	// Size of Linear Array
	    if (nElms == 0)
		FAILURE("No tests: -n", nElms);

//          Check if more than a trillion (64 bit only)
	    if ((double)nElms > 1e12)
		FAILURE("Too many Indexes=", nElms);
	    break;

	case 'S':		// Step Size, 0 == Random
	    SkipN = strtoul(optarg, NULL, 0);
	    break;

	case 'P':		// 
	    PtsPdec = strtoul(optarg, NULL, 0);
	    break;

	case 'b':		// May not work past 35 bits if changed
	    StartSeed = strtoul(optarg, NULL, 0);
	    break;

	case 'B':
	    BValue = strtoul(optarg, NULL, 0);
            if  (
                    (BValue > (sizeof(Word_t) * 8))
                           ||
                    (MagicList[BValue] == 0)
                )
	    {
		ErrorFlag++;
		printf("\nIllegal number of random bits of %lu !!!\n", BValue);
	    }
	    break;

	case 'p':		// Print test indexes
	    pFlag = 1;
	    break;

	case 'd':		// Delete indexes
	    dFlag = 0;
	    break;

	case 'D':		// Swizzle indexes
	    DFlag = 1;
	    break;

	case 'C':		// Skip counting test.
	    CFlag = 1;
	    break;

	default:
	    ErrorFlag++;
	    break;
	}
    }

    if (ErrorFlag)
    {
	printf("\n%s -n# -S# -B# -P# -b # -DRCpd\n\n", argv[0]);
	printf("Where:\n");
	printf("-n <#>  number of indexes used in tests\n");
	printf("-C      skip JudyCount tests\n");
	printf("-p      print index set - for debug\n");
	printf("-d      do not call JudyDel/Unset\n");
	printf("-D      Swizzle data (mirror)\n");
	printf("-S <#>  index skip amount, 0 = random\n");
	printf("-B <#>  # bits-1 in random number generator\n");
	printf("-P <#>  number measurement points per decade\n");
	printf("\n");

	exit(1);
    }
//  Set number of Random bits in LFSR
    RandomBit = 1UL << (BValue - 1);
    Magic     = MagicList[BValue];

    if (nElms > ((RandomBit-2) * 2))
    {
        printf("# Number = -n%lu of Indexes reduced to max expanse of Random numbers\n", nElms);
        nElms =  ((RandomBit-2) * 2);
    }

    printf("\n%s -n%lu -S%lu -B%lu", argv[0], nElms, SkipN, BValue);

    if (DFlag)
	printf(" -D");
    if (!dFlag)
	printf(" -d");
    if (pFlag)
	printf(" -p");
    if (CFlag)
	printf(" -C");
    printf("\n\n");

    if (sizeof(Word_t) == 8)
	printf("%s 64 Bit version\n", argv[0]);
    else if (sizeof(Word_t) == 4)
	printf("%s 32 Bit version\n", argv[0]);

//////////////////////////////////////////////////////////////
// CALCULATE NUMBER OF MEASUREMENT GROUPS
//////////////////////////////////////////////////////////////

//  Calculate Multiplier for number of points per decade
    Mult = pow(10.0, 1.0 / (double)PtsPdec);
    {
	double sum;
	Word_t numb, prevnumb;

//      Count number of measurements needed (10K max)
	sum = numb = 1;
	for (Groups = 2; Groups < 10000; Groups++)
	    if (NextNumb(&numb, &sum, Mult, nElms))
		break;

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
    }				// Groups = number of sizes

//////////////////////////////////////////////////////////////
// BEGIN TESTS AT EACH GROUP SIZE
//////////////////////////////////////////////////////////////

//  Get the kicker to test the LFSR
    FirstSeed = Seed = StartSeed & (RandomBit * 2 - 1);

    printf("Total Pop Total Ins New Ins Total Del");
    printf(" J1MU/I JLMU/I\n");

#ifdef testLFSR
{
    Word_t Seed1  = Seed;

    while(1)
    {
	Seed1 = GetNextIndex(Seed1);
	TotalPop++;
	if (TotalPop > 40000000000) printf("Total = %lu\n", TotalPop), exit(1);
    }
}
#endif // testLFSR

    for (grp = 0; grp < Groups; grp++)
    {
	Word_t LowIndex, HighIndex;
	Word_t Delta;
	Word_t NewSeed;

	Delta = Pms[grp].ms_delta;

//      Test JLI, J1S
	NewSeed = TestJudyIns(&J1, &JL, Seed, Delta);

//      Test JLG, J1T
	LowIndex = TestJudyGet(J1, JL, Seed, Delta);

//      Test JLI, J1S -dup
	LowIndex = TestJudyDup(&J1, &JL, Seed, Delta);

//      Test JLC, J1C
	if (!CFlag)
	{
	    TestJudyCount(J1, JL, LowIndex, Delta);
	}
//      Test JLN, J1N
	HighIndex = TestJudyNext(J1, JL, LowIndex, Delta);

//      Test JLP, J1P
	TestJudyPrev(J1, JL, HighIndex, Delta);

//      Test JLNE, J1NE
	TestJudyNextEmpty(J1, JL, LowIndex, Delta);

//      Test JLPE, J1PE
	TestJudyPrevEmpty(J1, JL, HighIndex, Delta);

//      Test JLD, J1U
	if (dFlag)
	{
	    TestJudyDel(&J1, &JL, Seed, Delta);
	}

	printf("%9lu %9lu %7lu %9lu", TotalPop, TotalIns, Delta, TotalDel);
	{
	    Word_t Count1, CountL;

//	    Print the number of bytes used per Index
	    J1C(Count1, J1, 0, ~0);
 	    printf(" %6.3f", (double)Judy1MemUsed(J1) / (double)Count1);
	    JLC(CountL, JL, 0, ~0);
 	    printf(" %6.3f", (double)JudyLMemUsed(JL) / (double)CountL);
	}
	printf("\n");

//      Advance Index number set
	Seed = NewSeed;
    }
    {
	Word_t Count1, CountL;
	Word_t Bytes;

	JLC(CountL, JL, 0, ~0);
	J1C(Count1, J1, 0, ~0);

	if (CountL != TotalPop)
	    FAILURE("JudyLCount wrong", CountL);

	if (Count1 != TotalPop)
	    FAILURE("Judy1Count wrong", Count1);

	if (TotalPop)
	{
	    J1FA(Bytes, J1);	// Free the Judy1 Array
	    printf("Judy1FreeArray = %6.3f Bytes/Index\n",
		   (double)Bytes / (double)Count1);

	    if (pFlag) { printf("J1FA: %8lu\tbytes = %lu\n", TotalPop, Bytes); }

	    JLFA(Bytes, JL);	// Free the JudyL Array
	    printf("JudyLFreeArray = %6.3f Bytes/Index\n",
		   (double)Bytes / (double)CountL);

	    if (pFlag) { printf("JLFA: %8lu\tbytes = %lu\n", TotalPop, Bytes); }

	    TotalPop = 0;
	}
    }
    printf("Passed JudyL and Judy1 tests\n");
    exit(0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyIns"

Word_t
TestJudyIns(void **J1, void **JL, Word_t Seed, Word_t Elements)
{
    Word_t TstIndex;
    Word_t elm;
    Word_t *PValue, *PValue1;
    Word_t Seed1;
    int Rcode;

    for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
    {
	Seed1 = GetNextIndex(Seed1);
	if (Seed1 == 0)
	    FAILURE("This command not robust if Index == 0", elm);

	if (DFlag)
	    TstIndex = Swizzle(Seed1);
	else
	    TstIndex = Seed1;

	if (pFlag) { printf("Ins: %8lu\t0x%lx\n", elm, TstIndex); }

	J1S(Rcode, *J1, TstIndex);
	if (Rcode == JERR)
	    FAILURE("Judy1Set failed at", elm);
	if (Rcode == 0)
	    FAILURE("Judy1Set failed - DUP Index, population =", TotalPop);

	J1T(Rcode, *J1, TstIndex);
	if (Rcode != 1)
	    FAILURE("Judy1Test failed - Index missing, population =", TotalPop);

	J1S(Rcode, *J1, TstIndex);
	if (Rcode != 0)
	    FAILURE("Judy1Set failed - Index missing, population =", TotalPop);

	JLI(PValue, *JL, TstIndex);
	if (PValue == PJERR)
	    FAILURE("JudyLIns failed at", elm);
	if (*PValue == TstIndex)
	    FAILURE("JudyLIns failed - DUP Index, population =", TotalPop);

//      Save Index in Value
	*PValue = TstIndex;

	JLG(PValue1, *JL, TstIndex);
	if (PValue != PValue1)
	    FAILURE("JudyLGet failed - Index missing, population =", TotalPop);

	JLI(PValue1, *JL, TstIndex);
	if (PValue != PValue1)
	{
	    if (*PValue1 != TstIndex)
	    {
	       FAILURE("JudyLIns failed - Index missing, population =", TotalPop);
	    }
	    else
	    {
// not ready for this yet! printf("Index moved -- TotalPop = %lu\n", TotalPop);
	    }
	}
	TotalPop++;
	TotalIns++;
    }
    return (Seed1);		// New seed
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyGet"

Word_t
TestJudyGet(void *J1, void *JL, Word_t Seed, Word_t Elements)
{
    Word_t LowIndex = ~0UL;
    Word_t TstIndex;
    Word_t elm;
    Word_t *PValue;
    Word_t Seed1;
    int Rcode;

    for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
    {
	Seed1 = GetNextIndex(Seed1);

	if (DFlag)
	    TstIndex = Swizzle(Seed1);
	else
	    TstIndex = Seed1;

	if (TstIndex < LowIndex)
	    LowIndex = TstIndex;

	J1T(Rcode, J1, TstIndex);
	if (Rcode != 1)
	    FAILURE("Judy1Test Rcode != 1", (Word_t) Rcode);

	JLG(PValue, JL, TstIndex);
	if (PValue == (Word_t *) NULL)
	    FAILURE("JudyLGet ret PValue = NULL", 0L);
	if (*PValue != TstIndex)
	    FAILURE("JudyLGet ret wrong Value at", elm);
    }

    return(LowIndex);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyDup"

Word_t
TestJudyDup(void **J1, void **JL, Word_t Seed, Word_t Elements)
{
    Word_t LowIndex = ~0UL;
    Word_t TstIndex;
    Word_t elm;
    Word_t *PValue;
    Word_t Seed1;
    int Rcode;

    for (Seed1 = Seed, elm = 0; elm < Elements; elm++)
    {
	Seed1 = GetNextIndex(Seed1);

	if (DFlag)
	    TstIndex = Swizzle(Seed1);
	else
	    TstIndex = Seed1;

	if (TstIndex < LowIndex)
	    LowIndex = TstIndex;

	J1S(Rcode, *J1, TstIndex);
	if (Rcode != 0)
	    FAILURE("Judy1Set Rcode != 0", (Word_t) Rcode);

	JLI(PValue, *JL, TstIndex);
	if (PValue == (Word_t *) NULL)
	    FAILURE("JudyLIns ret PValue = NULL", 0L);
	if (*PValue != TstIndex)
	    FAILURE("JudyLIns ret wrong Value at", elm);
    }

    return(LowIndex);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyCount"

int
TestJudyCount(void *J1, void *JL, Word_t LowIndex, Word_t Elements)
{
    Word_t elm;
    Word_t Count1, CountL;
    Word_t TstIndex = LowIndex;
    int Rcode;

    TstIndex = LowIndex;
    for (elm = 0; elm < Elements; elm++)
    {
	J1C(Count1, J1, LowIndex, TstIndex);
	if (Count1 == JERR)
	    FAILURE("Judy1Count ret JERR", (Word_t) Count1);

	if (Count1 != (elm + 1))
	{
	    J1C(CountL, J1, 0, -1);
	    printf("J1C(%lu, J1, 0, -1)\n", CountL);

	    JLC(CountL, JL, 0, -1);
	    printf("JLC(%lu, JL, 0, -1)\n", CountL);

	    printf("LowIndex = 0x%lx, TstIndex = 0x%lx, diff = %lu\n", LowIndex,
		   TstIndex, TstIndex - LowIndex);
	    JLC(CountL, JL, LowIndex, TstIndex);
	    printf("CountL = %lu, Count1 = %lu, should be: elm + 1 = %lu\n", CountL, Count1, elm + 1);
	    FAILURE("J1C at", elm);
	}

	JLC(CountL, JL, LowIndex, TstIndex);
	if (CountL == JERR)
	    FAILURE("JudyLCount ret JERR", (Word_t) CountL);

	if (CountL != (elm + 1)) FAILURE("JLC at", elm);

	J1N(Rcode, J1, TstIndex);
    }
    return(0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyNext"

Word_t TestJudyNext(void *J1, void *JL, Word_t LowIndex, Word_t Elements)
{
    Word_t JLindex, J1index;
    Word_t *PValue;
    Word_t elm;
    int Rcode;

//  Get an Index low enough for Elements
    J1index = JLindex = LowIndex;

    JLF(PValue, JL, JLindex);
    J1F(Rcode, J1, J1index);

    for (elm = 0; elm < Elements; elm++)
    {
	if (PValue == NULL)
	    FAILURE("JudyLNext ret NULL PValue at", elm);
	if (Rcode != 1)
	    FAILURE("Judy1Next Rcode != 1 =", (Word_t) Rcode);
	if (JLindex != J1index)
	    FAILURE("Judy1Next & Judy1Next ret different PIndex at", elm);

	JLN(PValue, JL, JLindex);	// Get next one
	J1N(Rcode, J1, J1index);	// Get next one
    }
//  perhaps a check should be done here -- if I knew what to expect.
    return(JLindex);		// return last one
}


#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyPrev"

int
TestJudyPrev(void *J1, void *JL, Word_t HighIndex, Word_t Elements)
{
    Word_t JLindex, J1index;
    Word_t *PValue;
    Word_t elm;
    int Rcode;

//  Get an Index high enough for Elements
    J1index = JLindex = HighIndex;

    JLL(PValue, JL, JLindex);
    J1L(Rcode, J1, J1index);

    for (elm = 0; elm < Elements; elm++)
    {
	if (PValue == NULL)
	    FAILURE("JudyLPrev ret NULL PValue at", elm);
	if (Rcode != 1)
	    FAILURE("Judy1Prev Rcode != 1 =", (Word_t) Rcode);
	if (JLindex != J1index)
	    FAILURE("Judy1Prev & Judy1Prev ret different PIndex at", elm);

	JLP(PValue, JL, JLindex);	// Get previous one
	J1P(Rcode, J1, J1index);	// Get previous one
    }
//  perhaps a check should be done here -- if I knew what to expect.
    return(0);
}


#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyNextEmpty"

int
TestJudyNextEmpty(void *J1, void *JL, Word_t LowIndex, Word_t Elements)
{
    Word_t elm;
    Word_t JLindex, J1index;
    Word_t Seed1;
    int Rcode;		// Return code

//  Set 1st search to  ..
    Seed1 = LowIndex;
    J1index = JLindex = Seed1;

    for (elm = 0; elm < Elements; elm++)
    {
        Word_t *PValue;

//      Find next Empty Index, JLindex is modified by JLNE
	JLNE(Rcode, JL, JLindex);	// Rcode = JudyLNextEmpty(JL, &JLindex, PJE0)
	if (Rcode != 1)
	    FAILURE("JudyLNextEmpty Rcode != 1 =", (Word_t) Rcode);

	if (pFlag) { printf("JNE: %8lu\t0x%lx\n", elm, JLindex); }

//      Find next Empty Index, J1index is modified by J1NE
	J1NE(Rcode, J1, J1index);	// Rcode = Judy1NextEmpty(J1, &J1index, PJE0)
	if (Rcode != 1)
	    FAILURE("Judy1NextEmpty Rcode != 1 =", (Word_t) Rcode);

	if (J1index != JLindex)
	    FAILURE("JLNE != J1NE returned index at", elm);

	J1T(Rcode, J1, J1index);
	if (Rcode != 0)
	    FAILURE("J1NE returned non-empty Index =", J1index);

	JLG(PValue, JL, JLindex);
	if (PValue != (Word_t *) NULL)
	    FAILURE("JLNE returned non-empty Index =", JLindex);
	
	Seed1 = GetNextIndex(Seed1);
        J1index = JLindex = Seed1;
    }
    return(0);
}


// Routine to JudyPrevEmpty routines

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyPrevEmpty"

int
TestJudyPrevEmpty(void *J1, void *JL, Word_t HighIndex, Word_t Elements)
{
    Word_t elm;
    Word_t JLindex, J1index;
    Word_t Seed1;
    int Rcode;

//  Set 1st search to  ..
    Seed1 = HighIndex;
    J1index = JLindex = Seed1;

    for (elm = 0; elm < Elements; elm++)
    {
        Word_t *PValue;

	J1PE(Rcode, J1, J1index);	// Rcode = Judy1PrevEmpty(J1, &J1index, PJE0)
	if (Rcode != 1)
	    FAILURE("Judy1PrevEmpty Rcode != 1 =", (Word_t) Rcode);

	if (pFlag) { printf("JPE: %8lu\t0x%lx\n", elm, J1index); }

//      Find next Empty Index, JLindex is modified by JLPE
	JLPE(Rcode, JL, JLindex);	// Rcode = JudyLPrevEmpty(JL, &JLindex, PJE0)
	if (Rcode != 1)
	    FAILURE("JudyLPrevEmpty Rcode != 1 =", (Word_t) Rcode);

	if (J1index != JLindex)
	    FAILURE("JLPE != J1PE returned index at", elm);

	J1T(Rcode, J1, J1index);
	if (Rcode != 0)
	    FAILURE("J1PE returned non-empty Index =", J1index);

	JLG(PValue, JL, JLindex);
	if (PValue != (Word_t *) NULL)
	    FAILURE("JLPE returned non-empty Index =", JLindex);

	Seed1 = GetNextIndex(Seed1);
        J1index = JLindex = Seed1;
    }
    return(0);
}

#undef __FUNCTI0N__
#define __FUNCTI0N__ "TestJudyDel"

int
TestJudyDel(void **J1, void **JL, Word_t Seed, Word_t Elements)
{
    Word_t TstIndex;
    Word_t elm;
    Word_t Seed1;
    int Rcode;

//  Only delete half of thoes inserted
    for (Seed1 = Seed, elm = 0; elm < (Elements / 2); elm++)
    {
	Seed1 = GetNextIndex(Seed1);

	if (DFlag)
	    TstIndex = Swizzle(Seed1);
	else
	    TstIndex = Seed1;

	if (pFlag) { printf("Del: %8lu\t0x%lx\n", elm, TstIndex); }

	TotalDel++;

	J1U(Rcode, *J1, TstIndex);
	if (Rcode != 1)
	    FAILURE("Judy1Unset ret Rcode != 1", (Word_t) Rcode);

	JLD(Rcode, *JL, TstIndex);
	if (Rcode != 1)
	    FAILURE("JudyLDel ret Rcode != 1", (Word_t) Rcode);

	TotalPop--;
    }
    return(0);
}

// Routine to get next size of Indexes
int				// return 1 if last number
NextNumb(Word_t * PNumber,	// pointer to returned next number
	 double *PDNumb,	// Temp double of above
	 double DMult,		// Multiplier
	 Word_t MaxNumb)	// Max number to return
{
    Word_t num;

//  Save prev number
    Word_t PrevNumb = *PNumber;

//  Verify integer number increased
    for (num = 0; num < 1000; num++)
    {
//      Calc next number
	*PDNumb *= DMult;

//      Return it in integer format
	*PNumber = (Word_t) (*PDNumb + 0.5);

	if (*PNumber != PrevNumb)
	    break;
    }

//  Verify it did exceed max ulong
    if ((*PDNumb + 0.5) > (double)(-1UL))
    {
//      It did, so return max number
	*PNumber = -1UL;
	return (1);		// flag it
    }

//  Verify it did not exceed max number
    if ((*PDNumb + 0.5) > (double)MaxNumb)
    {
//      it did, so return max
	*PNumber = MaxNumb;
	return(1);		// flag it
    }
    return(0);			// more available
}
