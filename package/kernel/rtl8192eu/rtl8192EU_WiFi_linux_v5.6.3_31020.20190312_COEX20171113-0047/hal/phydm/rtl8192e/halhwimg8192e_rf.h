/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

/*Image2HeaderVersion: R3 1.5.0*/
#if (RTL8192E_SUPPORT == 1)
#ifndef __INC_MP_RF_HW_IMG_8192E_H
#define __INC_MP_RF_HW_IMG_8192E_H

/* Please add following compiler flags definition (#define CONFIG_XXX_DRV_DIS)
 * into driver source code to reduce code size if necessary.
 * #define CONFIG_8192E_AP_DRV_DIS
 * #define CONFIG_8192E_PCIE_DRV_DIS
 * #define CONFIG_8192E_SDIO_DRV_DIS
 * #define CONFIG_8192E_USB_DRV_DIS
 * #define CONFIG_8192E_DRV_DIS
 * #define CONFIG_8192E_8192E_SAR_5MM_DRV_DIS
 */

#define CONFIG_8192E_AP
#ifdef CONFIG_8192E_AP_DRV_DIS
    #undef CONFIG_8192E_AP
#endif

#define CONFIG_8192E_PCIE
#ifdef CONFIG_8192E_PCIE_DRV_DIS
    #undef CONFIG_8192E_PCIE
#endif

#define CONFIG_8192E_SDIO
#ifdef CONFIG_8192E_SDIO_DRV_DIS
    #undef CONFIG_8192E_SDIO
#endif

#define CONFIG_8192E_USB
#ifdef CONFIG_8192E_USB_DRV_DIS
    #undef CONFIG_8192E_USB
#endif

#define CONFIG_8192E
#ifdef CONFIG_8192E_DRV_DIS
    #undef CONFIG_8192E
#endif

#define CONFIG_8192E_8192E_SAR_5MM
#ifdef CONFIG_8192E_8192E_SAR_5MM_DRV_DIS
    #undef CONFIG_8192E_8192E_SAR_5MM
#endif

/******************************************************************************
 *                           radioa.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_radioa(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_radioa(void);

/******************************************************************************
 *                           radiob.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_radiob(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_radiob(void);

/******************************************************************************
 *                           txpowertrack_ap.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_txpowertrack_ap(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_txpowertrack_ap(void);

/******************************************************************************
 *                           txpowertrack_pcie.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_txpowertrack_pcie(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_txpowertrack_pcie(void);

/******************************************************************************
 *                           txpowertrack_sdio.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_txpowertrack_sdio(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_txpowertrack_sdio(void);

/******************************************************************************
 *                           txpowertrack_usb.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_txpowertrack_usb(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_txpowertrack_usb(void);

/******************************************************************************
 *                           txpwr_lmt.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_txpwr_lmt(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_txpwr_lmt(void);

/******************************************************************************
 *                           txpwr_lmt_8192e_sar_5mm.TXT
 ******************************************************************************/

/* tc: Test Chip, mp: mp Chip*/
void
odm_read_and_config_mp_8192e_txpwr_lmt_8192e_sar_5mm(struct dm_struct *dm);
u32 odm_get_version_mp_8192e_txpwr_lmt_8192e_sar_5mm(void);

#endif
#endif /* end of HWIMG_SUPPORT*/

