#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <Judy.h>

#define MAXLENSTR 1000000

char    Index[MAXLENSTR];            // string to store.

Pvoid_t   PJHArray = (PWord_t)NULL;  // Judy array.

// By Doug Baskins Apr 2004 - for JudyHS man page -- but too long

int     // Usage:  JudyString file_to_store
main(int argc, char *argv[])
{
    Pvoid_t   PJHArray = (PWord_t)NULL; // Judy array.
    PWord_t   PValue;                   // Judy array element.
    Word_t    Bytes;                    // size of JudySL array.
    Word_t    Len;                      // length of string
    FILE     *fid = NULL;               // stream id
    int       Chr;                      // next char
    long      Lines;                    // number of lines input file
    Word_t    Dups;                     // Count duplicate lines

    if (argc < 2)
    {
        printf("Must supply input file in arg\n");
        exit(2);
    }
    if ((fid = fopen(argv[1], "r")) == NULL)
    {
        printf("Failed to open '%s'\n", argv[1]);
        exit(2);
    }
    printf("Open '%s' and store strings in JudyHS array\n", argv[1]);

    Lines = 0;
    Len = 0;
    Dups = 0;
    while ((Chr = fgetc(fid)) != EOF)
    {
        if (Chr == '\n' && Len)
        {
            Index[Len] = '\0';

//printf("%3lu,%lu: %s\n", Lines, Len, Index);
//printf("%s\n", Index);

            JHSI(PValue, PJHArray, Index, Len); // store string into array
            if  (*PValue != 0) Dups++;
            *PValue += 1;
            Lines++;
            Len = 0;
        }
        else if (Len < MAXLENSTR)
        {
            Index[Len] = Chr;
            Len++;
        }
    }

    fclose(fid);
    fid = NULL;

    printf("'%s' has %lu lines, %lu duplicate lines\n", argv[1], Lines, Dups);

    printf("Re-open '%s' and verify each string is in JudyHS array\n", argv[1]);

    if ((fid = fopen(argv[1], "r")) == NULL)
    {
        printf("Failed to re-open '%s'\n", argv[1]);
        exit(2);
    }

    Lines = 0;
    Len = 0;
    while ((Chr = fgetc(fid)) != EOF)
    {
        if (Len < MAXLENSTR) Index[Len] = Chr;

        if (Chr == '\n' && Len)
        {
            Index[Len] = '\0';

            JHSG(PValue, PJHArray, Index, Len); // store string into array
            if (PValue == NULL)
            {
                printf("'%s'\n", Index);
                printf("JHSG() failed at Line %lu\n", Lines);
                exit(1);
            }
            Len = 0;
            Lines++;
        }
        else Len++;
    }

    printf("Begin JHSFA (JudyHSFreeArray)\n");

    JHSFA(Bytes, PJHArray);                     // free array

    fprintf(stderr, "JHSFA() free'ed %lu bytes of memory\n", Bytes);
    return (0);
}
