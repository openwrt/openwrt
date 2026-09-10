// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bug.h>
#include <linux/errno.h>
#include <linux/mfd/syscon.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/string.h>

#include <linux/soc/realtek/otto_table.h>

/* The switch register block, taken from the parent syscon at probe. Every
 * access below goes through it, so it is also what says the driver is up.
 */
static struct regmap *otto_map;

/* One table access register: a command register selecting table and index,
 * and a window of data registers holding the entry.
 */
struct otto_table {
	u16 addr;
	u16 data;
	u8 c_bit;
	u8 t_bit;
	u8 rmode;
	struct mutex lock;
};

/* One row per table access register: name, command register, data window,
 * how many data registers the window holds, the read/write bit, the shift the
 * table type sits at, and whether the read/write bit is inverted. The enum,
 * the descriptors and the per-register width limit are all generated from
 * here, so an entry cannot be described in one place and not in the others.
 */
#define OTTO_REG_LIST(_)						\
	_(OTTO_REG_8380_L2,  0x6900, 0x6908,  3, 15, 13, 1)	\
	_(OTTO_REG_8380_0,   0x6914, 0x6918, 18, 14, 12, 1)	\
	_(OTTO_REG_8380_1,   0xA4C8, 0xA4CC,  6, 14, 12, 1)	\
	_(OTTO_REG_8390_L2,  0x1180, 0x1184,  3, 16, 14, 0)	\
	_(OTTO_REG_8390_0,   0x1190, 0x1194, 17, 15, 12, 0)	\
	_(OTTO_REG_8390_1,   0x6B80, 0x6B84,  4, 14, 12, 0)	\
	_(OTTO_REG_8390_2,   0x611C, 0x6120,  9,  8,  6, 0)	\
	_(OTTO_REG_9300_L2,  0xB320, 0xB334,  3, 18, 16, 0)	\
	_(OTTO_REG_9300_0,   0xB340, 0xB344, 19, 16, 12, 0)	\
	_(OTTO_REG_9300_1,   0xB3A0, 0xB3A4, 20, 16, 13, 0)	\
	_(OTTO_REG_9300_2,   0xCE04, 0xCE08,  6, 14, 12, 0)	\
	_(OTTO_REG_9300_HSB, 0xD600, 0xD604, 30,  7,  6, 0)	\
	_(OTTO_REG_9300_HSA, 0x7880, 0x7884, 22,  9,  8, 0)	\
	_(OTTO_REG_9310_0,   0x8500, 0x8508,  8, 19, 15, 0)	\
	_(OTTO_REG_9310_1,   0x40C0, 0x40C4, 22, 16, 14, 0)	\
	_(OTTO_REG_9310_2,   0x8528, 0x852C,  6, 18, 14, 0)	\
	_(OTTO_REG_9310_3,   0x0200, 0x0204,  9, 15, 12, 0)	\
	_(OTTO_REG_9310_4,   0x20dc, 0x20e0, 29,  7,  6, 0)	\
	_(OTTO_REG_9310_5,   0x7e1c, 0x7e20, 53,  8,  6, 0)	\

#define OTTO_REG_ENUM(name, addr, data, max, cbit, tbit, rmode)	name,

enum otto_table_reg {
	OTTO_REG_LIST(OTTO_REG_ENUM)
	OTTO_REG_END
};

#define OTTO_REG_MAX(name, addr, data, max, cbit, tbit, rmode)	\
	name##_MAX_DATA = max,

enum otto_table_reg_max {
	OTTO_REG_LIST(OTTO_REG_MAX)
};

#define OTTO_REG_DESC(name, addr_, data_, max_, cbit_, tbit_, rmode_)	\
	[name] = {							\
		.addr = addr_, .data = data_,				\
		.c_bit = cbit_, .t_bit = tbit_, .rmode = rmode_,	\
	},

static struct otto_table otto_regs[OTTO_REG_END] = {
	OTTO_REG_LIST(OTTO_REG_DESC)
};

#define OTTO_REG_MAX_DATA_(reg)	reg##_MAX_DATA
#define OTTO_REG_MAX_DATA(reg)	OTTO_REG_MAX_DATA_(reg)

/* Which access register selects a table, the type value it is selected with,
 * how many data registers one of its entries occupies and how many rows it
 * has.
 */
struct otto_table_map {
	u8 reg;
	u8 type;
	u8 width;
	u16 rows;
};

#define TBL_MAP(_id, _reg, _type, _width, _rows)			\
	[OTTO_TBL_HANDLE(_id)] = {					\
		.reg = (_reg), .type = (_type), .rows = (_rows),	\
		.width = (_width) +					\
			 BUILD_BUG_ON_ZERO((_width) >			\
					   OTTO_REG_MAX_DATA(_reg)),	\
	}

static const struct otto_table_map otto_table_maps[OTTO_TBL_COUNT] = {
	TBL_MAP(RTL8380_TBL_L2_UC, OTTO_REG_8380_L2, 0, 3, 8192),
	TBL_MAP(RTL8380_TBL_L2_IP_MC, OTTO_REG_8380_L2, 0, 3, 8192),
	TBL_MAP(RTL8380_TBL_L2_IP_MC_SIP, OTTO_REG_8380_L2, 0, 3, 8192),
	TBL_MAP(RTL8380_TBL_L2_MC, OTTO_REG_8380_L2, 0, 3, 8192),
	TBL_MAP(RTL8380_TBL_L2_NEXT_HOP, OTTO_REG_8380_L2, 0, 3, 8192),
	TBL_MAP(RTL8380_TBL_L2_NEXT_HOP_LEGACY, OTTO_REG_8380_L2, 0, 3, 8192),
	TBL_MAP(RTL8380_TBL_L2_CAM_UC, OTTO_REG_8380_L2, 1, 3, 64),
	TBL_MAP(RTL8380_TBL_L2_CAM_IP_MC, OTTO_REG_8380_L2, 1, 3, 64),
	TBL_MAP(RTL8380_TBL_L2_CAM_IP_MC_SIP, OTTO_REG_8380_L2, 1, 3, 64),
	TBL_MAP(RTL8380_TBL_L2_CAM_MC, OTTO_REG_8380_L2, 1, 3, 64),
	TBL_MAP(RTL8380_TBL_MC_PMSK, OTTO_REG_8380_L2, 2, 1, 512),

	TBL_MAP(RTL8380_TBL_VLAN, OTTO_REG_8380_0, 0, 2, 4096),
	TBL_MAP(RTL8380_TBL_IACL, OTTO_REG_8380_0, 1, 18, 1536),
	TBL_MAP(RTL8380_TBL_MSTI, OTTO_REG_8380_0, 2, 2, 64),
	TBL_MAP(RTL8380_TBL_LOG, OTTO_REG_8380_0, 3, 2, 128),

	TBL_MAP(RTL8380_TBL_UNTAG, OTTO_REG_8380_1, 0, 1, 4096),
	TBL_MAP(RTL8380_TBL_ROUTING, OTTO_REG_8380_1, 2, 2, 512),

	TBL_MAP(RTL8390_TBL_L2_UC, OTTO_REG_8390_L2, 0, 3, 16384),
	TBL_MAP(RTL8390_TBL_L2_IP_MC, OTTO_REG_8390_L2, 0, 3, 16384),
	TBL_MAP(RTL8390_TBL_L2_IP_MC_SIP, OTTO_REG_8390_L2, 0, 3, 16384),
	TBL_MAP(RTL8390_TBL_L2_MC, OTTO_REG_8390_L2, 0, 3, 16384),
	TBL_MAP(RTL8390_TBL_L2_NEXT_HOP, OTTO_REG_8390_L2, 0, 3, 16384),
	TBL_MAP(RTL8390_TBL_L2_NH_LEGACY, OTTO_REG_8390_L2, 0, 3, 16384),
	TBL_MAP(RTL8390_TBL_L2_CAM_UC, OTTO_REG_8390_L2, 1, 3, 64),
	TBL_MAP(RTL8390_TBL_L2_CAM_IP_MC, OTTO_REG_8390_L2, 1, 3, 64),
	TBL_MAP(RTL8390_TBL_L2_CAM_IP_MC_SIP, OTTO_REG_8390_L2, 1, 3, 64),
	TBL_MAP(RTL8390_TBL_L2_CAM_MC, OTTO_REG_8390_L2, 1, 3, 64),
	TBL_MAP(RTL8390_TBL_MC_PMSK, OTTO_REG_8390_L2, 2, 2, 4096),

	TBL_MAP(RTL8390_TBL_VLAN, OTTO_REG_8390_0, 0, 3, 4096),
	TBL_MAP(RTL8390_TBL_IACL, OTTO_REG_8390_0, 2, 17, 2304),
	TBL_MAP(RTL8390_TBL_EACL, OTTO_REG_8390_0, 2, 17, 2304),
	TBL_MAP(RTL8390_TBL_LOG, OTTO_REG_8390_0, 4, 2, 1024),
	TBL_MAP(RTL8390_TBL_MSTI, OTTO_REG_8390_0, 5, 4, 256),

	TBL_MAP(RTL8390_TBL_UNTAG, OTTO_REG_8390_1, 0, 2, 4096),
	TBL_MAP(RTL8390_TBL_ROUTING, OTTO_REG_8390_1, 2, 2, 2048),

	TBL_MAP(RTL9300_TBL_L2_UC, OTTO_REG_9300_L2, 0, 3, 16384),
	TBL_MAP(RTL9300_TBL_L2_MC, OTTO_REG_9300_L2, 0, 3, 16384),
	TBL_MAP(RTL9300_TBL_L2_CAM_UC, OTTO_REG_9300_L2, 1, 3, 64),
	TBL_MAP(RTL9300_TBL_L2_CAM_MC, OTTO_REG_9300_L2, 1, 3, 64),
	TBL_MAP(RTL9300_TBL_MC_PORTMASK, OTTO_REG_9300_L2, 2, 1, 1024),

	TBL_MAP(RTL9300_TBL_VLAN, OTTO_REG_9300_0, 1, 2, 4096),
	TBL_MAP(RTL9300_TBL_IACL, OTTO_REG_9300_0, 2, 19, 2048),
	TBL_MAP(RTL9300_TBL_VACL, OTTO_REG_9300_0, 2, 19, 2048),
	TBL_MAP(RTL9300_TBL_LOG, OTTO_REG_9300_0, 3, 2, 2048),
	TBL_MAP(RTL9300_TBL_MSTI, OTTO_REG_9300_0, 4, 2, 64),
	TBL_MAP(RTL9300_TBL_PORT_ISO_CTRL, OTTO_REG_9300_0, 6, 1, 1024),
	TBL_MAP(RTL9300_TBL_LAG, OTTO_REG_9300_0, 7, 3, 64),
	TBL_MAP(RTL9300_TBL_SRC_TRK_MAP, OTTO_REG_9300_0, 8, 1, 1024),

	TBL_MAP(RTL9300_TBL_L3_ROUTER_MAC, OTTO_REG_9300_1, 0, 7, 64),
	/* The host route tables hold six entries in every eight addresses. The
	 * SDK counts the 6144 entries and translates; l3.c translates first
	 * and passes the address, so the bound here is the address space.
	 */
	TBL_MAP(RTL9300_TBL_L3_HOST_ROUTE_IPUC, OTTO_REG_9300_1, 1, 5, 8192),
	TBL_MAP(RTL9300_TBL_L3_HOST_ROUTE_IP6MC, OTTO_REG_9300_1, 1, 11, 8192),
	TBL_MAP(RTL9300_TBL_L3_HOST_ROUTE_IP6UC, OTTO_REG_9300_1, 1, 5, 8192),
	TBL_MAP(RTL9300_TBL_L3_HOST_ROUTE_IPMC, OTTO_REG_9300_1, 1, 11, 8192),
	TBL_MAP(RTL9300_TBL_L3_PREFIX_ROUTE_IPUC, OTTO_REG_9300_1, 2, 11, 512),
	TBL_MAP(RTL9300_TBL_L3_PREFIX_ROUTE_IP6MC, OTTO_REG_9300_1, 2, 20, 512),
	TBL_MAP(RTL9300_TBL_L3_PREFIX_ROUTE_IP6UC, OTTO_REG_9300_1, 2, 11, 512),
	TBL_MAP(RTL9300_TBL_L3_PREFIX_ROUTE_IPMC, OTTO_REG_9300_1, 2, 20, 512),
	TBL_MAP(RTL9300_TBL_L3_NEXTHOP, OTTO_REG_9300_1, 3, 1, 2048),
	TBL_MAP(RTL9300_TBL_L3_EGR_INTF, OTTO_REG_9300_1, 4, 2, 128),

	TBL_MAP(RTL9300_TBL_UNTAG, OTTO_REG_9300_2, 0, 1, 4096),
	TBL_MAP(RTL9300_TBL_L3_EGR_INTF_MAC, OTTO_REG_9300_2, 2, 2, 2112),

	TBL_MAP(RTL9310_TBL_L2_UC, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_FT_L2_HASH_FMT0_0, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_FT_L2_HASH_FMT0_1, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_FT_L2_HASH_FMT1_0, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_FT_L2_HASH_FMT1_1, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_FT_L2_HASH_FMT2_0, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_FT_L2_HASH_FMT2_1, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_L2_CB_MC, OTTO_REG_9310_0, 0, 3, 32768),
	TBL_MAP(RTL9310_TBL_L2_CB_UC, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_L2_MC, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_L2_TNL_MCAST, OTTO_REG_9310_0, 0, 3, 32768),
	TBL_MAP(RTL9310_TBL_L2_TNL_UCAST, OTTO_REG_9310_0, 0, 3, 32768),
	TBL_MAP(RTL9310_TBL_PE_FWD, OTTO_REG_9310_0, 0, 4, 32768),
	TBL_MAP(RTL9310_TBL_WLC_MCAST, OTTO_REG_9310_0, 0, 3, 32768),
	TBL_MAP(RTL9310_TBL_WLC_UCAST, OTTO_REG_9310_0, 0, 3, 32768),
	TBL_MAP(RTL9310_TBL_L2_CAM_UC, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_FT_L2_CAM_FMT0_0, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_FT_L2_CAM_FMT0_1, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_FT_L2_CAM_FMT1_0, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_FT_L2_CAM_FMT1_1, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_FT_L2_CAM_FMT2_0, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_L2_CAM_CB_MC, OTTO_REG_9310_0, 1, 3, 128),
	TBL_MAP(RTL9310_TBL_L2_CAM_CB_UC, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_L2_CAM_MC, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_L2_TNL_MCAST_CAM, OTTO_REG_9310_0, 1, 3, 128),
	TBL_MAP(RTL9310_TBL_L2_TNL_UCAST_CAM, OTTO_REG_9310_0, 1, 3, 128),
	TBL_MAP(RTL9310_TBL_PE_FWD_CAM, OTTO_REG_9310_0, 1, 4, 128),
	TBL_MAP(RTL9310_TBL_WLC_MCAST_CAM, OTTO_REG_9310_0, 1, 3, 128),
	TBL_MAP(RTL9310_TBL_WLC_UCAST_CAM, OTTO_REG_9310_0, 1, 3, 128),
	TBL_MAP(RTL9310_TBL_MC_PMSK, OTTO_REG_9310_0, 2, 2, 4096),
	TBL_MAP(RTL9310_TBL_VLAN, OTTO_REG_9310_0, 3, 4, 4096),
	TBL_MAP(RTL9310_TBL_MSTI, OTTO_REG_9310_0, 5, 4, 128),
	TBL_MAP(RTL9310_TBL_SRC_TRK_MAP, OTTO_REG_9310_0, 13, 1, 1024),

	TBL_MAP(RTL9310_TBL_IACL, OTTO_REG_9310_1, 0, 22, 4096),
	TBL_MAP(RTL9310_TBL_EACL, OTTO_REG_9310_1, 0, 22, 4096),
	TBL_MAP(RTL9310_TBL_FT_EGR, OTTO_REG_9310_1, 0, 17, 4096),
	TBL_MAP(RTL9310_TBL_FT_IGR, OTTO_REG_9310_1, 0, 22, 4096),
	TBL_MAP(RTL9310_TBL_VACL, OTTO_REG_9310_1, 0, 22, 4096),

	TBL_MAP(RTL9310_TBL_LAG, OTTO_REG_9310_2, 0, 3, 128),
	TBL_MAP(RTL9310_TBL_PORT_ISO_CTRL, OTTO_REG_9310_2, 1, 2, 1024),

	TBL_MAP(RTL9310_TBL_VLAN_UNTAG, OTTO_REG_9310_3, 0, 2, 4096),

	TBL_MAP(RTL9310_TBL_STAT_PORT_MIB_CNTR, OTTO_REG_9310_5, 0, 53, 57),
	TBL_MAP(RTL9310_TBL_STAT_PORT_PRVTE_CNTR, OTTO_REG_9310_5, 1, 28, 57),
};

/* Whether the tables can be reached yet. A consumer that probes before this
 * driver has bound gets -EPROBE_DEFER and comes back; one running on a
 * devicetree without the node gets -ENODEV and should give up.
 */
int otto_table_loaded(void)
{
	struct device_node *np;
	bool present;

	/* Pairs with the release in probe: a map means the locks are usable */
	if (smp_load_acquire(&otto_map))
		return 0;

	np = of_find_compatible_node(NULL, NULL, "realtek,otto-table");
	present = of_device_is_available(np);
	of_node_put(np);

	return present ? -EPROBE_DEFER : -ENODEV;
}
EXPORT_SYMBOL_GPL(otto_table_loaded);

static int otto_table_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct regmap *map;

	map = syscon_node_to_regmap(dev->of_node->parent);
	if (IS_ERR(map))
		return dev_err_probe(dev, PTR_ERR(map), "no switch register map\n");

	for (int i = 0; i < OTTO_REG_END; i++)
		mutex_init(&otto_regs[i].lock);

	/* Publish the map only once the locks above can be taken */
	smp_store_release(&otto_map, map);

	return 0;
}

static const struct of_device_id otto_table_of_ids[] = {
	{ .compatible = "realtek,otto-table" },
	{ /* sentinel */ }
};

static struct platform_driver otto_table_driver = {
	.probe = otto_table_probe,
	.driver = {
		.name = "otto-table",
		.of_match_table = otto_table_of_ids,
		/* The tables are held across calls, so the driver must not go
		 * away under a consumer.
		 */
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(otto_table_driver);

static int otto_table_id_to_handle(enum otto_table_id id)
{
	if (id < OTTO_TBL_ID_BASE || id >= OTTO_TBL_END)
		return -EINVAL;

	return OTTO_TBL_HANDLE(id);
}

static enum otto_table_id otto_table_handle_to_id(int handle)
{
	return handle + OTTO_TBL_ID_BASE;
}

/* The register a table is reached through. NULL for a handle out of range,
 * which every entry point checks before touching the hardware.
 */
static struct otto_table *otto_table_reg(int handle)
{
	if (WARN_ONCE(!otto_map, "otto_table: access before the driver bound\n"))
		return NULL;

	if (handle < 0 || handle >= OTTO_TBL_COUNT)
		return NULL;

	return &otto_regs[otto_table_maps[handle].reg];
}

/* The command register masks the index to the width of its index field, which
 * cannot express the bound: it is wider than the table on 66 of the tables,
 * exactly as wide on the other 38, and six tables do not have a power-of-two
 * row count at all.
 */
static bool otto_table_index_ok(int handle, int idx)
{
	u16 rows = otto_table_maps[handle].rows;

	return !WARN_ONCE(idx < 0 || idx >= rows,
			  "otto_table: index %d in table %d, which has %u rows\n",
			  idx, otto_table_handle_to_id(handle), rows);
}

/* Take a table and hold it until otto_table_release() */
int otto_table_acquire(enum otto_table_id id)
{
	int handle = otto_table_id_to_handle(id);
	struct otto_table *r = otto_table_reg(handle);

	if (!r)
		return -EINVAL;

	mutex_lock(&r->lock);

	return handle;
}
EXPORT_SYMBOL_GPL(otto_table_acquire);

void otto_table_release(int handle)
{
	struct otto_table *r = otto_table_reg(handle);

	if (!r)
		return;

	mutex_unlock(&r->lock);
}
EXPORT_SYMBOL_GPL(otto_table_release);

static int otto_table_exec(int handle, bool is_write, int idx)
{
	const struct otto_table_map *map = &otto_table_maps[handle];
	struct otto_table *r = &otto_regs[map->reg];
	int ret = 0;
	u32 cmd, val;

	/* Read/write bit has inverted meaning on RTL838x */
	if (r->rmode)
		cmd = is_write ? 0 : BIT(r->c_bit);
	else
		cmd = is_write ? BIT(r->c_bit) : 0;

	cmd |= BIT(r->c_bit + 1); /* Execute bit */
	cmd |= map->type << r->t_bit; /* Table type */
	cmd |= idx & (BIT(r->t_bit) - 1); /* Index */

	/* Defensive pre check in case an earlier command never completed */
	ret = regmap_read_poll_timeout(otto_map, r->addr, val,
				       !(val & BIT(r->c_bit + 1)), 20, 10000);
	if (ret) {
		pr_err_ratelimited("otto_table: table %d busy, command not sent\n",
				   otto_table_handle_to_id(handle));
		return ret;
	}

	ret = regmap_write(otto_map, r->addr, cmd);
	if (ret)
		return ret;

	ret = regmap_read_poll_timeout(otto_map, r->addr, val,
				       !(val & BIT(r->c_bit + 1)), 20, 10000);
	if (ret)
		pr_err_ratelimited("otto_table: table %d did not complete\n",
				   otto_table_handle_to_id(handle));

	return ret;
}

int __otto_table_read_bytes(int handle, int idx, void *buf, int word_offset,
			    size_t size)
{
	struct otto_table *r = otto_table_reg(handle);
	unsigned int words, width;
	u32 *out = buf;
	int ret;

	/* The buffer is what this call produces. Clear it first so a refusal
	 * hands back zeroes rather than the stack the caller arrived with: no
	 * caller checks the return value, and every read-modify-write writes
	 * this buffer back to the table.
	 */
	memset(buf, 0, size);

	if (!r || !otto_table_index_ok(handle, idx))
		return -EINVAL;

	words = size / sizeof(u32);
	width = otto_table_maps[handle].width;
	if (WARN_ONCE(size % sizeof(u32) || word_offset < 0 || word_offset + words > width,
		      "otto_table: %zu bytes at word %d of table %d, a %u word entry\n",
		      size, word_offset, otto_table_handle_to_id(handle), width))
		return -EINVAL;

	ret = otto_table_exec(handle, false, idx);
	if (ret)
		return ret;

	return regmap_bulk_read(otto_map, r->data + word_offset * 4, out, words);
}
EXPORT_SYMBOL_GPL(__otto_table_read_bytes);

int __otto_table_write_bytes(int handle, int idx, const void *buf, size_t size)
{
	struct otto_table *r = otto_table_reg(handle);
	unsigned int words;
	const u32 *in = buf;
	int ret;

	if (!r || !otto_table_index_ok(handle, idx))
		return -EINVAL;

	words = otto_table_maps[handle].width;
	if (WARN_ONCE(size != words * sizeof(u32),
		      "otto_table: %zu bytes for table %d, a %u word entry\n",
		      size, otto_table_handle_to_id(handle), words))
		return -EINVAL;

	ret = regmap_bulk_write(otto_map, r->data, in, words);
	if (ret)
		return ret;

	return otto_table_exec(handle, true, idx);
}
EXPORT_SYMBOL_GPL(__otto_table_write_bytes);

int otto_table_read_bytes(enum otto_table_id id, int idx, void *buf,
			  int word_offset, size_t size)
{
	int handle = otto_table_acquire(id);
	int ret;

	if (handle < 0) {
		memset(buf, 0, size);
		return handle;
	}

	ret = __otto_table_read_bytes(handle, idx, buf, word_offset, size);
	otto_table_release(handle);

	return ret;
}
EXPORT_SYMBOL_GPL(otto_table_read_bytes);

int otto_table_write_bytes(enum otto_table_id id, int idx, const void *buf, size_t size)
{
	int handle = otto_table_acquire(id);
	int ret;

	if (handle < 0)
		return handle;

	ret = __otto_table_write_bytes(handle, idx, buf, size);
	otto_table_release(handle);

	return ret;
}
EXPORT_SYMBOL_GPL(otto_table_write_bytes);
