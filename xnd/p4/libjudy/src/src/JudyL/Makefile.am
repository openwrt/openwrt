INCLUDES =  -I. -I.. -I../JudyCommon/ 
AM_CFLAGS = -DJUDYL @WARN_CFLAGS@ 

noinst_LTLIBRARIES = libJudyL.la libnext.la libprev.la libcount.la libinline.la

libJudyL_la_SOURCES = JudyLCascade.c JudyLTables.c JudyLCount.c JudyLCreateBranch.c JudyLDecascade.c JudyLDel.c JudyLFirst.c JudyLFreeArray.c JudyLGet.c JudyLInsArray.c JudyLIns.c JudyLInsertBranch.c JudyLMallocIF.c JudyLMemActive.c JudyLMemUsed.c 

libnext_la_SOURCES = JudyLNext.c JudyLNextEmpty.c
libnext_la_CFLAGS = $(AM_CFLAGS) -DJUDYNEXT

libprev_la_SOURCES = JudyLPrev.c JudyLPrevEmpty.c
libprev_la_CFLAGS = $(AM_CFLAGS) -DJUDYPREV

libcount_la_SOURCES = JudyLByCount.c
libcount_la_CFLAGS = $(AM_CFLAGS) -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB

libinline_la_SOURCES = j__udyLGet.c
libinline_la_CFLAGS = $(AM_CFLAGS) -DJUDYGETINLINE

JudyLTables.c: JudyLTablesGen.c
	$(HOSTCC) $(INCLUDES) $(AM_CFLAGS) -DJU_64BIT $(HOSTCCFLAGS) -o JudyLTablesGen JudyLTablesGen.c; ./JudyLTablesGen 

JudyLByCount.c:   ../JudyCommon/JudyByCount.c
	cp -f ../JudyCommon/JudyByCount.c      		JudyLByCount.c   

JudyLCascade.c:../JudyCommon/JudyCascade.c
	cp -f ../JudyCommon/JudyCascade.c       	JudyLCascade.c

JudyLCount.c:../JudyCommon/JudyCount.c
	cp -f ../JudyCommon/JudyCount.c        		JudyLCount.c

JudyLCreateBranch.c:../JudyCommon/JudyCreateBranch.c 
	cp -f ../JudyCommon/JudyCreateBranch.c 		JudyLCreateBranch.c

JudyLDecascade.c:../JudyCommon/JudyDecascade.c    
	cp -f ../JudyCommon/JudyDecascade.c    		JudyLDecascade.c

JudyLDel.c:../JudyCommon/JudyDel.c       
	cp -f ../JudyCommon/JudyDel.c          		JudyLDel.c

JudyLFirst.c:../JudyCommon/JudyFirst.c
	cp -f ../JudyCommon/JudyFirst.c        		JudyLFirst.c

JudyLFreeArray.c:../JudyCommon/JudyFreeArray.c    
	cp -f ../JudyCommon/JudyFreeArray.c    		JudyLFreeArray.c

JudyLGet.c:../JudyCommon/JudyGet.c       
	cp -f ../JudyCommon/JudyGet.c          		JudyLGet.c

j__udyLGet.c:../JudyCommon/JudyGet.c 
	cp -f ../JudyCommon/JudyGet.c          		j__udyLGet.c

JudyLInsArray.c:../JudyCommon/JudyInsArray.c     
	cp -f ../JudyCommon/JudyInsArray.c     		JudyLInsArray.c

JudyLIns.c:../JudyCommon/JudyIns.c      
	cp -f ../JudyCommon/JudyIns.c          		JudyLIns.c

JudyLInsertBranch.c:../JudyCommon/JudyInsertBranch.c
	cp -f ../JudyCommon/JudyInsertBranch.c 		JudyLInsertBranch.c

JudyLMallocIF.c:../JudyCommon/JudyMallocIF.c     
	cp -f ../JudyCommon/JudyMallocIF.c     		JudyLMallocIF.c

JudyLMemActive.c:../JudyCommon/JudyMemActive.c
	cp -f ../JudyCommon/JudyMemActive.c    		JudyLMemActive.c

JudyLMemUsed.c:../JudyCommon/JudyMemUsed.c   
	cp -f ../JudyCommon/JudyMemUsed.c      		JudyLMemUsed.c

JudyLNext.c:../JudyCommon/JudyPrevNext.c
	cp -f ../JudyCommon/JudyPrevNext.c     		JudyLNext.c

JudyLPrev.c:../JudyCommon/JudyPrevNext.c
	cp -f ../JudyCommon/JudyPrevNext.c     		JudyLPrev.c

JudyLNextEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f ../JudyCommon/JudyPrevNextEmpty.c		JudyLNextEmpty.c

JudyLPrevEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f ../JudyCommon/JudyPrevNextEmpty.c		JudyLPrevEmpty.c
	
JudyLTablesGen.c:../JudyCommon/JudyTables.c
	cp -f ../JudyCommon/JudyTables.c       	 	JudyLTablesGen.c

DISTCLEANFILES = .deps Makefile 

CLEANFILES = JudyLByCount.c  \
	     JudyLCascade.c \
	     JudyLCount.c \
	     JudyLCreateBranch.c \
	     JudyLDecascade.c \
	     JudyLDel.c \
	     JudyLFirst.c \
	     JudyLFreeArray.c \
	     JudyLGet.c \
	     j__udyLGet.c \
	     JudyLInsArray.c \
	     JudyLIns.c \
	     JudyLInsertBranch.c \
	     JudyLMallocIF.c \
	     JudyLMemActive.c \
	     JudyLMemUsed.c \
	     JudyLNext.c \
	     JudyLPrev.c \
	     JudyLNextEmpty.c \
	     JudyLPrevEmpty.c \
	     JudyLTablesGen.c \
	     JudyLTables.c \
	     JudyLTablesGen \
	     .libs \
	     *.o \
	     *.lo \
	     *.la 
