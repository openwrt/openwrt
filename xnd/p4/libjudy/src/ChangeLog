1.0.5  Version (May 2007) by (twh)
o added proper clean targets to enable multiple builds
o added examples directory
o Correctly Detects 32/64-bit build environment
o Allow explicit configure for 32/64-bit environment

1.0.4  Version (May 2007) by (twh)
o fixed the Checkit problem "error Judy1PrevEmpty Rcode != 1 = 0"
o Fixed memory allignment in JudyMallocIF.c (doug)
o Fixed messages from "make check" (doug).

1.0.3  Version (Feb 2006 ) by (twh)
o fixed make files to break out each copy element
to be a unique target, this also seems to have 
resolved the issue where running make check rebuilds
the entire library again.

1.0.2  Version (Jan 2006 ) by (twh)

o fixed assumption of signed char in test programs.
o updated sh_build
o fixed generation of man pages from html
o fixed 32-bit and 64-bit configure

1.0.1 Version (Dec 2004) by (twh)

o fixed bootstrap to use later versions
o fixed manpage naming from (3X) to (3)
o Code changes to support Microsoft __inline directive
o Move away from using symlinks to using copies
o Added build.bat to support building on Windows

1.0.0 Version (Sept 2004) by (twh)

o Complete Autoconfisication of Judy
o Removed previous build environment
o Change INSTALL_IT back to INSTALL
o Moving to 1.0.0 to denote API change. 


0.1.6 Version (1June2004) by (dlb)

o See src/sh_build in case of 'make' failures
o The is an endian-neutral version I.E. (jp_DcdPop0 deleted)
o Should not require any special platform specific compile flags
o Includes JudyHS*() -- very fast, scalable string version 
o JudyHS*() is still preliminary and may need additional functionality.
o See test/manual/StringCompare.c for comparing different 'string' ADT's
o Deleted files: JudyMalloc.h, JudySL.h, JudySearch*
o All malloc() and free() is done thru interface routines in JudyMalloc.c
o Judy.h should work on all platforms that conform to ISO standards.
o After trying on many platforms, <stdint.h> was changed to <inttypes.h>
o jbgraph has some 'bash/ksh' isms that need to be removed.
o See test/manual/testjbgraph for plotting performance graphs
o 'libtools' stuff is in unknown shape.
o Does not "mangle" the root pointer (so old valgrind is not confused)
o Conform to standard "C"
o Change INSTALL to INSTALL_IT because it confused "make install"
o To he man pages need work to clean up the .html to be portable
o Plus hundreds of changes to make the source more portable.
