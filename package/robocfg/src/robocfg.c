/*
 * Broadcom BCM5325E/536x switch configuration utility
 *
 * Copyright (C) 2005 Oleg I. Vdovikin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
 * 02110-1301, USA.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

/* linux stuff */
typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;

#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/mii.h>

#include "etc53xx.h"
#define ROBO_PHY_ADDR	0x1E	/* robo switch phy address */

/* MII registers */
#define REG_MII_PAGE	0x10	/* MII Page register */
#define REG_MII_ADDR	0x11	/* MII Address register */
#define REG_MII_DATA0	0x18	/* MII Data register 0 */

#define REG_MII_PAGE_ENABLE	1
#define REG_MII_ADDR_WRITE	1
#define REG_MII_ADDR_READ	2

/* Private et.o ioctls */
#define SIOCGETCPHYRD           (SIOCDEVPRIVATE + 9)
#define SIOCSETCPHYWR           (SIOCDEVPRIVATE + 10)

typedef struct {
	struct ifreq ifr;
	int fd;
	int et;			/* use private ioctls */
} robo_t;

static u16 mdio_read(robo_t *robo, u16 phy_id, u8 reg)
{
	if (robo->et) {
		int args[2] = { reg };
		
		if (phy_id != ROBO_PHY_ADDR) {
			fprintf(stderr,
				"Access to real 'phy' registers unavaliable.\n"
				"Upgrade kernel driver.\n");

			return 0xffff;
		}
	
		robo->ifr.ifr_data = (caddr_t) args;
		if (ioctl(robo->fd, SIOCGETCPHYRD, (caddr_t)&robo->ifr) < 0) {
			perror("SIOCGETCPHYRD");
			exit(1);
		}
	
		return args[1];
	} else {
		struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&robo->ifr.ifr_data;
		mii->phy_id = phy_id;
		mii->reg_num = reg;
		if (ioctl(robo->fd, SIOCGMIIREG, &robo->ifr) < 0) {
			perror("SIOCGMIIREG");
			exit(1);
		}
		return mii->val_out;
	}
}

static void mdio_write(robo_t *robo, u16 phy_id, u8 reg, u16 val)
{
	if (robo->et) {
		int args[2] = { reg, val };

		if (phy_id != ROBO_PHY_ADDR) {
			fprintf(stderr,
				"Access to real 'phy' registers unavaliable.\n"
				"Upgrade kernel driver.\n");
			return;
		}
		
		robo->ifr.ifr_data = (caddr_t) args;
		if (ioctl(robo->fd, SIOCSETCPHYWR, (caddr_t)&robo->ifr) < 0) {
			perror("SIOCGETCPHYWR");
			exit(1);
		}
	} else {
		struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&robo->ifr.ifr_data;
		mii->phy_id = phy_id;
		mii->reg_num = reg;
		mii->val_in = val;
		if (ioctl(robo->fd, SIOCSMIIREG, &robo->ifr) < 0) {
			perror("SIOCSMIIREG");
			exit(1);
		}
	}
}

static int robo_reg(robo_t *robo, u8 page, u8 reg, u8 op)
{
	int i = 3;
	
	/* set page number */
	mdio_write(robo, ROBO_PHY_ADDR, REG_MII_PAGE, 
		(page << 8) | REG_MII_PAGE_ENABLE);
	
	/* set register address */
	mdio_write(robo, ROBO_PHY_ADDR, REG_MII_ADDR, 
		(reg << 8) | op);

	/* check if operation completed */
	while (i--) {
		if ((mdio_read(robo, ROBO_PHY_ADDR, REG_MII_ADDR) & 3) == 0)
			return 0;
	}

	fprintf(stderr, "robo_reg: timeout\n");
	exit(1);
	
	return 0;
}

static void robo_read(robo_t *robo, u8 page, u8 reg, u16 *val, int count)
{
	int i;
	
	robo_reg(robo, page, reg, REG_MII_ADDR_READ);
	
	for (i = 0; i < count; i++)
		val[i] = mdio_read(robo, ROBO_PHY_ADDR, REG_MII_DATA0 + i);
}

static u16 robo_read16(robo_t *robo, u8 page, u8 reg)
{
	robo_reg(robo, page, reg, REG_MII_ADDR_READ);
	
	return mdio_read(robo, ROBO_PHY_ADDR, REG_MII_DATA0);
}

static u32 robo_read32(robo_t *robo, u8 page, u8 reg)
{
	robo_reg(robo, page, reg, REG_MII_ADDR_READ);
	
	return mdio_read(robo, ROBO_PHY_ADDR, REG_MII_DATA0) +
		(mdio_read(robo, ROBO_PHY_ADDR, REG_MII_DATA0 + 1) << 16);
}

static void robo_write16(robo_t *robo, u8 page, u8 reg, u16 val16)
{
	/* write data */
	mdio_write(robo, ROBO_PHY_ADDR, REG_MII_DATA0, val16);

	robo_reg(robo, page, reg, REG_MII_ADDR_WRITE);
}

static void robo_write32(robo_t *robo, u8 page, u8 reg, u32 val32)
{
	/* write data */
	mdio_write(robo, ROBO_PHY_ADDR, REG_MII_DATA0, val32 & 65535);
	mdio_write(robo, ROBO_PHY_ADDR, REG_MII_DATA0 + 1, val32 >> 16);
	
	robo_reg(robo, page, reg, REG_MII_ADDR_WRITE);
}

/* checks that attached switch is 5325E/5350 */
static int robo_vlan5350(robo_t *robo)
{
	/* set vlan access id to 15 and read it back */
	u16 val16 = 15;
	robo_write16(robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350, val16);
	
	/* 5365 will refuse this as it does not have this reg */
	return (robo_read16(robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350) == val16);
}

u8 port[6] = { 0, 1, 2, 3, 4, 8 };
char ports[6] = { 'W', '4', '3', '2', '1', 'C' };
char *rxtx[4] = { "enabled", "rx_disabled", "tx_disabled", "disabled" };
char *stp[8] = { "none", "disable", "block", "listen", "learn", "forward", "6", "7" };

struct {
	char *name;
	u16 bmcr;
} media[5] = { { "auto", BMCR_ANENABLE | BMCR_ANRESTART }, 
	{ "10HD", 0 }, { "10FD", BMCR_FULLDPLX },
	{ "100HD", BMCR_SPEED100 }, { "100FD", BMCR_SPEED100 | BMCR_FULLDPLX } };

struct {
	char *name;
	u16 value;
} mdix[3] = { { "auto", 0x0000 }, { "on", 0x1800 }, { "off", 0x0800 } };

void usage()
{
	fprintf(stderr, "Broadcom BCM5325E/536x switch configuration utility\n"
		"Copyright (C) 2005 Oleg I. Vdovikin\n\n"
		"This program is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
		"GNU General Public License for more details.\n\n");

	fprintf(stderr, "Usage: robocfg <op> ... <op>\n"
			"Operations are as below:\n"
			"\tshow\n"
			"\tswitch <enable|disable>\n"
			"\tport <port_number> [state <%s|%s|%s|%s>]\n\t\t[stp %s|%s|%s|%s|%s|%s] [tag <vlan_tag>]\n"
			"\t\t[media %s|%s|%s|%s|%s] [mdi-x %s|%s|%s]\n"
			"\tvlan <vlan_number> [ports <ports_list>]\n"
			"\tvlans <enable|disable|reset>\n\n"
			"\tports_list should be one argument, space separated, quoted if needed,\n"
			"\tport number could be followed by 't' to leave packet vlan tagged (CPU \n"
			"\tport default) or by 'u' to untag packet (other ports default) before \n"
			"\tbringing it to the port, '*' is ignored\n"
			"\nSamples:\n"
			"1) ASUS WL-500g Deluxe stock config (eth0 is WAN, eth0.1 is LAN):\n"
			"robocfg switch disable vlans enable reset vlan 0 ports \"0 5u\" vlan 1 ports \"1 2 3 4 5t\""
			" port 0 state enabled stp none switch enable\n"
			"2) WRT54g, WL-500g Deluxe OpenWRT config (vlan0 is LAN, vlan1 is WAN):\n"
			"robocfg switch disable vlans enable reset vlan 0 ports \"1 2 3 4 5t\" vlan 1 ports \"0 5t\""
			" port 0 state enabled stp none switch enable\n",
			rxtx[0], rxtx[1], rxtx[2], rxtx[3], stp[0], stp[1], stp[2], stp[3], stp[4], stp[5],
			media[0].name, media[1].name, media[2].name, media[3].name, media[4].name,
			mdix[0].name, mdix[1].name, mdix[2].name);
}

int
main(int argc, char *argv[])
{
	u16 val16;
	u16 mac[3];
	int i = 0, j;
	int robo5350 = 0;
	u32 phyid;
	
	static robo_t robo;
	struct ethtool_drvinfo info;
	
	if ((robo.fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/* the only interface for now */
	strcpy(robo.ifr.ifr_name, "eth0");

	memset(&info, 0, sizeof(info));
	info.cmd = ETHTOOL_GDRVINFO;
	robo.ifr.ifr_data = (caddr_t)&info;
	if (ioctl(robo.fd, SIOCETHTOOL, (caddr_t)&robo.ifr) < 0) {
		perror("SIOCETHTOOL: your ethernet module is either unsupported or outdated");
//		exit(1);
	} else
	if (strcmp(info.driver, "et0") && strcmp(info.driver, "b44")) {
		fprintf(stderr, "No suitable module found for %s (managed by %s)\n", 
			robo.ifr.ifr_name, info.driver);
		exit(1);
	}
	
	/* try access using MII ioctls - get phy address */
	if (ioctl(robo.fd, SIOCGMIIPHY, &robo.ifr) < 0) {
		robo.et = 1;
	} else {
		/* got phy address check for robo address */
		struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&robo.ifr.ifr_data;
		if (mii->phy_id != ROBO_PHY_ADDR) {
			fprintf(stderr, "Invalid phy address (%d)\n", mii->phy_id);
			exit(1);
		}
	}

	phyid = mdio_read(&robo, ROBO_PHY_ADDR, 0x2) | 
		(mdio_read(&robo, ROBO_PHY_ADDR, 0x3) << 16);
	
	if (phyid == 0xffffffff || phyid == 0x55210022) {
		fprintf(stderr, "No Robo switch in managed mode found\n");
		exit(1);
	}
	
	robo5350 = robo_vlan5350(&robo);
	
	for (i = 1; i < argc;) {
		if (strcasecmp(argv[i], "port") == 0 && (i + 1) < argc)
		{
			int index = atoi(argv[++i]);
			/* read port specs */
			while (++i < argc) {
				if (strcasecmp(argv[i], "state") == 0 && ++i < argc) {
					for (j = 0; j < 4 && strcasecmp(argv[i], rxtx[j]); j++);
					if (j < 4) {
						/* change state */
						robo_write16(&robo,ROBO_CTRL_PAGE, port[index],
							(robo_read16(&robo, ROBO_CTRL_PAGE, port[index]) & ~(3 << 0)) | (j << 0));
					} else {
						fprintf(stderr, "Invalid state '%s'.\n", argv[i]);
						exit(1);
					}
				} else
				if (strcasecmp(argv[i], "stp") == 0 && ++i < argc) {
					for (j = 0; j < 8 && strcasecmp(argv[i], stp[j]); j++);
					if (j < 8) {
						/* change stp */
						robo_write16(&robo,ROBO_CTRL_PAGE, port[index],
							(robo_read16(&robo, ROBO_CTRL_PAGE, port[index]) & ~(7 << 5)) | (j << 5));
					} else {
						fprintf(stderr, "Invalid stp '%s'.\n", argv[i]);
						exit(1);
					}
				} else
				if (strcasecmp(argv[i], "media") == 0 && ++i < argc) {
					for (j = 0; j < 5 && strcasecmp(argv[i], media[j].name); j++);
					if (j < 5) {
                                    		mdio_write(&robo, port[index], MII_BMCR, media[j].bmcr);
					} else {
						fprintf(stderr, "Invalid media '%s'.\n", argv[i]);
						exit(1);
					}
				} else
				if (strcasecmp(argv[i], "mdi-x") == 0 && ++i < argc) {
					for (j = 0; j < 3 && strcasecmp(argv[i], mdix[j].name); j++);
					if (j < 3) {
                                    		mdio_write(&robo, port[index], 0x1c, mdix[j].value |
						    (mdio_read(&robo, port[index], 0x1c) & ~0x1800));
					} else {
						fprintf(stderr, "Invalid mdi-x '%s'.\n", argv[i]);
						exit(1);
					}
				} else
				if (strcasecmp(argv[i], "tag") == 0 && ++i < argc) {
					j = atoi(argv[i]);
					/* change vlan tag */
					robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_PORT0_DEF_TAG + (index << 1), j);
				} else break;
			}
		} else
		if (strcasecmp(argv[i], "vlan") == 0 && (i + 1) < argc)
		{
			int index = atoi(argv[++i]);
			while (++i < argc) {
				if (strcasecmp(argv[i], "ports") == 0 && ++i < argc) {
					char *ports = argv[i];
					int untag = 0;
					int member = 0;
					
					while (*ports >= '0' && *ports <= '9') {
						j = *ports++ - '0';
						member |= 1 << j;
						
						/* untag if needed, CPU port requires special handling */
						if (*ports == 'u' || (j != 5 && (*ports == ' ' || *ports == 0))) 
						{
							untag |= 1 << j;
							if (*ports) ports++;
							/* change default vlan tag */
							robo_write16(&robo, ROBO_VLAN_PAGE, 
								ROBO_VLAN_PORT0_DEF_TAG + (j << 1), index);
						} else 
						if (*ports == '*' || *ports == 't' || *ports == ' ') ports++;
						else break;
						
						while (*ports == ' ') ports++;
					}
					
					if (*ports) {
						fprintf(stderr, "Invalid ports '%s'.\n", argv[i]);
						exit(1);
					} else {
						/* write config now */
						val16 = (index) /* vlan */ | (1 << 12) /* write */ | (1 << 13) /* enable */;
						if (robo5350) {
							robo_write32(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_WRITE_5350,
								(1 << 20) /* valid */ | (untag << 6) | member);
							robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350, val16);
						} else {
							robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_WRITE,
								(1 << 14)  /* valid */ | (untag << 7) | member);
							robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS, val16);
						}
					}
				} else break;
			}
		} else
		if (strcasecmp(argv[i], "switch") == 0 && (i + 1) < argc)
		{
			/* enable/disable switching */
			robo_write16(&robo, ROBO_CTRL_PAGE, ROBO_SWITCH_MODE,
				(robo_read16(&robo, ROBO_CTRL_PAGE, ROBO_SWITCH_MODE) & ~2) |
				(*argv[++i] == 'e' ? 2 : 0));
			i++;
		} else
		if (strcasecmp(argv[i], "vlans") == 0 && (i + 1) < argc)
		{
			while (++i < argc) {
				if (strcasecmp(argv[i], "reset") == 0) {
					/* reset vlan validity bit */
					for (j = 0; j <= (robo5350 ? VLAN_ID_MAX5350 : VLAN_ID_MAX); j++) 
					{
						/* write config now */
						val16 = (j) /* vlan */ | (1 << 12) /* write */ | (1 << 13) /* enable */;
						if (robo5350) {
							robo_write32(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_WRITE_5350, 0);
							robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350, val16);
						} else {
							robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_WRITE, 0);
							robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS, val16);
						}
					}
				} else 
				if (strcasecmp(argv[i], "enable") == 0 || strcasecmp(argv[i], "disable") == 0) 
				{
					int disable = (*argv[i] == 'd') || (*argv[i] == 'D');
					/* enable/disable vlans */
					robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_CTRL0, disable ? 0 :
						(1 << 7) /* 802.1Q VLAN */ | (3 << 5) /* mac check and hash */);

					robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_CTRL1, disable ? 0 :
						(1 << 1) | (1 << 2) | (1 << 3) /* RSV multicast */);

					robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_CTRL4, disable ? 0 :
						(1 << 6) /* drop invalid VID frames */);

					robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_CTRL5, disable ? 0 :
						(1 << 3) /* drop miss V table frames */);

				} else break;
			}
		} else
		if (strcasecmp(argv[i], "show") == 0)
		{
			break;
		} else {
			fprintf(stderr, "Invalid option %s\n", argv[i]);
			usage();
			exit(1);
		}
	}

	if (i == argc) {
		if (argc == 1) usage();
		return 0;
	}
	
	/* show config */
		
	printf("Switch: %sabled\n", robo_read16(&robo, ROBO_CTRL_PAGE, ROBO_SWITCH_MODE) & 2 ? "en" : "dis");

	for (i = 0; i < 6; i++) {
		printf(robo_read16(&robo, ROBO_STAT_PAGE, ROBO_LINK_STAT_SUMMARY) & (1 << port[i]) ?
			"Port %d(%c): %s%s " : "Port %d(%c):  DOWN ", i, ports[i],
			robo_read16(&robo, ROBO_STAT_PAGE, ROBO_SPEED_STAT_SUMMARY) & (1 << port[i]) ? "100" : " 10",
			robo_read16(&robo, ROBO_STAT_PAGE, ROBO_DUPLEX_STAT_SUMMARY) & (1 << port[i]) ? "FD" : "HD");
		
		val16 = robo_read16(&robo, ROBO_CTRL_PAGE, port[i]);
		
		printf("%s stp: %s vlan: %d ", rxtx[val16 & 3], stp[(val16 >> 5) & 7],
			robo_read16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_PORT0_DEF_TAG + (i << 1)));
			
		robo_read(&robo, ROBO_STAT_PAGE, ROBO_LSA_PORT0 + port[i] * 6, mac, 3);
		
		printf("mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
			mac[2] >> 8, mac[2] & 255, mac[1] >> 8, mac[1] & 255, mac[0] >> 8, mac[0] & 255);
	}
	
	val16 = robo_read16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_CTRL0);
	
	printf("VLANs: %s %sabled%s%s\n", 
		robo5350 ? "BCM5325/535x" : "BCM536x",
		(val16 & (1 << 7)) ? "en" : "dis", 
		(val16 & (1 << 6)) ? " mac_check" : "", 
		(val16 & (1 << 5)) ? " mac_hash" : "");
	
	/* scan VLANs */
	for (i = 0; i <= (robo5350 ? VLAN_ID_MAX5350 : VLAN_ID_MAX); i++) {
		/* issue read */
		val16 = (i) /* vlan */ | (0 << 12) /* read */ | (1 << 13) /* enable */;
		
		if (robo5350) {
			u32 val32;
			robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350, val16);
			/* actual read */
			val32 = robo_read32(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_READ);
			if ((val32 & (1 << 20)) /* valid */) {
				printf("vlan%d:", i);
				for (j = 0; j < 6; j++) {
					if (val32 & (1 << j)) {
						printf(" %d%s", j, (val32 & (1 << (j + 6))) ? 
							(j == 5 ? "u" : "") : "t");
					}
				}
				printf("\n");
			}
		} else {
			robo_write16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS, val16);
			/* actual read */
			val16 = robo_read16(&robo, ROBO_VLAN_PAGE, ROBO_VLAN_READ);
			if ((val16 & (1 << 14)) /* valid */) {
				printf("vlan%d:", i);
				for (j = 0; j < 6; j++) {
					if (val16 & (1 << j)) {
						printf(" %d%s", j, (val16 & (1 << (j + 7))) ? 
							(j == 5 ? "u" : "") : "t");
					}
				}
				printf("\n");
			}
		}
	}
	
	return (0);
}
