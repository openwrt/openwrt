// @(#) $Revision: 4.3 $ $Source: /judy/src/funcs/Judy1OpCheck.c $

/*
 * Judy1OpCheck.c
 *
 * Test Judy1Op.c
 *
 * Returns 0 if successful, -1 if test fails.
 */

#include <errno.h>
#include <limits.h>
#include "Judy.h"
#include "Judy1Op.h"

#define LARRAYSIZE(array) sizeof(array)/sizeof(Word_t)

static Word_t set1[] = { 0L,
    5L,
    6L,
    7L,
    1024L,
    11111L,
    65534L,
    65535L,
    65536L,
    555555L,
    ULONG_MAX
};

static Word_t set2[] = { 7L,
    9L,
    1023L,
    12345L,
    65535L,
    ULONG_MAX
};

static Word_t resultAND[] = { 7L,
    65535L,
    ULONG_MAX
};

static Word_t resultOR[] = { 0L,
    5L,
    6L,
    7L,
    9L,
    1023L,
    1024L,
    11111L,
    12345L,
    65534L,
    65535L,
    65536L,
    555555L,
    ULONG_MAX
};

static Word_t result1ANDNOT2[] = { 0L,
    5L,
    6L,
    1024L,
    11111L,
    65534L,
    65536L,
    555555L
};

static Word_t result2ANDNOT1[] = { 9L,
    1023L,
    12345L
};

// fcn to init a Judy array with an array of ulong's
void     *
ularray2Judy(Word_t *ularray, Word_t ularray_size)
{
    Word_t    i;
    void     *PJArray = 0;
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

// fcn to test Judy1Op and check the results
void
testandcheck(void *PJSet1, void *PJSet2, Word_t operation,
             char *opstr, Word_t *result, int result_size)
{
    void     *PJArrayNew;
    Word_t    Index;
    int       i;
    int       judy_rv = 0;
    JError_t  JError;

    printf("Testing Judy1Op(%s) ...", opstr);
    if (Judy1Op(&PJArrayNew, PJSet1, PJSet2, operation, &JError) == JERR)
        printf(" failed, error %d\n", JU_ERRNO(&JError));
    else
    {                                   // check results
        for (i = 0, Index = 0L, judy_rv =
             Judy1First(PJArrayNew, &Index, &JError); judy_rv == 1;
             i++, judy_rv = Judy1Next(PJArrayNew, &Index, &JError))
        {
            if ((i >= result_size) || (Index != result[i]))
            {
                printf("Failed\n");
                return;
            }
        }
        if (i > result_size)
            printf("Failed\n");
        else
            printf("Ok\n");
    }
    return;
}

int
main()
{
    void     *PJSet1 = 0;
    void     *PJSet2 = 0;

    // Test Judy1Op
    // init PJSet1 and PJSet2
    PJSet1 = ularray2Judy(set1, LARRAYSIZE(set1));
    PJSet2 = ularray2Judy(set2, LARRAYSIZE(set2));

    testandcheck(PJSet1, PJSet2, JUDY1OP_AND, "AND",
                 resultAND, LARRAYSIZE(resultAND));
    testandcheck(PJSet1, PJSet2, JUDY1OP_OR, "OR",
                 resultOR, LARRAYSIZE(resultOR));
    testandcheck(PJSet1, PJSet2, JUDY1OP_ANDNOT, "1ANDNOT2",
                 result1ANDNOT2, LARRAYSIZE(result1ANDNOT2));
    testandcheck(PJSet2, PJSet1, JUDY1OP_ANDNOT, "2ANDNOT1",
                 result2ANDNOT1, LARRAYSIZE(result2ANDNOT1));

    return (0);
}
