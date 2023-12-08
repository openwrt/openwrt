/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_AX_ERR_H_
#define _MAC_AX_ERR_H_

#define MACSUCCESS	0  /* Success return value */
#define MACPFCB		1  /* Callback of platform is null */
#define MACPFED		2  /* Endian of platform error */
#define MACBADDR	3  /* Invalid base address */
#define MACLSUS		4  /* Leave suspend error */
#define MACNPTR		5  /* Pointer is null */
#define MACCHIPID	6  /* Chip ID is undefined */
#define MACADAPTER	7  /* Can not get MAC adapter */
#define MACSTCAL	8  /* Unexpected structure alignment */
#define MACNOBUF	9  /* Buffer space is not enough */
#define MACBUFSZ	10 /* Buffer size error */
#define MACNOITEM	11 /* Invalid item */
#define MACPOLLTO	12 /* Polling timeout */
#define MACPWRSW	13 /* Power switch fail */
#define MACBUFALLOC	14 /* Buffer allocation fail */
#define MACWQBUSY	15 /* Work queue is busy */
#define MACCMP		16 /* Failed compare result */
#define MACINTF		17 /* Wrong interface */
#define MACFWBIN	18 /* Incorrect FW bin file */
#define MACFFCFG	19 /* Wrong FIFO configuration */
#define MACSAMACID	20 /* Same MACID */
#define MACMACIDFL	21 /* MACID full */
#define MACNOFW		22 /* There is no FW */
#define MACPROCBUSY	23 /* Process is busy */
#define MACPROCERR	24 /* state machine error */
#define MACEFUSEBANK	25 /* switch efuse bank fail */
#define MACEFUSEREAD	26 /* read efuse fail */
#define MACEFUSEWRITE	27 /* write efuse fail */
#define MACEFUSESIZE	28 /* efuse size error */
#define MACEFUSEPARSE	29 /* eeprom parsing fail */
#define MACEFUSECMP	30 /* compare efuse fail */
#define MACSECUREON	31 /* secure on, no host indirect access */
#define MACTXCHDMA	32 /* invalid tx dma channel */
#define MACADDRCAMUPDERR 33 /* address cam update error */
#define MACPWRSTAT	34 /* Power state error */
#define MACSDIOMIXMODE	35 /* SDIO Tx mix mode */
#define MACSDIOSEQERR	36 /* SDIO Tx sequence error */
#define MACHFCH2CQTA	37 /* HCI FC invalid H2C quota */
#define MACHFCCH011QTA	38 /* HCI FC invalid CH0-11 quota */
#define MACHFCCH011GRP	39 /* HCI FC invalid CH0-11 group */
#define MACHFCPUBQTA	40 /* HCI FC invalid public quota */
#define MACHFCPUBINFO	41 /* HCI FC public info error */
#define MACRFPMCAM	42 /* RX forwarding PM CAM access fail */
#define MACHFSWDENOTNUF	43 /* HCI FC WDE page not enough */
#define MACHFSPLENOTNUF	44 /* HCI FC PLE page not enough */
#define MACMEMRO	45 /* Address is not writable */
#define MACFUNCINPUT	46 /* invalid function input */
#define MACALRDYON	47 /* MAC has already powered on */
#define MACADDRCAMFL	48 /* ADDRESS CAM full */
#define MACBSSIDCAMFL	49 /* BSSID CAM full */
#define MACGPIOUSED	50 /* GPIO is used */
#define MACDLELINK	51 /* DLE link error */
#define MACPOLLTXIDLE	52 /* polling Tx idle fail */
#define MACPARSEERR	53 /* parse report err */
#define MACROLEINITFL	54 /* Role API init fail or C2H notify role init fail */
#define MACPORTCFGTYPE	55 /* Port cfg type error */
#define MACPORTCFGPORT	56 /* Port cfg port error */
#define MACWNGKEYTYPE	57 /* Sec cam wrong key type*/
#define MACKEYNOTEXT	58 /* Delete key , key not exist*/
#define MACSECCAMFL	59 /* SEC CAM full*/
#define MACADDRCAMKEYFL	60 /* Addr CAM key full*/
#define MACNOROLE	61 /* SEC no this role*/
#define MACHWNOTEN	62 /* hw module not enable*/
#define MACPTMTXFAIL	63 /* platform TX fail*/
#define MACSSLINK	64 /* STA scheduler link error */
#define MACDBGPORTSEL	65 /* Debug port sel error */
#define MACDBGPORTDMP	66 /* Debug port dump error */
#define MACCPWMSEQERR	67 /* CPWM sequence mismatch */
#define MACCPWMSTATERR	68 /* CPWM state mismatch */
#define MACCPUSTATE	69 /* Incorrect CPU state */
#define MACPSSTATFAIL	70 /* protocol power state check tx pause fail */
#define MACLV1STEPERR	71 /* lv1 rcvy step sel error */
#define MACFWCHKSUM	72 /* FW checksum is incorrect */
#define MACFWSECBOOT	73 /* FW security boot is failed */
#define MACFWCUT	74 /* Mismatch chip and FW cut */
#define MACSUBSPCERR	75 /* Beacon sub-space setting fail */
#define MACLENCMP	76 /* Length is not match */
#define MACCHKSUMEMPTY	77 /* Checksum report empty  */
#define MACCHKSUMFAIL	78 /* Checksum report fail  */
#define MACVERERR	79 /* Map and mask version mismatch */
#define MACFWNONRDY	80 /* FW not ready h2c error*/
#define MACGPIONUM	81 /* The gpio number is wrong */
#define MACNOTSUP	82 /* The function is NOT supported */
#define MACCSIBUFIDERR	83 /* CSI buffer index is NOT supported */
#define MACSNDSTSIDERR	84 /* Sounding status ID is NOT supported */
#define MACCCTLWRFAIL	85 /* control info wrrite fail */
#define MACHWNOSUP	86 /* HW not support */
#define MACUNDEFCH	87 /* Channel is undefined */
#define MACHWERR	88 /* HW error */
#define MACFWTESTFAIL	89 /* FW auto test fail */
#define MACP2PSTFAIL	90 /* P2P state fail */
#define MACFLASHFAIL	91 /* FW auto test fail */
#define MACSETVALERR	92 /* Setting value error */
#define MACIOERRPWR	93 /* IO not allow when power not on */
#define MACIOERRSERL1	94 /* IO not allow when SER Lv1 */
#define MACIOERRLPS	95 /* IO not allow when LPS */
#define MACIOERRDMAC	96 /* IO not allow when dmac not en */
#define MACIOERRCMAC0	97 /* IO not allow when cmac0 not en */
#define MACIOERRCMAC1	98 /* IO not allow when cmac1 not en */
#define MACIOERRBB0	99 /* IO not allow when bb0 not en */
#define MACIOERRBB1	100 /* IO not allow when bb1 not en */
#define MACIOERRRF	101 /* IO not allow when rf not en */
#define MACIOERRIND	102 /* IO not allow when indirect access */
#define MACIOERRRSVD	103 /* IO not allow if address is rsvd */
#define MACC2HREGEMP	104 /* C2H reg empty */
#define MACBADC2HREG	105 /* received unexpected c2hreg */
#define MACFIOOFLD	106 /* IO offload fail */
#define MACROLEALOCFL	107 /* C2H notify alloc role failed */
#define MACROLEHWUPDFL	108 /* C2H notify addrcam upd failed*/
#define MACSDIOTXMODE	109 /* SDIO Tx mode undefined*/
#define MACSDIOOPNMODE	110 /* SDIO opn mode unknown*/
#define MACFWSTATUSFAIL 111 /* fw status command fail */
#define MACIOERRPLAT	112 /* IO not allow when platform not on */
#define MACCPWMPWRSTATERR	113 /* CPWM power state mismatch */
#define MACIOERRISH		114 /* IO not allow when io state hang */
#define MACHWDMACERR	115 /* DMAC_ERR_ISR */
#define MACHWCMAC0ERR	116 /* CMAC0_ERR_ISR */
#define MACDRVRM	117 /* driver is removed unexpectedly */
#define MACMCCGPFL	118 /* Get MCC Group index fail*/
#define MACFWSTATEERR	119 /* fw state error */
#define MACFWLOGINTERR 120 /*fw log parsing error*/
#define MACSYSSTEERR	121 /* Whole System Power State error */
#define MACHWCMAC1ERR	122 /* CMAC1_ERR_ISR */
#define MACFWASSERT	123 /* FW Assertion error */
#define MACFWEXCEP	124 /* FW Exception error */
#define MACFWRXI300	125 /* FW RXI300 error */
#define MACFWPCHANG	126 /* FW PC hang error */
#define MACRXDMAHANG	127 /*USB RXDMA HANG */
#define MACUSBRXHANG	128 /*USB RX HANG */
#define MACCPWMINTFERR	129 /* CPWM interface error */
#define MACUSBPAUSEERR	130 /* USB EP PAUSE error */
#define MACARDYDONE	131 /* The flow is already done */
#define MACPSSTATPWRBITFAIL	132 /* protocol power state check pwr bit fail */
#define MACIOERRINSEC	133 /* Security ic not allow indirect access */

/*MAC DBG Status Indication*/
#define MACSCH_NONEMPTY	1 /* MAC Scheduler non empty */

/* Debug Package Indication */
/* STA Scheduler 0, indirect */
#define SS_TX_LEN_BE		BIT(0)
#define SS_TX_LEN_BK		BIT(1)
#define SS_TX_LEN_VI		BIT(2)
#define SS_TX_LEN_VO		BIT(3)
#define SS_LINK_WMM0_BE		BIT(4)
#define SS_LINK_WMM0_BK		(SS_LINK_WMM0_BE << 1)
#define SS_LINK_WMM0_VI		(SS_LINK_WMM0_BE << 2)
#define SS_LINK_WMM0_VO		(SS_LINK_WMM0_BE << 3)
#define SS_LINK_WMM1_BE		(SS_LINK_WMM0_BE << 4)
#define SS_LINK_WMM1_BK		(SS_LINK_WMM0_BE << 5)
#define SS_LINK_WMM1_VI		(SS_LINK_WMM0_BE << 6)
#define SS_LINK_WMM1_VO		(SS_LINK_WMM0_BE << 7)
#define SS_LINK_WMM2_BE		(SS_LINK_WMM0_BE << 8)
#define SS_LINK_WMM2_BK		(SS_LINK_WMM0_BE << 9)
#define SS_LINK_WMM2_VI		(SS_LINK_WMM0_BE << 10)
#define SS_LINK_WMM2_VO		(SS_LINK_WMM0_BE << 11)
#define SS_LINK_WMM3_BE		(SS_LINK_WMM0_BE << 12)
#define SS_LINK_WMM3_BK		(SS_LINK_WMM0_BE << 13)
#define SS_LINK_WMM3_VI		(SS_LINK_WMM0_BE << 14)
#define SS_LINK_WMM3_VO		(SS_LINK_WMM0_BE << 15)
#define SS_LINK_UL		(SS_LINK_WMM0_BE << 16)
#define SS_POLL_OWN_TX_LEN	BIT(24)
#define SS_POLL_OWN_LINK	BIT(25)
#define SS_POLL_STAT_TX_LEN	BIT(26)
#define SS_POLL_STAT_LINK	BIT(27)
/* STA Scheduler 1, direct */
#define SS_TX_HW_LEN_UDN	BIT(0)
#define SS_TX_SW_LEN_UDN	BIT(1)
#define SS_TX_HW_LEN_OVF	BIT(2)
#define SS_STAT_FWTX		BIT(8)
#define SS_STAT_RPTA		BIT(9)
#define SS_STAT_WDEA		BIT(10)
#define SS_STAT_PLEA		BIT(11)
#define SS_STAT_ULRU		BIT(12)
#define SS_STAT_DLTX		BIT(13)

#ifdef CONFIG_NEW_HALMAC_INTERFACE
#define PLTFM_MSG_ALWAYS(...)                                                  \
	_os_dbgdump("[MAC][ERR] " fmt, ##__VA_ARGS__)
#else
#define PLTFM_MSG_ALWAYS(...)                                                  \
	adapter->pltfm_cb->msg_print(adapter->drv_adapter, _PHL_ALWAYS_,  __VA_ARGS__)
#endif

#if MAC_AX_DBG_MSG_EN

#ifdef CONFIG_NEW_HALMAC_INTERFACE

	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_ALWAYS)
	#define PLTFM_MSG_ALWAYS(...)                                         \
		_os_dbgdump("[MAC][LOG] " fmt, ##__VA_ARGS__)
	#else
	#define PLTFM_MSG_ALWAYS(...)	do {} while (0)
	#endif

	/* Enable debug msg depends on  HALMAC_MSG_LEVEL */
	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_ERR)
	#define PLTFM_MSG_ERR(...)                                           \
		_os_dbgdump("[MAC][ERR] " fmt, ##__VA_ARGS__)
	#else
	#define PLTFM_MSG_ERR(...)	do {} while (0)
	#endif

	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_WARNING)
	#define PLTFM_MSG_WARN(...)                                          \
		_os_dbgdump("[MAC][WARN] " fmt, ##__VA_ARGS__)
	#else
	#define PLTFM_MSG_WARN(...)	do {} while (0)
	#endif

	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_TRACE)
	#define PLTFM_MSG_TRACE(...)                                         \
		_os_dbgdump("[MAC][TRACE] " fmt, ##__VA_ARGS__)
	#else
	#define PLTFM_MSG_TRACE(...)	do {} while (0)
	#endif

#else

	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_ALWAYS)
	#define PLTFM_MSG_ALWAYS(...)                                         \
		adapter->pltfm_cb->msg_print(adapter->drv_adapter, _PHL_ALWAYS_, __VA_ARGS__)
	#else
	#define PLTFM_MSG_ALWAYS(...)	do {} while (0)
	#endif

	/* Enable debug msg depends on  HALMAC_MSG_LEVEL */
	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_ERR)
	#define PLTFM_MSG_ERR(...)                                           \
		adapter->pltfm_cb->msg_print(adapter->drv_adapter, _PHL_ERR_, __VA_ARGS__)
	#else
	#define PLTFM_MSG_ERR(...)	do {} while (0)
	#endif

	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_WARNING)
	#define PLTFM_MSG_WARN(...)                                          \
		adapter->pltfm_cb->msg_print(adapter->drv_adapter, _PHL_WARNING_, __VA_ARGS__)
	#else
	#define PLTFM_MSG_WARN(...)	do {} while (0)
	#endif

	#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_TRACE)
	#define PLTFM_MSG_TRACE(...)                                         \
		adapter->pltfm_cb->msg_print(adapter->drv_adapter, _PHL_DEBUG_, __VA_ARGS__)
	#else
	#define PLTFM_MSG_TRACE(...)	do {} while (0)
	#endif
#endif /*CONFIG_NEW_HALMAC_INTERFACE*/

#else

/* Disable debug msg  */
#define PLTFM_MSG_ALWAYS(...)	do {} while (0)
#define PLTFM_MSG_ERR(...)	do {} while (0)
#define PLTFM_MSG_WARN(...)	do {} while (0)
#define PLTFM_MSG_TRACE(...)	do {} while (0)

#endif

#endif
