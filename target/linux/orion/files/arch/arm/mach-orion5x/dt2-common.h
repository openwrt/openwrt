#ifndef __INC_DT2_COMMON_H
#define __INC_DT2_COMMON_H

#define ATAG_MV_UBOOT			0x41000403

struct tag_mv_uboot {
	u32 uboot_version;
	u32 tclk;
	u32 sysclk;
	u32 isUsbHost;
	u32 overEthAddr;
	u8  dt2_eeprom[256];
};

#define DT2_EEPROM_ADDR 		0x50
#define DT2_EEPROM_OFFSET		0
#define DT2_EEPROM_LENGTH		256

#define DT2_SERIAL_NUMBER_DEFAULT	"run on default\0"
#define DT2_REVISION_DEFAULT_INIT 	0xFF
#define DT2_CONFIG_FLAGS_DEFAULT 	0x00

#define _PACKED_	__attribute__((packed))

struct DT2_EEPROM_SD_CONFIG {
	unsigned int	ram_1;
	unsigned int	ram_2;
	unsigned int	ram_3;
	unsigned int	ram_4;
	unsigned char	ram_5;
	unsigned char	ram_6;
	unsigned short	ram_7;
	unsigned int 	magic_id;
	} _PACKED_;		// 24 Bytes in total

struct DT2_EEPROM_FC_CONFIG {
	unsigned char	rtc_sts_mask;
	unsigned char	rtc_sts_init;
	unsigned char	rtc_int_mask;
	unsigned char	rtc_int_init;
	unsigned char	rtc_atrim_init;
	unsigned char	rtc_dtrim_init;
	unsigned char	dummy1;
	unsigned char	dummy2;
	unsigned char	dt2_config_flags;	/* 0x80 to load rtc_values to RTC */
	unsigned char	dt2_revision;		/* upper nibble is HW, lower nibble is FW */
	unsigned char	dt2_serial_number[16];	/* Serial number of DT-2 */
	} _PACKED_;		// 26 Bytes in total

#define CFG_LOAD_RTC_VALUES 	0x80

struct DT2_EEPROM_GW_CONFIG {
	unsigned int	dummy1;
	unsigned int	dummy2;
	unsigned int	dummy3;
	unsigned char	dummy4;
	unsigned char	tos_video_val1;
	unsigned char	tos_video_val2;
	unsigned char	tos_voip_val;
	unsigned char	qos_igmp_cfg;
	unsigned char	num_of_ifs;
	unsigned short	vlan_ports_if[3];
	unsigned char	mac_addr[3][6];
	} _PACKED_;		// 42 Bytes in total

#define _SIZE_OF_ALL_STRUCTS_ (sizeof(struct DT2_EEPROM_SD_CONFIG) + sizeof(struct DT2_EEPROM_FC_CONFIG) +  sizeof(struct DT2_EEPROM_GW_CONFIG))

// MV = EEPROM - SD - FC - GW - CRC
struct DT2_EEPROM_MV_CONFIG {
	unsigned int reg_addr[(DT2_EEPROM_LENGTH - _SIZE_OF_ALL_STRUCTS_ - sizeof(unsigned int)) / (sizeof(unsigned int) * 2)];
	unsigned int reg_data[(DT2_EEPROM_LENGTH - _SIZE_OF_ALL_STRUCTS_ - sizeof(unsigned int)) / (sizeof(unsigned int) * 2)];
 	} _PACKED_;

struct DT2_EEPROM_STRUCT {
	struct DT2_EEPROM_MV_CONFIG mv;
	struct DT2_EEPROM_SD_CONFIG sd;
	struct DT2_EEPROM_FC_CONFIG fc;
	struct DT2_EEPROM_GW_CONFIG gw;
	unsigned int	crc;
	} _PACKED_;

#endif
