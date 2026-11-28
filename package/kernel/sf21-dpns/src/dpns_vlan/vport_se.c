#include "vport_se.h"

//index 0-9
void vlan_vport_map_write(VLAN_t *priv, u8 index, u16 vid,
			u8 iport, u8 ivport, u8 valid)
{
	union vlan_vport_map_cfg param = {0};

	param.table.valid 	= valid;
	param.table.key_vlan_id = vid;
	param.table.key_iport 	= iport;
	param.table.new_ivport	= ivport;

	reg_write(priv, VLAN_VPORT_MAP_ADDR(index), param.data[0]);
}

void vlan_vport_map_dump(VLAN_t *priv, u8 index)
{
	union vlan_vport_map_cfg param = {0};

	param.data[0] = reg_read(priv, VLAN_VPORT_MAP_ADDR(index));

	printk("\n---------------------------------------\n");
	printk("vlan vport map table:\n");
	printk("\t rsv			%u\n",		param.table.rsv);
	printk("\t valid		%u\n",		param.table.valid);
	printk("\t key_vlan_id		%u\n",		param.table.key_vlan_id);
	printk("\t key_iport		%u\n",		param.table.key_iport);
	printk("\t new_ivport		%u\n",		param.table.new_ivport);
}

//port 0-5
void modify_vport_map_update(VLAN_t *priv, u8 port, u8 ovport)
{
	reg_update(priv, MODIFY_VPORT_MAP_ADDR(port), 0, BIT(ovport));
}

void modify_vport_map_reset(VLAN_t *priv, u8 port, u8 ovport)
{
	reg_update(priv, MODIFY_VPORT_MAP_ADDR(port), BIT(ovport), 0);
}

void modify_vport_map_en(VLAN_t *priv, u8 valid)
{
	if (valid)
		reg_update(priv, MODIFY_VPORT_MAP_ADDR(5), 0, MODIFY_VPORT_MAP_EN);
	else
		reg_update(priv, MODIFY_VPORT_MAP_ADDR(5), MODIFY_VPORT_MAP_EN, 0);
}

void modify_vport_map_dump(VLAN_t *priv)
{
	union modify_vport_map_cfg param = {0};
	int i;

	printk("\n---------------------------------------\n");
	printk("modify vport map table:\n");
	for(i = 0; i < 6; i++) {
		param.data[0] = reg_read(priv, MODIFY_VPORT_MAP_ADDR(i));

		printk("\t port%dbitmap	%x\n", i, param.table.port_bitmap);

		if (i == 5)
			printk("\t valid %u\n", param.table.vport_map_en);
	}
}

//index0-4
void tmu_ivport_map_write(VLAN_t *priv, u8 index, u8 iport0,u8 iport1,
			   u8 iport2, u8 iport3, u8 iport4, u8 iport5)
{
	union tmu_ivport_map_cfg param = {0};
	union tmu_ivport_map_enable_cfg param_en = {0};

	if (index == 4) {
		//param_en.table.ivport_map_enable = 1;//如果可以循环配置，那用tmu_ivport_map_update
		param_en.table.map24_iportnum = iport0;
		param_en.table.map25_iportnum = iport1;
		param_en.table.map26_iportnum = iport2;

		printk("\t param_en.data[0]	%u\n",		param_en.data[0]);

		reg_write(priv, TMU_IVPORT_MAP_ADDR(index), param_en.data[0]);
	} else {
		param.table.map0_iportnum = iport0;
		param.table.map1_iportnum = iport1;
		param.table.map2_iportnum = iport2;
		param.table.map3_iportnum = iport3;
		param.table.map4_iportnum = iport4;
		param.table.map5_iportnum = iport5;

		printk("\t param.data[0]	%u\n",		param.data[0]);

		reg_write(priv, TMU_IVPORT_MAP_ADDR(index), param.data[0]);
	}
}

void tmu_ivport_map_init(VLAN_t *priv)
{
	int i;

	for (i = 0; i < 5; i++) {
		tmu_ivport_map_write(priv, i, 0+i*6, 1+i*6, 2+i*6, 3+i*6, 4+i*6, 5+i*6);
	}
}

//index0-4 ivport_idx 0-5,index为4的时候，ivport_idx最多只能是2
void tmu_ivport_map_update(VLAN_t *priv, u8 index, u8 ivport_idx, u8 iport)
{
	switch (ivport_idx)
	{
		case 0:
			reg_update(priv, TMU_IVPORT_MAP_ADDR(index),
				FIELD_PREP(IVPORT_MAP_PORTNUM0, 0x1f),
				FIELD_PREP(IVPORT_MAP_PORTNUM0, iport));
			break;

		case 1:
			reg_update(priv, TMU_IVPORT_MAP_ADDR(index),
				FIELD_PREP(IVPORT_MAP_PORTNUM1, 0x1f),
				FIELD_PREP(IVPORT_MAP_PORTNUM1, iport));
			break;

		case 2:
			reg_update(priv, TMU_IVPORT_MAP_ADDR(index),
				FIELD_PREP(IVPORT_MAP_PORTNUM2, 0x1f),
				FIELD_PREP(IVPORT_MAP_PORTNUM2, iport));
			break;

		case 3:
			reg_update(priv, TMU_IVPORT_MAP_ADDR(index),
				FIELD_PREP(IVPORT_MAP_PORTNUM3, 0x1f),
				FIELD_PREP(IVPORT_MAP_PORTNUM3, iport));
			break;

		case 4:
			reg_update(priv, TMU_IVPORT_MAP_ADDR(index),
				FIELD_PREP(IVPORT_MAP_PORTNUM4, 0x1f),
				FIELD_PREP(IVPORT_MAP_PORTNUM4, iport));
			break;

		case 5:
			reg_update(priv, TMU_IVPORT_MAP_ADDR(index),
				FIELD_PREP(IVPORT_MAP_PORTNUM5, 0x1f),
				FIELD_PREP(IVPORT_MAP_PORTNUM5, iport));
			break;

		default:
			break;
	}

}

void tmu_ivport_map_en(VLAN_t *priv, u8 valid)
{
	if (valid)
		reg_update(priv, TMU_IVPORT_MAP_ADDR(4), 0, IVPORT_MAP_PORTNUM_EN);
	else
		reg_update(priv, TMU_IVPORT_MAP_ADDR(4), IVPORT_MAP_PORTNUM_EN, 0);
}

void tmu_ivport_map_dump(VLAN_t *priv)
{
	union tmu_ivport_map_cfg param = {0};
	union tmu_ivport_map_enable_cfg param_en = {0};
	int i;

	printk("\n---------------------------------------\n");
	printk("tmu ivport map table:\n");
	for(i = 0; i < 4; i++) {
		param.data[0] = reg_read(priv, TMU_IVPORT_MAP_ADDR(i));

		printk("\t ivport%d_map_iportnum	%u\n", 0+i*6, param.table.map0_iportnum);
		printk("\t ivport%d_map_iportnum	%u\n", 1+i*6, param.table.map1_iportnum);
		printk("\t ivport%d_map_iportnum	%u\n", 2+i*6, param.table.map2_iportnum);
		printk("\t ivport%d_map_iportnum	%u\n", 3+i*6, param.table.map3_iportnum);
		printk("\t ivport%d_map_iportnum	%u\n", 4+i*6, param.table.map4_iportnum);
		printk("\t ivport%d_map_iportnum	%u\n", 5+i*6, param.table.map5_iportnum);
	}

	param_en.data[0] = reg_read(priv, TMU_IVPORT_MAP_ADDR(4));

	printk("\t ivport24_map_iportnum	%u\n", param_en.table.map24_iportnum);
	printk("\t ivport25_map_iportnum	%u\n", param_en.table.map25_iportnum);
	printk("\t ivport26_map_iportnum	%u\n", param_en.table.map26_iportnum);

	printk("\t valid			%u\n", param_en.table.ivport_map_enable);
}
