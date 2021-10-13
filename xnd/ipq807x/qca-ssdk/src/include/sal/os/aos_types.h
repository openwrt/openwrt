/*
 * Copyright (c) 2012,2018, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _AOS_TYPES_H
#define _AOS_TYPES_H

#include "aos_types_pvt.h"

#ifndef NULL
#define NULL 0
#endif

/**
 * @g aos_types types
 * @{
 *
 * @ig shim_ext
 */
/*
 *@ basic data types.
 */
typedef enum
{
    A_FALSE,
    A_TRUE
} a_bool_t;

typedef __a_uint8_t    a_uint8_t;
typedef __a_int8_t     a_int8_t;
typedef __a_uint16_t   a_uint16_t;
typedef __a_int16_t    a_int16_t;
typedef __a_uint32_t   a_uint32_t;
typedef __a_int32_t    a_int32_t;
typedef __a_uint64_t   a_uint64_t;
typedef __a_int64_t    a_int64_t;
typedef unsigned long  a_ulong_t;


typedef void *                   acore_t;

/**
 * @brief Platform/bus generic handle. Used for bus specific functions.
 */
typedef __aos_device_t              aos_device_t;

/**
 * @brief size of an object
 */
typedef __aos_size_t                aos_size_t;

/**
 * @brief Generic status to be used by acore.
 */
typedef enum
{
    A_STATUS_OK,
    A_STATUS_FAILED,
    A_STATUS_ENOENT,
    A_STATUS_ENOMEM,
    A_STATUS_EINVAL,
    A_STATUS_EINPROGRESS,
    A_STATUS_ENOTSUPP,
    A_STATUS_EBUSY,
} a_status_t;

/*
 * An ecore needs to provide a table of all pci device/vendor id's it
 * supports
 *
 * This table should be terminated by a NULL entry , i.e. {0}
 */
typedef struct
{
    a_uint32_t vendor;
    a_uint32_t device;
    a_uint32_t subvendor;
    a_uint32_t subdevice;
} aos_pci_dev_id_t;

#define AOS_PCI_ANY_ID  (~0)

/*
 * Typically core's can use this macro to create a table of various device
 * ID's
 */
#define AOS_PCI_DEVICE(_vendor, _device)   \
    (_vendor), (_device), AOS_PCI_ANY_ID, AOS_PCI_ANY_ID


typedef __aos_iomem_t   aos_iomem_t;
/*
 * These define the hw resources the OS has allocated for the device
 * Note that start defines a mapped area.
 */
typedef enum
{
    AOS_RESOURCE_TYPE_MEM,
    AOS_RESOURCE_TYPE_IO,
} aos_resource_type_t;

typedef struct
{
    a_uint32_t          start;
    a_uint32_t          end;
    aos_resource_type_t type;
} aos_resource_t;

#define AOS_DEV_ID_TABLE_MAX    256

typedef union
{
    aos_pci_dev_id_t  *pci;
    void              *raw;
} aos_bus_reg_data_t;

typedef void *aos_attach_data_t;

#define AOS_REGIONS_MAX     5

typedef enum
{
    AOS_BUS_TYPE_PCI = 1,
    AOS_BUS_TYPE_GENERIC,
} aos_bus_type_t;

typedef enum
{
    AOS_IRQ_NONE,
    AOS_IRQ_HANDLED,
} aos_irq_resp_t;

typedef enum
{
    AOS_DMA_MASK_32BIT,
    AOS_DMA_MASK_64BIT,
} aos_dma_mask_t;


/**
 * @brief DMA directions
 */
typedef enum
{
    AOS_DMA_TO_DEVICE = 0,     /**< Data is transfered from device to memory  */
    AOS_DMA_FROM_DEVICE,       /**< Data is transfered from memory to device  */
} aos_dma_dir_t;

/*
 * Protoypes shared between public and private headers
 */


/*
 * work queue(kernel thread) function callback
 */
typedef void (*aos_work_func_t)(void *);

/**
 * @brief Prototype of the critical region function that is to be
 * executed with spinlock held and interrupt disalbed
 */
typedef a_bool_t (*aos_irqlocked_func_t)(void *);

/**
 * @brief Prototype of timer function
 */
typedef void (*aos_timer_func_t)(void *);

#endif
