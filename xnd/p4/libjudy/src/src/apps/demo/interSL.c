// Copyright (C) 2000 - 2002 Hewlett-Packard Company
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

// @(#) $Revision: 4.5 $ $Source: /judy/src/apps/demo/interSL.c $
//
// INTERACTIVE JUDYSL DEMO PROGRAM
//
// This program is a very simple interactive demonstration of JudySL.  Text
// keywords are entered.  The program uses that text as a key into a JudySL
// array and increments a usage count.
//
// Keys can be single keywords like, "mykey", or multiple words like, "now is
// the time for the quick brown fox to take the bull by the horns".
//
// The program recognizes the following keywords as special:
//
//    CMD_LIST  lists all the currently entered keys and their counts
//    CMD_QUIT  terminates the program
//
// This program demonstrates:
//
//    JudySLIns
//    JudySLFirst
//    JudySLNext
//    how to access a JudySL value
//
// Note:  Using JudySL gives you fast lookups as with hashing but without
// having to define a hash function, and without having to predetermine the
// hash table size.  It also gives you a sorted list at the same time.

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Judy.h"

// Commands recognized by the program:

#define	CMD_LIST "list"
#define	CMD_QUIT "quit"

#define	PLURAL(count) ((count == 1) ? "" : "s")

main()
{
    char     Index [BUFSIZ];		// value from user.
    void  ** PPValue;			// associated with Index.
    void  *  PArray = (Pvoid_t) NULL;	// JudySL array.
    JError_t JError;                    // Judy error structure
    char  *  Pc;			// place in string.

// EMIT INSTRUCTIONS:

    (void) puts	  ("JudySL interactive demonstration:");
    (void) puts	  ("When asked for input, enter some text or:");
    (void) printf ("- \"%s\" to list previously entered text or\n", CMD_LIST);
    (void) printf ("- \"%s\" (or EOF) to quit the program\n\n",	    CMD_QUIT);


// ACCEPT COMMANDS:

    while (1)
    {
	(void) printf ("\nEnter key/list/quit: ");
	(void) fflush (stdout);

	if (fgets (Index, BUFSIZ, stdin) == (char *) NULL)
	    break;

	if ((Pc = strchr (Index, '\n')) != (char *) NULL)
	    *Pc = '\0';			// strip trailing newline.

// QUIT:

	if (! strcmp (Index, CMD_QUIT)) break;


// LIST ALL INPUT IN ALPHABETICAL ORDER:

	if (! strcmp (Index, CMD_LIST))
	{
	     Index[0] = '\0';

	     for (PPValue  = JudySLFirst (PArray, Index, 0);
		  PPValue != (PPvoid_t) NULL;
		  PPValue  = JudySLNext  (PArray, Index, 0))
	     {
		 (void) printf ("  \"%s\" stored %lu time%s\n",
				Index,  *((PWord_t) PPValue),
				PLURAL (*((PWord_t) PPValue)));
	     }

	     continue;
	}


// ALL OTHER VALUES ARE KEYS:
//
// Insert Index into the array.  If Index already exists, JudySLIns() returns a
// pointer to the old value.  If Index doesn't already exist, then a slot is
// created and a pointer to this slot (initialized to zero) is returned.

	if ((PPValue = JudySLIns (& PArray, Index, &JError)) == PPJERR)
	{					// assume out of memory.
	    (void) printf ("Error %d, cannot insert \"%s\" in array.\n",
			   JU_ERRNO(&JError), Index);
	    exit (1);
	}

	++(*((PWord_t) PPValue));		// increment usage count.

	(void) printf ("  \"%s\" stored %ld time%s\n",
		       Index,  *((PWord_t) PPValue),
		       PLURAL (*((PWord_t) PPValue)));

    } // while 1 (continuous loop until user quits)

    exit (0);
    /*NOTREACHED*/

} // main()
