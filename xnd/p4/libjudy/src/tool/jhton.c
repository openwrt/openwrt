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

// @(#) $Revision: 4.22 $ $Source: /cvsroot/judy/tool/jhton.c,v $
//
// JUDY HTML MANUAL ENTRY TO NROFF TRANSLATOR.
//
// USAGE:  <program> filename.htm[l]
// Writes nroff -man output to stdout.
// Suggestion:  Pipe output through rmnl to delete extraneous newlines that
// this program cannot easily avoid.
//
// Compile with -DDEBUG for assertions and other checks.
// If so, run with DUMP set in the env to dump the docnodes tree.
//
// CONCEPT:  This program was written out of necessity for the 11.11 OEUR
// release of Judy.  Ideally our manual entries would be written in an abstract
// XML format with an XSLT-based means to translate them to any other format,
// such as HTML or nroff.  In lieu of that, this program knows how to translate
// a limited subset of HTML, as used in our manual entries, to equivalent
// nroff, as described below preceding EmitNroffHeader().
//
// The translation is still complex enough to merit writing a parser that first
// builds a tree representation of the structured (HTML) document.  I would use
// yacc and lex if I knew them...
//
// This program is written for simplicity, cleanliness, and robustness, and not
// necessarily for speed.
//
// FLEXIBILITY:  It should be possible to teach this program new HTML tags; see
// data structures below.  The program might also be useful for other HTML
// manual entries, so long as they follow the simple conventions used in the
// Judy entries; see the comments before EmitNroffHeader().  You can also
// discover the format by trial and error -- this program issues verbose error
// messages.
//
// CONVENTIONS:
//
// - Global variables start with "g_" except option_*.
// - Global constants start with "gc_".
//
// - Pointer variable names start with one "P" per level of indirection.
//
// - Exception:  (char *) and (char[]) types, that is, strings, do not
//   necessarily start with "P".  A generic char pointer is named "Pch".
//   Variables of type (char **) start with a single P.
//
// - Exception:  the well known name "char ** argv".
//
// - Pointers to first elements of serial linked lists of structures have names
//   ending in "Phead".
//
// - Line lengths are less than 80 columns.  Message parameters to Error()
//   begin on the same line for easy finding.
//
// - Error messages might exceed one line when emitted, but no effort is made
//   to wrap them nicely.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>		// for varargs.
#include <string.h>		// for str*().
#include <errno.h>
#include <ctype.h>		// for isspace(), etc.
#include <time.h>
#include <assert.h>

#define	ISSPACE(Char) isspace((int) (Char))	// for convenience with lint.
#define	ISDIGIT(Char) isdigit((int) (Char))
#define	ISUPPER(Char) isupper((int) (Char))

#define	PUTS(String) (void) fputs(String, stdout)	// for convenience.
#define	PUTC(Char)   (void) putc((int) (Char), stdout)

#ifndef DEBUG
#define NDEBUG			// turn off assertions by default.
#endif

// Shorthand notation to avoid #ifdefs for single-line conditional statement:
//
// Warning:  This cannot be used around compiler directives, such as
// "#include", nor in the case where Code contains a comma other than nested
// within parentheses or quotes.

#ifndef DEBUG
#define	DBGCODE(Code) // null.
#else
#define	DBGCODE(Code) Code
#endif


// ****************************************************************************
// MISCELLANEOUS GLOBAL VALUES:

#define	FUNCTION			// null; easy to find functions.
#define	FALSE	0
#define	TRUE	1
#define	CHNULL	('\0')
#define	PCNULL	((char *) NULL)
typedef int bool_t;			// for clarity with Boolean values.

char * gc_usage[] = {
    "usage: %s filename.htm[l]",
    "",
    "Reads restricted (Judy-specific) HTML from filename.htm[l] and emits",
    "equivalent nroff -man to stdout.",
    PCNULL,
};

char * gc_myname;			// how program was invoked.

#define	OKEXIT	0
#define	NOEXIT	0			// values for Error().
#define	ERREXIT	1
#define	USAGE	2
#define	NOERRNO	0

// Prefix for printf formats:

#define	FILELINE "File \"%s\", line %d: "

// Common error string:

char * FmtErrLineEnds = FILELINE "Input line ends within an HTML tag; for this "
			"translator, all tags must be on a single input line";

// Macros for skipping whitespace or non-whitespace; in the latter case,
// stopping at end of line or end of tag:

#define	SKIPSPACE(Pch)	   { while (ISSPACE(*(Pch))) ++(Pch); }

#define	SKIPNONSPACE(Pch)  { while ((! ISSPACE(*(Pch))) \
				 && (*(Pch) != CHNULL)  \
				 && (*(Pch) != '>')) ++(Pch); }

// Highest line number + 1, and last input line number that caused output:

int g_linenumlim;
int g_prevlinenum = 0;

// <PRE> block equivalents in nroff need some special handling for bold font
// and for continuing a tagged paragraph; these are bit flags:

#define	INPRE_BLOCK  0x1	// came from <PRE>.
#define	INPRE_BOLD   0x2	// came from <B><PRE>.
#define	INPRE_INDENT 0x4	// under <DL> below top level.


// ****************************************************************************
// DOCUMENT NODE TYPES:
//
// If an HTML tag is not in this list, it's unrecognized and causes a fatal
// error.  Otherwise the tag type (dn_type) is one of DN_TYPE_*, which are
// defined so the code can use them, but they MUST match the order of
// initialization of g_dntype[].
//
// Note:  The default node type is DN_TYPE_TEXT, that is, text outside of any
// tag.

enum {
	DN_TYPE_TEXT = 0,
	DN_TYPE_HTML,
	DN_TYPE_HEAD,
	DN_TYPE_TITLE,
	DN_TYPE_BODY,
	DN_TYPE_COMM,
	DN_TYPE_TABLE,
	DN_TYPE_TR,
	DN_TYPE_TD,
	DN_TYPE_DL,
	DN_TYPE_DT,
	DN_TYPE_DD,
	DN_TYPE_A,
	DN_TYPE_B,
	DN_TYPE_I,
	DN_TYPE_PRE,
	DN_TYPE_P,
	DN_TYPE_BR
};

// Regarding dnt_nest:  If an HTML tag type is marked as nesting, that means
// it is required not to be a singleton in this context; it must have a closing
// tag, and when the tree is built, the intervening text is nested as a child.
// Otherwise, intervening text is a sibling; a closing tag is allowed (whether
// or not this makes sense), but is not required; however, if present, it must
// match.

struct docnode_type {
	char *	dnt_tag;		// HTML tag.
	bool_t	dnt_savetag;		// flag: save HTML tag.
	bool_t	dnt_nest;		// flag: see comments above.
	int	dnt_type;		// corresponding number.
} g_dntype[] = {

// Note:  HTML is case-insensitive, but for expediency this program is
// case-sensitive.  Tags must be as shown below.

    { "",	FALSE, FALSE, DN_TYPE_TEXT,  },	// special, see above.

    { "HTML",	FALSE, TRUE,  DN_TYPE_HTML,  },
    { "HEAD",	FALSE, TRUE,  DN_TYPE_HEAD,  },
    { "TITLE",	FALSE, TRUE,  DN_TYPE_TITLE, },
    { "BODY",	FALSE, TRUE,  DN_TYPE_BODY,  },

    { "!--",	TRUE,  FALSE, DN_TYPE_COMM,  },	// comments are singleton tags.

    { "TABLE",	FALSE, TRUE,  DN_TYPE_TABLE, },	// limited understanding!
    { "TR",	FALSE, TRUE,  DN_TYPE_TR,    },
    { "TD",	TRUE,  TRUE,  DN_TYPE_TD,    },

    { "DL",	FALSE, TRUE,  DN_TYPE_DL,    },
    { "DT",	FALSE, TRUE,  DN_TYPE_DT,    },
    { "DD",	FALSE, FALSE, DN_TYPE_DD,    },	// </DD> not req in our manuals.

    { "A",	TRUE,  TRUE,  DN_TYPE_A,     },	// either "name" or "href" type.
    { "B",	FALSE, TRUE,  DN_TYPE_B,     },
    { "I",	FALSE, TRUE,  DN_TYPE_I,     },

    { "PRE",	FALSE, TRUE,  DN_TYPE_PRE,   },

    { "P",	FALSE, FALSE, DN_TYPE_P,     },	// </P>  not req in our manuals.
    { "BR",	FALSE, FALSE, DN_TYPE_BR,    },	// </BR> not req in our manuals.

    { PCNULL,	FALSE, FALSE, 0,	     },	// end of list.
};

// Convenience macros:

#define	TAG(DN_Type)	 (g_dntype[DN_Type].dnt_tag)
#define	SAVETAG(DN_Type) (g_dntype[DN_Type].dnt_savetag)
#define	NEST(DN_Type)	 (g_dntype[DN_Type].dnt_nest)


// ****************************************************************************
// DOCUMENT NODE DATA STRUCTURES:
//
// Document nodes are saved in a doubly-linked tree of docnodes.  Each docnode
// points sideways to a doubly-linked list of sibling docnodes for
// previous/successive unnested document objects, plus points to its parent and
// to the first of a sideways doubly-linked child list of nested objects.  All
// data lives in malloc'd memory.
//
// The dn_text field is null for a tag node unless the tag text is worth
// saving.  The field is non-null for non-tag (document) text.

typedef	struct docnode * Pdn_t;

struct docnode {
	int	dn_type;	// node type, index in g_dntype[].
	int	dn_linenum;	// where introduced, for reconstructing.
	bool_t	dn_closed;	// flag: closing tag was seen.
	bool_t	dn_noemit;	// flag: skip on output, for marking ahead.
	bool_t	dn_bold;	// flag: for <PRE>, whole section is bold.

	char *	dn_text;	// node text; see above.

	Pdn_t	dn_Pprev;	// previous node in sibling list.
	Pdn_t	dn_Pnext;	// next     node in sibling list.
	Pdn_t	dn_Pparent;	// up-link to parent node, if any.
	Pdn_t	dn_Pchild;	// down-link to first node in child subtree.
};

#define	PDNNULL	((Pdn_t) NULL)

Pdn_t g_Pdnhead = PDNNULL;	// head of docnode tree.


// ****************************************************************************
// FUNCTION SIGNATURES (forward declarations):

int	main(int argc, char ** argv);

void	ReadInputFile( char * Filename, FILE * PFile);
void	CheckNesting(Pdn_t Pdn);
void	EmitNroffHeader(char * Filename, char ** PPageName);
void	EmitNroffBody(Pdn_t Pdn, int DLLevel, int InPRE, char * PageName);

void	ExtractHeader(  Pdn_t Pdn, char ** PFileRev,
		        char ** PPageName, char ** PPageSection,
			char *  PLcLetter, char ** PRevision);
char *	ExtractText(    Pdn_t Pdn);
void	ExtractPageInfo(Pdn_t Pdn, char * Pch,
			char ** PPageName, char ** PPageSection,
			char *  PLcLetter);

int	TagType(char * Tag, bool_t * isclosing, char * Filename, int Linenum);
Pdn_t	AppDocNode( Pdn_t Pdn,	     int linenum);
Pdn_t	NewDocNode( Pdn_t dn_Pparent, int linenum);
char *	SaveDocNode(Pdn_t Pdn, int DN_Type, char * Pch,
		    char * Filename, int Linenum);
bool_t	ParentPre(Pdn_t Pdn, bool_t BoldOnly);

void	MarkNoEmit(   Pdn_t Pdn, bool_t Font);
void	EmitText(     char * Pch, int InPRE, int Linenum);
void	EmitTextPRE(  char * Pch, int InPRE);
void	EmitTextBS(   char * Pch);
bool_t	NoWhiteSpace( char * Pch);
int	CountNewlines(char * Pch);

char *	StrSave(  char * String);
char *	StrSaveN( char * String, ...);
void *	Malloc(	  size_t Size);

void	Usage(void);
void    Error(int Exitvalue, int MyErrno, char * Message, ...);

DBGCODE(void DumpTree(Pdn_t Pdn, int Depth, bool_t Separator);)


// ****************************************************************************
// M A I N

FUNCTION int main(
	int	argc,
	char **	argv)
{
	char *	filename;	// input file.
	FILE *	Pfile;		// open input file.
	char *	pagename;	// such as "Judy1".

	gc_myname = argv[0];
	if (argc != 2) Usage();
	filename = argv[1];

#ifdef DEBUG
// Assert that each dnt_type matches its index in the table, since the code
// depends on this:

	{
	    int dn_type;

	    for (dn_type = 0; TAG(dn_type) != PCNULL; ++dn_type)
		assert(g_dntype[dn_type].dnt_type == dn_type);
	}
#endif


// READ FROM LIST OF FILES OR STDIN; BUILD TREE:

	if ((Pfile = fopen(filename, "r")) == (FILE *) NULL)
	{
	    Error(ERREXIT, errno, "Cannot open file \"%s\" to read it",
		  filename);
	}

	ReadInputFile(filename, Pfile);

	if (ferror(Pfile))
	    Error(ERREXIT, errno, "Cannot read from file \"%s\"", filename);

	DBGCODE(DumpTree(g_Pdnhead, /* Depth = */ 0, /* Separator = */ TRUE);)

	if (g_Pdnhead == PDNNULL)
	{
	    Error(ERREXIT, NOERRNO, "No HTML tags found in file \"%s\"",
		  filename);
	}

	CheckNesting(g_Pdnhead);


// EMIT NROFF VERSION OF TEXT:

	EmitNroffHeader(filename, &pagename);
	EmitNroffBody(g_Pdnhead, /* DLLevel = */ 0, /* InPRE = */ 0, pagename);
	PUTC('\n');			// ensure last line is terminated.

	return(0);

} // main()


// ****************************************************************************
// R E A D   I N P U T   F I L E
//
// Given a filename and stream pointer for reading, read and parse
// Judy-specific HTML and build a structure representing the document, under
// g_Pdnhead.  Set g_linenumlim.
//
// Note:  Ideally this would be a shorter function with helper subroutines, but
// I wrote this fast.  :-)
//
// Note:  This used be called just ReadFile(), but on win_ipf, at least the
// cross-compile environment, this resulted in a duplicate symbol error, as if
// ReadFile() is in a library somewhere.

FUNCTION void ReadInputFile(
	char *	Filename,
	FILE *	PFile)
{
	int	linenum = 0;		// input line number.
	char	line[BUFSIZ];		// read from file.
	char *	Pch;			// place in line.
	char *	Pch2;			// place in line.
	char *	Pchp;			// prior to skipped whitespace.
	char *	tagname;		// for error reporting.
	char	chold;			// old char.
	int	dn_type;		// docnode type.
	bool_t	isclosing;		// is a closing tag.
	Pdn_t	Pdn = PDNNULL;		// current docnode.
	Pdn_t	Pdnprev;		// previous in sibling list.


// READ LINE, TRIM AT ANY NEWLINE, AND SKIP LEADING WHITESPACE:

	while (fgets(line, BUFSIZ, PFile) != PCNULL)
	{
	    line[strcspn(line, "\n")] = CHNULL;
	    ++linenum;

	    Pch = Pchp = line;
	    SKIPSPACE(Pch);		// skip any leading whitespace.

	    if (! ParentPre(Pdn, /* BoldOnly = */ FALSE))
		Pchp = Pch;		// skip for storing, too.


// HANDLE EMPTY LINE:
//
// If within an "already open" DN_TYPE_TEXT, append a newline to the existing
// text in case it turns out to be significant later (mainly in a <PRE>
// section).

	    if (*Pch == CHNULL)
	    {
		if ((Pdn->dn_type) == DN_TYPE_TEXT)
		{
		    assert((Pdn->dn_text) != PCNULL);
		    (Pdn->dn_text) = StrSaveN(Pdn->dn_text, "\n", PCNULL);
		}
		continue;
	    }


// HANDLE DATA LINE:
//
// Look for a "<" that starts an HTML tag.

	    while (*Pch != CHNULL)		// more on line.
	    {
		Pch2 = strchr(Pch, '<');	// look for next tag start.


// SAVE DOCUMENT TEXT:
//
// Save any text preceding a tag (or through end of line) in a document text
// (DN_TYPE_TEXT) docnode; either the existing node if there is one, else a new
// node as a sibling or child of the previous, as appropriate.
//
// This builds one docnode for any inter-tag text, whether it's a portion of
// one line, or many lines long.  In some ways that's simplest and most
// efficient, and in other ways it's weird.
//
// Note:  Use Pchp here, not Pch, so as not to ignore leading whitespace on
// <PRE> lines.

		if (! ((Pch2 == PCNULL) ? strlen(Pchp) : Pch2 - Pchp))
		{
		    assert((Pch == Pch2) || (Pch2 == PCNULL));
		}
		else
		{
		    if (g_Pdnhead == PDNNULL)	// no current tree.
		    {
			Error(ERREXIT, NOERRNO, FILELINE "For this "
			      "translator, the HTML file must start with an "
			      "HTML tag in \"<>\", and no other text",
			      Filename, linenum);
		    }

// Current docnode is not DN_TYPE_TEXT, so append a new docnode child or
// sibling, as appropriate, to the tree:

		    assert(Pdn != PDNNULL);	// should already be assigned.

		    if ((Pdn->dn_type) != DN_TYPE_TEXT)
		    {
			Pdn = AppDocNode(Pdn, linenum);
			(Pdn->dn_type) = DN_TYPE_TEXT;
		    }

// Save current text in current docnode, appending to any existing text:
//
// Note:  To avoid messing up alignments in <PRE> text, use Pchp, not Pch,
// which can skip whitespace.

		    if (Pch2 != PCNULL) *Pch2 = CHNULL;	 // terminate briefly.

		    if ((Pdn->dn_text) == PCNULL)	// no existing text.
			(Pdn->dn_text) = StrSave(Pchp);
		    else
			(Pdn->dn_text) = StrSaveN(Pdn->dn_text, Pchp, PCNULL);

		    assert((Pdn->dn_text) != PCNULL);

// If there's still a tag on the line, unterminate Pch2; otherwise append a
// newline to the saved text in case more document text follows:

		    if (Pch2 != PCNULL)
			*Pch2 = '<';			// unterminate.
		    else
			(Pdn->dn_text) = StrSaveN(Pdn->dn_text, "\n", PCNULL);

		} // if text preceding tag.


// ANALYZE HTML TAG (if any):

		if (Pch2 == PCNULL) break;	// no tag on line; line is done.

		Pch = Pch2 + 1;		// skip "<".
		SKIPSPACE(Pch);		// skip any whitespace.

		if (*Pch == CHNULL)
		    Error(ERREXIT, NOERRNO, FmtErrLineEnds, Filename, linenum);

		Pch2 = Pch;
		SKIPNONSPACE(Pch2);  // find whitespace or end of line or tag.

		if (*Pch2 == CHNULL)
		    Error(ERREXIT, NOERRNO, FmtErrLineEnds, Filename, linenum);

		chold = *Pch2;
		*Pch2 = CHNULL;		// temporarily terminate.
		dn_type = TagType(Pch, &isclosing, Filename, linenum);
		*Pch2 = chold;


// HANDLE CLOSING TAG:
//
// First ensure the tag checks out OK.

		if (isclosing)
		{
		    if (*Pch2 != '>')
		    {
			Error(ERREXIT, NOERRNO, FILELINE "Closing HTML tag "
			      "\"%s\" must be followed immediately by \">\"; "
			      "this translator does not even allow whitespace",
			      Filename, linenum, Pch);
		    }

		    if (g_Pdnhead == PDNNULL)
		    {
			Error(ERREXIT, NOERRNO, FILELINE "Closing HTML tag "
			      "\"%s\" found before any opening tag in the file",
			      Filename, linenum, Pch);
		    }

		    tagname = Pch;		// for error reporting.
		    Pch = Pchp = Pch2 + 1;	// skip ">" in line.

// Check if the closing tag is an optional closing for the last unclosed,
// non-DN_TYPE_TEXT docnode in the current sibling list, if any:

		    for (Pdnprev  = Pdn;
			 Pdnprev != PDNNULL;
			 Pdnprev  = Pdnprev->dn_Pprev)
		    {
			if (((Pdnprev->dn_type) == DN_TYPE_TEXT)
			 || (Pdnprev->dn_closed))
			{
			    continue;		// skip text or closed tag.
			}

			if ((Pdnprev->dn_type) == dn_type)
			{
			    (Pdnprev->dn_closed) = TRUE;  // optional closing.
			    break;
			}
		    }

		    if (Pdnprev != PDNNULL) continue;	// matched closing.

// Otherwise check that the closing tag is the (required) closing tag for the
// (required) parent node (which must not have been closed yet):

		    if ((Pdn->dn_Pparent) == PDNNULL)
		    {
			Error(ERREXIT, NOERRNO, FILELINE "Closing HTML tag "
			      "\"%s\" does not match an opening tag",
			      Filename, linenum, tagname);
		    }

		    assert(! (Pdn->dn_Pparent->dn_closed));

		    if ((Pdn->dn_Pparent->dn_type) != dn_type)
		    {
			Error(ERREXIT, NOERRNO, FILELINE "Parent HTML tag "
			      "\"%s\" found on line %d requires a closing tag, "
			      "but \"%s\" does not match it; check for out-of-"
			      "order HTML tags", Filename, linenum,
			      TAG(Pdn->dn_Pparent->dn_type),
			      Pdn->dn_Pparent->dn_linenum, tagname);
		    }

// Go uplevel in the tree to the parent node:

		    Pdn = Pdn->dn_Pparent;
		    (Pdn->dn_closed) = TRUE;
		    continue;

		} // closing tag.


// NEW HTML TAG:  ADD SIBLING OR CHILD NODE TO TREE:
//
// Save appropriate information about the tag and move beyond its closing point
// in the input line.

		Pdn = AppDocNode(Pdn, linenum);
		Pch = Pch2;
		assert( Pch != PCNULL);
		assert(*Pch != CHNULL);
		Pch = Pchp = SaveDocNode(Pdn, dn_type, Pch, Filename, linenum);

	    } // while more on input line.
	} // while more in input file.

	g_linenumlim = linenum + 1;

} // ReadInputFile()


// ****************************************************************************
// C H E C K   N E S T I N G
//
// Given a docnode, recursively check that all nested HTML tags were closed.
// If not, error out.

FUNCTION void CheckNesting(
	Pdn_t Pdn)	// current docnode.
{
	if (NEST(Pdn->dn_type) && (! (Pdn->dn_closed)))
	{
	    Error(ERREXIT, NOERRNO, "No closing tag found for HTML tag \"%s\" "
		  "from input line %d", TAG(Pdn->dn_type), Pdn->dn_linenum);
	}

	if ((Pdn->dn_Pchild) != PDNNULL) CheckNesting(Pdn->dn_Pchild);
	if ((Pdn->dn_Pnext)  != PDNNULL) CheckNesting(Pdn->dn_Pnext);

} // CheckNesting()




// ****************************************************************************
// E M I T   N R O F F   H E A D E R
//
// Given the input filename, a pointer to a page name string to return, and the
// docnode tree under g_Pdnhead, extract header info and emit nroff header
// lines.

FUNCTION void EmitNroffHeader(
	char *	Filename,		// input file.
	char **	PPageName)		// such as "Judy1", to return.
{
	char *	filerev = PCNULL;	// from first comment in input.
	char *	pagesection;		// such as "3X".
	char	lcletter;		// manual tab section, such as "j".
	char *	revision;		// from centered table datum.
	time_t	currtime;		// for ctime().

// Extract "weird" header values:
//
// These must be found in the docnodes tree and prepared for emitting nroff.

	ExtractHeader(g_Pdnhead, &filerev,
		      PPageName, &pagesection, &lcletter, &revision);

	if (filerev == PCNULL)
	{
	    Error(ERREXIT, NOERRNO, "HTML file lacks comment lines; it must "
		  "contain at least one comment line, and the first one must "
		  "contain revision information");
	}

// Emit file header; note, ctime() output already contains a newline:

	(void) time(&currtime);
	(void) printf(".\\\" Auto-translated to nroff -man from %s by %s at %s",
		      Filename, gc_myname, ctime(&currtime));

	(void) printf(".\\\" %s\n",  filerev);
	(void) printf(".TA %c\n",    lcletter);
	(void) printf(".TH %s %s\n", *PPageName, pagesection);
	(void) puts(  ".ds )H Hewlett-Packard Company");
	(void) printf(".ds ]W %s\n", revision);

} // EmitNroffHeader()


// ****************************************************************************
// E M I T   N R O F F   B O D Y
//
// Given a current node in the docnodes tree, the current <DL> level, a flag
// whether below a <PRE> node, the manual entry page name, and in
// g_prevlinenum, the previous input line number that resulted in output,
// recursively emit nroff body text.  Translate the HTML docnodes as described
// in the comments prior to EmitNroffHeader(), and also translate certain HTML
// escaped chars back to literal form.  Hope the results are legal nroff
// without spurious unintended nroff commands embedded.
//
// Note:  This function recurses two ways; first, to the child subtree, and
// second, to the next sibling at the current level.

FUNCTION void EmitNroffBody(
	Pdn_t	Pdn,		// current top of subtree.
	int	DLLevel,	// <DL> level, top = 0.
	int	InPRE,		// bit flags for <PRE> handling.
	char *	PageName)	// such as "Judy1".
{
	int	DLcount = 0;	   // set to 1 if hit <DL> here.
	char *	suffix  = PCNULL;  // to print after children, before siblings.

// When about to emit text, if the previous output came from a lower input line
// number, start with a newline; otherwise do not, and let the text
// concatenate:
//
// Use CHECKPREV except when the text to be emitted is forced to a new line.

#ifdef CPPRINT		// for special debugging:
#define	CHECKPREVPRINT printf("\ncp %d %d\n", g_prevlinenum, Pdn->dn_linenum)
#else
#define	CHECKPREVPRINT // null
#endif

#define	CHECKPREV	\
	CHECKPREVPRINT;	\
	{ if (g_prevlinenum && (g_prevlinenum < (Pdn->dn_linenum))) PUTC('\n');}

// To support CHECKPREV, call SETPREV() after emitting text that might need a
// line break to a new line, or SETPREVNONL to ensure NO newline, that is, the
// next text concatenates on the same line:
//
// Note:  For a correct line number, SETPREV() must account for any newlines in
// the text just emitted.

#define	SETPREV(Text)	g_prevlinenum = (Pdn->dn_linenum) + CountNewlines(Text)
#define	SETPREVNONL	g_prevlinenum = g_linenumlim	// no newline.

// Check if under a lower-level <DL>, for continuing an indented paragraph:

#define	UNDER_DL ((DLLevel > 1)				\
	       && ((Pdn->dn_Pparent) != PDNNULL)	\
	       && ((Pdn->dn_Pparent->dn_type) == DN_TYPE_DL))


// SWITCH ON DOCNODE TYPE:

	if (Pdn->dn_noemit)	// upstream node said to skip this one.
	    goto NextNode;

	switch (Pdn->dn_type)
	{


// DOCUMENT TEXT:
//
// Just emit it with HTML escaped chars modified, with backslashes doubled,
// with no trailing newline, and if not within <PRE> text, with any leading
// whitespace deleted, so that, for example, something like "\fI text\fP" does
// not result.

	case DN_TYPE_TEXT:

	    assert((Pdn->dn_text) != PCNULL);
	    CHECKPREV;
	    EmitText(Pdn->dn_text, InPRE, Pdn->dn_linenum);
	    SETPREV(Pdn->dn_text);
	    break;


// IGNORE THESE TYPES:
//
// See EmitNroffHeader() for nroff equivalents already emitted in some cases.
// In some cases, mark all child nodes no-emit to ignore them.

	case DN_TYPE_HTML:
	case DN_TYPE_HEAD:
	case DN_TYPE_BODY:
	case DN_TYPE_COMM:	break;

	case DN_TYPE_TITLE:
	case DN_TYPE_TABLE:
	case DN_TYPE_TR:
	case DN_TYPE_TD:

	    MarkNoEmit(Pdn->dn_Pchild, /* Font = */ FALSE);
	    break;


// DESCRIPTIVE LIST:
//
// At the top level these represent manual entry sections, and any bold markers
// around the text are ignored.  Below the top level these translate to tagged
// paragraphs.  Here, just note the increment and continue the walk.

	case DN_TYPE_DL:

	    DLcount = 1;
	    break;


// DESCRIPTIVE LIST TAG:

	case DN_TYPE_DT:

	    assert(NEST(DN_TYPE_DT));		// tag text must be child.

	    if ((Pdn->dn_Pchild) == PDNNULL)	// no child exists.
	    {
		Error(ERREXIT, NOERRNO, "HTML tag \"%s\" found at input line "
		      "%d lacks text, which is required by this translator",
		      TAG(DN_TYPE_DT), Pdn->dn_linenum);
	    }

// Further handling depends on DLLevel as explained above:

	    if (DLLevel <= 1)		// major manual section.
	    {
		PUTS("\n.SH ");

		if ((Pdn->dn_Pchild->dn_type) == DN_TYPE_B)
		    (Pdn->dn_Pchild->dn_noemit) = TRUE;	 // skip <B>...</B>.
	    }

// If a <DT> immediately follows a previous <DT>, use .PD 0 for the successive
// .TP to join lines:

	    else
	    {
		if (((Pdn->dn_Pprev) != PDNNULL)
		 && ((Pdn->dn_Pprev->dn_type) == DN_TYPE_DT))
		{
		    PUTS("\n.PD 0\n");
		    suffix = "\n.PD\n";
		}

		PUTS("\n.TP 15\n.C ");
	    }

	    SETPREVNONL;
	    break;


// DESCRIPTIVE LIST DATUM:
//
// Just proceed to dump the embedded text.

	case DN_TYPE_DD: break;


// ANCHOR:
//
// Ignore inbound ("name") anchors and process outbound ("href") anchor labels
// into appropriately highlighted text.

	case DN_TYPE_A:
	{
	    size_t len;		// of substring.
	    Pdn_t  Pdn2;	// child node.
	    char * Pch;		// place in text.

	    assert((Pdn->dn_text) != PCNULL);

	    if (strstr(Pdn->dn_text, "name=") != PCNULL) break;

	    if (strstr(Pdn->dn_text, "href=") == PCNULL)
	    {
		Error(NOEXIT, NOERRNO, "Unrecognized HTML anchor type \"%s\" "
		      "at input line %d ignored; only \"name=\" and \"href=\" "
		      "are allowed by this translator",
		      Pdn->dn_text, Pdn->dn_linenum);
		break;
	    }

// Check for nested text (anchor label):
//
// TBD:  The error message lies a little.  If the text is something like,
// "foo<B>bar</B>", it passes this test; and later, all font tags in the anchor
// label are marked no-emit; and any other embedded tags, who knows what
// happens?

	    if (((Pdn2 = Pdn->dn_Pchild)->dn_type) != DN_TYPE_TEXT)
	    {
		Error(ERREXIT, NOERRNO, "HTML \"href\" anchor at input line "
		      "%d lacks a directly nested anchor label, with no "
		      "further nested tags; this translator cannot support "
		      "nested tags in anchor labels", Pdn->dn_linenum);
	    }
	    assert((Pdn2->dn_text) != PCNULL);

// If the anchor is within a <B><PRE>, do nothing special with fonts, as
// explained earlier:

	    if (ParentPre(Pdn, /* BoldOnly = */ TRUE)) break;

// Since anchor label text font will be forced in a moment, ignore any nested
// font directives so they don't mess up nroff:

	    MarkNoEmit(Pdn->dn_Pchild, /* Font = */ TRUE);

// See if anchor label appears to be a reference to the current page, to some
// other page, or else just make it italicized text:
//
// TBD:  This is pretty shaky, hope it's close enough.

	    len = strlen(PageName);

	    if (strncmp(Pdn2->dn_text, PageName, len) == 0)  // self-reference.
	    {
		CHECKPREV;
		PUTS("\\fB");			// bold font.
		SETPREVNONL;
		suffix = "\\fP";		// revert to previous font.
		break;
	    }

// Contains '(' and no whitespace => appears to reference some other page:
//
// Emit revised, tagged anchor label text immediately.

	    if (((Pch = strchr(Pdn2->dn_text, '(')) != PCNULL)
	     && NoWhiteSpace(Pdn2->dn_text))
	    {
		CHECKPREV;
		PUTS("\\fI");			// italic font.
		*Pch = CHNULL;			// terminate briefly.
		PUTS(Pdn2->dn_text);
		*Pch = '(';
		PUTS("\\fP");			// revert to previous font.
		PUTS(Pch);
		SETPREV(Pdn2->dn_text);

		(Pdn2->dn_noemit) = TRUE;	// skip later.
		break;
	    }

// Just make the anchor label italicized text:

	    CHECKPREV;
	    PUTS("\\fI");			// italic font.
	    SETPREVNONL;
	    suffix = "\\fP";			// revert to previous font.
	    break;

	} // case DN_TYPE_A


// BOLD TEXT:
//
// If the first child is <PRE>, use a "hard" font change; otherwise an in-line
// change.
//
// Note:  For <DT><B>, this node is already marked dn_noemit and not seen here.
//
// Note:  For <B><PRE>, nroff seems to reset font upon .PP, so mark the bold
// for later emission.

	case DN_TYPE_B:

	    if (((Pdn->dn_Pchild) != PDNNULL)
	     && ((Pdn->dn_Pchild->dn_type) == DN_TYPE_PRE))
	    {
		(Pdn->dn_Pchild->dn_bold) = TRUE;	// see above.
		break;
	    }

	    CHECKPREV;
	    PUTS("\\fB");			// bold font.
	    SETPREVNONL;
	    suffix = "\\fP";			// revert to previous font.
	    break;


// ITALIC TEXT:

	case DN_TYPE_I:

	    CHECKPREV;
	    PUTS("\\fI");			// italic font.
	    SETPREVNONL;
	    suffix = "\\fP";			// revert to previous font.
	    break;


// PREFORMATTED TEXT:
//
// Emit prefix/suffix directives based on example in strchr(3C).

	case DN_TYPE_PRE:

	    PUTS(UNDER_DL ? "\n.IP\n.nf\n.ps +1\n" : "\n.PP\n.nf\n.ps +1\n");
	    if (Pdn->dn_bold) PUTS(".ft B\n");	// deferred bold.
	    SETPREVNONL;
	    suffix = ((Pdn->dn_bold) ? "\n.ft P\n.ps\n.fi\n" : "\n.ps\n.fi\n");

	    // set for all children:
	    InPRE = INPRE_BLOCK
		  | ((Pdn->dn_bold) ? INPRE_BOLD   : 0)
		  | (UNDER_DL ?	      INPRE_INDENT : 0);
	    break;


// PARAGRAPH BREAK:
//
// If the parent is a <DL> below the top level, use .IP to continue a .TP
// (tagged paragraph); otherwise emit a standard .PP.

	case DN_TYPE_P:

	    PUTS(UNDER_DL ? "\n.IP\n" : "\n.PP\n");
	    SETPREVNONL;
	    break;


// LINE BREAK:

	case DN_TYPE_BR: PUTS("\n.br\n"); SETPREVNONL; break;


// UNRECOGNIZED DOCNODE TYPE:

	default:
	    Error(ERREXIT, NOERRNO, "Internal error: Unexpected docnode type "
		  "%d in docnodes tree", Pdn->dn_type);

	} // end switch on dn_type


// VISIT CHILD AND SIBLING DOCNODES:
//
// If this was a <DL> here, pass an incremented value to child nodes, but not
// to sibling nodes.

NextNode:
	if ((Pdn->dn_Pchild) != PDNNULL)
	    EmitNroffBody(Pdn->dn_Pchild, DLLevel + DLcount, InPRE, PageName);

	if (suffix != PCNULL) PUTS(suffix);

	if ((Pdn->dn_Pnext) != PDNNULL)
	    EmitNroffBody(Pdn->dn_Pnext, DLLevel, InPRE, PageName);

} // EmitNroffBody()


// ****************************************************************************
// E X T R A C T   H E A D E R
//
// Given a current docnode and pointers to values to return, walk the entire
// docnode tree once, recursively, in-order (parent then child then sibling) to
// extract nroff header information.  Find the first comment line, insist it
// contain "@\(#)", and put this in *PFileRev.  Also find exactly one
// DN_TYPE_TABLE, containing exactly one DN_TYPE_TR, containing one DN_TYPE_TD
// containing "align=\"left\"" and one DN_TYPE_TD containing
// "align=\"center\"", and extract from these the nroff .TH pagename,
// pagesection, and lcletter, and nroff ]W variable revision string,
// respectively.  Error out if anything goes wrong.
//
// Note:  Some of the returned strings are in separate malloc'd memory and
// others are not; treat them as read-only.

FUNCTION void ExtractHeader(
	Pdn_t	Pdn,		// current docnode.
	char **	PFileRev,	// from first comment in input.
	char **	PPageName,	// such as "Judy1".
	char **	PPageSection,	// such as "3X".
	char *	PLcLetter,	// manual tab section, such as "j".
	char **	PRevision)	// from centered table datum.
{
static	bool_t	found_filerev  = FALSE;
static	bool_t	found_table    = FALSE;
static	bool_t	found_tr       = FALSE;
static	bool_t	found_tdleft   = FALSE;
static	bool_t	found_tdcenter = FALSE;

	char *	text;		// from text node.

// Note:  The following are used for both 0 and >= 2 instances, so they don't
// include a line number because there is none for the 0 case:

#define	ERR_TABLE	"This translator expects exactly one HTML table " \
			"(\"TABLE\" tag) in the input file"
#define	ERR_TR		"This translator expects exactly one HTML table row " \
			"(\"TR\" tag) in the input file"
#define	ERR_TDLEFT	"This translator expects exactly one HTML table row " \
			"datum (\"TD\" tag) in the input containing " \
			"'align=\"left\"'"
#define	ERR_TDCENTER	"This translator expects exactly one HTML table row " \
			"datum (\"TD\" tag) in the input containing " \
			"'align=\"center\"'"


// CHECK CURRENT DOCNODE TYPE:

	switch (Pdn->dn_type)
	{
	case DN_TYPE_COMM:

	    if (found_filerev) break;	// already done.
	    found_filerev = TRUE;

// Hide the whatstring markers here from what(1) itself:

	    if (strstr(Pdn->dn_text, "@" "(#)") == PCNULL)
	    {
		Error(ERREXIT, NOERRNO, "First HTML comment line in input, "
		      "found at line %d, must contain a whatstring, marked by "
		      "\"@" "(#)\"", Pdn->dn_linenum);
	    }

	    *PFileRev = Pdn->dn_text;
	    break;

	case DN_TYPE_TABLE:

	    if (found_table) Error(ERREXIT, NOERRNO, ERR_TABLE);
	    found_table = TRUE;

	    if (((Pdn->dn_Pchild) == PDNNULL)
	     || ((Pdn->dn_Pchild->dn_type) != DN_TYPE_TR))
	    {
		Error(ERREXIT, NOERRNO, "The HTML \"%s\" tag at line %d must "
		      "be followed by a \"%s\" tag, but it is not",
		      TAG(DN_TYPE_TABLE), Pdn->dn_linenum, TAG(DN_TYPE_TR));
	    }
	    break;

	case DN_TYPE_TR:

	    if (found_tr) Error(ERREXIT, NOERRNO, ERR_TR);
	    found_tr = TRUE;

	    if (((Pdn->dn_Pchild) == PDNNULL)
	     || ((Pdn->dn_Pchild->dn_type) != DN_TYPE_TD))
	    {
		Error(ERREXIT, NOERRNO, "The HTML \"%s\" tag at line %d must "
		      "be followed by a \"%s\" tag, but it is not",
		      TAG(DN_TYPE_TR), Pdn->dn_linenum, TAG(DN_TYPE_TD));
	    }
	    break;

	case DN_TYPE_TD:

	    if (strstr(Pdn->dn_text, "align=\"left\"") != PCNULL)
	    {
		if (found_tdleft) Error(ERREXIT, NOERRNO, ERR_TDLEFT);
		found_tdleft = TRUE;

		text = StrSave(ExtractText(Pdn));
		ExtractPageInfo(Pdn, text, PPageName, PPageSection, PLcLetter);
	    }
	    else if (strstr(Pdn->dn_text, "align=\"center\"") != PCNULL)
	    {
		if (found_tdcenter) Error(ERREXIT, NOERRNO, ERR_TDCENTER);
		found_tdcenter = TRUE;

		*PRevision = ExtractText(Pdn);
	    }
	    // else ignore line.

	} // switch on dn_type


// VISIT CHILD AND SIBLING DOCNODES:
//
// Note:  Do this even though it seems redundant, to ensure no duplicates.

	if ((Pdn->dn_Pchild) != PDNNULL)
	{
	    ExtractHeader(Pdn->dn_Pchild, PFileRev,
			  PPageName, PPageSection, PLcLetter, PRevision);
	}

	if ((Pdn->dn_Pnext) != PDNNULL)
	{
	    ExtractHeader(Pdn->dn_Pnext, PFileRev,
			  PPageName, PPageSection, PLcLetter, PRevision);
	}


// AT TOP OF TREE, CHECK FOR SUCCESS:
//
// Note:  If you read the fine print, it's clear the ERR_TR here is impossible
// to hit.

	if (Pdn != g_Pdnhead) return;

	if (! found_table)	Error(ERREXIT, NOERRNO, ERR_TABLE);
	if (! found_tr)		Error(ERREXIT, NOERRNO, ERR_TR);
	if (! found_tdleft)	Error(ERREXIT, NOERRNO, ERR_TDLEFT);
	if (! found_tdcenter)	Error(ERREXIT, NOERRNO, ERR_TDCENTER);

} // ExtractHeader()


// ****************************************************************************
// E X T R A C T   T E X T
//
// Given a non-null docnode, return the non-null dn_text field from its first
// child (directly, not a copy).  Error out if anything goes wrong.

FUNCTION char * ExtractText(
	Pdn_t Pdn)	// parent node.
{
	assert(Pdn != PDNNULL);

#define	ERR_NULLTEXT "Node for HTML tag \"%s\", found at line %d, lacks a " \
		     "child \"text\" node containing a non-null text string " \
		     "as required"

// TBD:  This does not report a case of a text string containing only
// whitespace, but some callers do that themselves:

	if (((Pdn->dn_Pchild) == PDNNULL)
	 || ((Pdn->dn_Pchild->dn_type) != DN_TYPE_TEXT)
	 || ((Pdn->dn_Pchild->dn_text) == PCNULL)
	 || ((Pdn->dn_Pchild->dn_text[0]) == CHNULL))
	{
	    Error(ERREXIT, NOERRNO, ERR_NULLTEXT,
		  TAG(Pdn->dn_type), Pdn->dn_linenum);
	}

	return(Pdn->dn_Pchild->dn_text);

} // ExtractText()


// ****************************************************************************
// E X T R A C T   P A G E   I N F O
//
// Given a docnode, a non-null, modifiable string alleged to contain an nroff
// -man header, such as "Judy(3X)", and pointers to return values, break out
// and return the pieces.  Error out if anything goes wrong.
//
// Note:  Returned strings are in separate malloc'd memory.

FUNCTION void ExtractPageInfo(
	Pdn_t	Pdn,		// for error reporting.
	char *	Pch,		// string to decipher.
	char **	PPageName,	// such as "Judy1".
	char **	PPageSection,	// such as "3X".
	char *	PLcLetter)	// manual tab section, such as "j".
{
	char *	Pch2;		// second place in string.
	char *	Pch3 = PCNULL;	// third  place in string; init for gcc -Wall.

// Find start of string:

	assert(Pch != PCNULL);
	SKIPSPACE(Pch);

	if (*Pch == CHNULL)		// nothing but whitepace.
	{
	    Error(ERREXIT, NOERRNO, ERR_NULLTEXT,
		  TAG(Pdn->dn_type), Pdn->dn_linenum);
	}

// Find "(":

	if ((Pch2 = strchr(Pch, '(')) == PCNULL)
	{
	    Error(ERREXIT, NOERRNO, "Node for HTML tag \"%s\", found at "
		  "line %d, has a child \"text\" node whose text lacks a '('",
		  TAG(Pdn->dn_type), Pdn->dn_linenum);
	}

	if (Pch == Pch2)
	{
	    Error(ERREXIT, NOERRNO, "Node for HTML tag \"%s\", found at "
		  "line %d, has a child \"text\" node whose text starts with "
		  "'(' and lacks a leading pagename",
		  TAG(Pdn->dn_type), Pdn->dn_linenum);
	}

// Validate the "()" suffix, such as "(1)" or "(3X)":

	if ((! ISDIGIT(Pch2[1]))			// not "(<digit>".
	 || ((Pch3 = strchr(Pch2, ')')) == PCNULL)	// no ")".
	 || (Pch2 + 3 < Pch3)				// too far away.
	 || ((Pch2 + 3 == Pch3)				// <digit><suffix>.
	  && (! ISUPPER(Pch2[2]))))			// not <A-Z>.
	{
	    Error(ERREXIT, NOERRNO, "Node for HTML tag \"%s\", found at "
		  "line %d, has a child \"text\" node whose text lacks a "
		  "standard UNIX manual entry suffix in the form "
		  "\"(<digit>[<A-Z>])\", such as \"(1)\" or \"(3X)\"",
		  TAG(Pdn->dn_type), Pdn->dn_linenum);
	}

// Break out parts:

	*Pch2 = *Pch3 = CHNULL;			// terminate at '(' and ')'.
	*PPageName    = StrSave(Pch);
	*PPageSection = StrSave(Pch2 + 1);

// Look for *PLcLetter:

	if (! ISUPPER(**PPageName))
	{
	    Error(ERREXIT, NOERRNO, "Node for HTML tag \"%s\", found at "
		  "line %d, has a child \"text\" node whose text does not "
		  "start with an uppercase letter",
		  TAG(Pdn->dn_type), Pdn->dn_linenum);
	}

	*PLcLetter = tolower((int) (**PPageName));

} // ExtractPageInfo()


// ****************************************************************************
// T A G   T Y P E
//
// Given a non-null string that should be an HTML tag type, a pointer to a
// bool_t to return whether this is a closing tag, and a filename and line
// number for error reporting, look up the tag type in g_dntype[] and return
// its index.  Error out if not found.
//
// As a special case, if presented with "!---" with any number of dashes, look
// for "!--".

FUNCTION int TagType(
	char *	 Tag,		// to look up.
	bool_t * Pisclosing,	// return flag: is a closing tag.
	char *	 Filename,	// for error reporting.
	int	 Linenum)	// for error reporting.
{
	int	 dn_type;	// to return.
	char *	 mytag;		// local variation.

	assert( Tag != PCNULL);
	assert(*Tag != CHNULL);

// Check for closing tag (yes, even for types that don't really allow it):

	if ((*Pisclosing = (*Tag == '/')))	// (()) for gcc.
	{
	    ++Tag;
	    SKIPSPACE(Tag);

	    if (*Tag == CHNULL)
		Error(ERREXIT, NOERRNO, FmtErrLineEnds, Filename, Linenum);
	}

// Translate comment tag to known type:

	mytag = (strncmp(Tag, "!--", 3) ? Tag : "!--");	 // see above.

// Look up tag:
//
// Note:  Main code already asserted dnt_type == dn_type for each entry.

	for (dn_type = 0; TAG(dn_type) != PCNULL; ++dn_type)
	    if (strcmp(mytag, TAG(dn_type)) == 0) return(dn_type);

	Error(ERREXIT, NOERRNO, FILELINE "Unrecognized HTML tag \"%s\"; "
	      "see program source file for recognized types; this is a "
	      "limited, special-purpose translator", Filename, Linenum, Tag);

	/*NOTREACHED*/
	return(0);		// make some compilers happy.

} // TagType()


// ****************************************************************************
// A P P   D O C   N O D E
//
// Given a current docnode tree node, the input file line number, and
// g_Pdnhead, create a new docnode, append it to the tree in the right place,
// and return a pointer to it, with g_Pdnhead updated if required:
//
// * If empty tree, insert new as head of tree.
//
// * Otherwise if current node nests and is not closed, insert as its child.
//
// * Otherwise insert as a sibling of the current node.
//
// Note:  Most HTML tags are non-singletons and hence nest, but if the nesting
// doesn't make sense, too bad, it's not detected, at least not here.

FUNCTION Pdn_t AppDocNode(
	Pdn_t Pdn,		// current docnode tree node.
	int   Linenum)		// in input file.
{
// No current tree, insert first node:

	if (g_Pdnhead == PDNNULL)
	    return(g_Pdnhead = NewDocNode(PDNNULL, Linenum));

// Insert new node as child, with parent set to current node:

	if (NEST(Pdn->dn_type) && (! (Pdn->dn_closed)))
	    return((Pdn->dn_Pchild) = NewDocNode(Pdn, Linenum));

// Insert new node as sibling with same parent:

	(Pdn->dn_Pnext) = NewDocNode(Pdn->dn_Pparent, Linenum);
	(Pdn->dn_Pnext->dn_Pprev) = Pdn;
	return(Pdn->dn_Pnext);

} // AppDocNode()


// ****************************************************************************
// N E W   D O C   N O D E
//
// Malloc() a new docnode and initialize its fields except dn_type, with error
// checking.  Set its parent to the given value.

FUNCTION Pdn_t NewDocNode(
	Pdn_t dn_Pparent,	// parent to record.
	int   Linenum)		// in input file.
{
	Pdn_t Pdn = (Pdn_t) Malloc(sizeof(struct docnode));

	(Pdn -> dn_linenum) = Linenum;
	(Pdn -> dn_closed)  = FALSE;
	(Pdn -> dn_noemit)  = FALSE;
	(Pdn -> dn_bold)    = FALSE;
	(Pdn -> dn_text)    = PCNULL;
	(Pdn -> dn_Pprev)   = PDNNULL;
	(Pdn -> dn_Pnext)   = PDNNULL;
	(Pdn -> dn_Pparent) = dn_Pparent;
	(Pdn -> dn_Pchild)  = PDNNULL;

	return(Pdn);

} // NewDocNode()


// ****************************************************************************
// S A V E   D O C   N O D E
//
// Given a pointer to a docnode, the docnode type, a string for the current
// location (past tag name at whitespace or ">"), and a filename and line
// number for error reporting, save the docnode type in the node, and also save
// the tag text if appropriate; then find the end of the tag (">") and return
// past that location (possibly before more whitespace).  Error out in case of
// syntax error.

FUNCTION char * SaveDocNode(
	Pdn_t	Pdn,		// docnode to modify.
	int	DN_Type,	// new type to save.
	char *	Pch,		// current location past tagname.
	char *	Filename,	// for error reporting.
	int	Linenum)	// for error reporting.
{
	char *	Pch2 = PCNULL;	// second location; init for gcc -Wall.

	assert( Pch != PCNULL);
	assert(*Pch != CHNULL);

// Save type:

	(Pdn->dn_type) = DN_Type;

// Pass whitespace and then find the end of the tag:

	SKIPSPACE(Pch);

	if ((*Pch == CHNULL) || ((Pch2 = strchr(Pch, '>')) == PCNULL))
	    Error(ERREXIT, NOERRNO, FmtErrLineEnds, Filename, Linenum);

// Optionally save tag text:

	if (SAVETAG(DN_Type))
	{
	    *Pch2 = CHNULL;			// temporarily terminate.
	    (Pdn->dn_text) = StrSave(Pch);
	    *Pch2 = '>';
	}

	return(Pch2 + 1);

} // SaveDocNode()


// ****************************************************************************
// P A R E N T   P R E
//
// Given a docnode (can be null) and a flag whether only bold <PRE> is of
// interest, return TRUE if any of its parents is a <PRE> (marked for bold
// text), that is, DN_TYPE_PRE (with dn_bold set); otherwise return FALSE.

FUNCTION bool_t ParentPre(
	Pdn_t  Pdn,		// starting node.
	bool_t BoldOnly)	// flag: only care about bold <PRE>.
{
	if (Pdn == PDNNULL) return (FALSE);	// no parent.

	for (Pdn = Pdn->dn_Pparent; Pdn != PDNNULL; Pdn = Pdn->dn_Pparent)
	{
	    if (((Pdn->dn_type) == DN_TYPE_PRE)
	     && ((! BoldOnly) || (Pdn->dn_bold)))
	    {
		return(TRUE);
	    }
	}

	return(FALSE);

} // ParentPre()


// ****************************************************************************
// M A R K   N O   E M I T
//
// Given a docnode (can be null), and a flag, recursively mark the node and all
// children and siblings as do-not-emit, unless the flag is set, only mark font
// docnodes.

FUNCTION void MarkNoEmit(
	Pdn_t  Pdn,	// top node to mark.
	bool_t Font)	// flag: only mark font docnodes.
{
	if (Pdn == PDNNULL) return;

	if ((! Font)
	 || ((Pdn->dn_type) == DN_TYPE_B)
	 || ((Pdn->dn_type) == DN_TYPE_I))
	{
	    (Pdn->dn_noemit) = TRUE;
	}

	if ((Pdn->dn_Pchild) != PDNNULL) MarkNoEmit(Pdn->dn_Pchild, Font);
	if ((Pdn->dn_Pnext)  != PDNNULL) MarkNoEmit(Pdn->dn_Pnext,  Font);

} // MarkNoEmit()


// ****************************************************************************
// E M I T   T E X T
//
// Given a text string, a bitflag for <PRE> status, and an input line number
// for error reporting, copy the text string to stdout with no added newlines,
// but translating selected HTML escape codes to simple characters, doubling
// any backslashes, and if InPRE, inserting .IP (if INPRE_INDENT) or .PP at
// blank lines (between successive newlines), and if INPRE_BOLD, putting back
// bold font since .IP/.PP seems to reset the font.  Warn about unrecognized
// escape codes.

struct et_list {
	char * et_escape;	// expected text.
	size_t et_len;		// of expected text.
	char   et_emit;		// equivalent char.
} et_list[] = {
	{ "amp;", 4, '&', },
	{ "gt;",  3, '>', },
	{ "lt;",  3, '<', },
	{ PCNULL, 0, ' ', },	// end of list.
};

FUNCTION void EmitText(
	char * Pch,		// text to emit.
	int    InPRE,		// bitflag for <PRE> status.
	int    Linenum)		// for error reporting.
{
	char * Pch2;		// place in text.
	struct et_list * Pet;	// place in et_list[].

	while ((Pch2 = strchr(Pch, '&')) != PCNULL)	// another escape code.
	{
	    *Pch2 = CHNULL;		// briefly terminate.
	    EmitTextPRE(Pch, InPRE);	// emit preceding part.
	    *Pch2 = '&';
	    Pch = Pch2 + 1;			// past '&'.

	    for (Pet = et_list; Pet->et_escape != PCNULL; ++Pet)
	    {
		if (strncmp(Pch, Pet->et_escape, Pet->et_len) == 0)
		{
		    PUTC(Pet->et_emit);		// translate.
		    Pch += Pet->et_len;		// skip escapecode.
		    break;
		}
	    }

	    if (Pet->et_escape == PCNULL)	// no match found.
	    {
		Error(NOEXIT, NOERRNO, "Unrecognized HTML escape code in "
		      "line %d (or text beginning on that line): \"%.4s...\", "
		      "passed through unaltered", Linenum, Pch2);

		PUTC('&');		// emit start of escape code.
		// continue with Pch is just after the '&'.
	    }
	}

	EmitTextPRE(Pch, InPRE);	// emit remaining part.

} // EmitText()


// ****************************************************************************
// E M I T   T E X T   P R E
//
// Given a text string with no HTML escape codes in it and a bitflag for <PRE>
// status (see EmitText()), emit the string with <PRE> handling, and with any
// backslashes doubled.

FUNCTION void EmitTextPRE(
	char * Pch,		// string to emit.
	int    InPRE)		// bitflag for <PRE> status.
{
	char * Pch2;		// place in string.

	if (! InPRE) { EmitTextBS(Pch); return; }

	while ((Pch2 = strchr(Pch, '\n')) != PCNULL)	// another newline.
	{
	    *Pch2 = CHNULL;		// briefly terminate.
	    EmitTextBS(Pch);		// emit preceding part.
	    *Pch2 = '\n';
	    PUTC('\n');			// emit current newline.

	    if (*(Pch = Pch2 + 1) == '\n')	// successive newline.
	    {
		// emit before next newline:
		PUTS((InPRE & INPRE_INDENT) ? ".IP" : ".PP");

		// also reset font:
		if (InPRE & INPRE_BOLD) PUTS("\n.ft B");
	    }
	}

	EmitTextBS(Pch);		// emit trailing part.

} // EmitTextPRE()


// ****************************************************************************
// E M I T   T E X T   B S
//
// Given a text string with no HTML escape codes in it, emit the string with
// any backslashes doubled.

FUNCTION void EmitTextBS(
	char * Pch)		// string to emit.
{
	while (*Pch != CHNULL)
	{
	    PUTC(*Pch); if (*Pch == '\\') PUTC('\\');
	    ++Pch;
	}

} // EmitTextBS()


// ****************************************************************************
// N O   W H I T E   S P A C E
//
// Given a string, return TRUE if it contains no whitespace, otherwise FALSE.

FUNCTION bool_t NoWhiteSpace(
	char * Pch)	// string to check.
{
	assert(Pch != PCNULL);

	while (*Pch != CHNULL) { if (ISSPACE(*Pch)) return(FALSE); ++Pch; }
	return(TRUE);

} // NoWhiteSpace()


// ****************************************************************************
// C O U N T   N E W L I N E S
//
// Return the number of newline chars in a string.

FUNCTION int CountNewlines(
	char * Pch)	// in which to count newlines.
{
	int    count = 0;

	assert(Pch != PCNULL);

	while (*Pch != CHNULL) count += ((*Pch++) == '\n');
	return(count);

} // CountNewlines()


// ****************************************************************************
// S T R   S A V E
//
// Given a string, copy the string into malloc'd space and return a pointer to
// the new copy.  Error out if malloc() fails.

FUNCTION char * StrSave(
	char * string)
{
	return(strcpy((char *) Malloc((size_t) (strlen(string) + 1)), string));

} // StrSave()


// ****************************************************************************
// S T R   S A V E   N
//
// Given one or more strings, terminated by a null pointer, allocate space for
// the concatenation of the strings, concatenate them, and return a pointer to
// the result.  Also free() all but the last string.

FUNCTION char * StrSaveN(
	char *	String, ...)
{
	va_list Parg;			// place in arg list.
	char *	stringN;		// string from arg list.
	char *	stringN1 = PCNULL;	// previous string.
	size_t	size = 0;		// total bytes needed.
	char *	result;			// string to return.


// DETERMINE SPACE NEEDED:

	va_start(Parg, String);

	for (stringN  = String;
	     stringN != PCNULL;
	     stringN  = va_arg(Parg, char *))
	{
	    size += strlen(stringN);
	}

	va_end(Parg);


// ALLOCATE SPACE, CONCATENATE STRINGS:

	*(result = (char *) Malloc((size_t) (size + 1))) = CHNULL;

	va_start(Parg, String);

	for (stringN  = String;
	     stringN != PCNULL;
	     stringN  = va_arg(Parg, char *))
	{
	    if (stringN1 != PCNULL) free((void *) stringN1);
	    stringN1 = stringN;			// all but last string.
	    (void) strcat(result, stringN);
	}

	va_end(Parg);
	return(result);

} // StrSaveN()


// ****************************************************************************
// M A L L O C
//
// Do a malloc() with error checking.

FUNCTION void * Malloc(
	size_t	Size)	// bytes to get.
{
	void *	Pm;	// pointer to memory.

	if ((Pm = malloc(Size)) == (void *) NULL)
	    Error(ERREXIT, errno, "Cannot malloc %d bytes", Size);

	return(Pm);

} // Malloc()


// ****************************************************************************
// U S A G E
//
// Print usage messages (char *gc_usage[]) to stderr and exit with ERREXIT.
// Follow each message line by a newline.

FUNCTION void Usage(void)
{
	int which = 0;		// current line.

	while (gc_usage[which] != PCNULL)
	{
	    (void) fprintf(stderr, gc_usage[which++], gc_myname);
	    (void) putc('\n', stderr);
	}

	exit(ERREXIT);

} // Usage()


// ****************************************************************************
// E R R O R
//
// Given an exit value (NOEXIT, ERREXIT, or USAGE), an errno value (NOERRNO if
// none), a message (printf) string, and zero or more argument strings, print
// an error message to stderr and, if exitvalue is NOEXIT, return; if USAGE,
// print a pointer to the program's usage message; otherwise exit with the
// value.
//
// Message is preceded by "<myname>: " using global gc_myname, and by
// "Warning: " for NOEXIT, and followed by a period and newline.  If myerrno
// (system error number) is not NOERRNO, a relevant message is appended before
// the period.

FUNCTION void Error(
	int	Exitvalue,	// or NOEXIT for warning.
	int	MyErrno,	// system errno if relevant.
	char *	Message, ...)
{
	va_list Parg;		// place in arg list.

	(void) fprintf(stderr, "%s: ", gc_myname);
	if (Exitvalue == NOEXIT) (void) fputs("Warning: ", stderr);

	va_start(Parg, Message);
	(void) vfprintf(stderr, Message, Parg);
	va_end(Parg);

	if (MyErrno != NOERRNO)
	{
	    (void) fprintf(stderr, ": %s (errno = %d)", strerror(MyErrno),
			   MyErrno);
	}

	(void) putc('.',  stderr);
	(void) putc('\n', stderr);

	if (Exitvalue == USAGE)
	{
	    (void) fprintf(stderr, "For a usage summary, run %s -?\n",
			   gc_myname);
	}

	DBGCODE(DumpTree(g_Pdnhead, /* Depth = */ 0, /* Separator = */ FALSE);)

	if (Exitvalue != NOEXIT)
	    exit(Exitvalue);

} // Error()


#ifdef DEBUG

// ****************************************************************************
// D U M P   T R E E
//
// Dump to stdout a representation of the docnode tree under g_Pdnhead.
// Recursively traverse the tree in-order (parent then child then sibling).

FUNCTION void DumpTree(
	Pdn_t  Pdn,		// first node of current sibling list.
	int    Depth,		// current depth.
	bool_t Separator)	// print a separator line after a long dump.
{
	int   indent;		// for counting to Depth.

// Check if enabled:

	if (getenv("DUMP") == PCNULL)
	{
	    PUTS(".\\\" $DUMP not set; DumpTree() disabled.\n");
	    return;
	}

// Check for empty tree:

	if ((Depth == 0) && (Pdn == PDNNULL))
	{
	    PUTS("Head pointer is null.\n");
	    return;
	}

// Print siblings and each of their children, indented to Depth after the node
// address:

	while (Pdn != PDNNULL)
	{
	    (void) printf("%lx ", (unsigned long) Pdn);

	    for (indent = 0; indent <= Depth; ++indent) PUTC('.');

	    (void) printf(" %-5s %3d %c %lx %lx \"%s\"\n",
			  ((Pdn -> dn_type) == DN_TYPE_TEXT) ?
			      "text" : TAG(Pdn -> dn_type),
			  Pdn -> dn_linenum,
			  (Pdn->dn_closed) ? 'c' : 'o',
			  Pdn -> dn_Pparent,
			  Pdn -> dn_Pprev,
			  Pdn -> dn_text);

	    if ((Pdn -> dn_Pchild) != PDNNULL)
		DumpTree(Pdn -> dn_Pchild, Depth + 1, Separator);

	    Pdn = Pdn -> dn_Pnext;
	}

// Print separator line:

	if ((Depth == 0) && Separator)
	    PUTS("=======================================================\n");

} // DumpTree()

#endif // DEBUG
