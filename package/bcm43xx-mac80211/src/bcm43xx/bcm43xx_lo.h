#ifndef BCM43xx_LO_H_
#define BCM43xx_LO_H_

#include "bcm43xx_phy.h"

struct bcm43xx_wldev;

/* Local Oscillator control value-pair. */
struct bcm43xx_loctl {
	/* Control values. */
	s8 i;
	s8 q;
	/* "Used by hardware" flag. */
	u8 used;
};

/* TX Power LO Control Array.
 * Value-pairs to adjust the LocalOscillator are stored
 * in this structure.
 * There are two different set of values. One for "Flag is Set"
 * and one for "Flag is Unset".
 * By "Flag" the flag in struct bcm43xx_rfatt is meant.
 * The Value arrays are two-dimensional. The first index
 * is the baseband attenuation and the second index
 * is the radio attenuation.
 * Use bcm43xx_get_lo_g_ctl() to retrieve a value from the lists.
 */
struct bcm43xx_txpower_lo_control {
#define BCM43xx_NR_BB	9
#define BCM43xx_NR_RF	16
	/* LO Control values, with PAD Mixer */
	struct bcm43xx_loctl with_padmix[ BCM43xx_NR_BB ][ BCM43xx_NR_RF ];
	/* LO Control values, without PAD Mixer */
	struct bcm43xx_loctl no_padmix[ BCM43xx_NR_BB ][ BCM43xx_NR_RF ];

	/* Flag to indicate a complete rebuild of the two tables above
	 * to the LO measuring code. */
	u8 rebuild;

	/* Lists of valid RF and BB attenuation values for this device. */
	struct bcm43xx_rfatt_list rfatt_list;
	struct bcm43xx_bbatt_list bbatt_list;

	/* Current TX Bias value */
	u8 tx_bias;
	/* Current TX Magnification Value (if used by the device) */
	u8 tx_magn;

	/* GPHY LO is measured. */
	u8 lo_measured;

	/* Saved device PowerVector */
	u64 power_vector;
};


/* Measure the BPHY Local Oscillator. */
void bcm43xx_lo_b_measure(struct bcm43xx_wldev *dev);
/* Measure the BPHY/GPHY Local Oscillator. */
void bcm43xx_lo_g_measure(struct bcm43xx_wldev *dev);

/* Adjust the Local Oscillator to the saved attenuation
 * and txctl values.
 */
void bcm43xx_lo_g_adjust(struct bcm43xx_wldev *dev);
/* Adjust to specific values. */
void bcm43xx_lo_g_adjust_to(struct bcm43xx_wldev *dev,
			  u16 rfatt, u16 bbatt, u16 tx_control);

/* Returns the bcm43xx_lo_g_ctl corresponding to the current
 * attenuation values.
 */
struct bcm43xx_loctl * bcm43xx_lo_g_ctl_current(struct bcm43xx_wldev *dev);
/* Mark all possible bcm43xx_lo_g_ctl as "unused" */
void bcm43xx_lo_g_ctl_mark_all_unused(struct bcm43xx_wldev *dev);
/* Mark the bcm43xx_lo_g_ctl corresponding to the current
 * attenuation values as used.
 */
void bcm43xx_lo_g_ctl_mark_cur_used(struct bcm43xx_wldev *dev);

/* Get a reference to a LO Control value pair in the
 * TX Power LO Control Array.
 */
struct bcm43xx_loctl * bcm43xx_get_lo_g_ctl(struct bcm43xx_wldev *dev,
					 const struct bcm43xx_rfatt *rfatt,
					 const struct bcm43xx_bbatt *bbatt);

#endif /* BCM43xx_LO_H_ */
