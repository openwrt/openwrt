/******************************************************************************
**
** FILE NAME    : dwc_otg_cil_ifx.h
** PROJECT      : Twinpass/Danube
** MODULES      : DWC OTG USB
**
** DATE         : 07 Sep. 2007
** AUTHOR       : Sung Winder
** DESCRIPTION  : Default param value.
** COPYRIGHT    :       Copyright (c) 2007
**                      Infineon Technologies AG
**                      2F, No.2, Li-Hsin Rd., Hsinchu Science Park,
**                      Hsin-chu City, 300 Taiwan.
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date          $Author         $Comment
** 12 April 2007   Sung Winder     Initiate Version
*******************************************************************************/
#if !defined(__DWC_OTG_CIL_IFX_H__)
#define __DWC_OTG_CIL_IFX_H__

/* ================ Default param value ================== */
#define dwc_param_opt_default 1
#define dwc_param_otg_cap_default DWC_OTG_CAP_PARAM_NO_HNP_SRP_CAPABLE
#define dwc_param_dma_enable_default 1
#define dwc_param_dma_burst_size_default 32
#define dwc_param_speed_default DWC_SPEED_PARAM_HIGH
#define dwc_param_host_support_fs_ls_low_power_default 0
#define dwc_param_host_ls_low_power_phy_clk_default DWC_HOST_LS_LOW_POWER_PHY_CLK_PARAM_48MHZ
#define dwc_param_enable_dynamic_fifo_default 1
#define dwc_param_data_fifo_size_default 2048
#define dwc_param_dev_rx_fifo_size_default 1024
#define dwc_param_dev_nperio_tx_fifo_size_default 1024
#define dwc_param_dev_perio_tx_fifo_size_default 768
#define dwc_param_host_rx_fifo_size_default 640
#define dwc_param_host_nperio_tx_fifo_size_default 640
#define dwc_param_host_perio_tx_fifo_size_default 768
#define dwc_param_max_transfer_size_default 65535
#define dwc_param_max_packet_count_default 511
#define dwc_param_host_channels_default 16
#define dwc_param_dev_endpoints_default 6
#define dwc_param_phy_type_default DWC_PHY_TYPE_PARAM_UTMI
#define dwc_param_phy_utmi_width_default 16
#define dwc_param_phy_ulpi_ddr_default 0
#define dwc_param_phy_ulpi_ext_vbus_default DWC_PHY_ULPI_INTERNAL_VBUS
#define dwc_param_i2c_enable_default 0
#define dwc_param_ulpi_fs_ls_default 0
#define dwc_param_ts_dline_default 0

/* ======================================================= */

#endif // __DWC_OTG_CIL_IFX_H__

