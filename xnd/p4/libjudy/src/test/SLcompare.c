// @(#) $Revision: 4.1 $ $Source: /judy/test/manual/SLcompare.c $
//=======================================================================
//   Author Douglas L. Baskins, June 2002.
//   Permission to use this code is freely granted, provided that this
//   statement is retained.
//   email - doug@sourcejudy.com
//=======================================================================

// Compile for choice of algorithm:

//   cc -static -O -o Judy     -DJUDYMETHOD     SLcompare.c -lJudy
//   cc -static -O -o Hash     -DHASHMETHOD     SLcompare.c
//   cc -static -O -o Splay    -DSPLAYMETHOD    SLcompare.c
//   cc -static -O -o Redblack -DREDBLACKMETHOD SLcompare.c
//
// Note:  -static will generally get better performance because string
// routines are slower with shared librarys.
//
// Usage:
//
//   Judy     <textfile>
//   Hash     <textfile>
//   Splay    <textfile>
//   Redblack <textfile>
/*

This program will give you a chance to measure the speed/space
performance of 4 different string store/lookup algorithms on your data
set.  All are very fast and generally can scale to very large data sets.
The memory efficiency is usually best with the Judy algorithm because it
uses the opportunity to compress data in a digital tree.  The Hashing is
generally the fastest algorithm, however it looses its advantage when
the number of stored exceeds about 5X the size of the hash table.
Many thanks to J.  Zobel for supplying the code for Hash, Splay and 
Redblack algorithms.  

I will be including more algorithms as people donate code for testing.

The Judy code is available at: <http://sourceforge.net/projects/judy>
9/2002 (dlb).

This is the output of this program when run on a 750Mhz P3 laptop.

This output is using a file available from www.google.com.  It looks 
like 'http' web pages and was one of the files used in a March 2002 
contest by www.google.com

 wc google/data/repos.00
 3440314 14613027 162822437 google/data/repos.00

   lines avg_linelen  getline   stored RAMused/line  store/ln lookup/ln ADT
 2807737   58.0 byts 0.856 uS  1455201   104.0 byts  4.859 uS  4.566 uS SPLAY
 2807737   58.0 byts 0.862 uS  1455201    96.0 byts  2.523 uS  2.101 uS HASH
 2807737   58.0 byts 0.856 uS  1455201   104.0 byts  4.601 uS  4.001 uS REDBLK
 2807737   58.0 byts 0.875 uS  1455201    78.7 byts  3.898 uS  2.687 uS JUDY

With = 1.46 million lines stored and 2.81 million non-blank lines,
all the algorithms seem to keep their performance.  The hash table is a 
perfect size for this data set.  I suspect only the HASH algorithm will
slow down with larger data sets unless the hash table is increased larger
than 2**20 million entrys.  I have now tried a data set with about 10 
million unique lines on a 64 bit machine (google/data/repos.*).  
The speeds are about the same.  Lookup times are in the 2-4 uSec range 
and this is probably insignificant compared to the application needing 
the data.  I think all four methods are fine for speed, and Judy is a 
standout for its memory efficiency -- frequently using less memory than
the strings alone.

I plan on tuning JudySL in the near future.  This should improve the 
performance of JudySL with short (a text word) strings.  I have a data 
base of all (28.8 million) domain names on the internet as a test case.

wc /data/domnames.txt
28826508 28826508 488227095 /data/domnames.txt

For the curious, JudySL (I.E JSLI/G macros in this program) is simply an
application subroutine that uses JudyL to the work.  JudyL is a very
scaleable word to word (or pointer) mapper.  JudySL is implemented as a
digital tree using JudyL arrays as 2^32 (or 2^64 in 64 bit machines)
wide (virtual) nodes.  Each level in the digital tree decodes the next 4
(or 8) bytes of the line/string until only one entry would be in the
node.  Then the remaining undecoded portion of the line is stored as a
string from the parent node.  A digital tree does not need rotation or
balancing.  In practice, digital trees are rarely use because of their
poor memory efficiency in the nodes and a tendency to have large depths.
These problems are solved in the Judy algorithm.  For more details see
application notes on:  www.sourcejudy.com.  And for the really curious,
a technical paper is available in the application section.  If you would
like to be a reviewer, contact Doug Baskins at <doug@sourcejudy.com>

*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>              // printf
#include <fcntl.h>
#include <string.h>
#include <errno.h>              // errno
#include <sys/mman.h>           // mmap()
#include <sys/stat.h>           // stat()
#include <sys/time.h>           // gettimeofday()

// remove all uses of this in production code.
int       gDupCnt = 0;          // counter for duplicate strings

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
//
// optional for high resolution times and compile with timeit.c
//   cc -static -O -o Judy -DJUDYMETHOD SLcompare.c timeit.c -lJudy
//
//#include "timeit.h"

double    DeltaUSec;            // Global for remembering delta times

#ifndef _TIMEIT_H

// Note: I have found some Linux systems (2.4.18-6mdk) to have bugs in the 
// gettimeofday() routine.  Sometimes it returns a negative ~2840 microseconds
// instead of 0 or 1.  If you use the above #include "timeit.h" and compile with
// timeit.c and use -DJU_LINUX_IA32, that problem will be eliminated.  This is
// because for delta times less than .1 sec, the hardware free running timer
// is used instead of gettimeofday().  I have found the negative time problem
// appears about 40-50 times per second with consecutive gettimeofday() calls.

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

// define this if your malloc does not have mallinfo();
#define NOMALLINFO 1

double    DeltaMem;             // for remembering

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
//      G E T S T R I N G   F R O M   mmap()   F I L E   M A C R O
//=======================================================================
//
// From memory map POINTER store next '\n' terminated string to BUFFER
// Delete spaces, tabs, returns and resulting blank lines.

// POINTER must be check to be within memory mapped file range
//
// NOTE: This code will core-dump if a corrupt text file because
// POINTER is not checked to exceed end of file.

#define MAXLINE 10000   // max length line

#define GETLINE(BUFFER,POINTER)			\
{                                               \
    char _chr;                                  \
    int  _count = 0;                            \
    for (;;)	/* forever */			\
    {                                           \
        switch (_chr = *POINTER++)		\
        {					\
        case  ' ':	/* eat spaces  */	\
        case '\t':	/* eat tabs    */	\
        case '\r':	/* eat returns */	\
            continue;				\
	case '\n':      /* eat blank lines */	\
	    if (_count == 0) continue;		\
	case '\0':	/* Done */		\
            BUFFER[_count++] = '\0';              \
	    break;				\
	default:	/* copy char */		\
            if (_count == (MAXLINE - 1))	\
	    { 	        /* cut into 2 lines */	\
                BUFFER[_count++] = '\0';	\
	        POINTER--;			\
	        break;				\
	    }					\
            BUFFER[_count++] = _chr;	        \
	    continue;				\
	}					\
	break;					\
    }						\
}


//=======================================================================
//      J U D Y   M E T H O D
//=======================================================================

#ifdef JUDYMETHOD
#define ADTMETHOD       "JUDY"

#include <Judy.h>

#define MEMOVD          0       /* no overhead in Judy */
#define INITARRAY       Pvoid_t JudyArray = NULL

// store string into array
#define STORESTRING(STRING)                                             \
{                                                                       \
        PWord_t _PValue;                                                \
        JSLI(_PValue, JudyArray, (uint8_t *) STRING);                   \
        if (++(*_PValue) != 1) gDupCnt++;  /* count dup strings */      \
}

// get pointer to Value associated with string

#define GETSTRING(STRING)                                               \
{                                                                       \
        PWord_t _PValue;                                                \
        JSLG(_PValue, JudyArray, (uint8_t *) STRING);                   \
        if (_PValue == NULL)                 /* not found -- bug */     \
        {                                                               \
               printf("GETSTRING failed(Judy) -- BUG!\n\n");            \
               exit(1);                                                 \
        }                                                               \
}

#endif // JUDYMETHOD

// Note: this optimization by J. Zobel should not be necessary
// with the -static compile option (linux). (dlb)

#ifdef SLOW_STRCMP
int
scmp(char *s1, char *s2)
{
    while (*s1 != '\0' && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}
#else // ! SLOW_STRCMP

#define scmp strcmp

#endif // ! SLOW_STRCMP

//=======================================================================
//      H A S H   M E T H O D
//=======================================================================

#ifdef HASHMETHOD
#define ADTMETHOD       "HASH"

#define MEMOVD	(sizeof(ht))     /* the hash table is overhead */

#define STORESTRING(STRING)   hashinsert(ht, STRING)

#define GETSTRING(STRING)                                               \
{                                                                       \
        HASHREC *_return;                                               \
        _return = hashsearch(ht, STRING);                               \
        if (_return == NULL)              /* not found -- bug */        \
        {                                                               \
               printf("GETSTRING(hash) failed -- BUG!\n\n");            \
               exit(1);                                                 \
        }                                                               \
}

/* Author J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. */

#define TSIZE (1LU << 20)  /* many processors need this to be a pwr of 2 */
#define SEED	1159241
#define HASHFN  bitwisehash

#define INITARRAY       static HASHREC *ht[TSIZE]
#define SIZEOFINIT      sizeof(ht[TSIZE])

typedef struct hashrec
{
    char     *word;
    struct hashrec *next;
}
HASHREC;

/* Bitwise hash function.  Note that tsize does not have to be prime. */
unsigned int
bitwisehash(char *word, int tsize, unsigned int seed)
{
    char      c;
    unsigned int h;

    h = seed;
    for (; (c = *word) != '\0'; word++)
    {
        h ^= ((h << 5) + c + (h >> 2));
    }
    return ((unsigned int)((h & 0x7fffffff) % tsize));
}

#ifdef notdef                   // not needed if a static definition used in UNIX
/* Create hash table, initialise ptrs to NULL */
HASHREC **
inithashtable()
{
    int       i;
    HASHREC **ht;

    ht = (HASHREC **) malloc(sizeof(HASHREC *) * TSIZE);

    for (i = 0; i < TSIZE; i++)
        ht[i] = (HASHREC *) NULL;

    return (ht);
}
#endif // notdef

/* Search hash table for given string, return record if found, else NULL */
HASHREC  *
hashsearch(HASHREC ** ht, char *w)
{
    HASHREC  *htmp, *hprv;
    unsigned int hval = HASHFN(w, TSIZE, SEED);

    for (hprv = NULL, htmp = ht[hval];
         htmp != NULL && scmp(htmp->word, w) != 0;
         hprv = htmp, htmp = htmp->next)
    {
        ;
    }

    if (hprv != NULL && htmp != NULL) /* move to front on access */
    {
        hprv->next = htmp->next;
        htmp->next = ht[hval];
        ht[hval] = htmp;
    }

    return (htmp);
}

/* Search hash table for given string, insert if not found */
void
hashinsert(HASHREC ** ht, char *w)
{
    HASHREC  *htmp, *hprv;
    unsigned int hval = HASHFN(w, TSIZE, SEED);

    for (hprv = NULL, htmp = ht[hval];
         htmp != NULL && scmp(htmp->word, w) != 0;
         hprv = htmp, htmp = htmp->next)
    {
        ;
    }

    if (htmp == NULL)
    {
        htmp = (HASHREC *) malloc(sizeof(HASHREC));
        htmp->word = (char *)malloc(strlen(w) + 1);
        strcpy(htmp->word, w);
        htmp->next = NULL;
        if (hprv == NULL)
            ht[hval] = htmp;
        else
            hprv->next = htmp;

        /* new records are not moved to front */
    }
    else
    {
        if (hprv != NULL)       /* move to front on access */
        {
            hprv->next = htmp->next;
            htmp->next = ht[hval];
            ht[hval] = htmp;
        }
        gDupCnt++;              /* count duplicate strings */
    }

    return;
}

#endif // HASHMETHOD

//=======================================================================
//      S P L A Y   M E T H O D
//=======================================================================

#ifdef SPLAYMETHOD
#define ADTMETHOD       "SPLAY"

#define MEMOVD          0       /* not enough overhead to count */

/* Author J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. */

#define ROTATEFAC 11

typedef struct wordrec
{
    char     *word;
    struct wordrec *left, *right;
    struct wordrec *par;
}
TREEREC;

typedef struct ansrec
{
    struct wordrec *root;
    struct wordrec *ans;
}
ANSREC;

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

/* Search for word in a splay tree.  If word is found, bring it to
   root, possibly intermittently.  Structure ans is used to pass
   in the root, and to pass back both the new root (which may or
   may not be changed) and the looked-for record. */
void
splaysearch(ANSREC * ans, char *word)
{
    TREEREC  *curr = ans->root, *par, *gpar;
    int       val;

    scount--;

    if (ans->root == NULL)
    {
        ans->ans = NULL;
        return;
    }
    while (curr != NULL && (val = scmp(word, curr->word)) != 0)
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

    if (scount <= 0 && curr != NULL) /* Move node towards root */
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

void
splayinsert(ANSREC * ans, char *word)
{
    TREEREC  *curr = ans->root, *par, *gpar, *prev = NULL, *wcreate();
    int       val = 0;

    scount--;

    if (ans->root == NULL)
    {
        ans->ans = ans->root = wcreate(word, NULL);
        return;
    }

    while (curr != NULL && (val = scmp(word, curr->word)) != 0)
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
            curr = prev->right = wcreate(word, prev);
        else
            curr = prev->left = wcreate(word, prev);
    }
    else
        gDupCnt++;              /* count duplicate strings */

    ans->ans = curr;

    if (scount <= 0)            /* Move node towards root */
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

/* Create a node to hold a word */
TREEREC  *
wcreate(char *word, TREEREC * par)
{
    TREEREC  *tmp;

    tmp = (TREEREC *) malloc(sizeof(TREEREC));
    tmp->word = (char *)malloc(strlen(word) + 1);
    strcpy(tmp->word, word);
    tmp->left = tmp->right = NULL;

    tmp->par = par;

    return (tmp);
}

#define INITARRAY ANSREC      ans = {0}

#define STORESTRING(STRING)   splayinsert(&ans, STRING)

#define GETSTRING(STRING)     splaysearch(&ans, STRING)

#endif // SPLAYMETHOD

//=======================================================================
//      R E D B L A C K   M E T H O D
//=======================================================================

#ifdef REDBLACKMETHOD
#define ADTMETHOD       "REDBLACK"

#define MEMOVD          0       /* not enough overhead to count */

/* Author J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. */

typedef struct wordrec
{
    char     *word;
    struct wordrec *left, *right;
    struct wordrec *par;
    char      colour;
}
TREEREC;

typedef struct ansrec
{
    struct wordrec *root;
    struct wordrec *ans;
}
ANSREC;

#define RED		0
#define BLACK		1

/* Find word in a redblack tree */

void
redblacksearch(ANSREC * ans, char *word)
{
    TREEREC  *curr = ans->root;
    int       val;

    if (ans->root != NULL)
    {
        while (curr != NULL && (val = scmp(word, curr->word)) != 0)
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
leftrotate(ANSREC * ans, TREEREC * par)
{
    TREEREC  *curr, *gpar;

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
rightrotate(ANSREC * ans, TREEREC * par)
{
    TREEREC  *curr, *gpar;

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

/* Insert word into a redblack tree */
void
redblackinsert(ANSREC * ans, char *word)
{
    TREEREC  *curr = ans->root, *par, *gpar, *prev = NULL, *wcreate();
    int       val;

    if (ans->root == NULL)
    {
        ans->ans = ans->root = wcreate(word, NULL);
        return;
    }
    while (curr != NULL && (val = scmp(word, curr->word)) != 0)
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
            curr = prev->right = wcreate(word, prev);
        else
            curr = prev->left = wcreate(word, prev);

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
    else
        gDupCnt++;              /* count duplicate strings */

    return;
}

/* Create a node to hold a word */
TREEREC  *
wcreate(char *word, TREEREC * par)
{
    TREEREC  *tmp;

    tmp = (TREEREC *) malloc(sizeof(TREEREC));
    tmp->word = (char *)malloc(strlen(word) + 1);
    strcpy(tmp->word, word);
    tmp->left = tmp->right = NULL;

    tmp->par = par;

    return (tmp);
}

#define INITARRAY ANSREC      ans = {0}

#define STORESTRING(STRING)   redblackinsert(&ans, STRING)

#define GETSTRING(STRING)     redblacksearch(&ans, STRING)

#endif // REDBLACKMETHOD

//=======================================================================
//      M A I N   P R O G R A M  -by-  Doug Baskins
//=======================================================================

// error routine for system routines for accessing file

#define FILERROR                                                        \
{                                                                       \
    printf("%s: Cannot open file \"%s\": %s "                           \
		"(errno = %d)\n", argv[0], argv[1], strerror(errno),    \
		errno);                                                 \
    fprintf(stderr, "%s: Cannot open file \"%s\": %s "                  \
		"(errno = %d)\n", argv[0], argv[1], strerror(errno),    \
		errno);                                                 \
    exit(1);                                                            \
}


//=======================================================================
//      M E A S U R E  A D T   S P E E D  and  M E M O R Y  U S A G E
//=======================================================================

int
main(int argc, char *argv[])
{
    TIMER_vars(tm);             // declare timer variables
    int       fd;               // to read file.
    struct stat statbuf;        // to get size of file
    char     *Pfile;            // ram address of file
    size_t    fsize;            // file size in bytes
    char     *FSmap;            // start address of mapped file
    char     *FEmap;            // end address+1 of mapped file
    char      String[MAXLINE];  // input buffer
    int       StrCnt;           // line counter
    int       ii;               // temp

    INITARRAY;

// CHECK FOR REQUIRED INPUT FILE PARAMETER:

    if (argc != 2)
    {
        printf("Usage: %s <text file>\n", argv[0]);
        exit(1);
    }

// GET FILE SIZE

    if (stat(argv[1], &statbuf) == -1)
        FILERROR;

    fsize = statbuf.st_size;

// OPEN INPUT FILE:

    if ((fd = open(argv[1], O_RDONLY)) == -1)
        FILERROR;

// MEMORY MAP FILE

    Pfile =
        (char *)mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (Pfile == (char *)-1)
        FILERROR;

    FEmap = Pfile + fsize;      // set end+1 address

// print command name + run arguments

    printf("# ");
    for (ii = 0; ii < argc; ii++)
        printf("%s ", argv[ii]);
    printf("\n");
    fflush(stdout);

// file is in RAM, read again to measure time

    STARTTm(tm);                // start timer

    for (StrCnt = 0, FSmap = Pfile; FSmap < FEmap; )
    {
        GETLINE(String, FSmap); // 'read' next string

        StrCnt++;
    }
    ENDTm(DeltaUSec, tm);       // end timer
//
//  print header - 6 fields

    printf("#  lines avg_linelen  getline   stored");
    printf(" RAMused/line  store/ln lookup/ln ADT\n");

//  print numb lines  filesize/line

    printf("%8u", StrCnt);      // Number of lines
    printf(" %6.1f byts", (double)fsize / (double)StrCnt); // filesize/line
    fflush(stdout);

//  print time to read a line

    printf(" %5.3f uS", DeltaUSec / (double)StrCnt);
    fflush(stdout);

// INPUT/STORE LOOP:

// read each input line into String and store into ADT

    STARTmem;                   // current malloc() mem usage
    STARTTm(tm);                // start timer

    for (FSmap = Pfile; FSmap < FEmap; )
    {
        GETLINE(String, FSmap);  // 'read' next string

        STORESTRING(String);
    }
    ENDTm(DeltaUSec, tm);       // end timer
    ENDmem;                     // current malloc() mem usage

//  print number of non-duplicate lines

    printf(" %8u", StrCnt - gDupCnt); // duplicate lines

//  print RAM used by malloc() by ADT to store data

    printf(" %7.1f byts", (double)(DeltaMem + MEMOVD) /
            (double)(StrCnt - gDupCnt));

//  print time per line to store the data

    printf(" %6.3f uS", DeltaUSec / (double)StrCnt);
    fflush(stdout);

// READ BACK LOOP

    STARTTm(tm);                // start timer

    for (FSmap = Pfile; FSmap < FEmap; )
    {
        GETLINE(String, FSmap);  // 'read' next string

        GETSTRING(String);
    }
    ENDTm(DeltaUSec, tm);       // end timer

//  print time per line to lookup the data from the ADT

    printf(" %6.3f uS", DeltaUSec / (double)StrCnt); // store time/line
    printf(" %s\n", ADTMETHOD);

    return (0);                 // done

}                               // main()
