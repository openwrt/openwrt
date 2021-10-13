#include <stdlib.h>
#include <Judy.h>

main()	// Simple JudyL demo, see "run_demo" script; @(#) $Revision: 4.4 $
{
    Pvoid_t  Parray = (Pvoid_t) NULL;		// empty JudyL array.
    Word_t * Pvalue;				// value for one index.
    char     line[BUFSIZ];			// input line.
    Word_t   index;				// in JudyL array.

    printf("Interactive Judy demo program to input, sort, and list numbers.\n"
	   "Enter a number:  ");  // fflush(stdout); ?

    while (fgets(line, BUFSIZ, stdin))		// input.
    {
	index = strtoul(line, NULL, 0);		// note: bad input => 0.
	JLI(Pvalue, Parray, index);		// insert index in JudyL array.
	++(*Pvalue);				// count duplicates.

	printf("       Index  Dups\n");		// print all saved indexes:
	index = 0;				// start search at zero.
	JLF(Pvalue, Parray, index);		// find first saved index.

	while (Pvalue != NULL)
	{
	    printf("%12lu %5lu\n", index, *Pvalue);
	    JLN(Pvalue, Parray, index);		// find next saved index.
	}
	printf("Next:  ");  // fflush(stdout); ?
    }
}
