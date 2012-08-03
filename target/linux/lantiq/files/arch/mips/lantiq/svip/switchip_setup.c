/******************************************************************************
     Copyright (c) 2007, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
 ******************************************************************************
   Module      : switchip_setup.c
   Date        : 2007-11-09
   Description : Basic setup of embedded ethernet switch "SwitchIP"
   Remarks: andreas.schmidt@infineon.com

 *****************************************************************************/

/* TODO: get rid of #ifdef CONFIG_LANTIQ_MACH_EASY336 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/time.h>

#include <base_reg.h>
#include <es_reg.h>
#include <sys1_reg.h>
#include <dma_reg.h>
#include <lantiq_soc.h>

static struct svip_reg_sys1 *const sys1 = (struct svip_reg_sys1 *)LTQ_SYS1_BASE;
static struct svip_reg_es *const es = (struct svip_reg_es *)LTQ_ES_BASE;

/* PHY Organizationally Unique Identifier (OUI) */
#define PHY_OUI_PMC           0x00E004
#define PHY_OUI_VITESSE       0x008083
#define PHY_OUI_DEFAULT       0xFFFFFF

unsigned short switchip_phy_read(unsigned int phyaddr, unsigned int regaddr);
void switchip_phy_write(unsigned int phyaddr, unsigned int regaddr,
			unsigned short data);

static int phy_address[2] = {0, 1};
static u32 phy_oui;
static void switchip_mdio_poll_init(void);
static void _switchip_mdio_poll(struct work_struct *work);

/* struct workqueue_struct mdio_poll_task; */
static struct workqueue_struct *mdio_poll_workqueue;
DECLARE_DELAYED_WORK(mdio_poll_work, _switchip_mdio_poll);
static int old_link_status[2] = {-1, -1};

/**
 * Autonegotiation check.
 * This funtion checks for link changes. If a link change has occured it will
 * update certain switch registers.
 */
static void _switchip_check_phy_status(int port)
{
	int new_link_status;
	unsigned short reg1;

	reg1 = switchip_phy_read(phy_address[port], 1);
	if ((reg1 == 0xFFFF) || (reg1 == 0x0000))
		return; /* no PHY connected */

	new_link_status = reg1 & 4;
	if (old_link_status[port] ^ new_link_status) {
		/* link status change */
		if (!new_link_status) {
			if (port == 0)
				es_w32_mask(LTQ_ES_P0_CTL_REG_FLP, 0, p0_ctl);
			else
				es_w32_mask(LTQ_ES_P0_CTL_REG_FLP, 0, p1_ctl);

			/* read again; link bit is latched low! */
			reg1 = switchip_phy_read(phy_address[port], 1);
			new_link_status = reg1 & 4;
		}

		if (new_link_status) {
			unsigned short reg0, reg4, reg5, reg9, reg10;
			int phy_pause, phy_speed, phy_duplex;
			int aneg_enable, aneg_cmpt;

			reg0 = switchip_phy_read(phy_address[port], 0);
			reg4 = switchip_phy_read(phy_address[port], 4);
			aneg_enable = reg0 & 0x1000;
			aneg_cmpt = reg1 & 0x20;

			if (aneg_enable && aneg_cmpt) {
				reg5 = switchip_phy_read(phy_address[port], 5);
				switch (phy_oui) {
#ifdef CONFIG_LANTIQ_MACH_EASY336
				case PHY_OUI_PMC:
					/* PMC Sierra supports 1Gigabit FD,
					 * only. On successful
					 * auto-negotiation, we are sure this
					 * is what the LP can. */
					phy_pause = ((reg4 & reg5) & 0x0080) >> 7;
					phy_speed = 2;
					phy_duplex = 1;
					break;
#endif
				case PHY_OUI_VITESSE:
				case PHY_OUI_DEFAULT:
					reg9 = switchip_phy_read(phy_address[port], 9);
					reg10 = switchip_phy_read(phy_address[port], 10);

					/* Check if advertise and partner
					 * agree on pause */
					phy_pause = ((reg4 & reg5) & 0x0400) >> 10;

					/* Find the best mode both partners
					 * support
					 * Priority: 1GB-FD, 1GB-HD, 100MB-FD,
					 * 100MB-HD, 10MB-FD, 10MB-HD */
					phy_speed = ((((reg9<<2) & reg10)
						      & 0x0c00) >> 6) |
						(((reg4 & reg5) & 0x01e0) >> 5);

					if (phy_speed >= 0x0020) {
						phy_speed = 2;
						phy_duplex = 1;
					} else if (phy_speed >= 0x0010) {
						phy_speed = 2;
						phy_duplex = 0;
					} else if (phy_speed >= 0x0008) {
						phy_speed = 1;
						phy_duplex = 1;
					} else if (phy_speed >= 0x0004) {
						phy_speed = 1;
						phy_duplex = 0;
					} else if (phy_speed >= 0x0002) {
						phy_speed = 0;
						phy_duplex = 1;
					} else {
						phy_speed = 0;
						phy_duplex = 0;
					}
					break;
				default:
					phy_pause = (reg4 & 0x0400) >> 10;
					phy_speed = (reg0 & 0x40 ? 2 : (reg0 >> 13)&1);
					phy_duplex = (reg0 >> 8)&1;
					break;
				}
			} else {
				/* parallel detection or fixed speed */
				phy_pause = (reg4 & 0x0400) >> 10;
				phy_speed = (reg0 & 0x40 ? 2 : (reg0 >> 13)&1);
				phy_duplex = (reg0 >> 8)&1;
			}

			if (port == 0) {
				es_w32_mask(LTQ_ES_RGMII_CTL_REG_P0SPD,
					    LTQ_ES_RGMII_CTL_REG_P0SPD_VAL(phy_speed),
					    rgmii_ctl);
				es_w32_mask(LTQ_ES_RGMII_CTL_REG_P0DUP,
					    LTQ_ES_RGMII_CTL_REG_P0DUP_VAL(phy_duplex),
					    rgmii_ctl);
				es_w32_mask(LTQ_ES_RGMII_CTL_REG_P0FCE,
					    LTQ_ES_RGMII_CTL_REG_P0FCE_VAL(phy_pause),
					    rgmii_ctl);

				es_w32_mask(0, LTQ_ES_P0_CTL_REG_FLP, p0_ctl);
			} else {
				es_w32_mask(LTQ_ES_RGMII_CTL_REG_P1SPD,
					    LTQ_ES_RGMII_CTL_REG_P1SPD_VAL(phy_speed),
					    rgmii_ctl);
				es_w32_mask(LTQ_ES_RGMII_CTL_REG_P1DUP,
					    LTQ_ES_RGMII_CTL_REG_P1DUP_VAL(phy_duplex),
					    rgmii_ctl);
				es_w32_mask(LTQ_ES_RGMII_CTL_REG_P1FCE,
					    LTQ_ES_RGMII_CTL_REG_P0FCE_VAL(phy_pause),
					    rgmii_ctl);

				es_w32_mask(1, LTQ_ES_P0_CTL_REG_FLP, p1_ctl);
			}
		}
	}
	old_link_status[port] = new_link_status;
}

static void _switchip_mdio_poll(struct work_struct *work)
{
	if (es_r32(sw_gctl0) & LTQ_ES_SW_GCTL0_REG_SE) {
		_switchip_check_phy_status(0);
		_switchip_check_phy_status(1);
	}

	queue_delayed_work(mdio_poll_workqueue, &mdio_poll_work, HZ/2);
}

static void switchip_mdio_poll_init(void)
{
	mdio_poll_workqueue = create_workqueue("SVIP MDIP poll");
	INIT_DELAYED_WORK(&mdio_poll_work, _switchip_mdio_poll);

	queue_delayed_work(mdio_poll_workqueue, &mdio_poll_work, HZ/2);

}

unsigned short switchip_phy_read(unsigned int phyaddr, unsigned int regaddr)
{
	/* TODO: protect MDIO access with semaphore */
	es_w32(LTQ_ES_MDIO_CTL_REG_MBUSY
	       | LTQ_ES_MDIO_CTL_REG_OP_VAL(2) /* read operation */
	       | LTQ_ES_MDIO_CTL_REG_PHYAD_VAL(phyaddr)
	       | LTQ_ES_MDIO_CTL_REG_REGAD_VAL(regaddr), mdio_ctl);
	while (es_r32(mdio_ctl) & LTQ_ES_MDIO_CTL_REG_MBUSY);

	return es_r32(mdio_data) & 0xFFFF;
}
EXPORT_SYMBOL(switchip_phy_read);

void switchip_phy_write(unsigned int phyaddr, unsigned int regaddr,
			unsigned short data)
{
	/* TODO: protect MDIO access with semaphore */
	es_w32(LTQ_ES_MDIO_CTL_REG_WD_VAL(data)
	       | LTQ_ES_MDIO_CTL_REG_MBUSY
	       | LTQ_ES_MDIO_CTL_REG_OP_VAL(1) /* write operation */
	       | LTQ_ES_MDIO_CTL_REG_PHYAD_VAL(phyaddr)
	       | LTQ_ES_MDIO_CTL_REG_REGAD_VAL(regaddr), mdio_ctl);
	while (es_r32(mdio_ctl) & LTQ_ES_MDIO_CTL_REG_MBUSY);

	return;
}
EXPORT_SYMBOL(switchip_phy_write);

const static u32 switch_reset_offset_000[] = {
	/*b8000000:*/ 0xffffffff, 0x00000001, 0x00000001, 0x00000003,
	/*b8000010:*/ 0x04070001, 0x04070001, 0x04070001, 0xffffffff,
	/*b8000020:*/ 0x00001be8, 0x00001be8, 0x00001be8, 0xffffffff,
	/*b8000030:*/ 0x00000000, 0x00000000, 0x00080004, 0x00020001,
	/*b8000040:*/ 0x00000000, 0x00000000, 0x00080004, 0x00020001,
	/*b8000050:*/ 0x00000000, 0x00000000, 0x00080004, 0x00020001,
	/*b8000060:*/ 0x00000000, 0x00000000, 0x00081000, 0x001f7777,
	/*b8000070:*/ 0x00000000, 0x00000000, 0x0c00ac2b, 0x0000fa50,
	/*b8000080:*/ 0x00001000, 0x00001800, 0x00000000, 0x00000000,
	/*b8000090:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b80000a0:*/ 0x00000000, 0x00000050, 0x00000010, 0x00000000,
	/*b80000b0:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b80000c0:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b80000d0:*/ 0xffffffff, 0x00000000, 0x00000000
};
const static u32 switch_reset_offset_100[] = {
	/*b8000100:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000110:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000120:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000130:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000140:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000150:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000160:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000170:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000180:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b8000190:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b80001a0:*/ 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	/*b80001b0:*/ 0x00000000, 0x00000000
};

/*
 * Switch Reset.
 */
void switchip_reset(void)
{
	volatile unsigned int *reg;
	volatile unsigned int rdreg;
	int i;

	sys1_w32(SYS1_CLKENR_ETHSW, clkenr);
	asm("sync");

	/* disable P0 */
	es_w32_mask(0, LTQ_ES_P0_CTL_REG_SPS_VAL(1), p0_ctl);
	/* disable P1 */
	es_w32_mask(0, LTQ_ES_P0_CTL_REG_SPS_VAL(1), p1_ctl);
	/* disable P2 */
	es_w32_mask(0, LTQ_ES_P0_CTL_REG_SPS_VAL(1), p2_ctl);

	/**************************************
	 * BEGIN: Procedure to clear MAC table
	 **************************************/
	for (i = 0; i < 3; i++) {
		int result;

		/* check if access engine is available */
		while (es_r32(adr_tb_st2) & LTQ_ES_ADR_TB_ST2_REG_BUSY);

		/* initialise to first address */
		es_w32(LTQ_ES_ADR_TB_CTL2_REG_CMD_VAL(3)
		       | LTQ_ES_ADR_TB_CTL2_REG_AC_VAL(0), adr_tb_ctl2);

		/* wait while busy */
		while (es_r32(adr_tb_st2) & LTQ_ES_ADR_TB_ST2_REG_BUSY);

		/* setup the portmap */
		es_w32_mask(0, LTQ_ES_ADR_TB_CTL1_REG_PMAP_VAL(1 << i),
			    adr_tb_ctl1);

		do {
			/* search for addresses by port */
			es_w32(LTQ_ES_ADR_TB_CTL2_REG_CMD_VAL(2)
			       | LTQ_ES_ADR_TB_CTL2_REG_AC_VAL(9), adr_tb_ctl2);

			/* wait while busy */
			while (es_r32(adr_tb_st2) & LTQ_ES_ADR_TB_ST2_REG_BUSY);

			result = LTQ_ES_ADR_TB_ST2_REG_RSLT_GET(es_r32(adr_tb_st2));
			if (result == 0x101) {
				printk(KERN_ERR "%s, cmd error\n", __func__);
				return;
			}
			/* if Command OK, address found... */
			if (result == 0) {
				unsigned char mac[6];

				mac[5] = (es_r32(adr_tb_st0) >> 0) & 0xff;
				mac[4] = (es_r32(adr_tb_st0) >> 8) & 0xff;
				mac[3] = (es_r32(adr_tb_st0) >> 16) & 0xff;
				mac[2] = (es_r32(adr_tb_st0) >> 24) & 0xff;
				mac[1] = (es_r32(adr_tb_st1) >> 0) & 0xff;
				mac[0] = (es_r32(adr_tb_st1) >> 8) & 0xff;

				/* setup address */
				es_w32((mac[5] << 0) |
				       (mac[4] << 8) |
				       (mac[3] << 16) |
				       (mac[2] << 24), adr_tb_ctl0);
				es_w32(LTQ_ES_ADR_TB_CTL1_REG_PMAP_VAL(1<<i) |
				       LTQ_ES_ADR_TB_CTL1_REG_FID_VAL(0) |
				       (mac[0] << 8) |
				       (mac[1] << 0), adr_tb_ctl1);
				/* erase address */

				es_w32(LTQ_ES_ADR_TB_CTL2_REG_CMD_VAL(1) |
				       LTQ_ES_ADR_TB_CTL2_REG_AC_VAL(15),
				       adr_tb_ctl2);

				/* wait, while busy */
				while (es_r32(adr_tb_st2) &
				       LTQ_ES_ADR_TB_ST2_REG_BUSY);
			}
		} while (result == 0);
	}
	/**************************************
	 * END: Procedure to clear MAC table
	 **************************************/

	/* reset RMON counters */
	es_w32(LTQ_ES_RMON_CTL_REG_BAS | LTQ_ES_RMON_CTL_REG_CAC_VAL(3),
	       rmon_ctl);

	/* bring all registers to reset state */
	reg = LTQ_ES_PS_REG;
	for (i = 0; i < ARRAY_SIZE(switch_reset_offset_000); i++) {
		if ((reg == LTQ_ES_PS_REG) ||
		    (reg >= LTQ_ES_ADR_TB_CTL0_REG &&
		     reg <= LTQ_ES_ADR_TB_ST2_REG))
			continue;

		if (switch_reset_offset_000[i] != 0xFFFFFFFF) {
			/* write reset value to register */
			*reg = switch_reset_offset_000[i];
			/* read register value back */
			rdreg = *reg;
			if (reg == LTQ_ES_SW_GCTL1_REG)
				rdreg &= ~LTQ_ES_SW_GCTL1_REG_BISTDN;
			/* compare read value with written one */
			if (rdreg != switch_reset_offset_000[i]) {
				printk(KERN_ERR "%s,%d: reg %08x mismatch "
				       "[has:%08x, expect:%08x]\n",
				       __func__, __LINE__,
				       (unsigned int)reg, rdreg,
				       switch_reset_offset_000[i]);
			}
		}
		reg++;
	}

	reg = LTQ_ES_VLAN_FLT0_REG;
	for (i = 0; i < ARRAY_SIZE(switch_reset_offset_100); i++) {
		*reg = switch_reset_offset_100[i];
		rdreg = *reg;
		if (rdreg != switch_reset_offset_100[i]) {
			printk(KERN_ERR "%s,%d: reg %08x mismatch "
			       "[has:%08x, expect:%08x]\n", __func__, __LINE__,
			       (unsigned int)reg, rdreg,
			       switch_reset_offset_100[i]);
		}
		reg++;
	}
}
EXPORT_SYMBOL(switchip_reset);

static u32 get_phy_oui(unsigned char phy_addr)
{
	u32 oui;
	int i, bit, byte, shift, w;
	u16 reg_id[2];

	/* read PHY identifier registers 1 and 2 */
	reg_id[0] = switchip_phy_read(phy_addr, 2);
	reg_id[1] = switchip_phy_read(phy_addr, 3);

	oui = 0;
	w = 1;
	shift = 7;
	byte = 1;
	for (i = 0, bit = 10; i <= 21; i++, bit++) {
		oui |= ((reg_id[w] & (1<<bit)) ? 1 : 0) << shift;
		if (!(shift % 8)) {
			byte++;
			if (byte == 2)
				shift = 15;
			else
				shift = 21;
		} else {
			shift--;
		}
		if (w == 1 && bit == 15) {
			bit = -1;
			w = 0;
		}
	}
	return oui;
}

/*
 * Switch Initialization.
 */
int switchip_init(void)
{
	int eth_port, phy_present = 0;
	u16 reg, mode;

	sys1_w32(SYS1_CLKENR_ETHSW, clkenr);
	asm("sync");

	/* Enable Switch, if not already done so */
	if ((es_r32(sw_gctl0) & LTQ_ES_SW_GCTL0_REG_SE) == 0)
		es_w32_mask(0, LTQ_ES_SW_GCTL0_REG_SE, sw_gctl0);
	/* Wait for completion of MBIST */
	while (LTQ_ES_SW_GCTL1_REG_BISTDN_GET(es_r32(sw_gctl1)) == 0);

	switchip_reset();

	mode = LTQ_ES_RGMII_CTL_REG_IS_GET(es_r32(rgmii_ctl));
	eth_port = (mode == 2 ? 1 : 0);

	/* Set the primary port(port toward backplane) as sniffer port,
	   changing from P2 which is the reset setting */
	es_w32_mask(LTQ_ES_SW_GCTL0_REG_SNIFFPN,
		    LTQ_ES_SW_GCTL0_REG_SNIFFPN_VAL(eth_port),
		    sw_gctl0);

	/* Point MDIO state machine to invalid PHY addresses 8 and 9 */
	es_w32_mask(0, LTQ_ES_SW_GCTL0_REG_PHYBA, sw_gctl0);

	/* Add CRC for packets from DMA to PMAC.
	   Remove CRC for packets from PMAC to DMA. */
	es_w32(LTQ_ES_PMAC_HD_CTL_RC | LTQ_ES_PMAC_HD_CTL_AC, pmac_hd_ctl);

	phy_oui = get_phy_oui(0);
	switch (phy_oui) {
#ifdef CONFIG_LANTIQ_MACH_EASY336
	case PHY_OUI_PMC:
		phy_address[0] = (mode == 2 ? -1 : 2);
		phy_address[1] = (mode == 2 ? 2 : -1);
		break;
#endif
	case PHY_OUI_VITESSE:
	default:
		phy_oui = PHY_OUI_DEFAULT;
		phy_address[0] = (mode == 2 ? 1 : 0);
		phy_address[1] = (mode == 2 ? 0 : 1);
		break;
	}

	/****** PORT 0 *****/
	reg = switchip_phy_read(phy_address[0], 1);
	if ((reg != 0x0000) && (reg != 0xffff)) {
		/* PHY connected? */
		phy_present |= 1;
		/* Set Rx- and TxDelay in case of RGMII */
		switch (mode) {
		case 0: /* *RGMII,RGMII */
		case 2: /* RGMII,*GMII */
			/* program clock delay in PHY, not in SVIP */

			es_w32_mask(LTQ_ES_RGMII_CTL_REG_P0RDLY, 0, rgmii_ctl);
			es_w32_mask(LTQ_ES_RGMII_CTL_REG_P0TDLY, 0, rgmii_ctl);
			if (phy_oui == PHY_OUI_VITESSE ||
			    phy_oui == PHY_OUI_DEFAULT) {
				switchip_phy_write(phy_address[0], 31, 0x0001);
				switchip_phy_write(phy_address[0], 28, 0xA000);
				switchip_phy_write(phy_address[0], 31, 0x0000);
			}
		default:
			break;
		}
		if (phy_oui == PHY_OUI_VITESSE ||
		    phy_oui == PHY_OUI_DEFAULT) {
			/* Program PHY advertisements and
			 * restart auto-negotiation */
			switchip_phy_write(phy_address[0], 4, 0x05E1);
			switchip_phy_write(phy_address[0], 9, 0x0300);
			switchip_phy_write(phy_address[0], 0, 0x3300);
		} else {
			reg = switchip_phy_read(phy_address[1], 0);
			reg |= 0x1000; /* auto-negotiation enable */
			switchip_phy_write(phy_address[1], 0, reg);
			reg |= 0x0200; /* auto-negotiation restart */
			switchip_phy_write(phy_address[1], 0, reg);
		}
	} else {
		/* Force SWITCH link with highest capability:
		 * 100M FD for MII
		 * 1G FD for GMII/RGMII
		 */
		switch (mode) {
		case 1: /* *MII,MII */
		case 3: /* *MII,RGMII */
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P0SPD_VAL(1),
				    rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P0DUP_VAL(1),
				    rgmii_ctl);
			break;
		case 0: /* *RGMII,RGMII */
		case 2: /* RGMII,*GMII */
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P0SPD_VAL(2),
				    rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P0DUP_VAL(1),
				    rgmii_ctl);

			es_w32_mask(LTQ_ES_RGMII_CTL_REG_P0RDLY, 0, rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P0TDLY_VAL(2),
				    rgmii_ctl);
			break;
		}

		es_w32_mask(0, LTQ_ES_P0_CTL_REG_FLP, p0_ctl);
	}

	/****** PORT 1 *****/
	reg = switchip_phy_read(phy_address[1], 1);
	if ((reg != 0x0000) && (reg != 0xffff)) {
		/* PHY connected? */
		phy_present |= 2;
		/* Set Rx- and TxDelay in case of RGMII */
		switch (mode) {
		case 0: /* *RGMII,RGMII */
		case 3: /* *MII,RGMII */
			/* program clock delay in PHY, not in SVIP */

			es_w32_mask(LTQ_ES_RGMII_CTL_REG_P1RDLY, 0, rgmii_ctl);
			es_w32_mask(LTQ_ES_RGMII_CTL_REG_P1TDLY, 0, rgmii_ctl);
			if (phy_oui == PHY_OUI_VITESSE ||
			    phy_oui == PHY_OUI_DEFAULT) {
				switchip_phy_write(phy_address[1], 31, 0x0001);
				switchip_phy_write(phy_address[1], 28, 0xA000);
				switchip_phy_write(phy_address[1], 31, 0x0000);
			}
			break;
		case 2: /* RGMII,*GMII */

			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1SPD_VAL(2),
				    rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1DUP, rgmii_ctl);
#ifdef CONFIG_LANTIQ_MACH_EASY336
			if (phy_oui == PHY_OUI_PMC) {
				switchip_phy_write(phy_address[1], 24, 0x0510);
				switchip_phy_write(phy_address[1], 17, 0xA38C);
				switchip_phy_write(phy_address[1], 17, 0xA384);
			}
#endif
			break;
		default:
			break;
		}
		/* Program PHY advertisements and restart auto-negotiation */
		if (phy_oui == PHY_OUI_VITESSE ||
		    phy_oui == PHY_OUI_DEFAULT) {
			switchip_phy_write(phy_address[1], 4, 0x05E1);
			switchip_phy_write(phy_address[1], 9, 0x0300);
			switchip_phy_write(phy_address[1], 0, 0x3300);
		} else {
			reg = switchip_phy_read(phy_address[1], 0);
			reg |= 0x1000; /* auto-negotiation enable */
			switchip_phy_write(phy_address[1], 0, reg);
			reg |= 0x0200; /* auto-negotiation restart */
			switchip_phy_write(phy_address[1], 0, reg);
		}
	} else {
		/* Force SWITCH link with highest capability:
		 * 100M FD for MII
		 * 1G FD for GMII/RGMII
		 */
		switch (mode) {
		case 1: /* *MII,MII */
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1SPD_VAL(1),
				    rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1DUP, rgmii_ctl);
			break;
		case 0: /* *RGMII,RGMII */
		case 3: /* *MII,RGMII */
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1SPD_VAL(2),
				    rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1DUP, rgmii_ctl);
			es_w32_mask(LTQ_ES_RGMII_CTL_REG_P1RDLY, 0, rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1TDLY_VAL(2),
				    rgmii_ctl);
			break;
		case 2: /* RGMII,*GMII */
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1SPD_VAL(2),
				    rgmii_ctl);
			es_w32_mask(0, LTQ_ES_RGMII_CTL_REG_P1DUP, rgmii_ctl);
			break;
		}
		es_w32_mask(0, LTQ_ES_P0_CTL_REG_FLP, p0_ctl);
	}

	/*
	 * Allow unknown unicast/multicast and broadcasts
	 * on all ports.
	 */

	es_w32_mask(0, LTQ_ES_SW_GCTL1_REG_UP_VAL(7), sw_gctl1);
	es_w32_mask(0, LTQ_ES_SW_GCTL1_REG_BP_VAL(7), sw_gctl1);
	es_w32_mask(0, LTQ_ES_SW_GCTL1_REG_MP_VAL(7), sw_gctl1);
	es_w32_mask(0, LTQ_ES_SW_GCTL1_REG_RP_VAL(7), sw_gctl1);

	/* Enable LAN port(s) */
	if (eth_port == 0)
		es_w32_mask(LTQ_ES_P0_CTL_REG_SPS, 0, p0_ctl);
	else
		es_w32_mask(LTQ_ES_P0_CTL_REG_SPS, 0, p1_ctl);
	/* Enable CPU Port (Forwarding State) */
	es_w32_mask(LTQ_ES_P0_CTL_REG_SPS, 0, p2_ctl);

	if (phy_present)
		switchip_mdio_poll_init();

	return 0;
}
EXPORT_SYMBOL(switchip_init);

device_initcall(switchip_init);
