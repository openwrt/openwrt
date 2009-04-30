/*
 * swconfig-ip175c.c: Swconfig configuration for IC+ IP175C switch
 *
 * Copyright (C) 2008 Patrick Horn <patrick.horn@gmail.com>
 * Copyright (C) 2008 Martin Mares <mj@ucw.cz>
 * Copyright (C) 2009 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/switch.h>

#define MAX_VLANS 16
#define MAX_PORTS 9

typedef struct ip175c_reg {
	u16 p;			// phy
	u16 m;			// mii
} reg;
typedef char bitnum;

#define NOTSUPPORTED {-1,-1}

#define REG_SUPP(x) (((x).m != ((u16)-1)) && ((x).p != (u16)-1))

/*********** CONSTANTS ***********/
struct register_mappings {
	char *NAME;
	u16 MODEL_NO;			// compare to bits 4-9 of MII register 0,3.
	bitnum NUM_PORTS;
	bitnum CPU_PORT;

/* The default VLAN for each port.
	 Default: 0x0001 for Ports 0,1,2,3
		  0x0002 for Ports 4,5 */
	reg VLAN_DEFAULT_TAG_REG[MAX_PORTS];

/* These ports are tagged.
	 Default: 0x00 */
	reg ADD_TAG_REG;
	reg REMOVE_TAG_REG;
	bitnum ADD_TAG_BIT[MAX_PORTS];
/* These ports are untagged.
	 Default: 0x00 (i.e. do not alter any VLAN tags...)
	 Maybe set to 0 if user disables VLANs. */
	bitnum REMOVE_TAG_BIT[MAX_PORTS];

/* Port M and Port N are on the same VLAN.
	 Default: All ports on all VLANs. */
// Use register {29, 19+N/2}
	reg VLAN_LOOKUP_REG;
// Port 5 uses register {30, 18} but same as odd bits.
	reg VLAN_LOOKUP_REG_5;		// in a different register on IP175C.
	bitnum VLAN_LOOKUP_EVEN_BIT[MAX_PORTS];
	bitnum VLAN_LOOKUP_ODD_BIT[MAX_PORTS];

/* This VLAN corresponds to which ports.
	 Default: 0x2f,0x30,0x3f,0x3f... */
	reg TAG_VLAN_MASK_REG;
	bitnum TAG_VLAN_MASK_EVEN_BIT[MAX_PORTS];
	bitnum TAG_VLAN_MASK_ODD_BIT[MAX_PORTS];

	int RESET_VAL;
	reg RESET_REG;

	reg MODE_REG;
	int MODE_VAL;

/* General flags */
	reg ROUTER_CONTROL_REG;
	reg VLAN_CONTROL_REG;
	bitnum TAG_VLAN_BIT;
	bitnum ROUTER_EN_BIT;
	bitnum NUMLAN_GROUPS_MAX;
	bitnum NUMLAN_GROUPS_BIT;

	reg MII_REGISTER_EN;
	bitnum MII_REGISTER_EN_BIT;

	// set to 1 for 178C, 0 for 175C.
	bitnum SIMPLE_VLAN_REGISTERS;	// 175C has two vlans per register but 178C has only one.
};

static const struct register_mappings IP178C = {
	.NAME = "IP178C",
	.MODEL_NO = 0x18,
	.VLAN_DEFAULT_TAG_REG = {
		{30,3},{30,4},{30,5},{30,6},{30,7},{30,8},
		{30,9},{30,10},{30,11},
	},

	.ADD_TAG_REG = {30,12},
	.ADD_TAG_BIT = {0,1,2,3,4,5,6,7,8},
	.REMOVE_TAG_REG = {30,13},
	.REMOVE_TAG_BIT = {4,5,6,7,8,9,10,11,12},

	.SIMPLE_VLAN_REGISTERS = 1,

	.VLAN_LOOKUP_REG = {31,0},// +N
	.VLAN_LOOKUP_REG_5 = NOTSUPPORTED, // not used with SIMPLE_VLAN_REGISTERS
	.VLAN_LOOKUP_EVEN_BIT = {0,1,2,3,4,5,6,7,8},
	.VLAN_LOOKUP_ODD_BIT = {0,1,2,3,4,5,6,7,8},

	.TAG_VLAN_MASK_REG = {30,14}, // +N
	.TAG_VLAN_MASK_EVEN_BIT = {0,1,2,3,4,5,6,7,8},
	.TAG_VLAN_MASK_ODD_BIT = {0,1,2,3,4,5,6,7,8},

	.RESET_VAL = 0x55AA,
	.RESET_REG = {30,0},
	.MODE_VAL = 0,
	.MODE_REG = NOTSUPPORTED,

	.ROUTER_CONTROL_REG = {30,30},
	.ROUTER_EN_BIT = 11,
	.NUMLAN_GROUPS_MAX = 8,
	.NUMLAN_GROUPS_BIT = 8, // {0-2}

	.VLAN_CONTROL_REG = {30,13},
	.TAG_VLAN_BIT = 3,

	.CPU_PORT = 8,
	.NUM_PORTS = 9,

	.MII_REGISTER_EN = NOTSUPPORTED,

};

static const struct register_mappings IP175C = {
	.NAME = "IP175C",
	.MODEL_NO = 0x18,
	.VLAN_DEFAULT_TAG_REG = {
		{29,24},{29,25},{29,26},{29,27},{29,28},{29,30},
		NOTSUPPORTED,NOTSUPPORTED,NOTSUPPORTED
	},

	.ADD_TAG_REG = {29,23},
	.REMOVE_TAG_REG = {29,23},
	.ADD_TAG_BIT = {11,12,13,14,15,1,-1,-1,-1},
	.REMOVE_TAG_BIT = {6,7,8,9,10,0,-1,-1,-1},

	.SIMPLE_VLAN_REGISTERS = 0,

	.VLAN_LOOKUP_REG = {29,19},// +N/2
	.VLAN_LOOKUP_REG_5 = {30,18},
	.VLAN_LOOKUP_EVEN_BIT = {8,9,10,11,12,15,-1,-1,-1},
	.VLAN_LOOKUP_ODD_BIT = {0,1,2,3,4,7,-1,-1,-1},

	.TAG_VLAN_MASK_REG = {30,1}, // +N/2
	.TAG_VLAN_MASK_EVEN_BIT = {0,1,2,3,4,5,-1,-1,-1},
	.TAG_VLAN_MASK_ODD_BIT = {8,9,10,11,12,13,-1,-1,-1},

	.RESET_VAL = 0x175C,
	.RESET_REG = {30,0},
	.MODE_VAL = 0x175C,
	.MODE_REG = {29,31},

	.ROUTER_CONTROL_REG = {30,9},
	.ROUTER_EN_BIT = 3,
	.NUMLAN_GROUPS_MAX = 8,
	.NUMLAN_GROUPS_BIT = 0, // {0-2}

	.VLAN_CONTROL_REG = {30,9},
	.TAG_VLAN_BIT = 7,

	.NUM_PORTS = 6,
	.CPU_PORT = 5,

	.MII_REGISTER_EN = NOTSUPPORTED,

};

static const struct register_mappings IP175A = {
	.NAME = "IP175A",
	.MODEL_NO = 0x05,
	.VLAN_DEFAULT_TAG_REG = {
		{0,24},{0,25},{0,26},{0,27},{0,28},NOTSUPPORTED,
		NOTSUPPORTED,NOTSUPPORTED,NOTSUPPORTED
	},

	.ADD_TAG_REG = {0,23},
	.REMOVE_TAG_REG = {0,23},
	.ADD_TAG_BIT = {11,12,13,14,15,1,-1,-1,-1},
	.REMOVE_TAG_BIT = {6,7,8,9,10,0,-1,-1,-1},

	.SIMPLE_VLAN_REGISTERS = 1,

	// Only programmable via. EEPROM
	.VLAN_LOOKUP_REG = NOTSUPPORTED,// +N/2
	.VLAN_LOOKUP_REG_5 = NOTSUPPORTED,
	.VLAN_LOOKUP_EVEN_BIT = {8,9,10,11,12,15,-1,-1,-1},
	.VLAN_LOOKUP_ODD_BIT = {0,1,2,3,4,7,-1,-1,-1},

	.TAG_VLAN_MASK_REG = NOTSUPPORTED, // +N/2
	.TAG_VLAN_MASK_EVEN_BIT = {0,1,2,3,4,5,-1,-1,-1},
	.TAG_VLAN_MASK_ODD_BIT = {8,9,10,11,12,13,-1,-1,-1},

	.RESET_VAL = -1,
	.RESET_REG = NOTSUPPORTED,
	.MODE_VAL = 0,
	.MODE_REG = NOTSUPPORTED,

	.ROUTER_CONTROL_REG = NOTSUPPORTED,
	.VLAN_CONTROL_REG = NOTSUPPORTED,
	.TAG_VLAN_BIT = -1,
	.ROUTER_EN_BIT = -1,
	.NUMLAN_GROUPS_MAX = -1,
	.NUMLAN_GROUPS_BIT = -1, // {0-2}

	.NUM_PORTS = 6,
	.CPU_PORT = 5,

	.MII_REGISTER_EN = {0, 12},
	.MII_REGISTER_EN_BIT = 7,
};

struct ip175c_state {
	struct switch_dev dev;
	struct mii_bus *mii_bus;
	bool registered;

	int router_mode;		// ROUTER_EN
	int vlan_enabled;		// TAG_VLAN_EN
	struct port_state {
		u16 pvid;
		unsigned int shareports;
	} ports[MAX_PORTS];
	unsigned int add_tag;
	unsigned int remove_tag;
	int num_vlans;
	unsigned int vlan_ports[MAX_VLANS];
	const struct register_mappings *regs;
	reg proc_mii; /*!< phy/reg for the low level register access via /proc */

	char buf[80];
};

static int ip_phy_read(struct mii_bus *bus, int port, int reg)
{
	int val;

	mutex_lock(&bus->mdio_lock);
	val = bus->read(bus, port, reg);
	mutex_unlock(&bus->mdio_lock);

	return val;
}


static int ip_phy_write(struct mii_bus *bus, int port, int reg, u16 val)
{
	int err;

	mutex_lock(&bus->mdio_lock);
	err = bus->write(bus, port, reg, val);
	mutex_unlock(&bus->mdio_lock);

	return err;
}


static int getPhy (struct ip175c_state *state, reg mii)
{
	struct mii_bus *bus = state->mii_bus;
	int val;

	if (!REG_SUPP(mii))
		return -EFAULT;

	val = ip_phy_read(bus, mii.p, mii.m);
	if (val < 0)
		pr_warning("IP175C: Unable to get MII register %d,%d: error %d\n", mii.p,mii.m,-val);

	return val;
}

static int setPhy (struct ip175c_state *state, reg mii, u16 value)
{
	struct mii_bus *bus = state->mii_bus;
	int err;

	if (!REG_SUPP(mii))
		return -EFAULT;

	err = ip_phy_write(bus, mii.p, mii.m, value);
	if (err < 0) {
		pr_warning("IP175C: Unable to set MII register %d,%d to %d: error %d\n", mii.p,mii.m,value,-err);
		return err;
	}
	mdelay(2);
	getPhy(state, mii);
	return 0;
}

/**
 * These two macros are to simplify the mapping of logical bits to the bits in hardware.
 * NOTE: these macros will return if there is an error!
 */

#define GET_PORT_BITS(state, bits, addr, bit_lookup)		\
	do {							\
		int i, val = getPhy((state), (addr));		\
		if (val < 0)					\
			return val;				\
		(bits) = 0;					\
		for (i = 0; i < MAX_PORTS; i++) {		\
			if ((bit_lookup)[i] == -1) continue;	\
			if (val & (1<<(bit_lookup)[i]))		\
				(bits) |= (1<<i);		\
		}						\
	} while (0)

#define SET_PORT_BITS(state, bits, addr, bit_lookup)		\
	do {							\
		int i, val = getPhy((state), (addr));		\
		if (val < 0)					\
			return val;				\
		for (i = 0; i < MAX_PORTS; i++) {		\
			unsigned int newmask = ((bits)&(1<<i));	\
			if ((bit_lookup)[i] == -1) continue;	\
			val &= ~(1<<(bit_lookup)[i]);		\
			val |= ((newmask>>i)<<(bit_lookup)[i]);	\
		}						\
		val = setPhy((state), (addr), val);		\
		if (val < 0)					\
			return val;				\
	} while (0)

static int get_model(struct ip175c_state *state)
{
	reg oui_id_reg = {0, 2};
	int oui_id;
	reg model_no_reg = {0, 3};
	int model_no, model_no_orig;

	// 175 and 178 have the same oui ID.
	reg oui_id_reg_178c = {5, 2}; // returns error on IP175C.
	int is_178c = 0;

	oui_id = getPhy(state, oui_id_reg);
	if (oui_id != 0x0243) {
		// non
		return -ENODEV; // Not a IC+ chip.
	}
	oui_id = getPhy(state, oui_id_reg_178c);
	if (oui_id == 0x0243) {
		is_178c = 1;
	}

	model_no_orig = getPhy(state, model_no_reg);
	if (model_no_orig < 0) {
		return -ENODEV;
	}
	model_no = model_no_orig >> 4; // shift out revision number.
	model_no &= 0x3f; // only take the model number (low 6 bits).
	if (model_no == IP175A.MODEL_NO) {
		state->regs = &IP175A;
	} else if (model_no == IP175C.MODEL_NO) {
		if (is_178c) {
			state->regs = &IP178C;
		} else {
			state->regs = &IP175C;
		}
	} else {
		printk(KERN_WARNING "ip175c: Found an unknown IC+ switch with model number %02Xh.\n", model_no_orig);
		return -EPERM;
	}
	return 0;
}

/** Get only the vlan and router flags on the router **/
static int get_flags(struct ip175c_state *state)
{
	int val;

	state->router_mode = 0;
	state->vlan_enabled = -1; // hack
	state->num_vlans = 0;

	if (!REG_SUPP(state->regs->ROUTER_CONTROL_REG)) {
		return 0; // not an error if it doesn't support enable vlan.
	}

	val = getPhy(state, state->regs->ROUTER_CONTROL_REG);
	if (val < 0) {
		return val;
	}
	if (state->regs->ROUTER_EN_BIT >= 0)
		state->router_mode = ((val>>state->regs->ROUTER_EN_BIT) & 1);

	if (state->regs->NUMLAN_GROUPS_BIT >= 0) {
		state->num_vlans = (val >> state->regs->NUMLAN_GROUPS_BIT);
		state->num_vlans &= (state->regs->NUMLAN_GROUPS_MAX-1);
		state->num_vlans+=1; // does not include WAN.
	}


	val = getPhy(state, state->regs->VLAN_CONTROL_REG);
	if (val < 0) {
		return 0;
	}
	if (state->regs->TAG_VLAN_BIT >= 0)
		state->vlan_enabled = ((val>>state->regs->TAG_VLAN_BIT) & 1);

	return  0;
}
/** Get all state variables for VLAN mappings and port-based tagging. **/
static int get_state(struct ip175c_state *state)
{
	int i, j;
	int ret;
	ret = get_flags(state);
	if (ret < 0) {
		return ret;
	}
	GET_PORT_BITS(state, state->remove_tag,
				  state->regs->REMOVE_TAG_REG, state->regs->REMOVE_TAG_BIT);
	GET_PORT_BITS(state, state->add_tag,
				  state->regs->ADD_TAG_REG, state->regs->ADD_TAG_BIT);

	if (state->vlan_enabled == -1) {
		// not sure how to get this...
		state->vlan_enabled = (!state->remove_tag && !state->add_tag);
	}

	if (REG_SUPP(state->regs->VLAN_LOOKUP_REG)) {
	for (j=0; j<MAX_PORTS; j++) {
		state->ports[j].shareports = 0; // initialize them in case.
	}
	for (j=0; j<state->regs->NUM_PORTS; j++) {
		reg addr;
		const bitnum *bit_lookup = (j%2==0)?
			state->regs->VLAN_LOOKUP_EVEN_BIT:
			state->regs->VLAN_LOOKUP_ODD_BIT;
		addr = state->regs->VLAN_LOOKUP_REG;
		if (state->regs->SIMPLE_VLAN_REGISTERS) {
			addr.m += j;
		} else {
			switch (j) {
			case 0:
			case 1:
				break;
			case 2:
			case 3:
				addr.m+=1;
				break;
			case 4:
				addr.m+=2;
				break;
			case 5:
				addr = state->regs->VLAN_LOOKUP_REG_5;
				break;
			}
		}

		if (REG_SUPP(addr)) {
			GET_PORT_BITS(state, state->ports[j].shareports, addr, bit_lookup);
		}
	}
	} else {
		for (j=0; j<MAX_PORTS; j++) {
			state->ports[j].shareports = 0xff;
		}
	}

	for (i=0; i<MAX_PORTS; i++) {
		if (REG_SUPP(state->regs->VLAN_DEFAULT_TAG_REG[i])) {
			int val = getPhy(state, state->regs->VLAN_DEFAULT_TAG_REG[i]);
			if (val < 0) {
				return val;
			}
			state->ports[i].pvid = val;
		} else {
			state->ports[i].pvid = 0;
		}
	}

	if (REG_SUPP(state->regs->TAG_VLAN_MASK_REG)) {
		for (j=0; j<MAX_VLANS; j++) {
			reg addr = state->regs->TAG_VLAN_MASK_REG;
			const bitnum *bit_lookup = (j%2==0)?
				state->regs->TAG_VLAN_MASK_EVEN_BIT:
				state->regs->TAG_VLAN_MASK_ODD_BIT;
			if (state->regs->SIMPLE_VLAN_REGISTERS) {
				addr.m += j;
			} else {
				addr.m += j/2;
			}
			GET_PORT_BITS(state, state->vlan_ports[j], addr, bit_lookup);
		}
	} else {
		for (j=0; j<MAX_VLANS; j++) {
			state->vlan_ports[j] = 0;
			for (i=0; i<state->regs->NUM_PORTS; i++) {
				if ((state->ports[i].pvid == j) ||
						(state->ports[i].pvid == 0)) {
					state->vlan_ports[j] |= (1<<i);
				}
			}
		}
	}

	return 0;
}


/** Only update vlan and router flags in the switch **/
static int update_flags(struct ip175c_state *state)
{
	int val;

	if (!REG_SUPP(state->regs->ROUTER_CONTROL_REG)) {
		return 0;
	}

	val = getPhy(state, state->regs->ROUTER_CONTROL_REG);
	if (val < 0) {
		return val;
	}
	if (state->regs->ROUTER_EN_BIT >= 0) {
		if (state->router_mode) {
			val |= (1<<state->regs->ROUTER_EN_BIT);
		} else {
			val &= (~(1<<state->regs->ROUTER_EN_BIT));
		}
	}
	if (state->regs->TAG_VLAN_BIT >= 0) {
		if (state->vlan_enabled) {
			val |= (1<<state->regs->TAG_VLAN_BIT);
		} else {
			val &= (~(1<<state->regs->TAG_VLAN_BIT));
		}
	}
	if (state->regs->NUMLAN_GROUPS_BIT >= 0) {
		val &= (~((state->regs->NUMLAN_GROUPS_MAX-1)<<state->regs->NUMLAN_GROUPS_BIT));
		if (state->num_vlans > state->regs->NUMLAN_GROUPS_MAX) {
			val |= state->regs->NUMLAN_GROUPS_MAX << state->regs->NUMLAN_GROUPS_BIT;
		} else if (state->num_vlans >= 1) {
			val |= (state->num_vlans-1) << state->regs->NUMLAN_GROUPS_BIT;
		}
	}
	return setPhy(state, state->regs->ROUTER_CONTROL_REG, val);
}

/** Update all VLAN and port state.  Usually you should call "correct_vlan_state" first. **/
static int update_state(struct ip175c_state *state)
{
	int j;
	int i;
	SET_PORT_BITS(state, state->add_tag,
				  state->regs->ADD_TAG_REG, state->regs->ADD_TAG_BIT);
	SET_PORT_BITS(state, state->remove_tag,
				  state->regs->REMOVE_TAG_REG, state->regs->REMOVE_TAG_BIT);

	if (REG_SUPP(state->regs->VLAN_LOOKUP_REG)) {
	for (j=0; j<state->regs->NUM_PORTS; j++) {
		reg addr;
		const bitnum *bit_lookup = (j%2==0)?
			state->regs->VLAN_LOOKUP_EVEN_BIT:
			state->regs->VLAN_LOOKUP_ODD_BIT;

		// duplicate code -- sorry
		addr = state->regs->VLAN_LOOKUP_REG;
		if (state->regs->SIMPLE_VLAN_REGISTERS) {
			addr.m += j;
		} else {
			switch (j) {
			case 0:
			case 1:
				break;
			case 2:
			case 3:
				addr.m+=1;
				break;
			case 4:
				addr.m+=2;
				break;
			case 5:
				addr = state->regs->VLAN_LOOKUP_REG_5;
				break;
			default:
				addr.m = -1; // shouldn't get here, but...
				break;
			}
		}
		//printf("shareports for %d is %02X\n",j,state->ports[j].shareports);
		if (REG_SUPP(addr)) {
			SET_PORT_BITS(state, state->ports[j].shareports, addr, bit_lookup);
		}
	}
	}
	if (REG_SUPP(state->regs->TAG_VLAN_MASK_REG)) {
	for (j=0; j<MAX_VLANS; j++) {
		reg addr = state->regs->TAG_VLAN_MASK_REG;
		const bitnum *bit_lookup = (j%2==0)?
			state->regs->TAG_VLAN_MASK_EVEN_BIT:
			state->regs->TAG_VLAN_MASK_ODD_BIT;
		unsigned int vlan_mask;
		if (state->regs->SIMPLE_VLAN_REGISTERS) {
			addr.m += j;
		} else {
			addr.m += j/2;
		}
		vlan_mask = state->vlan_ports[j];
		SET_PORT_BITS(state, vlan_mask, addr, bit_lookup);
	}
	}

	for (i=0; i<MAX_PORTS; i++) {
		if (REG_SUPP(state->regs->VLAN_DEFAULT_TAG_REG[i])) {
			int err = setPhy(state, state->regs->VLAN_DEFAULT_TAG_REG[i],
					state->ports[i].pvid);
			if (err < 0) {
				return err;
			}
		}
	}

	return update_flags(state);

	// software reset: 30.0 = 0x175C
	// wait 2ms
	// reset ports 0,1,2,3,4
}

/*
  Uses only the VLAN port mask and the add tag mask to generate the other fields:
  which ports are part of the same VLAN, removing vlan tags, and VLAN tag ids.
 */
static void correct_vlan_state(struct ip175c_state *state)
{
	int i, j;
	state->num_vlans = 0;
	for (i=0; i<MAX_VLANS; i++) {
		if (state->vlan_ports[i] != 0) {
			state->num_vlans = i+1; //hack -- we need to store the "set" vlans somewhere...
		}
	}



	for (i=0; i<state->regs->NUM_PORTS; i++) {
		unsigned int portmask = (1<<i);
		if (!state->vlan_enabled) {
			// share with everybody!
			state->ports[i].shareports = (1<<state->regs->NUM_PORTS)-1;
			continue;
		}
		state->ports[i].shareports = portmask;
		for (j=0; j<MAX_VLANS; j++) {
			if (state->vlan_ports[j] & portmask)
				state->ports[i].shareports |= state->vlan_ports[j];
		}
	}
	state->remove_tag = ((~state->add_tag) & ((1<<state->regs->NUM_PORTS)-1));
}

static int ip175c_get_enable_vlan(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int err;

	err = get_state(state); // may be set in get_state.
	if (err < 0)
		return err;
	val->value.i = state->vlan_enabled;
	return 0;
}

static int ip175c_set_enable_vlan(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int err;
	int enable;
	int i;

	err = get_state(state);
	if (err < 0)
		return err;
	enable = val->value.i;

	if (state->vlan_enabled == enable) {
		// do not change any state.
		return 0;
	}
	state->vlan_enabled = enable;

	// Otherwise, if we are switching state, set fields to a known default.
	state->remove_tag = 0x0000;
	state->add_tag = 0x0000;
	for (i = 0; i < MAX_PORTS; i++)
		state->ports[i].shareports = 0xffff;

	for (i = 0; i < MAX_VLANS; i++)
		state->vlan_ports[i] = 0x0;

	if (state->vlan_enabled) {
		// updates other fields only based off vlan_ports and add_tag fields.
		// Note that by default, no ports are in any vlans.
		correct_vlan_state(state);
	}
	// ensure sane defaults?
	return update_state(state);
}

static int ip175c_get_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int err;
	int b;
	int ind;
	unsigned int ports;

	if (val->port_vlan >= dev->vlans || val->port_vlan < 0)
		return -EINVAL;

	err = get_state(state);
	if (err<0)
		return err;

	ports = state->vlan_ports[val->port_vlan];
	b = 0;
	ind = 0;
	while (b < MAX_PORTS) {
		if (ports&1) {
			int istagged = ((state->add_tag >> b) & 1);
			val->value.ports[ind].id = b;
			val->value.ports[ind].flags = (istagged << SWITCH_PORT_FLAG_TAGGED);
			ind++;
		}
		b++;
		ports >>= 1;
	}
	val->len = ind;

	return 0;
}

static int ip175c_set_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int i;
	int err;

	if (val->port_vlan >= dev->vlans || val->port_vlan < 0)
		return -EINVAL;

	err = get_state(state);
	if (err < 0)
		return err;

	state->vlan_ports[val->port_vlan] = 0;
	for (i = 0; i < val->len; i++) {
		int bitmask = (1<<val->value.ports[i].id);
		state->vlan_ports[val->port_vlan] |= bitmask;
		if (val->value.ports[i].flags & (1<<SWITCH_PORT_FLAG_TAGGED)) {
			state->add_tag |= bitmask;
		} else {
			state->add_tag &= (~bitmask);
		}
	}

	correct_vlan_state(state);
	err = update_state(state);

	return err;
}

static int ip175c_apply(struct switch_dev *dev)
{
	struct ip175c_state *state = dev->priv;
	int err;

	err = get_flags(state);
	if (err < 0)
		return err;

	if (REG_SUPP(state->regs->MII_REGISTER_EN)){
		int val = getPhy(state, state->regs->MII_REGISTER_EN);
		if (val < 0) {
			return val;
		}
		val |= (1<<state->regs->MII_REGISTER_EN_BIT);
		return setPhy(state, state->regs->MII_REGISTER_EN, val);
	}
	return 0;
}

static int ip175c_reset(struct switch_dev *dev)
{
	struct ip175c_state *state = dev->priv;
	int i, err;

	err = get_flags(state);
	if (err < 0)
		return err;

	if (REG_SUPP(state->regs->RESET_REG)) {
		err = setPhy(state, state->regs->RESET_REG, state->regs->RESET_VAL);
		if (err < 0)
			return err;
		err = getPhy(state, state->regs->RESET_REG);

		/* data sheet specifies reset period is 2 msec
		   (don't see any mention of the 2ms delay in the IP178C spec, only
		    in IP175C, but it can't hurt.) */
		mdelay(2);
	}

	if (REG_SUPP(state->regs->MODE_REG)) {
		err = setPhy(state, state->regs->MODE_REG, state->regs->RESET_VAL);
		if (err < 0)
			return err;
		err = getPhy(state, state->regs->MODE_REG);
	}

	/* reset switch ports */
	for (i = 0; i < 5; i++) {
		err = state->mii_bus->write(state->mii_bus, i,
					 MII_BMCR, BMCR_RESET);
		if (err < 0)
			return err;
	}

	return 0;
}

/*! get the current register number */
static int ip175c_get_tagged(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int err;

	err = get_state(state);
	if (err < 0)
		return err;

	if (state->add_tag & (1<<val->port_vlan)) {
		if (state->remove_tag & (1<<val->port_vlan))
			val->value.i = 3; // shouldn't ever happen.
		else
			val->value.i = 1;
	} else {
		if (state->remove_tag & (1<<val->port_vlan))
			val->value.i = 0;
		else
			val->value.i = 2;
	}
	return 0;
}

/*! set a new register address for low level access to registers */
static int ip175c_set_tagged(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int err;

	err = get_state(state);
	if (err < 0)
		return err;

	state->add_tag &= ~(1<<val->port_vlan);
	state->remove_tag &= ~(1<<val->port_vlan);

	if (val->value.i == 0)
		state->remove_tag |= (1<<val->port_vlan);
	if (val->value.i == 1)
		state->add_tag |= (1<<val->port_vlan);

	SET_PORT_BITS(state, state->add_tag,
				  state->regs->ADD_TAG_REG, state->regs->ADD_TAG_BIT);
	SET_PORT_BITS(state, state->remove_tag,
				  state->regs->REMOVE_TAG_REG, state->regs->REMOVE_TAG_BIT);

	return err;
}


/* low level /proc procedures */

/*! get the current phy address */
static int ip175c_get_phy(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;

	val->value.i = state->proc_mii.p;
	return 0;
}

/*! set a new phy address for low level access to registers */
static int ip175c_set_phy(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int new_reg = val->value.i;

	if (new_reg < 0 || new_reg > 31)
		state->proc_mii.p = (u16)-1;
	else
		state->proc_mii.p = (u16)new_reg;
	return 0;
}

/*! get the current register number */
static int ip175c_get_reg(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;

	val->value.i = state->proc_mii.m;
	return 0;
}

/*! set a new register address for low level access to registers */
static int ip175c_set_reg(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int new_reg = val->value.i;

	if (new_reg < 0 || new_reg > 31)
		state->proc_mii.m = (u16)-1;
	else
		state->proc_mii.m = (u16)new_reg;
	return 0;
}

/*! get the register content of state->proc_mii */
static int ip175c_get_val(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int retval = -EINVAL;
	if (REG_SUPP(state->proc_mii))
		retval = getPhy(state, state->proc_mii);

	if (retval < 0) {
		return retval;
	} else {
		val->value.i = retval;
		return 0;
	}
}

/*! write a value to the register defined by phy/reg above */
static int ip175c_set_val(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	int myval, err = 0;

	myval = val->value.i;
	if (myval <= 0xffff && myval >= 0 && REG_SUPP(state->proc_mii)) {
		err = setPhy(state, state->proc_mii, (u16)myval);
	}
	return err;
}

static int ip175c_read_name(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	val->value.s = state->regs->NAME; // just a const pointer, won't be freed by swconfig.
	return 0;
}


static int ip175c_set_port_speed(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	struct mii_bus *bus = state->mii_bus;
	int nr = val->port_vlan;
	int ctrl;
	int autoneg;
	int speed;
	if (val->value.i == 100) {
		speed = 1;
		autoneg = 0;
	} else if (val->value.i == 10) {
		speed = 0;
		autoneg = 0;
	} else {
		autoneg = 1;
		speed = 1;
	}

	/* can't set speed for cpu port */
	if (nr == state->regs->CPU_PORT)
		return -EINVAL;

	if (nr >= dev->ports || nr < 0)
		return -EINVAL;

	ctrl = ip_phy_read(bus, nr, 0);
	if (ctrl < 0)
		return -EIO;

	ctrl &= (~(1<<12));
	ctrl &= (~(1<<13));
	ctrl |= (autoneg<<12);
	ctrl |= (speed<<13);

	return ip_phy_write(bus, nr, 0, ctrl);
}

static int ip175c_get_port_speed(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	struct mii_bus *bus = state->mii_bus;
	int nr = val->port_vlan;
	int speed, status;

	if (nr == state->regs->CPU_PORT) {
		val->value.i = 100;
		return 0;
	}

	if (nr >= dev->ports || nr < 0)
		return -EINVAL;

	status = ip_phy_read(bus, nr, 1);
	speed = ip_phy_read(bus, nr, 18);
	if (status < 0 || speed < 0)
		return -EIO;

	if (status & 4)
		val->value.i = ((speed & (1<<11)) ? 100 : 10);
	else
		val->value.i = 0;

	return 0;
}


static int ip175c_get_port_status(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct ip175c_state *state = dev->priv;
	struct mii_bus *bus = state->mii_bus;
	int ctrl, speed, status;
	int nr = val->port_vlan;
	int len;
	char *buf = state->buf; // fixed-length at 80.

	if (nr == state->regs->CPU_PORT) {
		sprintf(buf, "up, 100 Mbps, cpu port");
		val->value.s = buf;
		return 0;
	}

	if (nr >= dev->ports || nr < 0)
		return -EINVAL;

	ctrl = ip_phy_read(bus, nr, 0);
	status = ip_phy_read(bus, nr, 1);
	speed = ip_phy_read(bus, nr, 18);
	if (ctrl < 0 || status < 0 || speed < 0)
		return -EIO;

	if (status & 4)
		len = sprintf(buf, "up, %d Mbps, %s duplex",
			((speed & (1<<11)) ? 100 : 10),
			((speed & (1<<10)) ? "full" : "half"));
	else
		len = sprintf(buf, "down");

	if (ctrl & (1<<12)) {
		len += sprintf(buf+len, ", auto-negotiate");
		if (!(status & (1<<5)))
			len += sprintf(buf+len, " (in progress)");
	} else {
		len += sprintf(buf+len, ", fixed speed (%d)",
			((ctrl & (1<<13)) ? 100 : 10));
	}

	buf[len] = '\0';
	val->value.s = buf;
	return 0;
}

static int ip175c_get_pvid(struct switch_dev *dev, int port, int *val)
{
	struct ip175c_state *state = dev->priv;

	*val = state->ports[port].pvid;
	return 0;
}

static int ip175c_set_pvid(struct switch_dev *dev, int port, int val)
{
	struct ip175c_state *state = dev->priv;

	state->ports[port].pvid = val;

	if (!REG_SUPP(state->regs->VLAN_DEFAULT_TAG_REG[port]))
		return 0;

	return setPhy(state, state->regs->VLAN_DEFAULT_TAG_REG[port], val);
}


enum Ports {
	IP175C_PORT_STATUS,
	IP175C_PORT_LINK,
	IP175C_PORT_TAGGED,
	IP175C_PORT_PVID,
};

enum Globals {
	IP175C_ENABLE_VLAN,
	IP175C_GET_NAME,
	IP175C_REGISTER_PHY,
	IP175C_REGISTER_MII,
	IP175C_REGISTER_VALUE,
	IP175C_REGISTER_ERRNO,
};

static const struct switch_attr ip175c_global[] = {
	[IP175C_ENABLE_VLAN] = {
		.id = IP175C_ENABLE_VLAN,
		.type = SWITCH_TYPE_INT,
		.name  = "enable_vlan",
		.description = "Flag to enable or disable VLANs and tagging",
		.get  = ip175c_get_enable_vlan,
		.set = ip175c_set_enable_vlan,
	},
	[IP175C_GET_NAME] = {
		.id = IP175C_GET_NAME,
		.type = SWITCH_TYPE_STRING,
		.description = "Returns the type of IC+ chip.",
		.name  = "name",
		.get  = ip175c_read_name,
		.set = NULL,
	},
	/* jal: added for low level debugging etc. */
	[IP175C_REGISTER_PHY] = {
		.id = IP175C_REGISTER_PHY,
		.type = SWITCH_TYPE_INT,
		.description = "Direct register access: set phy (0-4, or 29,30,31)",
		.name  = "phy",
		.get  = ip175c_get_phy,
		.set = ip175c_set_phy,
	},
	[IP175C_REGISTER_MII] = {
		.id = IP175C_REGISTER_MII,
		.type = SWITCH_TYPE_INT,
		.description = "Direct register access: set mii number (0-31)",
		.name  = "reg",
		.get  = ip175c_get_reg,
		.set = ip175c_set_reg,
	},
	[IP175C_REGISTER_VALUE] = {
		.id = IP175C_REGISTER_VALUE,
		.type = SWITCH_TYPE_INT,
		.description = "Direct register access: read/write to register (0-65535)",
		.name  = "val",
		.get  = ip175c_get_val,
		.set = ip175c_set_val,
	},
};

static const struct switch_attr ip175c_vlan[] = {
};

static const struct switch_attr ip175c_port[] = {
	[IP175C_PORT_STATUS] = {
		.id = IP175C_PORT_STATUS,
		.type = SWITCH_TYPE_STRING,
		.description = "Returns Detailed port status",
		.name  = "status",
		.get  = ip175c_get_port_status,
		.set = NULL,
	},
	[IP175C_PORT_LINK] = {
		.id = IP175C_PORT_LINK,
		.type = SWITCH_TYPE_INT,
		.description = "Link speed. Can write 0 for auto-negotiate, or 10 or 100",
		.name  = "link",
		.get  = ip175c_get_port_speed,
		.set = ip175c_set_port_speed,
	},
	[IP175C_PORT_TAGGED] = {
		.id = IP175C_PORT_LINK,
		.type = SWITCH_TYPE_INT,
		.description = "0 = untag, 1 = add tags, 2 = do not alter (This value is reset if vlans are altered)",
		.name  = "tagged",
		.get  = ip175c_get_tagged,
		.set = ip175c_set_tagged,
	},
};

static int ip175c_probe(struct phy_device *pdev)
{
	struct ip175c_state *state;
	struct switch_dev *dev;
	int err;

	/* we only attach to PHY 0, but use all available PHYs */
	if (pdev->addr != 0)
		return -ENODEV;

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;

	dev = &state->dev;
	dev->attr_global.attr = ip175c_global;
	dev->attr_global.n_attr = ARRAY_SIZE(ip175c_global);
	dev->attr_port.attr = ip175c_port;
	dev->attr_port.n_attr = ARRAY_SIZE(ip175c_port);
	dev->attr_vlan.attr = ip175c_vlan;
	dev->attr_vlan.n_attr = ARRAY_SIZE(ip175c_vlan);

	dev->get_port_pvid = ip175c_get_pvid;
	dev->set_port_pvid = ip175c_set_pvid;
	dev->get_vlan_ports = ip175c_get_ports;
	dev->set_vlan_ports = ip175c_set_ports;
	dev->apply_config = ip175c_apply;
	dev->reset_switch = ip175c_reset;

	dev->priv = state;
	pdev->priv = state;
	state->mii_bus = pdev->bus;

	err = get_model(state);
	if (err < 0)
		goto error;

	dev->vlans = MAX_VLANS;
	dev->cpu_port = state->regs->CPU_PORT;
	dev->ports = state->regs->NUM_PORTS;
	dev->name = state->regs->NAME;

	return 0;

error:
	kfree(state);
	return err;
}

static int ip175c_config_init(struct phy_device *pdev)
{
	struct ip175c_state *state = pdev->priv;
	struct net_device *dev = pdev->attached_dev;
	int err;

	pdev->irq = PHY_IGNORE_INTERRUPT;
	err = register_switch(&state->dev, dev);
	if (err < 0)
		return err;

	ip175c_reset(&state->dev);

	state->registered = true;
	netif_carrier_on(pdev->attached_dev);

	return 0;
}

static void ip175c_remove(struct phy_device *pdev)
{
	struct ip175c_state *state = pdev->priv;

	if (state->registered)
		unregister_switch(&state->dev);
	kfree(state);
}

static int ip175c_config_aneg(struct phy_device *pdev)
{
	return 0;
}

static int ip175c_read_status(struct phy_device *pdev)
{
	pdev->speed = SPEED_100;
	pdev->duplex = DUPLEX_FULL;
	pdev->pause = pdev->asym_pause = 0;
	return 0;
}

static struct phy_driver ip175c_driver = {
	.name		= "IC+ IP175C",
	.phy_id		= 0x02430d80,
	.phy_id_mask	= 0x0ffffff0,
	.features	= PHY_BASIC_FEATURES,
	.probe		= ip175c_probe,
	.remove		= ip175c_remove,
	.config_init	= ip175c_config_init,
	.config_aneg	= ip175c_config_aneg,
	.read_status	= ip175c_read_status,
	.driver		= { .owner = THIS_MODULE },
};


int __init ip175c_init(void)
{
	return phy_driver_register(&ip175c_driver);
}

void __exit ip175c_exit(void)
{
	phy_driver_unregister(&ip175c_driver);
}

MODULE_AUTHOR("Patrick Horn <patrick.horn@gmail.com>");
MODULE_AUTHOR("Felix Fietkau <nbd@openwrt.org>");
MODULE_LICENSE("GPL");

module_init(ip175c_init);
module_exit(ip175c_exit);

