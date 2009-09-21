/*
 * Copyright (c) 2007 Atheros Communications Inc.
 * All rights reserved.
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 */

#ifndef __DSET_INTERNAL_H__
#define __DSET_INTERNAL_H__

/*
 * Internal dset definitions, common for DataSet layer.
 */

#define DSET_TYPE_STANDARD      0
#define DSET_TYPE_BPATCHED      1
#define DSET_TYPE_COMPRESSED    2

/* Dataset descriptor */

typedef struct dset_descriptor_s {
  struct dset_descriptor_s  *next;         /* List link. NULL only at the last
                                              descriptor */
  A_UINT16                   id;           /* Dset ID */
  A_UINT16                   size;         /* Dset size. */
  void                      *DataPtr;      /* Pointer to raw data for standard
                                              DataSet or pointer to original
                                              dset_descriptor for patched
                                              DataSet */
  A_UINT32                   data_type;    /* DSET_TYPE_*, above */

  void                      *AuxPtr;       /* Additional data that might
                                              needed for data_type. For
                                              example, pointer to patch
                                              Dataset descriptor for BPatch. */
} dset_descriptor_t;

#endif /* __DSET_INTERNAL_H__ */
