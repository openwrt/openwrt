/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EN7571 GPON laser - Function prototypes and shared definitions
 */

#ifndef _EN7571_H
#define _EN7571_H

#include <linux/types.h>
#include <linux/bits.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/gpio/consumer.h>
#include <linux/pon_laser.h>
#include <linux/regulator/driver.h>
#include "en7571-hw.h"

struct dentry;

/*
 * Shared definitions, data structures and prototypes.
 */

/* en7571_info() selectors, vendor SELECT_* (en7571_def.h) */
#define EN7571_SEL_PAV_CAL	1
#define EN7571_SEL_IAV_NOW	2
#define EN7571_SEL_IBIAS_NOW	3
#define EN7571_SEL_IMOD_NOW	4
#define EN7571_SEL_P1_CAL	6
#define EN7571_SEL_PAV_NOW	7

/*
 * PON mode, from the calibration matrix or from the MAC.
 *
 * The numbers are the PON profile byte each calibration magic carries in its
 * second byte, so the matrix selects a mode by the same number the MAC names
 * it with: GPON_SYM and EPON keep theirs (10 and 14), which are also the SerDes
 * mode numbers on the vendor side.  GPON is the exception and is zero, so a
 * zeroed struct already says GPON.  The transceiver is the same part in all
 * three; they differ only in the T0/T1 delay byte the matrix programs.
 */
enum en7571_pon_mode {
	EN7571_PON_GPON		= 0,
	EN7571_PON_GPON_SYM	= 10,
	EN7571_PON_EPON		= 14,
};

struct en7571 {
	struct i2c_client *client;
	/* Serialises every register access; each entry point takes it itself */
	struct mutex lock;
	struct gpio_desc *tx_disable;
	struct gpio_desc *vcc_disable;
	struct regulator_dev *rdev;
	u32 cal[BOB_ENTRIES];

	/* Bias and modulation current, SFF-8472 units of 2 uA */
	u16 bias_8472;
	u16 mod_8472;
	u32 vcc_uV;
	/* SFF-8472 units, 0.1 uW */
	u32 tx_power_8472;
	u32 rx_power_8472;
	u32 rssi_vref;
	u32 pwradc_offset;
	/* Bandgap codes from the ADC calibration at probe (vendor slope/offset) */
	u32 adc_bg_1v76;
	u32 adc_bg_0v875;
	bool adc_cal_ok;
	/* Die and BOSA case temperature; both may be negative */
	s32 temp_millideg;
	s32 bosa_temp_millideg;
	/* Per-die temperature trim, sampled from eFuse during initialization */
	s32 efuse_temp_offset_millideg;
	/* EN7571_DDMI_{OFF,NORMAL,FAST}, read from the calibration matrix */
	u8 ddmi_mode;
	/*
	 * Whether the APD converter runs.  The vendor starts it and the
	 * temperature tracking only in its GPON branch, so this gates the
	 * tracking rather than the mode, and is kept apart from pon_mode because
	 * the MAC may select a mode at runtime.
	 */
	bool gpon;
	enum en7571_pon_mode pon_mode;
	/* APD update period in seconds (flash 0x1c, at least 10, default 60) */
	unsigned int tapd;
	/* Last APD target voltage (vendor en7571_APD_voltage), for debugfs */
	s32 apd_mv;
	bool tx_enabled;

	/*
	 * Laser control loop (en7571-ctrl.c), the state the vendor keeps in
	 * globals.  Protected by priv->lock except traffic, which the MAC sets
	 * from any context.
	 */
	/* 0 off, 1 normal, 2 inverse, 3 enhanced (vendor en7571_KT) */
	u8 kt;
	/* Single closed loop / open loop with LUT (flash 0x90 = 1 / 2) */
	bool scl;
	bool dol;
	/* Hold the MAC burst interface off around crossing writes */
	bool cross;
	u32 mbi_delay_us;
	/* PON traffic up (vendor gpPhyPriv flags.trafficStatus) */
	bool traffic;
	/* Faults already handed to the PON core, so each is sent once */
	u32 fe_alarms;
	/* Link state the loop last acted on, and failed link-up attempts */
	bool linkup;
	u8 unstable_cnt;
	bool need_cross_ibias;
	bool need_cross_imod;
	/* DAC codes the loop last set, and the Iav they imply */
	u16 ibias_now;
	u16 imod_now;
	u16 iav_now;
	/* Last valid PWRADC sum, kept when a conversion is not valid */
	u32 tssi;
	/* Ibias/Imod per 2.5 C from -40 C (vendor en7571_LUT_Ibias_Imod) */
	s32 lut[64][2];

	/* EN7571_ALARM_*, evaluated by the DDMI tick after the power samples */
	u32 alarm;

	/* Background DDMI sampling (en7571-regs.c) */
	struct delayed_work ddmi_work;
	unsigned int ddmi_cnt;
	/* Which EN7571_DD_* groups hold a sample the reader may trust, and when
	 * each was last taken
	 */
	u32 ddmi_valid;
	unsigned long ddmi_stamp[EN7571_DD_NR];
};

/*
 * The "bob" values (vendor en7571_BoB_info() and _Debug()) that the kernel log
 * dump and debugfs "bob" file present, gathered in one read of the device.
 */
struct en7571_bob {
	u32 los;
	u32 safe;
	u32 rogue;
	u32 ro_pwr;
	u16 bias;
	u16 mod;
	s32 ic_md;
	s32 bosa_md;
	s32 env_md;
	s32 apd_mv;
	u32 vcc_uV;
	u32 tx_power;
	u32 rx_power;
	bool traffic;
	bool dol;
	u16 ibias_now;
	u16 imod_now;
	u16 iav_now;
	u32 tssi;
	u8 ddmi_mode;
	u32 iav_imod;
	u32 pav_p1;
	u32 t0ct1c;
};

/*
 * Module parameters and kernel-log diagnostics.  "debug" selects what the
 * en7571_dbg() calls print.
 */
extern unsigned int en7571_debug;
#define EN7571_DBG_OPS		BIT(0)	/* transmitter, safe circuit, traffic, MBI hook */
#define EN7571_DBG_DUMP		BIT(1)	/* + the "dump" after the safe circuit reset */
#define en7571_dbg(_p, _bit, _fmt, ...)					\
	do {								\
		if (en7571_debug & (_bit))				\
			dev_info(&(_p)->client->dev, "dbg: " _fmt, ##__VA_ARGS__); \
	} while (0)

/*
 * Nearly every step in this driver is a register write or a read that is
 * checked the same way, so both spell it out: assign, and stop if it failed.
 * The caller must already have an int ret, which every one of them does.
 *
 * en7571_try() is the bare form and says nothing about which step failed.
 * en7571_try_probe() is for a probe path, where the step is worth naming:
 * it is the last thing between a silent probe failure and a device that
 * comes up looking fine.
 */
#define en7571_try(_op)						\
	do {							\
		ret = (_op);					\
		if (ret)					\
			return ret;				\
	} while (0)

#define en7571_try_probe(_dev, _op, _what)				\
	do {							\
		ret = (_op);					\
		if (ret)					\
			return dev_err_probe(_dev, ret, _what);	\
	} while (0)

/*
 * The same check where the step is not the last thing the function does and
 * there is an exit label to unwind through, which is how the measurement
 * sequences in en7571-calibration.c are written.
 */
#define en7571_try_goto(_op, _label)					\
	do {							\
		ret = (_op);					\
		if (ret)					\
			goto _label;				\
	} while (0)

/*
 * Everything the driver declares: the register access the rest is written on,
 * then the rest grouped by the file it lives in.
 */

/* en7571-regs.c: the register bus, the ADC readers and the DDMI tick */
int en7571_reg_read(struct en7571 *priv, u16 reg, u8 *buf, u8 len);
int en7571_reg_write(struct en7571 *priv, u16 reg, const u8 *buf, u8 len);
int en7571_update(struct en7571 *priv, u16 reg, u8 len, u8 index,
		  u8 keep, u8 set);
int en7571_rmw_byte(struct en7571 *priv, u16 reg, u8 index, u8 keep, u8 set);
int en7571_rmw8(struct en7571 *priv, u16 reg, u8 keep, u8 set);
int en7571_read_reg(struct en7571 *priv, u16 reg, u8 len, u32 *val);
int en7571_write_reg(struct en7571 *priv, u16 reg, u8 len, u32 val);
int en7571_svadc_get(struct en7571 *priv, u32 *raw);
int en7571_adc_calibration(struct en7571 *priv);
/* the samplers the tick and an on-demand read share */
int en7571_refresh_locked(struct en7571 *priv, u32 mask);
/* SFF-8472 unit conversions, shared with the alarm word below */
s16 en7571_temp_8472(s32 millideg);
u16 en7571_vcc_8472(u32 uV);
void en7571_ddmi_init(struct en7571 *priv);
void en7571_ddmi_start(struct en7571 *priv);
void en7571_ddmi_release(void *data);

/* en7571-calibration.c: the flash matrix and the runtime bring-up */
int en7571_read_calibration(struct en7571 *priv);
int en7571_detect(struct en7571 *priv);
int en7571_hw_init(struct en7571 *priv);
int en7571_pon_mode_from_magic(u32 magic, enum en7571_pon_mode *mode);
const char *en7571_pon_mode_name(enum en7571_pon_mode mode);
int en7571_set_pon_mode(struct en7571 *priv, enum en7571_pon_mode mode);

/*
 * en7571-ctrl.c: the control loops and what they drive.  Called with
 * priv->lock held except the setters and the entry points exported to the xPON
 * PHY, which take it themselves.
 */
void en7571_ctrl_init(struct en7571 *priv);
void en7571_ctrl_reset(struct en7571 *priv);
int en7571_hw_reset(struct en7571 *priv);
int en7571_dcl_start(struct en7571 *priv);
int en7571_pwradc_calibration(struct en7571 *priv);
int en7571_calc_tx_power_locked(struct en7571 *priv, u32 *tx_power_8472);
int en7571_rssi_calibrate(struct en7571 *priv);
int en7571_read_rssi_locked(struct en7571 *priv, u32 *rssi);
int en7571_calc_rx_power_locked(struct en7571 *priv, u32 rssi,
				u32 *rx_power_8472);
int en7571_apd_init(struct en7571 *priv);
int en7571_apd_set_locked(struct en7571 *priv, s32 bosa_millideg);
int en7571_safe_circuit_reset(struct en7571 *priv);
int en7571_safe_circuit_reset_locked(struct en7571 *priv);
int en7571_info(struct en7571 *priv, int sel, u32 *val);
int en7571_link_reg(struct en7571 *priv, bool enable);
int en7571_single_closed_loop_mode(struct en7571 *priv);
int en7571_open_loop_mode(struct en7571 *priv, bool open);
void en7571_lut_recover(struct en7571 *priv);
int en7571_lut_tracking(struct en7571 *priv);
s32 en7571_env_temp_md(struct en7571 *priv);
int en7571_ctrl_tick(struct en7571 *priv, unsigned int slot);
void en7571_set_traffic(struct en7571 *priv, bool up);

/* en7571-main.c: probe and the outside (hwmon, debugfs, regulator) */
void en7571_dump(struct en7571 *priv);

#endif /* _EN7571_H */
