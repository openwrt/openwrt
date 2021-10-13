INCLUDES =  -I. -I.. -I../JudyCommon/ 
AM_CFLAGS = -DJUDY1 @WARN_CFLAGS@ 

noinst_LTLIBRARIES = libJudy1.la libnext.la libprev.la libcount.la libinline.la

libJudy1_la_SOURCES = Judy1Test.c Judy1Tables.c Judy1Set.c Judy1SetArray.c Judy1Unset.c Judy1Cascade.c Judy1Count.c Judy1CreateBranch.c Judy1Decascade.c Judy1First.c Judy1FreeArray.c Judy1InsertBranch.c Judy1MallocIF.c Judy1MemActive.c Judy1MemUsed.c 

libnext_la_SOURCES = Judy1Next.c Judy1NextEmpty.c 
libnext_la_CFLAGS = $(AM_CFLAGS) -DJUDYNEXT

libprev_la_SOURCES = Judy1Prev.c Judy1PrevEmpty.c 
libprev_la_CFLAGS = $(AM_CFLAGS) -DJUDYPREV

libcount_la_SOURCES = Judy1ByCount.c
libcount_la_CFLAGS = $(AM_CFLAGS) -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB

libinline_la_SOURCES = j__udy1Test.c
libinline_la_CFLAGS = $(AM_CFLAGS) -DJUDYGETINLINE

Judy1Tables.c: Judy1TablesGen.c
	$(HOSTCC) $(INCLUDES) $(AM_CFLAGS)-DJU_64BIT $(HOSTCCFLAGS) -o Judy1TablesGen Judy1TablesGen.c; ./Judy1TablesGen

Judy1ByCount.c:../JudyCommon/JudyByCount.c
	cp -f ../JudyCommon/JudyByCount.c      		Judy1ByCount.c   

Judy1Cascade.c:../JudyCommon/JudyCascade.c
	cp -f ../JudyCommon/JudyCascade.c      		Judy1Cascade.c

Judy1Count.c:../JudyCommon/JudyCount.c
	cp -f ../JudyCommon/JudyCount.c        		Judy1Count.c

Judy1CreateBranch.c:../JudyCommon/JudyCreateBranch.c
	cp -f ../JudyCommon/JudyCreateBranch.c 		Judy1CreateBranch.c

Judy1Decascade.c:../JudyCommon/JudyDecascade.c
	cp -f ../JudyCommon/JudyDecascade.c    		Judy1Decascade.c

Judy1Unset.c:../JudyCommon/JudyDel.c
	cp -f ../JudyCommon/JudyDel.c          		Judy1Unset.c

Judy1First.c:../JudyCommon/JudyFirst.c
	cp -f ../JudyCommon/JudyFirst.c        		Judy1First.c

Judy1FreeArray.c:../JudyCommon/JudyFreeArray.c
	cp -f ../JudyCommon/JudyFreeArray.c    		Judy1FreeArray.c

Judy1Test.c:../JudyCommon/JudyGet.c
	cp -f ../JudyCommon/JudyGet.c          		Judy1Test.c

j__udy1Test.c:../JudyCommon/JudyGet.c
	cp -f ../JudyCommon/JudyGet.c          		j__udy1Test.c

Judy1SetArray.c:../JudyCommon/JudyInsArray.c
	cp -f ../JudyCommon/JudyInsArray.c     		Judy1SetArray.c

Judy1Set.c:../JudyCommon/JudyIns.c
	cp -f ../JudyCommon/JudyIns.c          		Judy1Set.c

Judy1InsertBranch.c:../JudyCommon/JudyInsertBranch.c
	cp -f ../JudyCommon/JudyInsertBranch.c 		Judy1InsertBranch.c

Judy1MallocIF.c:../JudyCommon/JudyMallocIF.c
	cp -f ../JudyCommon/JudyMallocIF.c     		Judy1MallocIF.c

Judy1MemActive.c:../JudyCommon/JudyMemActive.c
	cp -f ../JudyCommon/JudyMemActive.c    		Judy1MemActive.c

Judy1MemUsed.c:../JudyCommon/JudyMemUsed.c
	cp -f ../JudyCommon/JudyMemUsed.c      		Judy1MemUsed.c

Judy1Next.c:../JudyCommon/JudyPrevNext.c
	cp -f ../JudyCommon/JudyPrevNext.c     		Judy1Next.c

Judy1Prev.c:../JudyCommon/JudyPrevNext.c
	cp -f ../JudyCommon/JudyPrevNext.c     		Judy1Prev.c

Judy1NextEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f ../JudyCommon/JudyPrevNextEmpty.c		Judy1NextEmpty.c

Judy1PrevEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f ../JudyCommon/JudyPrevNextEmpty.c		Judy1PrevEmpty.c

Judy1TablesGen.c:../JudyCommon/JudyTables.c
	cp -f ../JudyCommon/JudyTables.c	        Judy1TablesGen.c

DISTCLEANFILES = .deps Makefile 

CLEANFILES = Judy1ByCount.c \
	     Judy1Cascade.c \
	     Judy1Count.c \
	     Judy1CreateBranch.c \
	     Judy1Decascade.c \
	     Judy1Unset.c \
	     Judy1First.c \
	     Judy1FreeArray.c \
	     Judy1Test.c \
	     j__udy1Test.c \
	     Judy1SetArray.c \
	     Judy1Set.c \
	     Judy1InsertBranch.c \
	     Judy1MallocIF.c \
	     Judy1MemActive.c \
	     Judy1MemUsed.c \
	     Judy1Next.c \
	     Judy1Prev.c \
	     Judy1NextEmpty.c \
	     Judy1PrevEmpty.c \
	     Judy1TablesGen.c \
	     Judy1Tables.c \
	     .libs \
	     Judy1TablesGen \
	     *.o \
	     *.lo \
	     *.la
