/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "yaffs_packedtags2.h"
#include "yportenv.h"
#include "yaffs_tagsvalidity.h"

/* This code packs a set of extended tags into a binary structure for
 * NAND storage
 */

/* Some of the information is "extra" struff which can be packed in to
 * speed scanning
 * This is defined by having the EXTRA_HEADER_INFO_FLAG set.
 */

/* Extra flags applied to chunkId */

#define EXTRA_HEADER_INFO_FLAG	0x80000000
#define EXTRA_SHRINK_FLAG	0x40000000
#define EXTRA_SHADOWS_FLAG	0x20000000
#define EXTRA_SPARE_FLAGS	0x10000000

#define ALL_EXTRA_FLAGS		0xF0000000

/* Also, the top 4 bits of the object Id are set to the object type. */
#define EXTRA_OBJECT_TYPE_SHIFT (28)
#define EXTRA_OBJECT_TYPE_MASK  ((0x0F) << EXTRA_OBJECT_TYPE_SHIFT)

static void yaffs_DumpPackedTags2(const yaffs_PackedTags2 * pt)
{
	T(YAFFS_TRACE_MTD,
	  (TSTR("packed tags obj %d chunk %d byte %d seq %d" TENDSTR),
	   pt->t.objectId, pt->t.chunkId, pt->t.byteCount,
	   pt->t.sequenceNumber));
}

static void yaffs_DumpTags2(const yaffs_ExtendedTags * t)
{
	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("ext.tags eccres %d blkbad %d chused %d obj %d chunk%d byte "
	    "%d del %d ser %d seq %d"
	    TENDSTR), t->eccResult, t->blockBad, t->chunkUsed, t->objectId,
	   t->chunkId, t->byteCount, t->chunkDeleted, t->serialNumber,
	   t->sequenceNumber));

}

void yaffs_PackTags2(yaffs_PackedTags2 * pt, const yaffs_ExtendedTags * t)
{
	pt->t.chunkId = t->chunkId;
	pt->t.sequenceNumber = t->sequenceNumber;
	pt->t.byteCount = t->byteCount;
	pt->t.objectId = t->objectId;

	if (t->chunkId == 0 && t->extraHeaderInfoAvailable) {
		/* Store the extra header info instead */
		/* We save the parent object in the chunkId */
		pt->t.chunkId = EXTRA_HEADER_INFO_FLAG
			| t->extraParentObjectId;
		if (t->extraIsShrinkHeader) {
			pt->t.chunkId |= EXTRA_SHRINK_FLAG;
		}
		if (t->extraShadows) {
			pt->t.chunkId |= EXTRA_SHADOWS_FLAG;
		}

		pt->t.objectId &= ~EXTRA_OBJECT_TYPE_MASK;
		pt->t.objectId |=
		    (t->extraObjectType << EXTRA_OBJECT_TYPE_SHIFT);

		if (t->extraObjectType == YAFFS_OBJECT_TYPE_HARDLINK) {
			pt->t.byteCount = t->extraEquivalentObjectId;
		} else if (t->extraObjectType == YAFFS_OBJECT_TYPE_FILE) {
			pt->t.byteCount = t->extraFileLength;
		} else {
			pt->t.byteCount = 0;
		}
	}

	yaffs_DumpPackedTags2(pt);
	yaffs_DumpTags2(t);

#ifndef YAFFS_IGNORE_TAGS_ECC
	{
		yaffs_ECCCalculateOther((unsigned char *)&pt->t,
					sizeof(yaffs_PackedTags2TagsPart),
					&pt->ecc);
	}
#endif
}

void yaffs_UnpackTags2(yaffs_ExtendedTags * t, yaffs_PackedTags2 * pt)
{

	memset(t, 0, sizeof(yaffs_ExtendedTags));

	yaffs_InitialiseTags(t);

	if (pt->t.sequenceNumber != 0xFFFFFFFF) {
		/* Page is in use */
#ifdef YAFFS_IGNORE_TAGS_ECC
		{
			t->eccResult = YAFFS_ECC_RESULT_NO_ERROR;
		}
#else
		{
			yaffs_ECCOther ecc;
			int result;
			yaffs_ECCCalculateOther((unsigned char *)&pt->t,
						sizeof
						(yaffs_PackedTags2TagsPart),
						&ecc);
			result =
			    yaffs_ECCCorrectOther((unsigned char *)&pt->t,
						  sizeof
						  (yaffs_PackedTags2TagsPart),
						  &pt->ecc, &ecc);
			switch(result){
				case 0:
					t->eccResult = YAFFS_ECC_RESULT_NO_ERROR;
					break;
				case 1:
					t->eccResult = YAFFS_ECC_RESULT_FIXED;
					break;
				case -1:
					t->eccResult = YAFFS_ECC_RESULT_UNFIXED;
					break;
				default:
					t->eccResult = YAFFS_ECC_RESULT_UNKNOWN;
			}
		}
#endif
		t->blockBad = 0;
		t->chunkUsed = 1;
		t->objectId = pt->t.objectId;
		t->chunkId = pt->t.chunkId;
		t->byteCount = pt->t.byteCount;
		t->chunkDeleted = 0;
		t->serialNumber = 0;
		t->sequenceNumber = pt->t.sequenceNumber;

		/* Do extra header info stuff */

		if (pt->t.chunkId & EXTRA_HEADER_INFO_FLAG) {
			t->chunkId = 0;
			t->byteCount = 0;

			t->extraHeaderInfoAvailable = 1;
			t->extraParentObjectId =
			    pt->t.chunkId & (~(ALL_EXTRA_FLAGS));
			t->extraIsShrinkHeader =
			    (pt->t.chunkId & EXTRA_SHRINK_FLAG) ? 1 : 0;
			t->extraShadows =
			    (pt->t.chunkId & EXTRA_SHADOWS_FLAG) ? 1 : 0;
			t->extraObjectType =
			    pt->t.objectId >> EXTRA_OBJECT_TYPE_SHIFT;
			t->objectId &= ~EXTRA_OBJECT_TYPE_MASK;

			if (t->extraObjectType == YAFFS_OBJECT_TYPE_HARDLINK) {
				t->extraEquivalentObjectId = pt->t.byteCount;
			} else {
				t->extraFileLength = pt->t.byteCount;
			}
		}
	}

	yaffs_DumpPackedTags2(pt);
	yaffs_DumpTags2(t);

}
