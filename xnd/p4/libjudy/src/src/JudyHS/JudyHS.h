// ****************************************************************************
// Quick and dirty header file for use with old Judy.h without JudyHS defs
// May 2004 (dlb) - No copyright or license -- it is free period.

#include <stdint.h>

// ****************************************************************************
// JUDYHSL MACROS:

#define JHSI(PV,    PArray,   PIndex,   Count)                          \
        J_2P(PV, (&(PArray)), PIndex,   Count, JudyHSIns, "JudyHSIns")
#define JHSG(PV,    PArray,   PIndex,   Count)                          \
        (PV) = (Pvoid_t) JudyHSGet(PArray, PIndex, Count)
#define JHSD(Rc,    PArray,   PIndex,   Count)                          \
        J_2I(Rc, (&(PArray)), PIndex, Count, JudyHSDel, "JudyHSDel")
#define JHSFA(Rc,    PArray)                                            \
        J_0I(Rc, (&(PArray)), JudyHSFreeArray, "JudyHSFreeArray")

// ****************************************************************************
// JUDY memory interface to malloc() FUNCTIONS:

extern Word_t JudyMalloc(Word_t);               // words reqd => words allocd.
extern Word_t JudyMallocVirtual(Word_t);        // words reqd => words allocd.
extern void   JudyFree(Pvoid_t, Word_t);        // block to free and its size in words.
extern void   JudyFreeVirtual(Pvoid_t, Word_t); // block to free and its size in words.

// ****************************************************************************
// JUDYHS FUNCTIONS:

extern PPvoid_t JudyHSGet(       Pcvoid_t,  void *, Word_t);
extern PPvoid_t JudyHSIns(       PPvoid_t,  void *, Word_t, P_JE);
extern int      JudyHSDel(       PPvoid_t,  void *, Word_t, P_JE);
extern Word_t   JudyHSFreeArray( PPvoid_t,                  P_JE);

extern uint32_t JudyHashStr(                void *, Word_t);
