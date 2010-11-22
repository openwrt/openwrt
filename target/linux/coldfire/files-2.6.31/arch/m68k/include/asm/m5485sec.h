/*
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 */

#ifndef M5485SEC_H
#define M5484SEC_H

#define   SEC_EUACR_U                   MCF_REG32(0x21000)
#define   SEC_EUACR_L                   MCF_REG32(0x21004)
#define   SEC_EUASR_U                   MCF_REG32(0x21028)
#define   SEC_EUASR_L                   MCF_REG32(0x2102C)
#define   SEC_SMCR                      MCF_REG32(0x21030)
#define   SEC_SISR_U                    MCF_REG32(0x21010)
#define   SEC_SISR_L                    MCF_REG32(0x21014)
#define   SEC_SICR_U                    MCF_REG32(0x21018)
#define   SEC_SICR_L                    MCF_REG32(0x2101C)
#define   SEC_SIMR_U                    MCF_REG32(0x21008)
#define   SEC_SIMR_L                    MCF_REG32(0x2100C)
#define   SEC_SID                       MCF_REG32(0x21020)

#define   SEC_SMCR_RESET                0x01000000
#define   SEC_SIMR_MASK_U               0x00000000
#define   SEC_SIMR_MASK_L               0x03333340

#define   SEC_CC0_FR                    MCF_REG32(0x2204C)
#define   SEC_CC0_CR                    MCF_REG32(0x2200C)
#define   SEC_CC0_CDPR                  MCF_REG32(0x22044)
#define   SEC_CC0_PSR_U                 MCF_REG32(0x22010)
#define   SEC_CC0_PSR_L                 MCF_REG32(0x22014)
#define   SEC_CC1_FR                    MCF_REG32(0x2304C)
#define   SEC_CC1_CR                    MCF_REG32(0x2300C)
#define   SEC_CC1_CDPR                  MCF_REG32(0x23044)
#define   SEC_CC1_PSR_U                 MCF_REG32(0x23010)
#define   SEC_CC1_PSR_L                 MCF_REG32(0x23014)

#define   SEC_CC_CR_RESET               0x00000001
#define   SEC_CC_CR_CONFIGURATION       0x0000001E
#define   SEC_CC_PSR_U_ERR_CH0          0x20000000
#define   SEC_CC_PSR_U_ERR_CH1          0x80000000
#define   SEC_CC_PSR_U_DN_CH0           0x10000000
#define   SEC_CC_PSR_U_DN_CH1           0x40000000

#define   SEC_DEU_DRCR                  MCF_REG32(0x2A018)
#define   SEC_DEU_DSR                   MCF_REG32(0x2A028)
#define   SEC_DEU_DISR                  MCF_REG32(0x2A030)
#define   SEC_DEU_DIMR                  MCF_REG32(0x2A038)

#define   SEC_DEU_DRCR_RESET            0x01000000
#define   SEC_DEU_DSR_RD                0x01000000
#define   SEC_DEU_DIMR_MASK             0xF63F0000

#define   SEC_AFEU_AFRCR                MCF_REG32(0x28018)
#define   SEC_AFEU_AFSR                 MCF_REG32(0x28028)
#define   SEC_AFEU_AFISR                MCF_REG32(0x28030)
#define   SEC_AFEU_AFIMR                MCF_REG32(0x28038)

#define   SEC_AFEU_AFRCR_RESET          0x01000000
#define   SEC_AFEU_AFSR_RD              0x01000000
#define   SEC_AFEU_AFIMR_MASK           0xF61F0000


#define   SEC_MDEU_MDRCR                MCF_REG32(0x2C018)
#define   SEC_MDEU_MDSR                 MCF_REG32(0x2C028)
#define   SEC_MDEU_MDISR                MCF_REG32(0x2C030)
#define   SEC_MDEU_MDIMR                MCF_REG32(0x2C038)

#define   SEC_MDEU_MDRCR_RESET          0x01000000
#define   SEC_MDEU_MDSR_RD              0x01000000
#define   SEC_MDEU_MDIMR_MASK           0xC41F0000


#define   SEC_RNG_RNGRCR                MCF_REG32(0x2E018)
#define   SEC_RNG_RNGSR                 MCF_REG32(0x2E028)
#define   SEC_RNG_RNGISR                MCF_REG32(0x2E030)
#define   SEC_RNG_RNGIMR                MCF_REG32(0x2E038)

#define   SEC_RNG_RNGRCR_RESET          0x01000000
#define   SEC_RNG_RNGSR_RD              0x01000000
#define   SEC_RNG_RNGIMR_MASK           0xC2100000

#define   SEC_AESU_AESRCR               MCF_REG32(0x32018)
#define   SEC_AESU_AESSR                MCF_REG32(0x32028)
#define   SEC_AESU_AESISR               MCF_REG32(0x32030)
#define   SEC_AESU_AESIMR               MCF_REG32(0x32038)

#define   SEC_AESU_AESRCR_RESET         0x01000000
#define   SEC_AESU_AESSR_RD             0x01000000
#define   SEC_AESU_AESIMR_MASK          0xF61F0000


#define   SEC_DESC_NUM                  20
#define   SEC_CHANNEL_NUMBER            2
#define   SEC_MAX_BUF_SIZE              32*1024
#define   SEC_INIT_TIMEOUT              1*HZ
#define   SEC_INTERRUPT                 37

/* Header descriptor values*/
#define   SEC_ALG_ENCR_DES_ECB_SINGLE   0x20100010
#define   SEC_ALG_DECR_DES_ECB_SINGLE   0x20000010
#define   SEC_ALG_ENCR_DES_ECB_TRIPLE   0x20300010
#define   SEC_ALG_DECR_DES_ECB_TRIPLE   0x20200010
#define   SEC_ALG_ENCR_DES_CBC_SINGLE   0x20500010
#define   SEC_ALG_DECR_DES_CBC_SINGLE   0x20400010
#define   SEC_ALG_ENCR_DES_CBC_TRIPLE   0x20700010
#define   SEC_ALG_DECR_DES_CBC_TRIPLE   0x20600010

#define   SEC_ALG_MDEU_SHA256           0x30500010
#define   SEC_ALG_MDEU_MD5              0x30600010
#define   SEC_ALG_MDEU_SHA              0x30400010
#define   SEC_ALG_MDEU_SHA256_HMAC      0x31D00010
#define   SEC_ALG_MDEU_MD5_HMAC         0x31E00010
#define   SEC_ALG_MDEU_SHA_HMAC         0x31C00010

#define   SEC_ALG_RNG                   0x40000010


#define   SEC_ALG_AFEU_KEY              0x10200050
#define   SEC_ALG_AFEU_CONTEXT          0x10700050

#define   SEC_ALG_ENCR_AESU_CBC         0x60300010
#define   SEC_ALG_DECR_AESU_CBC         0x60200010
#define   SEC_ALG_ENCR_AESU_ECB         0x60100010
#define   SEC_ALG_DECR_AESU_ECB         0x60000010
#define   SEC_ALG_AESU_CTR              0x60600010



#define   SEC_DESCHEAD_ERROR            0xFE000000
#define   SEC_DESCHEAD_COMPLETED        0xFF000000

#define SEC_DEVICE_NAME                 "cfsec"

/*!!! This number must be changed*/
#define SEC_MAJOR                       130

#define SEC_DEV_BUF                         1024
#define SEC_DEV_KEY_LEN                     64
#define SEC_DEV_VECTOR_LEN                  259

#define SEC_AES_BLCK_LEN                   16
#define SEC_DES_BLCK_LEN                   8


/* Descriptor structure of SEC*/
struct sec_descriptor {
	volatile unsigned long secdesc_header;
	unsigned long secdesc_len1;
	void *secdesc_ptr1;
	unsigned long secdesc_iv_in_len;
	void *secdesc_iv_in_ptr;
	unsigned long secdesc_key_len;
	void *secdesc_key_ptr;
	unsigned long secdesc_data_in_len;
	void *secdesc_data_in_ptr;
	unsigned long secdesc_data_out_len;
	void *secdesc_data_out_ptr;
	unsigned long secdesc_iv_out_len;
	void *secdesc_iv_out_ptr;
	unsigned long secdesc_len7;
	void *secdesc_ptr7;
	void *secdesc_ptrnext;
};

struct sec_device_data {
	unsigned char secdev_inbuf[SEC_DEV_BUF];
	unsigned char secdev_outbuf[SEC_DEV_BUF];
	unsigned char secdev_key[SEC_DEV_KEY_LEN];
	unsigned char secdev_iv[SEC_DEV_VECTOR_LEN];
	unsigned char secdev_ov[SEC_DEV_VECTOR_LEN];
	struct sec_descriptor *secdev_desc;
};

struct sec_descriptor *sec_desc_alloc(void);
inline void sec_desc_free(struct sec_descriptor *desc);
int sec_execute(int channel, struct sec_descriptor *desc, int timeout);
int sec_nonblock_execute(struct sec_descriptor *desc);
#endif
