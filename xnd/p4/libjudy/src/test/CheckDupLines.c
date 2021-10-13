#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <Judy.h>

//#include "JudyHS.h" 			// for Judy.h without JudyHS*()

// By Doug Baskins Apr 2004 - for JudyHS man page

#define MAXLINE 1000000			/* max length of line */
char      Index[MAXLINE];            	// string to check

int     // Usage:  CheckDupLines < file
main()
{
    Pvoid_t   PJArray = (PWord_t)NULL;  // Judy array.
    PWord_t   PValue;                   // Judy array element.
    Word_t    Bytes;                    // size of JudyHS array.
    Word_t    LineNumb = 0;		// current line number
    Word_t    Dups = 0;			// number of duplicate lines

    while (fgets(Index, MAXLINE, stdin) != (char *)NULL)
    {
	LineNumb++;			// line number

//	store string into array
        JHSI(PValue, PJArray, Index, strlen(Index)); 
        if (*PValue)			// check if duplicate
	{
            Dups++;			// yes, count
	    printf("Duplicate lines %lu:%lu:%s", *PValue, LineNumb, Index);
	}
	else
	{
	    *PValue = LineNumb;		// store Line number
	}
    }
    printf("%lu Duplicates, free JudyHS array of %lu Lines\n", 
		    Dups, LineNumb - Dups);
    JHSFA(Bytes, PJArray);              // free array
    printf("The JudyHS array allocated %lu bytes of memory\n", Bytes);
    return (0);
}
