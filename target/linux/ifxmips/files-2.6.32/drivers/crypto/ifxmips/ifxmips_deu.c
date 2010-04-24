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
  \ingroup API
  \brief ifx deu driver module
*/

/*!
  \file	ifxmips_deu.c
  \ingroup IFX_DEU
  \brief main deu driver file
*/

/*!
 \defgroup IFX_DEU_FUNCTIONS IFX_DEU_FUNCTIONS
 \ingroup IFX_DEU
 \brief IFX DEU functions
*/

/* Project header */
#include <linux/version.h>
#if defined(CONFIG_MODVERSIONS)
#define MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/crypto.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>       /* Stuff about file systems that we need */
#include <asm/byteorder.h>
#include <ifxmips_pmu.h>
#include "ifxmips_deu.h"

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
int disable_deudma = 0;
#else
int disable_deudma = 1;
#endif /* CONFIG_CRYPTO_DEV_IFXMIPS_DMA */

#ifdef CRYPTO_DEBUG
char deu_debug_level = 3;
#endif

/*! \fn static int __init deu_init (void)
 *  \ingroup IFX_DEU_FUNCTIONS
 *  \brief link all modules that have been selected in kernel config for ifx hw crypto support
 *  \return ret
*/
static int __init deu_init (void)
{
    int ret = -ENOSYS;
    u32 config;

    volatile struct clc_controlr_t *clc = (struct clc_controlr_t *) IFX_DEU_CLK;

    ifxmips_pmu_enable(1<<20);

    printk(KERN_INFO "Lantiq crypto hardware driver version %s\n", IFX_DEU_DRV_VERSION);

    chip_version();

    clc->FSOE = 0;
    clc->SBWE = 0;
    clc->SPEN = 0;
    clc->SBWE = 0;
    clc->DISS = 0;
    clc->DISR = 0;

    config = *IFX_DEU_ID;

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
    deu_dma_init ();
#endif

#if defined(CONFIG_CRYPTO_DEV_IFXMIPS_DES)
    if(config & IFX_DEU_ID_DES) {
        if ((ret = ifxdeu_init_des ())) {
            printk (KERN_ERR "IFX DES initialization failed!\n");
        }
    } else {
        printk (KERN_ERR "IFX DES not supported!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_IFXMIPS_AES)
    if(config & IFX_DEU_ID_AES) {
        if ((ret = ifxdeu_init_aes ())) {
            printk (KERN_ERR "IFX AES initialization failed!\n");
        }
    } else {
        printk (KERN_ERR "IFX AES not supported!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_IFXMIPS_ARC4)
    if ((ret = ifxdeu_init_arc4 ())) {
        printk (KERN_ERR "IFX ARC4 initialization failed!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_IFXMIPS_SHA1)
    if(config & IFX_DEU_ID_HASH) {
        if ((ret = ifxdeu_init_sha1 ())) {
            printk (KERN_ERR "IFX SHA1 initialization failed!\n");
        }
    } else {
        printk (KERN_ERR "IFX SHA1 not supported!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_IFXMIPS_MD5)
    if(config & IFX_DEU_ID_HASH) {
        if ((ret = ifxdeu_init_md5 ())) {
            printk (KERN_ERR "IFX MD5 initialization failed!\n");
        }
    } else {
        printk (KERN_ERR "IFX MD5 not supported!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_IFXMIPS_SHA1_HMAC)
    if ((ret = ifxdeu_init_sha1_hmac ())) {
        printk (KERN_ERR "IFX SHA1_HMAC initialization failed!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_IFXMIPS_MD5_HMAC)
    if ((ret = ifxdeu_init_md5_hmac ())) {
        printk (KERN_ERR "IFX MD5_HMAC initialization failed!\n");
    }
#endif
    return ret;
}

/*! \fn static void __exit deu_fini (void)
 *  \ingroup IFX_DEU_FUNCTIONS
 *  \brief remove the loaded crypto algorithms
*/
static void __exit deu_fini (void)
{
    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_DES)
    ifxdeu_fini_des ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_AES)
    ifxdeu_fini_aes ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_ARC4)
    ifxdeu_fini_arc4 ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_SHA1)
    ifxdeu_fini_sha1 ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_MD5)
    ifxdeu_fini_md5 ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_SHA1_HMAC)
    ifxdeu_fini_sha1_hmac ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_MD5_HMAC)
    ifxdeu_fini_md5_hmac ();
    #endif
    printk("DEU has exited successfully\n");

    #if defined(CONFIG_CRYPTO_DEV_IFXMIPS_DMA)
    ifxdeu_fini_dma();
    printk("DMA has deregistered successfully\n");
    #endif
}

module_init (deu_init);
module_exit (deu_fini);

MODULE_DESCRIPTION ("Infineon crypto engine support.");
MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Mohammad Firdaus");
