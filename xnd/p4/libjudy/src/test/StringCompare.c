// @(#) $Revision: 4.1 $ $Source: /judy/test/manual/StringCompare.c $
//=======================================================================
//   Author Douglas L. Baskins, Jan 2003.
//   Permission to use this code is freely granted, provided that this
//   statement is retained.
//   email - dougbaskins .at, yahoo.com
//=======================================================================

//=======================================================================
//
// This program will time various ADTs that store and retrieve strings.
// Currently there are 7 ADTs implemented:

/*
   1) Hash   - this is the fastest one when table size matches data size
   2) JudyHS - 2nd in speed and very scaleable.
   3) JLHash - this uses a JudyL() array instead of hash table and is
        very scaleable because the hash table size is ~4 billion.
   4) JudySL - ordered and requires null terminated strings.
   5) Ternary - code borrowed from Mr Dobbs, perhaps 2000
   6) Redblack - code borrowed from J. Zobel, April 2001.
   7) Splay  - code borrowed from J. Zobel, April 2001.

   Note: Splay, Redblack and Ternary methods are not very fast, so they
   have not been completed and made bug free.  I.E. no Delete and Free
   Array routines.  Ternary is fastest retrieve of these three, but uses
   an extraordinary amount of memory.
*/
//=======================================================================
//
// Compile: 
//
//   cc -O StringCompare.c -lm -lJudy -o StringCompare 
//           -or-
//   cc -O -DCPUMHZ=1299 StringCompare.c -lm -lJudy -o StringCompare

/* Notes:  
   1) Use '-DCPUMHZ=1299' in cc line if better clock resolution is desired
      and it compiles successfully.  The 1299 is the cpu MHz : 1298.916 from
      cat /proc/cpuinfo in a Linux system.

   2) -static will generally get better performance because memcmp(),
    memcpy() routines are usually slower with shared librarys.
*/

// Usage:
//
//   StringCompare -A Hash     <options>  textfile
//   StringCompare -A JudyHS   <options>  textfile
//   StringCompare -A JLHash   <options>  textfile
//   StringCompare -A JudySL   <options>  textfile
//   StringCompare -A Ternary  <options>  textfile
//   StringCompare -A Redblack <options>  textfile
//   StringCompare -A Splay    <options>  textfile

#include <stdlib.h>                     // malloc(3)
#include <unistd.h>                     // getopt(3)
#include <stdio.h>                      // printf(3)
#include <fcntl.h>                      // open(2)
#include <string.h>                     // memcmp(3), memcpy(3)
#include <errno.h>                      // errno(3)
#include <sys/mman.h>                   // mmap(2)
#include <sys/time.h>                   // gettimeofday(2)
#include <math.h>                       // pow(3)
#include <sys/utsname.h>                // uname(2)
#include <Judy.h>                       // Judy arrays

//=======================================================================
//   D e f i n e:    T O   T U R N   O F F   A S S E R T I O N S !!!!!!!!     
//=======================================================================
//#define NDEBUG 1
#include <assert.h>                     // assert(3)

//=======================================================================
//      G L O B A L  D A T A
//=======================================================================

int       foolflag = 0;                 // fool compiler from optimizing
static Word_t gStored = 0;              // number of strings inserted
static Word_t gChainln = 0;             // links traversed during RETRIVE

static Word_t PtsPdec = 40;             // default measurement points per decade

#define INFSTRGS 1000000000             // 1 billion strings is infinity

static Word_t nStrg = INFSTRGS;         // infinity -- measure all strings
static Word_t TValues = 100000;         // max measure points for RETRIVE tests
static int pFlag = 0;                   // pre-fault hash table pages into RAM
static int rFlag = 0;                   // do not randomize input file
static int aFlag = 0;                   // word align string buffers
static int DFlag = 0;                   // do the delete measurement
static int CFlag = 0;                   // build sequential Get buffers
static Word_t aCount = 0;               // Count of missaligned string buffers

//  define the maximum length of a string allowed
#define MAXSTRLEN       (100000)
static int MLength = MAXSTRLEN;
static Word_t HTblsz;                   // 1M default hash table size
static int fileidx;                     // argv[fileidx] == file string

// for saving input string data
typedef struct STRING_
{
    int       dt_strlen;
    uint8_t  *dt_string;

} dt_t   , *Pdt_t;

static Pdt_t PdtS_ = NULL;              // memory for Cache access Gets
static uint8_t *Strbuf_ = NULL;
static Word_t Strsiz_ = 0;

// Roundup BYTES to an even number of words

/*
 On Linux 2.6.3-4mdkenterprise (Mandrake 10.0 Community) printing (even
 to a file) makes the timings inaccurate.  So, use -L2 or greater to
 average (actually save min times) and print results after all tests are
 completed.
*/
#define Printf if (Pass == 0) printf

#define ROUNDUPWORD(BYTES) (((BYTES) + sizeof(Word_t) - 1) & (-sizeof(Word_t)))
#define BYTES2WORDS(BYTES) (((BYTES) + sizeof(Word_t) - 1) / (sizeof(Word_t)))

//=======================================================================
//      T I M I N G   M A C R O S
//=======================================================================

static double DeltaUSec;                // Global for remembering delta times

// Some operating systems have get_cycles() in /usr/include/asm/timex.h

#ifdef  CPUMHZ

//  For a 1.34 nS clock cycle processor (750Mhz)

#define CPUSPEED      (1.0 / (CPUMHZ))

#include <asm/timex.h>

#define TIMER_vars(T) cycles_t  __TVBeg_##T

#define STARTTm(T) __TVBeg_##T = get_cycles()

#define ENDTm(D,T) { (D) = (double)(get_cycles() - __TVBeg_##T) * CPUSPEED; }

#else  // ! CPUMHZ

#define TIMER_vars(T) struct timeval __TVBeg_##T, __TVEnd_##T

#define STARTTm(T) gettimeofday(&__TVBeg_##T, NULL)

#define ENDTm(D,T)                                                      \
{                                                                       \
    gettimeofday(&__TVEnd_##T, NULL);                                   \
    (D) = (double)(__TVEnd_##T.tv_sec  - __TVBeg_##T.tv_sec) * 1E6 +    \
         ((double)(__TVEnd_##T.tv_usec - __TVBeg_##T.tv_usec));         \
}

#endif // ! CPUMHZ

//=======================================================================
//      M E M O R Y   S I Z E   M A C R O S
//=======================================================================

// use mallinfo() instead of sbrk() for memory usage measurements
// this should include the RAM that was mmap()ed in malloc()

static Word_t DeltaMem;                 // for remembering

// Some mallocs have mallinfo()
// #define MALLINFO 1

#ifdef MALLINFO
#include <malloc.h>                     // mallinfo()

static struct mallinfo malStart;

#define STARTmem malStart = mallinfo()
#define ENDmem(DELTAMEM)                                        \
{                                                               \
    struct mallinfo malEnd = mallinfo();                        \
/* strange little dance from signed to unsigned to double */    \
    unsigned int _un_int = malEnd.arena - malStart.arena;       \
    (DELTAMEM) = (double)_un_int;      /* to double */          \
}
#else  // NO MALLINFO

// this usually works for machines with less than 1-2Gb RAM.
// (it does NOT include memory ACQUIRED by mmap())

static char *malStart;

#define STARTmem (malStart = (char *)sbrk(0))
#define ENDmem(DELTAMEM)                                        \
{                                                               \
    char  *malEnd =  (char *)sbrk(0);                           \
    (DELTAMEM) = (double)(malEnd - malStart);                   \
}
#endif // NO MALLINFO

//=======================================================================
//      F I L E  O P E N  and  M A L L O C  F A I L  M A C R O S
//=======================================================================

#define FILERROR                                                        \
{                                                                       \
    printf("\n !! OOps - Open file error \"%s\": %s (errno = %d)\n",    \
            argv[fileidx], strerror(errno), errno);                     \
    fprintf(stderr, " OOps - Open file error \"%s\": %s (errno = %d)\n",\
            argv[fileidx], strerror(errno), errno);                     \
    exit(1);                                                            \
}

#define MALLOCERROR                                                     \
{                                                                       \
    printf("\n !! OOps - malloc failed at Line = %d\n", __LINE__);      \
    fprintf(stderr, " OOps - malloc failed at Line = %d\n", __LINE__);  \
    exit(1);                                                            \
}

//=======================================================================
// This alternate form of JudyMalloc() is used to keep track how much ram is 
// used on some of the below ADT's
//=======================================================================

// JUDY INCLUDE FILES
//#include "Judy.h"

// ****************************************************************************
// J U D Y   M A L L O C
//
// Allocate RAM.  This is the single location in Judy code that calls
// malloc(3C).  Note:  JPM accounting occurs at a higher level.

static Word_t TotalJudyMalloc = 0;

Word_t
JudyMalloc(Word_t Words)
{
    Word_t    Addr;

    Addr = (Word_t)malloc(Words * sizeof(Word_t));

    if (Addr)
        TotalJudyMalloc += Words;

    return (Addr);

}                                       // JudyMalloc()

// ****************************************************************************
// J U D Y   F R E E

void
JudyFree(void *PWord, Word_t Words)
{
    free(PWord);
    assert((long)(TotalJudyMalloc - Words) >= 0L);

    TotalJudyMalloc -= Words;

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
// Routine to get next size of Indexes
//=======================================================================

static int
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
    }
    while (DDiff < 0.5);

//  Return it in integer format
    if (DDiff < 100.0)
        *PNumber += (Word_t)(DDiff + 0.5);
    else
        *PNumber = (Word_t)(*PDNumb + 0.5);

//  Verify it did not exceed max number
    if (*PNumber >= MaxNumb)
    {
        *PNumber = MaxNumb;             // it did, so return max
        return (1);                     // flag it
    }
    return (0);                         // more available
}

//=======================================================================
//      M E A S U R E M E N T  S T R U C T U R E
//=======================================================================

typedef struct _MEASUREMENTS_STRUCT *Pms_t;
typedef struct _MEASUREMENTS_STRUCT
{
    Word_t    ms_delta;                 // number of points in current group
    double    ms_Bytes;                 // average allocated memory/per string
    double    ms_mininsert;             // Min Retrive number
    double    ms_minretrive;            // Min Retrive number
} ms_t;

static Pms_t Pms;                       // array of MEASUREMENTS_STRUCT

// Method type

typedef enum
{
    M_invalid,
    M_Print,
    M_Hash,
    M_JLHash,
    M_JudySL,
    M_JudyHS,
    M_Splay,
    M_Redblack,
    M_Ternary
} Method_t;

//=======================================================================
//      R a n d o m i z e  i n p u t  s t r i n g s
//=======================================================================

static void
Randomize(Pdt_t Pstrstr, Word_t Len)
{
    Word_t    ii;

// swap the "random" index with the sequential one
    for (ii = 1; ii < Len; ii++)
    {
        dt_t      dttemp;
        Word_t    swapii;

//      get "random" index
        swapii = (Word_t)rand() % Len;

//      and swap
        dttemp = Pstrstr[ii];
        Pstrstr[ii] = Pstrstr[swapii];
        Pstrstr[swapii] = dttemp;
    }
}

//=======================================================================
//      B u i l d   s e q u e n c i a l   s t r i n g  b u f f e r
//=======================================================================

Pdt_t
BuildSeqBuf(Pdt_t Pstrstr, Word_t Len)
{
    Word_t    SumStrings = 0;
    Word_t    ii;
    Word_t    Strlen;
    uint8_t  *string;

    assert(Len <= TValues);

//  calculate how much memory needed for strings
    for (ii = 0; ii < Len; ii++)
    {
        Strlen = Pstrstr[ii].dt_strlen;
        if (aFlag)
            SumStrings += ROUNDUPWORD(Strlen + 1);
        else
            SumStrings += Strlen + 1;
    }
//  check if old string buffer is big enough
    if (SumStrings > Strsiz_)
    {
        if (Strbuf_)
            free(Strbuf_);
        else
            SumStrings += SumStrings / 5;       // bump 20%

        Strbuf_ = (uint8_t *) malloc(SumStrings);
        if (Strbuf_ == NULL)
            MALLOCERROR;
        Strsiz_ = SumStrings;
    }
    for (ii = 0, string = Strbuf_; ii < Len; ii++)
    {
        Strlen = Pstrstr[ii].dt_strlen;

        PdtS_[ii].dt_strlen = Strlen;
        PdtS_[ii].dt_string = string;

        memcpy(string, Pstrstr[ii].dt_string, Strlen + 1);

        if (aFlag)
            string += ROUNDUPWORD(Strlen + 1);
        else
            string += Strlen + 1;
    }
    return (PdtS_);
}

//=======================================================================
//      H A S H   M E T H O D   S T R U C T U R E S
//=======================================================================

// These structures are used in Hash() and JLHash() ADTs

// for storing length of string

// Hash chain structure (varible length depending on string)
// static part of the length
#define HSTRUCTOVD      (sizeof(hrec_t) - sizeof(int))
typedef struct HASHREC_ *Phrec_t;
typedef struct HASHREC_
{
    Phrec_t   hr_Next;                  // collision chain link pointer
    Word_t    hr_Value;                 // Data associated with string
    int       hr_Strlen;                // length of string 2 billion max
    uint8_t   hr_String[sizeof(int)];   // string is allocated with struct

} hrec_t;

//  hash head structure to keep hash array information
typedef struct HASHINFO_
{
    Pvoid_t   hi_Htbl;                  // Hash table
    Word_t    hi_tblsize;               // Hash table size (Words)
    Word_t    hi_TotalWords;            // Hash array total words
    Word_t    hi_Pop1;                  // Hash array total population

} hinfo_t, *Phinfo_t;

// size in words of the header structure
#define HASHHEADSZ  (sizeof(hinfo_t) / sizeof(Word_t))

//=======================================================================
//      H A S H   A L G O R I T H M
//=======================================================================
//
//  For CPUs with a slow mod (%) use table size a power of 2.  A test is
//  made to see if the SIZE is a power of 2, and if so an .AND.(&) is used
//  instead of a .MOD.(%) to trim the hash return size.  Note: a SIZE == 0,
//  results in no trimming of hash return size.

#define HASHSTR(STRING,LENGTH,SIZE)                     \
    ((SIZE) == ((SIZE) & -(SIZE))) ?                    \
        (HashStr(STRING, LENGTH) & ((SIZE) -1)) :       \
        (HashStr(STRING, LENGTH) % (SIZE))

//  String hash function.  Hash string to a unsigned int (uint32_t) This
//  one needs a fast 32 bit mpy, which is often very slow on older(RISC)
//  machines.  If you are sure you will not over populate the hash table,
//  then a poorer/faster hash algorithm should be used.  Replace with your
//  own, milage may vary.  This program measures the speed, whether used
//  or not.

static uint32_t
HashStr(void *Str, Word_t Len)
{
    uint32_t  A = 31415;
    uint32_t  hashv = Len;
    uint8_t  *k = (uint8_t *) Str;

    while (Len--)
    {
        hashv = (A * hashv) + *k++;
        A *= 27183;
    }
    return (hashv);
}

//=======================================================================
//      S T O R E  and  R E T R I V E  R O U T I N E S
//=======================================================================

//=======================================================================
//      H A S H  M E T H O D  U S I N G  J U D Y L  A S  H A S H  T A B L E
//=======================================================================

PWord_t
JLHashGet(Pvoid_t JLHash, uint8_t * String, Word_t Strlen)
{
    Phinfo_t  PHash = (Phinfo_t) JLHash;
    Phrec_t   Phrec, *PPhrec;
    uint32_t  hval;

    if (PHash == NULL)
        return (NULL);

//  get hash value, if mod(%) is slow (in some CPUs), make it a power of 2
    hval = HASHSTR(String, Strlen, PHash->hi_tblsize);

    JLG(PPhrec, PHash->hi_Htbl, hval);  // use JudyL to get &pointer

    if (PPhrec == NULL)
        return (NULL);                  // no table entry

//  search for matching string
    for (Phrec = *PPhrec; Phrec != NULL; Phrec = Phrec->hr_Next)
    {
        gChainln++;                     // Hash chain length
        if (Phrec->hr_Strlen == Strlen) // length match?
        {
            if (memcmp(Phrec->hr_String, String, Strlen) == 0)
                return (&(Phrec->hr_Value));    // match! pointer to Value
        }
    }
    return (NULL);
}

// Return pointer to struct hrec_t associated with string

PWord_t
JLHashIns(PPvoid_t PPHash, uint8_t * String, Word_t Strlen, Word_t TblSize)
{
    Phrec_t   Phrec, *PPhrec;
    Phinfo_t  PHash;
    Word_t    Len;
    uint32_t  hval;

    PHash = (Phinfo_t) * PPHash;        // core-dump if calling error
    if (PHash == NULL)                  // if hash table not allocated 
    {
//      allocate the header 
        PHash = (Phinfo_t) JudyMalloc(HASHHEADSZ);
        if (PHash == NULL)
            MALLOCERROR;

//      Initialize the header struct
        PHash->hi_tblsize = TblSize;
        PHash->hi_TotalWords = HASHHEADSZ;
        PHash->hi_Pop1 = 0;             // none yet
        PHash->hi_Htbl = NULL;
        *PPHash = (Pvoid_t)PHash;       // return header to caller
    }
//  get hash value, if mod(%) is slow (in some CPUs), make it a power of 2
    hval = HASHSTR(String, Strlen, PHash->hi_tblsize);

//  get pointer to hash table entry
    JLI(PPhrec, PHash->hi_Htbl, hval);  // JLI will exit if out of memory

//  search for matching string
    for (Phrec = *PPhrec; Phrec != NULL; Phrec = Phrec->hr_Next)
    {
        if (Phrec->hr_Strlen == Strlen) // string length match?
        {
            if (memcmp(Phrec->hr_String, String, Strlen) == 0)
            {
                return (&(Phrec->hr_Value));    // match! pointer to Value
            }
        }
    }

//  String match not found, so do an insert

    Len = BYTES2WORDS(Strlen + HSTRUCTOVD);
    Phrec = (Phrec_t) JudyMalloc(Len);  // get memory for storing string
    if (Phrec == NULL)
        MALLOCERROR;

    PHash->hi_TotalWords += Len;        // keep track of total mallocs

    Phrec->hr_Strlen = Strlen;          // set string length
    memcpy(Phrec->hr_String, String, Strlen);

    Phrec->hr_Next = *PPhrec;           // pointer to synonym
    *PPhrec = Phrec;                    // place new struct in front of list
    (PHash->hi_Pop1)++;                 // add one to population
    Phrec->hr_Value = (Word_t)0;        // zero the associated Value

    return (&(Phrec->hr_Value));        // return pointer to Value
}

// Return 1 if successful, else 0

int
JLHashDel(PPvoid_t PPHash, uint8_t * String, Word_t Strlen)
{
    Phrec_t   Phrec, *PPhrec, *PPhrec1;
    Phinfo_t  PHash;
    uint32_t  hval;

//  avoid an core dump here
    if (PPHash == NULL)
        return (0);
    PHash = (Phinfo_t) (*PPHash);       // get header
    if (PHash == NULL)
        return (0);                     // not found

//  get hash value, if mod(%) is slow (in some CPUs), make it a power of 2
    hval = HASHSTR(String, Strlen, PHash->hi_tblsize);

//  get pointer hash table entry
    JLG(PPhrec, PHash->hi_Htbl, hval);
    if (PPhrec == NULL)
        return (0);                     // hash entry not found

    PPhrec1 = PPhrec;                   // save head hash entry ^

//  search for matching string
    for (Phrec = *PPhrec; Phrec != NULL; Phrec = Phrec->hr_Next)
    {
        if (Phrec->hr_Strlen == Strlen) // string length match?
        {
            if (memcmp(Phrec->hr_String, String, Strlen) == 0)  // string match?
            {
                int       Rc;           // not used
                Word_t    Len;

                *PPhrec = Phrec->hr_Next;       // put next in previous

                Len = BYTES2WORDS(Strlen + HSTRUCTOVD);
                JudyFree(Phrec, Len);
                PHash->hi_TotalWords -= Len;    // ram usage accounting

                (PHash->hi_Pop1)--;     // Decrement population

                if (*PPhrec1 == NULL)   // no chain left
                {
//                  delete hash table entry
                    JLD(Rc, PHash->hi_Htbl, hval);
                    assert(Rc == 1);
                }
//              If last element, free everything
                if (PHash->hi_Pop1 == 0)
                {
                    assert(PHash->hi_TotalWords == HASHHEADSZ);

                    JudyFree(PHash, HASHHEADSZ);        // the header table
                    *PPHash = NULL;     // from caller
                }
                return (1);             // successful
            }
        }
        PPhrec = &(Phrec->hr_Next);     // previous = current
    }
    return (0);                         // string not found
}

// Free the whole JLHash structure

Word_t
JLHashFreeArray(PPvoid_t PPHash)
{
    Phrec_t   Phrec, *PPhrec;
    Phinfo_t  PHash;
    Word_t    DeletedWords, Bytes;
    Word_t    Index = 0;                // for First, Next loop

//  avoid an core dump here
    if (PPHash == NULL)
        return ((Word_t)0);
    PHash = (Phinfo_t) (*PPHash);       // get header
    if (PHash == NULL)
        return ((Word_t)0);             // not found

//  get bytes of memory usage in (JudyL) Hash table
    JLMU(Bytes, PHash->hi_Htbl);

    DeletedWords = HASHHEADSZ;          // start with header

//  Get 1st table entry in Hash table
    JLF(PPhrec, PHash->hi_Htbl, Index);

//  found an entry in hash table?
    while (PPhrec != NULL)
    {
        int       Rc;                   // not used
        Phrec = *PPhrec;

//      walk the synonym linked list
        while (Phrec != NULL)
        {
            Word_t    Len;
            Phrec_t   Phrecfree = Phrec;

//          number of words to free -- struct hrec_t
            Len = BYTES2WORDS(Phrec->hr_Strlen + HSTRUCTOVD);

//          sum total length of mallocs in words
            DeletedWords += Len;

            (PHash->hi_Pop1)--;         // Decrement population

//          get pointer to next synonym on list
            Phrec = Phrec->hr_Next;

//          free the struct hrec_t
            JudyFree(Phrecfree, Len);
        }
//      delete hash table entry
        JLD(Rc, PHash->hi_Htbl, Index);
        assert(Rc == 1);

//      get next hash table entry
        JLN(PPhrec, PHash->hi_Htbl, Index);
    }
    assert(PHash->hi_TotalWords == DeletedWords);
    assert(PHash->hi_Pop1 == 0);

    JudyFree(PHash, HASHHEADSZ);        // the header table
    *PPHash = NULL;                     // set pointer null

//  return total bytes freed
    return ((DeletedWords * sizeof(Word_t)) + Bytes);
}

//=======================================================================
//      H A S H  M E T H O D
//=======================================================================

PWord_t
HashGet(Phinfo_t PHash, uint8_t * String, Word_t Strlen)
{
    Phrec_t   Phrec, *Htbl;
    uint32_t  hval;

//  avoid an core dump here
    if (PHash == NULL)
        return (NULL);

//  get hash value, if mod(%) is slow (in some CPUs), make it a power of 2
    hval = HASHSTR(String, Strlen, PHash->hi_tblsize);

//  type Hash table pointer
    Htbl = (Phrec_t *) PHash->hi_Htbl;

//  search for matching string
    for (Phrec = Htbl[hval]; Phrec != NULL; Phrec = Phrec->hr_Next)
    {
        gChainln++;                     // Hash chain length
        if (Phrec->hr_Strlen == Strlen) // length match?
        {
            if (memcmp(Phrec->hr_String, String, Strlen) == 0)
                return (&(Phrec->hr_Value));    // match! pointer to Value
        }
    }
    return (NULL);
}

// Return pointer to struct hrec_t associated with string

Pvoid_t
HashIns(Phinfo_t * PPHash, uint8_t * String, Word_t Strlen, Word_t TblSize)
{
    Phrec_t   Phrec, *Htbl;
    Phinfo_t  PHash;
    Word_t    Len;
    uint32_t  hval;

    PHash = *PPHash;                    // core-dump if calling error
    if (PHash == NULL)                  // if hash table not allocated 
    {
//      allocate the header 
        PHash = (Phinfo_t) JudyMalloc(HASHHEADSZ);
        if (PHash == NULL)
            MALLOCERROR;

//      allocate the hash table
        PHash->hi_Htbl = (Pvoid_t)JudyMalloc(TblSize);
        if (PHash->hi_Htbl == NULL)
            MALLOCERROR;

//      you cant beat this with modern compilers/librarys
        memset(PHash->hi_Htbl, 0, TblSize * sizeof(Word_t));

//      Initialize the header struct
        PHash->hi_tblsize = TblSize;
        PHash->hi_TotalWords = TblSize + HASHHEADSZ;
        PHash->hi_Pop1 = 0;             // none yet
        *PPHash = PHash;                // return header to caller
    }
//  get hash value, if mod(%) is slow (in some CPUs), make it a power of 2
    hval = HASHSTR(String, Strlen, TblSize);

//  type Hash table pointer
    Htbl = (Phrec_t *) PHash->hi_Htbl;

//  search for matching string in hash table entry
    for (Phrec = Htbl[hval]; Phrec != NULL; Phrec = Phrec->hr_Next)
    {
        if (Phrec->hr_Strlen == Strlen) // string length match?
        {
            if (memcmp(Phrec->hr_String, String, Strlen) == 0)  // string match?
            {
                return (&(Phrec->hr_Value));    // match! pointer to Value
            }
        }
    }
//  string not found, so do an insert
    Len = BYTES2WORDS(Strlen + HSTRUCTOVD);
    Phrec = (Phrec_t) JudyMalloc(Len);
    if (Phrec == NULL)
        MALLOCERROR;
    PHash->hi_TotalWords += Len;        // keep track of total mallocs

    Phrec->hr_Strlen = Strlen;          // set string length
    memcpy(Phrec->hr_String, String, Strlen);   // copy it

//  place new allocation first in chain
    Phrec->hr_Next = Htbl[hval];
    Htbl[hval] = Phrec;

    (PHash->hi_Pop1)++;                 // add one to population
    Phrec->hr_Value = (Word_t)0;        // zero the associated Value

    return (&(Phrec->hr_Value));        // return pointer to Value
}

// Delete entry in hash array, return 1 if successful, else 0

int
HashDel(Phinfo_t * PPHash, uint8_t * String, Word_t Strlen)
{
    Phrec_t   Phrec, *PPhrec, *Htbl;
    Phinfo_t  PHash;
    uint32_t  hval;

//  avoid an core dump here
    if (PPHash == NULL)
        return (0);
    PHash = *PPHash;                    // get header
    if (PHash == NULL)
        return (0);                     // not found

//  get hash value, if mod(%) is slow (in some CPUs), make it a power of 2
    hval = HASHSTR(String, Strlen, PHash->hi_tblsize);

//  type Hash table pointer
    Htbl = (Phrec_t *) PHash->hi_Htbl;

//  get pointer hash table entry
    PPhrec = &Htbl[hval];

//  search for matching string
    for (Phrec = *PPhrec; Phrec != NULL; Phrec = Phrec->hr_Next)
    {
        if (Phrec->hr_Strlen == Strlen) // length match?
        {
            if (memcmp(Phrec->hr_String, String, Strlen) == 0)
            {
                Word_t    Len;

//              put next hrec_t in previous hrec_t
                *PPhrec = Phrec->hr_Next;

                Len = BYTES2WORDS(Strlen + HSTRUCTOVD);
                JudyFree(Phrec, Len);
                PHash->hi_TotalWords -= Len;
                (PHash->hi_Pop1)--;     // Decrement population

//              If last element, free everything
                if (PHash->hi_Pop1 == 0)
                {
                    assert(PHash->hi_TotalWords ==
                           (HASHHEADSZ + PHash->hi_tblsize));

                    JudyFree(Htbl, PHash->hi_tblsize);  // hash table
                    JudyFree(PHash, HASHHEADSZ);        // header struct
                    *PPHash = NULL;     // from caller
                }
                return (1);             // successful
            }
        }
        PPhrec = &(Phrec->hr_Next);     // previous = current
    }
    return (0);                         // not found
}

Word_t
HashFreeArray(Phinfo_t * PPHash)
{
    int       ii;
    Phrec_t   Phrec, *Htbl;
    Phinfo_t  PHash;
    Word_t    DeletedWords;

//  avoid an core dump here
    if (PPHash == NULL)
        return ((Word_t)0);
    PHash = (Phinfo_t) (*PPHash);       // get header
    if (PHash == NULL)
        return ((Word_t)0);

//  start accumulator of deleted memory
    DeletedWords = HASHHEADSZ + PHash->hi_tblsize;

//  type Hash table pointer
    Htbl = (Phrec_t *) PHash->hi_Htbl;

//  walk thru all table entrys
    for (ii = 0; ii < PHash->hi_tblsize; ii++)
    {
        Phrec = Htbl[ii];               // next hash table entry 

        while (Phrec != NULL)           // walk the synonym linked list
        {
            Word_t    Len;
            Phrec_t   Phrecfree;

//          get pointer to next synonym on list
            Phrecfree = Phrec;
            Phrec = Phrec->hr_Next;

            (PHash->hi_Pop1)--;         // Decrement population

//          number of words to free -- struct hrec_t
            Len = BYTES2WORDS(Phrecfree->hr_Strlen + HSTRUCTOVD);
            DeletedWords += Len;        // sum words freed

//          free the struct hrec_t
            JudyFree(Phrecfree, Len);
        }
    }

//  and free the hash table
    JudyFree(Htbl, PHash->hi_tblsize);
    assert(PHash->hi_TotalWords == DeletedWords);
    assert(PHash->hi_Pop1 == 0);

    JudyFree(PHash, HASHHEADSZ);        // the header table
    *PPHash = NULL;                     // set pointer null

//  return total bytes freed
    return (DeletedWords * sizeof(Word_t));
}

//=======================================================================
//      S P L A Y   M E T H O D
//=======================================================================

/* Author J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. */

#define ROTATEFAC 11

typedef struct spwordrec
{
    char     *word;
    struct spwordrec *left, *right;
    struct spwordrec *par;
} SPTREEREC;

typedef struct spansrec
{
    struct spwordrec *root;
    struct spwordrec *ans;
} SPANSREC;

SPANSREC  spans = { 0 };

#define ONELEVEL(PAR,CURR,DIR,RID)      \
    {                                   \
        PAR->DIR = CURR->RID;           \
        if(PAR->DIR!=NULL)              \
            PAR->DIR->PAR = PAR;        \
        CURR->RID = PAR;                \
        PAR->PAR = CURR;                \
        CURR->PAR = NULL;               \
    }

#define ZIGZIG(GPAR,PAR,CURR,DIR,RID)   \
    {                                   \
        CURR->PAR = GPAR->PAR;          \
        if (CURR->PAR != NULL)          \
        {                               \
            if (CURR->PAR->DIR == GPAR) \
                CURR->PAR->DIR = CURR;  \
            else                        \
                CURR->PAR->RID = CURR;  \
        }                               \
        GPAR->DIR = PAR->RID;           \
        if (GPAR->DIR != NULL)          \
            GPAR->DIR->PAR = GPAR;      \
        PAR->DIR = CURR->RID;           \
        if (CURR->RID != NULL)          \
            CURR->RID->PAR = PAR;       \
        CURR->RID = PAR;                \
        PAR->PAR = CURR;                \
        PAR->RID = GPAR;                \
        GPAR->PAR = PAR;                \
    }

#define ZIGZAG(GPAR,PAR,CURR,DIR,RID)   \
    {                                   \
        CURR->PAR = GPAR->PAR;          \
        if (CURR->PAR != NULL)          \
        {                               \
            if (CURR->PAR->DIR == GPAR) \
                CURR->PAR->DIR = CURR;  \
            else                        \
                CURR->PAR->RID = CURR;  \
        }                               \
        PAR->RID = CURR->DIR;           \
        if (PAR->RID != NULL)           \
            PAR->RID->PAR = PAR;        \
        GPAR->DIR = CURR->RID;          \
        if (GPAR->DIR != NULL)          \
            GPAR->DIR->PAR = GPAR;      \
        CURR->DIR = PAR;                \
        PAR->PAR = CURR;                \
        CURR->RID = GPAR;               \
        GPAR->PAR = CURR;               \
    }

int       scount = ROTATEFAC;

/* Create a node to hold a word */

static SPTREEREC *
spwcreate(char *word, SPTREEREC * par)
{
    SPTREEREC *tmp;

    tmp = (SPTREEREC *) malloc(sizeof(SPTREEREC));
    tmp->word = (char *)malloc(strlen(word) + 1);
    strcpy(tmp->word, word);
    tmp->left = tmp->right = NULL;

    tmp->par = par;

    gStored++;                          // count stored

    return (tmp);
}

/* Search for word in a splay tree.  If word is found, bring it to
   root, possibly intermittently.  Structure ans is used to pass
   in the root, and to pass back both the new root (which may or
   may not be changed) and the looked-for record. */

static void
splaysearch(SPANSREC * ans, char *word)
{
    SPTREEREC *curr = ans->root, *par, *gpar;
    int       val;

    scount--;

    if (ans->root == NULL)
    {
        ans->ans = NULL;
        return;
    }
    while (curr != NULL && (val = strcmp(word, curr->word)) != 0)
    {
        if (val > 0)
            curr = curr->right;
        else
            curr = curr->left;
    }

    ans->ans = curr;

    if (curr == ans->root)
    {
        return;
    }

    if (scount <= 0 && curr != NULL)    /* Move node towards root */
    {
        scount = ROTATEFAC;

        while ((par = curr->par) != NULL)
        {
            if (par->left == curr)
            {
                if ((gpar = par->par) == NULL)
                {
                    ONELEVEL(par, curr, left, right);
                }
                else if (gpar->left == par)
                {
                    ZIGZIG(gpar, par, curr, left, right);
                }
                else
                {
                    ZIGZAG(gpar, par, curr, right, left);
                }
            }
            else
            {
                if ((gpar = par->par) == NULL)
                {
                    ONELEVEL(par, curr, right, left);
                }
                else if (gpar->left == par)
                {
                    ZIGZAG(gpar, par, curr, left, right);
                }
                else
                {
                    ZIGZIG(gpar, par, curr, right, left);
                }
            }
        }
        ans->root = curr;
    }

    return;
}

/* Insert word into a splay tree.  If word is already present, bring it to
   root, possibly intermittently.  Structure ans is used to pass
   in the root, and to pass back both the new root (which may or
   may not be changed) and the looked-for record. */

static void
splayinsert(SPANSREC * ans, char *word)
{
    SPTREEREC *curr = ans->root, *par, *gpar, *prev = NULL, *spwcreate();
    int       val = 0;

    scount--;

    if (ans->root == NULL)
    {
        ans->ans = ans->root = spwcreate(word, NULL);
        return;
    }

    while (curr != NULL && (val = strcmp(word, curr->word)) != 0)
    {
        prev = curr;
        if (val > 0)
            curr = curr->right;
        else
            curr = curr->left;
    }

    if (curr == NULL)
    {
        if (val > 0)
            curr = prev->right = spwcreate(word, prev);
        else
            curr = prev->left = spwcreate(word, prev);
    }

    ans->ans = curr;

    if (scount <= 0)                    /* Move node towards root */
    {
        scount = ROTATEFAC;

        while ((par = curr->par) != NULL)
        {
            if (par->left == curr)
            {
                if ((gpar = par->par) == NULL)
                {
                    ONELEVEL(par, curr, left, right);
                }
                else if (gpar->left == par)
                {
                    ZIGZIG(gpar, par, curr, left, right);
                }
                else
                {
                    ZIGZAG(gpar, par, curr, right, left);
                }
            }
            else
            {
                if ((gpar = par->par) == NULL)
                {
                    ONELEVEL(par, curr, right, left);
                }
                else if (gpar->left == par)
                {
                    ZIGZAG(gpar, par, curr, left, right);
                }
                else
                {
                    ZIGZIG(gpar, par, curr, right, left);
                }
            }
        }
        ans->root = curr;
    }
    return;
}

//=======================================================================
//      R E D B L A C K   M E T H O D
//=======================================================================

/* Author J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. */

typedef struct rbwordrec
{
    char     *word;
    struct rbwordrec *left, *right;
    struct rbwordrec *par;
    char      colour;
} RBTREEREC;

typedef struct rbansrec
{
    struct rbwordrec *root;
    struct rbwordrec *ans;
} BRANSREC;

BRANSREC  rbans = { 0 };

#define RED                0
#define BLACK                1

/* Find word in a redblack tree */

static void
redblacksearch(BRANSREC * ans, char *word)
{
    RBTREEREC *curr = ans->root;
    int       val;

    if (ans->root != NULL)
    {
        while (curr != NULL && (val = strcmp(word, curr->word)) != 0)
        {
            if (val > 0)
                curr = curr->right;
            else
                curr = curr->left;
        }
    }

    ans->ans = curr;

    return;
}

/* Rotate the right child of par upwards */

/* Could be written as a macro, but not really necessary
as it is only called on insertion */

void
leftrotate(BRANSREC * ans, RBTREEREC * par)
{
    RBTREEREC *curr, *gpar;

    if ((curr = par->right) != NULL)
    {
        par->right = curr->left;
        if (curr->left != NULL)
            curr->left->par = par;
        curr->par = par->par;
        if ((gpar = par->par) == NULL)
            ans->root = curr;
        else
        {
            if (par == gpar->left)
                gpar->left = curr;
            else
                gpar->right = curr;
        }
        curr->left = par;
        par->par = curr;
    }
}

/* Rotate the left child of par upwards */
void
rightrotate(BRANSREC * ans, RBTREEREC * par)
{
    RBTREEREC *curr, *gpar;

    if ((curr = par->left) != NULL)
    {
        par->left = curr->right;
        if (curr->right != NULL)
            curr->right->par = par;
        curr->par = par->par;
        if ((gpar = par->par) == NULL)
            ans->root = curr;
        else
        {
            if (par == gpar->left)
                gpar->left = curr;
            else
                gpar->right = curr;
        }
        curr->right = par;
        par->par = curr;
    }
}

/* Create a node to hold a word */

RBTREEREC *
rbwcreate(char *word, RBTREEREC * par)
{
    RBTREEREC *tmp;

    tmp = (RBTREEREC *) malloc(sizeof(RBTREEREC));
    tmp->word = (char *)malloc(strlen(word) + 1);
    strcpy(tmp->word, word);
    tmp->left = tmp->right = NULL;

    tmp->par = par;

    gStored++;                          // count stored

    return (tmp);
}

/* Insert word into a redblack tree */

void
redblackinsert(BRANSREC * ans, char *word)
{
    RBTREEREC *curr = ans->root, *par, *gpar, *prev = NULL, *rbwcreate();
    int       val = 0;

    if (ans->root == NULL)
    {
        ans->ans = ans->root = rbwcreate(word, NULL);
        return;
    }
    while (curr != NULL && (val = strcmp(word, curr->word)) != 0)
    {
        prev = curr;
        if (val > 0)
            curr = curr->right;
        else
            curr = curr->left;
    }

    ans->ans = curr;

    if (curr == NULL)
        /* Insert a new node, rotate up if necessary */
    {
        if (val > 0)
            curr = prev->right = rbwcreate(word, prev);
        else
            curr = prev->left = rbwcreate(word, prev);

        curr->colour = RED;
        while ((par = curr->par) != NULL
               && (gpar = par->par) != NULL && curr->par->colour == RED)
        {
            if (par == gpar->left)
            {
                if (gpar->right != NULL && gpar->right->colour == RED)
                {
                    par->colour = BLACK;
                    gpar->right->colour = BLACK;
                    gpar->colour = RED;
                    curr = gpar;
                }
                else
                {
                    if (curr == par->right)
                    {
                        curr = par;
                        leftrotate(ans, curr);
                        par = curr->par;
                    }
                    par->colour = BLACK;
                    if ((gpar = par->par) != NULL)
                    {
                        gpar->colour = RED;
                        rightrotate(ans, gpar);
                    }
                }
            }
            else
            {
                if (gpar->left != NULL && gpar->left->colour == RED)
                {
                    par->colour = BLACK;
                    gpar->left->colour = BLACK;
                    gpar->colour = RED;
                    curr = gpar;
                }
                else
                {
                    if (curr == par->left)
                    {
                        curr = par;
                        rightrotate(ans, curr);
                        par = curr->par;
                    }
                    par->colour = BLACK;
                    if ((gpar = par->par) != NULL)
                    {
                        gpar->colour = RED;
                        leftrotate(ans, gpar);
                    }
                }
            }
        }
        if (curr->par == NULL)
            ans->root = curr;
        ans->root->colour = BLACK;
    }

    return;
}

//=======================================================================
//      T E R N A R Y   M E T H O D
//=======================================================================

typedef struct tnode *Tptr;
typedef struct tnode
{
    uint8_t   splitchar;
    Tptr      lokid, eqkid, hikid;
}
Tnode;

void
TernaryIns(Tptr * p, uint8_t * s, int Len)
{
    int       d;
    uint8_t  *instr = s;
    Tptr      pp;
    while ((pp = *p) != NULL)
    {
        if ((d = *s - pp->splitchar) == 0)
        {
            if (*s++ == 0)
            {
                printf("Oops duplicate Ternary string %s\n", instr);
                return;
            }
            p = &(pp->eqkid);
        }
        else if (d < 0)
            p = &(pp->lokid);
        else
            p = &(pp->hikid);
    }
    for (;;)
    {
        *p = (Tptr) malloc(sizeof(Tnode));
        pp = *p;
        pp->splitchar = *s;
        pp->lokid = pp->eqkid = pp->hikid = 0;
        if (*s++ == 0)
        {
            pp->eqkid = (Tptr) instr;
            gStored++;                  // number of strings stored
            return;
        }
        p = &(pp->eqkid);
    }
}

int
TernaryGet(Tptr p, uint8_t * s, int Len)
{
    while (p)
    {
        if (*s < p->splitchar)
            p = p->lokid;
        else if (*s == p->splitchar)
        {
            if (*s++ == 0)
                return 1;
            p = p->eqkid;
        }
        else
            p = p->hikid;
    }
    return 0;
}

//=======================================================================
//      M E A S U R E  A D T   S P E E D  and  M E M O R Y  U S A G E
//=======================================================================

//Word_t    TotalJudyMalloc;

#define         GETSTRING(PCurStr, Strlen)

int
main(int argc, char *argv[])
{
    TIMER_vars(tm);                     // declare timer variables
    FILE     *fid;                      // to read file.
    int       Chr;                      // char read from fgetc
    Pdt_t     Pdt, Pdts;                // array of lengths and pointers to str
    uint8_t  *PCurStr;                  // Current string pointer
    Word_t    LineCnt;                  // line counter
    int       Strlen;                   // = strlen();
    Word_t    StrTot;                   // Total len of strings
    Word_t    StrNumb;                  // current line number
    Word_t    ReadLin;                  // strings to read
    double    Mult;                     // multiplier between groups
    Word_t    Groups;                   // number of measurement groups
    Word_t    grp;                      // current group
    Pvoid_t   JudySL = NULL;            // JudySL root pointer
    Pvoid_t   JudyHS = NULL;            // JudyHS root pointer
    Pvoid_t   JLHash = NULL;            // JLHash root pointer
    Phinfo_t  HRoot = NULL;             // hash table root pointer
    Tptr      Ternary = { 0 };          // Ternary struct root pointer

    Method_t  Method = M_invalid;       // the method to measure
    Word_t    lines = 0;                // to shut up compiler
    Word_t    Bytes = 0;                // Bytes deallocated from FreeArray
    Word_t    StringMemory;             // Bytes allocated for input data
    int       Pass;
    int       Passes = 1;

    int       Opt;
    extern char *optarg;
    int       ErrorFlag = 0;

//  un-buffer output
    setbuf(stdout, NULL);

//============================================================
// PARSE INPUT PARAMETERS
//============================================================

    while ((Opt = getopt(argc, argv, "A:H:L:n:T:P:M:praDC")) != -1)
    {
        switch (Opt)
        {
        case 'A':
            if (Method != M_invalid)
            {
                printf("\nOnly ONE '-A<ADT>' is allowed!!!!!!\n");
                ErrorFlag++;
                break;
            }
            if (strcmp(optarg, "Print") == 0)
                Method = M_Print;
            if (strcmp(optarg, "Hash") == 0)
            {
                Method = M_Hash;
                HTblsz = 1LU << 20;     // default 1.0+ million
            }
            if (strcmp(optarg, "JLHash") == 0)
            {
                Method = M_JLHash;
                HTblsz = 0;             // max 2^32
            }
            if (strcmp(optarg, "JudySL") == 0)
                Method = M_JudySL;
            if (strcmp(optarg, "Splay") == 0)
                Method = M_Splay;
            if (strcmp(optarg, "Redblack") == 0)
                Method = M_Redblack;
            if (strcmp(optarg, "JudyHS") == 0)
                Method = M_JudyHS;
            if (strcmp(optarg, "Ternary") == 0)
                Method = M_Ternary;
            break;

        case 'H':                      // Size of Hash table
            HTblsz = strtoul(optarg, NULL, 0);
            break;

        case 'L':                      // Number of Loops
            Passes = atoi(optarg);
            if (Passes <= 0)
            {
                printf("\n !! OOps - Number of Loops must be > 0\n");
                ErrorFlag++;
            }
            break;

        case 'n':                      // Max population of arrays
            nStrg = strtoul(optarg, NULL, 0);   // Size of Linear Array
            if (nStrg == 0)
            {
                printf("\n !! OOps - Number of strings must be > 0\n");
                ErrorFlag++;
            }
            break;

        case 'T':                      // Maximum retrieve tests for timing 
            TValues = strtoul(optarg, NULL, 0);
            break;

        case 'P':                      // measurement points per decade
            PtsPdec = strtoul(optarg, NULL, 0);
            break;

        case 'M':                      // maximum length of input string
            MLength = atoi(optarg);
            break;

        case 'p':                      // pre-initialize Hash table
            pFlag = 1;
            break;

        case 'r':                      // do not randomize input
            if (CFlag)
            {
                printf
                    ("\n !! OOps '-r' and '-C' flag are mutually exclusive\n");
                ErrorFlag++;
                break;
            }
            rFlag = 1;
            break;

        case 'a':                      // word align string buffers
            aFlag = 1;
            break;

        case 'D':                      // do a delete at end
            DFlag = 1;
            break;

        case 'C':                      // build sequential Get string buffers
            if (rFlag)
            {
                printf
                    ("\n !! OOps '-C' and '-r' flag are mutually exclusive\n");
                ErrorFlag++;
                break;
            }
            CFlag = 1;
            break;

        default:
            ErrorFlag++;
            break;
        }
    }
    if (Method == -1)
    {
        printf
            ("\n !! OOps -- '-A <ADT>' I.E. '-AHash' or '-AJudyHS' is a required option\n");
        ErrorFlag++;
    }

    fileidx = optind;
    if (optind >= argc)
    {
        printf("\n !! OOps -- No input file specified\n");
        ErrorFlag++;
    }

    if (ErrorFlag)
    {
        printf("\n");
        printf("$ %s -A<ADT> -n# -H# -P# -T# -p -r -a InputStringFile\n\n",
               argv[0]);
        printf("Where: ");
        printf("'InputStringFile' is text file of strings to use in test\n\n");
        printf
            ("-A <ADT> is Hash|JLHash|JudySL|JudyHS|Splay|Redblack|Ternary|Print\n");
        printf("\n");
        printf("-n <#> max number of strings to use in tests (all)\n");
        printf("-H <#> is number elements in Hash table\n");
        printf("-P <#> number of measurement points per decade (40)\n");
        printf
            ("-T <#> Change the 'Get' number_of_strings to measure per data point\n");
        printf("-D     Use 'Delete' routine instead of 'FreeArray' routine\n");
        printf("-p     pre-zero hash table to fault in all pages\n");
        printf("-r     Do not randomize Insert and Get order of strings\n");
        printf("-C     Build contigious string buffers for 'Get' tests\n");
        printf("-a     Word_t align the start address of input strings\n");
        printf("-M <#> Change the maximum 'strlen(String)' of input Strings\n");
        printf("\n\n");

        exit(1);
    }

//  calculate max number mask used in hash routine

//============================================================
//  PRINT COMMAND NAME + RUN ARGUMENTS
//============================================================

    printf("# %s", argv[0]);
    if (nStrg != INFSTRGS)
        printf(" -n%lu", nStrg);
    switch (Method)
    {
    case M_Hash:
        printf(" -A Hash");
        break;
    case M_JLHash:
        printf(" -A JLHash");
        break;
    case M_JudySL:
        printf(" -A JudySL");
        break;
    case M_JudyHS:
        printf(" -A JudyHS");
        break;
    case M_Splay:
        printf(" -A Splay");
        break;
    case M_Redblack:
        printf(" -A Redblack");
        break;
    case M_Ternary:
        printf(" -A Ternary");
        break;
    default:
        break;
    }
    if (HTblsz)
        printf(" -H%lu", HTblsz);
    printf(" -P%lu", PtsPdec);
    printf(" -L%d", Passes);
    if (pFlag)
        printf(" -p");
    if (rFlag)
        printf(" -r");
    if (DFlag)
        printf(" -D");
    if (CFlag)
        printf(" -C");
    printf(" -M%d", MLength);
    printf(" %s", argv[fileidx]);
    printf("\n");

//  print some header

    printf("# This file is in a format to input to 'jbgraph'\n");
    printf("# XLABEL Stored\n");
    printf("# YLABEL Microseconds / Index\n");
    printf("# COLHEAD 1 Total Insert attempts\n");
    printf("# COLHEAD 2 Number Gets\n");
    printf("# COLHEAD 3 Duplicate strings\n");
    printf("# COLHEAD 4 Insert Time (uS)\n");
    printf("# COLHEAD 5 Get Time (uS)\n");
    printf("# COLHEAD 6 Hash Chain Length\n");
    printf("# COLHEAD 7 Average RAM/String\n");

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
        printf("# 64 Bit CPU\n");
    else if (sizeof(Word_t) == 4)
        printf("# 32 Bit CPU\n");
#ifdef  CPUMHZ
    printf("# Processor speed compiled at %d Mhz\n", CPUMHZ);
#endif // CPUMHZ

    if (Method == M_Hash)
        printf("# Hash record struct: sizeof(hrec_t) = %d\n", sizeof(hrec_t));
    if (Method == M_Ternary)
        printf("# Ternary record struct: sizeof(Tnode) = %d\n", sizeof(Tnode));

//  OPEN INPUT FILE:

    if ((fid = fopen(argv[fileidx], "r")) == NULL)
        FILERROR;

    for (StrTot = Strlen = LineCnt = 0; (Chr = fgetc(fid)) != EOF;)
    {
        if (Chr == '\n')
        {
            if (Strlen)                 // eat zero length lines
            {
                if (Strlen > MLength)
                    Strlen = MLength;
                LineCnt++;              // increase string count
                Strlen++;               // add a \0 for JudySL

                if (aFlag)              // for word alignment
                    StrTot += ROUNDUPWORD(Strlen);
                else
                    StrTot += Strlen;   // memory needed to store strings

                if (LineCnt == nStrg)   // shorten if required by -n option
                    break;

                Strlen = 0;
            }
        }
        else
        {
            Strlen++;
        }
    }
    fclose(fid);
    fid = NULL;
    nStrg = LineCnt;                    // adj if necessary

//  get struct to keep track of the strings
    StringMemory = sizeof(dt_t) * nStrg;
    Pdt = (Pdt_t) malloc(sizeof(dt_t) * nStrg);
    if (Pdt == NULL)
        MALLOCERROR;

//  get memory to store the strings
    StringMemory += StrTot;
    PCurStr = (uint8_t *) malloc(StrTot);
    if (PCurStr == NULL)
        MALLOCERROR;

//  BRING FILE INTO RAM, COUNT LINES and CHECK LENGTH

//============================================================
// CALCULATE NUMBER OF MEASUREMENT GROUPS -- points per decade
//============================================================

//  Calculate Multiplier for number of points per decade
    Mult = pow(10.0, 1.0 / (double)PtsPdec);
    {
        double    sum;
        Word_t    numb, prevnumb;

//      Count number of measurements needed (10K max)
        sum = numb = 1;
        for (Groups = 2; Groups < 10000; Groups++)
            if (NextNumb(&numb, &sum, Mult, nStrg))
                break;

//      Get memory for measurements
        Pms = (Pms_t) calloc(Groups, sizeof(ms_t));
        if (Pms == NULL)
            MALLOCERROR;

//      Now calculate number of Indexes for each measurement point
        numb = sum = 1;
        prevnumb = 0;
        for (grp = 0; grp < Groups; grp++)
        {
            Pms[grp].ms_delta = numb - prevnumb;
            Pms[grp].ms_mininsert = 10000000.0; // infinity
            Pms[grp].ms_minretrive = 10000000.0;        // infinity
            Pms[grp].ms_Bytes = 0.0;

            prevnumb = numb;

            NextNumb(&numb, &sum, Mult, nStrg);
        }
    }                                   // Groups = number of sizes

//  print remaining header

    if (Method == M_Hash)
    {
        printf("# Allocate Hash table = %lu elements\n", HTblsz);
    }
    if (Method == M_JLHash)
    {
        if (HTblsz)
            printf("# JLHash table virtual size = %lu\n", HTblsz);
        else
            printf("# JLHash table virtual size = 4294967296\n");
    }

//=======================================================================
// Read text input file into RAM
//=======================================================================

    if ((fid = fopen(argv[fileidx], "r")) == NULL)
        FILERROR;

    for (Strlen = LineCnt = 0; LineCnt < nStrg;)
    {
        Chr = fgetc(fid);
        if (Chr == '\n')
        {
            if (Strlen)                 // eat zero length lines
            {
                if (Strlen > MLength)
                    Strlen = MLength;
                Pdt[LineCnt].dt_string = PCurStr - Strlen;
                Pdt[LineCnt].dt_strlen = Strlen;
                LineCnt++;

                Strlen = 0;
                *PCurStr++ = '\0';      // for JudySL
                if (aFlag)              // for word alignment
                    PCurStr = (uint8_t *) ROUNDUPWORD((Word_t)PCurStr);

                if ((Word_t)PCurStr % sizeof(Word_t))
                    aCount++;
            }
        }
        else
        {
            if (Strlen < MLength)
            {
                Strlen++;
                if (Chr == '\0')
                    Chr = ' ';          // for JudySL
                *PCurStr++ = (uint8_t) Chr;
            }
        }
    }
    fclose(fid);
    fid = NULL;
    assert(nStrg == LineCnt);

    printf("# %lu (%.1f%%) non-Word_t aligned string buffers\n",
           aCount, (double)aCount / (double)LineCnt * 100.0);

    printf("# Ram used for input data = %lu bytes\n", StringMemory);

    printf("# Average string length = %.1f bytes\n",
           (double)(StrTot - LineCnt) / LineCnt);

// Allocate memory for Cached assess to 'Get' (largest delta). This flag
// will put the 'randomized' 'Get' order strings in a sequential buffer.
// Modern processors will 'read ahead' with an access to RAM is sequential
// -- thus saving the 'Get' having to bring the string into cache.
    if (CFlag)
    {
        PdtS_ = (Pdt_t) malloc(TValues * sizeof(dt_t));
        if (PdtS_ == NULL)
            MALLOCERROR;

//      now guess how much memory will be needed for the strings
        Strsiz_ = ((StrTot / nStrg) * TValues);
        Strsiz_ += Strsiz_;             // bump %20

        Strbuf_ = (uint8_t *) malloc(Strsiz_);
        if (Strbuf_ == NULL)
            MALLOCERROR;

        printf
            ("# %lu bytes malloc() for 'cached' strings for Get measurement\n",
             Strsiz_);
    }

//=======================================================================
//  TIME GETSTRING() from Cache (most of the time)
//=======================================================================

    STARTTm(tm);                        // start timer
    for (LineCnt = 0; LineCnt < nStrg; LineCnt++)
    {
        GETSTRING(PCurStr, Strlen);
        Strlen = Pdt[LineCnt].dt_strlen;
        PCurStr = Pdt[LineCnt].dt_string;

        if (strlen(PCurStr) != Strlen)  // bring string into Cache
        {
//          necessary to prevent cc from optimizing out
            printf(" !! OOps Bug, wrong string length\n");
            exit(1);
        }
    }
    ENDTm(DeltaUSec, tm);               // end timer

    printf
        ("# Access Time    = %6.3f uS average per string (mostly from Cache)\n",
         DeltaUSec / nStrg);

//=======================================================================
//  TIME GETSTRING() + HASHSTR() from Cache (most of the time)
//=======================================================================

    STARTTm(tm);                        // start timer
    for (LineCnt = 0; LineCnt < nStrg; LineCnt++)
    {
        uint32_t  hval;
        GETSTRING(PCurStr, Strlen);
        PCurStr = Pdt[LineCnt].dt_string;
        Strlen = Pdt[LineCnt].dt_strlen;
        hval = HASHSTR(PCurStr, Strlen, HTblsz);
        if (foolflag)
            printf("OOps foolflag is set, hval = %d\n", hval);
    }
    ENDTm(DeltaUSec, tm);               // end timer

    printf
        ("# HashStr() Time = %6.3f uS average per string (mostly from Cache)\n",
         DeltaUSec / nStrg);

//  randomize the input strings (adjacent strings will not be on same page)

    if (rFlag == 0)
    {
        Randomize(Pdt, nStrg);          // Randomize ALL to be stored

//=======================================================================
//  TIME GETSTRING() from RAM (most of the time)
//=======================================================================

        STARTTm(tm);                    // start timer
        for (LineCnt = 0; LineCnt < nStrg; LineCnt++)
        {
            GETSTRING(PCurStr, Strlen);
            Strlen = Pdt[LineCnt].dt_strlen;
            PCurStr = Pdt[LineCnt].dt_string;

            if (strlen(PCurStr) != Strlen)      // bring string into Cache
            {
//              necessary to prevent cc from optimizing out
                printf(" !! OOps Bug, wrong string length\n");
                exit(1);
            }
        }
        ENDTm(DeltaUSec, tm);           // end timer

        printf
            ("# Access Time    = %6.3f uS average per string (mostly from RAM)\n",
             DeltaUSec / nStrg);

//=======================================================================
//  TIME GETSTRING() + HASHSTR() from RAM (most of the time)
//=======================================================================

        STARTTm(tm);                    // start timer
        for (LineCnt = 0; LineCnt < nStrg; LineCnt++)
        {
            uint32_t  hval;
            GETSTRING(PCurStr, Strlen);
            Strlen = Pdt[LineCnt].dt_strlen;
            PCurStr = Pdt[LineCnt].dt_string;
            hval = HASHSTR(PCurStr, Strlen, HTblsz);
            if (foolflag)
                printf("OOps foolflag is set, hval = %u\n", hval);
        }
        ENDTm(DeltaUSec, tm);           // end timer

        printf
            ("# HashStr() Time = %6.3f uS average per string (mostly from RAM)\n",
             DeltaUSec / nStrg);
    }

//=======================================================================
//  Insert, Get and Delete loops
//=======================================================================

    for (Pass = 0; Pass < Passes; Pass++)
    {
        printf("# Pass %d\n", Pass);

//      heading of table 
        Printf
            ("# TotInserts  DeltaGets  DupStrs InsTime GetTime HChainLen Ram/String\n");
        gStored = 0;                    // number of strings inserted
        StrNumb = 0;                    // number of attempted strings inserted

        STARTmem;                       // current malloc() mem usage
        for (grp = 0; grp < Groups; grp++)
        {
            PWord_t   PValue;
            Word_t    Begin = gStored;  // remember current STOREed
            Word_t    Delta = Pms[grp].ms_delta;

            switch (Method)
            {
            case M_Print:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdt[StrNumb].dt_string;
                    Printf("%s\n", (char *)PCurStr);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_Hash:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdt[StrNumb].dt_strlen;
                    PCurStr = Pdt[StrNumb].dt_string;

                    PValue = HashIns(&HRoot, PCurStr, Strlen, HTblsz);
                    if ((*PValue)++ == 0)
                        gStored++;      // number of strings stored
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_JLHash:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdt[StrNumb].dt_strlen;
                    PCurStr = Pdt[StrNumb].dt_string;
                    PValue = JLHashIns(&JLHash, PCurStr, Strlen, HTblsz);
                    if ((*PValue)++ == 0)
                        gStored++;      // number of strings stored
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_JudySL:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdt[StrNumb].dt_string;

                    JSLI(PValue, JudySL, PCurStr);      // insert string
                    if ((*PValue)++ == 0)
                        gStored++;      // number of strings stored
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_JudyHS:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdt[StrNumb].dt_strlen;
                    PCurStr = Pdt[StrNumb].dt_string;

                    JHSI(PValue, JudyHS, PCurStr, Strlen);      // insert string
                    if ((*PValue)++ == 0)
                        gStored++;      // number of strings stored
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }

// NOTE:  the ADT's below here are so slow, that I did not add much effort 
// to clean them up. (dlb)

            case M_Splay:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdt[StrNumb].dt_string;

                    splayinsert(&spans, (char *)PCurStr);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_Redblack:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdt[StrNumb].dt_string;

                    redblackinsert(&rbans, (char *)PCurStr);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_Ternary:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < Delta; lines++, StrNumb++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdt[StrNumb].dt_strlen;
                    PCurStr = Pdt[StrNumb].dt_string;

                    TernaryIns(&Ternary, PCurStr, Strlen);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            default:
                assert(0);              // cant happen
                break;
            }
            ENDmem(DeltaMem);           // current malloc() mem usage

            ReadLin = StrNumb;          // adjust delta
            if (ReadLin > TValues)
                ReadLin = TValues;
//            if (Delta > TValues)
//                ReadLin = Delta;        // use the Delta

            Printf(" %11lu", StrNumb);  // Total stored
            Printf(" %10lu", ReadLin);  // Number to read back
            Begin = gStored - Begin;    // actual STORED
            assert(lines == Delta);
            Printf(" %8lu", Delta - Begin);     // Duplicate strings

//          Average time per line to store (including duplicate strings)
            Mult = DeltaUSec / (double)Delta;

            if (Mult < Pms[grp].ms_mininsert)
                Pms[grp].ms_mininsert = Mult;

            Printf(" %7.3f", Mult);

//          Bytes allocated thru malloc()
            if (TotalJudyMalloc == 0)
                Pms[grp].ms_Bytes = (double)DeltaMem;
            else
                Pms[grp].ms_Bytes = (double)(TotalJudyMalloc * sizeof(Word_t));

            Pms[grp].ms_Bytes /= (double)gStored;

            fflush(stdout);

//=======================================================================
//  READ BACK LOOP
//=======================================================================

            Pdts = Pdt;                 // Strings to 'Get'
            gChainln = 0;               // total chain lengths

            if (rFlag == 0)
            {
                Randomize(Pdt, StrNumb);        // Randomize ONLY those stored

                if (CFlag)
                {
//                  Allocate and make sequencial string buffer
                    Pdts = BuildSeqBuf(Pdt, ReadLin);
                }
            }
            switch (Method)
            {
            case M_Print:
                break;
            case M_Hash:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < ReadLin; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdts[lines].dt_strlen;
                    PCurStr = Pdts[lines].dt_string;

                    PValue = HashGet(HRoot, PCurStr, Strlen);   // get string
                    assert(PValue != NULL);
                    assert(*PValue > 0);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_JLHash:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < ReadLin; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdts[lines].dt_strlen;
                    PCurStr = Pdts[lines].dt_string;

                    PValue = JLHashGet(JLHash, PCurStr, Strlen);        // get string
                    assert(PValue != NULL);
                    assert(*PValue > 0);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_JudySL:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < ReadLin; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdts[lines].dt_string;

                    JSLG(PValue, JudySL, PCurStr);      // get string
                    assert(PValue != NULL);
                    assert(*PValue > 0);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_JudyHS:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < ReadLin; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdts[lines].dt_strlen;
                    PCurStr = Pdts[lines].dt_string;

                    JHSG(PValue, JudyHS, PCurStr, Strlen);      // get string
                    assert(PValue != NULL);
                    assert(*PValue > 0);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }

// NOTE:  the ADT's below here are so slow, that I did not add much effort 
// to clean them up. (dlb)

            case M_Splay:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < ReadLin; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdts[lines].dt_string;

                    splaysearch(&spans, (char *)PCurStr);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_Redblack:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < ReadLin; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdts[lines].dt_string;

                    redblacksearch(&rbans, (char *)PCurStr);
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            case M_Ternary:
            {
                STARTTm(tm);            // start timer
                for (lines = 0; lines < ReadLin; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdts[lines].dt_strlen;
                    PCurStr = Pdts[lines].dt_string;

                    if (TernaryGet(Ternary, PCurStr, Strlen) == 0)
                    {
                        printf("\n OOps - Ternary Bug at Line = %d\n",
                               __LINE__);
                        exit(1);
                    }
                }
                ENDTm(DeltaUSec, tm);   // end timer
                break;
            }
            default:
                assert(0);              // cant happen
                break;
            }
            Mult = DeltaUSec / (double)ReadLin;

//          save least value
            if (Mult < Pms[grp].ms_minretrive)
                Pms[grp].ms_minretrive = Mult;

            Printf(" %7.3f", Mult);     // RETRIVE per string
            Printf(" %9.6f", (double)gChainln / (double)ReadLin);

//      RAM USED PER STRING TO STORE DATA

            Printf(" %13.1f", (double)Pms[grp].ms_Bytes);
            Printf("\n");
            fflush(stdout);
        }
        if (Method == M_Print)
            exit(0);

        Printf("# Total Duplicate strings = %lu\n", nStrg - gStored);

//=======================================================================
//  Delete loop
//=======================================================================

        DeltaUSec = -1.0;               // set deleted flag

        if (rFlag == 0)
        {
            Randomize(Pdt, StrNumb);    // Randomize ONLY those stored
        }
        switch (Method)
        {
        case M_JudySL:
        {
            if (DFlag)
            {
                Printf("# Begin JudySLDel() loop...\n");
                STARTTm(tm);            // start timer
                for (lines = 0; lines < nStrg; lines++)
                {
                    int       Rc;
                    GETSTRING(PCurStr, Strlen);
                    PCurStr = Pdt[lines].dt_string;
                    JSLD(Rc, JudySL, PCurStr);  // delete string
                    assert(Rc != JERR);
                }
                ENDTm(DeltaUSec, tm);   // end timer
            }
            else
            {
                Printf("# Begin JudySLFreeArray()...\n");
                STARTTm(tm);            // start timer
                JSLFA(Bytes, JudySL);
                ENDTm(DeltaUSec, tm);   // end timer
            }
            break;
        }
        case M_JudyHS:
        {
            if (DFlag)
            {
                int       Rc;
                Printf("# Begin JudyHSDel() loop...");
                STARTTm(tm);            // start timer
                for (lines = 0; lines < nStrg; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdt[lines].dt_strlen;
                    PCurStr = Pdt[lines].dt_string;
                    JHSD(Rc, JudyHS, PCurStr, Strlen);  // Delete string
                    assert(Rc != JERR);
                }
                ENDTm(DeltaUSec, tm);   // end timer
            }
            else
            {
                Printf("# Begin JudyHSFreeArray()...\n");
                STARTTm(tm);            // start timer
                JHSFA(Bytes, JudyHS);
                ENDTm(DeltaUSec, tm);   // end timer
            }
            break;
        }
        case M_Hash:
        {
            if (DFlag)
            {
                Printf("# Begin HashDel() loop...\n");
                STARTTm(tm);            // start timer
                for (lines = 0; lines < nStrg; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdt[lines].dt_strlen;
                    PCurStr = Pdt[lines].dt_string;
                    HashDel(&HRoot, PCurStr, Strlen);   // Delete string
                }
                ENDTm(DeltaUSec, tm);   // end timer
            }
            else
            {
                Printf("# Begin HashFreeArray()...\n");
                STARTTm(tm);            // start timer
                Bytes = HashFreeArray(&HRoot);
                ENDTm(DeltaUSec, tm);   // end timer
            }
            break;
        }
        case M_JLHash:
        {
            if (DFlag)
            {
                Printf("# Begin JLHashDel() loop...\n");
                STARTTm(tm);            // start timer
                for (lines = 0; lines < nStrg; lines++)
                {
                    GETSTRING(PCurStr, Strlen);
                    Strlen = Pdt[lines].dt_strlen;
                    PCurStr = Pdt[lines].dt_string;
                    JLHashDel(&JLHash, PCurStr, Strlen);        // Delete string
                }
                ENDTm(DeltaUSec, tm);   // end timer
            }
            else
            {
                Printf("# Begin JLHashFreeArray()...\n");
                STARTTm(tm);            // start timer
                Bytes = JLHashFreeArray(&JLHash);
                ENDTm(DeltaUSec, tm);   // end timer
            }
            break;
        }
        default:
            printf("# Delete not implemented yet, so quit\n");
            Passes = 1;                 // No, delete, so quit
            break;
        }
//  average time per line to delete (including duplicate strings)
        if (Bytes)                      // Measured freed bytes?
        {
            Printf("#                      returned %lu bytes\n", Bytes);
        }
        if (TotalJudyMalloc)            // Any bytes left after free?
        {
            printf
                ("# !!! BUG, %lu bytes not deleted in *Free()\n",
                 TotalJudyMalloc * sizeof(Word_t));
        }
        if (DeltaUSec != -1.0)          // Measured how long to free?
        {
            Printf("# Free %lu strings, %0.3f uSecs Ave/string\n",
                   gStored, DeltaUSec / (double)gStored);
        }
        Printf("\n");
    }

    if (Passes != 1)
    {
        printf("# TotInserts      0     0 InsTime GetTime      0 Ram/String\n");
        StrNumb = 0;
        for (grp = 0; grp < Groups; grp++)
        {
            StrNumb += Pms[grp].ms_delta;

            printf(" %11lu", StrNumb);  // Total stored
            printf("      0     0");    // place holder
            printf(" %7.3f", Pms[grp].ms_mininsert);
            printf(" %7.3f", Pms[grp].ms_minretrive);
            printf("      0");          // place holder
            printf(" %7.3f", Pms[grp].ms_Bytes);
            printf("\n");
        }
    }
    exit(0);                            // done
}                                       // main()
