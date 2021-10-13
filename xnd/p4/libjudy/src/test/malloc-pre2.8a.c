/*
  [insert usage description etc here]

  preliminary version 2.8.x
*/

/*
  WIN32 sets up defaults for MS environment and compilers.
  Otherwise defaults are for unix.
*/

/* #define WIN32 */

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* No-op failure action */
#define MALLOC_FAILURE_ACTION

/* Win32 doesn't supply or need the following headers */
#define LACKS_UNISTD_H
#define LACKS_SYS_PARAM_H
#define LACKS_SYS_MMAN_H

/* Use the supplied emulation of sbrk */
#define MORECORE sbrk
#define MORECORE_CONTIGUOUS 1
#define MORECORE_FAILURE    ((void*)(-1))

/* Use the supplied emulation of mmap and munmap */
#define HAVE_MMAP 1
#define MUNMAP_FAILURE  (-1)
#define MMAP_CLEARS 1

/* These values don't really matter in windows mmap emulation */
#define MAP_PRIVATE 1
#define MAP_ANONYMOUS 2
#define PROT_READ 1
#define PROT_WRITE 2

/* Emulation functions defined at the end of this file */

/* If USE_MALLOC_LOCK, use supplied critical-section-based lock functions */
#ifdef USE_MALLOC_LOCK
static int slwait(int *sl);
static int slrelease(int *sl);
#endif

static long getpagesize(void);
static long getregionsize(void);
static void *sbrk(long size);
static void *mmap(void *ptr, long size, long prot, long type, long handle, long arg);
static long munmap(void *ptr, long size);

static void vminfo (unsigned long*free, unsigned long*reserved, unsigned long*committed);
static int cpuinfo (int whole, unsigned long*kernel, unsigned long*user);

#endif


#include <strings.h>

/*
  Void_t* is the pointer type that malloc should say it returns
*/

#ifndef Void_t
#define Void_t      void
#endif /*Void_t*/

#include <stddef.h>   /* for size_t */

#ifdef __cplusplus
extern "C" {
#endif

/* define LACKS_UNISTD_H if your system does not have a <unistd.h>. */

/* #define  LACKS_UNISTD_H */

#ifndef LACKS_UNISTD_H
#include <unistd.h>
#endif

/* define LACKS_SYS_PARAM_H if your system does not have a <sys/param.h>. */

/* #define  LACKS_SYS_PARAM_H */


#include <stdio.h>    /* needed for malloc_stats */
#include <errno.h>    /* needed for optional MALLOC_FAILURE_ACTION */


/*
  Debugging:

  Because freed chunks may be overwritten with bookkeeping fields, this
  malloc will often die when freed memory is overwritten by user
  programs.  This can be very effective (albeit in an annoying way)
  in helping track down dangling pointers.

  If you compile with -DDEBUG, a number of assertion checks are
  enabled that will catch more memory errors. You probably won't be
  able to make much sense of the actual assertion errors, but they
  should help you locate incorrectly overwritten memory.  The
  checking is fairly extensive, and will slow down execution
  noticeably. Calling malloc_stats or mallinfo with DEBUG set will
  attempt to check every non-mmapped allocated and free chunk in the
  course of computing the summmaries. (By nature, mmapped regions
  cannot be checked very much automatically.)

  Setting DEBUG may also be helpful if you are trying to modify
  this code. The assertions in the check routines spell out in more
  detail the assumptions and invariants underlying the algorithms.

  Setting DEBUG does NOT provide an automated mechanism for checking
  that all accesses to malloced memory stay within their
  bounds. However, there are several add-ons and adaptations of this
  or other mallocs available that do this.
*/

#if DEBUG
#include <assert.h>
/* #define assert(x) if(!(x)) abort() */ 

#else
#define assert(x) ((void)0)
#endif

/*
  The unsigned integer type used for comparing any two chunk sizes.
  This should be at least as wide as size_t, but should not be signed.
*/

#ifndef CHUNK_SIZE_T
#define CHUNK_SIZE_T unsigned long
#endif

#define MAX_CHUNK_SIZE  ((CHUNK_SIZE_T)(-1UL))

/* 
  The unsigned integer type used to hold addresses when they are are
  manipulated as integers. Except that it is not defined on all
  systems, intptr_t would suffice.
*/
#ifndef PTR_UINT
#define PTR_UINT unsigned long
#endif


/*
  INTERNAL_SIZE_T is the word-size used for internal bookkeeping
  of chunk sizes.

  The default version is the same as size_t.

  While not strictly necessary, it is best to define this as an
  unsigned type, even if size_t is a signed type. This may avoid some
  artificial size limitations on some systems.

  On a 64-bit machine, you may be able to reduce malloc overhead by
  defining INTERNAL_SIZE_T to be a 32 bit `unsigned int' at the
  expense of not being able to handle more than 2^32 of malloced
  space. If this limitation is acceptable, you are encouraged to set
  this unless you are on a platform requiring 16byte alignments. In
  this case the alignment requirements turn out to negate any
  potential advantages of decreasing size_t word size.

  Implementors: Beware of the possible combinations of:
     - INTERNAL_SIZE_T might be signed or unsigned, might be 32 or 64 bits,
       and might be the same width as int or as long
     - size_t might have different width and signedness as INTERNAL_SIZE_T
     - int and long might be 32 or 64 bits, and might be the same width
  To deal with this, most comparisons and difference computations
  among INTERNAL_SIZE_Ts should cast them to CHUNK_SIZE_T, being
  aware of the fact that casting an unsigned int to a wider long does
  not sign-extend. (This also makes checking for negative numbers
  awkward.) Some of these casts result in harmless compiler warnings
  on some systems.
*/

#ifndef INTERNAL_SIZE_T
#define INTERNAL_SIZE_T size_t
#endif

/* The corresponding word size */
#define SIZE_SZ                (sizeof(INTERNAL_SIZE_T))



/*
  MALLOC_ALIGNMENT is the minimum alignment for malloc'ed chunks.
  It must be a power of two at least 2 * SIZE_SZ, even on machines
  for which smaller alignments would suffice. It may be defined as
  larger than this though. Note however that code and data structures
  are optimized for the case of 8-byte alignment.
*/


#ifndef MALLOC_ALIGNMENT
#define MALLOC_ALIGNMENT       (2 * SIZE_SZ)
#endif

/* The corresponding bit mask value */
#define MALLOC_ALIGN_MASK      (MALLOC_ALIGNMENT - 1)



/*
  REALLOC_ZERO_BYTES_FREES should be set if a call to
  realloc with zero bytes should be the same as a call to free.
  Some people think it should. Otherwise, since this malloc
  returns a unique pointer for malloc(0), so does realloc(p, 0).
*/

/*   #define REALLOC_ZERO_BYTES_FREES */

/*
  TRIM_FASTBINS controls whether free() of a very small chunk can
  immediately lead to trimming. Setting to true (1) can reduce memory
  footprint, but will almost always slow down programs that use a lot
  of small chunks.

  Define this only if you are willing to give up some speed to more
  aggressively reduce system-level memory footprint when releasing
  memory in programs that use many small chunks.  You can get
  essentially the same effect by setting MXFAST to 0, but this can
  lead to even greater slowdowns in programs using many small chunks.
  TRIM_FASTBINS is an in-between compile-time option, that disables
  only those chunks bordering topmost memory from being placed in
  fastbins.
*/

#ifndef TRIM_FASTBINS
#define TRIM_FASTBINS  0
#endif


/*
  USE_DL_PREFIX will prefix all public routines with the string 'dl'.
  This is necessary when you only want to use this malloc in one part 
  of a program, using your regular system malloc elsewhere.
*/

/* #define USE_DL_PREFIX */


/*
  USE_MALLOC_LOCK causes wrapper functions to surround each
  callable routine with pthread mutex lock/unlock.

  USE_MALLOC_LOCK forces USE_PUBLIC_MALLOC_WRAPPERS to be defined
*/

/* #define USE_MALLOC_LOCK */


/*
  If USE_PUBLIC_MALLOC_WRAPPERS is defined, every public routine is
  actually a wrapper function that first calls MALLOC_PREACTION, then
  calls the internal routine, and follows it with
  MALLOC_POSTACTION. This is needed for locking, but you can also use
  this, without USE_MALLOC_LOCK, for purposes of interception,
  instrumentation, etc. It is a sad fact that using wrappers often
  noticeably degrades performance of malloc-intensive programs.
*/

#ifdef USE_MALLOC_LOCK
#define USE_PUBLIC_MALLOC_WRAPPERS
#else
/* #define USE_PUBLIC_MALLOC_WRAPPERS */
#endif


/* 
   Two-phase name translation.
   All of the actual routines are given mangled names.
   When wrappers are used, they become the public callable versions.
   When DL_PREFIX is used, the callable names are prefixed.
*/

#ifndef USE_PUBLIC_MALLOC_WRAPPERS
#define cALLOc      public_cALLOc
#define fREe        public_fREe
#define cFREe       public_cFREe
#define mALLOc      public_mALLOc
#define mEMALIGn    public_mEMALIGn
#define rEALLOc     public_rEALLOc
#define vALLOc      public_vALLOc
#define pVALLOc     public_pVALLOc
#define mALLINFo    public_mALLINFo
#define mALLOPt     public_mALLOPt
#define mTRIm       public_mTRIm
#define mSTATs      public_mSTATs
#define mUSABLe     public_mUSABLe
#define iCALLOc     public_iCALLOc
#define iCOMALLOc   public_iCOMALLOc
#endif

#ifdef USE_DL_PREFIX
#define public_cALLOc    dlcalloc
#define public_fREe      dlfree
#define public_cFREe     dlcfree
#define public_mALLOc    dlmalloc
#define public_mEMALIGn  dlmemalign
#define public_rEALLOc   dlrealloc
#define public_vALLOc    dlvalloc
#define public_pVALLOc   dlpvalloc
#define public_mALLINFo  dlmallinfo
#define public_mALLOPt   dlmallopt
#define public_mTRIm     dlmalloc_trim
#define public_mSTATs    dlmalloc_stats
#define public_mUSABLe   dlmalloc_usable_size
#define public_iCALLOc   dlindependent_calloc
#define public_iCOMALLOc dlindependent_comalloc
#else /* USE_DL_PREFIX */
#define public_cALLOc    calloc
#define public_fREe      free
#define public_cFREe     cfree
#define public_mALLOc    malloc
#define public_mEMALIGn  memalign
#define public_rEALLOc   realloc
#define public_vALLOc    valloc
#define public_pVALLOc   pvalloc
#define public_mALLINFo  mallinfo
#define public_mALLOPt   mallopt
#define public_mTRIm     malloc_trim
#define public_mSTATs    malloc_stats
#define public_mUSABLe   malloc_usable_size
#define public_iCALLOc   independent_calloc
#define public_iCOMALLOc independent_comalloc
#endif /* USE_DL_PREFIX */


/*
  HAVE_MEMCPY should be defined if you are not otherwise using
  ANSI STD C, but still have memcpy and memset in your C library
  and want to use them in calloc and realloc. Otherwise simple
  macro versions are defined below.

  USE_MEMCPY should be defined as 1 if you actually want to
  have memset and memcpy called. People report that the macro
  versions are faster than libc versions on some systems.
  
  Even if USE_MEMCPY is set to 1, loops to copy/clear small chunks
  (of <= 36 bytes) are manually unrolled in realloc and calloc.
*/

#define HAVE_MEMCPY

#ifndef USE_MEMCPY
#ifdef HAVE_MEMCPY
#define USE_MEMCPY 1
#else
#define USE_MEMCPY 0
#endif
#endif


#if (defined(HAVE_MEMCPY))
#ifdef WIN32
/* On Win32 memset and memcpy are already declared in windows.h */
#else
void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);
#endif
#endif

/*
  MALLOC_FAILURE_ACTION is the action to take before "return 0" when
  malloc fails to be able to return memory, either because memory is
  exhausted or because of illegal arguments.
  
  By default, sets errno if running on STD_C platform, else does nothing.  
*/

#ifndef MALLOC_FAILURE_ACTION
#define MALLOC_FAILURE_ACTION \
   errno = ENOMEM;
#endif

/*
  MORECORE-related declarations. By default, rely on sbrk
*/


#ifdef LACKS_UNISTD_H
#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
extern Void_t*     sbrk(ptrdiff_t);
#endif
#endif

/*
  MORECORE is the name of the routine to call to obtain more memory
  from the system.  See below for general guidance on writing
  alternative MORECORE functions, as well as a version for WIN32 and a
  sample version for pre-OSX macos.
*/

#ifndef MORECORE
#define MORECORE sbrk
#endif

/*
  MORECORE_FAILURE is the value returned upon failure of MORECORE
  as well as mmap. Since it cannot be an otherwise valid memory address,
  and must reflect values of standard sys calls, you probably ought not
  try to redefine it.
*/

#ifndef MORECORE_FAILURE
#define MORECORE_FAILURE (-1)
#endif

/*
  If MORECORE_CONTIGUOUS is true, take advantage of fact that
  consecutive calls to MORECORE with positive arguments always return
  contiguous increasing addresses.  This is true of unix sbrk.  Even
  if not defined, when regions happen to be contiguous, malloc will
  permit allocations spanning regions obtained from different
  calls. But defining this when applicable enables some stronger
  consistency checks and space efficiencies. 
*/

#ifndef MORECORE_CONTIGUOUS
#define MORECORE_CONTIGUOUS 1
#endif

/*
  Define MORECORE_CANNOT_TRIM if your version of MORECORE
  cannot release space back to the system when given negative
  arguments. This is generally necessary only if you are using
  a hand-crafted MORECORE function that cannot handle negative arguments.
*/

/* #define MORECORE_CANNOT_TRIM */


/*
  Define HAVE_MMAP as true to optionally make malloc() use mmap() to
  allocate very large blocks.  These will be returned to the
  operating system immediately after a free(). Also, if mmap
  is available, it is used as a backup strategy in cases where
  MORECORE fails to provide space from system.

  This malloc is best tuned to work with mmap for large requests.
  If you do not have mmap, operations involving very large chunks (1MB
  or so) may be slower than you'd like.
*/

#define HAVE_MMAP 0

#ifndef HAVE_MMAP
#define HAVE_MMAP 1
#endif

#if HAVE_MMAP
/* 
   Standard unix mmap using /dev/zero clears memory so calloc doesn't
   need to.
*/

#ifndef MMAP_CLEARS
#define MMAP_CLEARS 1
#endif

#else /* no mmap */
#ifndef MMAP_CLEARS
#define MMAP_CLEARS 0
#endif
#endif


/* 
   MMAP_AS_MORECORE_SIZE is the minimum mmap size argument to use if
   sbrk fails, and mmap is used as a backup (which is done only if
   HAVE_MMAP).  The value must be a multiple of page size.  This
   backup strategy generally applies only when systems have "holes" in
   address space, so sbrk cannot perform contiguous expansion, but
   there is still space available on system.  On systems for which
   this is known to be useful (i.e. most linux kernels), this occurs
   only when programs allocate huge amounts of memory.  Between this,
   and the fact that mmap regions tend to be limited, the size should
   be large, to avoid too many mmap calls and thus avoid running out
   of kernel resources.
*/

#ifndef MMAP_AS_MORECORE_SIZE
#define MMAP_AS_MORECORE_SIZE (1024 * 1024)
#endif

/*
  Define HAVE_MREMAP to make realloc() use mremap() to re-allocate
  large blocks.  This is currently only possible on Linux with
  kernel versions newer than 1.3.77.
*/

#ifndef HAVE_MREMAP
#ifdef linux
#define HAVE_MREMAP 1
#else
#define HAVE_MREMAP 0
#endif

#endif /* HAVE_MMAP */


/*
  The system page size. To the extent possible, this malloc manages
  memory from the system in page-size units.  Note that this value is
  cached during initialization into a field of malloc_state. So even
  if malloc_getpagesize is a function, it is only called once.

  The following mechanics for getpagesize were adapted from bsd/gnu
  getpagesize.h. If none of the system-probes here apply, a value of
  4096 is used, which should be OK: If they don't apply, then using
  the actual value probably doesn't impact performance.
*/

#ifndef malloc_getpagesize

#ifndef LACKS_UNISTD_H
#  include <unistd.h>
#endif

#  ifdef _SC_PAGESIZE         /* some SVR4 systems omit an underscore */
#    ifndef _SC_PAGE_SIZE
#      define _SC_PAGE_SIZE _SC_PAGESIZE
#    endif
#  endif

#  ifdef _SC_PAGE_SIZE
#    define malloc_getpagesize sysconf(_SC_PAGE_SIZE)
#  else
#    if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
       extern size_t getpagesize();
#      define malloc_getpagesize getpagesize()
#    else
#      ifdef WIN32 /* use supplied emulation of getpagesize */
#        define malloc_getpagesize getpagesize() 
#      else
#        ifndef LACKS_SYS_PARAM_H
#          include <sys/param.h>
#        endif
#        ifdef EXEC_PAGESIZE
#          define malloc_getpagesize EXEC_PAGESIZE
#        else
#          ifdef NBPG
#            ifndef CLSIZE
#              define malloc_getpagesize NBPG
#            else
#              define malloc_getpagesize (NBPG * CLSIZE)
#            endif
#          else
#            ifdef NBPC
#              define malloc_getpagesize NBPC
#            else
#              ifdef PAGESIZE
#                define malloc_getpagesize PAGESIZE
#              else /* just guess */
#                define malloc_getpagesize (4096) 
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

/*
  This version of malloc supports the standard SVID/XPG mallinfo
  routine that returns a struct containing usage properties and
  statistics. It should work on any SVID/XPG compliant system that has
  a /usr/include/malloc.h defining struct mallinfo. (If you'd like to
  install such a thing yourself, cut out the preliminary declarations
  as described above and below and save them in a malloc.h file. But
  there's no compelling reason to bother to do this.)

  The main declaration needed is the mallinfo struct that is returned
  (by-copy) by mallinfo().  The SVID/XPG malloinfo struct contains a
  bunch of fields that are not even meaningful in this version of
  malloc.  These fields are are instead filled by mallinfo() with
  other numbers that might be of interest.

  HAVE_USR_INCLUDE_MALLOC_H should be set if you have a
  /usr/include/malloc.h file that includes a declaration of struct
  mallinfo.  If so, it is included; else an SVID2/XPG2 compliant
  version is declared below.  These must be precisely the same for
  mallinfo() to work.  The original SVID version of this struct,
  defined on most systems with mallinfo, declares all fields as
  ints. But some others define as unsigned long. If your system
  defines the fields using a type of different width than listed here,
  you must #include your system version and #define
  HAVE_USR_INCLUDE_MALLOC_H.
*/

/* #define HAVE_USR_INCLUDE_MALLOC_H */

#ifdef HAVE_USR_INCLUDE_MALLOC_H
#include "/usr/include/malloc.h"
#else

/* SVID2/XPG mallinfo structure */

struct mallinfo {
  int arena;    /* non-mmapped space allocated from system */
  int ordblks;  /* number of free chunks */
  int smblks;   /* number of fastbin blocks */
  int hblks;    /* number of mmapped regions */
  int hblkhd;   /* space in mmapped regions */
  int usmblks;  /* maximum total allocated space */
  int fsmblks;  /* space available in freed fastbin blocks */
  int uordblks; /* total allocated space */
  int fordblks; /* total free space */
  int keepcost; /* top-most, releasable (via malloc_trim) space */
};

/*
  SVID/XPG defines four standard parameter numbers for mallopt,
  normally defined in malloc.h.  Only one of these (M_MXFAST) is used
  in this malloc. The others (M_NLBLKS, M_GRAIN, M_KEEP) don't apply,
  so setting them has no effect. But this malloc also supports other
  options in mallopt described below.
*/
#endif


/* ---------- description of public routines ------------ */

/*
  malloc(size_t n)
  Returns a pointer to a newly allocated chunk of at least n bytes, or null
  if no space is available. Additionally, on failure, errno is
  set to ENOMEM on ANSI C systems.

  If n is zero, malloc returns a minumum-sized chunk. (The minimum
  size is 16 bytes on most 32bit systems, and 24 or 32 bytes on 64bit
  systems.)  On most systems, size_t is an unsigned type, so calls
  with negative arguments are interpreted as requests for huge amounts
  of space, which will often fail. The maximum supported value of n
  differs across systems, but is in all cases less than the maximum
  representable value of a size_t.
*/
Void_t*  public_mALLOc(size_t);

/*
  free(Void_t* p)
  Releases the chunk of memory pointed to by p, that had been previously
  allocated using malloc or a related routine such as realloc.
  It has no effect if p is null. It can have arbitrary (i.e., bad!)
  effects if p has already been freed.

  Unless disabled (using mallopt), freeing very large spaces will
  when possible, automatically trigger operations that give
  back unused memory to the system, thus reducing program footprint.
*/
void     public_fREe(Void_t*);

/*
  calloc(size_t n_elements, size_t element_size);
  Returns a pointer to n_elements * element_size bytes, with all locations
  set to zero.
*/
Void_t*  public_cALLOc(size_t, size_t);

/*
  realloc(Void_t* p, size_t n)
  Returns a pointer to a chunk of size n that contains the same data
  as does chunk p up to the minimum of (n, p's size) bytes, or null
  if no space is available. 

  The returned pointer may or may not be the same as p. The algorithm
  prefers extending p when possible, otherwise it employs the
  equivalent of a malloc-copy-free sequence.

  If p is null, realloc is equivalent to malloc.  

  If space is not available, realloc returns null, errno is set (if on
  ANSI) and p is NOT freed.

  if n is for fewer bytes than already held by p, the newly unused
  space is lopped off and freed if possible.  Unless the #define
  REALLOC_ZERO_BYTES_FREES is set, realloc with a size argument of
  zero (re)allocates a minimum-sized chunk.

  Large chunks that were internally obtained via mmap will always
  be reallocated using malloc-copy-free sequences unless
  the system supports MREMAP (currently only linux).

  The old unix realloc convention of allowing the last-free'd chunk
  to be used as an argument to realloc is not supported.
*/

Void_t*  public_rEALLOc(Void_t*, size_t);

/*
  memalign(size_t alignment, size_t n);
  Returns a pointer to a newly allocated chunk of n bytes, aligned
  in accord with the alignment argument.

  The alignment argument should be a power of two. If the argument is
  not a power of two, the nearest greater power is used.
  8-byte alignment is guaranteed by normal malloc calls, so don't
  bother calling memalign with an argument of 8 or less.

  Overreliance on memalign is a sure way to fragment space.
*/
Void_t*  public_mEMALIGn(size_t, size_t);

/*
  valloc(size_t n);
  Equivalent to memalign(pagesize, n), where pagesize is the page
  size of the system. If the pagesize is unknown, 4096 is used.
*/
Void_t*  public_vALLOc(size_t);


/*
  mallopt(int parameter_number, int parameter_value)
  Sets tunable parameters The format is to provide a
  (parameter-number, parameter-value) pair.  mallopt then sets the
  corresponding parameter to the argument value if it can (i.e., so
  long as the value is meaningful), and returns 1 if successful else
  0.  SVID/XPG/ANSI defines four standard param numbers for mallopt,
  normally defined in malloc.h.  Only one of these (M_MXFAST) is used
  in this malloc. The others (M_NLBLKS, M_GRAIN, M_KEEP) don't apply,
  so setting them has no effect. But this malloc also supports four
  other options in mallopt. See below for details.  Briefly, supported
  parameters are as follows (listed defaults are for "typical"
  configurations).

  Symbol            param #   default    allowed param values
  M_MXFAST          1         64         0-64  (0 disables fastbins)
  M_TRIM_THRESHOLD -1         256*1024   any   (-1U disables trimming)
  M_TOP_PAD        -2         0          any  
  M_MMAP_THRESHOLD -3         256*1024   any   (or 0 if no MMAP support)
  M_MMAP_MAX       -4         65536      any   (0 disables use of mmap)
*/
int      public_mALLOPt(int, int);


/*
  mallinfo()
  Returns (by copy) a struct containing various summary statistics:

  arena:     current total non-mmapped bytes allocated from system 
  ordblks:   the number of free chunks 
  smblks:    the number of fastbin blocks (i.e., small chunks that
               have been freed but not use resused or consolidated)
  hblks:     current number of mmapped regions 
  hblkhd:    total bytes held in mmapped regions 
  usmblks:   the maximum total allocated space. This will be greater
                than current total if trimming has occurred.
  fsmblks:   total bytes held in fastbin blocks 
  uordblks:  current total allocated space (normal or mmapped)
  fordblks:  total free space 
  keepcost:  the maximum number of bytes that could ideally be released
               back to system via malloc_trim. ("ideally" means that
               it ignores page restrictions etc.)

  Because these fields are ints, but internal bookkeeping may
  be kept as longs, the reported values may wrap around zero and 
  thus be inaccurate.
*/
struct mallinfo public_mALLINFo(void);

/*
  independent_calloc(size_t n_elements, size_t element_size, Void_t* chunks[]);

  independent_calloc is similar to calloc, but instead of returning a
  single cleared space, it returns an array of pointers to n_elements
  independent elements that can hold contents of size elem_size, each
  of which starts out cleared, and can be independently freed,
  realloc'ed etc. The elements are guaranteed to be adjacently
  allocated (this is not guaranteed to occur with multiple callocs or
  mallocs), which may also improve cache locality in some
  applications.

  The "chunks" argument is optional (i.e., may be null, which is
  probably the most typical usage). If it is null, the returned array
  is itself dynamically allocated and should also be freed when it is
  no longer needed. Otherwise, the chunks array must be of at least
  n_elements in length. It is filled in with the pointers to the
  chunks.

  In either case, independent_calloc returns this pointer array, or
  null if the allocation failed.  If n_elements is zero and "chunks"
  is null, it returns a chunk representing an array with zero elements
  (which should be freed if not wanted).

  Each element must be individually freed when it is no longer
  needed. If you'd like to instead be able to free all at once, you
  should instead use regular calloc and assign pointers into this
  space to represent elements.  (In this case though, you cannot
  independently free elements.)
  
  independent_calloc simplifies and speeds up implementations of many
  kinds of pools.  It may also be useful when constructing large data
  structures that initially have a fixed number of fixed-sized nodes,
  but the number is not known at compile time, and some of the nodes
  may later need to be freed. For example:

  struct Node { int item; struct Node* next; };
  
  struct Node* build_list() {
    struct Node** pool;
    int n = read_number_of_nodes_needed();
    if (n <= 0) return 0;
    pool = (struct Node**)(independent_calloc(n, sizeof(struct Node), 0);
    if (pool == 0) die(); 
    // organize into a linked list... 
    struct Node* first = pool[0];
    for (i = 0; i < n-1; ++i) 
      pool[i]->next = pool[i+1];
    free(pool);     // Can now free the array (or not, if it is needed later)
    return first;
  }
*/
Void_t** public_iCALLOc(size_t, size_t, Void_t**);

/*
  independent_comalloc(size_t n_elements, size_t sizes[], Void_t* chunks[]);

  independent_comalloc allocates, all at once, a set of n_elements
  chunks with sizes indicated in the "sizes" array.    It returns
  an array of pointers to these elements, each of which can be
  independently freed, realloc'ed etc. The elements are guaranteed to
  be adjacently allocated (this is not guaranteed to occur with
  multiple callocs or mallocs), which may also improve cache locality
  in some applications.

  The "chunks" argument is optional (i.e., may be null). If it is null
  the returned array is itself dynamically allocated and should also
  be freed when it is no longer needed. Otherwise, the chunks array
  must be of at least n_elements in length. It is filled in with the
  pointers to the chunks.

  In either case, independent_comalloc returns this pointer array, or
  null if the allocation failed.  If n_elements is zero and chunks is
  null, it returns a chunk representing an array with zero elements
  (which should be freed if not wanted).
  
  Each element must be individually freed when it is no longer
  needed. If you'd like to instead be able to free all at once, you
  should instead use a single regular malloc, and assign pointers at
  particular offsets in the aggregate space. (In this case though, you 
  cannot independently free elements.)

  independent_comallac differs from independent_calloc in that each
  element may have a different size, and also that it does not
  automatically clear elements.

  independent_comalloc can be used to speed up allocation in cases
  where several structs or objects must always be allocated at the
  same time.  For example:

  struct Head { ... }
  struct Foot { ... }

  void send_message(char* msg) {
    int msglen = strlen(msg);
    size_t sizes[3] = { sizeof(struct Head), msglen, sizeof(struct Foot) };
    void* chunks[3];
    if (independent_comalloc(3, sizes, chunks) == 0)
      die();
    struct Head* head = (struct Head*)(chunks[0]);
    char*        body = (char*)(chunks[1]);
    struct Foot* foot = (struct Foot*)(chunks[2]);
    // ...
  }

  In general though, independent_comalloc is worth using only for
  larger values of n_elements. For small values, you probably won't
  detect enough difference from series of malloc calls to bother.

  Overuse of independent_comalloc can increase overall memory usage,
  since it cannot reuse existing noncontiguous small chunks that
  might be available for some of the elements.
*/
Void_t** public_iCOMALLOc(size_t, size_t*, Void_t**);


/*
  pvalloc(size_t n);
  Equivalent to valloc(minimum-page-that-holds(n)), that is,
  round up n to nearest pagesize.
 */
Void_t*  public_pVALLOc(size_t);

/*
  cfree(Void_t* p);
  Equivalent to free(p).

  cfree is needed/defined on some systems that pair it with calloc,
  for odd historical reasons (such as: cfree is used in example 
  code in the first edition of K&R).
*/
void     public_cFREe(Void_t*);

/*
  malloc_trim(size_t pad);

  If possible, gives memory back to the system (via negative
  arguments to sbrk) if there is unused memory at the `high' end of
  the malloc pool. You can call this after freeing large blocks of
  memory to potentially reduce the system-level memory requirements
  of a program. However, it cannot guarantee to reduce memory. Under
  some allocation patterns, some large free blocks of memory will be
  locked between two used chunks, so they cannot be given back to
  the system.
  
  The `pad' argument to malloc_trim represents the amount of free
  trailing space to leave untrimmed. If this argument is zero,
  only the minimum amount of memory to maintain internal data
  structures will be left (one page or less). Non-zero arguments
  can be supplied to maintain enough trailing space to service
  future expected allocations without having to re-obtain memory
  from the system.
  
  Malloc_trim returns 1 if it actually released any memory, else 0.
  On systems that do not support "negative sbrks", it will always
  rreturn 0.
*/
int      public_mTRIm(size_t);

/*
  malloc_usable_size(Void_t* p);

  Returns the number of bytes you can actually use in
  an allocated chunk, which may be more than you requested (although
  often not) due to alignment and minimum size constraints.
  You can use this many bytes without worrying about
  overwriting other allocated objects. This is not a particularly great
  programming practice. malloc_usable_size can be more useful in
  debugging and assertions, for example:

  p = malloc(n);
  assert(malloc_usable_size(p) >= 256);

*/
size_t   public_mUSABLe(Void_t*);

/*
  malloc_stats();
  Prints on stderr the amount of space obtained from the system (both
  via sbrk and mmap), the maximum amount (which may be more than
  current if malloc_trim and/or munmap got called), and the current
  number of bytes allocated via malloc (or realloc, etc) but not yet
  freed. Note that this is the number of bytes allocated, not the
  number requested. It will be larger than the number requested
  because of alignment and bookkeeping overhead. Because it includes
  alignment wastage as being in use, this figure may be greater than
  zero even when no user-level chunks are allocated.

  The reported current and maximum system memory can be inaccurate if
  a program makes other calls to system memory allocation functions
  (normally sbrk) outside of malloc.

  malloc_stats prints only the most commonly interesting statistics.
  More information can be obtained by calling mallinfo.

*/
void     public_mSTATs();

/* mallopt tuning options */

/*
  M_MXFAST is the maximum request size used for "fastbins", special bins
  that hold returned chunks without consolidating their spaces. This
  enables future requests for chunks of the same size to be handled
  very quickly, but can increase fragmentation, and thus increase the
  overall memory footprint of a program.

  This malloc manages fastbins very conservatively yet still
  efficiently, so fragmentation is rarely a problem for values less
  than or equal to the default.  The maximum supported value of MXFAST
  is 64 (also the default). You wouldn't want it any higher than this
  anyway.  Fastbins are designed especially for use with many small
  structs, objects or strings -- the default handles
  structs/objects/arrays with sizes up to 16 4byte fields, or small
  strings representing words, tokens, etc. Using fastbins for larger
  objects normally worsens fragmentation without improving speed.

  M_MXFAST is set in REQUEST size units. It is internally used in
  chunksize units, which adds padding and alignment.  You can reduce
  M_MXFAST to 0 to disable all use of fastbins.  This causes the malloc
  algorithm to be a closer approximation of fifo-best-fit in all cases,
  not just for larger requests, but will generally cause it to be
  slower.
*/


/* M_MXFAST is a standard SVID/XPG tuning option, usually listed in malloc.h */
#ifndef M_MXFAST
#define M_MXFAST            1    
#endif

#ifndef DEFAULT_MXFAST
#define DEFAULT_MXFAST     64
#endif


/*
  M_TRIM_THRESHOLD is the maximum amount of unused top-most memory
  to keep before releasing via malloc_trim in free().

  Automatic trimming is mainly useful in long-lived programs.
  Because trimming via sbrk can be slow on some systems, and can
  sometimes be wasteful (in cases where programs immediately
  afterward allocate more large chunks) the value should be high
  enough so that your overall system performance would improve by
  releasing this much memory.

  The trim threshold and the mmap control parameters (see below)
  can be traded off with one another. Trimming and mmapping are
  two different ways of releasing unused memory back to the
  system. Between these two, it is often possible to keep
  system-level demands of a long-lived program down to a bare
  minimum. For example, in one test suite of sessions measuring
  the XF86 X server on Linux, using a trim threshold of 128K and a
  mmap threshold of 192K led to near-minimal long term resource
  consumption.

  If you are using this malloc in a long-lived program, it should
  pay to experiment with these values.  As a rough guide, you
  might set to a value close to the average size of a process
  (program) running on your system.  Releasing this much memory
  would allow such a process to run in memory.  Generally, it's
  worth it to tune for trimming rather tham memory mapping when a
  program undergoes phases where several large chunks are
  allocated and released in ways that can reuse each other's
  storage, perhaps mixed with phases where there are no such
  chunks at all.  And in well-behaved long-lived programs,
  controlling release of large blocks via trimming versus mapping
  is usually faster.

  However, in most programs, these parameters serve mainly as
  protection against the system-level effects of carrying around
  massive amounts of unneeded memory. Since frequent calls to
  sbrk, mmap, and munmap otherwise degrade performance, the default
  parameters are set to relatively high values that serve only as
  safeguards.

  The trim value must be greater than page size to have any useful
  effect.  To disable trimming completely, you can set to 
  (unsigned long)(-1)

  Trim settings interact with fastbin (MXFAST) settings: Unless
  TRIM_FASTBINS is defined, automatic trimming never takes place upon
  freeing a chunk with size less than or equal to MXFAST. Trimming is
  instead delayed until subsequent freeing of larger chunks. However,
  you can still force an attempted trim by calling malloc_trim.

  Also, trimming is not generally possible in cases where
  the main arena is obtained via mmap.

  Note that the trick some people use of mallocing a huge space and
  then freeing it at program startup, in an attempt to reserve system
  memory, doesn't have the intended effect under automatic trimming,
  since that memory will immediately be returned to the system.
*/

#define M_TRIM_THRESHOLD       -1

#ifndef DEFAULT_TRIM_THRESHOLD

#define DEFAULT_TRIM_THRESHOLD (-1U)
/* #define DEFAULT_TRIM_THRESHOLD (256 * 1024) */
#endif

/*
  M_TOP_PAD is the amount of extra `padding' space to allocate or
  retain whenever sbrk is called. It is used in two ways internally:

  * When sbrk is called to extend the top of the arena to satisfy
  a new malloc request, this much padding is added to the sbrk
  request.

  * When malloc_trim is called automatically from free(),
  it is used as the `pad' argument.

  In both cases, the actual amount of padding is rounded
  so that the end of the arena is always a system page boundary.

  The main reason for using padding is to avoid calling sbrk so
  often. Having even a small pad greatly reduces the likelihood
  that nearly every malloc request during program start-up (or
  after trimming) will invoke sbrk, which needlessly wastes
  time.

  Automatic rounding-up to page-size units is normally sufficient
  to avoid measurable overhead, so the default is 0.  However, in
  systems where sbrk is relatively slow, it can pay to increase
  this value, at the expense of carrying around more memory than
  the program needs.
*/

#define M_TOP_PAD              -2

#ifndef DEFAULT_TOP_PAD
#define DEFAULT_TOP_PAD        (0)
#endif

/*
  M_MMAP_THRESHOLD is the request size threshold for using mmap()
  to service a request. Requests of at least this size that cannot
  be allocated using already-existing space will be serviced via mmap.
  (If enough normal freed space already exists it is used instead.)

  Using mmap segregates relatively large chunks of memory so that
  they can be individually obtained and released from the host
  system. A request serviced through mmap is never reused by any
  other request (at least not directly; the system may just so
  happen to remap successive requests to the same locations).

  Segregating space in this way has the benefits that:

   1. Mmapped space can ALWAYS be individually released back 
      to the system, which helps keep the system level memory 
      demands of a long-lived program low. 
   2. Mapped memory can never become `locked' between
      other chunks, as can happen with normally allocated chunks, which
      means that even trimming via malloc_trim would not release them.
   3. On some systems with "holes" in address spaces, mmap can obtain
      memory that sbrk cannot.

  However, it has the disadvantages that:

   1. The space cannot be reclaimed, consolidated, and then
      used to service later requests, as happens with normal chunks.
   2. It can lead to more wastage because of mmap page alignment
      requirements
   3. It causes malloc performance to be more dependent on host
      system memory management support routines which may vary in
      implementation quality and may impose arbitrary
      limitations. Generally, servicing a request via normal
      malloc steps is faster than going through a system's mmap.

  The advantages of mmap nearly always outweigh disadvantages for
  "large" chunks, but the value of "large" varies across systems.  The
  default is an empirically derived value that works well in most
  systems.
*/

#define M_MMAP_THRESHOLD      -3

#ifndef DEFAULT_MMAP_THRESHOLD
#define DEFAULT_MMAP_THRESHOLD (-1U) 
/*#define DEFAULT_MMAP_THRESHOLD (128 * 1024) */
#endif

/*
  M_MMAP_MAX is the maximum number of requests to simultaneously
  service using mmap. This parameter exists because
. Some systems have a limited number of internal tables for
  use by mmap, and using more than a few of them may degrade
  performance.

  The default is set to a value that serves only as a safeguard.
  Setting to 0 disables use of mmap for servicing large requests.  If
  HAVE_MMAP is not set, the default value is 0, and attempts to set it
  to non-zero values in mallopt will fail.
*/

#define M_MMAP_MAX             -4

#ifndef DEFAULT_MMAP_MAX
#if HAVE_MMAP
#define DEFAULT_MMAP_MAX       (65536)
#else
#define DEFAULT_MMAP_MAX       (0)
#endif
#endif

#ifdef __cplusplus
};  /* end of extern "C" */
#endif

/* 
  ========================================================================
  To make a fully customizable malloc.h header file, cut everything
  above this line, put into file malloc.h, edit to suit, and #include it 
  on the next line, as well as in programs that use this malloc.
  ========================================================================
*/

/* #include "malloc.h" */

/* --------------------- public wrappers ---------------------- */

#ifdef USE_PUBLIC_MALLOC_WRAPPERS

/* Declare all routines as internal */
static Void_t*  mALLOc(size_t);
static void     fREe(Void_t*);
static Void_t*  rEALLOc(Void_t*, size_t);
static Void_t*  mEMALIGn(size_t, size_t);
static Void_t*  vALLOc(size_t);
static Void_t*  pVALLOc(size_t);
static Void_t*  cALLOc(size_t, size_t);
static Void_t** iCALLOc(size_t, size_t, Void_t**);
static Void_t** iCOMALLOc(size_t, size_t*, Void_t**);
static void     cFREe(Void_t*);
static int      mTRIm(size_t);
static size_t   mUSABLe(Void_t*);
static void     mSTATs();
static int      mALLOPt(int, int);
static struct mallinfo mALLINFo(void);

/*
  MALLOC_PREACTION and MALLOC_POSTACTION should be
  defined to return 0 on success, and nonzero on failure.
  The return value of MALLOC_POSTACTION is currently ignored
  in wrapper functions since there is no reasonable default
  action to take on failure.
*/


#ifdef USE_MALLOC_LOCK

#ifdef WIN32

static int mALLOC_MUTEx;
#define MALLOC_PREACTION   slwait(&mALLOC_MUTEx)
#define MALLOC_POSTACTION  slrelease(&mALLOC_MUTEx)

#else

#include <pthread.h>

static pthread_mutex_t mALLOC_MUTEx = PTHREAD_MUTEX_INITIALIZER;

#define MALLOC_PREACTION   pthread_mutex_lock(&mALLOC_MUTEx)
#define MALLOC_POSTACTION  pthread_mutex_unlock(&mALLOC_MUTEx)

#endif /* USE_MALLOC_LOCK */

#else

/* Substitute anything you like for these */

#define MALLOC_PREACTION   (0)
#define MALLOC_POSTACTION  (0)

#endif

Void_t* public_mALLOc(size_t bytes) {
  Void_t* m;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = mALLOc(bytes);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

void public_fREe(Void_t* m) {
  if (MALLOC_PREACTION != 0) {
    return;
  }
  fREe(m);
  if (MALLOC_POSTACTION != 0) {
  }
}

Void_t* public_rEALLOc(Void_t* m, size_t bytes) {
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = rEALLOc(m, bytes);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

Void_t* public_mEMALIGn(size_t alignment, size_t bytes) {
  Void_t* m;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = mEMALIGn(alignment, bytes);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

Void_t* public_vALLOc(size_t bytes) {
  Void_t* m;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = vALLOc(bytes);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

Void_t* public_pVALLOc(size_t bytes) {
  Void_t* m;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = pVALLOc(bytes);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

Void_t* public_cALLOc(size_t n, size_t elem_size) {
  Void_t* m;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = cALLOc(n, elem_size);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}


Void_t** public_iCALLOc(size_t n, size_t elem_size, Void_t** chunks) {
  Void_t** m;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = iCALLOc(n, elem_size, chunks);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

Void_t** public_iCOMALLOc(size_t n, size_t sizes[], Void_t** chunks) {
  Void_t** m;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  m = iCOMALLOc(n, sizes, chunks);
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

void public_cFREe(Void_t* m) {
  if (MALLOC_PREACTION != 0) {
    return;
  }
  cFREe(m);
  if (MALLOC_POSTACTION != 0) {
  }
}

int public_mTRIm(size_t s) {
  int result;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  result = mTRIm(s);
  if (MALLOC_POSTACTION != 0) {
  }
  return result;
}

size_t public_mUSABLe(Void_t* m) {
  size_t result;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  result = mUSABLe(m);
  if (MALLOC_POSTACTION != 0) {
  }
  return result;
}

void public_mSTATs() {
  if (MALLOC_PREACTION != 0) {
    return;
  }
  mSTATs();
  if (MALLOC_POSTACTION != 0) {
  }
}

struct mallinfo public_mALLINFo() {
  struct mallinfo m;
  if (MALLOC_PREACTION != 0) {
    struct mallinfo nm = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    return nm;
  }
  m = mALLINFo();
  if (MALLOC_POSTACTION != 0) {
  }
  return m;
}

int public_mALLOPt(int p, int v) {
  int result;
  if (MALLOC_PREACTION != 0) {
    return 0;
  }
  result = mALLOPt(p, v);
  if (MALLOC_POSTACTION != 0) {
  }
  return result;
}

#endif



/* ------------- Optional versions of memcopy ---------------- */


#if USE_MEMCPY

/* 
  Note: memcpy is ONLY invoked with non-overlapping regions,
  so the (usually slower) memmove is not needed.
*/

#define MALLOC_COPY(dest, src, nbytes)  memcpy(dest, src, nbytes)
#define MALLOC_ZERO(dest, nbytes)       memset(dest, 0,   nbytes)

#else /* !USE_MEMCPY */

/* Use Duff's device for good zeroing/copying performance. */

#define MALLOC_ZERO(charp, nbytes)                                          \
do {                                                                        \
  INTERNAL_SIZE_T* mzp = (INTERNAL_SIZE_T*)(charp);                         \
  CHUNK_SIZE_T  mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T);                   \
  long mcn;                                                                 \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }           \
  switch (mctmp) {                                                          \
    case 0: for(;;) { *mzp++ = 0;                                           \
    case 7:           *mzp++ = 0;                                           \
    case 6:           *mzp++ = 0;                                           \
    case 5:           *mzp++ = 0;                                           \
    case 4:           *mzp++ = 0;                                           \
    case 3:           *mzp++ = 0;                                           \
    case 2:           *mzp++ = 0;                                           \
    case 1:           *mzp++ = 0; if(mcn <= 0) break; mcn--; }              \
  }                                                                         \
} while(0)

#define MALLOC_COPY(dest,src,nbytes)                                        \
do {                                                                        \
  INTERNAL_SIZE_T* mcsrc = (INTERNAL_SIZE_T*) src;                          \
  INTERNAL_SIZE_T* mcdst = (INTERNAL_SIZE_T*) dest;                         \
  CHUNK_SIZE_T  mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T);                   \
  long mcn;                                                                 \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }           \
  switch (mctmp) {                                                          \
    case 0: for(;;) { *mcdst++ = *mcsrc++;                                  \
    case 7:           *mcdst++ = *mcsrc++;                                  \
    case 6:           *mcdst++ = *mcsrc++;                                  \
    case 5:           *mcdst++ = *mcsrc++;                                  \
    case 4:           *mcdst++ = *mcsrc++;                                  \
    case 3:           *mcdst++ = *mcsrc++;                                  \
    case 2:           *mcdst++ = *mcsrc++;                                  \
    case 1:           *mcdst++ = *mcsrc++; if(mcn <= 0) break; mcn--; }     \
  }                                                                         \
} while(0)

#endif

/* ------------------ MMAP support ------------------  */


#if HAVE_MMAP

#include <fcntl.h>
#ifndef LACKS_SYS_MMAN_H
#include <sys/mman.h>
#endif

#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

/* 
   Nearly all versions of mmap support MAP_ANONYMOUS, 
   so the following is unlikely to be needed, but is
   supplied just in case.
*/

#ifndef MAP_ANONYMOUS

static int dev_zero_fd = -1; /* Cached file descriptor for /dev/zero. */

#define MMAP(addr, size, prot, flags) ((dev_zero_fd < 0) ? \
 (dev_zero_fd = open("/dev/zero", O_RDWR), \
  mmap((addr), (size), (prot), (flags), dev_zero_fd, 0)) : \
   mmap((addr), (size), (prot), (flags), dev_zero_fd, 0))

#else

#define MMAP(addr, size, prot, flags) \
 (mmap((addr), (size), (prot), (flags)|MAP_ANONYMOUS, -1, 0))

#endif


#endif /* HAVE_MMAP */


/*
  -----------------------  Chunk representations -----------------------
*/


/*
  This struct declaration is misleading (but accurate and necessary).
  It declares a "view" into memory allowing access to necessary
  fields at known offsets from a given base. See explanation below.
*/

struct malloc_chunk {

  INTERNAL_SIZE_T      prev_size;  /* Size of previous chunk (if free).  */
  INTERNAL_SIZE_T      size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;
};


typedef struct malloc_chunk* mchunkptr;

/* conversion from malloc headers to user pointers, and back */
#define chunk2mem(p)   ((Void_t*)((char*)(p) + 2*SIZE_SZ))
#define mem2chunk(mem) ((mchunkptr)((char*)(mem) - 2*SIZE_SZ))

/*
   malloc_chunk details:

    (The following includes lightly edited explanations by Colin Plumb.)

    Chunks of memory are maintained using a `boundary tag' method as
    described in e.g., Knuth or Standish.  (See the paper by Paul
    Wilson ftp://ftp.cs.utexas.edu/pub/garbage/allocsrv.ps for a
    survey of such techniques.)  Sizes of free chunks are stored both
    in the front of each chunk and at the end.  This makes
    consolidating fragmented chunks into bigger chunks very fast.  The
    size fields also hold bits representing whether chunks are free or
    in use.

    An allocated chunk looks like this:


    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of previous chunk, if allocated            | |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of chunk, in bytes                         |P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             User data starts here...                          .
            .                                                               .
            .             (malloc_usable_space() bytes)                     .
            .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk                                     |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


    Where "chunk" is the front of the chunk for the purpose of most of
    the malloc code, but "mem" is the pointer that is returned to the
    user.  "Nextchunk" is the beginning of the next contiguous chunk.

    Chunks always begin on even word boundries, so the mem portion
    (which is returned to the user) is also on an even word boundary, and
    thus at least double-word aligned.


    The P (PREV_INUSE) bit, stored in the unused low-order bit of the
    chunk size (which is always a multiple of two words), is an in-use
    bit for the *previous* chunk.  If that bit is *clear*, then the
    word before the current chunk size contains the previous chunk
    size, and can be used to find the front of the previous chunk.
    The very first chunk allocated always has this bit set,
    preventing access to non-existent (or non-owned) memory. If
    prev_inuse is set for any given chunk, then you CANNOT determine
    the size of the previous chunk, and might even get a memory
    addressing fault when trying to do so.

    Note that the `foot' of the current chunk is actually represented
    as the prev_size of the NEXT chunk. This makes it easier to
    deal with alignments etc but can be very confusing when trying
    to extend or adapt this code.

    The two exceptions to all this are

     1. The special chunk `top' doesn't bother using the
        trailing size field since there is no next contiguous chunk
        that would have to index off it. After initialization, `top'
        is forced to always exist.  If it would become less than
        MINSIZE bytes long, it is replenished.

     2. Chunks allocated via mmap, which have the second-lowest-order
        bit (IS_MMAPPED) set in their size fields.  Because they are
        allocated one-by-one, each must contain its own trailing size field.

*/



/*
  --------------- Operations on size fields ---------------
*/


/* size field is or'ed with PREV_INUSE when previous adjacent chunk in use */
#define PREV_INUSE 0x1

#if HAVE_MMAP
/* size field is or'ed with IS_MMAPPED if the chunk was obtained with mmap */
#define IS_MMAPPED 0x2
#else
#define IS_MMAPPED 0
#endif

/* extract inuse bit of previous chunk */
#define prev_inuse(p)       ((p)->size & PREV_INUSE)

/* check for mmap()'ed chunk */
#define chunk_is_mmapped(p) ((p)->size & IS_MMAPPED)

/* 
  Bits to mask off when extracting size 

  Note: IS_MMAPPED is intentionally not masked off from size field in
  macros for which mmapped chunks should never be seen. This should
  cause helpful core dumps to occur if it is tried by accident by
  people extending or adapting this malloc.
*/
#define SIZE_BITS (PREV_INUSE|IS_MMAPPED)

/* Get size, ignoring use bits */
#define chunksize(p)         ((CHUNK_SIZE_T)((p)->size & ~(SIZE_BITS)))


/* Ptr to next physical malloc_chunk. */
#define next_chunk(p) ((mchunkptr)( ((char*)(p)) + ((p)->size & ~PREV_INUSE)))

/* Ptr to previous physical malloc_chunk */
#define prev_chunk(p) ((mchunkptr)( ((char*)(p)) - ((p)->prev_size) ))

/* Treat space at ptr + offset as a chunk */
#define chunk_at_offset(p, s)  ((mchunkptr)(((char*)(p)) + (s)))

/* extract p's inuse bit */
#define inuse(p)\
((((mchunkptr)(((char*)(p))+((p)->size & ~PREV_INUSE)))->size) & PREV_INUSE)

/* set/clear chunk as being inuse without otherwise disturbing */
#define set_inuse(p)\
((mchunkptr)(((char*)(p)) + ((p)->size & ~PREV_INUSE)))->size |= PREV_INUSE

#define clear_inuse(p)\
((mchunkptr)(((char*)(p)) + ((p)->size & ~PREV_INUSE)))->size &= ~(PREV_INUSE)


/* check/set/clear inuse bits in known places */
#define inuse_bit_at_offset(p, s)\
 (((mchunkptr)(((char*)(p)) + (s)))->size & PREV_INUSE)

#define inuse_addr_at_offset(p, s)\
 (INTERNAL_SIZE_T*)(&(((mchunkptr)(((char*)(p)) + (s)))->size))

#define set_inuse_bit_at_offset(p, s)\
 (((mchunkptr)(((char*)(p)) + (s)))->size |= PREV_INUSE)

#define clear_inuse_bit_at_offset(p, s)\
 (((mchunkptr)(((char*)(p)) + (s)))->size &= ~(PREV_INUSE))


/* Set size at head, without disturbing its use bit */
#define set_head_size(p, s)  ((p)->size = (((p)->size & PREV_INUSE) | (s)))

/* Set size/use field */
#define set_head(p, s)       ((p)->size = (s))

/* Set size at footer (only when chunk is not in use) */
#define set_foot(p, s)     (((mchunkptr)((char*)(p) + (s)))->prev_size = (s))


/*
  ---------- Size and alignment checks and conversions ----------
*/


/* The smallest possible chunk */
#define MIN_CHUNK_SIZE        (sizeof(struct malloc_chunk))

/* The smallest size we can malloc is an aligned minimal chunk */

#define MINSIZE  \
  (CHUNK_SIZE_T)(((MIN_CHUNK_SIZE+MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK))

/* Check if m has acceptable alignment */

#define aligned_OK(m)  (((PTR_UINT)((m)) & (MALLOC_ALIGN_MASK)) == 0)


/* 
   Check if a request is so large that it would wrap around zero when
   padded and aligned. To simplify some other code, the bound is made
   low enough so that adding MINSIZE will also not wrap around sero.
*/

#define MAX_REQUEST_SIZE ((CHUNK_SIZE_T)(INTERNAL_SIZE_T)(-2 * MINSIZE))

#define request_out_of_range(req)                                \
  ((CHUNK_SIZE_T)(req) >= MAX_REQUEST_SIZE)

/* pad request bytes into a usable size -- internal version */

#define request2size(req)                                         \
  (((req) + SIZE_SZ + MALLOC_ALIGN_MASK < MINSIZE)  ?             \
   MINSIZE :                                                      \
   ((req) + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)


#define pad_request(req)                                         \
   (((req) + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)

/*  Same, except also perform argument check */

#define checked_request2size(req, sz)                             \
  if (request_out_of_range(req)) {                                \
    MALLOC_FAILURE_ACTION;                                        \
    return 0;                                                     \
  }                                                               \
  (sz) = request2size(req);                                              


typedef CHUNK_SIZE_T  bin_index_t;
typedef unsigned int  bitmap_t;


/*
  ---------- Overlaid Data types ----------
*/


/*
    "Small"  chunks are stored in circular doubly-linked lists, and look 
    like this:

    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of previous chunk                            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `head:' |             Size of chunk, in bytes                         |P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Forward pointer to next chunk in list             |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Back pointer to previous chunk in list            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Unused space (may be 0 bytes long)                .
            .                                                               .
            .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk, in bytes                           |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  Larger chunks are kept in a form of bitwise digital trees (aka tries) 
  keyed on chunksizes, in which

    * As seen as trees, they contains no duplicates (i.e., no
      duplicate sizes). If a chunk with the same size an an existing
      node is inserted, it is linked off the existing node using
      pointers that work in the same way as fd/bk pointers
      of small chunks

    * The decision to go left or right when searching is based on a
      sliding bit, starting at the most significant bit distinguishing
      sizes in the tree, and sliding right each level. All left
      children of a node are smaller than all right children, but not
      necessarily smaller than the node.

  The worst case number of steps to add or remove a node is thus
  bounded by the number of bits differentiating chunks within
  bins. Under current bin calculations, this ranges from 6 up to 21
  (for 32 bit sizes) or up to 53 (for 64 bit sizes). The typical case
  is of course much better.

  Tree chunks are overlaid in the same way as small chunks. Because
  malloc_tree_chunks are only for free chunks greater than 256 bytes, their 
  zie doesn;t impose any constraints on user chunk sizes.

    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of previous chunk                            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `head:' |             Size of chunk, in bytes                         |P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Forward pointer to next chunk of same size        |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Back pointer to previous chunk of same size       |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Pointer to left child (child[0])                  |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Pointer to right child (child[1])                 |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Pointer to parent                                 |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             bin index of this chunk                           |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Unused space                                      .
            .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk, in bytes                           |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/


struct malloc_tree_chunk {
  INTERNAL_SIZE_T      prev_size;           /* same as in malloc_chunk */
  INTERNAL_SIZE_T      size;       

  struct malloc_tree_chunk* fd;    /* double links -- used only if free. */
  struct malloc_tree_chunk* bk;

  struct malloc_tree_chunk* child[2];         
  struct malloc_tree_chunk* parent;         

  bin_index_t index;
};

typedef struct malloc_tree_chunk* tchunkptr;

typedef struct malloc_tree_chunk* tbinptr;




/*
  -------------------- Internal data structures --------------------

   All internal state is held in an instance of malloc_state defined
   below. There are no other static variables, except in two optional
   cases: 
   * If USE_MALLOC_LOCK is defined, the mALLOC_MUTEx declared above. 
   * If HAVE_MMAP is true, but mmap doesn't support
     MAP_ANONYMOUS, a dummy file descriptor for mmap.

   Beware of lots of tricks that minimize the total bookkeeping space
   requirements. The result is a little under 1K bytes (for 4byte
   pointers and size_t.)
*/

/*
  SmallBins

    An array of bin headers for free chunks. Most bins hold sizes that are
    unusual as malloc request sizes, but are more usual for fragments
    and consolidated sets of chunks, which is what these bins hold, so
    they can be found quickly.  All procedures maintain the invariant
    that no consolidated chunk physically borders another one, so each
    chunk in a list is known to be preceeded and followed by either
    inuse chunks or the ends of memory.

    To simplify use in double-linked lists, each bin header acts as a
    malloc_chunk pointing to the real first node, if it exists (else
    juct pointing to itself).  This avoids special-casing for headers.
    But to conserve space and improve locality, we allocate only the
    fd/bk pointers of bins, and then use repositioning tricks to treat
    these as the fields of a malloc_chunk*.

*/

typedef struct malloc_chunk* mbinptr;

/* addressing  */
#define bin_at(m, i) ((mbinptr)((char*)&((m)->bins[(i)<<1]) - (SIZE_SZ<<1)))

/* analog of ++bin */
#define next_bin(b)  ((mbinptr)((char*)(b) + (sizeof(mchunkptr)<<1)))

/* inverse of bin_at */
#define bin2idx(m, b) \
  ( ((int) (((char*)(b)) - (char*)(bin_at(m,0)))) / (2 * sizeof(mchunkptr)))

/*
  Indexing

    Bins for sizes < MIN_TREEBIN_SIZE bytes contain chunks of all the
    same size, spaced 8 bytes apart. Larger bins are approximately
    logarithmically spaced.
*/

#define NBINS              32
#define SMALLBIN_WIDTH      8
#define MIN_TREEBIN_SIZE  256
/* bit-shift corresponding to MIN_TREEBIN_SIZE */
#define TREE_BIN_SHIFT      8

#define in_smallbin_range(sz)  \
  ((CHUNK_SIZE_T)(sz) <= (CHUNK_SIZE_T)(MIN_TREEBIN_SIZE-1))


#define MAX_SMALL_REQUEST (MIN_TREEBIN_SIZE-(SIZE_SZ+MALLOC_ALIGNMENT))

#define smallbin_index(sz)       (bin_index_t)((sz) >> 3)

#define size_for_smallindex(i)   ((CHUNK_SIZE_T)(i) << 3) 

#define MIN_SMALLBIN_INDEX       (smallbin_index(MINSIZE))

#define MIN_SMALLBIN_BIT         (idx2bit(smallbin_index(MINSIZE)))


/* 
   There are 2 equally spaced treebins for each power of two from
   TREE_BIN_SHIFT to TREE_BIN_SHIFT+16. The last bin holds anything
   larger.
*/


static bin_index_t treebin_index(CHUNK_SIZE_T sz) {
  unsigned int  m;
  unsigned int x = sz >> TREE_BIN_SHIFT;
  if (x == 0)
    return 0;
  if (x > 0xFFFF) 
    return NBINS-1;

  /* On intel, use BSRL instruction to find highest bit */
#if defined(__GNUC__) && defined(i386)

  __asm__("bsrl %1,%0\n\t"
          : "=r" (m) 
          : "g"  (x));
    return (m << 1) + ((sz >> (m + (TREE_BIN_SHIFT-1)) & 1));

#else
  {
    /*
      Based on branch-free nlz algorithm in chapter 5 of Henry
      S. Warren Jr's book "Hacker's Delight".
    */

    unsigned int n = ((x - 0x100) >> 16) & 8;
    x <<= n; 
    m = ((x - 0x1000) >> 16) & 4;
    n += m; 
    x <<= m; 
    m = ((x - 0x4000) >> 16) & 2;
    n += m; 
    x = (x << m) >> 14;
    m = 13 - n + (x & ~(x>>1));
    /* shift up n and use the next bit to make finer-granularity bins. */
    return (m << 1) + ((sz >> (m + (TREE_BIN_SHIFT-1)) & 1));
  }
#endif
}

static bin_index_t bit2idx(bitmap_t x) {
#if defined(__GNUC__) && defined(i386)
  int r;
  __asm__("bsfl %1,%0\n\t"
	 : "=r" (r) : "g" (x));
  return (bin_index_t)r;
#else
  return (bin_index_t)(ffs(x)-1);
#endif
}



#define bin_index(sz) \
 ((in_smallbin_range(sz)) ? smallbin_index(sz) : treebin_index(sz))

/*
  The most significant bit distinguishing nodes in the tree
  associated with a given bin
*/

#define CHUNK_SIZE_BITS (sizeof(CHUNK_SIZE_T) * 8)

#define bitshift_for_index(idx) \
  (idx == NBINS-1)? \
    CHUNK_SIZE_BITS-1 : \
    (((idx) >> 1) + TREE_BIN_SHIFT-2)

#define tbin_at(m,i) (&((m)->treebins[i]))

#define minsize_for_treeindex(i)                             \
    (((CHUNK_SIZE_T)(1) << (((i) >> 1) + TREE_BIN_SHIFT)) |  \
    (((CHUNK_SIZE_T)((i) & 1)) <<                            \
     ( ( (i) >> 1) + TREE_BIN_SHIFT - 1)))



#define is_tbin(M, P) ((tbinptr*)(P) >= &((M)->treebins[0]) && \
                       (tbinptr*)(P) <  &((M)->treebins[NBINS]))

#define leftmost_child(t) \
  (((t)->child[0] != 0)? ((t)->child[0]) : ((t)->child[1]))


/*
  Fastbins

    An array of lists holding recently freed small chunks.  Fastbins
    are not doubly linked.  It is faster to single-link them, and
    since chunks are never removed from the middles of these lists,
    double linking is not necessary. Also, unlike regular bins, they
    are not even processed in FIFO order (they use faster LIFO) since
    ordering doesn't much matter in the transient contexts in which
    fastbins are normally used.

    Chunks in fastbins keep their inuse bit set, so they cannot
    be consolidated with other free chunks. malloc_consolidate
    releases all chunks in fastbins and consolidates them with
    other free chunks. 
*/

typedef struct malloc_chunk* mfastbinptr;

#define fastbin_at(m,i) (&((m)->fastbins[i]))

/* offset 2 to use otherwise unindexable first 2 bins */
#define fastbin_index(sz)        ((((unsigned int)(sz)) >> 3) - 2)


/* The maximum fastbin request size we support */
#define MAX_FAST_REQUEST     64

#define MAX_FAST_SIZE (request2size(MAX_FAST_REQUEST))

#define NFASTBINS  (fastbin_index(MAX_FAST_SIZE)+1)

/*
  FASTCHUNKS_BIT held in max_fast indicates that there are probably
  some fastbin chunks. It is set true on entering a chunk into any
  fastbin, and cleared only in malloc_consolidate.
*/

#define FASTCHUNKS_BIT        (2U)

#define have_fastchunks(M)   (((M)->max_fast &  FASTCHUNKS_BIT))
#define set_fastchunks(M)    ((M)->max_fast |=  (FASTCHUNKS_BIT))
#define clear_fastchunks(M)  ((M)->max_fast &= ~(FASTCHUNKS_BIT))

/* 
   Set value of max_fast. 
   Use impossibly small value if 0.
*/

#define set_max_fast(M, s) \
  (M)->max_fast = (((s) == 0)? SMALLBIN_WIDTH: request2size(s)) | \
  ((M)->max_fast &  (FASTCHUNKS_BIT))

#define get_max_fast(M) \
  ((M)->max_fast & ~(FASTCHUNKS_BIT))


/*
  Top

    The top-most available chunk (i.e., the one bordering the end of
    available memory) is treated specially. It is never included in
    any bin, is used only if no other chunk is available, and is
    released back to the system if it is very large (see
    M_TRIM_THRESHOLD).   

    Top initially points to a dummy bin with zero size, thus forcing
    extension on the first malloc request, so we avoid having any
    special code in malloc to check whether it even exists yet. 
*/


/*
  Binmap

    To help compensate for the large number of bins, a one-level index
    structure is used for bin-by-bin searching.  `binmap' is a
    bitvector recording whether bins are definitely empty so they can
    be skipped over during during traversals.  
*/

/* Conservatively use 32 bits per map word, even if on 64bit system */

#define idx2bit(i)           ((bitmap_t)(1) << (i))

#define mark_smallbin(m,i)   ((m)->smallbits |= idx2bit(i))
#define mark_treebin(m,i)    ((m)->treebits  |= idx2bit(i))

#define clear_smallbin(m,i)  ((m)->smallbits &= ~idx2bit(i))
#define clear_treebin(m,i)   ((m)->treebits  &= ~idx2bit(i))


/* isolate the least set bit of a bitmap */

#define least_bit(x)         ((x) & -(x))

/* create mask with all bits to left of least bit of x on */

#define left_bits(x)         ((x<<1) | -(x<<1))

/* create mask with all bits to left of or equal to least bit of x on */

#define same_or_left_bits(x) ((x) | -(x))


/*
  sysctl is a status word holding dynamically discovered
  or controlled properties of the morecore function
*/

#define MORECORE_CONTIGUOUS_BIT  (1U)
#define TRIM_DISABLE_BIT         (2U)
#define MMAP_DISABLE_BIT         (4U)

#define contiguous(M) \
        (((M)->sysctl &  MORECORE_CONTIGUOUS_BIT))
#define set_contiguous(M) \
        ((M)->sysctl |=  MORECORE_CONTIGUOUS_BIT)
#define set_noncontiguous(M) \
        ((M)->sysctl &= ~MORECORE_CONTIGUOUS_BIT)

#define disable_trim(M) \
        ((M)->sysctl |=  TRIM_DISABLE_BIT)
#define enable_trim(M) \
        ((M)->sysctl &=  ~TRIM_DISABLE_BIT)
#define trim_disabled(M) \
        ((M)->sysctl & TRIM_DISABLE_BIT)

#define enable_mmap(M) \
        ((M)->sysctl &=  ~MMAP_DISABLE_BIT)
#define disable_mmap(M) \
        ((M)->sysctl |=  MMAP_DISABLE_BIT)
#define mmap_disabled(M) \
        ((M)->sysctl &   MMAP_DISABLE_BIT)




/*
   ----------- Internal state representation and initialization -----------
*/

struct malloc_state {
  /* The maximum chunk size to be eligible for fastbin */
  CHUNK_SIZE_T     max_fast;

  /* Bitmap of bins */
  bitmap_t         smallbits;
  bitmap_t         treebits;

  /* Base of the topmost chunk -- not otherwise kept in a bin */
  mchunkptr        top;

  /* Fastbins */
  mfastbinptr      fastbins[NFASTBINS];

  /* Smallbins packed as described above */
  mchunkptr        bins[NBINS * 2];

  /* Treebins */
  tbinptr          treebins[NBINS];

  /* Padding to allow addressing past end of treebin array */
  struct malloc_tree_chunk initial_top;

  /* Tunable parameters */
  CHUNK_SIZE_T     trim_threshold;
  INTERNAL_SIZE_T  top_pad;
  INTERNAL_SIZE_T  mmap_threshold;

  /* Memory map support */
  int              n_mmaps;
  int              n_mmaps_max;
  int              max_n_mmaps;

  /* Cache malloc_getpagesize */
  unsigned int     pagesize;    

  /* Track properties of MORECORE */
  unsigned int     sysctl;

  /* Statistics */
  INTERNAL_SIZE_T  mmapped_mem;
  INTERNAL_SIZE_T  sbrked_mem;
  INTERNAL_SIZE_T  max_sbrked_mem;
  INTERNAL_SIZE_T  max_mmapped_mem;
  INTERNAL_SIZE_T  max_total_mem;
};

typedef struct malloc_state *mstate;

/* 
   There is exactly one instance of this struct in this malloc.
   If you are adapting this malloc in a way that does NOT use a static
   malloc_state, you MUST explicitly zero-fill it before using. This
   malloc relies on the property that malloc_state is initialized to
   all zeroes (as is true of C statics).
*/

static struct malloc_state av_;  /* never directly referenced */

/*
   All uses of av_ are via get_malloc_state().
   At most one "call" to get_malloc_state is made per invocation of
   the public versions of malloc and free, but other routines
   that in turn invoke malloc and/or free may call more then once. 
   Also, it is called in check* routines if DEBUG is set.
*/

#define get_malloc_state() (&(av_))

/*
  Initialize a malloc_state struct. This is called only
  in sysmalloc, to avoid it being inlined everywhere else,
  which causes useless code bloat.
*/

static void malloc_init_state(mstate av) {
  int     i;
  mbinptr bin;
  
  /* Establish circular links for bins */
  for (i = 0; i < NBINS; ++i) { 
    bin = bin_at(av,i);
    bin->fd = bin->bk = bin;
  }

  av->top_pad        = DEFAULT_TOP_PAD;
  av->n_mmaps_max    = DEFAULT_MMAP_MAX;
  av->mmap_threshold = DEFAULT_MMAP_THRESHOLD;
  av->trim_threshold = DEFAULT_TRIM_THRESHOLD;

#if MORECORE_CONTIGUOUS
  set_contiguous(av);
#else
  set_noncontiguous(av);
#endif

  set_max_fast(av, DEFAULT_MXFAST);

  av->top = (mchunkptr)(&(av->initial_top));
  av->pagesize  = malloc_getpagesize;
}

#define ensure_initialization(M) \
  if ((M)->top == 0) sysmalloc(M, 0);


/* 
   Other internal utilities
*/

static Void_t*  sysmalloc(mstate, CHUNK_SIZE_T);
static int  systrim(mstate, size_t);
static Void_t** iALLOc(size_t, size_t*, int, Void_t**);
static void insert_treenode(mstate, tchunkptr, CHUNK_SIZE_T);
#if 0
static void unlink_treenode(mstate, tchunkptr);
static void unlink_small_chunk(mstate av, mchunkptr p, CHUNK_SIZE_T size);
#endif
static void transfer_tree_links(tchunkptr oldt, tchunkptr newt);
static tchunkptr find_replacement(tchunkptr t);
static void unlink_chained_node(tchunkptr t);
static void insert_small_chunk(mstate av, mchunkptr p, CHUNK_SIZE_T nb);
static void insert_chunk(mstate av, mchunkptr p, CHUNK_SIZE_T nb);
static mchunkptr take_from_smallbin(mstate av, mchunkptr bin, bitmap_t bit);
static void unlink_chunk(mstate av, mchunkptr p, CHUNK_SIZE_T size);
static void malloc_consolidate(mstate);


#pragma no_inline(systrim)


#if HAVE_MMAP
static mchunkptr mmap_malloc(mstate, INTERNAL_SIZE_T);
#endif

/*
  Debugging support

  These routines make a number of assertions about the states
  of data structures that should be true at all times. If any
  are not true, it's very likely that a user program has somehow
  trashed memory. (It's also possible that there is a coding error
  in malloc. In which case, please report it!)
*/


#if ! DEBUG

#define check_chunk(P)
#define check_free_chunk(P)
#define check_inuse_chunk(P)
#define check_remalloced_chunk(P,N)
#define check_malloced_chunk(P,N)
#define check_malloc_state(M)
#define check_tree(P)

#else
#define check_chunk(P)              do_check_chunk(P)
#define check_free_chunk(P)         do_check_free_chunk(P)
#define check_inuse_chunk(P)        do_check_inuse_chunk(P)
#define check_remalloced_chunk(P,N) do_check_remalloced_chunk(P,N)
#define check_malloced_chunk(P,N)   do_check_malloced_chunk(P,N)
#define check_tree(P)          do_check_tree(P)
#define check_malloc_state(M)       do_check_malloc_state(M)

static void do_check_malloc_state(mstate);

/*
  Find x in a treebin. Used in other check functions.
*/

static tchunkptr tree_find(tchunkptr x) {
  mstate av = get_malloc_state();
  CHUNK_SIZE_T nb = chunksize(x);
  bin_index_t idx = treebin_index(nb);
  tbinptr* bin = tbin_at(av, idx);
  tchunkptr t = *bin;
  bin_index_t shift = bitshift_for_index(idx);
  CHUNK_SIZE_T allbits = 0;

  if (t == 0)
    return 0;

  while (t != 0) {
    CHUNK_SIZE_T size;
    if (t == x)
      return t;
    size = chunksize(t);
    assert((size & allbits) == allbits);
    if (size == nb) {
      tchunkptr p = t->bk;
      for (;;) {
        if (p == x)
          return p;
        else if (p == t)
          return 0;
        else
          p = p->bk;
      }
    }
    if (((nb >> shift) & 1) == 0) {
      t = t->child[0];
    }
    else {
      t = t->child[1];
      allbits |= 1U << shift;
    }

    --shift;
  }
  return 0;
}

/*
  Properties of all chunks
*/

static void do_check_chunk(mchunkptr p) {
  mstate av = get_malloc_state();
  CHUNK_SIZE_T  sz = chunksize(p);
  /* min and max possible addresses assuming contiguous allocation */
  char* max_address = (char*)(av->top) + chunksize(av->top);
  char* min_address = max_address - av->sbrked_mem;

  if (!chunk_is_mmapped(p)) {
    
    /* Has legal address ... */
    if (p != av->top) {
      if (contiguous(av)) {
        assert(((char*)p) >= min_address);
        assert(((char*)p + sz) <= ((char*)(av->top)));
      }
    }
    else {
      /* top size is always at least MINSIZE */
      assert((CHUNK_SIZE_T)(sz) >= MINSIZE);
      /* top predecessor always marked inuse */
      assert(prev_inuse(p));
    }
      
  }
  else {
#if HAVE_MMAP
    /* address is outside main heap  */
    if (contiguous(av) && av->top != (mchunkptr)(&(av->initial_top))) {
      assert(((char*)p) < min_address || ((char*)p) > max_address);
    }
    /* chunk is page-aligned */
    assert(((p->prev_size + sz) & (av->pagesize-1)) == 0);
    /* mem is aligned */
    assert(aligned_OK(chunk2mem(p)));
#else
    /* force an appropriate assert violation if debug set */
    assert(!chunk_is_mmapped(p));
#endif
  }
}

static void check_all_less(tchunkptr t, CHUNK_SIZE_T nb) {
  if (t == 0)
    return;
  assert(chunksize(t) < nb);
  check_all_less(t->child[0], nb);
  check_all_less(t->child[1], nb);
}

static void check_all_greater(tchunkptr t, CHUNK_SIZE_T nb) {
  if (t == 0)
    return;
  assert(chunksize(t) >= nb);
  check_all_greater(t->child[0], nb);
  check_all_greater(t->child[1], nb);
}


static INTERNAL_SIZE_T check_tree_fields(tchunkptr t) {
  INTERNAL_SIZE_T size = chunksize(t);
  assert(size >= MIN_TREEBIN_SIZE);
  do_check_chunk(((mchunkptr)t));
  assert(!inuse(t));
  assert(t->fd->bk == t);
  assert(t->bk->fd == t);
  assert(t->parent != t);
  assert(t->child[0] != t);
  assert(t->child[1] != t);
  if (t->child[0] != 0 && t->child[1] != 0) {
    check_all_less(t->child[0], chunksize(t->child[1]));
    check_all_greater(t->child[1], chunksize(t->child[0]));
  }
  return size;
}

static INTERNAL_SIZE_T do_check_tree(tchunkptr t) {
  tchunkptr p;
  tchunkptr h;
  INTERNAL_SIZE_T total = check_tree_fields(t);

  /* If t is on a same-sized list, another node on list must have a parent */
  if (t->parent == 0) {
    h = t->bk;
    while (h != t && h->parent == 0) 
      h = h->bk;
    assert(h != t);
  }
  else
    h = t;

  assert (h->parent->child[0] == h ||
          h->parent->child[1] == h ||
          *((tbinptr*)(h->parent)) == h);


  /* only one node on a same-sized list has parent or children */
  p = h->bk;
  while (p != h) {
    assert(p->child[0] == 0);
    assert(p->child[1] == 0);
    assert(p->parent == 0);
    assert(chunksize(p) == chunksize(h));
    total += check_tree_fields(p);
    p = p->bk;
  }

  if (h->child[0] != 0) {
    assert(h->child[0]->parent == h);
    total += do_check_tree(h->child[0]);
  }

  if (h->child[1] != 0) {
    assert(h->child[1]->parent == h);
    total += do_check_tree(h->child[1]);
  }

  return total;
}

static void do_check_links(mchunkptr p) {
  if (in_smallbin_range(chunksize(p))) {
    assert(p->fd->bk == p);
    assert(p->bk->fd == p);
  }
  else {
    do_check_tree((tchunkptr)p);
  }
}

/*
  Properties of free chunks
*/

static void do_check_free_chunk(mchunkptr p) {
  mstate av = get_malloc_state();

  INTERNAL_SIZE_T sz = p->size & ~PREV_INUSE;
  mchunkptr next = chunk_at_offset(p, sz);

  do_check_chunk(p);

  /* Chunk must claim to be free ... */
  assert(!inuse(p));
  assert (!chunk_is_mmapped(p));

  /* Unless a special marker, must have OK fields */
  if ((CHUNK_SIZE_T)(sz) >= MINSIZE)
  {
    assert((sz & MALLOC_ALIGN_MASK) == 0);
    assert(aligned_OK(chunk2mem(p)));
    /* ... matching footer field */
    assert(next->prev_size == sz);
    /* ... and is fully consolidated */
    assert(prev_inuse(p));
    assert (next == av->top || inuse(next));

    do_check_links(p);
  }
  else /* markers are always of size SIZE_SZ */
    assert(sz == SIZE_SZ);
}

/*
  Properties of inuse chunks
*/

static void do_check_inuse_chunk(mchunkptr p) {
  mstate av = get_malloc_state();
  mchunkptr next;
  do_check_chunk(p);

  if (chunk_is_mmapped(p))
    return; /* mmapped chunks have no next/prev */

  /* Check whether it claims to be in use ... */
  assert(inuse(p));

  next = next_chunk(p);

  /* ... and is surrounded by OK chunks.
    Since more things can be checked with free chunks than inuse ones,
    if an inuse chunk borders them and debug is on, it's worth doing them.
  */
  if (!prev_inuse(p))  {
    /* Note that we cannot even look at prev unless it is not inuse */
    mchunkptr prv = prev_chunk(p);
    assert(next_chunk(prv) == p);
    do_check_free_chunk(prv);
  }

  if (next == av->top) {
    assert(prev_inuse(next));
    assert(chunksize(next) >= MINSIZE);
  }
  else if (!inuse(next))
    do_check_free_chunk(next);
}


static void do_check_remalloced_chunk(mchunkptr p, INTERNAL_SIZE_T s) {
  INTERNAL_SIZE_T sz = p->size & ~PREV_INUSE;

  do_check_inuse_chunk(p);

  /* Legal size ... */
  assert((sz & MALLOC_ALIGN_MASK) == 0);
  assert((CHUNK_SIZE_T)(sz) >= MINSIZE);
  /* ... and alignment */
  assert(aligned_OK(chunk2mem(p)));
  /* chunk is less than MINSIZE more than request */
  assert((long)(sz) - (long)(s) >= 0);
  assert((long)(sz) - (long)(s + MINSIZE) < 0);
}

/*
  Properties of nonrecycled chunks at the point they are malloced
*/

static void do_check_malloced_chunk(mchunkptr p, INTERNAL_SIZE_T s) {
  /* same as recycled case ... */
  do_check_remalloced_chunk(p, s);
#if 0
  do_check_malloc_state(get_malloc_state());
#endif

  /*
    ... plus,  must obey implementation invariant that prev_inuse is
    always true of any allocated chunk; i.e., that each allocated
    chunk borders either a previously allocated and still in-use
    chunk, or the base of its memory arena. This is ensured
    by making all allocations from the the `lowest' part of any found
    chunk.  
  */

  assert(prev_inuse(p));
}


static CHUNK_SIZE_T do_check_smallbin(mstate av, int i, mbinptr b) {
  mchunkptr p = b->bk;
  mchunkptr q;
  bin_index_t idx;
  INTERNAL_SIZE_T size;
  CHUNK_SIZE_T  total = 0;
  unsigned int empty = (av->smallbits & (1 << i)) == 0;

  if (i >= 2) {
    if (empty)
      assert(p == b);
    if (p == b)
      assert(empty);

    if (p != b)
      assert(!empty);
  }

  for (; p != b; p = p->bk) {
    /* each chunk claims to be free */
    do_check_free_chunk(p);
    size = chunksize(p);
    total += size;
    if (i >= 2) {
      /* chunk belongs in bin */
      idx = smallbin_index(size);
      assert(idx == i);
      assert(p->bk == b || 
             (CHUNK_SIZE_T)chunksize(p->bk) == 
             (CHUNK_SIZE_T)chunksize(p));
    }
    /* chunk is followed by a legal chain of inuse chunks */
    for (q = next_chunk(p);
         (q != av->top && inuse(q) && 
          (CHUNK_SIZE_T)(chunksize(q)) >= MINSIZE);
         q = next_chunk(q))
      do_check_inuse_chunk(q);
  }

  return total;
}

/*
  Properties of malloc_state.

  This may be useful for debugging malloc, as well as detecting user
  programmer errors that somehow write into malloc_state.

  If you are extending or experimenting with this malloc, you can
  probably figure out how to hack this routine to print out or
  display chunk addresses, sizes, bins, and other instrumentation.
*/

static void do_check_malloc_state(mstate av) {
  int i;
  mbinptr b;
  CHUNK_SIZE_T  total = 0;
  tchunkptr t;
  unsigned int empty;
  tbinptr* tb;
  int max_fast_bin;
  mchunkptr p;

  /* internal size_t must be no wider than pointer type */
  assert(sizeof(INTERNAL_SIZE_T) <= sizeof(char*));

  /* alignment is a power of 2 */
  assert((MALLOC_ALIGNMENT & (MALLOC_ALIGNMENT-1)) == 0);

  /* cannot run remaining checks until fully initialized */
  if (av->top == 0 || av->top == (mchunkptr)(&(av->initial_top)))
    return;

  /* pagesize is a power of 2 */
  assert((av->pagesize & (av->pagesize-1)) == 0);

  /* check smallbins */
  for (i = 1; i < NBINS; ++i) {
    b = bin_at(av, i);
    total += do_check_smallbin(av, i, b);
  }

  /* check treebins */
  for (i = 0; i < NBINS; ++i) {
    tb = tbin_at(av, i);
    t = *tb;
    empty = (av->treebits & (1 << i)) == 0;
    if (t == 0)
      assert(empty);
    else if (t != 0) {
      assert(!empty);
      total += do_check_tree(t);
    }
  }


  /* top chunk is OK */
  check_chunk(av->top);
  /* top not in tree */
  if (!in_smallbin_range(chunksize(av->top)))
    assert(tree_find((tchunkptr)(av->top)) == 0);

  /* max_fast is in allowed range */
  assert(get_max_fast(av) <= request2size(MAX_FAST_SIZE));

  max_fast_bin = fastbin_index(av->max_fast);

  for (i = 0; i < NFASTBINS; ++i) {
    p = av->fastbins[i];

    /* all bins past max_fast are empty */
    if (i > max_fast_bin)
      assert(p == 0);

    while (p != 0) {
      /* each chunk claims to be inuse */
      do_check_inuse_chunk(p);
      total += chunksize(p);
      /* chunk belongs in this bin */
      assert(fastbin_index(chunksize(p)) == i);
      p = p->fd;
    }
  }

  /* sanity checks for statistics */

  assert(total <= (CHUNK_SIZE_T)(av->max_total_mem));

  assert(av->n_mmaps >= 0);
  assert(av->n_mmaps <= av->n_mmaps_max);
  assert(av->n_mmaps <= av->max_n_mmaps);

  assert((CHUNK_SIZE_T)(av->sbrked_mem) <=
         (CHUNK_SIZE_T)(av->max_sbrked_mem));

  assert((CHUNK_SIZE_T)(av->mmapped_mem) <=
         (CHUNK_SIZE_T)(av->max_mmapped_mem));

  assert((CHUNK_SIZE_T)(av->max_total_mem) >=
         (CHUNK_SIZE_T)(av->mmapped_mem) + (CHUNK_SIZE_T)(av->sbrked_mem));
}
#endif



/*
   ----------- Operations on trees -----------
*/

/*
  Insert chunk into corresponding list or tree
*/

static void insert_treenode(mstate av, tchunkptr x, 
                            CHUNK_SIZE_T nb) {
  bin_index_t idx = treebin_index(nb);
  tbinptr* bin = tbin_at(av, idx);
  tchunkptr t = *bin;
  
  x->child[0] = 0;
  x->child[1] = 0;
  x->index = idx;

  if (t == 0) {
    av->treebits |= idx2bit(idx);
    *bin = x;
    x->parent = (tchunkptr)bin;
    x->fd = x;
    x->bk = x;
  }
  else {
    bin_index_t shift = bitshift_for_index(idx);
    tchunkptr b;
    check_tree(t);

    while (chunksize(t) != nb) {
      tchunkptr* pchild = &(t->child[(nb >> shift--) & 1]);
      if (*pchild != 0) {
        t = *pchild;
      }
      else {
        *pchild = x;
        x->parent = t;
        x->fd = x;
        x->bk = x;
        return;
      }
    }
    /* Link in same-sized node */
    b = t->bk;
    x->parent = 0;
    x->fd = t;
    x->bk = b;
    b->fd = t->bk = x;
  }
}

static void transfer_tree_links(tchunkptr oldt, tchunkptr newt) {
  tchunkptr p = oldt->parent;
  newt->parent = p;

  if (p->child[0] == oldt) 
    p->child[0] = newt;
  else if (p->child[1] == oldt) 
    p->child[1] = newt;
  else 
    *((tbinptr*)p) = newt;

  if ( (newt->child[0] = oldt->child[0]) != 0)
    newt->child[0]->parent = newt;

  if ( (newt->child[1] = oldt->child[1]) != 0)
    newt->child[1]->parent = newt;
}

static tchunkptr find_replacement(tchunkptr t) {
  /* 
     Unless t is itself a leaf node, we must replace t with a leaf
     node (not merely one with an open left or right, as with binary
     search trees), to make sure that lefts and rights of descendents
     correspond properly to bit masks.  We use the leftmost leaf of
     right child, or, if there is no right child, the rightmost leaf
     of left child.  We could use any other leaf, but these choices
     will tend to maintain nicer trees.
  */
  tchunkptr* cp;
  tchunkptr c;
  if ((c = *(cp = &(t->child[1]))) == 0)
    if ((c = *(cp = &(t->child[0]->child[1]))) == 0)
      c = *(cp = &(t->child[0]));

  assert(c != 0);
  for (;;) {
    if (c->child[0] != 0) 
      c = *(cp = &(c->child[0]));
    else if (c->child[1] != 0) 
      c = *(cp = &(c->child[1]));
    else {
      *cp = 0; /* unlink from parent */
      return c;
    }
  }
}

static void unlink_leaf_node(mstate av, tchunkptr t) {
  tchunkptr p = t->parent;
  if (p->child[0] == t) 
    p->child[0] = 0;
  else if (p->child[1] == t) 
    p->child[1] = 0;
  else {
    assert(is_tbin(av, p));
    *((tbinptr*)p) = 0;
    av->treebits &= ~idx2bit(t->index);
  }
}

static void unlink_chained_node(tchunkptr t) {
  tchunkptr bck = t->bk;
  tchunkptr fwd = t->fd;
  fwd->bk = bck;
  bck->fd = fwd;
  /* t's parent is nonnull if t was head of chain */
  if (t->parent != 0) 
    transfer_tree_links(t, fwd);
  check_tree(fwd);
}



/*
   ----------- other helper functions -----------
  We expect these to be inlined
*/


static void insert_small_chunk(mstate av, mchunkptr p, CHUNK_SIZE_T nb) { 
  bin_index_t idx = smallbin_index(nb);
  mchunkptr bck = bin_at(av, idx);
  mchunkptr fwd = bck->fd;
  mark_smallbin(av, idx);
  p->fd = fwd;
  p->bk = bck;
  fwd->bk = bck->fd = p;

  assert(in_smallbin_range(nb));
}

static void insert_chunk(mstate av, mchunkptr p, CHUNK_SIZE_T nb) { 
  if (in_smallbin_range(nb)) 
    insert_small_chunk(av, p, nb);
  else 
    insert_treenode(av, (tchunkptr)p, nb);
}


static mchunkptr take_from_smallbin(mstate av, mchunkptr bin, bitmap_t bit) {
  mchunkptr p = bin->bk;
  mchunkptr bck = p->bk;
  assert(p != bin);
  bin->bk = bck;
  bck->fd = bin;
  if (bck == bin)
    av->smallbits &= ~bit; 
  return p;
}

static void unlink_chunk(mstate av, mchunkptr q, CHUNK_SIZE_T size) {
  mchunkptr fwd = q->fd;
  mchunkptr bck = q->bk;
  fwd->bk = bck;
  bck->fd = fwd;
  if (fwd == bck && in_smallbin_range(size)) {
    clear_smallbin(av, smallbin_index(size));
  }
  else if (!in_smallbin_range(size)) {
    tchunkptr t = (tchunkptr)q;
    tchunkptr c = (tchunkptr)fwd;
    if (c == t) {
      if (t->child[0] == t->child[1]) {
        unlink_leaf_node(av, t);
        return;
      }
      else {
        c = find_replacement(t);
      }
    }
    else {
      if (t->parent == 0) {
        return;
      }
    }

    transfer_tree_links(t, c);
    check_tree(c);
  }
}

static Void_t* use_treechunk(mstate av, 
                             CHUNK_SIZE_T nb,
                             tchunkptr bestchunk,
                             CHUNK_SIZE_T bestsize, 
                             tchunkptr leaf) {

  CHUNK_SIZE_T rsize;

  if (bestchunk->bk != bestchunk)
    unlink_chained_node(bestchunk);
  else {
    unlink_leaf_node(av, leaf);
    if (leaf != bestchunk) 
      transfer_tree_links(bestchunk, leaf);
  }
  
  rsize = bestsize - nb;
  if (rsize >= MINSIZE) {
    mchunkptr rem = chunk_at_offset(bestchunk, nb);
    set_head(bestchunk, nb | PREV_INUSE);
    set_head(rem, rsize | PREV_INUSE);
    set_foot(rem, rsize);
    insert_chunk(av, rem, rsize);
  }
  else {
    set_inuse_bit_at_offset(bestchunk, bestsize);
  }
  check_malloced_chunk((mchunkptr)(bestchunk), nb);
  return chunk2mem(bestchunk);
}


/*
  ------------------------------ malloc ------------------------------
*/

Void_t* mALLOc(size_t bytes) {
  mstate av = get_malloc_state();
  CHUNK_SIZE_T nb;

  checked_request2size(bytes, nb);

  if (nb <= (CHUNK_SIZE_T)(av->max_fast)) { 
    mfastbinptr*  fb = &(av->fastbins[(fastbin_index(nb))]);
    mchunkptr fp = *fb;
    if (fp != 0) {
      *fb = fp->fd;
      check_remalloced_chunk(fp, nb);
      return chunk2mem(fp);
    }
  }
    

  for (;;) {
    if (in_smallbin_range(nb)) {
      bin_index_t sidx = smallbin_index(nb);
      bitmap_t sbit = idx2bit(sidx);
      
      if (sbit <= av->smallbits) {
        mchunkptr p;
        if ((sbit & av->smallbits) != 0) {
          p = take_from_smallbin(av, bin_at(av,sidx), sbit);
          set_inuse_bit_at_offset(p, nb);
        }
        else {
          bitmap_t nbit = least_bit(left_bits(sbit) & av->smallbits);
          bin_index_t nidx = bit2idx(nbit);
          CHUNK_SIZE_T psize = size_for_smallindex(nidx);
          CHUNK_SIZE_T qsize = psize - nb;
          p = take_from_smallbin(av, bin_at(av, nidx), nbit);
          if (qsize < MINSIZE) {
            set_inuse_bit_at_offset(p, psize);
          }
          else {
            mchunkptr q = chunk_at_offset(p, nb);
            set_head(p, nb | PREV_INUSE);
            set_head(q, qsize | PREV_INUSE);
            set_foot(q, qsize);
            insert_small_chunk(av, q, qsize);
          }
        }
        check_malloced_chunk(p, nb);
        return chunk2mem(p);
      }
      
      if (av->treebits != 0) {
        bitmap_t vbit = least_bit(av->treebits);
        bin_index_t vidx = bit2idx(vbit);
        tbinptr* vbin = tbin_at(av, vidx);
        tchunkptr bestchunk = *vbin;
        tchunkptr c = leftmost_child(bestchunk);
        CHUNK_SIZE_T bestsize = chunksize(bestchunk);
        tchunkptr leaf;
        CHUNK_SIZE_T rsize;

        /*  Fast path if remainder will replace bestchunk */
        if (c == 0) {
          rsize = bestsize - nb;
          leaf = bestchunk;

          if (rsize >= minsize_for_treeindex(vidx) &&
              bestchunk->bk == bestchunk) {
            tchunkptr r = (tchunkptr)(chunk_at_offset(bestchunk, nb));

            set_head(bestchunk, nb | PREV_INUSE);
            set_head(r, rsize | PREV_INUSE);
            set_foot(r, rsize);
            *vbin = r;
            r->fd = r;
            r->bk = r;
            r->child[0] = 0;
            r->child[1] = 0;
            r->parent = (tchunkptr)vbin;
            r->index = vidx;
            check_malloced_chunk((mchunkptr)bestchunk, nb);
            return chunk2mem(bestchunk);
          }
        }
        else {
          do {
            CHUNK_SIZE_T csize = chunksize(c);
            if (csize < bestsize) {
              bestchunk = c;
              bestsize = csize;
            }
            leaf = c;
            c = leftmost_child(c);
          } while (c != 0);
        }
        return use_treechunk(av, nb, bestchunk, bestsize, leaf);
      }
    }
    else {
      bin_index_t tidx = treebin_index(nb);
      bitmap_t tbit = idx2bit(tidx);
      
      if (tbit <= av->treebits) {
        tchunkptr bestchunk = 0;
        CHUNK_SIZE_T bestsize = MAX_CHUNK_SIZE;
        tchunkptr leaf;
        bitmap_t vbit;
        
        for (;;) {
          if ((tbit & av->treebits) != 0) {
            tchunkptr t = *tbin_at(av, tidx);
            bin_index_t shift = bitshift_for_index(tidx);
            for (;;) {
              int dir;
              CHUNK_SIZE_T tsize = chunksize(t);
              leaf = t;
              if (tsize >= nb && tsize < bestsize) {
                bestchunk = t;
                bestsize = tsize;
                if (tsize == nb && t->bk != t)
                  break;
              }
              
              dir = (shift == 0)? 0 : (nb >> shift--) & 1;
              t = leaf->child[dir];
              if (t == 0) {
                shift = 0; /* if forced right, go leftmost from now on */
                t = leaf->child[1-dir];
                if (t == 0)
                  break;
              }
            } 
            if (bestchunk != 0)
              return use_treechunk(av, nb, bestchunk, bestsize, leaf);
          }
          if (have_fastchunks(av))
            malloc_consolidate(av);
          else
            break;
        }
        
        vbit = least_bit(left_bits(tbit) & av->treebits);
        if (vbit != 0) {
          bin_index_t vidx = bit2idx(vbit);
          tbinptr* vbin = tbin_at(av, vidx);
          tchunkptr c = *vbin;
          do {
            CHUNK_SIZE_T csize = chunksize(c);
            leaf = c;
            if (csize < bestsize) {
              bestchunk = c;
              bestsize = csize;
            }
            c = leftmost_child(c);
          } while (c != 0);
          return use_treechunk(av, nb, bestchunk, bestsize, leaf);
        }
      }
    }
    
    /*
      If large enough, split off the chunk bordering the end of memory
      (held in av->top). This is called in accord with the best-fit
      search rule.  In effect, av->top is treated as larger (and thus
      less well fitting) than any other available chunk since it can
      be extended to be as large as necessary (up to system
      limitations).
      
      We require that av->top always exists (i.e., has size >=
      MINSIZE) after initialization, so if it would otherwise be
      exhuasted by current request, it is replenished. (The main
      reason for ensuring it exists is that we may need MINSIZE space
      to put in fenceposts in sysmalloc.)
    */
    
    if (av->top != 0) {
      mchunkptr topchunk = av->top;
      CHUNK_SIZE_T topsize = chunksize(topchunk);
      
      if (topsize >= nb + MINSIZE) {
        CHUNK_SIZE_T remainder_size = topsize - nb;
        mchunkptr remainder = chunk_at_offset(topchunk, nb);
        
        av->top = remainder;
        set_head(topchunk, nb | PREV_INUSE);
        set_head(remainder, remainder_size | PREV_INUSE);
        
        check_malloced_chunk(topchunk, nb);
        return chunk2mem(topchunk);
      }
      else if (have_fastchunks(av)) {
        malloc_consolidate(av);
      }
      else
        break;
    }  
    else
      break;
  }
  return sysmalloc(av, nb);
}

/*
  ------------------------------ free ------------------------------
*/


void fREe(Void_t* mem) {
  mstate av = get_malloc_state();

  mchunkptr p = mem2chunk(mem);

  if (mem != 0) {
    INTERNAL_SIZE_T rawsize = p->size;
    CHUNK_SIZE_T size = chunksize(p);
    check_inuse_chunk(p);

    /*
      If eligible, place chunk on a fastbin so it can be found
      and used quickly in malloc.
    */

    if ((CHUNK_SIZE_T)(size) <= (CHUNK_SIZE_T)(av->max_fast)

#if TRIM_FASTBINS
        /* 
           If TRIM_FASTBINS set, don't place chunks
           bordering top into fastbins
        */
        && (chunk_at_offset(p, size) != av->top)
#endif
        ) {

      mfastbinptr* fb;
      set_fastchunks(av);
      fb = &(av->fastbins[fastbin_index(size)]);
      p->fd = *fb;
      *fb = p;
    }

    else if ((rawsize & IS_MMAPPED) == 0) {
      mchunkptr nextchunk = chunk_at_offset(p, size);
      CHUNK_SIZE_T nextsize;

      if ((rawsize & PREV_INUSE) == 0) {
        CHUNK_SIZE_T prevsize = p->prev_size;
        size += prevsize;
        p = chunk_at_offset(p, -((long) prevsize));
        unlink_chunk(av, p, prevsize);
      }

      nextsize = chunksize(nextchunk);
      if (nextchunk == av->top) {
        size += nextsize;
        set_head(p, size | PREV_INUSE);
        av->top = p;
        if (size >= av->trim_threshold) {
          systrim(av, av->top_pad);
        }
      }
      else {
        if (!inuse_bit_at_offset(nextchunk, nextsize)) {
          size += nextsize;
          unlink_chunk(av, nextchunk, nextsize);
        }
        else
          set_head(nextchunk, nextsize);

        set_head(p, size | PREV_INUSE);
        set_foot(p, size);
        insert_chunk(av, p, size);
      }
    }
    else {
#if HAVE_MMAP
      int ret;
      INTERNAL_SIZE_T offset = p->prev_size;
      av->n_mmaps--;
      av->mmapped_mem -= (size + offset);
      ret = munmap((char*)p - offset, size + offset);
      /* munmap returns non-zero on failure */
      assert(ret == 0);
#endif
    }
  }
}

/*
  ------------------------- malloc_consolidate -------------------------

  malloc_consolidate tears down chunks held in fastbins.
*/

static void malloc_consolidate(mstate av) {
  int i;
  clear_fastchunks(av);

  for (i = 0; i < NFASTBINS; ++i) {
    mfastbinptr* fb = &(av->fastbins[i]);
    mchunkptr p = *fb;
   
    if (p != 0) {
      *fb = 0;
      do {
        mchunkptr nextp = p->fd;
        INTERNAL_SIZE_T rawsize = p->size;
        CHUNK_SIZE_T size = chunksize(p);
        mchunkptr nextchunk = chunk_at_offset(p, size);
        CHUNK_SIZE_T nextsize;

        if ((rawsize & PREV_INUSE) == 0) {
          CHUNK_SIZE_T prevsize = p->prev_size;
          size += prevsize;
          p = chunk_at_offset(p, -((long) prevsize));
          unlink_chunk(av, p, prevsize);
        }

        nextsize = chunksize(nextchunk);
        if (nextchunk == av->top) {
          size += nextsize;
          set_head(p, size | PREV_INUSE);
          av->top = p;
        }
        else {
          if (!inuse_bit_at_offset(nextchunk, nextsize)) {
            size += nextsize;
            unlink_chunk(av, nextchunk, nextsize);
          }
          else
            set_head(nextchunk, nextsize);
          
          set_head(p, size | PREV_INUSE);
          set_foot(p, size);

          insert_chunk(av, p, size);
        }
        p = nextp;
      } while (p != 0);
    }
  }
}


/*
  ------------------------------ realloc ------------------------------
*/


Void_t* rEALLOc(Void_t* oldmem, size_t bytes) {
  mstate av = get_malloc_state();

  INTERNAL_SIZE_T  nb;              /* padded request size */

  mchunkptr        oldp;            /* chunk corresponding to oldmem */
  CHUNK_SIZE_T     oldsize;         /* its size */

  mchunkptr        newp;            /* chunk to return */
  CHUNK_SIZE_T     newsize;         /* its size */
  Void_t*          newmem;          /* corresponding user mem */

  mchunkptr        next;            /* next contiguous chunk after oldp */

  mchunkptr        remainder;       /* extra space at end of newp */
  CHUNK_SIZE_T     remainder_size;  /* its size */

  CHUNK_SIZE_T     copysize;        /* bytes to copy */
  unsigned int     ncopies;         /* INTERNAL_SIZE_T words to copy */
  INTERNAL_SIZE_T* s;               /* copy source */ 
  INTERNAL_SIZE_T* d;               /* copy destination */


#ifdef REALLOC_ZERO_BYTES_FREES
  if (bytes == 0) {
    fREe(oldmem);
    return 0;
  }
#endif

  /* realloc of null is supposed to be same as malloc */
  if (oldmem == 0) return mALLOc(bytes);

  checked_request2size(bytes, nb);

  oldp    = mem2chunk(oldmem);
  oldsize = chunksize(oldp);

  check_inuse_chunk(oldp);

  if (!chunk_is_mmapped(oldp)) {

    if ((CHUNK_SIZE_T)(oldsize) >= (CHUNK_SIZE_T)(nb)) {
      /* already big enough; split below */
      newp = oldp;
      newsize = oldsize;
    }

    else {
      next = chunk_at_offset(oldp, oldsize);

      /* Try to expand forward into top */
      if (next == av->top &&
          (CHUNK_SIZE_T)(newsize = oldsize + chunksize(next)) >=
          (CHUNK_SIZE_T)(nb + MINSIZE)) {
        set_head_size(oldp, nb);
        av->top = chunk_at_offset(oldp, nb);
        set_head(av->top, (newsize - nb) | PREV_INUSE);
        return chunk2mem(oldp);
      }
      
      /* Try to expand forward into next chunk;  split off remainder below */
      else if (next != av->top && 
               !inuse(next) &&
               (CHUNK_SIZE_T)(newsize = oldsize + chunksize(next)) >=
               (CHUNK_SIZE_T)(nb)) {
        newp = oldp;
        unlink_chunk(av, next, chunksize(next));
      }

      /* allocate, copy, free */
      else {
        newmem = mALLOc(nb - MALLOC_ALIGN_MASK);
        if (newmem == 0)
          return 0; /* propagate failure */
      
        newp = mem2chunk(newmem);
        newsize = chunksize(newp);
        
        /*
          Avoid copy if newp is next chunk after oldp.
        */
        if (newp == next) {
          newsize += oldsize;
          newp = oldp;
        }
        else {
          /*
            Unroll copy of <= 36 bytes (72 if 8byte sizes)
            We know that contents have an odd number of
            INTERNAL_SIZE_T-sized words; minimally 3.
          */
          
          copysize = oldsize - SIZE_SZ;
          s = (INTERNAL_SIZE_T*)(oldmem);
          d = (INTERNAL_SIZE_T*)(newmem);
          ncopies = copysize / sizeof(INTERNAL_SIZE_T);
          assert(ncopies >= 3);
          
          if (ncopies > 9)
            MALLOC_COPY(d, s, copysize);
          
          else {
            *(d+0) = *(s+0);
            *(d+1) = *(s+1);
            *(d+2) = *(s+2);
            if (ncopies > 4) {
              *(d+3) = *(s+3);
              *(d+4) = *(s+4);
              if (ncopies > 6) {
                *(d+5) = *(s+5);
                *(d+6) = *(s+6);
                if (ncopies > 8) {
                  *(d+7) = *(s+7);
                  *(d+8) = *(s+8);
                }
              }
            }
          }
          
          fREe(oldmem);
          check_inuse_chunk(newp);
          return chunk2mem(newp);
        }
      }
    }

    /* If possible, free extra space in old or extended chunk */

    assert((CHUNK_SIZE_T)(newsize) >= (CHUNK_SIZE_T)(nb));

    remainder_size = newsize - nb;

    if (remainder_size < MINSIZE) { /* not enough extra to split off */
      set_head_size(newp, newsize);
      set_inuse_bit_at_offset(newp, newsize);
    }
    else { /* split remainder */
      remainder = chunk_at_offset(newp, nb);
      set_head_size(newp, nb);
      set_head(remainder, remainder_size | PREV_INUSE);
      /* Mark remainder as inuse so free() won't complain */
      set_inuse_bit_at_offset(remainder, remainder_size);
      fREe(chunk2mem(remainder)); 
    }

    check_inuse_chunk(newp);
    return chunk2mem(newp);
  }

  /*
    Handle mmap cases
  */

  else {
#if HAVE_MMAP

#if HAVE_MREMAP
    INTERNAL_SIZE_T offset = oldp->prev_size;
    size_t pagemask = av->pagesize - 1;
    char *cp;
    CHUNK_SIZE_T  sum;
    
    /* Note the extra SIZE_SZ overhead */
    newsize = (nb + offset + SIZE_SZ + pagemask) & ~pagemask;

    /* don't need to remap if still within same page */
    if (oldsize == newsize - offset) 
      return oldmem;

    cp = (char*)mremap((char*)oldp - offset, oldsize + offset, newsize, 1);
    
    if (cp != (char*)MORECORE_FAILURE) {

      newp = (mchunkptr)(cp + offset);
      set_head(newp, (newsize - offset)|IS_MMAPPED);
      
      assert(aligned_OK(chunk2mem(newp)));
      assert((newp->prev_size == offset));
      
      /* update statistics */
      sum = av->mmapped_mem += newsize - oldsize;
      if (sum > (CHUNK_SIZE_T)(av->max_mmapped_mem)) 
        av->max_mmapped_mem = sum;
      sum += av->sbrked_mem;
      if (sum > (CHUNK_SIZE_T)(av->max_total_mem)) 
        av->max_total_mem = sum;
      
      return chunk2mem(newp);
    }
#endif

    /* Note the extra SIZE_SZ overhead. */
    if ((CHUNK_SIZE_T)(oldsize) >= (CHUNK_SIZE_T)(nb + SIZE_SZ)) 
      newmem = oldmem; /* do nothing */
    else {
      /* Must alloc, copy, free. */
      newmem = mALLOc(nb - MALLOC_ALIGN_MASK);
      if (newmem != 0) {
        MALLOC_COPY(newmem, oldmem, oldsize - 2*SIZE_SZ);
        fREe(oldmem);
      }
    }
    return newmem;

#else 
    /* If !HAVE_MMAP, but chunk_is_mmapped, user must have overwritten mem */
    check_malloc_state(av);
    MALLOC_FAILURE_ACTION;
    return 0;
#endif
  }
}

/*
  ------------------------------ memalign ------------------------------
*/

Void_t* mEMALIGn(size_t alignment, size_t bytes) {
  INTERNAL_SIZE_T nb;             /* padded  request size */
  char*           m;              /* memory returned by malloc call */
  mchunkptr       p;              /* corresponding chunk */
  char*           brk;            /* alignment point within p */
  mchunkptr       newp;           /* chunk to return */
  INTERNAL_SIZE_T newsize;        /* its size */
  INTERNAL_SIZE_T leadsize;       /* leading space before alignment point */
  mchunkptr       remainder;      /* spare room at end to split off */
  CHUNK_SIZE_T    remainder_size; /* its size */
  INTERNAL_SIZE_T size;

  /* If need less alignment than we give anyway, just relay to malloc */

  if (alignment <= MALLOC_ALIGNMENT) return mALLOc(bytes);

  /* Otherwise, ensure that it is at least a minimum chunk size */

  if (alignment <  MINSIZE) alignment = MINSIZE;

  /* Make sure alignment is power of 2 (in case MINSIZE is not).  */
  if ((alignment & (alignment - 1)) != 0) {
    size_t a = MALLOC_ALIGNMENT * 2;
    while ((CHUNK_SIZE_T)a < (CHUNK_SIZE_T)alignment) a <<= 1;
    alignment = a;
  }

  checked_request2size(bytes, nb);

  /*
    Strategy: find a spot within that chunk that meets the alignment
    request, and then possibly free the leading and trailing space.
  */


  /* Call malloc with worst case padding to hit alignment. */

  m  = (char*)(mALLOc(nb + alignment + MINSIZE));

  if (m == 0) return 0; /* propagate failure */

  p = mem2chunk(m);

  if ((((PTR_UINT)(m)) % alignment) != 0) { /* misaligned */

    /*
      Find an aligned spot inside chunk.  Since we need to give back
      leading space in a chunk of at least MINSIZE, if the first
      calculation places us at a spot with less than MINSIZE leader,
      we can move to the next aligned spot -- we've allocated enough
      total room so that this is always possible.
    */

    brk = (char*)mem2chunk((PTR_UINT)(((PTR_UINT)(m + alignment - 1)) &
                           -((signed long) alignment)));
    if ((CHUNK_SIZE_T)(brk - (char*)(p)) < MINSIZE)
      brk += alignment;

    newp = (mchunkptr)brk;
    leadsize = brk - (char*)(p);
    newsize = chunksize(p) - leadsize;

    /* For mmapped chunks, just adjust offset */
    if (chunk_is_mmapped(p)) {
      newp->prev_size = p->prev_size + leadsize;
      set_head(newp, newsize|IS_MMAPPED);
      return chunk2mem(newp);
    }

    /* Otherwise, give back leader, use the rest */
    set_head(newp, newsize | PREV_INUSE);
    set_inuse_bit_at_offset(newp, newsize);
    set_head_size(p, leadsize);
    fREe(chunk2mem(p));
    p = newp;

    assert (newsize >= nb &&
            (((PTR_UINT)(chunk2mem(p))) % alignment) == 0);
  }

  /* Also give back spare room at the end */
  if (!chunk_is_mmapped(p)) {
    size = chunksize(p);
    if ((CHUNK_SIZE_T)(size) > (CHUNK_SIZE_T)(nb + MINSIZE)) {
      remainder_size = size - nb;
      remainder = chunk_at_offset(p, nb);
      set_head(remainder, remainder_size | PREV_INUSE);
      set_head_size(p, nb);
      fREe(chunk2mem(remainder));
    }
  }

  check_inuse_chunk(p);
  return chunk2mem(p);
}

/*
  ------------------------------ calloc ------------------------------
*/

Void_t* cALLOc(size_t n_elements, size_t elem_size) {
  Void_t* mem = mALLOc(n_elements * elem_size);

  if (mem != 0) {
    mchunkptr p = mem2chunk(mem);
    INTERNAL_SIZE_T* d = (INTERNAL_SIZE_T*)mem;

    if (!chunk_is_mmapped(p))
    {  
      /*
        Unroll clear of <= 36 bytes (72 if 8byte sizes)
        We know that contents have an odd number of
        INTERNAL_SIZE_T-sized words; minimally 3.
      */

      CHUNK_SIZE_T clearsize = chunksize(p) - SIZE_SZ;
      CHUNK_SIZE_T nclears = clearsize / sizeof(INTERNAL_SIZE_T);
      assert(nclears >= 3);

      if (nclears > 9)
        MALLOC_ZERO(d, clearsize);

      else {
        *(d+0) = 0;
        *(d+1) = 0;
        *(d+2) = 0;
        if (nclears > 4) {
          *(d+3) = 0;
          *(d+4) = 0;
          if (nclears > 6) {
            *(d+5) = 0;
            *(d+6) = 0;
            if (nclears > 8) {
              *(d+7) = 0;
              *(d+8) = 0;
            }
          }
        }
      }
    }
#if ! MMAP_CLEARS
    else
    {
      /*
        Note the additional SIZE_SZ
      */
      CHUNK_SIZE_T clearsize = chunksize(p) - 2*SIZE_SZ;
      MALLOC_ZERO(d, clearsize);
    }
#endif
  }
  return mem;
}

/*
  ------------------------------ cfree ------------------------------
*/

void cFREe(Void_t *mem) {
  fREe(mem);
}

/*
  ------------------------- independent_calloc -------------------------
*/


Void_t** iCALLOc(size_t n_elements, size_t elem_size, Void_t* chunks[]) {
  size_t sz = elem_size; /* serves as 1-element array */
  /* opts arg of 3 means all elements are same size, and should be cleared */
  return iALLOc(n_elements, &sz, 3, chunks);
}

/*
  ------------------------- independent_comalloc -------------------------
*/

Void_t** iCOMALLOc(size_t n_elements, size_t sizes[], Void_t* chunks[]) {
  return iALLOc(n_elements, sizes, 0, chunks);
}


/*
  ------------------------------ ialloc ------------------------------
  ialloc provides common support for independent_X routines, handling all of
  the combinations that can result.

  The opts arg has:
    bit 0 set if all elements are same size (using sizes[0])
    bit 1 set if elements should be zeroed
*/


static Void_t** iALLOc(size_t n_elements, 
                       size_t* sizes,  
                       int opts,
                       Void_t* chunks[]) {
  mstate av = get_malloc_state();
  INTERNAL_SIZE_T element_size;   /* chunksize of each element, if all same */
  INTERNAL_SIZE_T contents_size;  /* total size of elements */
  INTERNAL_SIZE_T array_size;     /* request size of pointer array */
  Void_t*         mem;            /* malloced aggregate space */
  mchunkptr       p;              /* corresponding chunk */
  CHUNK_SIZE_T remainder_size; /* remaining bytes while splitting */
  Void_t**        marray;         /* either "chunks" or malloced ptr array */
  mchunkptr       array_chunk;    /* chunk for malloced ptr array */
  unsigned int    mprops;         /* to disable mmap */
  CHUNK_SIZE_T size;           
  size_t          i;

  ensure_initialization(av);

  /* compute array length, if needed */
  if (chunks != 0) {
    if (n_elements == 0)
      return chunks; /* nothing to do */
    marray = chunks;
    array_size = 0;
  }
  else {
    /* if empty req, must still return chunk representing empty array */
    if (n_elements == 0) 
      return (Void_t**) mALLOc(0);
    marray = 0;
    array_size = request2size(n_elements * (sizeof(Void_t*)));
  }

  /* compute total element size */
  if (opts & 0x1) { /* all-same-size */
    element_size = request2size(*sizes);
    contents_size = n_elements * element_size;
  }
  else { /* add up all the sizes */
    element_size = 0;
    contents_size = 0;
    for (i = 0; i != n_elements; ++i) 
      contents_size += request2size(sizes[i]);     
  }

  /* subtract out alignment bytes from total to minimize overallocation */
  size = contents_size + array_size - MALLOC_ALIGN_MASK;
  
  /* 
     Allocate the aggregate chunk.
     But first disable mmap so malloc won't use it, since
     we would not be able to later free/realloc space internal
     to a segregated mmap region.
 */
  
  mprops = av->sysctl;   /* disable mmap */
  disable_mmap(av);
  mem = mALLOc(size);
  av->sysctl = mprops; /* reset mmap */
  if (mem == 0) 
    return 0;

  p = mem2chunk(mem);
  assert(!chunk_is_mmapped(p)); 
  remainder_size = chunksize(p);


  if (opts & 0x2) {       /* optionally clear the elements */
    MALLOC_ZERO(mem, remainder_size - SIZE_SZ - array_size);
  }

  /* If not provided, allocate the pointer array as final part of chunk */
  if (marray == 0) {
    array_chunk = chunk_at_offset(p, contents_size);
    marray = (Void_t**) (chunk2mem(array_chunk));
    set_head(array_chunk, (remainder_size - contents_size) | PREV_INUSE);
    remainder_size = contents_size;
  }

  /* split out elements */
  for (i = 0; ; ++i) {
    marray[i] = chunk2mem(p);
    if (i != n_elements-1) {
      if (element_size != 0) 
        size = element_size;
      else
        size = request2size(sizes[i]);          
      remainder_size -= size;
      set_head(p, size | PREV_INUSE);
      p = chunk_at_offset(p, size);
    }
    else { /* the final element absorbs any overallocation slop */
      set_head(p, remainder_size | PREV_INUSE);
      break;
    }
  }

#if DEBUG
  if (marray != chunks) {
    /* final element must have exactly exhausted chunk */
    if (element_size != 0) 
      assert(remainder_size == element_size);
    else
      assert(remainder_size == request2size(sizes[i]));
    check_inuse_chunk(mem2chunk(marray));
  }

  for (i = 0; i != n_elements; ++i)
    check_inuse_chunk(mem2chunk(marray[i]));
#endif

  return marray;
}


/*
  ------------------------------ valloc ------------------------------
*/

Void_t* vALLOc(size_t bytes) {
  mstate av = get_malloc_state();
  ensure_initialization(av);
  return mEMALIGn(av->pagesize, bytes);
}

/*
  ------------------------------ pvalloc ------------------------------
*/


Void_t* pVALLOc(size_t bytes) {
  mstate av = get_malloc_state();
  size_t pagesz;

  ensure_initialization(av);
  pagesz = av->pagesize;
  return mEMALIGn(pagesz, (bytes + pagesz - 1) & ~(pagesz - 1));
}
   

/*
  ------------------------------ malloc_trim ------------------------------
*/

int mTRIm(size_t pad) {
  mstate av = get_malloc_state();
  return systrim(av, pad);
}


/*
  ------------------------- malloc_usable_size -------------------------
*/

size_t mUSABLe(Void_t* mem) {
  mchunkptr p;
  if (mem != 0) {
    p = mem2chunk(mem);
    if (chunk_is_mmapped(p))
      return chunksize(p) - 2*SIZE_SZ;
    else if (inuse(p))
      return chunksize(p) - SIZE_SZ;
  }
  return 0;
}

/*
  ------------------------------ mallinfo ------------------------------
*/

/*
  Recursive helper function for mallinfo
*/

static void count_tree_blocks(tchunkptr t, int* pcount, INTERNAL_SIZE_T* pavail) {
  while (t != 0) {
    tchunkptr p = t->bk;
    do {
      (*pcount)++;
      *pavail += chunksize(p);
      p = p->bk;
    } while (p != t);
    if (t->child[0] != 0)
      count_tree_blocks(t->child[0], pcount, pavail);
    t = t->child[1];
  }
}
    


struct mallinfo mALLINFo()
{
  mstate av = get_malloc_state();
  struct mallinfo mi;
  INTERNAL_SIZE_T avail;
  INTERNAL_SIZE_T topsize;
  int nblocks;
  INTERNAL_SIZE_T fastavail;
  int nfastblocks;
  mchunkptr p;

  if (av->top == 0) {
    avail = 0;
    topsize = 0;
    nblocks = 0;
  }
  else {
    int i;
    check_malloc_state(av);
    
    topsize = chunksize(av->top);
    avail = topsize;
    nblocks = 1;  /* top always exists */

    /* traverse fastbins */
    nfastblocks = 0;
    fastavail = 0;
    
    for (i = 0; i < NFASTBINS; ++i) {
      for (p = av->fastbins[i]; p != 0; p = p->fd) {
        ++nfastblocks;
        fastavail += chunksize(p);
      }
    }
    
    avail += fastavail;
    
    /* traverse small bins */
    for (i = 2; i < NBINS; ++i) {
      mbinptr b = bin_at(av, i);
      mchunkptr p;
      for (p = b->bk; p != b; p = p->bk) {
        ++nblocks;
        avail += chunksize(p);
      }
    }
    
    /* traverse tree bins */
    for (i = 0; i < NBINS; ++i) {
      tchunkptr t = *(tbin_at(av, i));
      if (t != 0)
        count_tree_blocks(t, &nblocks, &avail);
    }
  }

  mi.smblks = nfastblocks;
  mi.smblks = 0;
  mi.ordblks = nblocks;
  mi.fordblks = avail;
  mi.uordblks = av->sbrked_mem - avail;
  mi.arena = av->sbrked_mem;
  mi.hblks = av->n_mmaps;
  mi.hblkhd = av->mmapped_mem;
  mi.fsmblks = 0;
  mi.keepcost = topsize;
  mi.usmblks = av->max_total_mem;
  return mi;
}

/*
  ------------------------------ malloc_stats ------------------------------
*/

void mSTATs() {
  struct mallinfo mi = mALLINFo();

#ifdef WIN32
  {
    CHUNK_SIZE_T  free, reserved, committed;
    vminfo (&free, &reserved, &committed);
    fprintf(stderr, "free bytes       = %10lu\n", 
            free);
    fprintf(stderr, "reserved bytes   = %10lu\n", 
            reserved);
    fprintf(stderr, "committed bytes  = %10lu\n", 
            committed);
  }
#endif


  fprintf(stderr, "max system bytes = %10lu\n",
          (CHUNK_SIZE_T)(mi.usmblks));
  fprintf(stderr, "system bytes     = %10lu\n",
          (CHUNK_SIZE_T)(mi.arena + mi.hblkhd));
  fprintf(stderr, "in use bytes     = %10lu\n",
          (CHUNK_SIZE_T)(mi.uordblks + mi.hblkhd));

#if 0
  fprintf(stderr, "n0     = %10u\n", n0);
  fprintf(stderr, "n1     = %10u\n", n1);
  fprintf(stderr, "n2     = %10u\n", n2);
  fprintf(stderr, "n3     = %10u\n", n3);
  fprintf(stderr, "n4     = %10u\n", n4);
  fprintf(stderr, "n5     = %10u\n", n5);
  fprintf(stderr, "n6     = %10u\n", n6);
  fprintf(stderr, "n7     = %10u\n", n7);
  fprintf(stderr, "n8     = %10u\n", n8);
#endif


#ifdef WIN32 
  {
    CHUNK_SIZE_T  kernel, user;
    if (cpuinfo (TRUE, &kernel, &user)) {
      fprintf(stderr, "kernel ms        = %10lu\n", 
              kernel);
      fprintf(stderr, "user ms          = %10lu\n", 
              user);
    }
  }
#endif
}


/*
  ------------------------------ mallopt ------------------------------
*/

int mALLOPt(int param_number, int value) {
  mstate av = get_malloc_state();

  ensure_initialization(av);

  switch(param_number) {
  case M_MXFAST:
    malloc_consolidate(av);
    if (value >= 0 && value <= MAX_FAST_SIZE) {
      set_max_fast(av, value);
      return 1;
    }
    else
      return 0;

  case M_TRIM_THRESHOLD:
    av->trim_threshold = value;
    return 1;

  case M_TOP_PAD:
    av->top_pad = value;
    return 1;

  case M_MMAP_THRESHOLD:
    av->mmap_threshold = value;
    return 1;

  case M_MMAP_MAX:
#if !HAVE_MMAP
    if (value != 0)
      return 0;
#endif
    av->n_mmaps_max = value;
    return 1;

  default:
    return 0;
  }
}

/* ----------- Routines dealing with system allocation -------------- */

#if HAVE_MMAP
static mchunkptr mmap_malloc(mstate av, INTERNAL_SIZE_T nb) {
  char* mm;                       /* return value from mmap call*/
  CHUNK_SIZE_T    sum;            /* for updating stats */
  mchunkptr       p;              /* the allocated/returned chunk */
  long            size;           
  INTERNAL_SIZE_T front_misalign; /* unusable bytes at front of new space */
  long            correction;     
  size_t          pagemask  = av->pagesize - 1;

  /*
    Round up size to nearest page.  For mmapped chunks, the overhead
    is one SIZE_SZ unit larger than for normal chunks, because there
    is no following chunk whose prev_size field could be used.
  */
  size = (nb + SIZE_SZ + MALLOC_ALIGN_MASK + pagemask) & ~pagemask;
  
  /* Don't try if size wraps around 0 */
  if ((CHUNK_SIZE_T)(size) > (CHUNK_SIZE_T)(nb)) {
    
    mm = (char*)(MMAP(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE));
    
    if (mm != (char*)(MORECORE_FAILURE)) {
      
      /*
        The offset to the start of the mmapped region is stored
        in the prev_size field of the chunk. This allows us to adjust
        returned start address to meet alignment requirements here 
        and in memalign(), and still be able to compute proper
        address argument for later munmap in free() and realloc().
      */
      
      front_misalign = (INTERNAL_SIZE_T)chunk2mem(mm) & MALLOC_ALIGN_MASK;
      if (front_misalign > 0) {
        correction = MALLOC_ALIGNMENT - front_misalign;
        p = (mchunkptr)(mm + correction);
        p->prev_size = correction;
        set_head(p, (size - correction) |IS_MMAPPED);
      }
      else {
        p = (mchunkptr)mm;
        p->prev_size = 0;
        set_head(p, size|IS_MMAPPED);
      }
      
      /* update statistics */
      
      if (++av->n_mmaps > av->max_n_mmaps) 
        av->max_n_mmaps = av->n_mmaps;
      
      sum = av->mmapped_mem += size;
      if (sum > (CHUNK_SIZE_T)(av->max_mmapped_mem)) 
        av->max_mmapped_mem = sum;
      sum += av->sbrked_mem;
      if (sum > (CHUNK_SIZE_T)(av->max_total_mem)) 
        av->max_total_mem = sum;
      
      check_chunk(p);
      
      return p;
    }
  }
  return 0;
}
#endif


/*
  sysmalloc handles malloc cases requiring more memory from the system.
  On entry, it is assumed that av->top does not have enough
  space to service request for nb bytes, thus requiring that av->top
  be extended or replaced.
*/

static Void_t* sysmalloc(mstate av, CHUNK_SIZE_T nb) {
  mchunkptr       old_top;        /* incoming value of av->top */
  INTERNAL_SIZE_T old_size;       /* its size */
  char*           old_end;        /* its end address */

  long            size;           /* arg to first MORECORE or mmap call */
  char*           brk;            /* return value from MORECORE */

  long            correction;     /* arg to 2nd MORECORE call */
  char*           snd_brk;        /* 2nd return val */

  INTERNAL_SIZE_T front_misalign; /* unusable bytes at front of new space */
  INTERNAL_SIZE_T end_misalign;   /* partial page left at end of new space */
  char*           aligned_brk;    /* aligned offset into brk */

  mchunkptr       p;              /* the allocated/returned chunk */
  mchunkptr       remainder;      /* remainder from allocation */
  CHUNK_SIZE_T    remainder_size; /* its size */

  CHUNK_SIZE_T    sum;            /* for updating stats */

  size_t          pagemask;

  /*
    Initialize av if necessary 
   */
  if (av->top == 0) {
    malloc_init_state(av);
    /* to allow call solely for initialization */
    if (nb == 0)
      return 0;
  }


#if HAVE_MMAP
  /*
    If have mmap, and the request size meets the mmap threshold, and
    the system supports mmap, and there are few enough currently
    allocated mmapped regions, try to directly map this request
    rather than expanding top.
  */

  if ((CHUNK_SIZE_T)(nb) >= (CHUNK_SIZE_T)(av->mmap_threshold) &&
      (av->n_mmaps < av->n_mmaps_max) &&
      !mmap_disabled(av)) {
    Void_t* mp = mmap_malloc(av, nb);
    if (mp != 0)
      return chunk2mem(mp);
  }
#endif


  pagemask = av->pagesize - 1;

  /* Record incoming configuration of top */

  old_top  = av->top;
  old_size = chunksize(old_top);
  old_end  = (char*)(chunk_at_offset(old_top, old_size));

  brk = snd_brk = (char*)(MORECORE_FAILURE); 

  /* 
     If not the first time through, we require old_size to be
     at least MINSIZE and to have prev_inuse set.
  */

  assert((old_top == (mchunkptr)(&(av->initial_top)) && old_size == 0) || 
         ((CHUNK_SIZE_T) (old_size) >= MINSIZE &&
          prev_inuse(old_top)));

  /* Precondition: not enough current space to satisfy nb request */
  assert((CHUNK_SIZE_T)(old_size) < (CHUNK_SIZE_T)(nb + MINSIZE));

  /* Request enough space for nb + pad + overhead */

  size = nb + av->top_pad + MINSIZE;

  /*
    If contiguous, we can subtract out existing space that we hope to
    combine with new space. We add it back later only if
    we don't actually get contiguous space.
  */

  if (contiguous(av))
    size -= old_size;

  /*
    Round to a multiple of page size.
    If MORECORE is not contiguous, this ensures that we only call it
    with whole-page arguments.  And if MORECORE is contiguous and
    this is not first time through, this preserves page-alignment of
    previous calls. Otherwise, we correct to page-align below.
  */

  size = (size + pagemask) & ~pagemask;

  /*
    Don't try to call MORECORE if argument is so big as to appear
    negative. Note that since mmap takes size_t arg, it may succeed
    below even if we cannot call MORECORE.
  */

  if (size > 0) 
    brk = (char*)(MORECORE(size));

  /*
    If have mmap, try using it as a backup when MORECORE fails or
    cannot be used. This is worth doing on systems that have "holes" in
    address space, so sbrk cannot extend to give contiguous space, but
    space is available elsewhere.  Note that we ignore mmap max count
    and threshold limits, since the space will not be used as a
    segregated mmap region.
  */

#if HAVE_MMAP
  if (brk == (char*)(MORECORE_FAILURE)) {

    /* Cannot merge with old top, so add its size back in */
    if (contiguous(av))
      size = (size + old_size + pagemask) & ~pagemask;

    /* If we are relying on mmap as backup, then use larger units */
    if ((CHUNK_SIZE_T)(size) < (CHUNK_SIZE_T)(MMAP_AS_MORECORE_SIZE))
      size = MMAP_AS_MORECORE_SIZE;

    /* Don't try if size wraps around 0 */
    if ((CHUNK_SIZE_T)(size) > (CHUNK_SIZE_T)(nb)) {

      brk = (char*)(MMAP(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE));
      
      if (brk != (char*)(MORECORE_FAILURE)) {
        
        /* We do not need, and cannot use, another sbrk call to find end */
        snd_brk = brk + size;
        
        /* 
           Record that we no longer have a contiguous sbrk region. 
           After the first time mmap is used as backup, we do not
           ever rely on contiguous space since this could incorrectly
           bridge regions.
        */
        set_noncontiguous(av);
      }
    }
  }
#endif

  if (brk != (char*)(MORECORE_FAILURE)) {
    av->sbrked_mem += size;

    /*
      If MORECORE extends previous space, we can likewise extend top size.
    */
    
    if (brk == old_end && snd_brk == (char*)(MORECORE_FAILURE)) {
      set_head(old_top, (size + old_size) | PREV_INUSE);
    }

    /*
      Otherwise, make adjustments:
      
      * If the first time through or noncontiguous, we need to call sbrk
        just to find out where the end of memory lies.

      * We need to ensure that all returned chunks from malloc will meet
        MALLOC_ALIGNMENT

      * If there was an intervening foreign sbrk, we need to adjust sbrk
        request size to account for fact that we will not be able to
        combine new space with existing space in old_top.

      * Almost all systems internally allocate whole pages at a time, in
        which case we might as well use the whole last page of request.
        So we allocate enough more memory to hit a page boundary now,
        which in turn causes future contiguous calls to page-align.
    */
    
    else {
      front_misalign = 0;
      end_misalign = 0;
      correction = 0;
      aligned_brk = brk;

      /*
        If MORECORE returns an address lower than we have seen before,
        we know it isn't really contiguous.  This and some subsequent
        checks help cope with non-conforming MORECORE functions and
        the presence of "foreign" calls to MORECORE from outside of
        malloc or by other threads.  We cannot guarantee to detect
        these in all cases, but cope with the ones we do detect.
      */
      if (contiguous(av) && old_size != 0 && brk < old_end) {
        set_noncontiguous(av);
      }
      
      /* handle contiguous cases */
      if (contiguous(av)) { 

        /* 
           We can tolerate forward non-contiguities here (usually due
           to foreign calls) but treat them as part of our space for
           stats reporting.
        */
        if (old_size != 0) 
          av->sbrked_mem += brk - old_end;
        
        /* Guarantee alignment of first new chunk made from this space */

        front_misalign = (INTERNAL_SIZE_T)chunk2mem(brk) & MALLOC_ALIGN_MASK;
        if (front_misalign > 0) {

          /*
            Skip over some bytes to arrive at an aligned position.
            We don't need to specially mark these wasted front bytes.
            They will never be accessed anyway because
            prev_inuse of av->top (and any chunk created from its start)
            is always true after initialization.
          */

          correction = MALLOC_ALIGNMENT - front_misalign;
          aligned_brk += correction;
        }
        
        /*
          If this isn't adjacent to existing space, then we will not
          be able to merge with old_top space, so must add to 2nd request.
        */
        
        correction += old_size;
        
        /* Extend the end address to hit a page boundary */
        end_misalign = (INTERNAL_SIZE_T)(brk + size + correction);
        correction += ((end_misalign + pagemask) & ~pagemask) - end_misalign;
        
        assert(correction >= 0);
        snd_brk = (char*)(MORECORE(correction));
        
        if (snd_brk == (char*)(MORECORE_FAILURE)) {
          /*
            If can't allocate correction, try to at least find out current
            brk.  It might be enough to proceed without failing.
          */
          correction = 0;
          snd_brk = (char*)(MORECORE(0));
        }
        else if (snd_brk < brk) {
          /*
            If the second call gives noncontiguous space even though
            it says it won't, the only course of action is to ignore
            results of second call, and conservatively estimate where
            the first call left us. Also set noncontiguous, so this
            won't happen again, leaving at most one hole.
            
            Note that this check is intrinsically incomplete.  Because
            MORECORE is allowed to give more space than we ask for,
            there is no reliable way to detect a noncontiguity
            producing a forward gap for the second call.
          */
          snd_brk = brk + size;
          correction = 0;
          set_noncontiguous(av);
        }

      }
      
      /* handle non-contiguous cases */
      else { 
        /* MORECORE/mmap must correctly align */
        assert(aligned_OK(chunk2mem(brk)));
        
        /* Find out current end of memory */
        if (snd_brk == (char*)(MORECORE_FAILURE)) {
          snd_brk = (char*)(MORECORE(0));
          av->sbrked_mem += snd_brk - brk - size;
        }
      }
      
      /* Adjust top based on results of second sbrk */
      if (snd_brk != (char*)(MORECORE_FAILURE)) {
        av->top = (mchunkptr)aligned_brk;
        set_head(av->top, (snd_brk - aligned_brk + correction) | PREV_INUSE);
        av->sbrked_mem += correction;
     
        /*
          If not the first time through, we either have a
          gap due to foreign sbrk or a non-contiguous region.  Insert a
          double fencepost at old_top to prevent consolidation with space
          we don't own. These fenceposts are artificial chunks that are
          marked as inuse and are in any case too small to use.  We need
          two to make sizes and alignments work out.
        */
   
        if (old_size != 0) {
          /* 
             Shrink old_top to insert fenceposts, keeping size a
             multiple of MALLOC_ALIGNMENT. We know there is at least
             enough space in old_top to do this.
          */
          old_size = (old_size - 3*SIZE_SZ) & ~MALLOC_ALIGN_MASK;
          set_head(old_top, old_size | PREV_INUSE);
          
          /*
            Note that the following assignments completely overwrite
            old_top when old_size was previously MINSIZE.  This is
            intentional. We need the fencepost, even if old_top
            otherwise gets lost.
          */
          chunk_at_offset(old_top, old_size          )->size =
            SIZE_SZ|PREV_INUSE;

          chunk_at_offset(old_top, old_size + SIZE_SZ)->size =
            SIZE_SZ|PREV_INUSE;

          /* 
             If possible, release the rest, suppressing trimming.
          */
          if (old_size >= MINSIZE) {
            unsigned int mprops = av->sysctl;
            disable_trim(av);
            fREe(chunk2mem(old_top));
            av->sysctl = mprops;
          }
        }
      }
    }
    
    /* Update statistics */
    sum = av->sbrked_mem;
    if (sum > (CHUNK_SIZE_T)(av->max_sbrked_mem))
      av->max_sbrked_mem = sum;
    
    sum += av->mmapped_mem;
    if (sum > (CHUNK_SIZE_T)(av->max_total_mem))
      av->max_total_mem = sum;

    
    /* finally, do the allocation */

    p = av->top;
    size = chunksize(p);
    
    /* check that one of the above allocation paths succeeded */
    if ((CHUNK_SIZE_T)(size) >= (CHUNK_SIZE_T)(nb + MINSIZE)) {
      remainder_size = size - nb;
      remainder = chunk_at_offset(p, nb);
      av->top = remainder;
      set_head(p, nb | PREV_INUSE);
      set_head(remainder, remainder_size | PREV_INUSE);
      check_malloced_chunk(p, nb);
      check_malloc_state(av);
      return chunk2mem(p);
    }

  }

  /* catch all failure paths */
  check_malloc_state(av);
  MALLOC_FAILURE_ACTION;
  return 0;
}


/*
  systrim is an inverse of sorts to sysmalloc.  It gives memory back
  to the system (via negative arguments to sbrk) if there is unused
  memory at the `high' end of the malloc pool. It is called
  automatically by free() when top space exceeds the trim
  threshold. It is also called by the public malloc_trim routine.  It
  returns 1 if it actually released any memory, else 0.
*/

static int systrim(mstate av, size_t pad) {
  long  top_size;        /* Amount of top-most memory */
  long  extra;           /* Amount to release */
  long  released;        /* Amount actually released */
  char* current_brk;     /* address returned by pre-check sbrk call */
  char* new_brk;         /* address returned by post-check sbrk call */
  size_t pagesz;

  ensure_initialization(av);

  if (have_fastchunks(av)) 
    malloc_consolidate(av);

  if (!trim_disabled(av)) {
    
#ifndef MORECORE_CANNOT_TRIM
    
    pagesz = av->pagesize;
    top_size = chunksize(av->top);
    
    /* Release in pagesize units, keeping at least one page */
    extra = ((top_size - pad - MINSIZE + (pagesz-1)) / pagesz - 1) * pagesz;
    
    if (extra > 0) {
      
      /*
        Only proceed if end of memory is where we last set it.
        This avoids problems if there were foreign sbrk calls.
      */
      current_brk = (char*)(MORECORE(0));
      if (current_brk == (char*)(av->top) + top_size) {
        
        /*
          Attempt to release memory. We ignore MORECORE return value,
          and instead call again to find out where new end of memory is.
          This avoids problems if first call releases less than we asked,
          of if failure somehow altered brk value. (We could still
          encounter problems if it altered brk in some very bad way,
          but the only thing we can do is adjust anyway, which will cause
          some downstream failure.)
        */
        
        MORECORE(-extra);
        new_brk = (char*)(MORECORE(0));
        
        if (new_brk != (char*)MORECORE_FAILURE) {
          released = (long)(current_brk - new_brk);
          
          if (released != 0) {
            /* Success. Adjust top. */
            av->sbrked_mem -= released;
            set_head(av->top, (top_size - released) | PREV_INUSE);
            check_malloc_state(av);
            return 1;
          }
        }
      }
    }
  }
#endif
  return 0;
}


/* 
  -------------------- Alternative MORECORE functions --------------------
*/


/*
  General Requirements for MORECORE.

  The MORECORE function must have the following properties:

  If MORECORE_CONTIGUOUS is false:

    * MORECORE must allocate in multiples of pagesize. It will
      only be called with arguments that are multiples of pagesize.

    * MORECORE(0) must return an address that is at least 
      MALLOC_ALIGNMENT aligned. (Page-aligning always suffices.)

  else (i.e. If MORECORE_CONTIGUOUS is true):

    * Consecutive calls to MORECORE with positive arguments
      return increasing addresses, indicating that space has been
      contiguously extended. 

    * MORECORE need not allocate in multiples of pagesize.
      Calls to MORECORE need not have args of multiples of pagesize.

    * MORECORE need not page-align.

  In either case:

    * MORECORE may allocate more memory than requested. (Or even less,
      but this will generally result in a malloc failure.)

    * MORECORE must not allocate memory when given argument zero, but
      instead return one past the end address of memory from previous
      nonzero call. This malloc does NOT call MORECORE(0)
      until at least one call with positive arguments is made, so
      the initial value returned is not important.

    * Even though consecutive calls to MORECORE need not return contiguous
      addresses, it must be OK for malloc'ed chunks to span multiple
      regions in those cases where they do happen to be contiguous.

    * MORECORE need not handle negative arguments -- it may instead
      just return MORECORE_FAILURE when given negative arguments.
      Negative arguments are always multiples of pagesize. MORECORE
      must not misinterpret negative args as large positive unsigned
      args. You can suppress all such calls from even occurring by defining
      MORECORE_CANNOT_TRIM,

  There is some variation across systems about the type of the
  argument to sbrk/MORECORE. If size_t is unsigned, then it cannot
  actually be size_t, because sbrk supports negative args, so it is
  normally the signed type of the same width as size_t (sometimes
  declared as "intptr_t", and sometimes "ptrdiff_t").  It doesn't much
  matter though. Internally, we use "long" as arguments, which should
  work across all reasonable possibilities.

  Additionally, if MORECORE ever returns failure for a positive
  request, and HAVE_MMAP is true, then mmap is used as a noncontiguous
  system allocator. This is a useful backup strategy for systems with
  holes in address spaces -- in this case sbrk cannot contiguously
  expand the heap, but mmap may be able to map noncontiguous space.

  If you'd like mmap to ALWAYS be used, you can define MORECORE to be
  a function that always returns MORECORE_FAILURE.

  Malloc only has limited ability to detect failures of MORECORE
  to supply contiguous space when it says it can. In particular,
  multithreaded programs that do not use locks may result in
  rece conditions across calls to MORECORE that result in gaps
  that cannot be detected as such, and subsequent corruption.

  If you are using this malloc with something other than sbrk (or its
  emulation) to supply memory regions, you probably want to set
  MORECORE_CONTIGUOUS as false.  As an example, here is a custom
  allocator kindly contributed for pre-OSX macOS.  It uses virtually
  but not necessarily physically contiguous non-paged memory (locked
  in, present and won't get swapped out).  You can use it by
  uncommenting this section, adding some #includes, and setting up the
  appropriate defines above:

      #define MORECORE osMoreCore
      #define MORECORE_CONTIGUOUS 0

  There is also a shutdown routine that should somehow be called for
  cleanup upon program exit.

  #define MAX_POOL_ENTRIES 100
  #define MINIMUM_MORECORE_SIZE  (64 * 1024)
  static int next_os_pool;
  void *our_os_pools[MAX_POOL_ENTRIES];

  void *osMoreCore(int size)
  {
    void *ptr = 0;
    static void *sbrk_top = 0;

    if (size > 0)
    {
      if (size < MINIMUM_MORECORE_SIZE)
         size = MINIMUM_MORECORE_SIZE;
      if (CurrentExecutionLevel() == kTaskLevel)
         ptr = PoolAllocateResident(size + RM_PAGE_SIZE, 0);
      if (ptr == 0)
      {
        return (void *) MORECORE_FAILURE;
      }
      // save ptrs so they can be freed during cleanup
      our_os_pools[next_os_pool] = ptr;
      next_os_pool++;
      ptr = (void *) ((((CHUNK_SIZE_T) ptr) + RM_PAGE_MASK) & ~RM_PAGE_MASK);
      sbrk_top = (char *) ptr + size;
      return ptr;
    }
    else if (size < 0)
    {
      // we don't currently support shrink behavior
      return (void *) MORECORE_FAILURE;
    }
    else
    {
      return sbrk_top;
    }
  }

  // cleanup any allocated memory pools
  // called as last thing before shutting down driver

  void osCleanupMem(void)
  {
    void **ptr;

    for (ptr = our_os_pools; ptr < &our_os_pools[MAX_POOL_ENTRIES]; ptr++)
      if (*ptr)
      {
         PoolDeallocate(*ptr);
         *ptr = 0;
      }
  }

*/


/* 
  -------------------------------------------------------------- 

  Emulation of sbrk for win32. 
  Donated by J. Walter <Walter@GeNeSys-e.de>.
  For additional information about this code, and malloc on Win32, see 
     http://www.genesys-e.de/jwalter/
*/


#ifdef WIN32

#ifdef _DEBUG
/* #define TRACE */
#endif

/* Support for USE_MALLOC_LOCK */
#ifdef USE_MALLOC_LOCK

/* Wait for spin lock */
static int slwait (int *sl) {
    while (InterlockedCompareExchange ((void **) sl, (void *) 1, (void *) 0) != 0) 
	    Sleep (0);
    return 0;
}

/* Release spin lock */
static int slrelease (int *sl) {
    InterlockedExchange (sl, 0);
    return 0;
}

#ifdef NEEDED
/* Spin lock for emulation code */
static int g_sl;
#endif

#endif /* USE_MALLOC_LOCK */

/* getpagesize for windows */
static long getpagesize (void) {
    static long g_pagesize = 0;
    if (! g_pagesize) {
        SYSTEM_INFO system_info;
        GetSystemInfo (&system_info);
        g_pagesize = system_info.dwPageSize;
    }
    return g_pagesize;
}
static long getregionsize (void) {
    static long g_regionsize = 0;
    if (! g_regionsize) {
        SYSTEM_INFO system_info;
        GetSystemInfo (&system_info);
        g_regionsize = system_info.dwAllocationGranularity;
    }
    return g_regionsize;
}

/* A region list entry */
typedef struct _region_list_entry {
    void *top_allocated;
    void *top_committed;
    void *top_reserved;
    long reserve_size;
    struct _region_list_entry *previous;
} region_list_entry;

/* Allocate and link a region entry in the region list */
static int region_list_append (region_list_entry **last, void *base_reserved, long reserve_size) {
    region_list_entry *next = HeapAlloc (GetProcessHeap (), 0, sizeof (region_list_entry));
    if (! next)
        return FALSE;
    next->top_allocated = (char *) base_reserved;
    next->top_committed = (char *) base_reserved;
    next->top_reserved = (char *) base_reserved + reserve_size;
    next->reserve_size = reserve_size;
    next->previous = *last;
    *last = next;
    return TRUE;
}
/* Free and unlink the last region entry from the region list */
static int region_list_remove (region_list_entry **last) {
    region_list_entry *previous = (*last)->previous;
    if (! HeapFree (GetProcessHeap (), sizeof (region_list_entry), *last))
        return FALSE;
    *last = previous;
    return TRUE;
}

#define CEIL(size,to)	(((size)+(to)-1)&~((to)-1))
#define FLOOR(size,to)	((size)&~((to)-1))

#define SBRK_SCALE  0
/* #define SBRK_SCALE  1 */
/* #define SBRK_SCALE  2 */
/* #define SBRK_SCALE  4  */

/* sbrk for windows */
static void *sbrk (long size) {
    static long g_pagesize, g_my_pagesize;
    static long g_regionsize, g_my_regionsize;
    static region_list_entry *g_last;
    void *result = (void *) MORECORE_FAILURE;
#ifdef TRACE
    printf ("sbrk %d\n", size);
#endif
#if defined (USE_MALLOC_LOCK) && defined (NEEDED)
    /* Wait for spin lock */
    slwait (&g_sl);
#endif
    /* First time initialization */
    if (! g_pagesize) {
        g_pagesize = getpagesize ();
        g_my_pagesize = g_pagesize << SBRK_SCALE;
    }
    if (! g_regionsize) {
        g_regionsize = getregionsize ();
        g_my_regionsize = g_regionsize << SBRK_SCALE;
    }
    if (! g_last) {
        if (! region_list_append (&g_last, 0, 0)) 
           goto sbrk_exit;
    }
    /* Assert invariants */
    assert (g_last);
    assert ((char *) g_last->top_reserved - g_last->reserve_size <= (char *) g_last->top_allocated &&
            g_last->top_allocated <= g_last->top_committed);
    assert ((char *) g_last->top_reserved - g_last->reserve_size <= (char *) g_last->top_committed &&
            g_last->top_committed <= g_last->top_reserved &&
            (unsigned) g_last->top_committed % g_pagesize == 0);
    assert ((unsigned) g_last->top_reserved % g_regionsize == 0);
    assert ((unsigned) g_last->reserve_size % g_regionsize == 0);
    /* Allocation requested? */
    if (size >= 0) {
        /* Allocation size is the requested size */
        long allocate_size = size;
        /* Compute the size to commit */
        long to_commit = (char *) g_last->top_allocated + allocate_size - (char *) g_last->top_committed;
        /* Do we reach the commit limit? */
        if (to_commit > 0) {
            /* Round size to commit */
            long commit_size = CEIL (to_commit, g_my_pagesize);
            /* Compute the size to reserve */
            long to_reserve = (char *) g_last->top_committed + commit_size - (char *) g_last->top_reserved;
            /* Do we reach the reserve limit? */
            if (to_reserve > 0) {
                /* Compute the remaining size to commit in the current region */
                long remaining_commit_size = (char *) g_last->top_reserved - (char *) g_last->top_committed;
                if (remaining_commit_size > 0) {
                    /* Assert preconditions */
                    assert ((unsigned) g_last->top_committed % g_pagesize == 0);
                    assert (0 < remaining_commit_size && remaining_commit_size % g_pagesize == 0); {
                        /* Commit this */
                        void *base_committed = VirtualAlloc (g_last->top_committed, remaining_commit_size,
							                                 MEM_COMMIT, PAGE_READWRITE);
                        /* Check returned pointer for consistency */
                        if (base_committed != g_last->top_committed)
                            goto sbrk_exit;
                        /* Assert postconditions */
                        assert ((unsigned) base_committed % g_pagesize == 0);
#ifdef TRACE
                        printf ("Commit %p %d\n", base_committed, remaining_commit_size);
#endif
                        /* Adjust the regions commit top */
                        g_last->top_committed = (char *) base_committed + remaining_commit_size;
                    }
                } {
                    /* Now we are going to search and reserve. */
                    int contiguous = -1;
                    int found = FALSE;
                    MEMORY_BASIC_INFORMATION memory_info;
                    void *base_reserved;
                    long reserve_size;
                    do {
                        /* Assume contiguous memory */
                        contiguous = TRUE;
                        /* Round size to reserve */
                        reserve_size = CEIL (to_reserve, g_my_regionsize);
                        /* Start with the current region's top */
                        memory_info.BaseAddress = g_last->top_reserved;
                        /* Assert preconditions */
                        assert ((unsigned) memory_info.BaseAddress % g_pagesize == 0);
                        assert (0 < reserve_size && reserve_size % g_regionsize == 0);
                        while (VirtualQuery (memory_info.BaseAddress, &memory_info, sizeof (memory_info))) {
                            /* Assert postconditions */
                            assert ((unsigned) memory_info.BaseAddress % g_pagesize == 0);
#ifdef TRACE
                            printf ("Query %p %d %s\n", memory_info.BaseAddress, memory_info.RegionSize, 
                                    memory_info.State == MEM_FREE ? "FREE": 
                                    (memory_info.State == MEM_RESERVE ? "RESERVED":
                                     (memory_info.State == MEM_COMMIT ? "COMMITTED": "?")));
#endif
                            /* Region is free, well aligned and big enough: we are done */
                            if (memory_info.State == MEM_FREE &&
                                (unsigned) memory_info.BaseAddress % g_regionsize == 0 &&
                                memory_info.RegionSize >= (unsigned) reserve_size) {
                                found = TRUE;
                                break;
                            }
                            /* From now on we can't get contiguous memory! */
                            contiguous = FALSE;
                            /* Recompute size to reserve */
                            reserve_size = CEIL (allocate_size, g_my_regionsize);
                            memory_info.BaseAddress = (char *) memory_info.BaseAddress + memory_info.RegionSize;
                            /* Assert preconditions */
                            assert ((unsigned) memory_info.BaseAddress % g_pagesize == 0);
                            assert (0 < reserve_size && reserve_size % g_regionsize == 0);
                        }
                        /* Search failed? */
                        if (! found) 
                            goto sbrk_exit;
                        /* Assert preconditions */
                        assert ((unsigned) memory_info.BaseAddress % g_regionsize == 0);
                        assert (0 < reserve_size && reserve_size % g_regionsize == 0);
                        /* Try to reserve this */
                        base_reserved = VirtualAlloc (memory_info.BaseAddress, reserve_size, 
					                                  MEM_RESERVE, PAGE_NOACCESS);
                        if (! base_reserved) {
                            int rc = GetLastError ();
                            if (rc != ERROR_INVALID_ADDRESS) 
                                goto sbrk_exit;
                        }
                        /* A null pointer signals (hopefully) a race condition with another thread. */
                        /* In this case, we try again. */
                    } while (! base_reserved);
                    /* Check returned pointer for consistency */
                    if (memory_info.BaseAddress && base_reserved != memory_info.BaseAddress)
                        goto sbrk_exit;
                    /* Assert postconditions */
                    assert ((unsigned) base_reserved % g_regionsize == 0);
#ifdef TRACE
                    printf ("Reserve %p %d\n", base_reserved, reserve_size);
#endif
                    /* Did we get contiguous memory? */
                    if (contiguous) {
                        long start_size = (char *) g_last->top_committed - (char *) g_last->top_allocated;
                        /* Adjust allocation size */
                        allocate_size -= start_size;
                        /* Adjust the regions allocation top */
                        g_last->top_allocated = g_last->top_committed;
                        /* Recompute the size to commit */
                        to_commit = (char *) g_last->top_allocated + allocate_size - (char *) g_last->top_committed;
                        /* Round size to commit */
                        commit_size = CEIL (to_commit, g_my_pagesize);
                    } 
                    /* Append the new region to the list */
                    if (! region_list_append (&g_last, base_reserved, reserve_size))
                        goto sbrk_exit;
                    /* Didn't we get contiguous memory? */
                    if (! contiguous) {
                        /* Recompute the size to commit */
                        to_commit = (char *) g_last->top_allocated + allocate_size - (char *) g_last->top_committed;
                        /* Round size to commit */
                        commit_size = CEIL (to_commit, g_my_pagesize);
                    }
                }
            } 
            /* Assert preconditions */
            assert ((unsigned) g_last->top_committed % g_pagesize == 0);
            assert (0 < commit_size && commit_size % g_pagesize == 0); {
                /* Commit this */
                void *base_committed = VirtualAlloc (g_last->top_committed, commit_size, 
				    			                     MEM_COMMIT, PAGE_READWRITE);
                /* Check returned pointer for consistency */
                if (base_committed != g_last->top_committed)
                    goto sbrk_exit;
                /* Assert postconditions */
                assert ((unsigned) base_committed % g_pagesize == 0);
#ifdef TRACE
                printf ("Commit %p %d\n", base_committed, commit_size);
#endif
                /* Adjust the regions commit top */
                g_last->top_committed = (char *) base_committed + commit_size;
            }
        } 
        /* Adjust the regions allocation top */
        g_last->top_allocated = (char *) g_last->top_allocated + allocate_size;
        result = (char *) g_last->top_allocated - size;
    /* Deallocation requested? */
    } else if (size < 0) {
        long deallocate_size = - size;
        /* As long as we have a region to release */
        while ((char *) g_last->top_allocated - deallocate_size < (char *) g_last->top_reserved - g_last->reserve_size) {
            /* Get the size to release */
            long release_size = g_last->reserve_size;
            /* Get the base address */
            void *base_reserved = (char *) g_last->top_reserved - release_size;
            /* Assert preconditions */
            assert ((unsigned) base_reserved % g_regionsize == 0); 
            assert (0 < release_size && release_size % g_regionsize == 0); {
                /* Release this */
                int rc = VirtualFree (base_reserved, 0, 
                                      MEM_RELEASE);
                /* Check returned code for consistency */
                if (! rc)
                    goto sbrk_exit;
#ifdef TRACE
                printf ("Release %p %d\n", base_reserved, release_size);
#endif
            }
            /* Adjust deallocation size */
            deallocate_size -= (char *) g_last->top_allocated - (char *) base_reserved;
            /* Remove the old region from the list */
            if (! region_list_remove (&g_last))
                goto sbrk_exit;
        } {
            /* Compute the size to decommit */
            long to_decommit = (char *) g_last->top_committed - ((char *) g_last->top_allocated - deallocate_size);
            if (to_decommit >= g_my_pagesize) {
                /* Compute the size to decommit */
                long decommit_size = FLOOR (to_decommit, g_my_pagesize);
                /*  Compute the base address */
                void *base_committed = (char *) g_last->top_committed - decommit_size;
                /* Assert preconditions */
                assert ((unsigned) base_committed % g_pagesize == 0);
                assert (0 < decommit_size && decommit_size % g_pagesize == 0); {
                    /* Decommit this */
                    int rc = VirtualFree ((char *) base_committed, decommit_size, 
                                          MEM_DECOMMIT);
                    /* Check returned code for consistency */
                    if (! rc)
                        goto sbrk_exit;
#ifdef TRACE
                    printf ("Decommit %p %d\n", base_committed, decommit_size);
#endif
                }
                /* Adjust deallocation size and regions commit and allocate top */
                deallocate_size -= (char *) g_last->top_allocated - (char *) base_committed;
                g_last->top_committed = base_committed;
                g_last->top_allocated = base_committed;
            }
        }
        /* Adjust regions allocate top */
        g_last->top_allocated = (char *) g_last->top_allocated - deallocate_size;
        /* Check for underflow */
        if ((char *) g_last->top_reserved - g_last->reserve_size > (char *) g_last->top_allocated ||
            g_last->top_allocated > g_last->top_committed) {
            /* Adjust regions allocate top */
            g_last->top_allocated = (char *) g_last->top_reserved - g_last->reserve_size;
            goto sbrk_exit;
        }
        result = g_last->top_allocated;
    }
    /* Assert invariants */
    assert (g_last);
    assert ((char *) g_last->top_reserved - g_last->reserve_size <= (char *) g_last->top_allocated &&
            g_last->top_allocated <= g_last->top_committed);
    assert ((char *) g_last->top_reserved - g_last->reserve_size <= (char *) g_last->top_committed &&
            g_last->top_committed <= g_last->top_reserved &&
            (unsigned) g_last->top_committed % g_pagesize == 0);
    assert ((unsigned) g_last->top_reserved % g_regionsize == 0);
    assert ((unsigned) g_last->reserve_size % g_regionsize == 0);

sbrk_exit:
#if defined (USE_MALLOC_LOCK) && defined (NEEDED)
    /* Release spin lock */
    slrelease (&g_sl);
#endif
    return result;
}

/* mmap for windows */
static void *mmap (void *ptr, long size, long prot, long type, long handle, long arg) {
    static long g_pagesize;
    static long g_regionsize;
#ifdef TRACE
    printf ("mmap %d\n", size);
#endif
#if defined (USE_MALLOC_LOCK) && defined (NEEDED)
    /* Wait for spin lock */
    slwait (&g_sl);
#endif
    /* First time initialization */
    if (! g_pagesize) 
        g_pagesize = getpagesize ();
    if (! g_regionsize) 
        g_regionsize = getregionsize ();
    /* Assert preconditions */
    assert ((unsigned) ptr % g_regionsize == 0);
    assert (size % g_pagesize == 0);
    /* Allocate this */
    ptr = VirtualAlloc (ptr, size,
					    MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
    if (! ptr) {
        ptr = (void *) MORECORE_FAILURE;
        goto mmap_exit;
    }
    /* Assert postconditions */
    assert ((unsigned) ptr % g_regionsize == 0);
#ifdef TRACE
    printf ("Commit %p %d\n", ptr, size);
#endif
mmap_exit:
#if defined (USE_MALLOC_LOCK) && defined (NEEDED)
    /* Release spin lock */
    slrelease (&g_sl);
#endif
    return ptr;
}

/* munmap for windows */
static long munmap (void *ptr, long size) {
    static long g_pagesize;
    static long g_regionsize;
    int rc = MUNMAP_FAILURE;
#ifdef TRACE
    printf ("munmap %p %d\n", ptr, size);
#endif
#if defined (USE_MALLOC_LOCK) && defined (NEEDED)
    /* Wait for spin lock */
    slwait (&g_sl);
#endif
    /* First time initialization */
    if (! g_pagesize) 
        g_pagesize = getpagesize ();
    if (! g_regionsize) 
        g_regionsize = getregionsize ();
    /* Assert preconditions */
    assert ((unsigned) ptr % g_regionsize == 0);
    assert (size % g_pagesize == 0);
    /* Free this */
    if (! VirtualFree (ptr, 0, 
                       MEM_RELEASE))
        goto munmap_exit;
    rc = 0;
#ifdef TRACE
    printf ("Release %p %d\n", ptr, size);
#endif
munmap_exit:
#if defined (USE_MALLOC_LOCK) && defined (NEEDED)
    /* Release spin lock */
    slrelease (&g_sl);
#endif
    return rc;
}

static void vminfo (CHUNK_SIZE_T  *free, CHUNK_SIZE_T  *reserved, CHUNK_SIZE_T  *committed) {
    MEMORY_BASIC_INFORMATION memory_info;
    memory_info.BaseAddress = 0;
    *free = *reserved = *committed = 0;
    while (VirtualQuery (memory_info.BaseAddress, &memory_info, sizeof (memory_info))) {
        switch (memory_info.State) {
        case MEM_FREE:
            *free += memory_info.RegionSize;
            break;
        case MEM_RESERVE:
            *reserved += memory_info.RegionSize;
            break;
        case MEM_COMMIT:
            *committed += memory_info.RegionSize;
            break;
        }
        memory_info.BaseAddress = (char *) memory_info.BaseAddress + memory_info.RegionSize;
    }
}

static int cpuinfo (int whole, CHUNK_SIZE_T  *kernel, CHUNK_SIZE_T  *user) {
    if (whole) {
        __int64 creation64, exit64, kernel64, user64;
        int rc = GetProcessTimes (GetCurrentProcess (), 
                                  (FILETIME *) &creation64,  
                                  (FILETIME *) &exit64, 
                                  (FILETIME *) &kernel64, 
                                  (FILETIME *) &user64);
        if (! rc) {
            *kernel = 0;
            *user = 0;
            return FALSE;
        } 
        *kernel = (CHUNK_SIZE_T) (kernel64 / 10000);
        *user = (CHUNK_SIZE_T) (user64 / 10000);
        return TRUE;
    } else {
        __int64 creation64, exit64, kernel64, user64;
        int rc = GetThreadTimes (GetCurrentThread (), 
                                 (FILETIME *) &creation64,  
                                 (FILETIME *) &exit64, 
                                 (FILETIME *) &kernel64, 
                                 (FILETIME *) &user64);
        if (! rc) {
            *kernel = 0;
            *user = 0;
            return FALSE;
        } 
        *kernel = (CHUNK_SIZE_T) (kernel64 / 10000);
        *user = (CHUNK_SIZE_T) (user64 / 10000);
        return TRUE;
    }
}

#endif /* WIN32 */

/* ------------------------------------------------------------
History:
    V2.8.0 (not yet released)
      * Use trees for non-small bins
         Also requiring different size->bin algorithm

    V2.7.1 Thu Jul 25 10:58:03 2002  Doug Lea  (dl at gee)
      * Allow tuning of FIRST_SORTED_BIN_SIZE
      * Use PTR_UINT as type for all ptr->int casts. Thanks to John Belmonte.
      * Better detection and support for non-contiguousness of MORECORE. 
        Thanks to Andreas Mueller, Conal Walsh, and Wolfram Gloger
      * Bypass most of malloc if no frees. Thanks To Emery Berger.
      * Fix freeing of old top non-contiguous chunk im sysmalloc.
      * Raised default trim and map thresholds to 256K.
      * Fix mmap-related #defines. Thanks to Lubos Lunak.
      * Fix copy macros; added LACKS_FCNTL_H. Thanks to Neal Walfield.
      * Branch-free bin calculation
      * Default trim and mmap thresholds now 256K.

    V2.7.0 Sun Mar 11 14:14:06 2001  Doug Lea  (dl at gee)
      * Introduce independent_comalloc and independent_calloc.
        Thanks to Michael Pachos for motivation and help.
      * Make optional .h file available
      * Allow > 2GB requests on 32bit systems.
      * new WIN32 sbrk, mmap, munmap, lock code from <Walter@GeNeSys-e.de>.
        Thanks also to Andreas Mueller <a.mueller at paradatec.de>,
        and Anonymous.
      * Allow override of MALLOC_ALIGNMENT (Thanks to Ruud Waij for 
        helping test this.)
      * memalign: check alignment arg
      * realloc: don't try to shift chunks backwards, since this
        leads to  more fragmentation in some programs and doesn't
        seem to help in any others.
      * Collect all cases in malloc requiring system memory into sysmalloc
      * Use mmap as backup to sbrk
      * Place all internal state in malloc_state
      * Introduce fastbins (although similar to 2.5.1)
      * Many minor tunings and cosmetic improvements
      * Introduce USE_PUBLIC_MALLOC_WRAPPERS, USE_MALLOC_LOCK 
      * Introduce MALLOC_FAILURE_ACTION, MORECORE_CONTIGUOUS
        Thanks to Tony E. Bennett <tbennett@nvidia.com> and others.
      * Include errno.h to support default failure action.

    V2.6.6 Sun Dec  5 07:42:19 1999  Doug Lea  (dl at gee)
      * return null for negative arguments
      * Added Several WIN32 cleanups from Martin C. Fong <mcfong at yahoo.com>
         * Add 'LACKS_SYS_PARAM_H' for those systems without 'sys/param.h'
          (e.g. WIN32 platforms)
         * Cleanup header file inclusion for WIN32 platforms
         * Cleanup code to avoid Microsoft Visual C++ compiler complaints
         * Add 'USE_DL_PREFIX' to quickly allow co-existence with existing
           memory allocation routines
         * Set 'malloc_getpagesize' for WIN32 platforms (needs more work)
         * Use 'assert' rather than 'ASSERT' in WIN32 code to conform to
           usage of 'assert' in non-WIN32 code
         * Improve WIN32 'sbrk()' emulation's 'findRegion()' routine to
           avoid infinite loop
      * Always call 'fREe()' rather than 'free()'

    V2.6.5 Wed Jun 17 15:57:31 1998  Doug Lea  (dl at gee)
      * Fixed ordering problem with boundary-stamping

    V2.6.3 Sun May 19 08:17:58 1996  Doug Lea  (dl at gee)
      * Added pvalloc, as recommended by H.J. Liu
      * Added 64bit pointer support mainly from Wolfram Gloger
      * Added anonymously donated WIN32 sbrk emulation
      * Malloc, calloc, getpagesize: add optimizations from Raymond Nijssen
      * malloc_extend_top: fix mask error that caused wastage after
        foreign sbrks
      * Add linux mremap support code from HJ Liu

    V2.6.2 Tue Dec  5 06:52:55 1995  Doug Lea  (dl at gee)
      * Integrated most documentation with the code.
      * Add support for mmap, with help from
        Wolfram Gloger (Gloger@lrz.uni-muenchen.de).
      * Use last_remainder in more cases.
      * Pack bins using idea from  colin@nyx10.cs.du.edu
      * Use ordered bins instead of best-fit threshhold
      * Eliminate block-local decls to simplify tracing and debugging.
      * Support another case of realloc via move into top
      * Fix error occuring when initial sbrk_base not word-aligned.
      * Rely on page size for units instead of SBRK_UNIT to
        avoid surprises about sbrk alignment conventions.
      * Add mallinfo, mallopt. Thanks to Raymond Nijssen
        (raymond@es.ele.tue.nl) for the suggestion.
      * Add `pad' argument to malloc_trim and top_pad mallopt parameter.
      * More precautions for cases where other routines call sbrk,
        courtesy of Wolfram Gloger (Gloger@lrz.uni-muenchen.de).
      * Added macros etc., allowing use in linux libc from
        H.J. Lu (hjl@gnu.ai.mit.edu)
      * Inverted this history list

    V2.6.1 Sat Dec  2 14:10:57 1995  Doug Lea  (dl at gee)
      * Re-tuned and fixed to behave more nicely with V2.6.0 changes.
      * Removed all preallocation code since under current scheme
        the work required to undo bad preallocations exceeds
        the work saved in good cases for most test programs.
      * No longer use return list or unconsolidated bins since
        no scheme using them consistently outperforms those that don't
        given above changes.
      * Use best fit for very large chunks to prevent some worst-cases.
      * Added some support for debugging

    V2.6.0 Sat Nov  4 07:05:23 1995  Doug Lea  (dl at gee)
      * Removed footers when chunks are in use. Thanks to
        Paul Wilson (wilson@cs.texas.edu) for the suggestion.

    V2.5.4 Wed Nov  1 07:54:51 1995  Doug Lea  (dl at gee)
      * Added malloc_trim, with help from Wolfram Gloger
        (wmglo@Dent.MED.Uni-Muenchen.DE).

    V2.5.3 Tue Apr 26 10:16:01 1994  Doug Lea  (dl at g)

    V2.5.2 Tue Apr  5 16:20:40 1994  Doug Lea  (dl at g)
      * realloc: try to expand in both directions
      * malloc: swap order of clean-bin strategy;
      * realloc: only conditionally expand backwards
      * Try not to scavenge used bins
      * Use bin counts as a guide to preallocation
      * Occasionally bin return list chunks in first scan
      * Add a few optimizations from colin@nyx10.cs.du.edu

    V2.5.1 Sat Aug 14 15:40:43 1993  Doug Lea  (dl at g)
      * faster bin computation & slightly different binning
      * merged all consolidations to one part of malloc proper
         (eliminating old malloc_find_space & malloc_clean_bin)
      * Scan 2 returns chunks (not just 1)
      * Propagate failure in realloc if malloc returns 0
      * Add stuff to allow compilation on non-ANSI compilers
          from kpv@research.att.com

    V2.5 Sat Aug  7 07:41:59 1993  Doug Lea  (dl at g.oswego.edu)
      * removed potential for odd address access in prev_chunk
      * removed dependency on getpagesize.h
      * misc cosmetics and a bit more internal documentation
      * anticosmetics: mangled names in macros to evade debugger strangeness
      * tested on sparc, hp-700, dec-mips, rs6000
          with gcc & native cc (hp, dec only) allowing
          Detlefs & Zorn comparison study (in SIGPLAN Notices.)

    Trial version Fri Aug 28 13:14:29 1992  Doug Lea  (dl at g.oswego.edu)
      * Based loosely on libg++-1.2X malloc. (It retains some of the overall
         structure of old version,  but most details differ.)

*/


