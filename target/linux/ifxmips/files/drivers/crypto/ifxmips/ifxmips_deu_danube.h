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
 *   Copyright (C) 2009 Mohammad Firdaus / Infineon Technologies
 */

/*!
  \defgroup IFX_DEU IFX_DEU_DRIVERS
  \ingroup API
  \brief deu driver module
*/

/*!
  \file    ifxmips_deu_danube.h
  \brief board specific driver header file for danube
*/

/*!
  \defgroup BOARD_SPECIFIC_FUNCTIONS IFX_BOARD_SPECIFIC_FUNCTIONS
  \ingroup IFX_DEU
  \brief board specific deu header files
*/

#ifndef IFXMIPS_DEU_DANUBE_H
#define IFXMIPS_DEU_DANUBE_H

#ifdef CONFIG_CRYPTO_DEV_DMA
#define DEU_DWORD_REORDERING(ptr, buffer, in_out, bytes)      memory_alignment(ptr, buffer, in_out, bytes)
#define AES_MEMORY_COPY(outcopy, out_dma, out_arg, nbytes)    aes_dma_memory_copy(outcopy, out_dma, out_arg, nbytes)
#define DES_MEMORY_COPY(outcopy, out_dma, out_arg, nbytes)    des_dma_memory_copy(outcopy, out_dma, out_arg, nbytes)
#define BUFFER_IN       1
#define BUFFER_OUT      0
#define DELAY_PERIOD    9
#define AES_ALGO        1
#define DES_ALGO        0
#define FREE_MEMORY(buff)   memory_release(buff)
#define ALLOCATE_MEMORY(val, type) type ? aes_memory_allocate(val) : des_memory_allocate(val)
#endif /* CONFIG_CRYPTO_DEV_DMA */

#define INPUT_ENDIAN_SWAP(input) input_swap(input)
#define DEU_ENDIAN_SWAP(input)   endian_swap(input)
#define AES_DMA_MISC_CONFIG()

#define WAIT_AES_DMA_READY()          \
    do {                   \
        int i;                  \
        volatile struct deu_dma_t *dma = (struct deu_dma_t *) IFX_DEU_DMA_CON; \
        volatile struct aes_t *aes = (volatile struct aes_t *) AES_START; \
        for (i = 0; i < 10; i++)      \
            udelay(DELAY_PERIOD);     \
        while (dma->controlr.BSY) {}; \
        while (aes->controlr.BUS) {}; \
    } while (0)

#define WAIT_DES_DMA_READY()          \
    do {                   \
        int i;                  \
        volatile struct deu_dma_t *dma = (struct deu_dma_t *) IFX_DEU_DMA_CON; \
        volatile struct des_t *des = (struct des_t *) DES_3DES_START; \
        for (i = 0; i < 10; i++)      \
            udelay(DELAY_PERIOD);     \
        while (dma->controlr.BSY) {}; \
        while (des->controlr.BUS) {}; \
    } while (0)

#define SHA_HASH_INIT                  \
    do {                               \
        volatile struct deu_hash_t *hash = (struct deu_hash_t *) HASH_START; \
        hash->controlr.SM = 1;    \
        hash->controlr.ALGO = 0;  \
        hash->controlr.INIT = 1;  \
    } while(0)

/* DEU STRUCTURES */

struct clc_controlr_t {
    u32 Res:26;
    u32 FSOE:1;
    u32 SBWE:1;
    u32 EDIS:1;
    u32 SPEN:1;
    u32 DISS:1;
    u32 DISR:1;

};

struct des_t {
    struct des_controlr {
        u32 KRE:1;
        u32 reserved1:5;
        u32 GO:1;
        u32 STP:1;
        u32 Res2:6;
        u32 NDC:1;
        u32 ENDI:1;
        u32 Res3:2;
        u32 F:3;
        u32 O:3;
        u32 BUS:1;
        u32 DAU:1;
        u32 ARS:1;
        u32 SM:1;
        u32 E_D:1;
        u32 M:3;

    } controlr;
    u32 IHR;
    u32 ILR;
    u32 K1HR;
    u32 K1LR;
    u32 K2HR;
    u32 K2LR;
    u32 K3HR;
    u32 K3LR;
    u32 IVHR;
    u32 IVLR;
    u32 OHR;
    u32 OLR;
};

struct aes_t {
    struct aes_controlr {

        u32 KRE:1;
        u32 reserved1:4;
        u32 PNK:1;
        u32 GO:1;
        u32 STP:1;

        u32 reserved2:6;
        u32 NDC:1;
        u32 ENDI:1;
        u32 reserved3:2;

        u32 F:3;    //fbs
        u32 O:3;    //om
        u32 BUS:1;  //bsy
        u32 DAU:1;
        u32 ARS:1;
        u32 SM:1;
        u32 E_D:1;
        u32 KV:1;
        u32 K:2;    //KL

    } controlr;
    u32 ID3R;       //80h
    u32 ID2R;       //84h
    u32 ID1R;       //88h
    u32 ID0R;       //8Ch
    u32 K7R;        //90h
    u32 K6R;        //94h
    u32 K5R;        //98h
    u32 K4R;        //9Ch
    u32 K3R;        //A0h
    u32 K2R;        //A4h
    u32 K1R;        //A8h
    u32 K0R;        //ACh
    u32 IV3R;       //B0h
    u32 IV2R;       //B4h
    u32 IV1R;       //B8h
    u32 IV0R;       //BCh
    u32 OD3R;       //D4h
    u32 OD2R;       //D8h
    u32 OD1R;       //DCh
    u32 OD0R;       //E0h
};

struct deu_hash_t {
    struct hash_controlr {
        u32 reserved1:5;
        u32 KHS:1;
        u32 GO:1;
        u32 INIT:1;
        u32 reserved2:6;
        u32 NDC:1;
        u32 ENDI:1;
        u32 reserved3:7;
        u32 DGRY:1;
        u32 BSY:1;
        u32 reserved4:1;
        u32 IRCL:1;
        u32 SM:1;
        u32 KYUE:1;
                u32 HMEN:1;
        u32 SSEN:1;
        u32 ALGO:1;

    } controlr;
    u32 MR;         //B4h
    u32 D1R;        //B8h
    u32 D2R;        //BCh
    u32 D3R;        //C0h
    u32 D4R;        //C4h
    u32 D5R;        //C8h

    u32 dummy;      //CCh

    u32 KIDX;       //D0h
    u32 KEY;        //D4h
    u32 DBN;        //D8h
};

struct deu_dma_t {
    struct dma_controlr {
        u32 reserved1:22;
        u32 BS:2;
        u32 BSY:1;
        u32 reserved2:1;
        u32 ALGO:2;
        u32 RXCLS:2;
        u32 reserved3:1;
        u32 EN:1;

    } controlr;
};

#endif  /* IFXMIPS_DEU_DANUBE_H */
