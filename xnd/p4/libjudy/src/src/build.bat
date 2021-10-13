@ECHO OFF

echo Set Compiler
SET CC=cl

echo Set Options
SET COPT=-DJU_WIN
SET O=-DJUDY1
SET L=-DJUDYL
SET INC=-I.. -I..\JudyCommon

echo Deleting Old Files
del JudyCommon\*.obj JudySL\*.obj JudyHS\*.obj Judy1\*.obj JudyL\*.obj *.dll

echo Giving Judy1 the proper Names
copy JudyCommon\JudyByCount.c      	Judy1\Judy1ByCount.c   
copy JudyCommon\JudyCascade.c      	Judy1\Judy1Cascade.c
copy JudyCommon\JudyCount.c        	Judy1\Judy1Count.c
copy JudyCommon\JudyCreateBranch.c 	Judy1\Judy1CreateBranch.c
copy JudyCommon\JudyDecascade.c    	Judy1\Judy1Decascade.c
copy JudyCommon\JudyDel.c          	Judy1\Judy1Unset.c
copy JudyCommon\JudyFirst.c        	Judy1\Judy1First.c
copy JudyCommon\JudyFreeArray.c    	Judy1\Judy1FreeArray.c
copy JudyCommon\JudyGet.c          	Judy1\Judy1Test.c
copy JudyCommon\JudyGet.c          	Judy1\j__udy1Test.c
copy JudyCommon\JudyInsArray.c     	Judy1\Judy1SetArray.c
copy JudyCommon\JudyIns.c          	Judy1\Judy1Set.c
copy JudyCommon\JudyInsertBranch.c 	Judy1\Judy1InsertBranch.c
copy JudyCommon\JudyMallocIF.c     	Judy1\Judy1MallocIF.c
copy JudyCommon\JudyMemActive.c    	Judy1\Judy1MemActive.c
copy JudyCommon\JudyMemUsed.c      	Judy1\Judy1MemUsed.c
copy JudyCommon\JudyPrevNext.c     	Judy1\Judy1Next.c
copy JudyCommon\JudyPrevNext.c     	Judy1\Judy1Prev.c
copy JudyCommon\JudyPrevNextEmpty.c	Judy1\Judy1NextEmpty.c
copy JudyCommon\JudyPrevNextEmpty.c	Judy1\Judy1PrevEmpty.c
copy JudyCommon\JudyTables.c	        Judy1\Judy1TablesGen.c

echo Giving JudyL the proper Names
copy JudyCommon\JudyByCount.c      	JudyL\JudyLByCount.c   
copy JudyCommon\JudyCascade.c           JudyL\JudyLCascade.c
copy JudyCommon\JudyCount.c        	JudyL\JudyLCount.c
copy JudyCommon\JudyCreateBranch.c 	JudyL\JudyLCreateBranch.c
copy JudyCommon\JudyDecascade.c    	JudyL\JudyLDecascade.c
copy JudyCommon\JudyDel.c          	JudyL\JudyLDel.c
copy JudyCommon\JudyFirst.c        	JudyL\JudyLFirst.c
copy JudyCommon\JudyFreeArray.c    	JudyL\JudyLFreeArray.c
copy JudyCommon\JudyGet.c          	JudyL\JudyLGet.c
copy JudyCommon\JudyGet.c          	JudyL\j__udyLGet.c
copy JudyCommon\JudyInsArray.c     	JudyL\JudyLInsArray.c
copy JudyCommon\JudyIns.c          	JudyL\JudyLIns.c
copy JudyCommon\JudyInsertBranch.c 	JudyL\JudyLInsertBranch.c
copy JudyCommon\JudyMallocIF.c     	JudyL\JudyLMallocIF.c
copy JudyCommon\JudyMemActive.c    	JudyL\JudyLMemActive.c
copy JudyCommon\JudyMemUsed.c      	JudyL\JudyLMemUsed.c
copy JudyCommon\JudyPrevNext.c     	JudyL\JudyLNext.c
copy JudyCommon\JudyPrevNext.c     	JudyL\JudyLPrev.c
copy JudyCommon\JudyPrevNextEmpty.c	JudyL\JudyLNextEmpty.c
copy JudyCommon\JudyPrevNextEmpty.c	JudyL\JudyLPrevEmpty.c
copy JudyCommon\JudyTables.c	        JudyL\JudyLTablesGen.c


echo Compile JudyCommon\JudyMalloc - common to Judy1 and JudyL
cd JudyCommon
%CC% -I. -I.. -DJU_WIN -c JudyMalloc.c

cd ..

echo This table is constructed from Juudy1.h data to match malloc(3) needs
cd Judy1
%CC% %INC% %COPT% %O% Judy1TablesGen.c -o Judy1TablesGen
del Judy1TablesGen.obj
Judy1TablesGen
%CC% %INC% %COPT% %O% -c Judy1Tables.c

echo compile the main line Judy1 modules
echo %CC% %INC% %COPT% %O% -c Judy1Test.c
%CC% %INC% %COPT% %O% -c Judy1Test.c
echo %CC% %INC% %COPT% %O% -c -DJUDYGETINLINE j__udy1Test.c
%CC% %INC% %COPT% %O% -c -DJUDYGETINLINE j__udy1Test.c
echo %CC% %INC% %COPT% %O% -c Judy1Set.c
%CC% %INC% %COPT% %O% -c Judy1Set.c
echo %CC% %INC% %COPT% %O% -c Judy1SetArray.c
%CC% %INC% %COPT% %O% -c Judy1SetArray.c
echo %CC% %INC% %COPT% %O% -c Judy1Unset.c
%CC% %INC% %COPT% %O% -c Judy1Unset.c
echo %CC% %INC% %COPT% %O% -c Judy1First.c
%CC% %INC% %COPT% %O% -c Judy1First.c
echo %CC% %INC% %COPT% %O% -DJUDYNEXT -c Judy1Next.c
%CC% %INC% %COPT% %O% -DJUDYNEXT -c Judy1Next.c
echo %CC% %INC% %COPT% %O% -DJUDYPREV -c Judy1Prev.c
%CC% %INC% %COPT% %O% -DJUDYPREV -c Judy1Prev.c
echo %CC% %INC% %COPT% %O% -DJUDYNEXT -c Judy1NextEmpty.c
%CC% %INC% %COPT% %O% -DJUDYNEXT -c Judy1NextEmpty.c
echo %CC% %INC% %COPT% %O% -DJUDYPREV -c Judy1PrevEmpty.c
%CC% %INC% %COPT% %O% -DJUDYPREV -c Judy1PrevEmpty.c
echo %CC% %INC% %COPT% %O% -c Judy1Count.c
%CC% %INC% %COPT% %O% -c Judy1Count.c
echo %CC% %INC% %COPT% %O% -c -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB Judy1ByCount.c
%CC% %INC% %COPT% %O% -c -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB Judy1ByCount.c
echo %CC% %INC% %COPT% %O% -c Judy1FreeArray.c
%CC% %INC% %COPT% %O% -c Judy1FreeArray.c
echo %CC% %INC% %COPT% %O% -c Judy1MemUsed.c
%CC% %INC% %COPT% %O% -c Judy1MemUsed.c
echo %CC% %INC% %COPT% %O% -c Judy1MemActive.c
%CC% %INC% %COPT% %O% -c Judy1MemActive.c
echo %CC% %INC% %COPT% %O% -c Judy1Cascade.c
%CC% %INC% %COPT% %O% -c Judy1Cascade.c
echo %CC% %INC% %COPT% %O% -c Judy1Decascade.c
%CC% %INC% %COPT% %O% -c Judy1Decascade.c
echo %CC% %INC% %COPT% %O% -c Judy1CreateBranch.c
%CC% %INC% %COPT% %O% -c Judy1CreateBranch.c
echo %CC% %INC% %COPT% %O% -c Judy1InsertBranch.C
%CC% %INC% %COPT% %O% -c Judy1InsertBranch.C
echo %CC% %INC% %COPT% %O% -c Judy1MallocIF.c
%CC% %INC% %COPT% %O% -c Judy1MallocIF.c

cd ..
cd JudyL

echo This table is constructed from Juudy1.h data to match malloc(3) needs
%CC% %INC% %COPT% JudyLTablesGen.c %L% -o JudyLTablesGen
del JudyLTablesGen.obj
JudyLTablesGen
%CC% %INC% %COPT% %L% -c JudyLTables.c

echo compile the main line JudyL modules
echo %CC% %INC% %COPT% %L% -c JudyLGet.c
%CC% %INC% %COPT% %L% -c JudyLGet.c
echo %CC% %INC% %COPT% %L% -c -DJUDYGETINLINE j__udyLGet.c
%CC% %INC% %COPT% %L% -c -DJUDYGETINLINE j__udyLGet.c
echo %CC% %INC% %COPT% %L% -c JudyLIns.c
%CC% %INC% %COPT% %L% -c JudyLIns.c
echo %CC% %INC% %COPT% %L% -c JudyLInsArray.c
%CC% %INC% %COPT% %L% -c JudyLInsArray.c
echo %CC% %INC% %COPT% %L% -c JudyLDel.c
%CC% %INC% %COPT% %L% -c JudyLDel.c
echo %CC% %INC% %COPT% %L% -c JudyLFirst.c
%CC% %INC% %COPT% %L% -c JudyLFirst.c
echo %CC% %INC% %COPT% %L% -c -DJUDYNEXT JudyLNext.c
%CC% %INC% %COPT% %L% -c -DJUDYNEXT JudyLNext.c
echo %CC% %INC% %COPT% %L% -c -DJUDYPREV JudyLPrev.c
%CC% %INC% %COPT% %L% -c -DJUDYPREV JudyLPrev.c
echo %CC% %INC% %COPT% %L% -c -DJUDYNEXT JudyLNextEmpty.c
%CC% %INC% %COPT% %L% -c -DJUDYNEXT JudyLNextEmpty.c
echo %CC% %INC% %COPT% %L% -c -DJUDYPREV JudyLPrevEmpty.c
%CC% %INC% %COPT% %L% -c -DJUDYPREV JudyLPrevEmpty.c
echo %CC% %INC% %COPT% %L% -c JudyLCount.c
%CC% %INC% %COPT% %L% -c JudyLCount.c
echo %CC% %INC% %COPT% %L% -c -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB JudyLByCount.c
%CC% %INC% %COPT% %L% -c -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB JudyLByCount.c
echo %CC% %INC% %COPT% %L% -c JudyLFreeArray.c
%CC% %INC% %COPT% %L% -c JudyLFreeArray.c
echo %CC% %INC% %COPT% %L% -c JudyLMemUsed.c
%CC% %INC% %COPT% %L% -c JudyLMemUsed.c
echo %CC% %INC% %COPT% %L% -c JudyLMemActive.c
%CC% %INC% %COPT% %L% -c JudyLMemActive.c
echo %CC% %INC% %COPT% %L% -c JudyLCascade.c
%CC% %INC% %COPT% %L% -c JudyLCascade.c
echo %CC% %INC% %COPT% %L% -c JudyLDecascade.c
%CC% %INC% %COPT% %L% -c JudyLDecascade.c
echo %CC% %INC% %COPT% %L% -c JudyLCreateBranch.c
%CC% %INC% %COPT% %L% -c JudyLCreateBranch.c
echo %CC% %INC% %COPT% %L% -c JudyLInsertBranch.c
%CC% %INC% %COPT% %L% -c JudyLInsertBranch.c
echo %CC% %INC% %COPT% %L% -c JudyLMallocIF.c
%CC% %INC% %COPT% %L% -c JudyLMallocIF.c

cd ..
cd JudySL
echo Compile the JudySL routine
echo %CC% %INC% %COPT% -c JudySL.c
%CC% %INC% %COPT% -c JudySL.c

cd ..
cd JudyHS
echo Compile the JudyHS routine
echo %CC% %INC% %COPT% -c JudyHS.c
%CC% %INC% %COPT% -c JudyHS.c

cd ..
echo Make a Judy dll by linking all the objects togeather
link /DLL JudyCommon\*.obj Judy1\*.obj JudyL\*.obj JudySL\*.obj JudyHS\*.obj /OUT:Judy.dll

echo Make a Judy archive library by linking all the objects togeather
link /LIB JudyCommon\*.obj Judy1\*.obj JudyL\*.obj JudySL\*.obj JudyHS\*.obj /OUT:Judy.lib
