#ifndef _JUDY1OP_INCLUDED
#define	_JUDY1OP_INCLUDED

// @(#) $Revision: 4.1 $ $Source: /judy/src/funcs/Judy1Op.h $
//
// HEADER FILE FOR EXPORTED FEATURES FROM Judy1Op().

#define	JUDY1OP_AND    1L
#define	JUDY1OP_OR     2L
#define	JUDY1OP_ANDNOT 3L

extern int Judy1Op(PPvoid_t PPDest, Pvoid_t PSet1, Pvoid_t PSet2,
                   Word_t Operation, JError_t * PJError);

#endif // ! _JUDY1OP_INCLUDED
