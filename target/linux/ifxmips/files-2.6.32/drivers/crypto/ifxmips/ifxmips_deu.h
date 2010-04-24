/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2010 Ralph Hempel <ralph.hempel@lantiq.com>
 *   Copyright (C) 2009 Mohammad Firdaus
 */

/*!
  \defgroup IFX_DEU IFX_DEU_DRIVERS
  \ingroup  API
  \brief ifx deu driver module
*/

/*!
  \file	ifxmips_deu.h
  \brief main deu driver header file
*/

/*!
  \defgroup IFX_DEU_DEFINITIONS IFX_DEU_DEFINITIONS
  \ingroup  IFX_DEU
  \brief ifx deu definitions
*/


#ifndef IFXMIPS_DEU_H
#define IFXMIPS_DEU_H

#define IFX_DEU_DRV_VERSION         "1.0.1"

#include "ifxmips_deu_danube.h"

#define IFXDEU_ALIGNMENT 16

#define PFX	"ifxdeu: "

#define IFXDEU_CRA_PRIORITY	300
#define IFXDEU_COMPOSITE_PRIORITY 400


#define IFX_DEU_BASE_ADDR       (KSEG1 | 0x1E103100)
#define IFX_DEU_CLK             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0000))
#define IFX_DEU_ID              ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0008))
#define IFX_DES_CON             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0010))
#define IFX_AES_CON             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0050))
#define IFX_HASH_CON            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00B0))
#define IFX_ARC4_CON            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0100))

#define IFX_DEU_ID_REV          0x00001F
#define IFX_DEU_ID_ID           0x00FF00
#define IFX_DEU_ID_DMA          0x010000
#define IFX_DEU_ID_HASH         0x020000
#define IFX_DEU_ID_AES          0x040000
#define IFX_DEU_ID_3DES         0x080000
#define IFX_DEU_ID_DES          0x100000

#define CRYPTO_DIR_ENCRYPT      1
#define CRYPTO_DIR_DECRYPT      0

#undef CRYPTO_DEBUG

#ifdef CRYPTO_DEBUG
extern char deu_debug_level;
#define DPRINTF(level, format, args...) if (level < deu_debug_level) printk(KERN_INFO "[%s %s %d]: " format, __FILE__, __func__, __LINE__, ##args);
#else
#define DPRINTF(level, format, args...)
#endif

#define IFX_MPS                                 (KSEG1 | 0x1F107000)

#define IFX_MPS_CHIPID                          ((volatile u32*)(IFX_MPS + 0x0344))
#define IFX_MPS_CHIPID_VERSION_GET(value)       (((value) >> 28) & 0xF)
#define IFX_MPS_CHIPID_VERSION_SET(value)       (((value) & 0xF) << 28)
#define IFX_MPS_CHIPID_PARTNUM_GET(value)       (((value) >> 12) & 0xFFFF)
#define IFX_MPS_CHIPID_PARTNUM_SET(value)       (((value) & 0xFFFF) << 12)
#define IFX_MPS_CHIPID_MANID_GET(value)         (((value) >> 1) & 0x7FF)
#define IFX_MPS_CHIPID_MANID_SET(value)         (((value) & 0x7FF) << 1)

void chip_version(void);

int __init ifxdeu_init_des (void);
int __init ifxdeu_init_aes (void);
int __init ifxdeu_init_arc4 (void);
int __init ifxdeu_init_sha1 (void);
int __init ifxdeu_init_md5 (void);
int __init ifxdeu_init_sha1_hmac (void);
int __init ifxdeu_init_md5_hmac (void);

void __exit ifxdeu_fini_des (void);
void __exit ifxdeu_fini_aes (void);
void __exit ifxdeu_fini_arc4 (void);
void __exit ifxdeu_fini_sha1 (void);
void __exit ifxdeu_fini_md5 (void);
void __exit ifxdeu_fini_sha1_hmac (void);
void __exit ifxdeu_fini_md5_hmac (void);
void __exit ifxdeu_fini_dma(void);

int deu_dma_init (void);

#define DEU_WAKELIST_INIT(queue) \
    init_waitqueue_head(&queue)

#define DEU_WAIT_EVENT_TIMEOUT(queue, event, flags, timeout)     \
    do {                                                         \
        wait_event_interruptible_timeout((queue),                \
            test_bit((event), &(flags)), (timeout));            \
        clear_bit((event), &(flags));                            \
    }while (0)


#define DEU_WAKEUP_EVENT(queue, event, flags)         \
    do {                                              \
        set_bit((event), &(flags));                   \
        wake_up_interruptible(&(queue));              \
    }while (0)

#define DEU_WAIT_EVENT(queue, event, flags)           \
    do {                                              \
        wait_event_interruptible(queue,               \
            test_bit((event), &(flags)));             \
        clear_bit((event), &(flags));                 \
    }while (0)

typedef struct deu_drv_priv {
    wait_queue_head_t  deu_thread_wait;
#define DEU_EVENT       1
    volatile  long     deu_event_flags;
    u8                 *deu_rx_buf;
    u32                deu_rx_len;
}deu_drv_priv_t;

#endif	/* IFXMIPS_DEU_H */
