/*
 *	Defines for ADM5120 built in ethernet switch driver
 *
 *	Copyright Jeroen Vreeken (pe1rxq@amsat.org), 2005
 *
 *	Values come from ADM5120 datasheet and original ADMtek 2.4 driver,
 *	Copyright ADMtek Inc.
 */

#ifndef _INCLUDE_ADM5120SW_H_
#define _INCLUDE_ADM5120SW_H_

#define SW_BASE	KSEG1ADDR(0x12000000)
#define SW_DEVS	6
#define SW_IRQ 	9

#define ETH_TX_TIMEOUT	HZ/4
#define ETH_FCS 4;

#define ADM5120_CODE		0x00		/* CPU description */
#define ADM5120_CODE_PQFP	0x20000000	/* package type */
#define ADM5120_CPUP_CONF	0x24		/* CPU port config */
#define ADM5120_DISCCPUPORT	0x00000001	/* disable cpu port */
#define ADM5120_CRC_PADDING	0x00000002	/* software crc */
#define ADM5120_DISUNSHIFT	9
#define ADM5120_DISUNALL	0x00007e00	/* disable unknown from all */
#define ADM5120_DISMCSHIFT	16
#define ADM5120_DISMCALL	0x003f0000	/* disable multicast from all */
#define ADM5120_PORT_CONF0	0x28
#define ADM5120_ENMC		0x00003f00	/* Enable MC routing (ex cpu) */
#define ADM5120_ENBP		0x003f0000	/* Enable Back Pressure */
#define ADM5120_VLAN_GI		0x40		/* VLAN settings */
#define ADM5120_VLAN_GII	0x44
#define ADM5120_SEND_TRIG	0x48
#define ADM5120_SEND_TRIG_L	0x00000001
#define ADM5120_SEND_TRIG_H	0x00000002
#define ADM5120_MAC_WT0		0x58
#define ADM5120_MAC_WRITE	0x00000001
#define ADM5120_MAC_WRITE_DONE	0x00000002
#define ADM5120_VLAN_EN		0x00000040
#define ADM5120_MAC_WT1		0x5c
#define ADM5120_PHY_CNTL2	0x7c
#define ADM5120_AUTONEG		0x0000001f	/* Auto negotiate */
#define ADM5120_NORMAL		0x01f00000	/* PHY normal mode */
#define ADM5120_AUTOMDIX	0x3e000000	/* Auto MDIX */
#define ADM5120_PHY_CNTL3	0x80
#define ADM5120_PHY_NTH		0x00000400
#define ADM5120_INT_ST		0xb0
#define ADM5120_INT_RXH		0x0000004
#define ADM5120_INT_RXL		0x0000008
#define ADM5120_INT_HFULL	0x0000010
#define ADM5120_INT_LFULL	0x0000020
#define ADM5120_INT_TXH		0x0000001
#define ADM5120_INT_TXL		0x0000002
#define ADM5120_INT_MASK	0xb4
#define ADM5120_INTMASKALL	0x1FDEFFF	/* All interrupts */
#define ADM5120_INTHANDLE	(ADM5120_INT_RXH | ADM5120_INT_RXL | \
				 ADM5120_INT_HFULL | ADM5120_INT_LFULL | \
				 ADM5120_INT_TXH | ADM5120_INT_TXL)
#define ADM5120_SEND_HBADDR	0xd0
#define ADM5120_SEND_LBADDR	0xd4
#define ADM5120_RECEIVE_HBADDR	0xd8
#define ADM5120_RECEIVE_LBADDR	0xdc

struct adm5120_dma {
	u32 data;
	u32 cntl;
	u32 len;
	u32 status;
} __attribute__ ((packed));

#define	ADM5120_DMA_MASK	0x00ffffff
#define ADM5120_DMA_OWN		0x80000000	/* buffer owner */
#define ADM5120_DMA_RINGEND	0x10000000	/* Last in DMA ring */

#define ADM5120_DMA_ADDR(ptr)	((u32)(ptr) & ADM5120_DMA_MASK)
#define ADM5120_DMA_PORTID	0x00007000
#define ADM5120_DMA_PORTSHIFT	12
#define ADM5120_DMA_LEN		0x07ff0000
#define ADM5120_DMA_LENSHIFT	16
#define ADM5120_DMA_FCSERR	0x00000008

#define ADM5120_DMA_TXH		16
#define ADM5120_DMA_TXL		64
#define ADM5120_DMA_RXH		16
#define ADM5120_DMA_RXL		8

#define ADM5120_DMA_RXSIZE	1550
#define ADM5120_DMA_EXTRA	20

struct adm5120_sw {
	int			port;
	struct net_device_stats	stats;
};

#define SIOCSMATRIX	SIOCDEVPRIVATE
#define SIOCGMATRIX	SIOCDEVPRIVATE+1
#define SIOCGADMINFO	SIOCDEVPRIVATE+2

struct adm5120_info {
	u16	magic;
	u16	ports;
	u16	vlan;
};

#endif /* _INCLUDE_ADM5120SW_H_ */
