// SPDX-License-Identifier: GPL-2.0

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/iopoll.h>

#include "cs752x_ne.h"

#define FETOP_FE_AN_BNG_MAC_ACCESS		0x0004
#define FETOP_FE_L4_PORT_RNGS_ACCESS		0x0010
#define FETOP_FE_ETYPE_ACCESS			0x001c
#define FETOP_FE_LLC_HDR_ACCESS			0x0024
#define FETOP_FE_PRSR_CFG_0			0x003c
#define FETOP_FE_STS_GLBL			0x0044
#define FETOP_FE_LPB_ACCESS			0x004c
#define FETOP_FE_CLASS_ACCESS			0x0054
#define FETOP_FE_SDB_ACCESS			0x00a8
#define FETOP_FE_VLN_ACCESS			0x00c8
#define FETOP_FE_FWDRSLT_ACCESS			0x00d4
#define FETOP_FE_QOSRSLT_ACCESS			0x00e8
#define FETOP_FE_VOQPOL_ACCESS			0x00f4
#define FETOP_FE_FVLAN_ACCESS			0x0100
#define FETOP_FE_L3TBL_ACCESS			0x010c
#define FETOP_FE_L2TBL_ACCESS			0x0124
#define FETOP_FE_ACLRULE_ACCESS			0x0138
#define FETOP_FE_ACLACTION_ACCESS		0x01ac
#define FETOP_FE_PE_CONFIG_1			0x0224
#define FETOP_FE_PE_VOQ_DRP_ACCESS		0x023c
#define FETOP_FE_PKTLEN_RNGS_ACCESS		0x0288
#define FETOP_HASH_STATUS			0x2000
#define FETOP_HASH_INIT				0x2004
#define FETOP_HASH_MASK_MEM_ACCESS		0x2068

#define FE_TABLE_ACCESS_START			BIT(31)
#define FE_TABLE_ACCESS_RBW_WRITE		BIT(30)

#define FE_STS_GLBL_MEM_INIT_DONE		BIT(0)

#define FE_HASH_STATUS_INIT_IN_PROGRESS		BIT(0)
#define FE_HASH_STATUS_READY_FOR_INIT		BIT(1)
#define FE_HASH_INIT_INIT			BIT(0)

#define FE_PE_CONFIG_1_INS_CPUHDR_VOQ		GENMASK(7, 0)
#define FE_PE_CONFIG_1_INS_CPUHDR_CRYPTO	BIT(8)
#define FE_PE_CONFIG_1_INS_CPUHDR_ENCAP		BIT(9)
#define FE_PE_CONFIG_1_PARSE_LIMIT		GENMASK(15, 11)

#define FE_PE_CONFIG_1_ALL_CPU_PORTS		0xff
#define FE_PE_CONFIG_1_PARSE_LIMIT_MIN_CYCLE	0x18

#define FE_AN_BNG_MAC_ENTRY_MAX			15
#define FE_L4_PORT_RNGS_MAX			32
#define FE_ETH_CAM_ENTRY_MAX			63
#define FE_LLC_ENTRY_MAX			3
#define FE_LPB_ENTRY_MAX			8
#define FE_CLASS_ENTRY_MAX			64
#define FE_SDB_ENTRY_MAX			64
#define FE_VLAN_ENTRY_MAX			4096
#define FE_FWD_RESULT_ENTRY_MAX			8192
#define FE_QOS_RESULT_ENTRY_MAX			128
#define FE_VOQ_POL_ENTRY_MAX			512
#define FE_FVLAN_ENTRY_MAX			512
#define FE_L3_ADDR_ENTRY_MAX			1024
#define FE_L2_ADDR_PAIR_ENTRY_MAX		512
#define FE_ACL_ENTRY_MAX			128
#define FE_PE_VOQ_DROP_ENTRY_MAX		32
#define FE_HASH_MASK_ENTRY_MAX			64
#define FE_PKTLEN_RANGE_ENTRY_MAX		4

#define FE_TABLE_DATA_REGS_MAX			28

#define FE_TABLE_ACCESS_TIMEOUT_US		10000
#define FE_HASH_INIT_POLL_US			1000
#define FE_HASH_INIT_TIMEOUT_US			1000000

#define CS_GE_PORT_COUNT			3
#define CS_CPU_PORT0_VOQ_BASE			48
#define CS_CPU_PORT_VOQ_STRIDE			8

#define CS_LPB_PVID_NONE			4095

#define CS_CLASS_PORT_RULE_PRIORITY		50
#define CS_CLASS_CATCHALL_RULE_PRIORITY		0
#define CS_CLASS_CATCHALL_IDX			CS_GE_PORT_COUNT

#define CS_FWD_ENTRY_FIRST_IDX			1

struct fe_table {
	const char *name;
	u16 access;
	u16 entries;
	u8 addr_bits;
	u8 nregs;
};

struct fe_field {
	u8 reg;
	u8 bit;
	u8 width;
};

#define FE_TABLE(_name, _access, _entries, _addr_bits, _nregs)		\
	{								\
		.name = _name,						\
		.access = _access,					\
		.entries = _entries,					\
		.addr_bits = _addr_bits,				\
		.nregs = _nregs,					\
	}

#define FE_FIELD(_reg, _bit, _width)					\
	(&(const struct fe_field){					\
		.reg = _reg,						\
		.bit = _bit,						\
		.width = _width,					\
	})

static const struct fe_table fe_table_an_bng_mac =
	FE_TABLE("AN_BNG_MAC", FETOP_FE_AN_BNG_MAC_ACCESS,
		 FE_AN_BNG_MAC_ENTRY_MAX, 4, 2);
static const struct fe_table fe_table_l4_port_rngs =
	FE_TABLE("L4_PORT_RNGS", FETOP_FE_L4_PORT_RNGS_ACCESS,
		 FE_L4_PORT_RNGS_MAX, 5, 2);
static const struct fe_table fe_table_etype =
	FE_TABLE("ETYPE", FETOP_FE_ETYPE_ACCESS, FE_ETH_CAM_ENTRY_MAX, 6, 1);
static const struct fe_table fe_table_llc_hdr =
	FE_TABLE("LLC_HDR", FETOP_FE_LLC_HDR_ACCESS, FE_LLC_ENTRY_MAX, 2, 1);
static const struct fe_table fe_table_lpb =
	FE_TABLE("LPB", FETOP_FE_LPB_ACCESS, FE_LPB_ENTRY_MAX, 3, 1);
static const struct fe_table fe_table_class =
	FE_TABLE("CLASS", FETOP_FE_CLASS_ACCESS, FE_CLASS_ENTRY_MAX, 6, 20);
static const struct fe_table fe_table_sdb =
	FE_TABLE("SDB", FETOP_FE_SDB_ACCESS, FE_SDB_ENTRY_MAX, 6, 7);
static const struct fe_table fe_table_vln =
	FE_TABLE("VLN", FETOP_FE_VLN_ACCESS, FE_VLAN_ENTRY_MAX, 12, 2);
static const struct fe_table fe_table_fwdrslt =
	FE_TABLE("FWDRSLT", FETOP_FE_FWDRSLT_ACCESS,
		 FE_FWD_RESULT_ENTRY_MAX, 13, 4);
static const struct fe_table fe_table_qosrslt =
	FE_TABLE("QOSRSLT", FETOP_FE_QOSRSLT_ACCESS,
		 FE_QOS_RESULT_ENTRY_MAX, 7, 2);
static const struct fe_table fe_table_voqpol =
	FE_TABLE("VOQPOL", FETOP_FE_VOQPOL_ACCESS, FE_VOQ_POL_ENTRY_MAX, 9, 2);
static const struct fe_table fe_table_fvlan =
	FE_TABLE("FVLAN", FETOP_FE_FVLAN_ACCESS, FE_FVLAN_ENTRY_MAX, 9, 2);
static const struct fe_table fe_table_l3tbl =
	FE_TABLE("L3TBL", FETOP_FE_L3TBL_ACCESS, FE_L3_ADDR_ENTRY_MAX, 10, 5);
static const struct fe_table fe_table_l2tbl =
	FE_TABLE("L2TBL", FETOP_FE_L2TBL_ACCESS,
		 FE_L2_ADDR_PAIR_ENTRY_MAX, 9, 4);
static const struct fe_table fe_table_aclrule =
	FE_TABLE("ACLRULE", FETOP_FE_ACLRULE_ACCESS, FE_ACL_ENTRY_MAX, 7, 28);
static const struct fe_table fe_table_aclaction =
	FE_TABLE("ACLACTION", FETOP_FE_ACLACTION_ACCESS,
		 FE_ACL_ENTRY_MAX, 7, 10);
static const struct fe_table fe_table_pe_voq_drp =
	FE_TABLE("PE_VOQ_DRP", FETOP_FE_PE_VOQ_DRP_ACCESS,
		 FE_PE_VOQ_DROP_ENTRY_MAX, 5, 1);
static const struct fe_table fe_table_hash_mask =
	FE_TABLE("HASH_MASK", FETOP_HASH_MASK_MEM_ACCESS,
		 FE_HASH_MASK_ENTRY_MAX, 6, 5);
static const struct fe_table fe_table_pktlen_rngs =
	FE_TABLE("PKTLEN_RNGS", FETOP_FE_PKTLEN_RNGS_ACCESS,
		 FE_PKTLEN_RANGE_ENTRY_MAX, 2, 1);

static const struct fe_table *const fe_flush_tables[] = {
	&fe_table_an_bng_mac,
	&fe_table_l4_port_rngs,
	&fe_table_etype,
	&fe_table_llc_hdr,
	&fe_table_lpb,
	&fe_table_class,
	&fe_table_sdb,
	&fe_table_vln,
	&fe_table_fwdrslt,
	&fe_table_qosrslt,
	&fe_table_voqpol,
	&fe_table_fvlan,
	&fe_table_l3tbl,
	&fe_table_l2tbl,
	&fe_table_aclrule,
	&fe_table_aclaction,
	&fe_table_pe_voq_drp,
	&fe_table_hash_mask,
	&fe_table_pktlen_rngs,
};

#define FE_LPB_LSPID				FE_FIELD(0, 0, 4)
#define FE_LPB_PVID				FE_FIELD(0, 4, 12)
#define FE_LPB_MEM_PARITY			FE_FIELD(0, 24, 1)

#define FE_CLASS_SVIDX				FE_FIELD(0, 0, 6)
#define FE_CLASS_LSPID				FE_FIELD(0, 6, 4)
#define FE_CLASS_LSPID_MASK			FE_FIELD(15, 1, 1)
#define FE_CLASS_IP_DA_MASK			FE_FIELD(16, 12, 9)
#define FE_CLASS_IP_SA_MASK			FE_FIELD(16, 21, 9)
#define FE_CLASS_MCGID				FE_FIELD(18, 12, 9)
#define FE_CLASS_MCGID_MASK			FE_FIELD(18, 21, 9)
#define FE_CLASS_RSVD_621_606			FE_FIELD(18, 30, 16)
#define FE_CLASS_RULE_PRI			FE_FIELD(19, 14, 6)
#define FE_CLASS_ENTRY_VLD			FE_FIELD(19, 20, 1)
#define FE_CLASS_MEM_PARITY			FE_FIELD(19, 21, 1)
#define FE_CLASS_DATA19_RSRVD			FE_FIELD(19, 22, 10)

#define FE_SDB_TTL_HOPLIMIT_ZERO_DISCARD_EN	FE_FIELD(4, 26, 1)
#define FE_SDB_UU_FLOWIDX			FE_FIELD(5, 1, 13)
#define FE_SDB_BC_FLOWIDX			FE_FIELD(5, 16, 13)
#define FE_SDB_UM_FLOWIDX			FE_FIELD(5, 29, 13)
#define FE_SDB_DROP				FE_FIELD(6, 11, 1)
#define FE_SDB_ACL_DISABLE			FE_FIELD(6, 13, 1)
#define FE_SDB_MEM_PARITY			FE_FIELD(6, 14, 1)

#define FWD_VOQ_POLICY				FE_FIELD(0, 0, 1)
#define FWD_POL_POLICY				FE_FIELD(0, 1, 2)
#define FWD_VOQ_POL_TBL_IDX			FE_FIELD(0, 3, 9)
#define FWD_DROP				FE_FIELD(3, 14, 1)
#define FWD_MEM_PARITY				FE_FIELD(3, 17, 1)

#define FWD_VOQPOL_VOQ_BASE			FE_FIELD(0, 0, 8)
#define FWD_VOQPOL_COS_NOP			FE_FIELD(1, 7, 1)
#define FWD_VOQPOL_MEM_PARITY			FE_FIELD(1, 8, 1)

static void fe_set_field(u32 *data, const struct fe_field *f, u32 val)
{
	u64 mask = GENMASK_ULL(f->bit + f->width - 1, f->bit);
	u64 v = ((u64)val << f->bit) & mask;

	data[f->reg] &= ~lower_32_bits(mask);
	data[f->reg] |= lower_32_bits(v);

	if (upper_32_bits(mask)) {
		data[f->reg + 1] &= ~upper_32_bits(mask);
		data[f->reg + 1] |= upper_32_bits(v);
	}
}

static int fe_table_write(struct device *dev, void __iomem *fe,
			  const struct fe_table *t, unsigned int idx,
			  const u32 *data)
{
	unsigned int i;
	u32 val;
	int ret;

	for (i = 0; i < t->nregs; i++)
		writel(data[i], fe + t->access + (t->nregs - i) * 4);

	writel(FE_TABLE_ACCESS_START | FE_TABLE_ACCESS_RBW_WRITE |
	       (idx & GENMASK(t->addr_bits - 1, 0)), fe + t->access);

	ret = readl_poll_timeout(fe + t->access, val,
				 !(val & FE_TABLE_ACCESS_START), 0,
				 FE_TABLE_ACCESS_TIMEOUT_US);
	if (ret)
		dev_err(dev, "FE %s table entry %u write timeout\n",
			t->name, idx);

	return ret;
}

static int fe_table_flush(struct device *dev, void __iomem *fe,
			  const struct fe_table *t)
{
	static const u32 zero[FE_TABLE_DATA_REGS_MAX];
	unsigned int idx;
	int ret;

	for (idx = 0; idx < t->entries; idx++) {
		ret = fe_table_write(dev, fe, t, idx, zero);
		if (ret)
			return ret;
	}

	dev_dbg(dev, "FE %s table flushed (%u entries)\n", t->name,
		t->entries);

	return 0;
}

static int fe_hash_init(struct device *dev, void __iomem *fe)
{
	u32 val;
	int ret;

	ret = readl_poll_timeout(fe + FETOP_HASH_STATUS, val,
				 val & FE_HASH_STATUS_READY_FOR_INIT,
				 FE_HASH_INIT_POLL_US, FE_HASH_INIT_TIMEOUT_US);
	if (ret) {
		dev_err(dev, "FE hash not ready for init: 0x%08x\n", val);
		return ret;
	}

	writel(FE_HASH_INIT_INIT, fe + FETOP_HASH_INIT);

	ret = read_poll_timeout(readl, val,
				!(val & FE_HASH_STATUS_INIT_IN_PROGRESS),
				FE_HASH_INIT_POLL_US, FE_HASH_INIT_TIMEOUT_US,
				true, fe + FETOP_HASH_STATUS);
	if (ret) {
		dev_err(dev, "FE hash init timeout: 0x%08x\n", val);
		return ret;
	}

	return 0;
}

static int fe_lpb_write(struct device *dev, void __iomem *fe,
			unsigned int lspid)
{
	u32 data[1] = {};

	fe_set_field(data, FE_LPB_LSPID, lspid);
	fe_set_field(data, FE_LPB_PVID, CS_LPB_PVID_NONE);
	fe_set_field(data, FE_LPB_MEM_PARITY, 0);

	return fe_table_write(dev, fe, &fe_table_lpb, lspid, data);
}

static int fe_voqpol_write(struct device *dev, void __iomem *fe,
			   unsigned int idx, unsigned int voq_base)
{
	u32 data[2] = {};

	fe_set_field(data, FWD_VOQPOL_VOQ_BASE, voq_base);
	fe_set_field(data, FWD_VOQPOL_COS_NOP, 0);
	fe_set_field(data, FWD_VOQPOL_MEM_PARITY, 0);

	return fe_table_write(dev, fe, &fe_table_voqpol, idx, data);
}

static int fe_fwdrslt_write(struct device *dev, void __iomem *fe,
			    unsigned int idx, unsigned int voqpol_idx)
{
	u32 data[4] = {};

	fe_set_field(data, FWD_VOQ_POLICY, 1);
	fe_set_field(data, FWD_POL_POLICY, 0);
	fe_set_field(data, FWD_VOQ_POL_TBL_IDX, voqpol_idx);
	fe_set_field(data, FWD_DROP, 0);
	fe_set_field(data, FWD_MEM_PARITY, 0);

	return fe_table_write(dev, fe, &fe_table_fwdrslt, idx, data);
}

static int fe_sdb_write(struct device *dev, void __iomem *fe,
			unsigned int idx, unsigned int fwdrslt_idx)
{
	u32 data[7] = {};

	fe_set_field(data, FE_SDB_TTL_HOPLIMIT_ZERO_DISCARD_EN, 1);
	fe_set_field(data, FE_SDB_UU_FLOWIDX, fwdrslt_idx);
	fe_set_field(data, FE_SDB_BC_FLOWIDX, fwdrslt_idx);
	fe_set_field(data, FE_SDB_UM_FLOWIDX, fwdrslt_idx);
	fe_set_field(data, FE_SDB_DROP, 0);
	fe_set_field(data, FE_SDB_ACL_DISABLE, 0);
	fe_set_field(data, FE_SDB_MEM_PARITY, 0);

	return fe_table_write(dev, fe, &fe_table_sdb, idx, data);
}

static int fe_class_write(struct device *dev, void __iomem *fe,
			  unsigned int idx, unsigned int sdb_idx,
			  unsigned int lspid, unsigned int lspid_mask,
			  unsigned int rule_priority)
{
	u32 data[20];
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(data); i++)
		data[i] = U32_MAX;

	fe_set_field(data, FE_CLASS_SVIDX, sdb_idx);
	fe_set_field(data, FE_CLASS_LSPID, lspid);
	fe_set_field(data, FE_CLASS_LSPID_MASK, lspid_mask);
	fe_set_field(data, FE_CLASS_IP_DA_MASK, 0);
	fe_set_field(data, FE_CLASS_IP_SA_MASK, 0);
	fe_set_field(data, FE_CLASS_MCGID, 0);
	fe_set_field(data, FE_CLASS_MCGID_MASK, 0);
	fe_set_field(data, FE_CLASS_RSVD_621_606, 0);
	fe_set_field(data, FE_CLASS_RULE_PRI, rule_priority);
	fe_set_field(data, FE_CLASS_ENTRY_VLD, 1);
	fe_set_field(data, FE_CLASS_MEM_PARITY, 0);
	fe_set_field(data, FE_CLASS_DATA19_RSRVD, 0);

	return fe_table_write(dev, fe, &fe_table_class, idx, data);
}

static int fe_cpu_path_setup(struct device *dev, void __iomem *fe,
			     unsigned int idx, unsigned int lspid,
			     unsigned int lspid_mask, unsigned int voq_base,
			     unsigned int rule_priority)
{
	unsigned int fwd_idx = CS_FWD_ENTRY_FIRST_IDX + idx;
	int ret;

	ret = fe_voqpol_write(dev, fe, fwd_idx, voq_base);
	if (ret)
		return ret;

	ret = fe_fwdrslt_write(dev, fe, fwd_idx, fwd_idx);
	if (ret)
		return ret;

	ret = fe_sdb_write(dev, fe, idx, fwd_idx);
	if (ret)
		return ret;

	ret = fe_class_write(dev, fe, idx, idx, lspid, lspid_mask,
			     rule_priority);
	if (ret)
		return ret;

	dev_dbg(dev, "FE class %u: lspid %u mask %u -> sdb %u fwd %u voq %u\n",
		idx, lspid, lspid_mask, idx, fwd_idx, voq_base);

	return 0;
}

int cs752x_fe_init(struct device *dev, void __iomem *fe)
{
	unsigned int i;
	u32 val;
	int ret;

	val = readl(fe + FETOP_FE_STS_GLBL);
	if (!(val & FE_STS_GLBL_MEM_INIT_DONE))
		dev_dbg(dev, "FE_STS_GLBL mem_init_done not set: 0x%08x\n",
			val);

	for (i = 0; i < ARRAY_SIZE(fe_flush_tables); i++) {
		ret = fe_table_flush(dev, fe, fe_flush_tables[i]);
		if (ret)
			return ret;
	}

	ret = fe_hash_init(dev, fe);
	if (ret)
		return ret;

	for (i = 0; i < FE_LPB_ENTRY_MAX; i++) {
		ret = fe_lpb_write(dev, fe, i);
		if (ret)
			return ret;
	}

	for (i = 0; i < CS_GE_PORT_COUNT; i++) {
		ret = fe_cpu_path_setup(dev, fe, i, i, 0,
					CS_CPU_PORT0_VOQ_BASE +
					i * CS_CPU_PORT_VOQ_STRIDE,
					CS_CLASS_PORT_RULE_PRIORITY);
		if (ret)
			return ret;
	}

	ret = fe_cpu_path_setup(dev, fe, CS_CLASS_CATCHALL_IDX, 0, 1,
				CS_CPU_PORT0_VOQ_BASE,
				CS_CLASS_CATCHALL_RULE_PRIORITY);
	if (ret)
		return ret;

	writel(FIELD_PREP(FE_PE_CONFIG_1_INS_CPUHDR_VOQ,
			  FE_PE_CONFIG_1_ALL_CPU_PORTS) |
	       FE_PE_CONFIG_1_INS_CPUHDR_CRYPTO |
	       FE_PE_CONFIG_1_INS_CPUHDR_ENCAP |
	       FIELD_PREP(FE_PE_CONFIG_1_PARSE_LIMIT,
			  FE_PE_CONFIG_1_PARSE_LIMIT_MIN_CYCLE),
	       fe + FETOP_FE_PE_CONFIG_1);

	dev_dbg(dev, "FE_PE_CONFIG_1 0x%08x, FE_PRSR_CFG_0 0x%08x\n",
		readl(fe + FETOP_FE_PE_CONFIG_1),
		readl(fe + FETOP_FE_PRSR_CFG_0));

	return 0;
}
