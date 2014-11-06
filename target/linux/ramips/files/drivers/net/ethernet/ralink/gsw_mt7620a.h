/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2009-2013 John Crispin <blogic@openwrt.org>
 */

#ifndef _RALINK_GSW_MT7620_H__
#define _RALINK_GSW_MT7620_H__

extern int mt7620_gsw_config(struct fe_priv *priv);
extern int mt7620_gsw_probe(struct fe_priv *priv);
extern void mt7620_set_mac(struct fe_priv *priv, unsigned char *mac);
extern int mt7620_mdio_write(struct mii_bus *bus, int phy_addr, int phy_reg, u16 val);
extern int mt7620_mdio_read(struct mii_bus *bus, int phy_addr, int phy_reg);
extern void mt7620_mdio_link_adjust(struct fe_priv *priv, int port);
extern void mt7620_port_init(struct fe_priv *priv, struct device_node *np);
extern int mt7620a_has_carrier(struct fe_priv *priv);

#endif
