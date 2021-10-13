
/*
 * @(#) $Revision: 4.2 $ $Source: /judy/src/funcs/Judy1Op.c $
 *
 * Judy1 set operations.
 *
 * The name of this function, "Judy1Op", was carefully chosen from
 * a list of alternatives:
 *
 *   Judy1Op()  - It's hard to see that O is a letter and not a zero.
 *   Judy1Set() - Sounds like you are setting a bit.
 *   Judy1BS()  - BS for Bit Set functions.
 *   Judy1SO()  - SO for Set Operations
 *   Judy1AndIDontGiveADarn() - too long but goes with
 *   Judy1WhoseOnFirst() - now called Judy1First() and
 *   Judy1WhatsOnSecond() - now called Judy1Next()
 *
 * But Judy1SetOp() would conflict with Judy1Set() if we rename Judy1Set(), so
 * Judy1Op() it is.
 */

#include "Judy.h"
#include "Judy1Op.h"

/*******************************************************************
 * Name: Judy1Op
 *
 * Description:
 *       Logical set operations on Judy1 arrays.
 *
 *       All of these operations can be done on an unbounded array because
 *       the dreaded "NOT" is avoided.  The "NOT"'s can be implemented
 *       when Judy1 supports them.
 *
 * Parameters:
 *       PPvoid_t PPDest (OUT)
 *            Ptr to the Judy destination array.
 *            Any initial value pointed to by PPDest is ignored.
 *
 *       Pvoid_t PSet1 (IN)
 *            First Judy1 set.
 *            This will be NULL for an empty Judy1 array.
 *
 *       Pvoid_t PSet2 (IN)
 *            Second Judy1 set.
 *            This will be NULL for an empty Judy1 array.
 *
 *       Word_t Operation (IN)
 *            Operation to be performed (ie. PSet1 {Operation} PSet2)
 *            Valid Operation values are:
 *
 *            JUDY1OP_AND     - intersection of two sets
 *            JUDY1OP_OR      - union of two sets
 *            JUDY1OP_ANDNOT  - set1 with set2 removed
 *
 *       JError_t * PJError (OUT)
 *            Judy Error struct used to return Judy error.
 *
 * Returns:
 *      !JERR if successful
 *       JERR if an error occurs
 *       If the error is a caller error (invalid Operation or no PPDest)
 *       then the PJError error code will be JU_ERRNO_NONE.
 */
int
Judy1Op(PPvoid_t PPDest, Pvoid_t PSet1, Pvoid_t PSet2,
        Word_t Operation, JError_t * PJError)
{
    Pvoid_t   PnewJArray = 0;           // empty Judy array
    Word_t    Index1 = 0L;
    Word_t    Index2 = 0L;
    int       Judy_rv;

    if (!PPDest)
        return JERR;

    switch (Operation)
    {
    case JUDY1OP_AND:
        // step through each array looking for index matches
        Judy_rv = Judy1First(PSet1, &Index1, PJError);
        Judy_rv += Judy1First(PSet2, &Index2, PJError);
        while (Judy_rv == 2)
        {
            if (Index1 < Index2)
            {
                Index1 = Index2;
                Judy_rv = Judy1First(PSet1, &Index1, PJError);
            }
            else if (Index1 > Index2)
            {
                Index2 = Index1;
                Judy_rv = Judy1First(PSet2, &Index2, PJError);
            }
            else
            {
                // do the AND
                Judy_rv = Judy1Set(&PnewJArray, Index1, PJError);
                if (Judy_rv == JERR)
                    return JERR;

                // bump to the next bits
                Judy_rv = Judy1Next(PSet1, &Index1, PJError);
                Judy_rv += Judy1Next(PSet2, &Index2, PJError);
            }
        }
        *PPDest = PnewJArray;
        break;

    case JUDY1OP_OR:
        /* Set all the bits from PSet1 */
        for (Index1 = 0L, Judy_rv = Judy1First(PSet1, &Index1, PJError);
             Judy_rv == 1; Judy_rv = Judy1Next(PSet1, &Index1, PJError))
        {
            if (Judy1Set(&PnewJArray, Index1, PJError) == JERR)
                return JERR;
        }

        /* Set all the bits from PSet2 */
        for (Index1 = 0L, Judy_rv = Judy1First(PSet2, &Index1, PJError);
             Judy_rv == 1; Judy_rv = Judy1Next(PSet2, &Index1, PJError))
        {
            if (Judy1Set(&PnewJArray, Index1, PJError) == JERR)
                return JERR;
        }
        *PPDest = PnewJArray;
        break;

    case JUDY1OP_ANDNOT:
        // PSet1 with PSet2 removed
        // 0010 = PSet1(1010) ANDNOT PSet2(1100)

        for (Index1 = 0L, Judy_rv = Judy1First(PSet1, &Index1, PJError);
             Judy_rv == 1; Judy_rv = Judy1Next(PSet1, &Index1, PJError))
        {
            // if bit doesn't exist in PSet2, then add to result
            if (0 == Judy1Test(PSet2, Index1, PJError))
            {
                if (Judy1Set(&PnewJArray, Index1, PJError) == JERR)
                    return JERR;
            }
        }
        *PPDest = PnewJArray;
        break;

    default:
        return JERR;
    }
    return !JERR;
}                                       /* Judy1Op */
