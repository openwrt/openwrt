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

struct __attribute__ ((__packed__)) rtph_fw_dir {
	uint32_t seqid;
	uint32_t offset;
};

struct __attribute__ ((__packed__)) rtph_fw_head {
	uint32_t magic;
	uint32_t checksum;
	uint32_t dirsize;
	struct rtph_fw_dir dir[];
};

struct __attribute__ ((__packed__)) rtph_fw_data {
	int16_t port;
	uint16_t reg;
	uint16_t val;
	uint16_t mask;
};

struct __attribute__ ((__packed__)) rtph_fw_seq {
        uint16_t portstart;
        uint16_t portend;
        struct rtph_fw_data data[];
};

/* TODO: fixed path? */
#define FIRMWARE_838X_8380_1			"rtl838x_phy/rtl838x_8380.fw"
#define FIRMWARE_838X_8214FC_1			"rtl838x_phy/rtl838x_8214fc.fw"
#define FIRMWARE_838X_8218b_1			"rtl838x_phy/rtl838x_8218b.fw"

#define RTPH_FIRMWARE_MAGIC			0x83009300
#define RTPH_FIRMWARE_839X			"rtl839x_phy.fw"

#define RTPH_FW_8218B_6276A_PERCHIP		0x455b5b24
#define RTPH_FW_8218B_6276A_PERPORT		0xace3ba53
#define RTPH_FW_8218B_6276C_IPD_PERPORT		0xbcd92344
#define RTPH_FW_8218B_6276C_IPD_PERCHIP		0x5561c233
#define RTPH_FW_8218B_6276_PATCH		0xa0607d5f

#define PHY_ID_RTL8214C		0x001cc942
#define PHY_ID_RTL8218B_E	0x001cc980
#define PHY_ID_RTL8214_OR_8218	0x001cc981
#define PHY_ID_RTL8218D		0x001cc983
#define PHY_ID_RTL8218B_I	0x001cca40
#define PHY_ID_RTL8221B		0x001cc849
#define PHY_ID_RTL8226		0x001cc838
#define PHY_ID_RTL8390_GENERIC	0x001ccab0
#define PHY_ID_RTL8393_I	0x001c8393
#define PHY_ID_RTL9300_I	0x70d03106

/* These PHYs share the same id (0x001cc981) */
#define RTPH_PHY_IS_RTL8214FC			1
#define RTPH_PHY_IS_RTL8214FB			2
#define RTPH_PHY_IS_RTL8218B_E			3

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

/* RTL930X SerDes supports the following modes:
 * 0x02: SGMII		0x04: 1000BX_FIBER	0x05: FIBER100
 * 0x06: QSGMII		0x09: RSGMII		0x0d: USXGMII
 * 0x10: XSGMII		0x12: HISGMII		0x16: 2500Base_X
 * 0x17: RXAUI_LITE	0x19: RXAUI_PLUS	0x1a: 10G Base-R
 * 0x1b: 10GR1000BX_AUTO			0x1f: OFF
 */
#define RTPH_930X_SDS_MODE_SGMII		0x02
#define RTPH_930X_SDS_MODE_1000BASEX		0x04
#define RTPH_930X_SDS_MODE_USXGMII		0x0d
#define RTPH_930X_SDS_MODE_XGMII		0x10
#define RTPH_930X_SDS_MODE_HSGMII		0x12
#define RTPH_930X_SDS_MODE_2500BASEX		0x16
#define RTPH_930X_SDS_MODE_10GBASER		0x1a
#define RTPH_930X_SDS_OFF			0x1f
#define RTPH_930X_SDS_MASK			0x1f