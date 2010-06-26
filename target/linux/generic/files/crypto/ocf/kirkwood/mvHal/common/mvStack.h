/******************************************************************************* 
*                   Copyright 2003, Marvell Semiconductor Israel LTD.          * 
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      * 
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  * 
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        * 
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     * 
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       * 
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   * 
*                                                                              * 
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, * 
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    * 
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  * 
* (MJKK), MARVELL SEMICONDUCTOR ISRAEL LTD (MSIL).                             * 
********************************************************************************
* mvStack.h - Header File for :
*
* FILENAME:    $Workfile: mvStack.h $
* REVISION:    $Revision: 1.1 $
* LAST UPDATE: $Modtime:  $
*
* DESCRIPTION:
*     This file defines simple Stack (LIFO) functionality.
*
*******************************************************************************/

#ifndef __mvStack_h__
#define __mvStack_h__


/* includes */
#include "mvTypes.h"


/* defines  */


/* typedefs */
/* Data structure describes general purpose Stack */
typedef struct 
{
    int     stackIdx;
    int     numOfElements;
    MV_U32* stackElements;
} MV_STACK;

static INLINE MV_BOOL mvStackIsFull(void* stackHndl)
{	
    MV_STACK*   pStack = (MV_STACK*)stackHndl;

    if(pStack->stackIdx == pStack->numOfElements)
        return MV_TRUE;

    return MV_FALSE;
}

static INLINE MV_BOOL mvStackIsEmpty(void* stackHndl)
{	
    MV_STACK*   pStack = (MV_STACK*)stackHndl;

    if(pStack->stackIdx == 0)
        return MV_TRUE;

    return MV_FALSE;
}
/* Purpose: Push new element to stack
 * Inputs:
 *	- void* 	stackHndl 	- Stack handle as returned by "mvStackCreate()" function.
 *	- MV_U32	value		- New element.
 *
 * Return: MV_STATUS  	MV_FULL - Failure. Stack is full.
 *						MV_OK   - Success. Element is put to stack.
 */
static INLINE void mvStackPush(void* stackHndl, MV_U32 value)
{	
    MV_STACK*   pStack = (MV_STACK*)stackHndl;

#ifdef MV_RT_DEBUG
    if(pStack->stackIdx == pStack->numOfElements)
    {
        mvOsPrintf("mvStackPush: Stack is FULL\n");
        return;
    }
#endif /* MV_RT_DEBUG */

    pStack->stackElements[pStack->stackIdx] = value;
    pStack->stackIdx++;
}

/* Purpose: Pop element from the top of stack and copy it to "pValue"
 * Inputs:
 *	- void* 	stackHndl 	- Stack handle as returned by "mvStackCreate()" function.
 *	- MV_U32	value		- Element in the top of stack.
 *
 * Return: MV_STATUS  	MV_EMPTY - Failure. Stack is empty.
 *						MV_OK    - Success. Element is removed from the stack and
 *									copied to pValue argument
 */
static INLINE MV_U32   mvStackPop(void* stackHndl)
{
    MV_STACK*   pStack = (MV_STACK*)stackHndl;

#ifdef MV_RT_DEBUG
    if(pStack->stackIdx == 0)
    {
        mvOsPrintf("mvStackPop: Stack is EMPTY\n");
        return 0;
    }
#endif /* MV_RT_DEBUG */

    pStack->stackIdx--;
    return pStack->stackElements[pStack->stackIdx];
}

static INLINE int       mvStackIndex(void* stackHndl)
{
    MV_STACK*   pStack = (MV_STACK*)stackHndl;

    return pStack->stackIdx;
}

static INLINE int       mvStackFreeElements(void* stackHndl)
{
    MV_STACK*   pStack = (MV_STACK*)stackHndl;

    return (pStack->numOfElements - pStack->stackIdx);
}

/* mvStack.h API list */

/* Create new Stack */
void*       mvStackCreate(int numOfElements);

/* Delete existing stack */
MV_STATUS   mvStackDelete(void* stackHndl);

/* Print status of the stack */
void        mvStackStatus(void* stackHndl, MV_BOOL isPrintElements);

#endif /* __mvStack_h__ */

