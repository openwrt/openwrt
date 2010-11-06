/************************************************************************
 *
 *    Copyright (c) 2005
 *    Infineon Technologies AG
 *    St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 ************************************************************************/
#ifndef _BSP_SUP_H_
#define _BSP_SUP_H_

#define ADD_WAN_MAC
#define CONFIG_IFX_GAN
#define UBOOT_CFG_ENV_SIZE	(0x400-4)
#define ADM8668_BL_MAGIC	0x6c62676d
#define ADM8668_MAC_MAGIC	0x69666164
#define ADM8668_VER_MAGIC	0x7276676d
#define ADM8668_ID_MAGIC	0x6469676d
#define ADM8668_IF_MAGIC	0x6669676d
#define ADM8668_WANMAC_MAGIC	0x69666164
#define ADM8668_IMEI_MAGIC	0x6669676e

#define BSP_IFNAME_MAX_LEN	15
#define BOOT_LINE_SIZE		255
#define BSP_STR_LEN		79


/*
 * Boot mode configuration
 */
typedef struct BTMODE_CFG_S
{
	unsigned long btmode;
	unsigned long dlmethod;
} BTMODE_CFG_T;


/*
 * Interface configuration
 */
typedef struct IF_CFG_S
{
	char ifname[BSP_IFNAME_MAX_LEN+1];
	unsigned long ip;
	unsigned long mask;
	unsigned long gateway;
} IF_CFG_T;


/*
 * Board configuration
 */
typedef struct BOARD_CFG_S
{
	unsigned long blmagic;
	unsigned char blreserved[UBOOT_CFG_ENV_SIZE];

	unsigned long macmagic;
	unsigned char mac[8];
	unsigned long macnum;

	unsigned long idmagic;
	unsigned char serial[BSP_STR_LEN+1];

	unsigned long vermagic;
	unsigned char ver[BSP_STR_LEN+1];

	unsigned long ifmagic;
	IF_CFG_T ifcfg[8];

	unsigned long btmagic;
	BTMODE_CFG_T bootmode;

	unsigned long wanmagic;
    	unsigned char wanmac[8];

	unsigned long imeimagic;
	unsigned char imei0[16];
	unsigned char imei1[16];
} BOARD_CFG_T, *PBOARD_CFG_T;

#define ADM8668_BOARD_CFG_ADDR	(ADM8668_SMEM0_BASE + CONFIG_ADM8668_BSP_OFFSET*1024)
#define ADM8668_BOARD_CFG_SIZE	(CONFIG_ADM8668_BSP_SIZE*1024)

#endif  /* _BSP_SUP_H_   */
