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

// @(#) $Revision: 4.38 $ $Source: /judy/src/JudySL/JudySL.c $
//
// JUDY FUNCTIONS FOR STRING INDEXES, where associated values are longs.  One
// JudySL*() corresponds to each JudyL*() function (with exceptions).
//
// See the manual entry for details.
//
// METHOD:  Break up each null-terminated Index (string) into chunks of W
// bytes, where W is the machines word size, with null-padding in the last
// word if necessary.  Store strings as a tree of JudyL arrays, that is, array
// of array of array...  where each level consumes W bytes (one word) as an
// index to the JudyL array at that level.  Since strings can begin on
// arbitrary byte boundaries, copy each chunk of W bytes from Index into a
// word-aligned object before using it as a Judy index.
//
// The JudySL tree also supports "single-index shortcut leaves".  A simple
// JudySL array (tree of JudyL arrays) would go as many levels deep as the
// Index (string) is long, which wastes time and memory when an Index is unique
// beyond a certain point.  When theres just one Index under a pointer, given
// a reliable way to tell that the pointer is not a root pointer to another
// JudyL array, it should save a lot of time to instead point to a "leaf"
// object, similar to leaves in JudyL arrays.
//
// TBD:  Multi-index leaves, like those in JudyL, are also worth considering,
// but their payback for JudySL is less certain.  Likewise, shortcut branches
// are worth considering too.
//
// This code uses the Judy.h definitions and Doug Baskins convention of a "P"
// prefix for pointers, except no "P" for the first level of char * (strings).

// IMPORTS:

#include <string.h>                     // for strcmp(), strlen(), strcpy()
#include <Judy.h>

#ifndef NDEDUG
#define NDEBUG 1
#endif
#include <assert.h>

//=======================================================================
// Compile:
//
//    cc -O JudyHS.c -c
//
//    Notes:
//    1) use -DJU_64BIT for 64 bit compiles (HP, Sun, IPF, Motorola/IBM? etc..)
//    2) In gcc version 3.3.1 for a Centrino, -O2 is faster than -O
//    3) In gcc version 3.3.2 for a Centrino, -O3 is faster than -O2
//=======================================================================

#define JU_SET_ERRNO(PJERROR, JERRNO)           \
{                                               \
    if (PJERROR != (PJError_t)NULL)             \
    {                                           \
        JU_ERRNO(PJERROR) = (JERRNO);           \
        JU_ERRID(PJERROR) = __LINE__;           \
    }                                           \
}

#define JU_SET_ERRNO_NONNULL(PJERROR, JERRNO)   \
{                                               \
    JU_ERRNO(PJERROR) = (JERRNO);               \
    JU_ERRID(PJERROR) = __LINE__;               \
}

// SUPPORT FOR HANDLING WORDS:

#define WORDSIZE     (sizeof (Word_t))  // bytes in word = JudyL index.
#define WORDS(BYTES) (((BYTES) + WORDSIZE - 1) / WORDSIZE)      // round up.

// To mark a pointer is to a "short cut leaf", set least bit

#define IS_PSCL(PSCL)     (((Word_t) (PSCL)) & JLAP_INVALID)
#define CLEAR_PSCL(PSCL)  ((Pscl_t)(((Word_t) (PSCL)) & (~JLAP_INVALID)))
#define SET_PSCL(PSCL)    (((Word_t) (PSCL)) | JLAP_INVALID)

// MISCELLANEOUS GLOBALS:

// Get the Index (string) length in bytes, including the trailing \0, which
// is an integral part of the string:

// A string is "in the last word" if a previously-set byte count is at or below
// the system word size, or in some cases if the last byte in the (null-padded)
// word is null (assume big-endian, including in a register on a little-endian
// machine):

#define LASTWORD_BY_VALUE(WORD) (! ((WORD) & 0xffL))

#ifdef JU_64BIT

// copy from 1..7 bytes from string to Word_t and test if \0 bytes
//
#define        COPYSTRINGtoWORD(WORD,STR)               \
{                                                       \
    do                                                  \
    {                                                   \
        uint8_t chr;                                    \
        WORD =      (Word_t)(STR)[0] << 56;             \
        if (!(WORD)) break;                             \
        if (!(chr  = (STR)[1])) break;                  \
        WORD += ((Word_t)(chr) << 48);                  \
        if (!(chr  = (STR)[2])) break;                  \
        WORD += ((Word_t)(chr) << 40);                  \
        if (!(chr  = (STR)[3])) break;                  \
        WORD += ((Word_t)(chr) << 32);                  \
        if (!(chr  = (STR)[4])) break;                  \
        WORD += ((Word_t)(chr) << 24);                  \
        if (!(chr  = (STR)[5])) break;                  \
        WORD += ((Word_t)(chr) << 16);                  \
        if (!(chr  = (STR)[6])) break;                  \
        WORD += ((Word_t)(chr) << 8) + (STR)[7];        \
    } while(0);                                         \
}

// copy Word_t from 1..8 bytes to string and test of \0 bytes
//
#define         COPYWORDtoSTRING(STR,WORD)                      \
{                                                               \
    do                                                          \
    {                                                           \
        if (!((STR)[0] = (uint8_t)((WORD) >> 56))) break;       \
        if (!((STR)[1] = (uint8_t)((WORD) >> 48))) break;       \
        if (!((STR)[2] = (uint8_t)((WORD) >> 40))) break;       \
        if (!((STR)[3] = (uint8_t)((WORD) >> 32))) break;       \
        if (!((STR)[4] = (uint8_t)((WORD) >> 24))) break;       \
        if (!((STR)[5] = (uint8_t)((WORD) >> 16))) break;       \
        if (!((STR)[6] = (uint8_t)((WORD) >>  8))) break;       \
        (STR)[7]       = (uint8_t)(WORD);                       \
    } while(0);                                                 \
}

#else  // JU_32BIT

// copy from 1..4 bytes from string to Word_t and test if \0 bytes

#define        COPYSTRINGtoWORD(WORD,STR)               \
{                                                       \
    do                                                  \
    {                                                   \
        uint8_t chr;                                    \
        WORD =       (STR)[0] << 24;                    \
        if (WORD == 0) break;                           \
        if (!(chr  = (STR)[1])) break;                  \
        WORD += (Word_t)(chr << 16);                    \
        if (!(chr  = (STR)[2])) break;                  \
        WORD += (Word_t)(chr << 8) + (STR)[3];          \
    } while(0);                                         \
}

// copy Word_t from 1..4 bytes to string and test of \0 bytes

#define        COPYWORDtoSTRING(STR,WORD)                       \
{                                                               \
    do                                                          \
    {                                                           \
        if (!((STR)[0] = (uint8_t)((WORD) >> 24))) break;       \
        if (!((STR)[1] = (uint8_t)((WORD) >> 16))) break;       \
        if (!((STR)[2] = (uint8_t)((WORD) >>  8))) break;       \
        (STR)[3]       = (uint8_t)(WORD);                       \
    } while(0);                                                 \
}
#endif // JU_32BIT


// SUPPORT FOR SINGLE-INDEX SHORTCUT LEAVES:

typedef struct SHORCUTLEAF
{
    Pvoid_t   scl_Pvalue;               // callers value area.
    uint8_t   scl_Index[WORDSIZE];      // base Index string.
} scl_t  , *Pscl_t;

// overhead of the scl_Pvalue only, the scl_Index is calculate elsewhere

#define STRUCTOVD       (sizeof(scl_t) - WORDSIZE)

// How big to malloc a shortcut leaf; stringlen should already include the
// trailing null char:

#define SCLSIZE(LEN)  (((LEN) + STRUCTOVD + WORDSIZE - 1) / WORDSIZE)

// string routines, may replace with your own
//
#define STRCMP(S1,S2)   strcmp((void *)(S1), (void *)(S2))
#define STRCPY(S1,S2)   strcpy((void *)(S1), (void *)(S2))
#define STRLEN(S1)      (strlen((void *)(S1)) + 1)


// Index and value area for a shortcut leaf, depending on how it matches the
// undecoded remainder of the Index, given a Pscl_t that includes type bits
// that must be cleared:
//
// PSCLINDEX() and PSCLVALUE() are also useful when Pscl contains uncleared
// TYPE bits.
//
// Note:  SCLCMP() cannot take advantage of knowing the Index length because
// the scl_Index length is not pre-known when these macros are used.

#define PSCLINDEX(PSCL)  ((CLEAR_PSCL(PSCL))->scl_Index)
#define PSCLVALUE(PSCL)  ((CLEAR_PSCL(PSCL))->scl_Pvalue)

#define SCLCMP(INDEX,PSCL) STRCMP(INDEX, PSCLINDEX(PSCL))

#define PPSCLVALUE_EQ(INDEX,PSCL)                                       \
    ((SCLCMP(INDEX, PSCL) == 0) ? &PSCLVALUE(PSCL) : (PPvoid_t)NULL)

#define PPSCLVALUE_LT(INDEX,PSCL)                                       \
    ((SCLCMP(INDEX, PSCL) < 0) ? &PSCLVALUE(PSCL) : (PPvoid_t)NULL)

#define PPSCLVALUE_GT(INDEX,PSCL)                                       \
    ((SCLCMP(INDEX, PSCL) > 0) ? &PSCLVALUE(PSCL) : (PPvoid_t)NULL)

// Common in-lined code to append or free a shortcut leaf:
//
// See header comments about premature return().  Note that malloc() does not
// pre-zero the memory, so ensure scl_Pvalue is zeroed, just like a value area
// in a JudyL array.  Hope strcpy() is fast enough in this context.

#define APPEND_SCL(PSCL,PPARRAY,INDEX,LEN,PJERROR)                      \
{                                                                       \
    if (((PSCL) = (Pscl_t) JudyMalloc(SCLSIZE(LEN))) == (Pscl_t)NULL)   \
    {                                                                   \
        JU_SET_ERRNO(PJERROR, JU_ERRNO_NOMEM);                          \
        return (PPJERR);                                                \
    }                                                                   \
    *(PPARRAY) = (Pvoid_t)SET_PSCL(PSCL);                               \
    ((PSCL)->scl_Pvalue) = (Pvoid_t)NULL;                               \
    (void)STRCPY((PSCL)->scl_Index, INDEX);                             \
}

// "FORWARD" DECLARATIONS:

static void JudySLModifyErrno(PJError_t PJError,
                              Pcvoid_t PArray, Pcvoid_t PArrayOrig);
static int JudySLDelSub(PPvoid_t PPArray, PPvoid_t PPArrayOrig,
                        const uint8_t * Index, Word_t len, PJError_t PJError);
static PPvoid_t JudySLPrevSub(Pcvoid_t PArray, uint8_t * Index, int orig,
                              Word_t len, PJError_t PJError);
static PPvoid_t JudySLNextSub(Pcvoid_t PArray, uint8_t * Index, int orig,
                              Word_t len, PJError_t PJError);

// ****************************************************************************
// J U D Y   S L   M O D I F Y   E R R N O
//
// Common code for error translation:  When a caller passes an invalid JAP
// ("not a JudyL pointer"), OR if the JudySL array is corrupted at a lower
// level, various JudyL*() calls return JU_ERRNO_NOTJUDYL.  If the caller wants
// detailed error info, convert this particular error to JU_ERRNO_NOTJUDYSL if
// at the top of the tree, otherwise convert it to JU_ERRNO_CORRUPT, meaning
// there was a corruption (the only one even detectable outside JudyL) in the
// JudySL tree; but pass through any other errors unaltered.

static void
JudySLModifyErrno(PJError_t PJError,    // to modify if non-null.
                  Pcvoid_t PArray,      // current JudyL array.
                  Pcvoid_t PArrayOrig   // top-of-tree JudyL array.
    )
{                                       //  map this Judy errno.
    if ((PJError != PJE0) && (JU_ERRNO(PJError) == JU_ERRNO_NOTJUDYL))
    {
        if (PArray == PArrayOrig)       // callers fault.
        {
            JU_SET_ERRNO_NONNULL(PJError, JU_ERRNO_NOTJUDYSL);
        }
        else                            // lower level.
        {
            JU_SET_ERRNO_NONNULL(PJError, JU_ERRNO_CORRUPT);
        }
    }
}                                       // JudySLModifyErrno()

// ****************************************************************************
// J U D Y   S L   G E T
//
// See comments in file header and below.

PPvoid_t
JudySLGet(Pcvoid_t PArray, const uint8_t * Index, PJError_t PJError)
{
    const uint8_t *pos = Index;         // place in Index.
    Word_t    indexword;                // buffer for aligned copy.
    PPvoid_t  PPValue;                  // from JudyL array.

// CHECK FOR CALLER ERROR (NULL POINTER):

    if (Index == (uint8_t *) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPINDEX);
        return (PPJERR);
    }

// SEARCH NEXT LEVEL JUDYL ARRAY IN TREE:
//
// Use or copy each word from the Index string and check for it in the next
// level JudyL array in the array tree, but first watch for shortcut leaves.
// Upon invalid Index or end of Index (string) in current word, return.

    do                                  // until return.
    {
        if (IS_PSCL(PArray))            // a shortcut leaf.
            return (PPSCLVALUE_EQ(pos, PArray));

        COPYSTRINGtoWORD(indexword, pos);       // copy next 4[8] bytes.

        JLG(PPValue, PArray, indexword);

        if ((PPValue == (PPvoid_t) NULL) || LASTWORD_BY_VALUE(indexword))
            return (PPValue);

// CONTINUE TO NEXT LEVEL DOWN JUDYL ARRAY TREE:
//
// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated here as a dead-end (not a core
// dump or assertion; see version 1.25).

        pos += WORDSIZE;
        PArray = *PPValue;              // each value -> next array.
    } while(1);                         // forever
//  NOTREACHED JudySLGet()
}

// ****************************************************************************
// J U D Y   S L   I N S
//
// See also the comments in JudySLGet(), which is somewhat similar, though
// simpler.
//
// Theory of operation:
//
// Upon encountering a null pointer in the tree of JudyL arrays, insert a
// shortcut leaf -- including directly under a null root pointer for the first
// Index in the JudySL array.
//
// Upon encountering a pre-existing shortcut leaf, if the old Index is equal to
// the new one, return the old value area.  Otherwise, "carry down" the old
// Index until the old and new Indexes diverge, at which point each Index
// either terminates in the last JudyL array or a new shortcut leaf is inserted
// under it for the Indexs remainder.
//
// TBD:  Running out of memory below the starting point causes a premature
// return below (in several places) and leaves a dead-end in the JudySL tree.
// Ideally the code here would back this out rather than wasting a little
// memory, but in lieu of that, the get, delete, and search functions
// understand dead-ends and handle them appropriately.

PPvoid_t
JudySLIns(PPvoid_t PPArray, const uint8_t * Index, PJError_t PJError)
{
    PPvoid_t  PPArrayOrig = PPArray;    // for error reporting.
    const uint8_t *pos = Index;         // place in Index.
    const uint8_t *pos2 = (uint8_t *) NULL;     // old Index (SCL being moved).
    Word_t    len;                      // bytes remaining.

// Note:  len2 is set when needed and only used when valid, but this is not
// clear to gcc -Wall, so initialize it here to avoid a warning:

    Word_t    len2 = 0;                 // for old Index (SCL being moved).
    Word_t    scl2 = 0;                 // size in words of SCL
    Word_t    indexword;                // buffer for aligned copy.
    Word_t    indexword2;               // for old Index (SCL being moved).
    PPvoid_t  PPValue;                  // from JudyL array.
    PPvoid_t  PPValue2;                 // for old Index (SCL being moved).
    Pscl_t    Pscl = (Pscl_t) NULL;     // shortcut leaf.
    Pscl_t    Pscl2;                    // for old Index (SCL being moved).

// CHECK FOR CALLER ERROR (NULL POINTERS):

    if (PPArray == (PPvoid_t) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPPARRAY);
        return (PPJERR);
    }
    if (Index == (uint8_t *) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPINDEX);
        return (PPJERR);
    }

    len = STRLEN(Index);        // bytes remaining.

// APPEND SHORTCUT LEAF:
//
// If PPArray, which is the root pointer to the first or next JudyL array in
// the tree, points to null (no next JudyL array), AND there is no shortcut
// leaf being carried down, append a shortcut leaf here for the new Index, no
// matter how much of the Index string remains (one or more bytes, including
// the trailing \0).

    while (1)                           // until return.
    {
        if (*PPArray == (Pvoid_t)NULL)  // no next JudyL array.
        {
            if (Pscl == (Pscl_t) NULL)  // no SCL being carried down.
            {
                APPEND_SCL(Pscl, PPArray, pos, len, PJError);   // returns if error.
                return (&(Pscl->scl_Pvalue));
            }
            // else do nothing here; see below.
        }

// CARRY DOWN PRE-EXISTING SHORTCUT LEAF:
//
// When PPArray points to a pre-existing shortcut leaf, if its Index is equal
// to the Index to be inserted, meaning no insertion is required, return its
// value area; otherwise, "move it aside" and "carry it down" -- replace it
// (see below) with one or more levels of JudyL arrays.  Moving it aside
// initially just means setting Pscl non-null, both as a flag and for later
// use, and clearing the pointer to the SCL in the JudyL array.

        else if (IS_PSCL(*PPArray))
        {
            assert(Pscl == (Pscl_t) NULL);      // no nested SCLs.

            Pscl = CLEAR_PSCL(*PPArray);

            pos2 = Pscl->scl_Index;     // note: pos2 is always word-aligned.
            len2 = STRLEN(pos2);        // bytes remaining.

//          first check if string is already inserted

            if ((len == len2) && (STRCMP(pos, pos2) == 0))
                return (&(Pscl->scl_Pvalue));

            *PPArray = (Pvoid_t)NULL;   // disconnect SCL.

            scl2 = SCLSIZE(len2);       // save for JudyFree

            // continue with *PPArray now clear, and Pscl, pos2, len2 set.
        }

// CHECK IF OLD AND NEW INDEXES DIVERGE IN THE CURRENT INDEX WORD:
//
// If a shortcut leaf is being carried down and its remaining Index chars now
// diverge from the remaining chars of the Index being inserted, that is, if
// the next words of each Index differ, "plug in" the old Index here, in a new
// JudyL array, before proceeding.
//
// Note:  Call JudyLIns() for the SCL Index word before calling it for the new
// Index word, so PPValue remains correct for the latter.  (JudyLIns() return
// values are not stable across multiple calls.)
//
// Note:  Although pos2 is word-aligned, and a Pscl_t is a whole number of
// words in size, pos2 is not certain to be null-padded through a whole word,
// so copy it first to an index word for later use.
//
// See header comments about premature return().

        COPYSTRINGtoWORD(indexword, pos);       // copy next 4[8] bytes.

        if (Pscl != (Pscl_t) NULL)
        {
            COPYSTRINGtoWORD(indexword2, pos2); // copy next 4[8] bytes.

            if (indexword != indexword2)        // SCL and new Indexes diverge.
            {
                assert(*PPArray == (Pvoid_t)NULL);      // should be new JudyL array.

// Note:  If JudyLIns() returns JU_ERRNO_NOTJUDYL here, *PPArray should not be
// modified, so JudySLModifyErrno() can do the right thing.

                if ((PPValue2 = JudyLIns(PPArray, indexword2, PJError))
                    == PPJERR)
                {
                    JudySLModifyErrno(PJError, *PPArray, *PPArrayOrig);
                    return (PPJERR);
                }

                assert(PPValue2 != (PPvoid_t) NULL);

// If the old (SCL) Index terminates here, copy its value directly into the
// JudyL value area; otherwise create a new shortcut leaf for it, under
// *PPValue2 (skipping the word just inserted), and copy its value to the new
// SCL:

                if (len2 <= WORDSIZE)
                {
                    *((PWord_t)PPValue2) = (Word_t)(Pscl->scl_Pvalue);
                }
                else
                {
                    APPEND_SCL(Pscl2, PPValue2, pos2 + WORDSIZE,
                               len2 - WORDSIZE, PJError);
                    (Pscl2->scl_Pvalue) = Pscl->scl_Pvalue;
                }
//              old SCL no longer needed.

                JudyFree((void *)Pscl, scl2);

                Pscl = (Pscl_t) NULL;
            }
        }

// APPEND NEXT LEVEL JUDYL ARRAY TO TREE:
//
// If a shortcut leaf was carried down and diverged at this level, the code
// above already appended the new JudyL array, but the next word of the new
// Index still must be inserted in it.
//
// See header comments about premature return().
//
// Note:  If JudyLIns() returns JU_ERRNO_NOTJUDYL here, *PPArray should not be
// modified, so JudySLModifyErrno() can do the right thing.

        if ((PPValue = JudyLIns(PPArray, indexword, PJError)) == PPJERR)
        {
            JudySLModifyErrno(PJError, *PPArray, *PPArrayOrig);
            return (PPJERR);
        }

        assert(PPValue != (PPvoid_t) NULL);

// CHECK IF NEW INDEX TERMINATES:
//
// Note that if it does, and an old SCL was being carried down, it must have
// diverged by this point, and is already handled.

        if (len <= WORDSIZE)
        {
            assert(Pscl == (Pscl_t) NULL);
            return (PPValue);           // is value for whole Index string.
        }

        pos += WORDSIZE;
        len -= WORDSIZE;
        pos2 += WORDSIZE;               // useless unless Pscl is set.
        len2 -= WORDSIZE;

        PPArray = PPValue;              // each value -> next array.
    }                                   // while.
}                                       // NOTREACHED, JudySLIns()

// ****************************************************************************
// J U D Y   S L   D E L
//
// See the comments in JudySLGet(), which is somewhat similar.
//
// Unlike JudySLGet() and JudySLIns(), recurse downward through the tree of
// JudyL arrays to find and delete the given Index, if present, and then on the
// way back up, any of its parent arrays which ends up empty.
//
// TECHNICAL NOTES:
//
// Recursion seems bad, but this allows for an arbitrary-length Index.  Also, a
// more clever iterative solution that used JudyLCount() (see below) would
// still require a function call per tree level, so why not just recurse?
//
// An earlier version (1.20) used a fixed-size stack, which limited the Index
// size.  We were going to replace this with using JudyLCount(), in order to
// note and return to (read this carefully) the highest level JudyL array with
// a count of 1, all of whose descendant JudyL arrays also have a count of 1,
// and delete from that point downwards.  This solution would traverse the
// array tree downward looking to see if the given Index is in the tree, then
// if so, delete layers downwards starting below the last one that contains
// other Indexes than the one being deleted.
//
// TBD:  To save time coding, and to very likely save time overall during
// execution, this function does "lazy deletions", or putting it more nicely,
// it allows "hysteresis" in the JudySL tree, when shortcut leafs are present.
// It only removes the specified Index, and recursively any empty JudyL arrays
// above it, without fully reversing the effects of JudySLIns().  This is
// probably OK because any application that calls JudySLDel() is likely to call
// JudySLIns() again with the same or a neighbor Index.

int
JudySLDel(PPvoid_t PPArray, const uint8_t * Index, PJError_t PJError)   // optional, for returning error info.
{

// Check for caller error (null pointer):

    if (PPArray == (PPvoid_t) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPPARRAY);
        return (JERR);
    }
    if (Index == (uint8_t *) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPINDEX);
        return (JERR);
    }

// Do the deletion:

    return (JudySLDelSub(PPArray, PPArray, Index, STRLEN(Index), PJError));

}                                       // JudySLDel()

// ****************************************************************************
// J U D Y   S L   D E L   S U B
//
// This is the "engine" for JudySLDel() that expects aligned and len to already
// be computed (only once).  See the header comments for JudySLDel().

static int
JudySLDelSub(PPvoid_t PPArray,          // in which to delete.
             PPvoid_t PPArrayOrig,      // for error reporting.
             const uint8_t * Index,     // to delete.
             Word_t len,                // bytes remaining.
             PJError_t PJError)         // optional, for returning error info.
{
    Word_t    indexword;                // next word to find.
    PPvoid_t  PPValue;                  // from JudyL array.
    int       retcode;                  // from lower-level call.

    assert(PPArray != (PPvoid_t) NULL);
    assert(Index != (uint8_t *) NULL);

// DELETE SHORTCUT LEAF:
//
// As described above, this can leave an empty JudyL array, or one containing
// only a single other Index word -- which could be, but is not, condensed into
// a higher-level shortcut leaf.  More precisely, at this level it leaves a
// temporary "dead end" in the JudySL tree, similar to when running out of
// memory during JudySLIns(), and this is somewhat cleaned up by higher
// recursions of the same function (see below); but remaining shortcut leaves
// for other Indexes are not coalesced.

    if (IS_PSCL(*PPArray))
    {
        Pscl_t    Pscll = CLEAR_PSCL(*PPArray);
        Word_t    words;

        if (STRCMP(Index, Pscll->scl_Index))
            return (0);                 // incorrect index.

        words = SCLSIZE(STRLEN(Pscll->scl_Index));
        JudyFree((void *)Pscll, words);

        *PPArray = (Pvoid_t)NULL;
        return (1);                     // correct index deleted.
    }

// DELETE LAST INDEX WORD, FROM CURRENT JUDYL ARRAY:
//
// When at the end of the full Index, delete the last word, if present, from
// the current JudyL array, and return the result all the way up.

    COPYSTRINGtoWORD(indexword, Index); // copy next 4[8] bytes.

    if (len <= WORDSIZE)
    {
        if ((retcode = JudyLDel(PPArray, indexword, PJError)) == JERR)
        {
            JudySLModifyErrno(PJError, *PPArray, *PPArrayOrig);
            return (JERR);
        }
        return (retcode);
    }

// DELETE BELOW NON-LAST INDEX WORD IN CURRENT JUDYL ARRAY:
//
// If a word before the end of the full Index is present in the current JudyL
// array, recurse through its value, which must be a pointer to another JudyL
// array, to continue the deletion at the next level.  Return the JudyLGet()
// return if the Indexs current word is not in the JudyL array, or if no
// delete occurs below this level, both of which mean the whole Index is not
// currently valid.
//

    JLG(PPValue, *PPArray, indexword);
    if (PPValue == (PPvoid_t) NULL)
        return (0);                     // Index not in JudySL array.
// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated as a dead-end (not a core dump
// or assertion; see version 1.25).
    if ((retcode =
         JudySLDelSub(PPValue, PPArrayOrig, Index + WORDSIZE,
                      len - WORDSIZE, PJError)) != 1)
    {
        return (retcode);               // no lower-level delete, or error.
    }

// DELETE EMPTY JUDYL ARRAY:
//
// A delete occurred below in the tree.  If the child JudyL array became empty,
// delete the current Index word from the current JudyL array, which could
// empty the current array and null out *PPArray in turn (or pass through an
// error).  Otherwise simply indicate that a deletion did occur.

    if (*PPValue == (Pvoid_t)NULL)
    {
        if ((retcode = JudyLDel(PPArray, indexword, PJError)) == JERR)
        {
            JudySLModifyErrno(PJError, *PPArray, *PPArrayOrig);
            return (JERR);
        }

        return (retcode);
    }

    return (1);
}                                       // JudySLDelSub()

// ****************************************************************************
// J U D Y   S L   P R E V
//
// Recursively traverse the JudySL tree downward using JudyLGet() to look for
// each successive index word from Index in the JudyL array at each level.  At
// the last level for the Index (LASTWORD_BY_LEN()), use JudyLPrev() instead of
// JudyLGet(), to exclude the initial Index.  If this doesnt result in finding
// a previous Index, work back up the tree using JudyLPrev() at each higher
// level to search for a previous index word.  Upon finding a previous index
// word, descend again if/as necessary, this time inclusively, to find and
// return the full previous Index.
//
// Also support shortcut leaves.
//
// Since this function is recursive and it also needs to know if its still
// looking for the original Index (to exclude it at the LASTWORD_BY_LEN()
// level) or for the remaining words of the previous Index (inclusive),
// actually call a subroutine that takes an additional parameter.
//
// See also the technical notes in JudySLDel() regarding the use of recursion
// rather than iteration.

PPvoid_t
JudySLPrev(Pcvoid_t PArray, uint8_t * Index, PJError_t PJError) // optional, for returning error info.
{
// Check for caller error (null pointer), or empty JudySL array:

    if (Index == (uint8_t *) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPINDEX);
        return (PPJERR);
    }

    if (PArray == (Pvoid_t)NULL)
        return ((PPvoid_t) NULL);
// Do the search:
    return (JudySLPrevSub(PArray, Index, /* original = */ 1,
                          STRLEN(Index), PJError));
}                                       // JudySLPrev()

// ****************************************************************************
// J U D Y   S L   P R E V   S U B
//
// This is the "engine" for JudySLPrev() that knows whether its still looking
// for the original Index (exclusive) or a neighbor index (inclusive), and that
// expects aligned and len to already be computed (only once).  See the header
// comments for JudySLPrev().

static    PPvoid_t
JudySLPrevSub(Pcvoid_t PArray, uint8_t * Index, int orig,
              Word_t len,               // bytes remaining.
              PJError_t PJError)        // optional, for returning error info.
{
    Word_t    indexword;                // next word to find.
    PPvoid_t  PPValue;                  // from JudyL array.
// ORIGINAL SEARCH:
//
// When at a shortcut leaf, copy its remaining Index (string) chars into Index
// and return its value area if the current Index is after (greater than) the
// SCLs index; otherwise return null.
    if (orig)
    {
        if (IS_PSCL(PArray))
        {
            if ((PPValue = PPSCLVALUE_GT(Index, PArray)) != (PPvoid_t) NULL)
                (void)STRCPY(Index, PSCLINDEX(PArray));
            return (PPValue);
        }

// If the current Index word:
// - is not the last word in Index (end of string),
// - exists in the current JudyL array, and,
// - a previous Index is found below it, return that Indexs value area.

        COPYSTRINGtoWORD(indexword, Index);     // copy next 4[8] bytes.
        if (len > WORDSIZE)             // not at end of Index.
        {
            JLG(PPValue, PArray, indexword);
            if (PPValue != (PPvoid_t) NULL)
            {

// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated as a dead-end (not a core dump
// or assertion; see version 1.25):

                PPValue = JudySLPrevSub(*PPValue, Index + WORDSIZE,
                                        /* original = */ 1,
                                        len - WORDSIZE, PJError);
                if (PPValue == PPJERR)
                    return (PPJERR);    // propagate error.
                if (PPValue != (PPvoid_t) NULL)
                    return (PPValue);   // see above.
            }
        }

// Search for previous index word:
//
// One of the above conditions is false.  Search the current JudyL array for
// the Index word, if any, prior to the current index word.  If none is found,
// return null; otherwise fall through to common later code.

        if ((PPValue = JudyLPrev(PArray, &indexword, PJError)) == PPJERR)
        {
            JudySLModifyErrno(PJError, PArray, orig ? PArray : (Pvoid_t)NULL);
            return (PPJERR);
        }

        if (PPValue == (PPvoid_t) NULL)
            return ((PPvoid_t) NULL);   // no previous index word.
    }                                   // if.

// SUBSEQUENT SEARCH:
//
// A higher level search already excluded the initial Index, then found a
// previous index word, and is now traversing down to determine the rest of the
// Index and to obtain its value area.  If at a shortcut leaf, return its value
// area.  Otherwise search the current JudyL array backward from the upper
// limit for its last index word.  If no index word is found, return null --
// should never happen unless the JudySL tree is corrupt; otherwise fall
// through to common later code.

    else
    {
        if (IS_PSCL(PArray))            // at shortcut leaf.
        {
            (void)STRCPY(Index, PSCLINDEX(PArray));
            return (&PSCLVALUE(PArray));
        }

        indexword = ~0UL;
        if ((PPValue = JudyLLast(PArray, &indexword, PJError)) == PPJERR)
        {
            JudySLModifyErrno(PJError, PArray, orig ? PArray : (Pvoid_t)NULL);
            return (PPJERR);
        }

// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated as a dead-end (not a core dump
// or assertion; see version 1.25):

        if (PPValue == (PPvoid_t) NULL)
            return ((PPvoid_t) NULL);   // no previous index word.
    }

// FOUND PREVIOUS INDEX WORD:
//
// A previous (if original) or last (if subsequent) index word was located in
// the current JudyL array.  Store it into the callers Index (string).  Then
// if the found (previous) Index ends here, return its value area; otherwise do
// a subsequent search below this point, which should never fail unless the
// JudySL tree is corrupt, but this is detected at a lower level by the above
// assertion.
//
// Note:  Treat Index as unaligned, even if it is aligned, to avoid writing
// past the end of allocated memory (in case its less than a whole number of
// words).

    COPYWORDtoSTRING(Index, indexword); // copy next 4[8] bytes.
    if (LASTWORD_BY_VALUE(indexword))
        return (PPValue);
// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated as a dead-end (not a core dump
// or assertion; see version 1.25):
    return (JudySLPrevSub(*PPValue, Index + WORDSIZE, /* original = */ 0,
                          len - WORDSIZE, PJError));
}                                       // JudySLPrevSub()

// ****************************************************************************
// J U D Y   S L   N E X T
//
// See the comments in JudySLPrev(), which is very similar.
//
// TBD:  Could the two functions call a common engine function with various
// subfunctions and other constants specified?

PPvoid_t
JudySLNext(Pcvoid_t PArray, uint8_t * Index, PJError_t PJError) // optional, for returning error info.
{
// Check for caller error (null pointer), or empty JudySL array:

    if (Index == (uint8_t *) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPINDEX);
        return (PPJERR);
    }

    if (PArray == (Pvoid_t)NULL)
        return ((PPvoid_t) NULL);
// Do the search:
    return (JudySLNextSub(PArray, Index, /* original = */ 1,
                          STRLEN(Index), PJError));
}                                       // JudySLNext()

// ****************************************************************************
// J U D Y   S L   N E X T   S U B
//
// See the comments in JudySLPrevSub(), which is very similar.

static    PPvoid_t
JudySLNextSub(Pcvoid_t PArray, uint8_t * Index, int orig,
              Word_t len,               // bytes remaining.
              PJError_t PJError)        // optional, for returning error info.
{
    Word_t    indexword;                // next word to find.
    PPvoid_t  PPValue;                  // from JudyL array.
    if (orig)
    {
        if (IS_PSCL(PArray))
        {
            if ((PPValue = PPSCLVALUE_LT(Index, PArray)) != (PPvoid_t) NULL)
                (void)STRCPY(Index, PSCLINDEX(PArray));
            return (PPValue);
        }

        COPYSTRINGtoWORD(indexword, Index);     // copy next 4[8] bytes.

        if (len > WORDSIZE)             // not at end of Index.
        {
            JLG(PPValue, PArray, indexword);
            if (PPValue != (PPvoid_t) NULL)
            {
// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated as a dead-end (not a core dump
// or assertion; see version 1.25):

                PPValue = JudySLNextSub(*PPValue, Index + WORDSIZE,
                                        /* original = */ 1,
                                        len - WORDSIZE, PJError);
                if (PPValue == PPJERR)
                    return (PPJERR);    // propagate error.
                if (PPValue != (PPvoid_t) NULL)
                    return (PPValue);   // see above.
            }
        }

        if ((PPValue = JudyLNext(PArray, &indexword, PJError)) == PPJERR)
        {
            JudySLModifyErrno(PJError, PArray, orig ? PArray : (Pvoid_t)NULL);
            return (PPJERR);
        }

        if (PPValue == (PPvoid_t) NULL)
            return ((PPvoid_t) NULL);   // no next index word.
    }
    else
    {
        if (IS_PSCL(PArray))            // at shortcut leaf.
        {
            (void)STRCPY(Index, PSCLINDEX(PArray));
            return (&PSCLVALUE(PArray));
        }

        indexword = 0;
        if ((PPValue = JudyLFirst(PArray, &indexword, PJError)) == PPJERR)
        {
            JudySLModifyErrno(PJError, PArray, orig ? PArray : (Pvoid_t)NULL);
            return (PPJERR);
        }

// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated as a dead-end (not a core dump
// or assertion; see version 1.25):

        if (PPValue == (PPvoid_t) NULL)
            return ((PPvoid_t) NULL);   // no next index word.
    }

    COPYWORDtoSTRING(Index, indexword); // copy next 4[8] bytes
    if (LASTWORD_BY_VALUE(indexword))
        return (PPValue);
// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue; this is automatically treated as a dead-end (not a core dump
// or assertion; see version 1.25):
    return (JudySLNextSub(*PPValue, Index + WORDSIZE, /* original = */ 0,
                          len - WORDSIZE, PJError));
}                                       // JudySLNextSub()

// ****************************************************************************
// J U D Y   S L   F I R S T
//
// Like JudyLFirst(), do a JudySLGet(), then if necessary a JudySLNext().

PPvoid_t
JudySLFirst(Pcvoid_t PArray, uint8_t * Index, PJError_t PJError)        // optional, for returning error info.
{
    PPvoid_t  PPValue;                  // from JudyL array.
    if (Index == (uint8_t *) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPINDEX);
        return (PPJERR);
    }

    if ((PPValue = JudySLGet(PArray, Index, PJError)) == PPJERR)
        return (PPJERR);                // propagate serious error.
    if ((PPValue == (PPvoid_t) NULL)    // first try failed.
        && ((PPValue = JudySLNext(PArray, Index, PJError)) == PPJERR))
    {
        return (PPJERR);                // propagate serious error.
    }

    return (PPValue);
}                                       // JudySLFirst()

// ****************************************************************************
// J U D Y   S L   L A S T
//
// Like JudyLLast(), do a JudySLGet(), then if necessary a JudySLPrev().

PPvoid_t
JudySLLast(Pcvoid_t PArray, uint8_t * Index, PJError_t PJError) // optional, for returning error info.
{
    PPvoid_t  PPValue;                  // from JudyL array.
    if (Index == (uint8_t *) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPINDEX);
        return (PPJERR);
    }

    if ((PPValue = JudySLGet(PArray, Index, PJError)) == PPJERR)
        return (PPJERR);                // propagate serious error.
    if ((PPValue == (PPvoid_t) NULL)    // first try failed.
        && ((PPValue = JudySLPrev(PArray, Index, PJError)) == PPJERR))
    {
        return (PPJERR);                // propagate serious error.
    }

    return (PPValue);
}                                       // JudySLLast()

// ****************************************************************************
// J U D Y   S L   F R E E   A R R A Y
//
// Walk the JudySL tree of JudyL arrays to free each JudyL array, depth-first.
// During the walk, ignore indexes (strings) that end in the current JudyL
// array to be freed.  Just recurse through those indexes which do not end,
// that is, those whose associated value areas point to subsidiary JudyL
// arrays, except for those which point to shortcut leaves.  Return the total
// bytes freed in all of the JudyL arrays at or below the current level.
//
// Like the JudyLFreeArray() and Judy1FreeArray() code, this is written
// recursively, which is probably fast enough, to allow indexes (strings) of
// arbitrary size.  If recursion turns out to be a problem, consider instead
// doing some large, fixed number of iterative descents (like 100) using a
// fixed-size "stack" (array), then recursing upon overflow (relatively
// rarely).

Word_t
JudySLFreeArray(PPvoid_t PPArray, PJError_t PJError)    // optional, for returning error info.
{
    PPvoid_t  PPArrayOrig = PPArray;    // for error reporting.
    Word_t    indexword = 0;            // word just found.
    PPvoid_t  PPValue;                  // from Judy array.
    Word_t    bytes_freed = 0;          // bytes freed at this level.
    Word_t    bytes_total = 0;          // bytes freed at all levels.
    if (PPArray == (PPvoid_t) NULL)
    {
        JU_SET_ERRNO(PJError, JU_ERRNO_NULLPPARRAY);
        return (JERR);
    }

// FREE SHORTCUT LEAF:

    if (IS_PSCL(*PPArray))
    {
        Word_t    freewords;
        Pscl_t    Pscl;

        Pscl = CLEAR_PSCL(*PPArray);

        freewords = SCLSIZE(STRLEN(Pscl->scl_Index));

        JudyFree((void *)Pscl, freewords);

        *PPArray = (Pvoid_t)NULL;

        return (freewords * WORDSIZE);
    }

// FREE EACH SUB-ARRAY (DEPTH-FIRST):
//
// If a previous JudySLIns() ran out of memory partway down the tree, it left a
// null *PPValue.  This is automatically treated correctly here as a dead-end.
//
// An Index (string) ends in the current word iff the last byte of the
// (null-padded) word is null.

    for (PPValue = JudyLFirst(*PPArray, &indexword, PJError);
         (PPValue != (PPvoid_t) NULL) && (PPValue != PPJERR);
         PPValue = JudyLNext(*PPArray, &indexword, PJError))
    {
        if (!LASTWORD_BY_VALUE(indexword))
        {
            if ((bytes_freed = JudySLFreeArray(PPValue, PJError)) == JERR)
                return (JERR);          // propagate serious error.
            bytes_total += bytes_freed;
        }
    }

// Check for a serious error in a JudyL*() call:

    if (PPValue == PPJERR)
    {
        JudySLModifyErrno(PJError, *PPArray, *PPArrayOrig);
        return (JERR);
    }

// Now free the current array, which also nulls the pointer:
//
// Note:  *PPArray can be null here for a totally null JudySL array =>
// JudyLFreeArray() returns zero.

    if ((bytes_freed = JudyLFreeArray(PPArray, PJError)) == JERR)
    {
        JudySLModifyErrno(PJError, *PPArray, *PPArrayOrig);
        return (JERR);
    }
    return (bytes_total + bytes_freed);
}                                       // JudySLFreeArray()
