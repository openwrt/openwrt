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
  \brief deu driver module
*/

/*!
  \file	ifxmips_deu_danube.c
  \ingroup IFX_DEU
  \brief board specific deu driver file for danube
*/

/*!
  \defgroup BOARD_SPECIFIC_FUNCTIONS IFX_BOARD_SPECIFIC_FUNCTIONS
  \ingroup IFX_DEU
  \brief board specific deu functions
*/

/* Project header files */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <asm/io.h> //dma_cache_inv
#include "ifxmips_deu.h"

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
u32 *des_buff_in = NULL;
u32 *des_buff_out = NULL;
u32 *aes_buff_in = NULL;
u32 *aes_buff_out = NULL;
_ifx_deu_device ifx_deu[1];
#endif

/* Function Declerations */
int aes_memory_allocate(int value);
int des_memory_allocate(int value);
void memory_release(u32 *addr);
int aes_chip_init (void);
void des_chip_init (void);
int deu_dma_init (void);
u32 endian_swap(u32 input);
u32* memory_alignment(const u8 *arg, u32 *buff_alloc, int in_out, int nbytes);
void dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes);
void __exit ifxdeu_fini_dma(void);

#define DES_3DES_START  IFX_DES_CON
#define AES_START       IFX_AES_CON

/* Variables definition */
int ifx_danube_pre_1_4;
u8 *g_dma_page_ptr = NULL;
u8 *g_dma_block = NULL;
u8 *g_dma_block2 = NULL;


/*! \fn int deu_dma_init (void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief Initialize DMA for DEU usage. DMA specific registers are
 *         intialized here, including a pointer to the device, memory
 *         space for the device and DEU-DMA descriptors
 *  \return -1 if fail, otherwise return 0
*/

#ifdef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
int deu_dma_init (void)
{
    struct dma_device_info *dma_device = NULL;
    int i = 0;
    volatile struct deu_dma_t *dma = (struct deu_dma_t *) IFX_DEU_DMA_CON;
    struct dma_device_info *deu_dma_device_ptr;

    // get one free page and share between g_dma_block and g_dma_block2
    printk("PAGE_SIZE = %ld\n", PAGE_SIZE);
    g_dma_page_ptr = (u8 *)__get_free_page(GFP_KERNEL); // need 16-byte alignment memory block
    g_dma_block = g_dma_page_ptr; // need 16-byte alignment memory block
    g_dma_block2 = (u8 *)(g_dma_page_ptr + (PAGE_SIZE >> 1)); // need 16-byte alignment memory block


    deu_dma_device_ptr = dma_device_reserve ("DEU");
    if (!deu_dma_device_ptr) {
        printk ("DEU: reserve DMA fail!\n");
        return -1;
    }
    ifx_deu[0].dma_device = deu_dma_device_ptr;
    dma_device = deu_dma_device_ptr;
    //dma_device->priv = &deu_dma_priv;
    dma_device->buffer_alloc = &deu_dma_buffer_alloc;
    dma_device->buffer_free = &deu_dma_buffer_free;
    dma_device->intr_handler = &deu_dma_intr_handler;
    dma_device->tx_endianness_mode = IFX_DMA_ENDIAN_TYPE3;
    dma_device->rx_endianness_mode = IFX_DMA_ENDIAN_TYPE3;
    dma_device->port_num = 1;
    dma_device->tx_burst_len = 4;
    dma_device->max_rx_chan_num = 1;
    dma_device->max_tx_chan_num = 1;
    dma_device->port_packet_drop_enable = 0;

    for (i = 0; i < dma_device->max_rx_chan_num; i++) {
        dma_device->rx_chan[i]->packet_size = DEU_MAX_PACKET_SIZE;
        dma_device->rx_chan[i]->desc_len = 1;
        dma_device->rx_chan[i]->control = IFX_DMA_CH_ON;
        dma_device->rx_chan[i]->byte_offset = 0;
        dma_device->rx_chan[i]->chan_poll_enable = 1;

    }

    for (i = 0; i < dma_device->max_tx_chan_num; i++) {
        dma_device->tx_chan[i]->control = IFX_DMA_CH_ON;
        dma_device->tx_chan[i]->desc_len = 1;
        dma_device->tx_chan[i]->chan_poll_enable = 1;
    }

    dma_device->current_tx_chan = 0;
    dma_device->current_rx_chan = 0;

    dma_device_register (dma_device);
    for (i = 0; i < dma_device->max_rx_chan_num; i++) {
        (dma_device->rx_chan[i])->open (dma_device->rx_chan[i]);
    }

    dma->controlr.BS = 0;
    dma->controlr.RXCLS = 0;
    dma->controlr.EN = 1;


    *IFX_DMA_PS = 1;

    /* DANUBE PRE 1.4 SOFTWARE FIX */
    if (ifx_danube_pre_1_4)
        *IFX_DMA_PCTRL = 0x14;
    else
        *IFX_DMA_PCTRL = 0xF14;

    return 0;
}

/*! \fn u32 *memory_alignment(const u8 *arg, u32 *buffer_alloc, int in_buff, int nbytes)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief  A fix to align mis-aligned address for Danube version 1.3 chips which has
 *          memory alignment issues.
 *  \param arg Pointer to the input / output memory address
 *  \param buffer_alloc A pointer to the buffer
 *  \param in_buff Input (if == 1) or Output (if == 0) buffer
 *  \param nbytes Number of bytes of data
 *  \return returns arg: if address is aligned, buffer_alloc: if memory address is not aligned
*/

u32 *memory_alignment(const u8 *arg, u32 *buffer_alloc, int in_buff, int nbytes)
{
    if (ifx_danube_pre_1_4) {
    /* for input buffer */
        if(in_buff) {
            if (((u32) arg) & 0xF) {
                memcpy(buffer_alloc, arg, nbytes);
                return (u32 *) buffer_alloc;
            }
            else
                return (u32 *) arg;
        }
        else {
    /* for output buffer */
            if (((u32) arg) & 0x3)
                return buffer_alloc;
            else
                return (u32 *) arg;
        }
    }

   return (u32 *) arg;
}

/*! \fn  void aes_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief copy the DMA data to the memory address space for AES. The swaping of the 4 bytes
 *         is done only for Danube version 1.3 (FIX). Otherwise, it is a direct memory copy
 *         to out_arg pointer
 *  \param outcopy Pointer to the address to store swapped copy
 *  \param out_dma A pointer to the memory address that stores the DMA data
 *  \param out_arg The pointer to the memory address that needs to be copied to
 *  \param nbytes Number of bytes of data
*/

void aes_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes)
{
    int i = 0;
    int x = 0;

    /* DANUBE PRE 1.4 SOFTWARE FIX */
    if (ifx_danube_pre_1_4) {
        for (i = 0; i < (nbytes / 4); i++) {
           x = i ^ 0x3;
           outcopy[i] = out_dma[x];

        }
        if (((u32) out_arg) & 0x3) {
            memcpy((u8 *)out_arg, outcopy, nbytes);
        }
    }
    else
        memcpy (out_arg, out_dma, nbytes);
}

/*! \fn void des_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief copy the DMA data to the memory address space for DES. The swaping of the 4 bytes
 *         is done only for Danube version 1.3 (FIX). Otherwise, it is a direct memory copy
 *         to out_arg pointer
 *
 *  \param outcopy Pointer to the address to store swapped copy
 *  \param out_dma A pointer to the memory address that stores the DMA data
 *  \param out_arg The pointer to the memory address that needs to be copied to
 *  \param nbytes Number of bytes of data
*/

void des_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes)
{
    int i = 0;
    int x = 0;

    /* DANUBE PRE 1.4 SOFTWARE FIX */
    if (ifx_danube_pre_1_4) {
        for (i = 0; i < (nbytes / 4); i++) {
           x = i ^ 1;
           outcopy[i] = out_dma[x];

        }
        if (((u32) out_arg) & 0x3) {
            memcpy((u8 *)out_arg, outcopy, nbytes);
        }
    }
    else
        memcpy (out_arg, out_dma, nbytes);
}

/*! \fn int des_memory_allocate(int value)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief allocates memory to the necessary memory input/output buffer location, used during
 *          the DES algorithm DMA transfer (memory alignment issues)
 *  \param  value value determinds whether the calling of the function is for a input buffer
 *                or for an output buffer memory allocation
*/

int des_memory_allocate(int value)
{
    if (ifx_danube_pre_1_4) {
        if (value == BUFFER_IN) {
            des_buff_in = kmalloc(DEU_MAX_PACKET_SIZE, GFP_ATOMIC);
            if (!des_buff_in)
                return -1;
            else
                return 0;
        }
        else {
            des_buff_out = kmalloc(DEU_MAX_PACKET_SIZE, GFP_ATOMIC);
            if (!des_buff_out)
                return -1;
            else
	        return 0;
        }
    }

    else
        return 0;
}

/*! \fn int aes_memory_allocate(int value)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief allocates memory to the necessary memory input/output buffer location, used during
 *  	   the AES algorithm DMA transfer (memory alignment issues)
 *  \param value value determinds whether the calling of the function is for a input buffer
 *               or for an output buffer memory allocation
*/

int aes_memory_allocate(int value)
{
    if (ifx_danube_pre_1_4) {
        if (value == BUFFER_IN) {
            aes_buff_in = kmalloc(DEU_MAX_PACKET_SIZE, GFP_ATOMIC);
            if (!aes_buff_in)
                return -1;
            else
                return 0;
        }
        else {
            aes_buff_out = kmalloc(DEU_MAX_PACKET_SIZE, GFP_ATOMIC);
            if (!aes_buff_out)
                return -1;
            else
	        return 0;
        }
    }

    else
        return 0;
}

/*! \fn void memory_release(u32 *addr)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief frees previously allocated memory
 *  \param addr memory address of the buffer that needs to be freed
*/

void memory_release(u32 *addr)
{
    if (addr)
        kfree(addr);
    return;
}

/*! \fn	__exit ifxdeu_fini_dma(void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief unregister dma devices after exit
*/

void __exit ifxdeu_fini_dma(void)
{
    if (g_dma_page_ptr)
        free_page((u32) g_dma_page_ptr);
    dma_device_release(ifx_deu[0].dma_device);
    dma_device_unregister(ifx_deu[0].dma_device);

}

#endif /* CONFIG_CRYPTO_DEV_IFXMIPS_DMA */

/*! \fn u32 endian_swap(u32 input)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief function is not used
 *  \param input Data input to be swapped
 *  \return input
*/

u32 endian_swap(u32 input)
{
    return input;
}

/*! \fn	u32 input_swap(u32 input)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief Swap the input data if the current chip is Danube version
 *         1.4 and do nothing to the data if the current chip is
 *         Danube version 1.3
 *  \param input data that needs to be swapped
 *  \return input or swapped input
*/

u32 input_swap(u32 input)
{
    if (!ifx_danube_pre_1_4) {
        u8 *ptr = (u8 *)&input;
        return ((ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0]);
    }
    else
        return input;
}



/*! \fn void aes_chip_init (void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 * \brief initialize AES hardware
*/

int aes_chip_init (void)
{
    volatile struct aes_t *aes = (struct aes_t *) AES_START;

#ifndef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
    //start crypto engine with write to ILR
    aes->controlr.SM = 1;
    aes->controlr.ARS = 1;
#else
    aes->controlr.SM = 1;
    aes->controlr.ARS = 1; // 0 for dma
#endif
    return 0;
}

/*! \fn void des_chip_init (void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief initialize DES hardware
*/

void des_chip_init (void)
{
        volatile struct des_t *des = (struct des_t *) DES_3DES_START;

#ifndef CONFIG_CRYPTO_DEV_IFXMIPS_DMA
        // start crypto engine with write to ILR
        des->controlr.SM = 1;
        des->controlr.ARS = 1;
#else
        des->controlr.SM = 1;
        des->controlr.ARS = 1; // 0 for dma

#endif
}

/*! \fn void chip_version (void)
 *  \ingroup IFX_DES_FUNCTIONS
 *  \brief To find the version of the chip by looking at the chip ID
 *  \param ifx_danube_pre_1_4 (sets to 1 if Chip is Danube less than v1.4)
*/

void chip_version(void)
{
    /* DANUBE PRE 1.4 SOFTWARE FIX */
    int chip_id = 0;
    chip_id = *IFX_MPS_CHIPID;
    chip_id >>= 28;

    if (chip_id >= 4) {
        ifx_danube_pre_1_4 = 0;
        printk("Danube Chip ver. 1.4 detected. \n");
    }
    else {
        ifx_danube_pre_1_4 = 1;
        printk("Danube Chip ver. 1.3 or below detected. \n");
    }

    return;
}

