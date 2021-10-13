
/*
 * @(#) $Revision: 4.3 $ $Source: /judy/src/funcs/Judy1DupCheck.c $
 *
 * Judy1DupCheck.c
 *
 * Test Judy1Dup.c
 */

#include <errno.h>
#include <limits.h>
#include "Judy1Dup.h"

#define LARRAYSIZE(array) sizeof(array)/sizeof(Word_t)

// fcn to init a Judy array with an array of ulong's
Pvoid_t
ularray2Judy(Word_t *ularray, Word_t ularray_size)
{
    Word_t    i;
    Pvoid_t   PJArray = 0;
    JError_t  JError;

    for (i = 0L; i < ularray_size; i++)
    {
        if (Judy1Set(&PJArray, ularray[i], &JError) == JERR)
        {
            printf("ularray2Judy: Judy1Set failure, error %d\n",
                   JU_ERRNO(&JError));
            exit(2);
        }
    }
    return PJArray;
}                                       /* ularray2Judy */

int
main()
{
    static Word_t knowns[] = { 0, 1, 1024, 4095, 4096, 4097, 4098, 123456 };
    int       i;
    Pvoid_t   PJArray = 0;
    Pvoid_t   PJArrayNew = 0;
    Word_t    Index;
    int       Judy_rv;                  // Judy Return value
    JError_t  JError;

    //  populate a judy array with known values
    PJArray = ularray2Judy(knowns, LARRAYSIZE(knowns));

    printf("Testing Judy1Dup ...");

    // dup the judy array
    if ((Judy1Dup(&PJArrayNew, PJArray, &JError)) == JERR)
    {
        printf("Judy1Dup failed: error %d\n", JU_ERRNO(&JError));
        return (2);
    }

    // compare the duped array with known values
    for (i = 0, Index = 0L, Judy_rv = Judy1First(PJArrayNew, &Index, &JError);
         Judy_rv == 1; i++, Judy_rv = Judy1Next(PJArrayNew, &Index, &JError))
    {
        if (Index != knowns[i])
        {
            printf("Judy1DupCheck Failed: Judy1Dup does not match original\n");
            return (2);
        }
    }

    if (i != LARRAYSIZE(knowns))
    {
        printf
            ("Judy1DupCheck Failed: Judy1Dup does not match original (too short)\n");
        exit(2);
    }
    else
        printf("Ok\n");

    return (0);
}
