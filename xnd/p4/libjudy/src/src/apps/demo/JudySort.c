#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <Judy.h>

// By Doug Baskins Aug 2002 - for JudySL man page

int     // Usage:  JudySort < file_to_sort
main()
{
    Pvoid_t   PJArray = (PWord_t)NULL;  // Judy array.
    PWord_t   PValue;                   // Judy array element.
    Word_t    Bytes;                    // size of JudySL array.
    char      Index[BUFSIZ];            // string to sort.

    while (fgets(Index, sizeof(Index), stdin) != (char *)NULL)
    {
        JSLI(PValue, PJArray, Index);   // store string into array
        ++(*PValue);                    // count instances of string
    }
    Index[0] = '\0';                    // start with smallest string.
    JSLF(PValue, PJArray, Index);       // get first string
    while (PValue != NULL)
    {
        while ((*PValue)--)             // print duplicates
            printf("%s", Index);
        JSLN(PValue, PJArray, Index);   // get next string
    }
    JSLFA(Bytes, PJArray);              // free array

    fprintf(stderr, "The JudySL array used %lu bytes of memory\n", Bytes);
    return (0);
}
