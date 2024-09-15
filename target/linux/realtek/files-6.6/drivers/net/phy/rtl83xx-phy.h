// SPDX-License-Identifier: GPL-2.0-only

struct rtl83xx_shared_private {
	char *name;
};

struct __attribute__ ((__packed__)) part {
	uint16_t start;
	uint8_t wordsize;
	uint8_t words;
};

struct __attribute__ ((__packed__)) fw_header {
	uint32_t magic;
	uint32_t phy;
	uint32_t checksum;
	uint32_t version;
	struct part parts[10];
};

struct rtph_soc_data {
	unsigned int id;
	unsigned int family;
	unsigned int rawpage;
};

/* TODO: fixed path? */
#define FIRMWARE_838X_8380_1	"rtl838x_phy/rtl838x_8380.fw"
#define FIRMWARE_838X_8214FC_1	"rtl838x_phy/rtl838x_8214fc.fw"
#define FIRMWARE_838X_8218b_1	"rtl838x_phy/rtl838x_8218b.fw"

/* External RTL8218B and RTL8214FC IDs are identical */
#define PHY_ID_RTL8214C		0x001cc942
#define PHY_ID_RTL8214FC	0x001cc981
#define PHY_ID_RTL8218B_E	0x001cc981
#define PHY_ID_RTL8218D		0x001cc983
#define PHY_ID_RTL8218B_I	0x001cca40
#define PHY_ID_RTL8221B		0x001cc849
#define PHY_ID_RTL8226		0x001cc838
#define PHY_ID_RTL8390_GENERIC	0x001ccab0
#define PHY_ID_RTL8393_I	0x001c8393
#define PHY_ID_RTL9300_I	0x70d03106

/* SOC identifiers */
#define RTPH_SOC_FAMILY_8380  			(0x8380)
#define RTPH_SOC_FAMILY_8390  			(0x8390)
#define RTPH_SOC_FAMILY_9300  			(0x9300)
#define RTPH_SOC_FAMILY_9310  			(0x9310)
#define RTPH_SOC_TYPE_8380			(0x8380)
#define RTPH_SOC_TYPE_8393			(0x8393)

/* Switch register helpers */
#define RTPH_SWITCH_ADDR_BASE			(0xbb000000)
#define RTPH_REG(x)				((void __iomem __force *)RTPH_SWITCH_ADDR_BASE + (x))
#define iomask32(mask, value, addr)		iowrite32((ioread32(addr) & ~(mask)) | (value), addr)

/*  Switch registers */
#define RTPH_838X_INT_MODE_CTRL			RTPH_REG(0x005c)
#define RTPH_838X_INT_RW_CTRL			RTPH_REG(0x0058)
#define RTPH_838X_MODEL_NAME_INFO		RTPH_REG(0x00d4)
#define RTPH_838X_PLL_CML_CTRL			RTPH_REG(0x0ff8)
#define RTPH_838X_SDS_CFG_REG			RTPH_REG(0x0034)
#define RTPH_838X_SDS_MODE_SEL			RTPH_REG(0x0028)
#define RTPH_838X_SDS4_FIB_REG0			RTPH_REG(0xf800)
#define RTPH_838X_SMI_POLL_CTRL			RTPH_REG(0xa17c)

#define RTPH_839X_MODEL_NAME_INFO		RTPH_REG(0x0ff0)
#define RTPH_839X_SDS12_13_XSG0			RTPH_REG(0xb800)
#define RTPH_839X_SMI_PORT_POLLING_CTRL		RTPH_REG(0x03fc)

#define RTPH_930X_MAC_FORCE_MODE_CTRL		RTPH_REG(0xca1c)
#define RTPH_930X_SDS_INDACS_CMD		RTPH_REG(0x03b0)
#define RTPH_930X_SDS_INDACS_DATA		RTPH_REG(0x03b4)
#define RTPH_930X_SMI_POLL_CTRL			RTPH_REG(0xca90)

#define RTPH_931X_CHIP_INFO_ADDR		RTPH_REG(0x0008)
#define RTPH_931X_PS_SERDES_OFF_MODE_CTRL_ADDR	RTPH_REG(0x13f4)
#define RTPH_931X_SERDES_INDRT_ACCESS_CTRL	RTPH_REG(0x5638)
#define RTPH_931X_SERDES_INDRT_DATA_CTRL	RTPH_REG(0x563c)
#define RTPH_931X_SERDES_MODE_CTRL		RTPH_REG(0x13cc)

#define RTPH_93XX_MODEL_NAME_INFO		RTPH_REG(0x0004)

/* Registers of the internal Serdes of the 8380 */
#define RTL838X_SDS_MODE_SEL			(0x0028)
#define RTL838X_SDS_CFG_REG			(0x0034)
#define RTL838X_INT_MODE_CTRL			(0x005c)
#define RTL838X_DMY_REG31			(0x3b28)

#define RTL8380_SDS4_FIB_REG0			(0xF800)
#define RTL838X_SDS4_REG28			(0xef80)
#define RTL838X_SDS4_DUMMY0			(0xef8c)
#define RTL838X_SDS5_EXT_REG6			(0xf18c)
#define RTL838X_SDS4_FIB_REG0			(RTL838X_SDS4_REG28 + 0x880)
#define RTL838X_SDS5_FIB_REG0			(RTL838X_SDS4_REG28 + 0x980)

/* Registers of the internal SerDes of the RTL8390 */
#define RTL839X_SDS12_13_XSG0			(0xB800)

/* Registers of the internal Serdes of the 9300 */
#define RTL930X_SDS_INDACS_CMD			(0x03B0)
#define RTL930X_SDS_INDACS_DATA			(0x03B4)
#define RTL930X_MAC_FORCE_MODE_CTRL		(0xCA1C)

/* Registers of the internal SerDes of the 9310 */
#define RTL931X_SERDES_INDRT_ACCESS_CTRL	(0x5638)
#define RTL931X_SERDES_INDRT_DATA_CTRL		(0x563C)
#define RTL931X_SERDES_MODE_CTRL		(0x13cc)
#define RTL931X_PS_SERDES_OFF_MODE_CTRL_ADDR	(0x13F4)
#define RTL931X_MAC_SERDES_MODE_CTRL(sds)	(0x136C + (((sds) << 2)))
