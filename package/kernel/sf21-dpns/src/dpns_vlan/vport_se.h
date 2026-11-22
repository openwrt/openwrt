#include "se_common.h"

#define VLAN_VPORT_MAP_ADDR(idx)	0x1800e4 + ((idx)*4)	// 10 x 4
#define MODIFY_VPORT_MAP_ADDR(idx)	0x028040 + ((idx)*4)	// 5 x 4
#define TMU_IVPORT_MAP_ADDR(idx)	0x148030 + ((idx)*4)	// 4 x 4

//#define IVPORT_MAP_PORTNUM(idx)		GENMASK(4 + idx*5, 0 + idx*5)
#define IVPORT_MAP_PORTNUM0		GENMASK(4, 0)
#define IVPORT_MAP_PORTNUM1		GENMASK(9, 5)
#define IVPORT_MAP_PORTNUM2		GENMASK(14, 10)
#define IVPORT_MAP_PORTNUM3		GENMASK(19, 15)
#define IVPORT_MAP_PORTNUM4		GENMASK(24, 19)
#define IVPORT_MAP_PORTNUM5		GENMASK(29, 25)

#define IVPORT_MAP_PORTNUM_EN		BIT(15)
#define MODIFY_VPORT_MAP_EN		BIT(31)

// 0-9 total 10 maps
struct vlan_vport_map {
	u16 new_ivport		: 5;
	u16 key_iport		: 5;
	u16 key_vlan_id		: 12;
	u16 valid		: 1;
	u16 rsv			: 9;
} __packed;	//<=32

union vlan_vport_map_cfg {
	struct vlan_vport_map table;
	u32 data[1];
};

// 0-4 total 5 maps
struct modify_vport_map {
	u32 port_bitmap		: 27;
	u32 rsv			: 4;
	u32 vport_map_en	: 1;
} __packed;	// <=32

union modify_vport_map_cfg {
	struct modify_vport_map table;
	u32 data[1];
};

// 0-3 total 4 maps
struct tmu_ivport_map {
	u8 map0_iportnum	: 5;
	u8 map1_iportnum	: 5;
	u8 map2_iportnum	: 5;
	u8 map3_iportnum	: 5;
	u8 map4_iportnum	: 5;
	u8 map5_iportnum	: 5;
	u8 rsv			: 2;
} __packed;	//<=32

union tmu_ivport_map_cfg {
	struct tmu_ivport_map table;
	u32 data[1];
};

// total 1 maps
struct tmu_ivport_map_enable {
	u16 map24_iportnum	: 5;
	u16 map25_iportnum	: 5;
	u16 map26_iportnum	: 5;
	u16 ivport_map_enable	: 1;
	u32 rsv			: 16;
} __packed;	//<=32

union tmu_ivport_map_enable_cfg {
	struct tmu_ivport_map_enable table;
	u32 data[1];
};

void vlan_vport_map_dump(VLAN_t *priv, u8 index);

void modify_vport_map_dump(VLAN_t *priv);

void tmu_ivport_map_dump(VLAN_t *priv);

void modify_vport_map_en(VLAN_t *priv, u8 valid);

void tmu_ivport_map_en(VLAN_t *priv, u8 valid);

void vlan_vport_map_write(VLAN_t *priv, u8 index, u16 vid,
			u8 iport, u8 ivport, u8 valid);

void modify_vport_map_update(VLAN_t *priv, u8 port, u8 ovport);

void modify_vport_map_reset(VLAN_t *priv, u8 port, u8 ovport);

void tmu_ivport_map_write(VLAN_t *priv, u8 index, u8 iport0,u8 iport1,
			   u8 iport2, u8 iport3, u8 iport4, u8 iport5);

void tmu_ivport_map_update(VLAN_t *priv, u8 index, u8 ivport_idx, u8 iport);

int vport_update(VLAN_t *priv, u16 vid, u8 port, u8 vport);

int vport_reset(VLAN_t *priv, u16 vid, u8 port, u8 vport);

void tmu_ivport_map_init(VLAN_t *priv);
