/******************************************************************************/
/*                                                                            */
/* Broadcom BCM5700 Linux Network Driver, Copyright (c) 2000 - 2004 Broadcom  */
/* Corporation.                                                               */
/* All rights reserved.                                                       */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation, located in the file LICENSE.                 */
/*                                                                            */
/* Queue functions.                                                           */
/*    void          QQ_InitQueue(PQQ_CONTAINER pQueue)                        */
/*    char          QQ_Full(PQQ_CONTAINER pQueue)                             */
/*    char          QQ_Empty(PQQ_CONTAINER pQueue)                            */
/*    unsigned int QQ_GetSize(PQQ_CONTAINER pQueue)                          */
/*    unsigned int QQ_GetEntryCnt(PQQ_CONTAINER pQueue)                      */
/*    char          QQ_PushHead(PQQ_CONTAINER pQueue, PQQ_ENTRY pEntry)       */
/*    char          QQ_PushTail(PQQ_CONTAINER pQueue, PQQ_ENTRY pEntry)       */
/*    PQQ_ENTRY     QQ_PopHead(PQQ_CONTAINER pQueue)                          */
/*    PQQ_ENTRY     QQ_PopTail(PQQ_CONTAINER pQueue)                          */
/*    PQQ_ENTRY     QQ_GetHead(PQQ_CONTAINER pQueue, unsigned int Idx)       */
/*    PQQ_ENTRY     QQ_GetTail(PQQ_CONTAINER pQueue, unsigned int Idx)       */
/*                                                                            */
/*                                                                            */
/* History:                                                                   */
/*    02/25/00 Hav Khauv        Initial version.                              */
/******************************************************************************/

#ifndef BCM_QUEUE_H
#define BCM_QUEUE_H



/******************************************************************************/
/* Queue definitions. */
/******************************************************************************/

/* Entry for queueing. */
typedef void *PQQ_ENTRY;


/* Queue header -- base type. */
typedef struct {
    unsigned int Head;
    unsigned int Tail;
    unsigned int Size;
    MM_ATOMIC_T EntryCnt;
    PQQ_ENTRY Array[1];
} QQ_CONTAINER, *PQQ_CONTAINER;


/* Declare queue type macro. */
#define DECLARE_QUEUE_TYPE(_QUEUE_TYPE, _QUEUE_SIZE)            \
                                                                \
    typedef struct {                                            \
        QQ_CONTAINER Container;                                 \
        PQQ_ENTRY EntryBuffer[_QUEUE_SIZE];                     \
    } _QUEUE_TYPE, *P##_QUEUE_TYPE



/******************************************************************************/
/* Compilation switches. */
/******************************************************************************/

#if DBG
#undef QQ_NO_OVERFLOW_CHECK
#undef QQ_NO_UNDERFLOW_CHECK
#endif /* DBG */

#ifdef QQ_USE_MACROS
/* notdone */
#else

#ifdef QQ_NO_INLINE
#define __inline
#endif /* QQ_NO_INLINE */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static void 
QQ_InitQueue(
PQQ_CONTAINER pQueue,
unsigned int QueueSize) {
    pQueue->Head = 0;
    pQueue->Tail = 0;
    pQueue->Size = QueueSize+1;
    MM_ATOMIC_SET(&pQueue->EntryCnt, 0);
} /* QQ_InitQueue */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static char 
QQ_Full(
PQQ_CONTAINER pQueue) {
    unsigned int NewHead;

    NewHead = (pQueue->Head + 1) % pQueue->Size;

    return(NewHead == pQueue->Tail);
} /* QQ_Full */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static char 
QQ_Empty(
PQQ_CONTAINER pQueue) {
    return(pQueue->Head == pQueue->Tail);
} /* QQ_Empty */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static unsigned int 
QQ_GetSize(
PQQ_CONTAINER pQueue) {
    return pQueue->Size;
} /* QQ_GetSize */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static unsigned int 
QQ_GetEntryCnt(
PQQ_CONTAINER pQueue) {
    return MM_ATOMIC_READ(&pQueue->EntryCnt);
} /* QQ_GetEntryCnt */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/*    TRUE entry was added successfully.                                      */
/*    FALSE queue is full.                                                    */
/******************************************************************************/
__inline static char 
QQ_PushHead(
PQQ_CONTAINER pQueue, 
PQQ_ENTRY pEntry) {
    unsigned int Head;

    Head = (pQueue->Head + 1) % pQueue->Size;

#if !defined(QQ_NO_OVERFLOW_CHECK)
    if(Head == pQueue->Tail) {
        return 0;
    } /* if */
#endif /* QQ_NO_OVERFLOW_CHECK */

    pQueue->Array[pQueue->Head] = pEntry;
    MM_WMB();
    pQueue->Head = Head;
    MM_ATOMIC_INC(&pQueue->EntryCnt);

    return -1;
} /* QQ_PushHead */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/*    TRUE entry was added successfully.                                      */
/*    FALSE queue is full.                                                    */
/******************************************************************************/
__inline static char 
QQ_PushTail(
PQQ_CONTAINER pQueue,
PQQ_ENTRY pEntry) {
    unsigned int Tail;

    Tail = pQueue->Tail;
    if(Tail == 0) {
        Tail = pQueue->Size;
    } /* if */
    Tail--;

#if !defined(QQ_NO_OVERFLOW_CHECK)
    if(Tail == pQueue->Head) {
        return 0;
    } /* if */
#endif /* QQ_NO_OVERFLOW_CHECK */

    pQueue->Array[Tail] = pEntry;
    MM_WMB();
    pQueue->Tail = Tail;
    MM_ATOMIC_INC(&pQueue->EntryCnt);

    return -1;
} /* QQ_PushTail */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static PQQ_ENTRY
QQ_PopHead(
PQQ_CONTAINER pQueue) {
    unsigned int Head;
    unsigned int Tail;
    PQQ_ENTRY Entry;

    Head = pQueue->Head;
    Tail = pQueue->Tail;

    MM_MB();
#if !defined(QQ_NO_UNDERFLOW_CHECK)
    if(Head == Tail) {
        return (PQQ_ENTRY) 0;
    } /* if */
#endif /* QQ_NO_UNDERFLOW_CHECK */

    if(Head == 0) {
        Head = pQueue->Size;
    } /* if */
    Head--;

    Entry = pQueue->Array[Head];
    MM_MB();
    pQueue->Head = Head;
    MM_ATOMIC_DEC(&pQueue->EntryCnt);

    return Entry;
} /* QQ_PopHead */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static PQQ_ENTRY
QQ_PopTail(
PQQ_CONTAINER pQueue) {
    unsigned int Head;
    unsigned int Tail;
    PQQ_ENTRY Entry;

    Head = pQueue->Head;
    Tail = pQueue->Tail;

    MM_MB();
#if !defined(QQ_NO_UNDERFLOW_CHECK)
    if(Tail == Head) {
        return (PQQ_ENTRY) 0;
    } /* if */
#endif /* QQ_NO_UNDERFLOW_CHECK */

    Entry = pQueue->Array[Tail];
    MM_MB();
    pQueue->Tail = (Tail + 1) % pQueue->Size;
    MM_ATOMIC_DEC(&pQueue->EntryCnt);

    return Entry;
} /* QQ_PopTail */



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static PQQ_ENTRY
QQ_GetHead(
    PQQ_CONTAINER pQueue,
    unsigned int Idx)
{
    if(Idx >= (unsigned int) MM_ATOMIC_READ(&pQueue->EntryCnt))
    {
        return (PQQ_ENTRY) 0;
    }

    if(pQueue->Head > Idx)
    {
        Idx = pQueue->Head - Idx;
    }
    else
    {
        Idx = pQueue->Size - (Idx - pQueue->Head);
    }
    Idx--;

    return pQueue->Array[Idx];
}



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
__inline static PQQ_ENTRY
QQ_GetTail(
    PQQ_CONTAINER pQueue,
    unsigned int Idx)
{
    if(Idx >= (unsigned int) MM_ATOMIC_READ(&pQueue->EntryCnt))
    {
        return (PQQ_ENTRY) 0;
    }

    Idx += pQueue->Tail;
    if(Idx >= pQueue->Size)
    {
        Idx = Idx - pQueue->Size;
    }

    return pQueue->Array[Idx];
}

#endif /* QQ_USE_MACROS */



#endif /* QUEUE_H */
