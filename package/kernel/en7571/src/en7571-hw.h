/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EN7571 GPON laser - Hardware description
 *
 * Everything that describes the part itself, true of it regardless of what
 * the driver does with it:
 *
 *  - the register map: live register addresses, the flash calibration matrix
 *    ("BOB") offsets, and the bit masks and field positions inside both
 *
 *  - the fixed operating points the vendor programs or compares against: DAC
 *    limits, step sizes, loop hysteresis, LUT geometry, APD defaults
 *
 *  - the SFF-8472 side: alarm bit positions, alarm thresholds, and the byte
 *    positions of the diagnostic page the debugfs "a2" file exposes
 *
 * What the driver is built out of - its structures, its enums and every
 * function it declares - is in en7571.h.
 */

#ifndef _EN7571_HW_H
#define _EN7571_HW_H

#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>

/*
 * The flash calibration matrix and the live register map.
 */

/* Flash calibration matrix ("BOB" for BOSA Onboard calibration) */
#define BOB_IAV_IMOD		0x00
#define BOB_PAV_P1		0x04
/*
 * Vendor en7571_flash_reserved_8.  Used as the Ibias (high half)
 * and Imod (low half) DAC codes at the calibration temperature
 * (en7571_tuneBiasModCurrent writes it, en7571_LUT_recover reads it).
 */
#define BOB_IBIAS_IMOD		0x08
#define BOB_T0CT1C		0x0c
#define BOB_APD_SLOPE_UP		0x10
#define BOB_APD_SLOPE_DN		0x14
#define BOB_APD_CHANGE_POINT	0x18
#define BOB_T_APD		0x1c
#define BOB_LOS_THLD		0x20
/*
 * [7:0] BOSA offset below the die temperature in C (signed), [31:16]
 * ambient offset in 0.1 C; each half 0xffff when absent
 */
#define BOB_TEMP_OFFSET		0x24
/* Temperature sensor trim: [15:0] slope in 1/256, [31:16] offset in C (s16) */
#define BOB_TEMP_SENSOR		0x28
#define BOB_INTERNAL_DDMI	0x2c
#define BOB_APD_VOLTAGE_HI		0x30
#define BOB_APD_VOLTAGE_LO		0x34
/* DDMI current calibration: [15:0] gain in 1/256, [31:16] offset, 2 uA */
#define BOB_DDMI_IBIAS		0x38
#define BOB_DDMI_IMOD		0x3c
/*
 * 0x40 holds the one transmit power calibration point there is: the high half
 * is the power in 0.01 mW and the low half the PWRADC code.  The vendor's
 * TxPower path (en7571_TxPower_get_8472, 936 B of it) reads this word and
 * nothing else, so there is no multi-point transmit curve to reconstruct.
 */
#define BOB_DDMI_TX_P1		0x40
/*
 * 0x48 is not a second power point.  The SDK header names it after a TX
 * power table, which is where the gap at 0x44 comes from - 0x44 and 0x48 are
 * read only by mt7570_TxPower_get_8472, a two-point curve in a different
 * transceiver that shares this header.  For the EN7571 the word is read by
 * en7571_load_Tx_cal_data() alone, and not as a power at all: its two halves
 * go into [15:14] and [11:9] of register 0x00 as configuration fields.  Named
 * for the register it lands in, since what the two fields mean is not in the
 * binary.
 */
#define BOB_TIAMUX_CFG		0x48
#define BOB_DDMI_RX_P1		0x50
#define BOB_DDMI_RX_P2		0x54
#define BOB_DDMI_RX_P3		0x58
#define BOB_DDMI_RX_P4		0x5c
/*
 * The vendor source calls 0x60 en7571_flash_ERC, but the stock driver has no
 * EN7571 ER controller: en7571_config() reads it as mbi_delay, the time in
 * microseconds the MAC burst interface is held off around an Ibias crossing.
 */
#define BOB_MBI_DELAY		0x60
#define BOB_KT			0x64
#define BOB_KT_SWITCH		0x68
#define BOB_CROSS		0x6c
/* Bias loop limiters, written whole into PWR_LIMITER_0/2 when present */
#define BOB_IBIAS_LIMIT		0x70
#define BOB_IMOD_LIMIT		0x74
/* Calibration temperature in 0.1 C, signed (en7571_LUT_recover) */
#define BOB_TEMP_CAL		0x8c
/* 1: single closed loop, 2: open loop with LUT, else dual closed loop + KT */
#define BOB_LOOP_MODE		0x90
#define BOB_MAGIC		0x94
/*
 * Ibias/Imod lookup table, one word per 10 C from -40 C to 110 C, Ibias in
 * the high half and Imod in the low half (en7571_LUT_recover reads offsets
 * 0xa0 to 0xdc).
 */
#define BOB_LUT			0xa0
#define BOB_LUT_NR		16
/* The record must reach the magic; the LUT words past it are optional */
#define BOB_LEN			0xa1
#define BOB_MAX_LEN		(BOB_LUT + BOB_LUT_NR * 4)
#define BOB_ENTRIES		(BOB_MAX_LEN / 4)

/* Live registers */
#define EN7571_TIAMUX			0x0000
#define EN7571_TIASD			0x000c
#define EN7571_RG_ANA_CTRL1		0x0038
#define EN7571_RG_EFUSE_TEMP		0x0044
#define EN7571_SAFE_PROTECT		0x0100
/* en7571_TxFault_status(): BitRead(0x100, 15, 15) */
#define   EN7571_TX_FAULT_STATUS	BIT(15)
/* en7571_Los_status(): BitRead(0x130, 24, 24) */
#define EN7571_LOS_STATUS		0x0130
#define   EN7571_LOS_STATUS_LOS		BIT(24)
#define EN7571_LOS_CTRL1		0x011c
#define EN7571_LOS_CTRL2		0x0120
#define EN7571_P0_PWR_CTRL_CS2		0x0138
#define EN7571_P0_PWR_CTRL_CS3		0x013c
#define EN7571_P1_PWR_CTRL_CS2		0x0148
#define EN7571_P1_PWR_CTRL_CS3		0x014c
#define EN7571_DUMMY			0x015c
#define EN7571_ROGUE_ONU_DET_CTRL	0x0168
/* en7571_TxSD_status(): BitRead(0x168, 11, 11) */
#define   EN7571_TX_SD_STATUS		BIT(11)
#define EN7571_FT_ADC_CLK_CLR		0x0170
#define EN7571_RG_PWRADC_DATA		0x0204
#define EN7571_PWR_LIMITER_0		0x0220
#define EN7571_PWR_LIMITER_2		0x0224
#define EN7571_PWR_CTRL_0		0x0228
#define EN7571_PWR_CTRL_1		0x022c
#define EN7571_PWR_CTRL_2		0x0230
#define EN7571_PWR_CTRL_3		0x0234
#define EN7571_PWR_CTRL_4		0x0238
#define EN7571_PWR_CTRL_5		0x023c
#define EN7571_PWR_CTRL_6		0x0240
#define EN7571_PWR_CTRL_7		0x0244
#define EN7571_PWR_CTRL_8		0x0248
#define EN7571_PWR_CTRL_9		0x024c
#define EN7571_PWR_CTRL_A		0x0250
#define EN7571_PWR_CTRL_B		0x0254
#define EN7571_PWR_CTRL_C		0x0258
#define EN7571_PWR_CTRL_D		0x025c
#define EN7571_PWR_CTRL_E		0x0260
#define EN7571_PWR_CTRL_FLASH_1		0x027c
#define EN7571_PWR_CTRL_FLASH_2		0x0280
#define EN7571_RG_PWR_CTRL_BEN_0	0x02b4
#define EN7571_SW_RESET			0x0300
#define EN7571_T1DELAY			0x0008

/*
 * ADC source select.  The select field is SVADC_PD byte 0 bits [4:1]
 * (keep mask 0xe1); the 0.875 V bandgap is selected in the byte above it
 * (SVADC_PD + 1, keep mask 0xb3).  Selecting 0 in the field is the supply
 * voltage input.
 */
#define EN7571_SVADC_PD		0x0024
#define EN7571_ADC_SELECT_MASK	0xe1
#define EN7571_ADC_SEL_VCC	0x00
#define EN7571_ADC_SEL_TEMP	0x08
#define EN7571_ADC_SEL_BG_1V76	0x06
#define EN7571_ADC_BG_0V875_MASK	0xb3
#define EN7571_ADC_SEL_BG_0V875	0x4c
#define EN7571_ADC_SEL_RSSI	0x02

/*
 * TIAMUX byte 1.  The vendor takes the two halves of flash 0x48 into [15:14]
 * and [11:9] here through its BitWrite(), which reads and writes the whole
 * 32-bit word around them; the field positions and the 4-byte access width are
 * the vendor's and are kept, where the rest of this driver only ever touches
 * byte 0 of this register.  The masks are in the driver's "keep these bits"
 * form, as everywhere else.
 */
#define EN7571_TIAMUX_HI_MASK	0x3f	/* clears [15:14], byte 1 bits 7:6 */
#define EN7571_TIAMUX_HI_SHIFT	6
#define EN7571_TIAMUX_LO_MASK	0xf1	/* clears [11:9], byte 1 bits 3:1 */
#define EN7571_TIAMUX_LO_SHIFT	1

/*
 * ADC probe/latch: the latch is bit 4 of the byte at 0x0159, which the vendor
 * reaches as PROBE_CONTROL (0x0158) + 1; the result is PROBE_STATUS at 0x0154.
 */
#define EN7571_ADC_PROBE_STATUS	0x0154
#define EN7571_ADC_LATCH_REG	0x0159
#define EN7571_ADC_LATCH_MASK	0xef
#define EN7571_ADC_LATCH	0x10

/* LA_PWD byte 0 bit 4: receive termination high-Z while Vcc is sampled */
#define EN7571_LA_RX_HIGHZ_MASK	0xef
#define EN7571_LA_RX_HIGHZ	0x10

/* PWRADC */
#define EN7571_RG_PWRADC_DATA2	0x0208
#define EN7571_PWRADC_TRIG_MASK	0x7f
#define EN7571_PWRADC_COUNT_MASK	0xf8
#define EN7571_PWRADC_COUNT_1024	0x07
#define EN7571_PWRADC_TRIG	0x80
#define EN7571_PWRADC_VALID	0x10
#define EN7571_PWRADC_SUM_MASK	0xfffff

/* RG_ANA_CTRL1 byte 3 bit 4, RG_PWRK1 (PWRADC dark calibration) */
#define EN7571_PWRK1_MASK		0xef
#define EN7571_PWRK1_1			0x10

/* Power control readback */
#define EN7571_RO_PWR_CTRL_0	0x02a4
/* en7571_info(8), the ER DAC: 0x2a4 [25:16] */
#define   EN7571_RO_ER_DAC		GENMASK(25, 16)
#define EN7571_RO_PWR_CTRL_3	0x02b0

/* RSSI/ADLCH */
#define EN7571_RG_ADLCH_CTRL	0x0200
#define EN7571_LA_PWD		0x0014
#define EN7571_ADLCH_COUNT_MASK	0x8f
#define EN7571_ADLCH_COUNT_1024	0x70
#define EN7571_ADLCH_TRIG_MASK	0x7f
#define EN7571_ADLCH_TRIG	0x80
#define EN7571_ADLCH_VALID	0x80
#define EN7571_ADLCH_SUM_MASK	0xfffff

/* APD bias supply */
#define EN7571_APD_DAC_CODE	0x0030
#define EN7571_APD_CONTROL_ENABLE	0x01
#define EN7571_APD_SOFTSTART_ENABLE	0x20
#define EN7571_APD_SEGMENT_BASE	0x40

/*
 * PWR_CTRL_0.  Byte 0 bit 0 selects EN7571 mode (vendor "pwr_ctrl_en") and
 * byte 1 bit 0 is the reset of the dual closed loop (vendor "rst_b"), which
 * is started once the bias loop has been programmed.  They are separate
 * fields, not two views of one bit.
 */
#define EN7571_PWR_MODE_MASK		0xfe
#define EN7571_PWR_MODE_EN7571		0x01
#define EN7571_PWR_DCL_RST_MASK		0xfe
#define EN7571_PWR_DCL_RST_RELEASE	0x01

/* PWR_CTRL_2 byte 0 bit 0, rg_pwr_ctrl_e3_Imod_Adj_sel: hardware KT */
#define EN7571_IMOD_ADJ_SEL_MASK	0xfe
#define EN7571_IMOD_ADJ_SEL_KT		0x01

/* DUMMY byte 1: bit 0 is the hardware reset, bit 2 turns burst control off */
#define EN7571_DUMMY_HWRESET_MASK	0xfe
#define EN7571_DUMMY_HWRESET_SET	0x01
#define EN7571_BURST_CTRL_MASK		0xfb
#define EN7571_BURST_CTRL_OFF		0x04

/*
 * PWR_CTRL_8 bytes [1:0], MPDH update threshold (byte 0, 0x04) and largest
 * Imod step (byte 1, rg_pwr_ctrl_delta_ImodMax_setting 0x02).  The vendor
 * zeroes both while it reads a live current and puts 0x0204 back after; the
 * run value is what en7571_reg_init() programs, so only the pair is needed.
 */
#define EN7571_LOOP_STEP_HOLD		0x0000
#define EN7571_LOOP_STEP_RUN		0x0204

/* P0/P1_PWR_CTRL_CS3 byte 0 bits [1:0]: 2 holds the DAC (open loop) */
#define EN7571_OPEN_LOOP_MASK		0xfc
#define EN7571_OPEN_LOOP		0x02

/* MPDH step sizes in PWR_CTRL_3 byte 2 and PWR_CTRL_4 bytes 2 and 0 */
#define EN7571_STEPSIZE_MASK		0xc0
#define EN7571_STEPSIZE_MPDH		0x25

/* T1DELAY byte 0 is cleared down to its high nibble while the link is up */
#define EN7571_T1DELAY_LINK_MASK	0xf0

/* LOS comparator thresholds, both 7 bits; bit 7 is the comparison threshold */
#define EN7571_LOS_COMP_MASK		0x80
#define EN7571_LOS_THLD_DEFAULT		0x20
#define EN7571_SD_THLD_DEFAULT		0x30
#define EN7571_FLASH_LOS_MASK		0x0000007f
#define EN7571_FLASH_SD_MASK		0x007f0000
#define EN7571_FLASH_SD_SHIFT		16

/* DDMI enable mode held in the calibration matrix (BOB_INTERNAL_DDMI) */
#define EN7571_DDMI_OFF			0
#define EN7571_DDMI_NORMAL		1
#define EN7571_DDMI_FAST		2

/* Flash matrix word layout for the bias/modulation and power/ER pairs */
#define EN7571_FLASH_IAV_MASK		0x0fff0000
#define EN7571_FLASH_IMOD_MASK		0x00000fff
#define EN7571_FLASH_PAV_MASK		0x0fff0000
#define EN7571_FLASH_P1_MASK		0x000003ff

/* RSSI gain ladder */
#define EN7571_RSSI_GAIN_MASK	0xf8
#define EN7571_RSSI_GAIN_MAX	0x05

/* Calibration bits in LA_PWD byte 1 (keep masks, vendor form) */
#define EN7571_RSSI_CAL_MASK	0xef
#define EN7571_RSSI_CAL_ENABLE	0x10
#define EN7571_RSSI_CAL_SEL_MASK	0xbf
#define EN7571_RSSI_CAL_SEL	0x40

/* RSSI noise guard */
#define EN7571_RSSI_NOISE_GUARD	0x32

/*
 * T1DELAY fields used by the TGEN sequence (all keep masks, vendor form).
 * Byte 0 is the T0/T1 delay, bytes 1 and 2 are T1C and T0C, byte 3 holds
 * ERC enable (bit 3) and the T1/T0 timer reset (bit 5).
 */
#define EN7571_T1T0_DELAY_GPON		0xaa
/*
 * 0x77 is the vendor's value for both the GPON-SYM magic
 * (passed through as is) and the EPON one (argument 0), so the
 * two share one constant here.
 */
#define EN7571_T1T0_DELAY_OTHER		0x77
#define EN7571_TGEN_RESET_MASK		0xdf
#define EN7571_TGEN_RESET_T1T0		0x20
#define EN7571_ERC_ENABLE_MASK		0xf7
#define EN7571_ERC_ENABLE		0x08
#define FLASH_T0C_MASK			0x00ff0000
#define FLASH_T1C_MASK			0xff000000

/* SAFE_PROTECT byte 1 bit 6: safe circuit (TX fault latch) reset */
#define EN7571_SAFE_CIRCUIT_MASK	0xbf
#define EN7571_SAFE_CIRCUIT_RESET	0x40
/*
 * SAFE_PROTECT byte 1 bit 7: the latch, i.e. a transmitter fault is held
 * (en7571.c:2553-2555 reads the low two bytes and takes ptr[1] >> 7).
 */
#define EN7571_SAFE_PROTECT_FAULT	BIT(15)

/*
 * Flash calibration magic (stock en7571_init()).
 *
 * The second byte is the PON profile and is what selects the mode: 0x07 GPON,
 * 0x0a GPON-SYM (10, the SerDes mode number of the symmetric rate), 0x0e EPON
 * (14).  All three are initialised from; GPON and GPON-SYM share a branch and
 * differ only in the TGEN argument, EPON has its own
 * and skips the APD converter.
 */
#define BOB_MAGIC_GPON		0x07050701
#define BOB_MAGIC_GPON_SYM	0xa7050701
#define BOB_MAGIC_EPON		0xe7050701

/* Flash calibration data masks */
#define FLASH_TX_POWER_MASK		0xffff0000
#define FLASH_TX_POWER_OFFSET		16
#define FLASH_MPD_ADC_MASK		0x0000ffff
#define FLASH_RX_POWER_MASK		0xffff0000
#define FLASH_RX_POWER_OFFSET		16
#define FLASH_RX_RSSI_MASK		0x0000ffff

/********************************************************************/

/* Module and device name used by the bus, the regulator and the log */
#define EN7571_DRV_NAME	"en7571"

/*
 * DDMI channel groups: the unit of both caching and sampling.  The poller
 * takes one per second, on the vendor's stagger (0, 2, 4, 6, 8), so that no
 * two I2C bursts land in the same second.
 */
#define EN7571_DD_VCC	BIT(0)
#define EN7571_DD_BIAS	BIT(1)
#define EN7571_DD_TEMP	BIT(2)
#define EN7571_DD_TXPWR	BIT(3)
#define EN7571_DD_RXPWR	BIT(4)
#define EN7571_DD_ALL	(EN7571_DD_VCC | EN7571_DD_BIAS | EN7571_DD_TEMP | \
			 EN7571_DD_TXPWR | EN7571_DD_RXPWR)
#define EN7571_DD_NR	5

/*
 * SFF-8472 transceiver alarm word (vendor en7571_alarm), in the vendor's bit
 * numbering rather than the SFF-8472 flag positions.  The xPON driver reports
 * it masked with 0x3f.
 */
#define EN7571_ALARM_TX_POWER_HIGH	BIT(0)
#define EN7571_ALARM_TX_POWER_LOW	BIT(1)
#define EN7571_ALARM_BIAS_HIGH		BIT(2)
#define EN7571_ALARM_BIAS_LOW		BIT(3)
#define EN7571_ALARM_RX_POWER_HIGH	BIT(4)
#define EN7571_ALARM_RX_POWER_LOW	BIT(5)
/* Not in the vendor word: these thresholds the vendor defines but never checks */
#define EN7571_ALARM_TEMP_HIGH		BIT(6)
#define EN7571_ALARM_TEMP_LOW		BIT(7)
#define EN7571_ALARM_VCC_HIGH		BIT(8)
#define EN7571_ALARM_VCC_LOW		BIT(9)

/* SFF-8472 A2h byte positions, exposed verbatim by the debugfs page */
#define EN7571_SFF_A2_SIZE	256
#define EN7571_SFF_CC_DMI	95
#define EN7571_SFF_TEMP		96
#define EN7571_SFF_VCC		98
#define EN7571_SFF_BIAS		100
#define EN7571_SFF_TX_POWER	102
#define EN7571_SFF_RX_POWER	104
#define EN7571_SFF_ALARM_FLAGS	112
#define EN7571_SFF_WARN_FLAGS	116

/* hwmon power channel index, in the order the vendor numbers them */
#define EN7571_TX_POWER_CHANNEL	0

/* Size of the seq_file buffer the "dump" parameter formats into */
#define EN7571_DUMP_SIZE	4096

/* Oldest a cached group may be handed out; older and the reader resamples */
#define EN7571_DD_MAX_AGE	(10 * HZ)

/*
 * Modulation codes where the vendor's double arithmetic lands just below
 * code * 10.99 and truncates to one less: bit n is set for code 100 * n.
 */
#define EN7571_MOD_LOW_CODES	(BIT_ULL(3) | BIT_ULL(6) | BIT_ULL(12) | \
				 BIT_ULL(13) | BIT_ULL(24) | BIT_ULL(25) | \
				 BIT_ULL(26) | BIT_ULL(33) | BIT_ULL(35))

/* KT factors used when flash 0x64 is empty (HT 0x0020 low, LT 0x0006 high) */
#define EN7571_KT_DEFAULT	0x00060020

/* Imod targets: Imod_cal * (2^17 + dIav * k) >> 17, or (2^10 + r * k) >> 12 */
#define EN7571_KT_ONE		0x20000
#define EN7571_KT_SHIFT		17
#define EN7571_KT_ENH_ONE	0x400
#define EN7571_KT_ENH_SHIFT	12

/* SWKT: the KT factors to load when Iav is far from calibration */

/* DAC limits of the software loops (flash 0x70/0x74 unused) */
#define EN7571_IBIAS_MIN	0x080
#define EN7571_IBIAS_FLOOR	0x100
#define EN7571_IBIAS_MAX	0xf7f
#define EN7571_IMOD_MIN		0x032
#define EN7571_IMOD_MAX		0xf7f
#define EN7571_IBIAS_STEP	4
/* No Ibias crossing for a Pav error below this */
#define EN7571_PAV_HYST		0x10
/* Imod crossing up only past 0x10 into the next 256, down below 0xf0 */
#define EN7571_IMOD_HYST_UP	0x11
#define EN7571_IMOD_HYST_DN	0xf0

/* Link up: Pav within 3 of its target, or accepted after 5 attempts */
#define EN7571_LINKUP_PAV_ERR	3
#define EN7571_LINKUP_TRIES	5

/* LUT: 64 entries 2.5 C apart from -40 C; flash points 10 C apart */
#define EN7571_LUT_NR		64
#define EN7571_LUT_T0_MD	(-40000)
#define EN7571_LUT_STEP_MD	2500
#define EN7571_LUT_EMPTY	0xfff
/* Calibration temperature range accepted by en7571_LUT_recover, in C */
#define EN7571_TCAL_MIN		(-40)
#define EN7571_TCAL_MAX		110
/* Ambient is the die temperature less flash 0x24[31:16] / 10, default 10 C */
#define EN7571_ENV_OFFSET_DEFAULT	10

/* APD bias supply defaults (millivolts) */
#define EN7571_APD_V_NT_DEFAULT		35000
#define EN7571_APD_SLOPE_UP_DEFAULT	100
#define EN7571_APD_SLOPE_DN_DEFAULT	70
#define EN7571_APD_V_00_DEFAULT		30000
#define EN7571_APD_V_40_DEFAULT		36000
#define EN7571_APD_V_80_DEFAULT		42000
#define EN7571_APD_V_C0_DEFAULT		45600

/* APD temperature pivot */
#define EN7571_APD_T_PIVOT_MD		25000

/* PWRADC calibration delay */
#define PWRADC_CAL_DELAY_MS		20

/* RSSI/ADLCH conversion delay */
#define ADLCH_CONV_DELAY_MS		20

/*
 * APD update period in seconds: vendor default en7571_Tapd = 60, a value
 * from flash below 10 is raised to 10.
 */
#define EN7571_TAPD_DEFAULT		60
#define EN7571_TAPD_MIN			10

/*
 * BOSA temperature the vendor assumes before the first measurement
 * (en7571_BOSA_temperature = 20.0); the initial APD code is computed from it.
 */
#define EN7571_BOSA_TEMP_INIT_MD	20000

/*
 * APD voltage the vendor reports before the first APD tracking
 * (en7571_init(): en7571_APD_voltage = 35.0); debugfs only.
 */
#define EN7571_APD_VOLTAGE_INIT_MV	35000

/* BOSA offset below the die temperature when flash has none, in C */
#define EN7571_BOSA_OFFSET_DEFAULT	5

/*
 * Transceiver alarm thresholds in SFF-8472 units (vendor en7571_*_alarm_thod).
 * Below the low or above the high raises the alarm; no hysteresis, no warning
 * level.  The VCC and temperature thresholds the vendor defines but never
 * checks are evaluated here as well.
 */
#define EN7571_TX_POWER_LOW_ALARM_THOD	0x2710	/* 0.1 uW, 0 dBm */
#define EN7571_TX_POWER_HIGH_ALARM_THOD	0x8a99	/* 0.1 uW, 5.5 dBm */
#define EN7571_TX_CUR_LOW_ALARM_THOD	0x01f4	/* 2 uA, 1 mA */
#define EN7571_TX_CUR_HIGH_ALARM_THOD	0xc350	/* 2 uA, 100 mA */
#define EN7571_RX_POWER_LOW_ALARM_THOD	0x000a	/* 0.1 uW, -30 dBm */
#define EN7571_RX_POWER_HIGH_ALARM_THOD	0x09cf	/* 0.1 uW, -6 dBm */
#define EN7571_VCC_LOW_ALARM_THOD	0x7148	/* 100 uV, 2.9 V */
#define EN7571_VCC_HIGH_ALARM_THOD	0x9088	/* 100 uV, 3.7 V */
#define EN7571_TEMP_LOW_ALARM_THOD	((s16)0xfb00)	/* 1/256 C, -5 C */
#define EN7571_TEMP_HIGH_ALARM_THOD	((s16)0x5500)	/* 1/256 C, 85 C */

/* ADC calibration: number of samples averaged per bandgap reference */
#define EN7571_ADC_CAL_SAMPLES		8

#endif /* _EN7571_HW_H */
