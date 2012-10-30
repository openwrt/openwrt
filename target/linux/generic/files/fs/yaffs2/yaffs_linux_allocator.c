/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 *
 * Note: Tis code is currently unused. Being checked in in case it becomes useful.
 */


#include "yaffs_allocator.h"
#include "yaffs_guts.h"
#include "yaffs_trace.h"
#include "yportenv.h"
#include "yaffs_linux.h"
/*
 * Start out with the same allocator as yaffs direct.
 * Todo: Change to Linux slab allocator.
 */



#define NAMELEN  20
struct yaffs_AllocatorStruct {
	char tnode_name[NAMELEN+1];
	char object_name[NAMELEN+1];
	struct kmem_cache *tnode_cache;
	struct kmem_cache *object_cache;
};

typedef struct yaffs_AllocatorStruct yaffs_Allocator;

int mount_id;

void yaffs_deinit_raw_tnodes_and_objs(yaffs_dev_t *dev)
{
	yaffs_Allocator *allocator = (yaffs_Allocator *)dev->allocator;

	T(YAFFS_TRACE_ALLOCATE,(TSTR("Deinitialising yaffs allocator\n")));

	if(allocator){
		if(allocator->tnode_cache){
			kmem_cache_destroy(allocator->tnode_cache);
			allocator->tnode_cache = NULL;
		} else {
			T(YAFFS_TRACE_ALWAYS,
				(TSTR("NULL tnode cache\n")));
			YBUG();
		}

		if(allocator->object_cache){
			kmem_cache_destroy(allocator->object_cache);
			allocator->object_cache = NULL;
		} else {
			T(YAFFS_TRACE_ALWAYS,
				(TSTR("NULL object cache\n")));
			YBUG();
		}

		YFREE(allocator);

	} else {
		T(YAFFS_TRACE_ALWAYS,
			(TSTR("Deinitialising NULL allocator\n")));
		YBUG();
	}
	dev->allocator = NULL;
}


static void fake_ctor0(void *data){data = data;}
static void fake_ctor1(void *data){data = data;}
static void fake_ctor2(void *data){data = data;}
static void fake_ctor3(void *data){data = data;}
static void fake_ctor4(void *data){data = data;}
static void fake_ctor5(void *data){data = data;}
static void fake_ctor6(void *data){data = data;}
static void fake_ctor7(void *data){data = data;}
static void fake_ctor8(void *data){data = data;}
static void fake_ctor9(void *data){data = data;}

static void (*fake_ctor_list[10]) (void *) = {
	fake_ctor0,
	fake_ctor1,
	fake_ctor2,
	fake_ctor3,
	fake_ctor4,
	fake_ctor5,
	fake_ctor6,
	fake_ctor7,
	fake_ctor8,
	fake_ctor9,
};

void yaffs_init_raw_tnodes_and_objs(yaffs_dev_t *dev)
{
	yaffs_Allocator *allocator;
	unsigned mount_id = yaffs_dev_to_lc(dev)->mount_id;

	T(YAFFS_TRACE_ALLOCATE,(TSTR("Initialising yaffs allocator\n")));

	if(dev->allocator)
		YBUG();
	else if(mount_id >= 10){
		T(YAFFS_TRACE_ALWAYS,(TSTR("Bad mount_id %u\n"),mount_id));
	} else {
		 allocator = YMALLOC(sizeof(yaffs_Allocator));
		 memset(allocator,0,sizeof(yaffs_Allocator));
		 dev->allocator = allocator;

		if(!dev->allocator){
			T(YAFFS_TRACE_ALWAYS,
				(TSTR("yaffs allocator creation failed\n")));
			YBUG();
			return;

		}

		sprintf(allocator->tnode_name,"yaffs_t_%u",mount_id);
		sprintf(allocator->object_name,"yaffs_o_%u",mount_id);

		allocator->tnode_cache =
			kmem_cache_create(allocator->tnode_name,
				dev->tnode_size,
				0, 0,
				fake_ctor_list[mount_id]);
		if(allocator->tnode_cache)
			T(YAFFS_TRACE_ALLOCATE,
				(TSTR("tnode cache \"%s\" %p\n"),
				allocator->tnode_name,allocator->tnode_cache));
		else {
			T(YAFFS_TRACE_ALWAYS,
				(TSTR("yaffs cache creation failed\n")));
			YBUG();
		}


		allocator->object_cache = 
			kmem_cache_create(allocator->object_name,
				sizeof(yaffs_obj_t),
				0, 0,
				fake_ctor_list[mount_id]);

		if(allocator->object_cache)
			T(YAFFS_TRACE_ALLOCATE,
				(TSTR("object cache \"%s\" %p\n"),
				allocator->object_name,allocator->object_cache));

		else {
			T(YAFFS_TRACE_ALWAYS,
				(TSTR("yaffs cache creation failed\n")));
			YBUG();
		}
	} 
}


yaffs_tnode_t *yaffs_alloc_raw_tnode(yaffs_dev_t *dev)
{
	yaffs_Allocator *allocator = dev->allocator;
	if(!allocator || !allocator->tnode_cache){
		YBUG();
		return NULL;
	}
	return kmem_cache_alloc(allocator->tnode_cache, GFP_NOFS);
}

void yaffs_free_raw_tnode(yaffs_dev_t *dev, yaffs_tnode_t *tn)
{
	yaffs_Allocator *allocator = dev->allocator;
	kmem_cache_free(allocator->tnode_cache,tn);
}

yaffs_obj_t *yaffs_alloc_raw_obj(yaffs_dev_t *dev)
{
	yaffs_Allocator *allocator = dev->allocator;
	if(!allocator){
		YBUG();
		return NULL;
	}
	if(!allocator->object_cache){
		YBUG();
		return NULL;
	}
	return kmem_cache_alloc(allocator->object_cache, GFP_NOFS);
}

void yaffs_free_raw_obj(yaffs_dev_t *dev, yaffs_obj_t *obj)
{
	yaffs_Allocator *allocator = dev->allocator;
	kmem_cache_free(allocator->object_cache,obj);
}
