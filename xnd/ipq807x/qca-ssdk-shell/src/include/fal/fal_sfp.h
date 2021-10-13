/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup fal_sfp FAL_SFP
 * @{
 */
#ifndef _FAL_SFP_H_
#define _FAL_SFP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "sw.h"
#include "fal/fal_type.h"

typedef enum {
	FAL_SFP_CC_BASE = 0,  /* Check code for Base ID Fields (addresses 0 to 62) */
	FAL_SFP_CC_EXT,       /* Check code for the Extended ID Fields (addresses 64 to 94) */
	FAL_SFP_CC_DMI,       /* Check code for Base Diagnostic Fields (addresses 0 to 94) */
} fal_sfp_cc_type_t;

typedef struct {
	a_uint32_t addr;       /* Addr A0h or A2h */
	a_uint32_t offset;     /* offset in eeprom */
	a_uint32_t count;      /* data bytes */
	a_uint8_t data[256];   /* data value */
} fal_sfp_data_t;

/* A0h offset: 0-2 bytes */
typedef struct {
	a_uint8_t identifier;      /* type of transceiver */
	/*
	 * 00h: Unknown or unspecified
	 * 01h: GBIC
	 * 02h: Module soldered to motherboard (ex: SFF)
	 * 03h: SFP or SFP “Plus”
	 * 04h: Reserved for “300 pin XBI” devices*
	 * 05h: Reserved for “Xenpak” devices*
	 * 06h: Reserved for “XFP” devices*
	 * 07h: Reserved for “XFF” devices*
	 * 08h: Reserved for “XFP-E” devices*
	 * 09h: Reserved for “XPak” devices*
	 * 0Ah: Reserved for “X2” devices*
	 * 0Bh: Reserved for “DWDM-SFP” devices*
	 * 0Ch: Reserved for “QSFP” devices*
	 * 0D-7Fh: Reserved, unallocated
	 * 80-FFh: Vendor specific
	 */
	a_uint8_t ext_indentifier; /* Extended identifier of type of transceiver */
	/*
	 * 00h: GBIC definition is not specified or not compliant with a defined MOD_DEF.
	 * 01h: GBIC is compliant with MOD_DEF 1
	 * 02h: GBIC is compliant with MOD_DEF 2
	 * 03h: GBIC is compliant with MOD_DEF 3
	 * 04h: GBIC/SFP function is defined by two-wire interface ID only
	 * 05h: GBIC is compliant with MOD_DEF 5
	 * 06h: GBIC is compliant with MOD_DEF 6
	 * 07h: GBIC is compliant with MOD_DEF 7
	 * 08-FFh: Unallocated
	 */
	a_uint8_t connector_type;  /* Code for connector type */
	/*
	 * 00h: Unknown or unspecified
	 * 01h: SC
	 * 02h: Fibre Channel Style 1 copper connector
	 * 03h: Fibre Channel Style 2 copper connector
	 * 04h: BNC/TNC
	 * 05h: Fibre Channel coaxial headers
	 * 06h: FiberJack
	 * 07h: LC
	 * 08h: MT-RJ
	 * 09h: MU
	 * 0AH: SG
	 * 0Bh: Optical pigtail
	 * 0Ch: MPO Parallel Optic
	 * 0D-1Fh: Unallocated
	 * 20h: HSSDC II
	 * 21h: Copper pigtail
	 * 22h: RJ45
	 * 23h-7Fh: Unallocated
	 * 80h-FFh: Vendor specific
	 */
} fal_sfp_dev_type_t;

/* A0h offset: 3-10 bytes */
typedef struct {
	a_uint8_t eth_10g_ccode;           /* 10G Ethernet Compliance Codes */
	/*
	 * offset 3 bit7: 10G Base-ER
	 * offset 3 bit6: 10G Base-LRM
	 * offset 3 bit5: 10G Base-LR
	 * offset 3 bit4: 10G Base-SR
	 */
	a_uint8_t infiniband_ccode;        /* Infiniband Compliance Codes */
	/*
	 * offset 3 bit3: 1X SX
	 * offset 3 bit2: 1X LX
	 * offset 3 bit1: 1X Copper Active
	 * offset 3 bit0: 1X Copper Passive
	 */
	a_uint8_t escon_ccode;             /* ESCON Compliance Codes */
	/*
	 * offset 4 bit7: ESCON MMF, 1310nm LED
	 * offset 4 bit6: ESCON SMF, 1310nm Laser
	 */
	a_uint16_t sonet_ccode;            /* SONET Compliance Codes */
	/*
	 * offset 4 bit5: OC-192, short reach
	 * offset 4 bit4: SONET reach specifier bit 1
	 * offset 4 bit3: SONET reach specifier bit 2
	 * offset 4 bit2: OC-48, long reach
	 * offset 4 bit1: OC-48, intermediate reach
	 * offset 4 bit0: OC-48, short reach
	 * offset 5 bit7: Unallocated
	 * offset 5 bit6: OC-12, single mode, long reach
	 * offset 5 bit5: OC-12, single mode, inter. reach
	 * offset 5 bit4: OC-12, short reach
	 * offset 5 bit3: Unallocated
	 * offset 5 bit2: OC-3, single mode, long reach
	 * offset 5 bit1: OC-3, single mode, inter. reach
	 * offset 5 bit0: OC-3, short reach
	 */
	a_uint8_t eth_ccode;               /* Ethernet Compliance Codes */
	/*
	 * offset 6 bit7: BASE-PX
	 * offset 6 bit6: BASE-BX10
	 * offset 6 bit5: 100BASE-FX
	 * offset 6 bit4: 100BASE-LX/LX10
	 * offset 6 bit3: 1000BASE-T
	 * offset 6 bit2: 1000BASE-CX
	 * offset 6 bit1: 1000BASE-LX
	 * offset 6 bit0: 1000BASE-SX
	 */
	a_uint8_t fibre_chan_link_length;  /* Fibre Channel Link Length */
	/*
	 * offset 7 bit7: very long distance (V)
	 * offset 7 bit6: short distance (S)
	 * offset 7 bit5: intermediate distance (I)
	 * offset 7 bit4: long distance (L)
	 * offset 7 bit3: medium distance (M)
	 */
	a_uint8_t fibre_chan_tech;         /* Fibre Channel Technology */
	/*
	 * offset 7 bit2: Shortwave laser, linear Rx (SA)
	 * offset 7 bit1: Longwave laser (LC)
	 * offset 7 bit0: Electrical inter-enclosure (EL)
	 * offset 8 bit7: Electrical intra-enclosure (EL)
	 * offset 8 bit6: Shortwave laser w/o OFC (SN)
	 * offset 8 bit5: Shortwave laser with OFC4 (SL)
	 * offset 8 bit4: Longwave laser (LL)
	 */
	a_uint8_t sfp_cable_tech;          /* SFP+ Cable Technology */
	/*
	 * offset 8 bit3: Active Cable
	 * offset 8 bit2: passive Cable
	 * offset 8 bit1: unallocated
	 * offset 8 bit0: unallocated
	 */
	a_uint8_t fibre_chan_trans_md;     /* Fibre Channel Transmission Media */
	/*
	 * offset 9 bit7: twin axial pair(TW)
	 * offset 9 bit6: twisted pair(TP)
	 * offset 9 bit5: miniature coax(MI)
	 * offset 9 bit4: video coax(TV)
	 * offset 9 bit3: multimode, 62.5um(M6)
	 * offset 9 bit2: multimode, 50um(M5,M5E)
	 * offset 9 bit1: unallocated
	 * offset 9 bit0: sigle mode(SM)
	 */
	a_uint8_t fibre_chan_speed;        /* Fibre Channel Speed */
	/*
	 * offset 10 bit7: 1200 MBytes/sec
	 * offset 10 bit6: 800 MBytes/sec
	 * offset 10 bit5: 1600 MBytes/sec
	 * offset 10 bit4: 400 MBytes/sec
	 * offset 10 bit3: unallocated
	 * offset 10 bit2: 200 MBytes/sec
	 * offset 10 bit1: unallocated
	 * offset 10 bit0: 100 MBytes/sec
	 */
} fal_sfp_transc_code_t;

/* A0h offset: 11-13 bytes */
typedef struct {
	a_uint8_t encode;              /* the serial encoding mechanism */
	/*
	 * 0h: Unspecified
	 * 1h: 8B/10B
	 * 2h: 4B/5B
	 * 3h: NRZ
	 * 4h: Manchester
	 * 5h: SONET Scrambled
	 * 6h： 64B/66B
	 * 7h-FFh: Unallocated
	 */
	a_uint8_t nominal_bit_rate;    /* the nomial bit rate */
	/*
	 * these bits necessary to encode and delimit the signal
	 * as well as those bits carrying data information,
	 * the actual transfer rate will depend on the encoding
	 * of the data definded by the encode value above.
	 */
	a_uint8_t rate_id;             /* the selected rate */
	/*
	 * 0h: Unspecified
	 * 1h: Defined for SFF-8079 (4/2/1G Rate_Select & AS0/AS1)
	 * 2h: Defined for SFF-8431 (8/4/2G Rx Rate_Select only)
	 * 3h: Unspecified *
	 * 4h: Defined for SFF-8431 (8/4/2G Tx Rate_Select only)
	 * 5h: Unspecified *
	 * 6h: Defined for SFF-8431 (8/4/2G Independent Rx & Tx Rate_select)
	 * 7h-FFh: Unallocated
	 */
} fal_sfp_rate_encode_t;


/* A0h offset: 14-19 bytes */
typedef struct {
	a_uint8_t single_mode_length_km;     /* the link length of km in single mode */
	a_uint8_t single_mode_length_100m;   /* the link length of 100m in single mode */
	a_uint8_t om2_mode_length_10m;       /* the link length of 10m micron multimode OM2 */
	a_uint8_t om1_mode_length_10m;       /* the link length of 10m micron multimode OM1 */
	a_uint8_t copper_mode_length_1m;     /* the link length of 1m copper mode */
	a_uint8_t om3_mode_length_1m;        /* the link length of 10m micron multimode OM3 */
} fal_sfp_link_length_t;

/* A0h offset: 20-35 bytes vendor name*/
/* A0h offset: 36th byte transceiver code */
/* A0h offset: 37-39 bytes vendor oui */
/* A0h offset: 40-55 bytes vendor pn */
/* A0h offset: 56-59 bytes vendor rev */
/* A0h offset: 68-83 bytes for the vendor serial number */
/* A0h offset: 84-91 bytes for the vendor's date code in ascii charaters */
typedef struct {
	a_uint8_t vendor_name[16];     /* vendor name */
	a_uint8_t vendor_oui[3];       /* vendor OUI */
	a_uint8_t vendor_pn[16];       /* vendor PN */
	a_uint8_t vendor_rev[4];       /* vendor Rev */
	a_uint8_t vendor_sn[16];       /* the vendor serial number */
	a_uint8_t vendor_date_code[8]; /* the vendor's date code in ascii charaters */
} fal_sfp_vendor_info_t;

/* A0h offset: 60-61 bytes */
typedef struct {
	a_uint16_t laser_wavelength;   /* the output laser wave length in nm */
} fal_sfp_laser_wavelength_t;

/* A0h offset: 62 byte unallocated */
/* A0h offset: 63 byte for checksum  base */

/* A0h offset: 64-65 bytes */
typedef struct {
	/* the implemented options */
	a_uint8_t linear_recv_output:1;
	a_uint8_t pwr_level_declar:1;
	a_uint8_t cool_transc_declar:1;
	a_uint8_t reversed_1:5;

	a_uint8_t reversed_2:1;
	a_uint8_t loss_signal:1;
	a_uint8_t loss_invert_signal:1;
	a_uint8_t tx_fault_signal:1;
	a_uint8_t tx_disable:1;
	a_uint8_t rate_sel:1;
	a_uint8_t reversed_3:2;
	/*
	 * offset 64 bit7-3: Unallocated
	 * offset 64 bit2: Cooled Transceiver Declaration (see SFF-8431)
	 * offset 64 bit1: Power Level Declaration (see SFF-8431)
	 * offset 64 bit0: Linear Receiver Output Implemented (see SFF-8431)
	 * offset 65 bit7-6: Unallocated
	 * offset 65 bit5: RATE_SELECT functionality is implemented
	 * offset 65 bit4: TX_DISABLE is implemented and disables the high speed serial output
	 * offset 65 bit3: TX_FAULT signal implemented. (See SFP MSA)
	 * offset 65 bit2: Loss of Signal implemented, signal inverted from standard definition
	 * offset 65 bit1: Loss of Signal implemented, signal as defined in SFP MSA
	 * offset 65 bit0: Unallocated
	 */
} fal_sfp_option_t;

/* A0h offset: 66-67 bytes */
typedef struct {
	a_uint8_t upper_rate_limit;    /* the upper rate limit */
	a_uint8_t lower_rate_limit;    /* the lower rate limit */
} fal_sfp_rate_t;

/* A0h offset: 68-83 bytes for the vendor serial number */
/* A0h offset: 84-91 bytes for the vendor's date code in ascii charaters */

/* A0h offset: 92-94 bytes */
typedef struct {
	/* digital diagnostic monitoring type */
	a_uint8_t reserved_type:2;
	a_uint8_t addr_mode:1;
	a_uint8_t rec_pwr_type:1;
	a_uint8_t external_cal:1;
	a_uint8_t internal_cal:1;
	a_uint8_t diag_mon_flag:1;
	a_uint8_t legacy_type:1;
	/*
	 * bit 7: Reserved for legacy diagnostic
	 * bit 6: Digital diagnostic monitoring implemented
	 * bit 5: Internally calibrated
	 * bit 4: Externally calibrated
	 * bit 3: Received power measurement type
	 * 0 = OMA, 1 = average power
	 * bit 2: Address change required “addressing modes”
	 * bit1~0: unallocated
	 */
	/* optional digital diagnostic features implemented */
	a_uint8_t reserved_op:1;
	a_uint8_t soft_rate_sel_op:1;
	a_uint8_t app_sel_op:1;
	a_uint8_t soft_rate_ctrl_op:1;
	a_uint8_t rx_los_op:1;
	a_uint8_t tx_fault_op:1;
	a_uint8_t tx_disable_ctrl_op:1;
	a_uint8_t alarm_warning_flag_op:1;
	/*
	 * bit 7: Optional Alarm/warning flags implemented for all monitored quantities
	 * bit 6: Optional soft TX_DISABLE control and monitoring implemented
	 * bit 5: Optional soft TX_FAULT monitoring implemented
	 * bit 4: Optional soft RX_LOS monitoring implemented
	 * bit 3: Optional soft RATE_SELECT control and monitoring implemented
	 * bit 2: Optional Application Select control implemented per SFF-8079
	 * bit 1: Optional soft Rate Select control implemented per SFF-8431
	 * bit 0: Unallocated
	 */
	a_uint8_t compliance_feature; /* the implemented features */
	/*
	 * 00h: Digital diagnostic functionality not included or undefined
	 * 01h: Includes functionality described in Rev 9.3 of SFF-8472.
	 * 02h: Includes functionality described in Rev 9.5 of SFF-8472.
	 * 03h: Includes functionality described in Rev 10.2 of SFF-8472.
	 * 04h: Includes functionality described in Rev 10.4 of SFF-8472.
	 * 05h~FFh: Unallocated
	 */
} fal_sfp_enhanced_cfg_t;

/* A0h offset: 95th byte for checksum extended id field */

/* A0h offset: 96-127 bytes Vendor Specific */
/* A0h offset: 128-255 bytes Reserved */

/* A2h offset: 0-39 bytes */
typedef struct {
	a_uint16_t temp_high_alarm;      /* the temperature alarm high value */
	a_uint16_t temp_low_alarm;       /* the temperature alarm low value */
	a_uint16_t temp_high_warning;    /* the temperature warning high value */
	a_uint16_t temp_low_warning;     /* the temperature warning low value */
	a_uint16_t vol_high_alarm;       /* the voltage alarm high value */
	a_uint16_t vol_low_alarm;        /* the voltage alarm low value */
	a_uint16_t vol_high_warning;     /* the voltage warning high value */
	a_uint16_t vol_low_warning;      /* the voltage warning low value */
	a_uint16_t bias_high_alarm;      /* the bias alarm high value */
	a_uint16_t bias_low_alarm;       /* the bias alarm low value */
	a_uint16_t bias_high_warning;    /* the bias warning high value */
	a_uint16_t bias_low_warning;     /* the bias warning low value */
	a_uint16_t tx_power_high_alarm;  /* the tx power alarm high value */
	a_uint16_t tx_power_low_alarm;   /* the tx power alarm low value */
	a_uint16_t tx_power_high_warning;/* the tx power warning high value */
	a_uint16_t tx_power_low_warning; /* the tx power warning low value */
	a_uint16_t rx_power_high_alarm;  /* the rx power alarm high value */
	a_uint16_t rx_power_low_alarm;   /* the rx power alarm low value */
	a_uint16_t rx_power_high_warning;/* the rx power warning high value */
	a_uint16_t rx_power_low_warning; /* the rx power warning low value */
} fal_sfp_internal_threshold_t;

/* A2h offset: 40-55 bytes Reserved for future monitored quantities */

/* A2h offset: 56-91 bytes */
typedef struct {
	a_uint32_t rx_power4;      /* Single precision floating point Rx optical power4 */
	a_uint32_t rx_power3;      /* Single precision floating point Rx optical power3 */
	a_uint32_t rx_power2;      /* Single precision floating point Rx optical power2 */
	a_uint32_t rx_power1;      /* Single precision floating point Rx optical power1 */
	a_uint32_t rx_power0;      /* Single precision floating point Rx optical power0 */
	a_uint16_t tx_bias_slope;  /* Fixed decimal for laser bias current */
	a_uint16_t tx_bias_offset; /* Fixed decimal for laser bias current */
	a_uint16_t tx_power_slope; /* Fixed decimal for transmitter coupled output power */
	a_uint16_t tx_power_offset;/* Fixed decimal for transmitter coupled output power */
	a_uint16_t temp_slope;     /* Fixed decimal for internal module temperature	*/
	a_uint16_t temp_offset;    /* Fixed decimal for internal module temperature */
	a_uint16_t vol_slope;      /* Fixed decimal for internal module supply voltage */
	a_uint16_t vol_offset;     /* Fixed decimal for internal module supply voltage */
} fal_sfp_cal_const_t;

/* A2h offset: 92-94 bytes Unallocated */

/* A2h offset: 95th byte checsum for Base Diagnostic Fields*/

/* A2h offset: 96-105 bytes */
typedef struct {
	a_uint16_t cur_temp;        /* Internally measured module temperature */
	a_uint16_t cur_vol;         /* Internally measured supply voltage in transceiver */
	a_uint16_t tx_cur_bias;     /* Internally measured TX Bias Current */
	a_uint16_t tx_cur_power;    /* Measured TX output power */
	a_uint16_t rx_cur_power;    /* Measured RX input power */
} fal_sfp_realtime_diag_t;

/* A2h offset: 106-109 bytes Unallocated */

/* A2h offset: 110th byte for base control/status */
/* A2h offset: 111th byte Reserved */
/* A2h offset: 118th byte for extend control/status */
typedef struct {
	/* Optional Status/Control Bits */
	a_uint8_t data_ready:1;
	a_uint8_t rx_los:1;
	a_uint8_t tx_fault:1;
	a_uint8_t soft_rate_sel:1;
	a_uint8_t rate_sel:1;
	a_uint8_t rs_state:1;
	a_uint8_t soft_tx_disable:1;
	a_uint8_t tx_disable:1;

	/*
	 * bit7: tx disable state
	 * bit6: soft tx disable select
	 * bit5: rs state
	 * bit4: rate_select state
	 * bit3: soft rate_select select
	 * bit2: tx fault state
	 * bit1: rx_los state
	 * bit0: data_ready_bar state
	 */

	/* Extended Control/Status Bits */
	a_uint8_t pwr_level_sel:1;
	a_uint8_t pwr_level_op_state:1;
	a_uint8_t reserved_1:1;
	a_uint8_t soft_rs_sel:1;
	a_uint8_t reserved_2:4;
	/*
	 * bit7-4: Reserved
	 * bit3:   soft RS select
	 * bit2:   Reserved
	 * bit1:   Power level operation state
	 * bit0:   Power level select
	 */
} fal_sfp_ctrl_status_t;

/* A2h offset: 112-113 byte Alarm flag bits */
/* A2h offset: 114-115 byte Alarm Reserved */
/* A2h offset: 116-117 byte Warning flag bits */
typedef struct {
	/* internal diagnose exceeds high alarm level */
	a_uint8_t tx_pwr_low_alarm:1;
	a_uint8_t tx_pwr_high_alarm:1;
	a_uint8_t tx_bias_low_alarm:1;
	a_uint8_t tx_bias_high_alarm:1;
	a_uint8_t vcc_low_alarm:1;
	a_uint8_t vcc_high_alarm:1;
	a_uint8_t tmp_low_alarm:1;
	a_uint8_t tmp_high_alarm:1;
	a_uint8_t reserved_alarm:6;
	a_uint8_t rx_pwr_low_alarm:1;
	a_uint8_t rx_pwr_high_alarm:1;

	/* internal diagnose exceeds high warn level */
	a_uint8_t tx_pwr_low_warning:1;
	a_uint8_t tx_pwr_high_warning:1;
	a_uint8_t tx_bias_low_warning:1;
	a_uint8_t tx_bias_high_warning:1;
	a_uint8_t vcc_low_warning:1;
	a_uint8_t vcc_high_warning:1;
	a_uint8_t tmp_low_warning:1;
	a_uint8_t tmp_high_warning:1;
	a_uint8_t reserved_warning:6;
	a_uint8_t rx_pwr_low_warning:1;
	a_uint8_t rx_pwr_high_warning:1;
	/*
	 * bit 15: internal temperature exceeds high alarm/warning level
	 * bit 14: internal temperature is below low alarm/warning level
	 * bit 13: internal supply voltage exceeds high alarm/warning level
	 * bit 12: internal supply voltage is below low alarm/warning level
	 * bit 11: TX Bias current exceeds high alarm/warning level
	 * bit 10: TX Bias current is below low alarm/warning level
	 * bit 09: TX output power exceeds high alarm/warning level
	 * bit 08: TX output power is below low alarm/warning level
	 * bit 07: Received Power exceeds high alarm/warning level
	 * bit 06: Received Power is below low alarm/warning level
	 * bit 05~00: reserved alarm/warning
	 */
} fal_sfp_alarm_warn_flag_t;

/* A2h offset: 119th byte Reserved */
/* A2h offset: 120-127 byte Vendor Specific locations */
/* A2h offset: 128-247 byte User EEPROM */
/* A2h offset: 248-255 byte Vendor Specific locations */

sw_error_t
fal_sfp_eeprom_data_get(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry);

sw_error_t
fal_sfp_eeprom_data_set(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry);

sw_error_t
fal_sfp_device_type_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_dev_type_t *sfp_id);

sw_error_t
fal_sfp_transceiver_code_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_transc_code_t *transc_code);

sw_error_t
fal_sfp_rate_encode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_rate_encode_t *encode);

sw_error_t
fal_sfp_link_length_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_link_length_t *link_len);

sw_error_t
fal_sfp_vendor_info_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_vendor_info_t *vender_info);

sw_error_t
fal_sfp_laser_wavelength_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_laser_wavelength_t *laser_wavelen);

sw_error_t
fal_sfp_option_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_option_t *option);

sw_error_t
fal_sfp_ctrl_rate_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_rate_t *rate_limit);

sw_error_t
fal_sfp_enhanced_cfg_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_enhanced_cfg_t *enhanced_feature);

sw_error_t
fal_sfp_diag_internal_threshold_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_internal_threshold_t *threshold);

sw_error_t
fal_sfp_diag_extenal_calibration_const_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_cal_const_t *cal_const);

sw_error_t
fal_sfp_diag_realtime_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_realtime_diag_t *real_diag);

sw_error_t
fal_sfp_diag_ctrl_status_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_ctrl_status_t *ctrl_status);

sw_error_t
fal_sfp_diag_alarm_warning_flag_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_alarm_warn_flag_t *alarm_warn_flag);

sw_error_t
fal_sfp_checkcode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_cc_type_t cc_type, a_uint8_t *ccode);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_SFP_H_ */
/**
 * @}
 */
