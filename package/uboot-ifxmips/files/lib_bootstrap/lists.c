#include <common.h>
#include <malloc.h>
#include <lists.h>

#define MAX(a,b) 	(((a)>(b)) ? (a) : (b))
#define MIN(a,b) 	(((a)<(b)) ? (a) : (b))
#define CAT4CHARS(a,b,c,d)	((a<<24) | (b<<16) | (c<<8) | d)

/* increase list size by 10% every time it is full */
#define kDefaultAllocationPercentIncrease	10

/* always increase list size by 4 items when it is full */
#define kDefaultAllocationminNumItemsIncrease	4

/*
 * how many items to expand the list by when it becomes full
 * = current listSize (in items) + (hiword percent of list size) + loword
 */
#define NUMITEMSPERALLOC(list)	MAX(((*list)->listSize * \
				    ((*list)->percentIncrease + 100)) / 100, \
				    (*list)->minNumItemsIncrease )

#define ITEMPTR(list,item)	&(((char *)&(*list)->itemList)[(*(list))->itemSize * (item)])

#define LIST_SIGNATURE		CAT4CHARS('L', 'I', 'S', 'T');

#define calloc(size,num)	malloc(size*num)

/********************************************************************/

Handle NewHandle (unsigned int numBytes)
{
	void *memPtr;
	HandleRecord *hanPtr;

	memPtr = calloc (numBytes, 1);
	hanPtr = (HandleRecord *) calloc (sizeof (HandleRecord), 1);
	if (hanPtr && (memPtr || numBytes == 0)) {
		hanPtr->ptr = memPtr;
		hanPtr->size = numBytes;
		return (Handle) hanPtr;
	} else {
		free (memPtr);
		free (hanPtr);
		return NULL;
	}
}
/********************************************************************/

void DisposeHandle (Handle handle)
{
	if (handle) {
		free (*handle);
		free ((void *) handle);
	}
}
/********************************************************************/

unsigned int GetHandleSize (Handle handle)
{
	return ((HandleRecord *) handle)->size;
}
/********************************************************************/

int SetHandleSize (Handle handle, unsigned int newSize)
{
	HandleRecord *hanRecPtr = (HandleRecord *) handle;
	void *newPtr, *oldPtr;
	unsigned int oldSize;


	oldPtr = hanRecPtr->ptr;
	oldSize = hanRecPtr->size;

	if (oldSize == newSize)
		return 1;

	if (oldPtr == NULL) {
		newPtr = malloc (newSize);
	} else {
		newPtr = realloc (oldPtr, newSize);
	}
	if (newPtr || (newSize == 0)) {
		hanRecPtr->ptr = newPtr;
		hanRecPtr->size = newSize;
		if (newSize > oldSize)
			memset ((char *) newPtr + oldSize, 0, newSize - oldSize);
		return 1;
	} else
		return 0;
}

#ifdef	CFG_ALL_LIST_FUNCTIONS

/*  Used to compare list elements by their raw data contents */
static int ListMemBlockCmp (void *a, void *b, int size)
{
	return memcmp (a, b, size);
}

/***************************************************************************/

/*
 * Binary search numElements of size elementSize in array for a match
 * to the. item. Return the index of the element that matches
 * (0 - numElements - 1). If no match is found return the -i-1 where
 * i is the index (0 - numElements) where the item should be placed.
 * (*theCmp)(a,b) should return <0 if a<b, 0 if a==b, >0 if a>b.
 *
 * This function is like the C-Library function bsearch() except that
 * this function returns the index where the item should be placed if
 * it is not found.
 */
int BinSearch ( void *array, int numElements, int elementSize,
		void *itemPtr, CompareFunction compareFunction)
{
	int low, high, mid, cmp;
	void *arrayItemPtr;

	for (low = 0, high = numElements - 1, mid = 0, cmp = -1; low <= high;) {
		mid = (low + high) >> 1;

		arrayItemPtr = (void *) (((char *) array) + (mid * elementSize));
		cmp = compareFunction
			? compareFunction (itemPtr, arrayItemPtr)
			: ListMemBlockCmp (itemPtr, arrayItemPtr, elementSize);
		if (cmp == 0) {
			return mid;
		} else if (cmp < 0) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}
	if (cmp > 0)
		mid++;

	return -mid - 1;
}

#endif	/* CFG_ALL_LIST_FUNCTIONS */

/*******************************************************************************/

/*
 * If numNewItems == 0 then expand the list by the number of items
 * indicated by its allocation policy.
 * If numNewItems > 0 then expand the list by exactly the number of
 * items indicated.
 * If numNewItems < 0 then expand the list by the absolute value of
 * numNewItems plus the number of items indicated by its allocation
 * policy.
 * Returns 1 for success, 0 if out of memory
*/
static int ExpandListSpace (list_t list, int numNewItems)
{
	if (numNewItems == 0) {
		numNewItems = NUMITEMSPERALLOC (list);
	} else if (numNewItems < 0) {
		numNewItems = (-numNewItems) + NUMITEMSPERALLOC (list);
	}

	if (SetHandleSize ((Handle) list,
			   sizeof (ListStruct) +
			   ((*list)->listSize +
			   numNewItems) * (*list)->itemSize)) {
		(*list)->listSize += numNewItems;
		return 1;
	} else {
		return 0;
	}
}

/*******************************/

#ifdef	CFG_ALL_LIST_FUNCTIONS

/*
 * This function reallocate the list, minus any currently unused
 * portion of its allotted memory.
 */
void ListCompact (list_t list)
{

	if (!SetHandleSize ((Handle) list,
			    sizeof (ListStruct) +
			    (*list)->numItems * (*list)->itemSize)) {
		return;
	}

	(*list)->listSize = (*list)->numItems;
}

#endif	/* CFG_ALL_LIST_FUNCTIONS */

/*******************************/

list_t ListCreate (int elementSize)
{
	list_t list;

	list = (list_t) (NewHandle (sizeof (ListStruct)));  /* create empty list */
	if (list) {
		(*list)->signature = LIST_SIGNATURE;
		(*list)->numItems = 0;
		(*list)->listSize = 0;
		(*list)->itemSize = elementSize;
		(*list)->percentIncrease = kDefaultAllocationPercentIncrease;
		(*list)->minNumItemsIncrease =
				kDefaultAllocationminNumItemsIncrease;
	}

	return list;
}

/*******************************/

void ListSetAllocationPolicy (list_t list, int minItemsPerAlloc,
			      int percentIncreasePerAlloc)
{
	(*list)->percentIncrease = percentIncreasePerAlloc;
	(*list)->minNumItemsIncrease = minItemsPerAlloc;
}

/*******************************/

void ListDispose (list_t list)
{
	DisposeHandle ((Handle) list);
}
/*******************************/

#ifdef	CFG_ALL_LIST_FUNCTIONS

void ListDisposePtrList (list_t list)
{
	int index;
	int numItems;

	if (list) {
		numItems = ListNumItems (list);

		for (index = 1; index <= numItems; index++)
			free (*(void **) ListGetPtrToItem (list, index));

		ListDispose (list);
	}
}

/*******************************/

/*
 * keeps memory, resets the number of items to 0
 */
void ListClear (list_t list)
{
	if (!list)
		return;
	(*list)->numItems = 0;
}

/*******************************/

/*
 * copy is only as large as necessary
 */
list_t ListCopy (list_t originalList)
{
	list_t tempList = NULL;
	int numItems;

	if (!originalList)
		return NULL;

	tempList = ListCreate ((*originalList)->itemSize);
	if (tempList) {
		numItems = ListNumItems (originalList);

		if (!SetHandleSize ((Handle) tempList,
				    sizeof (ListStruct) +
				    numItems * (*tempList)->itemSize)) {
			ListDispose (tempList);
			return NULL;
		}

		(*tempList)->numItems = (*originalList)->numItems;
		(*tempList)->listSize = (*originalList)->numItems;
		(*tempList)->itemSize = (*originalList)->itemSize;
		(*tempList)->percentIncrease = (*originalList)->percentIncrease;
		(*tempList)->minNumItemsIncrease =
				(*originalList)->minNumItemsIncrease;

		memcpy (ITEMPTR (tempList, 0), ITEMPTR (originalList, 0),
				numItems * (*tempList)->itemSize);
	}

	return tempList;
}

/********************************/

/*
 * list1 = list1 + list2
 */
int ListAppend (list_t list1, list_t list2)
{
	int numItemsL1, numItemsL2;

	if (!list2)
		return 1;

	if (!list1)
		return 0;
	if ((*list1)->itemSize != (*list2)->itemSize)
		return 0;

	numItemsL1 = ListNumItems (list1);
	numItemsL2 = ListNumItems (list2);

	if (numItemsL2 == 0)
		return 1;

	if (!SetHandleSize ((Handle) list1,
			    sizeof (ListStruct) + (numItemsL1 + numItemsL2) *
					(*list1)->itemSize)) {
		return 0;
	}

	(*list1)->numItems = numItemsL1 + numItemsL2;
	(*list1)->listSize = numItemsL1 + numItemsL2;

	memmove (ITEMPTR (list1, numItemsL1),
		 ITEMPTR (list2, 0),
		 numItemsL2 * (*list2)->itemSize);

	return 1;
}

#endif	/* CFG_ALL_LIST_FUNCTIONS */

/*******************************/

/*
 * returns 1 if the item is inserted, returns 0 if out of memory or
 * bad arguments were passed.
 */
int ListInsertItem (list_t list, void *ptrToItem, int itemPosition)
{
	return ListInsertItems (list, ptrToItem, itemPosition, 1);
}

/*******************************/

int ListInsertItems (list_t list, void *ptrToItems, int firstItemPosition,
		     int numItemsToInsert)
{
	int numItems = (*list)->numItems;

	if (firstItemPosition == numItems + 1)
		firstItemPosition = LIST_END;
	else if (firstItemPosition > numItems)
		return 0;

	if ((*list)->numItems >= (*list)->listSize) {
		if (!ExpandListSpace (list, -numItemsToInsert))
			return 0;
	}

	if (firstItemPosition == LIST_START) {
		if (numItems == 0) {
			/* special case for empty list */
			firstItemPosition = LIST_END;
		} else {
			firstItemPosition = 1;
		}
	}

	if (firstItemPosition == LIST_END) {	/* add at the end of the list */
		if (ptrToItems)
			memcpy (ITEMPTR (list, numItems), ptrToItems,
					(*list)->itemSize * numItemsToInsert);
		else
			memset (ITEMPTR (list, numItems), 0,
					(*list)->itemSize * numItemsToInsert);

		(*list)->numItems += numItemsToInsert;
	} else {					/* move part of list up to make room for new item */
		memmove (ITEMPTR (list, firstItemPosition - 1 + numItemsToInsert),
			 ITEMPTR (list, firstItemPosition - 1),
			 (numItems + 1 - firstItemPosition) * (*list)->itemSize);

		if (ptrToItems)
			memmove (ITEMPTR (list, firstItemPosition - 1), ptrToItems,
					 (*list)->itemSize * numItemsToInsert);
		else
			memset (ITEMPTR (list, firstItemPosition - 1), 0,
					(*list)->itemSize * numItemsToInsert);

		(*list)->numItems += numItemsToInsert;
	}

	return 1;
}

#ifdef CFG_ALL_LIST_FUNCTIONS

/*******************************/

int ListEqual (list_t list1, list_t list2)
{
	if (list1 == list2)
		return 1;

	if (list1 == NULL || list2 == NULL)
		return 0;

	if ((*list1)->itemSize == (*list1)->itemSize) {
	    if ((*list1)->numItems == (*list2)->numItems) {
		return (memcmp (ITEMPTR (list1, 0), ITEMPTR (list2, 0),
				(*list1)->itemSize * (*list1)->numItems) == 0);
	    }
	}

	return 0;
}

/*******************************/

/*
 * The item pointed to by ptrToItem is copied over the current item
 * at itemPosition
 */
void ListReplaceItem (list_t list, void *ptrToItem, int itemPosition)
{
	ListReplaceItems (list, ptrToItem, itemPosition, 1);
}

/*******************************/

/*
 * The item pointed to by ptrToItems is copied over the current item
 * at itemPosition
 */
void ListReplaceItems ( list_t list, void *ptrToItems,
			int firstItemPosition, int numItemsToReplace)
{

	if (firstItemPosition == LIST_END)
		firstItemPosition = (*list)->numItems;
	else if (firstItemPosition == LIST_START)
		firstItemPosition = 1;

	memmove (ITEMPTR (list, firstItemPosition - 1), ptrToItems,
			 (*list)->itemSize * numItemsToReplace);
}

/*******************************/

void ListGetItem (list_t list, void *itemDestination, int itemPosition)
{
	ListGetItems (list, itemDestination, itemPosition, 1);
}

#endif	/* CFG_ALL_LIST_FUNCTIONS */

/*******************************/

#if defined(CFG_ALL_LIST_FUNCTIONS) || defined(CFG_DEVICE_DEREGISTER)

void ListRemoveItem (list_t list, void *itemDestination, int itemPosition)
{
	ListRemoveItems (list, itemDestination, itemPosition, 1);
}

/*******************************/

void ListRemoveItems (list_t list, void *itemsDestination,
		      int firstItemPosition, int numItemsToRemove)
{
	int firstItemAfterChunk, numToMove;

	if (firstItemPosition == LIST_START)
		firstItemPosition = 1;
	else if (firstItemPosition == LIST_END)
		firstItemPosition = (*list)->numItems;

	if (itemsDestination != NULL)
		memcpy (itemsDestination, ITEMPTR (list, firstItemPosition - 1),
				(*list)->itemSize * numItemsToRemove);

	firstItemAfterChunk = firstItemPosition + numItemsToRemove;
	numToMove = (*list)->numItems - (firstItemAfterChunk - 1);

	if (numToMove > 0) {
		/*
		 * move part of list down to cover hole left by removed item
		 */
		memmove (ITEMPTR (list, firstItemPosition - 1),
				 ITEMPTR (list, firstItemAfterChunk - 1),
				 (*list)->itemSize * numToMove);
	}

	(*list)->numItems -= numItemsToRemove;
}
#endif	/* CFG_ALL_LIST_FUNCTIONS || CFG_DEVICE_DEREGISTER */

/*******************************/

void ListGetItems (list_t list, void *itemsDestination,
		   int firstItemPosition, int numItemsToGet)
{

	if (firstItemPosition == LIST_START)
		firstItemPosition = 1;
	else if (firstItemPosition == LIST_END)
		firstItemPosition = (*list)->numItems;

	memcpy (itemsDestination,
		ITEMPTR (list, firstItemPosition - 1),
		(*list)->itemSize * numItemsToGet);
}

/*******************************/

/*
 * Returns a pointer to the item at itemPosition. returns null if an
 * errors occurred.
 */
void *ListGetPtrToItem (list_t list, int itemPosition)
{
	if (itemPosition == LIST_START)
		itemPosition = 1;
	else if (itemPosition == LIST_END)
		itemPosition = (*list)->numItems;

	return ITEMPTR (list, itemPosition - 1);
}

/*******************************/

/*
 * returns a pointer the lists data (abstraction violation for
 * optimization)
 */
void *ListGetDataPtr (list_t list)
{
	return &((*list)->itemList[0]);
}

/********************************/

#ifdef	CFG_ALL_LIST_FUNCTIONS

int ListApplyToEach (list_t list, int ascending,
		     ListApplicationFunc funcToApply,
		     void *callbackData)
{
	int result = 0, index;

	if (!list || !funcToApply)
		goto Error;

	if (ascending) {
		for (index = 1; index <= ListNumItems (list); index++) {
			result = funcToApply (index,
					      ListGetPtrToItem (list, index),
					      callbackData);
			if (result < 0)
				goto Error;
		}
	} else {
		for (index = ListNumItems (list);
		     index > 0 && index <= ListNumItems (list);
		     index--) {
			result = funcToApply (index,
					      ListGetPtrToItem (list, index),
					      callbackData);
			if (result < 0)
				goto Error;
		}
	}

Error:
	return result;
}

#endif /* CFG_ALL_LIST_FUNCTIONS */

/********************************/

int ListGetItemSize (list_t list)
{
	return (*list)->itemSize;
}

/********************************/

int ListNumItems (list_t list)
{
	return (*list)->numItems;
}

/*******************************/

#ifdef	CFG_ALL_LIST_FUNCTIONS

void ListRemoveDuplicates (list_t list, CompareFunction compareFunction)
{
	int numItems, index, startIndexForFind, duplicatesIndex;

	numItems = ListNumItems (list);

	for (index = 1; index < numItems; index++) {
		startIndexForFind = index + 1;
		while (startIndexForFind <= numItems) {
			duplicatesIndex =
				ListFindItem (list,
					      ListGetPtrToItem (list, index),
					      startIndexForFind,
					      compareFunction);
			if (duplicatesIndex > 0) {
				ListRemoveItem (list, NULL, duplicatesIndex);
				numItems--;
				startIndexForFind = duplicatesIndex;
			} else {
				break;
			}
		}
	}
}

/*******************************/


/*******************************/

int ListFindItem (list_t list, void *ptrToItem, int startingPosition,
		  CompareFunction compareFunction)
{
	int numItems, size, index, cmp;
	void *listItemPtr;

	if ((numItems = (*list)->numItems) == 0)
		return 0;

	size = (*list)->itemSize;

	if (startingPosition == LIST_START)
		startingPosition = 1;
	else if (startingPosition == LIST_END)
		startingPosition = numItems;

	for (index = startingPosition; index <= numItems; index++) {
		listItemPtr = ITEMPTR (list, index - 1);
		cmp = compareFunction
			? compareFunction (ptrToItem, listItemPtr)
			: ListMemBlockCmp (ptrToItem, listItemPtr, size);
		if (cmp == 0)
			return index;
	}

	return 0;
}

/*******************************/

int ShortCompare (void *a, void *b)
{
	if (*(short *) a < *(short *) b)
		return -1;
	if (*(short *) a > *(short *) b)
		return 1;
	return 0;
}

/*******************************/

int IntCompare (void *a, void *b)
{
	if (*(int *) a < *(int *) b)
		return -1;
	if (*(int *) a > *(int *) b)
		return 1;
	return 0;
}

/*******************************/

int CStringCompare (void *a, void *b)
{
	return strcmp (*(char **) a, *(char **) b);
}

/*******************************/


int ListBinSearch (list_t list, void *ptrToItem,
		   CompareFunction compareFunction)
{
	int index;

	index = BinSearch (ITEMPTR (list, 0),
			   (int) (*list)->numItems,
			   (int) (*list)->itemSize, ptrToItem,
			   compareFunction);

	if (index >= 0)
		index++;			/* lists start from 1 */
	else
		index = 0;			/* item not found */

	return index;
}

/**************************************************************************/

/*
 * Reserves memory for numItems in the list. If it succeeds then
 * numItems items can be inserted without possibility of an out of
 * memory error (useful to simplify error recovery in complex
 * functions). Returns 1 if success, 0 if out of memory.
 */
int ListPreAllocate (list_t list, int numItems)
{
	if ((*list)->listSize - (*list)->numItems < numItems) {
		return ExpandListSpace (list,
					numItems - ((*list)->listSize -
						(*list)->numItems));
	} else {
		return 1;	/* enough items are already pre-allocated */
	}
}

#endif /* CFG_ALL_LIST_FUNCTIONS */
