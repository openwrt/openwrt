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
  \ingroup  IFX_API
  \brief ifx deu driver module
*/

/*!
  \file	ifxmips_deu_dma.c
  \ingroup IFX_DEU
  \brief DMA deu driver file
*/

/*!
 \defgroup IFX_DMA_FUNCTIONS IFX_DMA_FUNCTIONS
 \ingroup IFX_DEU
 \brief deu-dma driver functions
*/

/* Project header files */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <asm/io.h>
#include "ifxmips_deu.h"

extern _ifx_deu_device ifx_deu[1];
extern spinlock_t ifx_deu_lock;

//extern deu_drv_priv_t deu_dma_priv;

/*! \fn int deu_dma_intr_handler (struct dma_device_info *dma_dev, int status)
 *  \ingroup IFX_DMA_FUNCTIONS
 *  \brief callback function for deu dma interrupt
 *  \param dma_dev dma device
 *  \param status not used
*/
int deu_dma_intr_handler (struct dma_device_info *dma_dev, int status)
{
#if 0
    int len = 0;
    while (len <= 20000) { len++; }
    u8 *buf;
    int len = 0;


    deu_drv_priv_t *deu_priv = (deu_drv_priv_t *)dma_dev->priv;
    //printk("status:%d \n",status);
    switch(status) {
        case RCV_INT:
            len = dma_device_read(dma_dev, (u8 **)&buf, NULL);
            if ( len != deu_priv->deu_rx_len) {
                printk(KERN_ERR "%s packet length %d is not equal to expect %d\n",
                    __func__, len, deu_priv->deu_rx_len);
                return -1;
            }
            memcpy(deu_priv->deu_rx_buf, buf, deu_priv->deu_rx_len);
            /* Reset for next usage */
            deu_priv->deu_rx_buf = NULL;
            deu_priv->deu_rx_len = 0;
            DEU_WAKEUP_EVENT(deu_priv->deu_thread_wait, DEU_EVENT, deu_priv->deu_event_flags);
            break;
        case TX_BUF_FULL_INT:
             // delay for buffer to be cleared
             while (len <= 20000) { len++; }
             break;

        case TRANSMIT_CPT_INT:
            break;
        default:
            break;
    }
#endif
    return 0;
}

extern u8 *g_dma_block;
extern u8 *g_dma_block2;

/*! \fn u8 *deu_dma_buffer_alloc (int len, int *byte_offset, void **opt)
 *  \ingroup IFX_DMA_FUNCTIONS
 *  \brief callback function for allocating buffers for dma receive descriptors
 *  \param len not used
 *  \param byte_offset dma byte offset
 *  \param *opt not used
 *
*/
u8 *deu_dma_buffer_alloc (int len, int *byte_offset, void **opt)
{
    u8 *swap = NULL;

    // dma-core needs at least 2 blocks of memory
    swap = g_dma_block;
    g_dma_block = g_dma_block2;
    g_dma_block2 = swap;

    //dma_cache_wback_inv((unsigned long) g_dma_block, (PAGE_SIZE >> 1));
    *byte_offset = 0;

    return g_dma_block;
}

/*! \fn int deu_dma_buffer_free (u8 * dataptr, void *opt)
 *  \ingroup IFX_DMA_FUNCTIONS
 *  \brief callback function for freeing dma transmit descriptors
 *  \param dataptr data pointer to be freed
 *  \param opt not used
*/
int deu_dma_buffer_free (u8 *dataptr, void *opt)
{
#if 0
    printk("Trying to free memory buffer\n");
    if (dataptr == NULL && opt == NULL)
        return 0;
    else if (opt == NULL) {
        kfree(dataptr);
        return 1;
    }
    else if (dataptr == NULL) {
       kfree(opt);
       return 1;
    }
    else {
       kfree(opt);
       kfree(dataptr);
    }
#endif
    return 0;
}

