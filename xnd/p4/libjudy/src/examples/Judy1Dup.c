/* @(#) $Revision: 4.2 $ $Source: /judy/src/funcs/Judy1Dup.c $ */

#include "Judy.h"

/*******************************************************************
 * Name: Judy1Dup
 *
 * Description:
 *       Clone (duplicate) a Judy Array.
 *
 * Parameters:
 *       PPvoid_t PPDest (OUT)
 *            Pointer to a new Judy array with the same
 *            index/value pairs as PSource.
 *            Any initial value pointed to by PPDest is ignored.
 *
 *       Pvoid_t PSource (IN)
 *            Ptr to source Judy array being duplicated.
 *            If PSource is NULL, TRUE is returned since this
 *            is simply a valid Null Judy array.
 *
 *       JError_t *PJError (OUT)
 *            Judy error structure pointer.
 *
 * Returns:
 *       JERR - error, see PJError
 *      !JERR - success
 */

int
Judy1Dup(PPvoid_t PPDest, Pvoid_t PSource, JError_t * PJError)
{
    Pvoid_t   newJArray = 0;            // new Judy1 array to ppopulate
    Word_t    kindex;                   // Key/index
    int       Ins_rv = 0;               // Insert return value

    for (kindex = 0L, Ins_rv = Judy1First(PSource, &kindex, PJError);
         Ins_rv == 1; Ins_rv = Judy1Next(PSource, &kindex, PJError))
    {
        Ins_rv = Judy1Set(&newJArray, kindex, PJError);
    }
    if (Ins_rv == JERR)
        return Ins_rv;

    *PPDest = newJArray;
    return Ins_rv;
}                                       /*  Judy1Dup */
