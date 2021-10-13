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

// @(#) $Revision: 4.17 $ $Source: /judy/src/Judy.h.check.c $
//
// Fake "program" to test the exports in Judy.h by exercising each one.  This
// program should compile OK (with libJudy.a) but does not run OK.

#include "Judy.h"

int
main()
{
	Pvoid_t  PArray  = (Pvoid_t) NULL;
	PPvoid_t PPArray = &PArray;
	Word_t	 Index	 = 0;
	PWord_t  PIndex  = &Index;
	uint8_t *CIndex  = NULL;
	PPvoid_t PPvoid;
	Word_t	 myword;
	Word_t	 Length;
	int	 myint;

// JUDY FUNCTIONS:

	myint  = Judy1Test	 ( PArray,  Index,		PJE0);
	myint  = Judy1Set	 (PPArray,  Index,		PJE0);
	myint  = Judy1SetArray	 (PPArray,  Index, &Index,	PJE0);
	myint  = Judy1Unset	 (PPArray,  Index,		PJE0);
	myword = Judy1Count	 ( PArray,  Index,  Index,	PJE0);
	myint  = Judy1ByCount	 ( PArray,  Index, PIndex,	PJE0);
	myword = Judy1FreeArray	 (PPArray,			PJE0);
	myword = Judy1MemUsed	 ( PArray			    );
	myword = Judy1MemActive	 ( PArray			    );
	myint  = Judy1First	 ( PArray, PIndex,		PJE0);
	myint  = Judy1Next	 ( PArray, PIndex,		PJE0);
	myint  = Judy1Last	 ( PArray, PIndex,		PJE0);
	myint  = Judy1Prev	 ( PArray, PIndex,		PJE0);
	myint  = Judy1FirstEmpty ( PArray, PIndex,		PJE0);
	myint  = Judy1NextEmpty	 ( PArray, PIndex,		PJE0);
	myint  = Judy1LastEmpty	 ( PArray, PIndex,		PJE0);
	myint  = Judy1PrevEmpty	 ( PArray, PIndex,		PJE0);

	PPvoid = JudyLGet	 ( PArray,  Index,		PJE0);
	PPvoid = JudyLIns	 (PPArray,  Index,		PJE0);
	myint  = JudyLInsArray	 (PPArray,  Index, &Index, &Index, PJE0);
	myint  = JudyLDel	 (PPArray,  Index,		PJE0);
	myword = JudyLCount	 ( PArray,  Index,  Index,	PJE0);
	PPvoid = JudyLByCount	 ( PArray,  Index, PIndex,	PJE0);
	myword = JudyLFreeArray	 (PPArray,			PJE0);
	myword = JudyLMemUsed	 ( PArray			    );
	myword = JudyLMemActive	 ( PArray			    );
	PPvoid = JudyLFirst	 ( PArray, PIndex,		PJE0);
	PPvoid = JudyLNext	 ( PArray, PIndex,		PJE0);
	PPvoid = JudyLLast	 ( PArray, PIndex,		PJE0);
	PPvoid = JudyLPrev	 ( PArray, PIndex,		PJE0);
	myint  = JudyLFirstEmpty ( PArray, PIndex,		PJE0);
	myint  = JudyLNextEmpty	 ( PArray, PIndex,		PJE0);
	myint  = JudyLLastEmpty	 ( PArray, PIndex,		PJE0);
	myint  = JudyLPrevEmpty	 ( PArray, PIndex,		PJE0);

	PPvoid = JudySLGet	 ( PArray, CIndex,		PJE0);
	PPvoid = JudySLIns	 (PPArray, CIndex,		PJE0);
	myint =	 JudySLDel	 (PPArray, CIndex,		PJE0);
	myword = JudySLFreeArray (PPArray,			PJE0);
	PPvoid = JudySLFirst	 ( PArray, CIndex,		PJE0);
	PPvoid = JudySLNext	 ( PArray, CIndex,		PJE0);
	PPvoid = JudySLLast	 ( PArray, CIndex,		PJE0);
	PPvoid = JudySLPrev	 ( PArray, CIndex,		PJE0);

        PPvoid = JudyHSGet       ( PArray, CIndex, Length);
        PPvoid = JudyHSIns       (PPArray, CIndex, Length,      PJE0);
        myint  = JudyHSDel       (PPArray, CIndex, Length,      PJE0);


// MACRO EQUIVALENTS:

	J1T   (myint,  PArray, Index);
	J1S   (myint,  PArray, Index);
	J1SA  (myint,  PArray, Index, &Index);
	J1U   (myint,  PArray, Index);
	J1F   (myint,  PArray, Index);
	J1N   (myint,  PArray, Index);
	J1L   (myint,  PArray, Index);
	J1P   (myint,  PArray, Index);
	J1FE  (myint,  PArray, Index);
	J1NE  (myint,  PArray, Index);
	J1LE  (myint,  PArray, Index);
	J1PE  (myint,  PArray, Index);
	J1C   (myword, PArray, Index, Index);
	J1BC  (myint,  PArray, Index, Index);
	J1FA  (myword, PArray);

	JLG   (PPvoid, PArray, Index);
	JLI   (PPvoid, PArray, Index);
	JLIA  (myint,  PArray, Index, &Index, &Index);
	JLD   (myint,  PArray, Index);
	JLF   (PPvoid, PArray, Index);
	JLN   (PPvoid, PArray, Index);
	JLL   (PPvoid, PArray, Index);
	JLP   (PPvoid, PArray, Index);
	JLFE  (myint,  PArray, Index);
	JLNE  (myint,  PArray, Index);
	JLLE  (myint,  PArray, Index);
	JLPE  (myint,  PArray, Index);
	JLC   (myword, PArray, Index,  Index);
	JLBC  (PPvoid, PArray, myword, Index);
	JLFA  (myword, PArray);

	JSLG  (PPvoid, PArray, CIndex);
	JSLI  (PPvoid, PArray, CIndex);
	JSLD  (myint,  PArray, CIndex);
	JSLF  (PPvoid, PArray, CIndex);
	JSLN  (PPvoid, PArray, CIndex);
	JSLL  (PPvoid, PArray, CIndex);
	JSLP  (PPvoid, PArray, CIndex);
	JSLFA (myword, PArray);

        JHSI  (PPvoid, PArray, CIndex, Length);
        JHSG  (PPvoid, PArray, CIndex, Length);
        JHSD  (myint,  PArray, CIndex, Length);

        return(0);

} // main()
