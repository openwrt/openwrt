/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _HAL_API_TMP_H_
#define _HAL_API_TMP_H_


/**** may be get from hal_com **********************************/

#define H2C_MSR_ROLE_RSVD	0
#define H2C_MSR_ROLE_STA	1
#define H2C_MSR_ROLE_AP	2
#define H2C_MSR_ROLE_GC	3
#define H2C_MSR_ROLE_GO	4
#define H2C_MSR_ROLE_TDLS	5
#define H2C_MSR_ROLE_ADHOC	6
#define H2C_MSR_ROLE_MESH	7
#define H2C_MSR_ROLE_MAX	8

/*************************************************************************************/
typedef enum _HW_VARIABLES {
	HW_VAR_NET_TYPE,
	HW_VAR_SET_OPMODE,
	HW_VAR_MAC_ADDR,
	HW_VAR_BSSID,
	HW_VAR_BASIC_RATE,
	HW_VAR_TXPAUSE,
	HW_VAR_BCN_FUNC,
	HW_VAR_CORRECT_TSF,
	HW_VAR_RCR,
	HW_VAR_MLME_DISCONNECT,
	HW_VAR_MLME_SITESURVEY,
	HW_VAR_MLME_JOIN,
	HW_VAR_ON_RCR_AM,
	HW_VAR_OFF_RCR_AM,
	HW_VAR_BEACON_INTERVAL,
	HW_VAR_SLOT_TIME,
	HW_VAR_RESP_SIFS,
	HW_VAR_ACK_PREAMBLE,
	HW_VAR_SEC_CFG,
	HW_VAR_SEC_DK_CFG,
	HW_VAR_BCN_VALID,
	HW_VAR_FREECNT,
	HW_VAR_STOP_BCN,
	HW_VAR_RESUME_BCN,
	/* PHYDM odm->SupportAbility */
	HW_VAR_CAM_EMPTY_ENTRY,
	HW_VAR_CAM_INVALID_ALL,
	HW_VAR_AC_PARAM_VO,
	HW_VAR_AC_PARAM_VI,
	HW_VAR_AC_PARAM_BE,
	HW_VAR_AC_PARAM_BK,
	HW_VAR_ACM_CTRL,
	HW_VAR_AMPDU_MIN_SPACE,
#ifdef CONFIG_80211N_HT
	HW_VAR_AMPDU_FACTOR,
#endif /* CONFIG_80211N_HT */
	HW_VAR_RXDMA_AGG_PG_TH,
	HW_VAR_SET_RPWM,
	HW_VAR_CPWM,
	HW_VAR_H2C_FW_PWRMODE,
	HW_VAR_H2C_INACTIVE_IPS,
	HW_VAR_H2C_FW_JOINBSSRPT,
	HW_VAR_TRIGGER_GPIO_0,
	HW_VAR_BT_SET_COEXIST,
	HW_VAR_BT_ISSUE_DELBA,
	HW_VAR_FIFO_CLEARN_UP,
	HW_VAR_RESTORE_HW_SEQ,
	HW_VAR_CHECK_TXBUF,
	HW_VAR_PCIE_STOP_TX_DMA,
	HW_VAR_APFM_ON_MAC, /* Auto FSM to Turn On, include clock, isolation, power control for MAC only */
	/* The valid upper nav range for the HW updating, if the true value is larger than the upper range, the HW won't update it. */
	/* Unit in microsecond. 0 means disable this function. */
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
	HW_VAR_WOWLAN,
	HW_VAR_WAKEUP_REASON,
#endif
	HW_VAR_RPWM_TOG,
#ifdef CONFIG_GPIO_WAKEUP
	HW_VAR_WOW_OUTPUT_GPIO,
	HW_VAR_WOW_INPUT_GPIO,
	HW_SET_GPIO_WL_CTRL,
#endif
	HW_VAR_SYS_CLKR,
	HW_VAR_NAV_UPPER,
	HW_VAR_CHK_HI_QUEUE_EMPTY,
	HW_VAR_CHK_MGQ_CPU_EMPTY,
	HW_VAR_DL_BCN_SEL,
	HW_VAR_AMPDU_MAX_TIME,
	HW_VAR_WIRELESS_MODE,
	HW_VAR_USB_MODE,
	HW_VAR_PORT_SWITCH,
	HW_VAR_PORT_CFG,
	HW_VAR_SOUNDING_ENTER,
	HW_VAR_SOUNDING_LEAVE,
	HW_VAR_SOUNDING_RATE,
	HW_VAR_SOUNDING_STATUS,
	HW_VAR_SOUNDING_FW_NDPA,
	HW_VAR_SOUNDING_CLK,
	HW_VAR_SOUNDING_SET_GID_TABLE,
	HW_VAR_SOUNDING_CSI_REPORT,
	HW_VAR_DL_RSVD_PAGE,
	HW_VAR_DUMP_MAC_QUEUE_INFO,
	HW_VAR_ASIX_IOT,
	HW_VAR_CH_SW_NEED_TO_TAKE_CARE_IQK_INFO,
	HW_VAR_CH_SW_IQK_INFO_BACKUP,
	HW_VAR_CH_SW_IQK_INFO_RESTORE,

	HW_VAR_DBI,
	HW_VAR_MDIO,
	HW_VAR_L1OFF_CAPABILITY,
	HW_VAR_L1OFF_NIC_SUPPORT,
#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
	HW_VAR_TDLS_BCN_EARLY_C2H_RPT,
#endif
#endif
	HW_VAR_DUMP_MAC_TXFIFO,
	HW_VAR_PWR_CMD,

	HW_VAR_SET_SOML_PARAM,
	HW_VAR_ENABLE_RX_BAR,
	HW_VAR_TSF_AUTO_SYNC,
	HW_VAR_LPS_STATE_CHK,
	#ifdef CONFIG_RTS_FULL_BW
	HW_VAR_SET_RTS_BW,
	#endif
#if defined(CONFIG_PCI_HCI)
	HW_VAR_ENSWBCN,
#endif
	HW_VAR_ACKTO,
	HW_VAR_ACKTO_CCK,
} HW_VARIABLES;

static inline u8 rtw_hal_set_hwreg(_adapter *padapter, u8 var, u8 *val)
{
	return 0;
}
static inline void rtw_hal_get_hwreg(_adapter *padapter, u8 var, u8 *val)
{}
typedef enum _HAL_DEF_VARIABLE {
	HAL_DEF_IS_SUPPORT_ANT_DIV,
	HAL_DEF_DBG_DUMP_RXPKT,/* for dbg */
	HAL_DEF_BEAMFORMER_CAP,
	HAL_DEF_BEAMFORMEE_CAP,
	HW_VAR_MAX_RX_AMPDU_FACTOR,
	HW_DEF_RA_INFO_DUMP,
	HAL_DEF_DBG_DUMP_TXPKT,
	HAL_DEF_TX_PAGE_SIZE,
	HW_VAR_BEST_AMPDU_DENSITY,
} HAL_DEF_VARIABLE;

static inline u8 rtw_hal_set_def_var(_adapter *padapter, HAL_DEF_VARIABLE def_var, void *val)
{
	return 0;
}

u8 rtw_hal_get_def_var(struct _ADAPTER *a, struct _ADAPTER_LINK *alink,
				enum _HAL_DEF_VARIABLE def_var, void *val);

static inline void rtw_hal_sec_read_cam_ent(_adapter *adapter, u8 id, u8 *ctrl, u8 *mac, u8 *key)
{}
static inline void rtw_hal_sec_write_cam_ent(_adapter *adapter, u8 id, u16 ctrl, u8 *mac, u8 *key)
{}
static inline void rtw_hal_sec_clr_cam_ent(_adapter *adapter, u8 id)
{}
static inline bool rtw_hal_sec_read_cam_is_gk(_adapter *adapter, u8 id)
{
	return _TRUE;
}

static inline void rtw_hal_linked_info_dump(_adapter *padapter, u8 benable)
{}

static inline bool rtw_hal_get_phy_edcca_flag(_adapter *adapter)
{
	return _TRUE;
}

static inline u64 rtw_hal_get_tsftr_by_port(_adapter *adapter, u8 port)
{
	return 1;
}

static inline void rtw_hal_dump_rsvd_page(void *sel, _adapter *adapter, u8 page_offset, u8 page_num)
{}


/*u8 beamforming_get_htndp_tx_rate(void *dm_void, u8 bfer_str_num);*/
static inline u8 rtw_hal_get_htndp_tx_rate(_adapter *adapter, u8 bfer_str_num)
{
	return 0;
}
/*u8 beamforming_get_vht_ndp_tx_rate(void *dm_void, u8 bfer_str_num);*/
static inline u8 rtw_hal_get_vht_ndp_tx_rate(_adapter *adapter, u8 bfer_str_num)
{
	return 0;
}

static inline u8 rtw_hal_get_sounding_info(_adapter *adapter,u16 *throughput,
				       u8 total_bfee_num, u16 *tx_rate)
{
	return 0;
}

static inline void rtw_hal_dump_target_tx_power(void *sel, _adapter *adapter)
{}

static inline void rtw_hal_dump_tx_power_by_rate(void *sel, _adapter *adapter)
{}

static inline void rtw_hal_dump_trx_mode(void *sel, _adapter *adapter)
{}

static inline void rtw_hal_phy_adaptivity_parm_msg(void *sel, _adapter *adapter)
{}

#if defined(CONFIG_RTW_LED) && defined(CONFIG_RTW_SW_LED)
#ifndef CONFIG_RTW_SW_LED_TRX_DA_CLASSIFY
#define CONFIG_RTW_SW_LED_TRX_DA_CLASSIFY 0
#endif

#if CONFIG_RTW_SW_LED_TRX_DA_CLASSIFY
void rtw_sw_led_blink_uc_trx_only(LED_DATA *led);
void rtw_sw_led_ctl_mode_uc_trx_only(_adapter *adapter, LED_CTL_MODE ctl);
#endif
void rtw_led_control(_adapter *adapter, LED_CTL_MODE ctl);
void rtw_led_tx_control(_adapter *adapter, const u8 *da);
void rtw_led_rx_control(_adapter *adapter, const u8 *da);
void rtw_led_set_iface_en(_adapter *adapter, u8 en);
void rtw_led_set_iface_en_mask(_adapter *adapter, u8 mask);
void rtw_led_set_ctl_en_mask(_adapter *adapter, u32 ctl_mask);
void rtw_led_set_ctl_en_mask_primary(_adapter *adapter);
void rtw_led_set_ctl_en_mask_virtual(_adapter *adapter);
#else
#define rtw_led_control(adapter, ctl) do {} while (0)
#define rtw_led_tx_control(adapter, da) do {} while (0)
#define rtw_led_rx_control(adapter, da) do {} while (0)
#define rtw_led_set_iface_en(adapter, en) do {} while (0)
#define rtw_led_set_iface_en_mask(adapter, mask) do {} while (0)
#define rtw_led_set_ctl_en_mask(adapter, ctl_mask) do {} while (0)
#define rtw_led_set_ctl_en_mask_primary(adapter) do {} while (0)
#define rtw_led_set_ctl_en_mask_virtual(adapter) do {} while (0)
#endif /* defined(CONFIG_RTW_LED) && defined(CONFIG_RTW_SW_LED) */

#ifdef CONFIG_PCI_HCI
static inline u8 rtw_hal_pci_dbi_read(_adapter *padapter, u16 addr)
{	return 0;}
static inline u8 rtw_hal_pci_l1off_nic_support(_adapter *padapter)
{	return 0;}

static inline u8 rtw_hal_pci_l1off_capability(_adapter *padapter)
{	return 0;}

static inline void rtw_hal_unmap_beacon_icf(_adapter *padapter)
{
	//hal->hal_ops.unmap_beacon_icf(padapter);
}
#endif

#if defined(CONFIG_PCI_HCI)
static inline u8 rtw_hal_check_nic_enough_desc_all(_adapter *padapter)
{ return _SUCCESS;}
#endif

static inline s32 rtw_hal_macid_sleep(_adapter *adapter, u8 macid)
{	return 0;}
static inline s32 rtw_hal_macid_wakeup(_adapter *adapter, u8 macid)
{	return 0;}
static inline s32 rtw_hal_macid_sleep_all_used(_adapter *adapter)
{	return 0;}
static inline s32 rtw_hal_macid_wakeup_all_used(_adapter *adapter)
{	return 0;}

static void rtw_hal_c2h_pkt_hdl(_adapter *adapter, u8 *buf, u16 len)
{
	//adapter->dvobj->hal_func.hal_mac_c2h_handler(adapter, buf, len);
}
static inline s32 rtw_hal_fill_h2c_cmd(_adapter *padapter, u8 ElementID, u32 CmdLen, u8 *pCmdBuffer)
{
/*
	_adapter *pri_adapter = GET_PRIMARY_ADAPTER(padapter);

	if (GET_PHL_COM(pri_adapter)->fw_ready == _TRUE)
		return hal->hal_ops.fill_h2c_cmd(padapter, ElementID, CmdLen, pCmdBuffer);
	else if (padapter->registrypriv.mp_mode == 0)
		RTW_PRINT(FUNC_ADPT_FMT" FW doesn't exit when no MP mode, by pass H2C id:0x%02x\n"
			  , FUNC_ADPT_ARG(padapter), ElementID);
*/
	return 0;
}

static inline u8 rtw_hal_get_port(_adapter *adapter)
{	return 0;}

static inline void rtw_hal_read_edca(_adapter *adapter, u16 *vo_params, u16 *vi_params,
			u16 *be_params, u16 *bk_params)
{
	//hal->hal_func.read_wmmedca_reg(padapter, vo_params, vi_params, be_params, bk_params);
}

/************************ xmit *******************/
static inline void rtw_hal_bcn_param_setting(_adapter *padapter)
{
	//hal->hal_ops.set_beacon_param_handler(padapter);
}

static inline void rtw_hal_set_tx_power_level(_adapter *adapter, u8 channel)
{}


/****************** GEORGIA_TODO_REDEFINE_IO ************************/
static inline u32 rtw_hal_get_ltsf(_adapter *adapter)/*get tst low 4 bytes */
{
	return 0;
}

static inline u32 rtw_hal_get_dma_statu(_adapter *adapter)
{
	return 0;
}

#ifdef RTW_SUPPORT_PLATFORM_SHUTDOWN
static inline u8 rtw_hal_sdio_leave_suspend(_adapter *adapter)
{
	return 0;
}
#endif

static inline void rtw_hal_get_version(char *str, u32 len)
{
	//get hal version
	//rtw_halmac_get_version(str, 30);
	// get fw version
	// get phy (bb/rf) version
	// get btc version
}


#endif /*_HAL_API_TMP_H_*/
