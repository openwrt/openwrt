#ifndef BCM43xx_H_
#define BCM43xx_H_

#include <linux/hw_random.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/stringify.h>
#include <linux/netdevice.h>
#include <linux/pci.h>
#include <asm/atomic.h>
#include <asm/io.h>

#include <linux/ssb/ssb.h>
#include <linux/ssb/ssb_driver_chipcommon.h>

#include <linux/wireless.h>
#include <net/mac80211.h>

#include "bcm43xx_debugfs.h"
#include "bcm43xx_leds.h"
#include "bcm43xx_lo.h"
#include "bcm43xx_phy.h"


#define PFX				KBUILD_MODNAME ": "

#define BCM43xx_IRQWAIT_MAX_RETRIES	50

#define BCM43xx_IO_SIZE			8192

#define BCM43xx_RX_MAX_SSI		60

/* MMIO offsets */
#define BCM43xx_MMIO_DMA0_REASON	0x20
#define BCM43xx_MMIO_DMA0_IRQ_MASK	0x24
#define BCM43xx_MMIO_DMA1_REASON	0x28
#define BCM43xx_MMIO_DMA1_IRQ_MASK	0x2C
#define BCM43xx_MMIO_DMA2_REASON	0x30
#define BCM43xx_MMIO_DMA2_IRQ_MASK	0x34
#define BCM43xx_MMIO_DMA3_REASON	0x38
#define BCM43xx_MMIO_DMA3_IRQ_MASK	0x3C
#define BCM43xx_MMIO_DMA4_REASON	0x40
#define BCM43xx_MMIO_DMA4_IRQ_MASK	0x44
#define BCM43xx_MMIO_DMA5_REASON	0x48
#define BCM43xx_MMIO_DMA5_IRQ_MASK	0x4C
#define BCM43xx_MMIO_MACCTL		0x120
#define BCM43xx_MMIO_STATUS_BITFIELD	0x120//TODO replace all instances by MACCTL
#define BCM43xx_MMIO_STATUS2_BITFIELD	0x124
#define BCM43xx_MMIO_GEN_IRQ_REASON	0x128
#define BCM43xx_MMIO_GEN_IRQ_MASK	0x12C
#define BCM43xx_MMIO_RAM_CONTROL	0x130
#define BCM43xx_MMIO_RAM_DATA		0x134
#define BCM43xx_MMIO_PS_STATUS		0x140
#define BCM43xx_MMIO_RADIO_HWENABLED_HI	0x158
#define BCM43xx_MMIO_SHM_CONTROL	0x160
#define BCM43xx_MMIO_SHM_DATA		0x164
#define BCM43xx_MMIO_SHM_DATA_UNALIGNED	0x166
#define BCM43xx_MMIO_XMITSTAT_0		0x170
#define BCM43xx_MMIO_XMITSTAT_1		0x174
#define BCM43xx_MMIO_REV3PLUS_TSF_LOW	0x180 /* core rev >= 3 only */
#define BCM43xx_MMIO_REV3PLUS_TSF_HIGH	0x184 /* core rev >= 3 only */

/* 32-bit DMA */
#define BCM43xx_MMIO_DMA32_BASE0	0x200
#define BCM43xx_MMIO_DMA32_BASE1	0x220
#define BCM43xx_MMIO_DMA32_BASE2	0x240
#define BCM43xx_MMIO_DMA32_BASE3	0x260
#define BCM43xx_MMIO_DMA32_BASE4	0x280
#define BCM43xx_MMIO_DMA32_BASE5	0x2A0
/* 64-bit DMA */
#define BCM43xx_MMIO_DMA64_BASE0	0x200
#define BCM43xx_MMIO_DMA64_BASE1	0x240
#define BCM43xx_MMIO_DMA64_BASE2	0x280
#define BCM43xx_MMIO_DMA64_BASE3	0x2C0
#define BCM43xx_MMIO_DMA64_BASE4	0x300
#define BCM43xx_MMIO_DMA64_BASE5	0x340
/* PIO */
#define BCM43xx_MMIO_PIO1_BASE		0x300
#define BCM43xx_MMIO_PIO2_BASE		0x310
#define BCM43xx_MMIO_PIO3_BASE		0x320
#define BCM43xx_MMIO_PIO4_BASE		0x330

#define BCM43xx_MMIO_PHY_VER		0x3E0
#define BCM43xx_MMIO_PHY_RADIO		0x3E2
#define BCM43xx_MMIO_PHY0		0x3E6
#define BCM43xx_MMIO_ANTENNA		0x3E8
#define BCM43xx_MMIO_CHANNEL		0x3F0
#define BCM43xx_MMIO_CHANNEL_EXT	0x3F4
#define BCM43xx_MMIO_RADIO_CONTROL	0x3F6
#define BCM43xx_MMIO_RADIO_DATA_HIGH	0x3F8
#define BCM43xx_MMIO_RADIO_DATA_LOW	0x3FA
#define BCM43xx_MMIO_PHY_CONTROL	0x3FC
#define BCM43xx_MMIO_PHY_DATA		0x3FE
#define BCM43xx_MMIO_MACFILTER_CONTROL	0x420
#define BCM43xx_MMIO_MACFILTER_DATA	0x422
#define BCM43xx_MMIO_RCMTA_COUNT	0x43C
#define BCM43xx_MMIO_RADIO_HWENABLED_LO	0x49A
#define BCM43xx_MMIO_GPIO_CONTROL	0x49C
#define BCM43xx_MMIO_GPIO_MASK		0x49E
#define BCM43xx_MMIO_TSF_0		0x632 /* core rev < 3 only */
#define BCM43xx_MMIO_TSF_1		0x634 /* core rev < 3 only */
#define BCM43xx_MMIO_TSF_2		0x636 /* core rev < 3 only */
#define BCM43xx_MMIO_TSF_3		0x638 /* core rev < 3 only */
#define BCM43xx_MMIO_RNG		0x65A
#define BCM43xx_MMIO_POWERUP_DELAY	0x6A8

/* SPROM boardflags_lo values */
#define BCM43xx_BFL_BTCOEXIST		0x0001 /* implements Bluetooth coexistance */
#define BCM43xx_BFL_PACTRL		0x0002 /* GPIO 9 controlling the PA */
#define BCM43xx_BFL_AIRLINEMODE		0x0004 /* implements GPIO 13 radio disable indication */
#define BCM43xx_BFL_RSSI		0x0008 /* software calculates nrssi slope. */
#define BCM43xx_BFL_ENETSPI		0x0010 /* has ephy roboswitch spi */
#define BCM43xx_BFL_XTAL_NOSLOW		0x0020 /* no slow clock available */
#define BCM43xx_BFL_CCKHIPWR		0x0040 /* can do high power CCK transmission */
#define BCM43xx_BFL_ENETADM		0x0080 /* has ADMtek switch */
#define BCM43xx_BFL_ENETVLAN		0x0100 /* can do vlan */
#define BCM43xx_BFL_AFTERBURNER		0x0200 /* supports Afterburner mode */
#define BCM43xx_BFL_NOPCI		0x0400 /* leaves PCI floating */
#define BCM43xx_BFL_FEM			0x0800 /* supports the Front End Module */
#define BCM43xx_BFL_EXTLNA		0x1000 /* has an external LNA */
#define BCM43xx_BFL_HGPA		0x2000 /* had high gain PA */
#define BCM43xx_BFL_BTCMOD		0x4000 /* BFL_BTCOEXIST is given in alternate GPIOs */
#define BCM43xx_BFL_ALTIQ		0x8000 /* alternate I/Q settings */

/* GPIO register offset, in both ChipCommon and PCI core. */
#define BCM43xx_GPIO_CONTROL		0x6c

/* SHM Routing */
enum {
	BCM43xx_SHM_UCODE,	/* Microcode memory */
	BCM43xx_SHM_SHARED,	/* Shared memory */
	BCM43xx_SHM_SCRATCH,	/* Scratch memory */
	BCM43xx_SHM_HW,		/* Internal hardware register */
	BCM43xx_SHM_RCMTA,	/* Receive match transmitter address (rev >= 5 only) */
};
/* SHM Routing modifiers */
#define BCM43xx_SHM_AUTOINC_R		0x0200 /* Auto-increment address on read */
#define BCM43xx_SHM_AUTOINC_W		0x0100 /* Auto-increment address on write */
#define BCM43xx_SHM_AUTOINC_RW		(BCM43xx_SHM_AUTOINC_R | \
					 BCM43xx_SHM_AUTOINC_W)

/* Misc SHM_SHARED offsets */
#define BCM43xx_SHM_SH_WLCOREREV	0x0016 /* 802.11 core revision */
#define BCM43xx_SHM_SH_PCTLWDPOS	0x0008
#define BCM43xx_SHM_SH_RXPADOFF		0x0034 /* RX Padding data offset (PIO only) */
#define BCM43xx_SHM_SH_PHYVER		0x0050 /* PHY version */
#define BCM43xx_SHM_SH_PHYTYPE		0x0052 /* PHY type */
#define BCM43xx_SHM_SH_ANTSWAP		0x005C /* Antenna swap threshold */
#define BCM43xx_SHM_SH_HOSTFLO		0x005E /* Hostflags for ucode options (low) */
#define BCM43xx_SHM_SH_HOSTFHI		0x0060 /* Hostflags for ucode options (high) */
#define BCM43xx_SHM_SH_RFATT		0x0064 /* Current radio attenuation value */
#define BCM43xx_SHM_SH_RADAR		0x0066 /* Radar register */
#define BCM43xx_SHM_SH_PHYTXNOI		0x006E /* PHY noise directly after TX (lower 8bit only) */
#define BCM43xx_SHM_SH_RFRXSP1		0x0072 /* RF RX SP Register 1 */
#define BCM43xx_SHM_SH_CHAN		0x00A0 /* Current channel (low 8bit only) */
#define  BCM43xx_SHM_SH_CHAN_5GHZ	0x0100 /* Bit set, if 5Ghz channel */
#define BCM43xx_SHM_SH_BCMCFIFOID	0x0108 /* Last posted cookie to the bcast/mcast FIFO */
/* SHM_SHARED TX FIFO variables */
#define BCM43xx_SHM_SH_SIZE01		0x0098 /* TX FIFO size for FIFO 0 (low) and 1 (high) */
#define BCM43xx_SHM_SH_SIZE23		0x009A /* TX FIFO size for FIFO 2 and 3 */
#define BCM43xx_SHM_SH_SIZE45		0x009C /* TX FIFO size for FIFO 4 and 5 */
#define BCM43xx_SHM_SH_SIZE67		0x009E /* TX FIFO size for FIFO 6 and 7 */
/* SHM_SHARED background noise */
#define BCM43xx_SHM_SH_JSSI0		0x0088 /* Measure JSSI 0 */
#define BCM43xx_SHM_SH_JSSI1		0x008A /* Measure JSSI 1 */
#define BCM43xx_SHM_SH_JSSIAUX		0x008C /* Measure JSSI AUX */
/* SHM_SHARED crypto engine */
#define BCM43xx_SHM_SH_DEFAULTIV	0x003C /* Default IV location */
#define BCM43xx_SHM_SH_NRRXTRANS	0x003E /* # of soft RX transmitter addresses (max 8) */
#define BCM43xx_SHM_SH_KTP		0x0056 /* Key table pointer */
#define BCM43xx_SHM_SH_TKIPTSCTTAK	0x0318
#define BCM43xx_SHM_SH_KEYIDXBLOCK	0x05D4 /* Key index/algorithm block (v4 firmware) */
#define BCM43xx_SHM_SH_PSM		0x05F4 /* PSM transmitter address match block (rev < 5) */
/* SHM_SHARED WME variables */
#define BCM43xx_SHM_SH_EDCFSTAT		0x000E /* EDCF status */
#define BCM43xx_SHM_SH_TXFCUR		0x0030 /* TXF current index */
#define BCM43xx_SHM_SH_EDCFQ		0x0240 /* EDCF Q info */
/* SHM_SHARED powersave mode related */
#define BCM43xx_SHM_SH_SLOTT		0x0010 /* Slot time */
#define BCM43xx_SHM_SH_DTIMPER		0x0012 /* DTIM period */
#define BCM43xx_SHM_SH_NOSLPZNATDTIM	0x004C /* NOSLPZNAT DTIM */
/* SHM_SHARED beacon variables */
#define BCM43xx_SHM_SH_BTL0		0x0018 /* Beacon template length 0 */
#define BCM43xx_SHM_SH_BTL1		0x001A /* Beacon template length 1 */
#define BCM43xx_SHM_SH_BTSFOFF		0x001C /* Beacon TSF offset */
#define BCM43xx_SHM_SH_TIMBPOS		0x001E /* TIM B position in beacon */
#define BCM43xx_SHM_SH_SFFBLIM		0x0044 /* Short frame fallback retry limit */
#define BCM43xx_SHM_SH_LFFBLIM		0x0046 /* Long frame fallback retry limit */
#define BCM43xx_SHM_SH_BEACPHYCTL	0x0054 /* Beacon PHY TX control word (see PHY TX control) */
/* SHM_SHARED ACK/CTS control */
#define BCM43xx_SHM_SH_ACKCTSPHYCTL	0x0022 /* ACK/CTS PHY control word (see PHY TX control) */
/* SHM_SHARED probe response variables */
#define BCM43xx_SHM_SH_PRSSID		0x0160 /* Probe Response SSID */
#define BCM43xx_SHM_SH_PRSSIDLEN	0x0048 /* Probe Response SSID length */
#define BCM43xx_SHM_SH_PRTLEN		0x004A /* Probe Response template length */
#define BCM43xx_SHM_SH_PRMAXTIME	0x0074 /* Probe Response max time */
#define BCM43xx_SHM_SH_PRPHYCTL		0x0188 /* Probe Response PHY TX control word */
/* SHM_SHARED rate tables */
#define BCM43xx_SHM_SH_OFDMDIRECT	0x01C0 /* Pointer to OFDM direct map */
#define BCM43xx_SHM_SH_OFDMBASIC	0x01E0 /* Pointer to OFDM basic rate map */
#define BCM43xx_SHM_SH_CCKDIRECT	0x0200 /* Pointer to CCK direct map */
#define BCM43xx_SHM_SH_CCKBASIC		0x0220 /* Pointer to CCK basic rate map */
/* SHM_SHARED microcode soft registers */
#define BCM43xx_SHM_SH_UCODEREV		0x0000 /* Microcode revision */
#define BCM43xx_SHM_SH_UCODEPATCH	0x0002 /* Microcode patchlevel */
#define BCM43xx_SHM_SH_UCODEDATE	0x0004 /* Microcode date */
#define BCM43xx_SHM_SH_UCODETIME	0x0006 /* Microcode time */
#define BCM43xx_SHM_SH_UCODESTAT	0x0040 /* Microcode debug status code */
#define  BCM43xx_SHM_SH_UCODESTAT_INVALID	0
#define  BCM43xx_SHM_SH_UCODESTAT_INIT		1
#define  BCM43xx_SHM_SH_UCODESTAT_ACTIVE	2
#define  BCM43xx_SHM_SH_UCODESTAT_SUSP		3 /* suspended */
#define  BCM43xx_SHM_SH_UCODESTAT_SLEEP		4 /* asleep (PS) */
#define BCM43xx_SHM_SH_MAXBFRAMES	0x0080 /* Maximum number of frames in a burst */
#define BCM43xx_SHM_SH_SPUWKUP		0x0094 /* pre-wakeup for synth PU in us */
#define BCM43xx_SHM_SH_PRETBTT		0x0096 /* pre-TBTT in us */

/* SHM_SCRATCH offsets */
#define BCM43xx_SHM_SC_MINCONT		0x0003 /* Minimum contention window */
#define BCM43xx_SHM_SC_MAXCONT		0x0004 /* Maximum contention window */
#define BCM43xx_SHM_SC_CURCONT		0x0005 /* Current contention window */
#define BCM43xx_SHM_SC_SRLIMIT		0x0006 /* Short retry count limit */
#define BCM43xx_SHM_SC_LRLIMIT		0x0007 /* Long retry count limit */
#define BCM43xx_SHM_SC_DTIMC		0x0008 /* Current DTIM count */
#define BCM43xx_SHM_SC_BTL0LEN		0x0015 /* Beacon 0 template length */
#define BCM43xx_SHM_SC_BTL1LEN		0x0016 /* Beacon 1 template length */
#define BCM43xx_SHM_SC_SCFB		0x0017 /* Short frame transmit count threshold for rate fallback */
#define BCM43xx_SHM_SC_LCFB		0x0018 /* Long frame transmit count threshold for rate fallback */


/* Hardware Radio Enable masks */
#define BCM43xx_MMIO_RADIO_HWENABLED_HI_MASK (1 << 16)
#define BCM43xx_MMIO_RADIO_HWENABLED_LO_MASK (1 << 4)

/* HostFlags. See bcm43xx_hf_read/write() */
#define BCM43xx_HF_ANTDIVHELP		0x00000001 /* ucode antenna div helper */
#define BCM43xx_HF_SYMW			0x00000002 /* G-PHY SYM workaround */
#define BCM43xx_HF_RXPULLW		0x00000004 /* RX pullup workaround */
#define BCM43xx_HF_CCKBOOST		0x00000008 /* 4dB CCK power boost (exclusive with OFDM boost) */
#define BCM43xx_HF_BTCOEX		0x00000010 /* Bluetooth coexistance */
#define BCM43xx_HF_GDCW			0x00000020 /* G-PHY DV canceller filter bw workaround */
#define BCM43xx_HF_OFDMPABOOST		0x00000040 /* Enable PA gain boost for OFDM */
#define BCM43xx_HF_ACPR			0x00000080 /* Disable for Japan, channel 14 */
#define BCM43xx_HF_EDCF			0x00000100 /* on if WME and MAC suspended */
#define BCM43xx_HF_TSSIRPSMW		0x00000200 /* TSSI reset PSM ucode workaround */
#define BCM43xx_HF_DSCRQ		0x00000400 /* Disable slow clock request in ucode */
#define BCM43xx_HF_ACIW			0x00000800 /* ACI workaround: shift bits by 2 on PHY CRS */
#define BCM43xx_HF_2060W		0x00001000 /* 2060 radio workaround */
#define BCM43xx_HF_RADARW		0x00002000 /* Radar workaround */
#define BCM43xx_HF_USEDEFKEYS		0x00004000 /* Enable use of default keys */
#define BCM43xx_HF_BT4PRIOCOEX		0x00010000 /* Bluetooth 2-priority coexistance */
#define BCM43xx_HF_FWKUP		0x00020000 /* Fast wake-up ucode */
#define BCM43xx_HF_VCORECALC		0x00040000 /* Force VCO recalculation when powering up synthpu */
#define BCM43xx_HF_PCISCW		0x00080000 /* PCI slow clock workaround */
#define BCM43xx_HF_4318TSSI		0x00200000 /* 4318 TSSI */
#define BCM43xx_HF_FBCMCFIFO		0x00400000 /* Flush bcast/mcast FIFO immediately */
#define BCM43xx_HF_HWPCTL		0x00800000 /* Enable hardwarre power control */
#define BCM43xx_HF_BTCOEXALT		0x01000000 /* Bluetooth coexistance in alternate pins */
#define BCM43xx_HF_TXBTCHECK		0x02000000 /* Bluetooth check during transmission */
#define BCM43xx_HF_SKCFPUP		0x04000000 /* Skip CFP update */


/* MacFilter offsets. */
#define BCM43xx_MACFILTER_SELF		0x0000
#define BCM43xx_MACFILTER_BSSID		0x0003

/* PowerControl */
#define BCM43xx_PCTL_IN			0xB0
#define BCM43xx_PCTL_OUT		0xB4
#define BCM43xx_PCTL_OUTENABLE		0xB8
#define BCM43xx_PCTL_XTAL_POWERUP	0x40
#define BCM43xx_PCTL_PLL_POWERDOWN	0x80

/* PowerControl Clock Modes */
#define BCM43xx_PCTL_CLK_FAST		0x00
#define BCM43xx_PCTL_CLK_SLOW		0x01
#define BCM43xx_PCTL_CLK_DYNAMIC	0x02

#define BCM43xx_PCTL_FORCE_SLOW		0x0800
#define BCM43xx_PCTL_FORCE_PLL		0x1000
#define BCM43xx_PCTL_DYN_XTAL		0x2000

/* PHYVersioning */
#define BCM43xx_PHYTYPE_A		0x00
#define BCM43xx_PHYTYPE_B		0x01
#define BCM43xx_PHYTYPE_G		0x02

/* PHYRegisters */
#define BCM43xx_PHY_ILT_A_CTRL		0x0072
#define BCM43xx_PHY_ILT_A_DATA1		0x0073
#define BCM43xx_PHY_ILT_A_DATA2		0x0074
#define BCM43xx_PHY_G_LO_CONTROL	0x0810
#define BCM43xx_PHY_ILT_G_CTRL		0x0472
#define BCM43xx_PHY_ILT_G_DATA1		0x0473
#define BCM43xx_PHY_ILT_G_DATA2		0x0474
#define BCM43xx_PHY_A_PCTL		0x007B
#define BCM43xx_PHY_G_PCTL		0x0029
#define BCM43xx_PHY_A_CRS		0x0029
#define BCM43xx_PHY_RADIO_BITFIELD	0x0401
#define BCM43xx_PHY_G_CRS		0x0429
#define BCM43xx_PHY_NRSSILT_CTRL	0x0803
#define BCM43xx_PHY_NRSSILT_DATA	0x0804

/* RadioRegisters */
#define BCM43xx_RADIOCTL_ID		0x01

/* MAC Control bitfield */
#define BCM43xx_MACCTL_ENABLED		0x00000001 /* MAC Enabled */
#define BCM43xx_MACCTL_PSM_RUN		0x00000002 /* Run Microcode */
#define BCM43xx_MACCTL_PSM_JMP0		0x00000004 /* Microcode jump to 0 */
#define BCM43xx_MACCTL_SHM_ENABLED	0x00000100 /* SHM Enabled */
#define BCM43xx_MACCTL_SHM_UPPER	0x00000200 /* SHM Upper */
#define BCM43xx_MACCTL_IHR_ENABLED	0x00000400 /* IHR Region Enabled */
#define BCM43xx_MACCTL_PSM_DBG		0x00002000 /* Microcode debugging enabled */
#define BCM43xx_MACCTL_GPOUTSMSK	0x0000C000 /* GPOUT Select Mask */
#define BCM43xx_MACCTL_BE		0x00010000 /* Big Endian mode */
#define BCM43xx_MACCTL_INFRA		0x00020000 /* Infrastructure mode */
#define BCM43xx_MACCTL_AP		0x00040000 /* AccessPoint mode */
#define BCM43xx_MACCTL_RADIOLOCK	0x00080000 /* Radio lock */
#define BCM43xx_MACCTL_BEACPROMISC	0x00100000 /* Beacon Promiscuous */
#define BCM43xx_MACCTL_KEEP_BADPLCP	0x00200000 /* Keep frames with bad PLCP */
#define BCM43xx_MACCTL_KEEP_CTL		0x00400000 /* Keep control frames */
#define BCM43xx_MACCTL_KEEP_BAD		0x00800000 /* Keep bad frames (FCS) */
#define BCM43xx_MACCTL_PROMISC		0x01000000 /* Promiscuous mode */
#define BCM43xx_MACCTL_HWPS		0x02000000 /* Hardware Power Saving */
#define BCM43xx_MACCTL_AWAKE		0x04000000 /* Device is awake */
#define BCM43xx_MACCTL_CLOSEDNET	0x08000000 /* Closed net (no SSID bcast) */
#define BCM43xx_MACCTL_TBTTHOLD		0x10000000 /* TBTT Hold */
#define BCM43xx_MACCTL_DISCTXSTAT	0x20000000 /* Discard TX status */
#define BCM43xx_MACCTL_DISCPMQ		0x40000000 /* Discard Power Management Queue */
#define BCM43xx_MACCTL_GMODE		0x80000000 /* G Mode */

/* StatusBitField *///FIXME rename these all
#define BCM43xx_SBF_MAC_ENABLED		0x00000001
#define BCM43xx_SBF_2			0x00000002 /*FIXME: fix name*/
#define BCM43xx_SBF_CORE_READY		0x00000004
#define BCM43xx_SBF_400			0x00000400 /*FIXME: fix name*/
#define BCM43xx_SBF_4000		0x00004000 /*FIXME: fix name*/
#define BCM43xx_SBF_8000		0x00008000 /*FIXME: fix name*/
#define BCM43xx_SBF_XFER_REG_BYTESWAP	0x00010000
#define BCM43xx_SBF_MODE_NOTADHOC	0x00020000
#define BCM43xx_SBF_MODE_AP		0x00040000
#define BCM43xx_SBF_RADIOREG_LOCK	0x00080000
#define BCM43xx_SBF_MODE_MONITOR	0x00400000
#define BCM43xx_SBF_MODE_PROMISC	0x01000000
#define BCM43xx_SBF_PS1			0x02000000
#define BCM43xx_SBF_PS2			0x04000000
#define BCM43xx_SBF_NO_SSID_BCAST	0x08000000
#define BCM43xx_SBF_TIME_UPDATE		0x10000000
#define BCM43xx_SBF_80000000		0x80000000 /*FIXME: fix name*/

/* 802.11 core specific TM State Low flags */
#define BCM43xx_TMSLOW_GMODE		0x20000000 /* G Mode Enable */
#define BCM43xx_TMSLOW_PLLREFSEL	0x00200000 /* PLL Frequency Reference Select */
#define BCM43xx_TMSLOW_MACPHYCLKEN	0x00100000 /* MAC PHY Clock Control Enable (rev >= 5) */
#define BCM43xx_TMSLOW_PHYRESET		0x00080000 /* PHY Reset */
#define BCM43xx_TMSLOW_PHYCLKEN		0x00040000 /* PHY Clock Enable */

/* 802.11 core specific TM State High flags */
#define BCM43xx_TMSHIGH_FCLOCK		0x00040000 /* Fast Clock Available (rev >= 5)*/
#define BCM43xx_TMSHIGH_APHY		0x00020000 /* A-PHY available (rev >= 5) */
#define BCM43xx_TMSHIGH_GPHY		0x00010000 /* G-PHY available (rev >= 5) */

/* Generic-Interrupt reasons. */
#define BCM43xx_IRQ_MAC_SUSPENDED	0x00000001
#define BCM43xx_IRQ_BEACON		0x00000002
#define BCM43xx_IRQ_TBTT_INDI		0x00000004
#define BCM43xx_IRQ_BEACON_TX_OK	0x00000008
#define BCM43xx_IRQ_BEACON_CANCEL	0x00000010
#define BCM43xx_IRQ_ATIM_END		0x00000020
#define BCM43xx_IRQ_PMQ			0x00000040
#define BCM43xx_IRQ_PIO_WORKAROUND	0x00000100
#define BCM43xx_IRQ_MAC_TXERR		0x00000200
#define BCM43xx_IRQ_PHY_TXERR		0x00000800
#define BCM43xx_IRQ_PMEVENT		0x00001000
#define BCM43xx_IRQ_TIMER0		0x00002000
#define BCM43xx_IRQ_TIMER1		0x00004000
#define BCM43xx_IRQ_DMA			0x00008000
#define BCM43xx_IRQ_TXFIFO_FLUSH_OK	0x00010000
#define BCM43xx_IRQ_CCA_MEASURE_OK	0x00020000
#define BCM43xx_IRQ_NOISESAMPLE_OK	0x00040000
#define BCM43xx_IRQ_UCODE_DEBUG		0x08000000
#define BCM43xx_IRQ_RFKILL		0x10000000
#define BCM43xx_IRQ_TX_OK		0x20000000
#define BCM43xx_IRQ_PHY_G_CHANGED	0x40000000
#define BCM43xx_IRQ_TIMEOUT		0x80000000

#define BCM43xx_IRQ_ALL			0xFFFFFFFF
#define BCM43xx_IRQ_MASKTEMPLATE	(BCM43xx_IRQ_MAC_SUSPENDED |	\
					 BCM43xx_IRQ_BEACON |		\
					 BCM43xx_IRQ_TBTT_INDI |	\
					 BCM43xx_IRQ_ATIM_END |		\
					 BCM43xx_IRQ_PMQ |		\
					 BCM43xx_IRQ_MAC_TXERR |	\
					 BCM43xx_IRQ_PHY_TXERR |	\
					 BCM43xx_IRQ_DMA |		\
					 BCM43xx_IRQ_TXFIFO_FLUSH_OK |	\
					 BCM43xx_IRQ_NOISESAMPLE_OK |	\
					 BCM43xx_IRQ_UCODE_DEBUG |	\
					 BCM43xx_IRQ_RFKILL |		\
					 BCM43xx_IRQ_TX_OK)

/* Device specific rate values.
 * The actual values defined here are (rate_in_mbps * 2).
 * Some code depends on this. Don't change it. */
#define BCM43xx_CCK_RATE_1MB		0x02
#define BCM43xx_CCK_RATE_2MB		0x04
#define BCM43xx_CCK_RATE_5MB		0x0B
#define BCM43xx_CCK_RATE_11MB		0x16
#define BCM43xx_OFDM_RATE_6MB		0x0C
#define BCM43xx_OFDM_RATE_9MB		0x12
#define BCM43xx_OFDM_RATE_12MB		0x18
#define BCM43xx_OFDM_RATE_18MB		0x24
#define BCM43xx_OFDM_RATE_24MB		0x30
#define BCM43xx_OFDM_RATE_36MB		0x48
#define BCM43xx_OFDM_RATE_48MB		0x60
#define BCM43xx_OFDM_RATE_54MB		0x6C
/* Convert a bcm43xx rate value to a rate in 100kbps */
#define BCM43xx_RATE_TO_BASE100KBPS(rate)	(((rate) * 10) / 2)


#define BCM43xx_DEFAULT_SHORT_RETRY_LIMIT	7
#define BCM43xx_DEFAULT_LONG_RETRY_LIMIT	4

/* Max size of a security key */
#define BCM43xx_SEC_KEYSIZE			16
/* Security algorithms. */
enum {
	BCM43xx_SEC_ALGO_NONE = 0, /* unencrypted, as of TX header. */
	BCM43xx_SEC_ALGO_WEP40,
	BCM43xx_SEC_ALGO_TKIP,
	BCM43xx_SEC_ALGO_AES,
	BCM43xx_SEC_ALGO_WEP104,
	BCM43xx_SEC_ALGO_AES_LEGACY,
};


#ifdef assert
# undef assert
#endif
#ifdef CONFIG_BCM43XX_MAC80211_DEBUG
# define assert(expr) \
	do {									\
		if (unlikely(!(expr))) {					\
		printk(KERN_ERR PFX "ASSERTION FAILED (%s) at: %s:%d:%s()\n",	\
			#expr, __FILE__, __LINE__, __FUNCTION__);		\
		}								\
	} while (0)
# define BCM43xx_DEBUG	1
#else
# define assert(expr)	do { /* nothing */ } while (0)
# define BCM43xx_DEBUG	0
#endif

/* rate limited printk(). */
#ifdef printkl
# undef printkl
#endif
#define printkl(f, x...)  do { if (printk_ratelimit()) printk(f ,##x); } while (0)
/* rate limited printk() for debugging */
#ifdef dprintkl
# undef dprintkl
#endif
#ifdef CONFIG_BCM43XX_MAC80211_DEBUG
# define dprintkl		printkl
#else
# define dprintkl(f, x...)	do { /* nothing */ } while (0)
#endif

/* debugging printk() */
#ifdef dprintk
# undef dprintk
#endif
#ifdef CONFIG_BCM43XX_MAC80211_DEBUG
# define dprintk(f, x...)  do { printk(f ,##x); } while (0)
#else
# define dprintk(f, x...)  do { /* nothing */ } while (0)
#endif


struct net_device;
struct pci_dev;
struct bcm43xx_dmaring;
struct bcm43xx_pioqueue;

struct bcm43xx_initval {
	u16 offset;
	u16 size;
	u32 value;
} __attribute__((__packed__));

#define BCM43xx_PHYMODE(phytype)	(1 << (phytype))
#define BCM43xx_PHYMODE_A		BCM43xx_PHYMODE(BCM43xx_PHYTYPE_A)
#define BCM43xx_PHYMODE_B		BCM43xx_PHYMODE(BCM43xx_PHYTYPE_B)
#define BCM43xx_PHYMODE_G		BCM43xx_PHYMODE(BCM43xx_PHYTYPE_G)

struct bcm43xx_phy {
	/* Possible PHYMODEs on this PHY */
	u8 possible_phymodes;
	/* GMODE bit enabled? */
	u8 gmode;
	/* Possible ieee80211 subsystem hwmodes for this PHY.
	 * Which mode is selected, depends on thr GMODE enabled bit */
#define BCM43xx_MAX_PHYHWMODES	2
	struct ieee80211_hw_mode hwmodes[BCM43xx_MAX_PHYHWMODES];

	/* Analog Type */
	u8 analog;
	/* BCM43xx_PHYTYPE_ */
	u8 type;
	/* PHY revision number. */
	u8 rev;

	/* Radio versioning */
	u16 radio_manuf;	/* Radio manufacturer */
	u16 radio_ver;		/* Radio version */
	u8 radio_rev;		/* Radio revision */

	u8 radio_on:1;		/* Radio switched on/off */
	u8 locked:1;		/* Only used in bcm43xx_phy_{un}lock() */
	u8 dyn_tssi_tbl:1;	/* tssi2dbm is kmalloc()ed. */

	/* ACI (adjacent channel interference) flags. */
	u8 aci_enable:1;
	u8 aci_wlan_automatic:1;
	u8 aci_hw_rssi:1;

	u16 minlowsig[2];
	u16 minlowsigpos[2];

	/* TSSI to dBm table in use */
	const s8 *tssi2dbm;
	/* Target idle TSSI */
	int tgt_idle_tssi;
	/* Current idle TSSI */
	int cur_idle_tssi;

	/* LocalOscillator control values. */
	struct bcm43xx_txpower_lo_control *lo_control;
	/* Values from bcm43xx_calc_loopback_gain() */
	s16 max_lb_gain;	/* Maximum Loopback gain in hdB */
	s16 trsw_rx_gain;	/* TRSW RX gain in hdB */
	s16 lna_lod_gain;	/* LNA lod */
	s16 lna_gain;		/* LNA */
	s16 pga_gain;		/* PGA */

	/* PHY lock for core.rev < 3
	 * This lock is only used by bcm43xx_phy_{un}lock()
	 */
	spinlock_t lock;

	/* Desired TX power level (in dBm).
	 * This is set by the user and adjusted in bcm43xx_phy_xmitpower(). */
	u8 power_level;
	/* A-PHY TX Power control value. */
	u16 txpwr_offset;

	/* Current TX power level attenuation control values */
	struct bcm43xx_bbatt bbatt;
	struct bcm43xx_rfatt rfatt;
	u8 tx_control; /* BCM43xx_TXCTL_XXX */
#ifdef CONFIG_BCM43XX_MAC80211_DEBUG
	u8 manual_txpower_control; /* Manual TX-power control enabled? */
#endif

	/* Current Interference Mitigation mode */
	int interfmode;
	/* Stack of saved values from the Interference Mitigation code.
	 * Each value in the stack is layed out as follows:
	 * bit 0-11:  offset
	 * bit 12-15: register ID
	 * bit 16-32: value
	 * register ID is: 0x1 PHY, 0x2 Radio, 0x3 ILT
	 */
#define BCM43xx_INTERFSTACK_SIZE	26
	u32 interfstack[BCM43xx_INTERFSTACK_SIZE];//FIXME: use a data structure

	/* Saved values from the NRSSI Slope calculation */
	s16 nrssi[2];
	s32 nrssislope;
	/* In memory nrssi lookup table. */
	s8 nrssi_lt[64];

	/* current channel */
	u8 channel;

	u16 lofcal;

	u16 initval;//FIXME rename?
};

/* Data structures for DMA transmission, per 80211 core. */
struct bcm43xx_dma {
	struct bcm43xx_dmaring *tx_ring0;
	struct bcm43xx_dmaring *tx_ring1;
	struct bcm43xx_dmaring *tx_ring2;
	struct bcm43xx_dmaring *tx_ring3;
	struct bcm43xx_dmaring *tx_ring4;
	struct bcm43xx_dmaring *tx_ring5;

	struct bcm43xx_dmaring *rx_ring0;
	struct bcm43xx_dmaring *rx_ring3; /* only available on core.rev < 5 */
};

/* Data structures for PIO transmission, per 80211 core. */
struct bcm43xx_pio {
	struct bcm43xx_pioqueue *queue0;
	struct bcm43xx_pioqueue *queue1;
	struct bcm43xx_pioqueue *queue2;
	struct bcm43xx_pioqueue *queue3;
};

/* Context information for a noise calculation (Link Quality). */
struct bcm43xx_noise_calculation {
	u8 channel_at_start;
	u8 calculation_running:1;
	u8 nr_samples;
	s8 samples[8][4];
};

struct bcm43xx_stats {
	u8 link_noise;
	/* Store the last TX/RX times here for updating the leds. */
	unsigned long last_tx;
	unsigned long last_rx;
};

struct bcm43xx_key {
	u8 enabled;
	u8 algorithm;
	u8 address[6];
};

struct bcm43xx_wldev;

/* Data structure for the WLAN parts (802.11 cores) of the bcm43xx chip. */
struct bcm43xx_wl {
	/* Pointer to the active wireless device on this chip */
	struct bcm43xx_wldev *current_dev;
	/* Pointer to the ieee80211 hardware data structure */
	struct ieee80211_hw *hw;

	spinlock_t irq_lock;
	struct mutex mutex;
	spinlock_t leds_lock;

	/* We can only have one operating interface (802.11 core)
	 * at a time. General information about this interface follows.
	 */

	/* Opaque ID of the operating interface (!= monitor
	 * interface) from the ieee80211 subsystem.
	 * Do not modify.
	 */
	int if_id;
	/* MAC address (can be NULL). */
	const u8 *mac_addr;
	/* Current BSSID (can be NULL). */
	const u8 *bssid;
	/* Interface type. (IEEE80211_IF_TYPE_XXX) */
	int if_type;
	/* Counter of active monitor interfaces. */
	int monitor;
	/* Is the card operating in AP, STA or IBSS mode? */
	unsigned int operating:1;
	/* Promisc mode active?
	 * Note that (monitor != 0) implies promisc.
	 */
	unsigned int promisc:1;
	/* Stats about the wireless interface */
	struct ieee80211_low_level_stats ieee_stats;

	struct hwrng rng;
	u8 rng_initialized;
	char rng_name[30 + 1];

	/* List of all wireless devices on this chip */
	struct list_head devlist;
	u8 nr_devs;
};

/* Pointers to the firmware data and meta information about it. */
struct bcm43xx_firmware {
	/* Microcode */
	const struct firmware *ucode;
	/* PCM code */
	const struct firmware *pcm;
	/* Initial MMIO values 0 */
	const struct firmware *initvals0;
	/* Initial MMIO values 1 */
	const struct firmware *initvals1;
	/* Firmware revision */
	u16 rev;
	/* Firmware patchlevel */
	u16 patch;
};

/* Device (802.11 core) initialization status. */
enum {
	BCM43xx_STAT_UNINIT,		/* Uninitialized. */
	BCM43xx_STAT_INITIALIZING,	/* bcm43xx_wireless_core_init() in progress. */
	BCM43xx_STAT_INITIALIZED,	/* Initialized. Note that this doesn't mean it's started. */
};
#define bcm43xx_status(bcm)		atomic_read(&(bcm)->init_status)
#define bcm43xx_set_status(bcm, stat)	do {			\
		atomic_set(&(bcm)->init_status, (stat));	\
		smp_wmb();					\
					} while (0)

/* XXX---   HOW LOCKING WORKS IN BCM43xx   ---XXX
 *
 * You should always acquire both, wl->mutex and wl->irq_lock unless:
 * - You don't need to acquire wl->irq_lock, if the interface is stopped.
 * - You don't need to acquire wl->mutex in the IRQ handler, IRQ tasklet
 *   and packet TX path (and _ONLY_ there.)
 */

/* Data structure for one wireless device (802.11 core) */
struct bcm43xx_wldev {
	struct ssb_device *dev;
	struct bcm43xx_wl *wl;

	/* Driver initialization status BCM43xx_STAT_*** */
	atomic_t init_status;
	/* Interface started? (bcm43xx_wireless_core_start()) */
	u8 started;

	u16 was_initialized:1,		/* for suspend/resume. */
	    was_started:1,		/* for suspend/resume. */
	    __using_pio:1,		/* Internal, use bcm43xx_using_pio(). */
	    bad_frames_preempt:1,	/* Use "Bad Frames Preemption" (default off) */
	    reg124_set_0x4:1,		/* Some variable to keep track of IRQ stuff. */
	    short_preamble:1,		/* TRUE, if short preamble is enabled. */
	    short_slot:1,		/* TRUE, if short slot timing is enabled. */
	    radio_hw_enable:1;          /* saved state of radio hardware enabled state */

	/* PHY/Radio device. */
	struct bcm43xx_phy phy;
	union {
		/* DMA engines. */
		struct bcm43xx_dma dma;
		/* PIO engines. */
		struct bcm43xx_pio pio;
	};

	/* Various statistics about the physical device. */
	struct bcm43xx_stats stats;

#define BCM43xx_NR_LEDS		4
	struct bcm43xx_led leds[BCM43xx_NR_LEDS];

	/* Reason code of the last interrupt. */
	u32 irq_reason;
	u32 dma_reason[6];
	/* saved irq enable/disable state bitfield. */
	u32 irq_savedstate;
	/* Link Quality calculation context. */
	struct bcm43xx_noise_calculation noisecalc;
	/* if > 0 MAC is suspended. if == 0 MAC is enabled. */
	int mac_suspended;

	/* Interrupt Service Routine tasklet (bottom-half) */
	struct tasklet_struct isr_tasklet;

	/* Periodic tasks */
	struct delayed_work periodic_work;
	unsigned int periodic_state;

	struct work_struct restart_work;

	/* encryption/decryption */
	u16 ktp; /* Key table pointer */
	u8 max_nr_keys;
	struct bcm43xx_key key[58];

	/* Cached beacon template while uploading the template. */
	struct sk_buff *cached_beacon;

	/* Firmware data */
	struct bcm43xx_firmware fw;

	/* Devicelist in struct bcm43xx_wl (all 802.11 cores) */
	struct list_head list;

	/* Debugging stuff follows. */
#ifdef CONFIG_BCM43XX_MAC80211_DEBUG
	struct bcm43xx_dfsentry *dfsentry;
#endif
};


static inline
struct bcm43xx_wl * hw_to_bcm43xx_wl(struct ieee80211_hw *hw)
{
	return hw->priv;
}

/* Helper function, which returns a boolean.
 * TRUE, if PIO is used; FALSE, if DMA is used.
 */
#if defined(CONFIG_BCM43XX_MAC80211_DMA) && defined(CONFIG_BCM43XX_MAC80211_PIO)
static inline
int bcm43xx_using_pio(struct bcm43xx_wldev *dev)
{
	return dev->__using_pio;
}
#elif defined(CONFIG_BCM43XX_MAC80211_DMA)
static inline
int bcm43xx_using_pio(struct bcm43xx_wldev *dev)
{
	return 0;
}
#elif defined(CONFIG_BCM43XX_MAC80211_PIO)
static inline
int bcm43xx_using_pio(struct bcm43xx_wldev *dev)
{
	return 1;
}
#else
# error "Using neither DMA nor PIO? Confused..."
#endif


static inline
struct bcm43xx_wldev * dev_to_bcm43xx_wldev(struct device *dev)
{
	struct ssb_device *ssb_dev = dev_to_ssb_dev(dev);
	return ssb_get_drvdata(ssb_dev);
}

/* Is the device operating in a specified mode (IEEE80211_IF_TYPE_XXX). */
static inline
int bcm43xx_is_mode(struct bcm43xx_wl *wl, int type)
{
	if (type == IEEE80211_IF_TYPE_MNTR)
		return !!(wl->monitor);
	return (wl->operating &&
		wl->if_type == type);
}

static inline
u16 bcm43xx_read16(struct bcm43xx_wldev *dev, u16 offset)
{
	return ssb_read16(dev->dev, offset);
}

static inline
void bcm43xx_write16(struct bcm43xx_wldev *dev, u16 offset, u16 value)
{
	ssb_write16(dev->dev, offset, value);
}

static inline
u32 bcm43xx_read32(struct bcm43xx_wldev *dev, u16 offset)
{
	return ssb_read32(dev->dev, offset);
}

static inline
void bcm43xx_write32(struct bcm43xx_wldev *dev, u16 offset, u32 value)
{
	ssb_write32(dev->dev, offset, value);
}

/** Limit a value between two limits */
#ifdef limit_value
# undef limit_value
#endif
#define limit_value(value, min, max)  \
	({						\
		typeof(value) __value = (value);	\
		typeof(value) __min = (min);		\
		typeof(value) __max = (max);		\
		if (__value < __min)			\
			__value = __min;		\
		else if (__value > __max)		\
			__value = __max;		\
		__value;				\
	})

/* Macros for printing a value in Q5.2 format */
#define Q52_FMT		"%u.%u"
#define Q52_ARG(q52)	((q52) / 4), ((((q52) & 3) * 100) / 4)

#endif /* BCM43xx_H_ */
