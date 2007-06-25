#ifndef BCM43xx_PHY_H_
#define BCM43xx_PHY_H_

#include <linux/types.h>

struct bcm43xx_wldev;


/*** PHY Registers ***/

/* Routing */
#define BCM43xx_PHYROUTE_OFDM_GPHY	0x400
#define BCM43xx_PHYROUTE_EXT_GPHY	0x800

/* Base registers. */
#define BCM43xx_PHY_BASE(reg)	(reg)
/* OFDM (A) registers of a G-PHY */
#define BCM43xx_PHY_OFDM(reg)	((reg) | BCM43xx_PHYROUTE_OFDM_GPHY)
/* Extended G-PHY registers */
#define BCM43xx_PHY_EXTG(reg)	((reg) | BCM43xx_PHYROUTE_EXT_GPHY)


/* OFDM (A) PHY Registers */
#define BCM43xx_PHY_VERSION_OFDM	BCM43xx_PHY_OFDM(0x00)	/* Versioning register for A-PHY */
#define BCM43xx_PHY_BBANDCFG		BCM43xx_PHY_OFDM(0x01)	/* Baseband config */
#define  BCM43xx_PHY_BBANDCFG_RXANT	0x180			/* RX Antenna selection */
#define  BCM43xx_PHY_BBANDCFG_RXANT_SHIFT	7
#define BCM43xx_PHY_PWRDOWN		BCM43xx_PHY_OFDM(0x03)	/* Powerdown */
#define BCM43xx_PHY_CRSTHRES1		BCM43xx_PHY_OFDM(0x06)	/* CRS Threshold 1 */
#define BCM43xx_PHY_LNAHPFCTL		BCM43xx_PHY_OFDM(0x1C)	/* LNA/HPF control */
#define BCM43xx_PHY_ADIVRELATED		BCM43xx_PHY_OFDM(0x27)	/* FIXME rename */
#define BCM43xx_PHY_CRS0		BCM43xx_PHY_OFDM(0x29)
#define BCM43xx_PHY_ANTDWELL		BCM43xx_PHY_OFDM(0x2B)	/* Antenna dwell */
#define  BCM43xx_PHY_ANTDWELL_AUTODIV1	0x0100			/* Automatic RX diversity start antenna */
#define BCM43xx_PHY_ENCORE		BCM43xx_PHY_OFDM(0x49)	/* "Encore" (RangeMax / BroadRange) */
#define  BCM43xx_PHY_ENCORE_EN		0x0200			/* Encore enable */
#define BCM43xx_PHY_LMS			BCM43xx_PHY_OFDM(0x55)
#define BCM43xx_PHY_OFDM61		BCM43xx_PHY_OFDM(0x61)	/* FIXME rename */
#define  BCM43xx_PHY_OFDM61_10		0x0010			/* FIXME rename */
#define BCM43xx_PHY_IQBAL		BCM43xx_PHY_OFDM(0x69)	/* I/Q balance */
#define BCM43xx_PHY_OTABLECTL		BCM43xx_PHY_OFDM(0x72)	/* OFDM table control (see below) */
#define  BCM43xx_PHY_OTABLEOFF		0x03FF			/* OFDM table offset (see below) */
#define  BCM43xx_PHY_OTABLENR		0xFC00			/* OFDM table number (see below) */
#define  BCM43xx_PHY_OTABLENR_SHIFT	10
#define BCM43xx_PHY_OTABLEI		BCM43xx_PHY_OFDM(0x73)	/* OFDM table data I */
#define BCM43xx_PHY_OTABLEQ		BCM43xx_PHY_OFDM(0x74)	/* OFDM table data Q */
#define BCM43xx_PHY_HPWR_TSSICTL	BCM43xx_PHY_OFDM(0x78)	/* Hardware power TSSI control */
#define BCM43xx_PHY_NRSSITHRES		BCM43xx_PHY_OFDM(0x8A)	/* NRSSI threshold */
#define BCM43xx_PHY_ANTWRSETT		BCM43xx_PHY_OFDM(0x8C)	/* Antenna WR settle */
#define  BCM43xx_PHY_ANTWRSETT_ARXDIV	0x2000			/* Automatic RX diversity enabled */
#define BCM43xx_PHY_CLIPPWRDOWNT	BCM43xx_PHY_OFDM(0x93)	/* Clip powerdown threshold */
#define BCM43xx_PHY_OFDM9B		BCM43xx_PHY_OFDM(0x9B)	/* FIXME rename */
#define BCM43xx_PHY_N1P1GAIN		BCM43xx_PHY_OFDM(0xA0)
#define BCM43xx_PHY_P1P2GAIN		BCM43xx_PHY_OFDM(0xA1)
#define BCM43xx_PHY_N1N2GAIN		BCM43xx_PHY_OFDM(0xA2)
#define BCM43xx_PHY_CLIPTHRES		BCM43xx_PHY_OFDM(0xA3)
#define BCM43xx_PHY_CLIPN1P2THRES	BCM43xx_PHY_OFDM(0xA4)
#define BCM43xx_PHY_DIVSRCHIDX		BCM43xx_PHY_OFDM(0xA8)	/* Divider search gain/index */
#define BCM43xx_PHY_CLIPP2THRES		BCM43xx_PHY_OFDM(0xA9)
#define BCM43xx_PHY_CLIPP3THRES		BCM43xx_PHY_OFDM(0xAA)
#define BCM43xx_PHY_DIVP1P2GAIN		BCM43xx_PHY_OFDM(0xAB)
#define BCM43xx_PHY_DIVSRCHGAINBACK	BCM43xx_PHY_OFDM(0xAD)	/* Divider search gain back */
#define BCM43xx_PHY_DIVSRCHGAINCHNG	BCM43xx_PHY_OFDM(0xAE)	/* Divider search gain change */
#define BCM43xx_PHY_CRSTHRES1_R1	BCM43xx_PHY_OFDM(0xC0)	/* CRS Threshold 1 (rev 1 only) */
#define BCM43xx_PHY_CRSTHRES2_R1	BCM43xx_PHY_OFDM(0xC1)	/* CRS Threshold 2 (rev 1 only) */
#define BCM43xx_PHY_TSSIP_LTBASE	BCM43xx_PHY_OFDM(0x380)	/* TSSI power lookup table base */
#define BCM43xx_PHY_DC_LTBASE		BCM43xx_PHY_OFDM(0x3A0)	/* DC lookup table base */
#define BCM43xx_PHY_GAIN_LTBASE		BCM43xx_PHY_OFDM(0x3C0)	/* Gain lookup table base */

/* CCK (B) PHY Registers */
#define BCM43xx_PHY_VERSION_CCK		BCM43xx_PHY_BASE(0x00)	/* Versioning register for B-PHY */
#define BCM43xx_PHY_CCKBBANDCFG		BCM43xx_PHY_BASE(0x01)	/* Contains antenna 0/1 control bit */
#define BCM43xx_PHY_PGACTL		BCM43xx_PHY_BASE(0x15)	/* PGA control */
#define  BCM43xx_PHY_PGACTL_LPF		0x1000			/* Low pass filter (?) */
#define  BCM43xx_PHY_PGACTL_LOWBANDW	0x0040			/* Low bandwidth flag */
#define  BCM43xx_PHY_PGACTL_UNKNOWN	0xEFA0
#define BCM43xx_PHY_FBCTL1		BCM43xx_PHY_BASE(0x18)	/* Frequency bandwidth control 1 */
#define BCM43xx_PHY_ITSSI		BCM43xx_PHY_BASE(0x29)	/* Idle TSSI */
#define BCM43xx_PHY_LO_LEAKAGE		BCM43xx_PHY_BASE(0x2D)	/* Measured LO leakage */
#define BCM43xx_PHY_ENERGY		BCM43xx_PHY_BASE(0x33)	/* Energy */
#define BCM43xx_PHY_SYNCCTL		BCM43xx_PHY_BASE(0x35)
#define BCM43xx_PHY_FBCTL2		BCM43xx_PHY_BASE(0x38)	/* Frequency bandwidth control 2 */
#define BCM43xx_PHY_DACCTL		BCM43xx_PHY_BASE(0x60)	/* DAC control */
#define BCM43xx_PHY_RCCALOVER		BCM43xx_PHY_BASE(0x78)	/* RC calibration override */

/* Extended G-PHY Registers */
#define BCM43xx_PHY_CLASSCTL		BCM43xx_PHY_EXTG(0x02)	/* Classify control */
#define BCM43xx_PHY_GTABCTL		BCM43xx_PHY_EXTG(0x03)	/* G-PHY table control (see below) */
#define  BCM43xx_PHY_GTABOFF		0x03FF			/* G-PHY table offset (see below) */
#define  BCM43xx_PHY_GTABNR		0xFC00			/* G-PHY table number (see below) */
#define  BCM43xx_PHY_GTABNR_SHIFT	10
#define BCM43xx_PHY_GTABDATA		BCM43xx_PHY_EXTG(0x04)	/* G-PHY table data */
#define BCM43xx_PHY_LO_MASK		BCM43xx_PHY_EXTG(0x0F)	/* Local Oscillator control mask */
#define BCM43xx_PHY_LO_CTL		BCM43xx_PHY_EXTG(0x10)	/* Local Oscillator control */
#define BCM43xx_PHY_RFOVER		BCM43xx_PHY_EXTG(0x11)	/* RF override */
#define BCM43xx_PHY_RFOVERVAL		BCM43xx_PHY_EXTG(0x12)	/* RF override value */
#define  BCM43xx_PHY_RFOVERVAL_EXTLNA	0x8000
#define  BCM43xx_PHY_RFOVERVAL_LNA	0x7000
#define  BCM43xx_PHY_RFOVERVAL_LNA_SHIFT	12
#define  BCM43xx_PHY_RFOVERVAL_PGA	0x0F00
#define  BCM43xx_PHY_RFOVERVAL_PGA_SHIFT	8
#define  BCM43xx_PHY_RFOVERVAL_UNK	0x0010 /* Unknown, always set. */
#define  BCM43xx_PHY_RFOVERVAL_TRSWRX	0x00E0
#define  BCM43xx_PHY_RFOVERVAL_BW	0x0003 /* Bandwidth flags */
#define   BCM43xx_PHY_RFOVERVAL_BW_LPF	0x0001 /* Low Pass Filter */
#define   BCM43xx_PHY_RFOVERVAL_BW_LBW	0x0002 /* Low Bandwidth (when set), high when unset */
#define BCM43xx_PHY_ANALOGOVER		BCM43xx_PHY_EXTG(0x14)	/* Analog override */
#define BCM43xx_PHY_ANALOGOVERVAL	BCM43xx_PHY_EXTG(0x15)	/* Analog override value */



/*** OFDM table numbers ***/
#define BCM43xx_OFDMTAB(number, offset)	(((number) << BCM43xx_PHY_OTABLENR_SHIFT) | (offset))
#define BCM43xx_OFDMTAB_AGC1		BCM43xx_OFDMTAB(0x00, 0)
#define BCM43xx_OFDMTAB_GAIN0		BCM43xx_OFDMTAB(0x00, 0)
#define BCM43xx_OFDMTAB_GAINX		BCM43xx_OFDMTAB(0x01, 0)	//TODO rename
#define BCM43xx_OFDMTAB_GAIN1		BCM43xx_OFDMTAB(0x01, 4)
#define BCM43xx_OFDMTAB_AGC3		BCM43xx_OFDMTAB(0x02, 0)
#define BCM43xx_OFDMTAB_GAIN2		BCM43xx_OFDMTAB(0x02, 3)
#define BCM43xx_OFDMTAB_LNAHPFGAIN1	BCM43xx_OFDMTAB(0x03, 0)
#define BCM43xx_OFDMTAB_WRSSI		BCM43xx_OFDMTAB(0x04, 0)
#define BCM43xx_OFDMTAB_LNAHPFGAIN2	BCM43xx_OFDMTAB(0x04, 0)
#define BCM43xx_OFDMTAB_NOISESCALE	BCM43xx_OFDMTAB(0x05, 0)
#define BCM43xx_OFDMTAB_AGC2		BCM43xx_OFDMTAB(0x06, 0)
#define BCM43xx_OFDMTAB_ROTOR		BCM43xx_OFDMTAB(0x08, 0)
#define BCM43xx_OFDMTAB_ADVRETARD	BCM43xx_OFDMTAB(0x09, 0)
#define BCM43xx_OFDMTAB_DAC		BCM43xx_OFDMTAB(0x0C, 0)
#define BCM43xx_OFDMTAB_DC		BCM43xx_OFDMTAB(0x0E, 7)
#define BCM43xx_OFDMTAB_PWRDYN2		BCM43xx_OFDMTAB(0x0E, 12)
#define BCM43xx_OFDMTAB_LNAGAIN		BCM43xx_OFDMTAB(0x0E, 13)
//TODO
#define BCM43xx_OFDMTAB_LPFGAIN		BCM43xx_OFDMTAB(0x0F, 12)
#define BCM43xx_OFDMTAB_RSSI		BCM43xx_OFDMTAB(0x10, 0)
//TODO
#define BCM43xx_OFDMTAB_AGC1_R1		BCM43xx_OFDMTAB(0x13, 0)
#define BCM43xx_OFDMTAB_GAINX_R1	BCM43xx_OFDMTAB(0x14, 0)	//TODO rename
#define BCM43xx_OFDMTAB_MINSIGSQ	BCM43xx_OFDMTAB(0x14, 1)
#define BCM43xx_OFDMTAB_AGC3_R1		BCM43xx_OFDMTAB(0x15, 0)
#define BCM43xx_OFDMTAB_WRSSI_R1	BCM43xx_OFDMTAB(0x15, 4)
#define BCM43xx_OFDMTAB_TSSI		BCM43xx_OFDMTAB(0x15, 0)
#define BCM43xx_OFDMTAB_DACRFPABB	BCM43xx_OFDMTAB(0x16, 0)
#define BCM43xx_OFDMTAB_DACOFF		BCM43xx_OFDMTAB(0x17, 0)
#define BCM43xx_OFDMTAB_DCBIAS		BCM43xx_OFDMTAB(0x18, 0)

u16 bcm43xx_ofdmtab_read16(struct bcm43xx_wldev *dev, u16 table, u16 offset);
void bcm43xx_ofdmtab_write16(struct bcm43xx_wldev *dev, u16 table,
			     u16 offset, u16 value);
u32 bcm43xx_ofdmtab_read32(struct bcm43xx_wldev *dev, u16 table, u16 offset);
void bcm43xx_ofdmtab_write32(struct bcm43xx_wldev *dev, u16 table,
			     u16 offset, u32 value);


/*** G-PHY table numbers */
#define BCM43xx_GTAB(number, offset)	(((number) << BCM43xx_PHY_GTABNR_SHIFT) | (offset))
#define BCM43xx_GTAB_NRSSI		BCM43xx_GTAB(0x00, 0)
#define BCM43xx_GTAB_TRFEMW		BCM43xx_GTAB(0x0C, 0x120)
#define BCM43xx_GTAB_ORIGTR		BCM43xx_GTAB(0x2E, 0x298)

u16 bcm43xx_gtab_read(struct bcm43xx_wldev *dev, u16 table, u16 offset);	//TODO implement
void bcm43xx_gtab_write(struct bcm43xx_wldev *dev, u16 table,
			u16 offset, u16 value);					//TODO implement



#define BCM43xx_DEFAULT_CHANNEL_A	36
#define BCM43xx_DEFAULT_CHANNEL_BG	6

enum {
	BCM43xx_ANTENNA0,	/* Antenna 0 */
	BCM43xx_ANTENNA1,	/* Antenna 0 */
	BCM43xx_ANTENNA_AUTO1,	/* Automatic, starting with antenna 1 */
	BCM43xx_ANTENNA_AUTO0,	/* Automatic, starting with antenna 0 */

	BCM43xx_ANTENNA_AUTO	= BCM43xx_ANTENNA_AUTO0,
	BCM43xx_ANTENNA_DEFAULT = BCM43xx_ANTENNA_AUTO,
};

enum {
	BCM43xx_INTERFMODE_NONE,
	BCM43xx_INTERFMODE_NONWLAN,
	BCM43xx_INTERFMODE_MANUALWLAN,
	BCM43xx_INTERFMODE_AUTOWLAN,
};


/* Masks for the different PHY versioning registers. */
#define BCM43xx_PHYVER_ANALOG		0xF000
#define BCM43xx_PHYVER_ANALOG_SHIFT	12
#define BCM43xx_PHYVER_TYPE		0x0F00
#define BCM43xx_PHYVER_TYPE_SHIFT	8
#define BCM43xx_PHYVER_VERSION		0x00FF


void bcm43xx_raw_phy_lock(struct bcm43xx_wldev *dev);
#define bcm43xx_phy_lock(dev, flags) \
	do {					\
		local_irq_save(flags);		\
		bcm43xx_raw_phy_lock(dev);	\
	} while (0)
void bcm43xx_raw_phy_unlock(struct bcm43xx_wldev *dev);
#define bcm43xx_phy_unlock(dev, flags) \
	do {					\
		bcm43xx_raw_phy_unlock(dev);	\
		local_irq_restore(flags);	\
	} while (0)

u16 bcm43xx_phy_read(struct bcm43xx_wldev *dev, u16 offset);
void bcm43xx_phy_write(struct bcm43xx_wldev *dev, u16 offset, u16 val);

int bcm43xx_phy_init_tssi2dbm_table(struct bcm43xx_wldev *dev);

void bcm43xx_phy_early_init(struct bcm43xx_wldev *dev);
int bcm43xx_phy_init(struct bcm43xx_wldev *dev);

void bcm43xx_set_rx_antenna(struct bcm43xx_wldev *dev, int antenna);

void bcm43xx_phy_xmitpower(struct bcm43xx_wldev *dev);
void bcm43xx_gphy_dc_lt_init(struct bcm43xx_wldev *dev);

/* Returns the boolean whether the board has HardwarePowerControl */
#define has_hardware_pctl(phy) \
	(((phy)->type == BCM43xx_PHYTYPE_A && (phy)->rev >= 5) ||	\
	 ((phy)->type == BCM43xx_PHYTYPE_G && (phy)->rev >= 6))
/* Returns the boolean whether "TX Magnification" is enabled. */
#define has_tx_magnification(phy) \
	(((phy)->rev >= 2) &&			\
	 ((phy)->radio_ver == 0x2050) &&	\
	 ((phy)->radio_rev == 8))
/* Card uses the loopback gain stuff */
#define has_loopback_gain(phy) \
	(((phy)->rev > 1) || ((phy)->gmode))

/* Radio Attenuation (RF Attenuation) */
struct bcm43xx_rfatt {
	u8 att;		/* Attenuation value */
	u8 with_padmix;	/* Flag, PAD Mixer enabled. */
};
struct bcm43xx_rfatt_list {
	/* Attenuation values list */
	const struct bcm43xx_rfatt *list;
	u8 len;
	/* Minimum/Maximum attenuation values */
	u8 min_val;
	u8 max_val;
};

/* Baseband Attenuation */
struct bcm43xx_bbatt {
	u8 att;		/* Attenuation value */
};
struct bcm43xx_bbatt_list {
	/* Attenuation values list */
	const struct bcm43xx_bbatt *list;
	u8 len;
	/* Minimum/Maximum attenuation values */
	u8 min_val;
	u8 max_val;
};

/* tx_control bits. */
#define BCM43xx_TXCTL_PA3DB	0x40 /* PA Gain 3dB */
#define BCM43xx_TXCTL_PA2DB	0x20 /* PA Gain 2dB */
#define BCM43xx_TXCTL_TXMIX	0x10 /* TX Mixer Gain */

/* Write BasebandAttenuation value to the device. */
void bcm43xx_phy_set_baseband_attenuation(struct bcm43xx_wldev *dev,
					  u16 baseband_attenuation);


extern const u8 bcm43xx_radio_channel_codes_bg[];

void bcm43xx_radio_lock(struct bcm43xx_wldev *dev);
void bcm43xx_radio_unlock(struct bcm43xx_wldev *dev);

u16 bcm43xx_radio_read16(struct bcm43xx_wldev *dev, u16 offset);
void bcm43xx_radio_write16(struct bcm43xx_wldev *dev, u16 offset, u16 val);

u16 bcm43xx_radio_init2050(struct bcm43xx_wldev *dev);
void bcm43xx_radio_init2060(struct bcm43xx_wldev *dev);

void bcm43xx_radio_turn_on(struct bcm43xx_wldev *dev);
void bcm43xx_radio_turn_off(struct bcm43xx_wldev *dev);

int bcm43xx_radio_selectchannel(struct bcm43xx_wldev *dev, u8 channel,
				int synthetic_pu_workaround);

u8 bcm43xx_radio_aci_detect(struct bcm43xx_wldev *dev, u8 channel);
u8 bcm43xx_radio_aci_scan(struct bcm43xx_wldev *dev);

int bcm43xx_radio_set_interference_mitigation(struct bcm43xx_wldev *dev, int mode);

void bcm43xx_calc_nrssi_slope(struct bcm43xx_wldev *dev);
void bcm43xx_calc_nrssi_threshold(struct bcm43xx_wldev *dev);
s16 bcm43xx_nrssi_hw_read(struct bcm43xx_wldev *dev, u16 offset);
void bcm43xx_nrssi_hw_write(struct bcm43xx_wldev *dev, u16 offset, s16 val);
void bcm43xx_nrssi_hw_update(struct bcm43xx_wldev *dev, u16 val);
void bcm43xx_nrssi_mem_update(struct bcm43xx_wldev *dev);

void bcm43xx_radio_set_tx_iq(struct bcm43xx_wldev *dev);
u16 bcm43xx_radio_calibrationvalue(struct bcm43xx_wldev *dev);

void bcm43xx_put_attenuation_into_ranges(struct bcm43xx_wldev *dev,
					 int *_bbatt, int *_rfatt);

void bcm43xx_set_txpower_g(struct bcm43xx_wldev *dev,
			   const struct bcm43xx_bbatt *bbatt,
			   const struct bcm43xx_rfatt *rfatt,
			   u8 tx_control);

#endif /* BCM43xx_PHY_H_ */
