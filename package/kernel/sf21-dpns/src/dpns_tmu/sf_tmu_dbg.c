#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/debugfs.h>
#include <linux/stat.h>
#include <linux/bitfield.h>
#include <linux/platform_device.h>
#include "sf_tmu.h"
#include "sf_tmu_regs.h"

#define REG_OP_READ				0
#define REG_OP_WRITE				1

extern TMU_t *g_tmu;

enum component {
	PORT = 0,
	QUEUE,
	SCHED,
	SHAPER,
};

struct reg {
	u32 reg;
	u32 shift;
	u32 mask;

	u8 port;
	u8 comp; // enum component
	union {
		u8 nr_comp; // comp idx
		u8 queue;
		u8 sched;
		u8 shaper;
	} u;
};

struct dbg_desc {
	const char *name;
	const struct file_operations *fops;
	umode_t mode;
	struct reg reg;
};

static struct dentry *tmu_dir;
static struct reg *desc_regs;

static int reg_dump_show(struct seq_file *s, void *data)
{
	TMU_t *tmu = g_tmu;
	u32 i, j, val;

	if (!tmu)
		return -ENODEV;

	seq_printf(s, "TMU_VERSION: 		0x%08x\n", sf_readl(g_tmu, TMU_VERSION_INFO));
	seq_printf(s, "TMU_CTRL: 		0x%08x\n", sf_readl(g_tmu, TMU_CTRL));
	seq_printf(s, "TMU_LLM_FIFO_CTRL0: 	0x%08x\n", sf_readl(g_tmu, TMU_LLM_FIFO_CTRL0));
	seq_printf(s, "TMU_LLM_FIFO_CTRL1: 	0x%08x\n", sf_readl(g_tmu, TMU_LLM_FIFO_CTRL1));

	seq_printf(s, "\n");

	for (i = 0; i < TMU_MAX_PORT_CNT; i++) {

		seq_printf(s, "port %d\n", i);

		for (j = 0; j < TMU_PORT_QUEUE_CNT; j++) {
			seq_printf(s, "port %d queue %d:\n", i, j);

			tmu_queue_readl(tmu, i, j, TMU_PORT_QUEUE_CFG0, &val);
			seq_printf(s, "TMU_PORT_QUEUE_CFG0: 	0x%08x\n", val);

			tmu_queue_readl(tmu, i, j, TMU_PORT_QUEUE_CFG1, &val);
			seq_printf(s, "TMU_PORT_QUEUE_CFG1: 	0x%08x\n", val);

			tmu_queue_readl(tmu, i, j, TMU_PORT_QUEUE_CFG2, &val);
			seq_printf(s, "TMU_PORT_QUEUE_CFG2: 	0x%08x\n", val);

			tmu_queue_readl(tmu, i, j, TMU_PORT_QUEUE_STS0, &val);
			seq_printf(s, "TMU_PORT_QUEUE_STS0: 	0x%08x\n", val);

			tmu_queue_readl(tmu, i, j, TMU_PORT_QUEUE_STS1, &val);
			seq_printf(s, "TMU_PORT_QUEUE_STS1: 	0x%08x\n", val);

			tmu_queue_readl(tmu, i, j, TMU_PORT_QUEUE_STS2, &val);
			seq_printf(s, "TMU_PORT_QUEUE_STS2: 	0x%08x\n", val);

			tmu_queue_readl(tmu, i, j, TMU_PORT_QUEUE_CFG3, &val);
			seq_printf(s, "TMU_PORT_QUEUE_CFG3: 	0x%08x\n", val);
		}

		for (j = 0; j < TMU_SCH_CNT; j++) {
			seq_printf(s, "port %d scheduler %d:\n", i, j);

			tmu_sched_readl(tmu, i, j, TMU_SCH_CTRL, &val);
			seq_printf(s, "TMU_SCH_CTRL: 		0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q0_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q0_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q1_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q1_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q2_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q2_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q3_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q3_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q4_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q4_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q5_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q5_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q6_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q6_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_Q7_WEIGHT, &val);
			seq_printf(s, "TMU_SCH_Q7_WEIGHT: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_QUEUE_ALLOC0, &val);
			seq_printf(s, "TMU_SCH_QUEUE_ALLOC0: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_QUEUE_ALLOC1, &val);
			seq_printf(s, "TMU_SCH_QUEUE_ALLOC1: 	0x%08x\n", val);

			tmu_sched_readl(tmu, i, j, TMU_SCH_BIT_RATE, &val);
			seq_printf(s, "TMU_SCH_BIT_RATE: 	0x%08x\n", val);

			if (j == 0) {
				tmu_sched_readl(tmu, i, j, TMU_SCH0_POS, &val);
				seq_printf(s, "TMU_SCH0_POS: 		0x%08x\n", val);
			}
		}

		for (j = 0; j < TMU_SHP_CNT; j++) {
			seq_printf(s, "port %d shaper %d:\n", i, j);

			tmu_shaper_readl(tmu, i, j, TMU_SHP_CTRL, &val);
			seq_printf(s, "TMU_SHP_CTRL: 		0x%08x\n", val);

			tmu_shaper_readl(tmu, i, j, TMU_SHP_WEIGHT, &val);
			seq_printf(s, "TMU_SHP_WEIGHT: 	0x%08x\n", val);

			tmu_shaper_readl(tmu, i, j, TMU_SHP_MAX_CREDIT, &val);
			seq_printf(s, "TMU_SHP_MAX_CREDIT: 	0x%08x\n", val);

			tmu_shaper_readl(tmu, i, j, TMU_SHP_CTRL2, &val);
			seq_printf(s, "TMU_SHP_CTRL2: 		0x%08x\n", val);

			tmu_shaper_readl(tmu, i, j, TMU_SHP_MIN_CREDIT, &val);
			seq_printf(s, "TMU_SHP_MIN_CREDIT: 	0x%08x\n", val);

			tmu_shaper_readl(tmu, i, j, TMU_SHP_STATUS, &val);
			seq_printf(s, "TMU_SHP_STATUS: 	0x%08x\n", val);
		}

		seq_printf(s, "port %d tdq:\n", i);

		tmu_port_readl(tmu, i, TMU_TDQ_IFG, &val);
		seq_printf(s, "TMU_TDQ_IFG: 		0x%08x\n", val);

		tmu_port_readl(tmu, i, TMU_TDQ_CTRL, &val);
		seq_printf(s, "TMU_TDQ_CTRL: 		0x%08x\n", val);

		seq_printf(s, "\n");
	}

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(reg_dump);

static int tmu_port_reset_set(void *priv_data, u64 val)
{
	struct reg *r = priv_data;

	if (!g_tmu)
		return -ENODEV;

	if (val)
		_tmu_reset(g_tmu, r->port);

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(port_reset_fops, NULL, tmu_port_reset_set, "%llu\n");

static int reg_masked_get(void *priv_data, u64 *val)
{
	struct reg *r = priv_data;
	TMU_t *tmu = g_tmu;
	int err;
	u32 _val;

	*val = 0;

	if (!g_tmu)
		return -ENODEV;

	switch (r->comp) {
	case PORT:
		err = tmu_port_rm32(tmu, r->port, r->reg, r->mask, r->shift, &_val);
		break;

	case QUEUE:
		err = tmu_queue_rm32(tmu, r->port, r->u.queue, r->reg, r->mask, r->shift, &_val);
		break;

	case SCHED:
		if (r->u.sched != 0 && r->reg == TMU_SCH0_POS)
			return -EINVAL;

		err = tmu_sched_rm32(tmu, r->port, r->u.sched, r->reg, r->mask, r->shift, &_val);

		break;

	case SHAPER:
		err = tmu_shaper_rm32(tmu, r->port, r->u.shaper, r->reg, r->mask, r->shift, &_val);
		break;

	default:
		return -EINVAL;
	}

	*val = _val;

	return err;
}

static int reg_masked_set(void *priv_data, u64 val)
{
	struct reg *r = priv_data;
	TMU_t *tmu = g_tmu;
	int err;

	if (!g_tmu)
		return -ENODEV;

	switch (r->comp) {
	case PORT:
		err = tmu_port_rmw32(tmu, r->port, r->reg, r->mask, r->shift, val);
		break;

	case QUEUE:
		err = tmu_queue_rmw32(tmu, r->port, r->u.queue, r->reg, r->mask, r->shift, val);
		break;

	case SCHED:
		if (r->u.sched != 0 && r->reg == TMU_SCH0_POS)
			return -EINVAL;

		err = tmu_sched_rmw32(tmu, r->port, r->u.sched, r->reg, r->mask, r->shift, val);

		break;

	case SHAPER:
		err = tmu_shaper_rmw32(tmu, r->port, r->u.shaper, r->reg, r->mask, r->shift, val);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

#define REG_RW_DEC_ATTR(name) \
	DEFINE_DEBUGFS_ATTRIBUTE(name##_fops, reg_masked_get, reg_masked_set, "%llu\n")

#define REG_RO_DEC_ATTR(name) \
	DEFINE_DEBUGFS_ATTRIBUTE(name##_fops, reg_masked_get, NULL, "%llu\n")

#define Q_REG_RW(_name, _reg, _mask, _shift) 		\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR | S_IWUSR,		\
		.reg = { 				\
			.comp = QUEUE,			\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

#define Q_REG_RO(_name, _reg, _mask, _shift) 		\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR,			\
		.reg = { 				\
			.comp = QUEUE,			\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

#define SCH_REG_RW(_name, _reg, _mask, _shift) 		\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR | S_IWUSR,		\
		.reg = { 				\
			.comp = SCHED, 			\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

#define SCH_REG_RO(_name, _reg, _mask, _shift) 		\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR,			\
		.reg = { 				\
			.comp = SCHED, 			\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

#define SHP_REG_RW(_name, _reg, _mask, _shift) 		\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR | S_IWUSR,		\
		.reg = { 				\
			.comp = SHAPER, 		\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

#define SHP_REG_RO(_name, _reg, _mask, _shift) 		\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR,			\
		.reg = { 				\
			.comp = SHAPER, 		\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

#define PORT_REG_RW(_name, _reg, _mask, _shift) 	\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR | S_IWUSR,		\
		.reg = { 				\
			.comp = PORT, 			\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

#define PORT_REG_RO(_name, _reg, _mask, _shift) 	\
	{ 						\
		.name = __stringify(_name),		\
		.fops = &_name##_fops, 			\
		.mode = S_IRUSR,			\
		.reg = { 				\
			.comp = PORT, 			\
			.reg = _reg, 			\
			.mask = _mask, 			\
			.shift = _shift, 		\
		}, 					\
	}

static int wred_drop_probs_show(struct seq_file *s, void *data)
{
	struct reg *r = s->private;
	TMU_t *tmu = g_tmu;
	u8 drop_probs[TMU_WRED_PROB_CNT] = { 0 };
	int i;

	if (!g_tmu)
		return -ENODEV;

	tmu_wred_drop_probs_get(tmu, r->port, r->u.queue, drop_probs);

	for (i = 0; i < ARRAY_SIZE(drop_probs); i++) {
		seq_printf(s, "%u ", drop_probs[i]);
	}
	seq_printf(s, "\n");

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(wred_drop_probs);

static ssize_t wred_drop_probs_write(struct file *file,
				     const char __user *ubuf,
				     size_t count, loff_t *ppos)
{
	struct seq_file *s = file->private_data;
	struct reg *r = s->private;
	TMU_t *tmu = g_tmu;
	char buf[64] = { 0 };
	ssize_t sz = min_t(size_t, sizeof(buf) - 1, count);
	u8 probs[TMU_WRED_PROB_CNT] = { 0 };

	if (!g_tmu)
		return -ENODEV;

	if (copy_from_user(buf, ubuf, sz))
		return -EINVAL;

	if (8 != sscanf(buf, "%hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
	                &probs[0], &probs[1], &probs[2], &probs[3],
	                &probs[4], &probs[5], &probs[6], &probs[7])) {
		return -EINVAL;
	}

	tmu_wred_drop_probs_set(tmu, r->port, r->u.queue, probs);

	return count;
}

static const struct file_operations wred_drop_probs_rw_fops = {
	.open		= wred_drop_probs_open,
	.write		= wred_drop_probs_write,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int sched_q_weight_show(struct seq_file *s, void *data)
{
	struct reg *r = s->private;
	TMU_t *tmu = g_tmu;
	int i;

	if (!g_tmu)
		return -ENODEV;

	for (i = 0; i < TMU_SCH_Q_WEIGHT_CNT; i++) {
		u32 weight;
		tmu_sched_weight_get(tmu, r->port, r->u.sched, i, &weight);
		seq_printf(s, "%u ", weight);
	}
	seq_printf(s, "\n");

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(sched_q_weight);

static ssize_t sched_q_weight_write(struct file *file,
				    const char __user *ubuf,
				    size_t count, loff_t *ppos)
{
	struct seq_file *s = file->private_data;
	struct reg *r = s->private;
	TMU_t *tmu = g_tmu;
	char buf[64] = { 0 };
	ssize_t sz = min_t(size_t, sizeof(buf) - 1, count);
	u32 w[TMU_SCH_Q_WEIGHT_CNT] = { 0 };
	int i;

	if (!g_tmu)
		return -ENODEV;

	if (copy_from_user(buf, ubuf, sz))
		return -EINVAL;

	if (8 != sscanf(buf, "%u %u %u %u %u %u %u %u",
	                &w[0], &w[1], &w[2], &w[3],
	                &w[4], &w[5], &w[6], &w[7])) {
		return -EINVAL;
	}

	for (i = 0; i < TMU_SCH_Q_WEIGHT_CNT; i++) {
		tmu_sched_weight_set(tmu, r->port, r->u.sched, i, w[i]);
	}

	return count;
}

static const struct file_operations sched_q_weight_rw_fops = {
	.open		= sched_q_weight_open,
	.write		= sched_q_weight_write,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int sched_q_map_show(struct seq_file *s, void *data)
{
	struct reg *r = s->private;
	TMU_t *tmu = g_tmu;
	u32 map[TMU_SCH_Q_ALLOC_CNT];
	int i;

	if (!g_tmu)
		return -ENODEV;

	tmu_sched_queue_map_get(tmu, r->port, r->u.sched, map);

	for (i = 0; i < ARRAY_SIZE(map); i++) {
		seq_printf(s, "%u ", map[i]);
	}
	seq_printf(s, "\n");

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(sched_q_map);

static ssize_t sched_q_map_write(struct file *file,
				 const char __user *ubuf,
				 size_t count, loff_t *ppos)
{
	struct seq_file *s = file->private_data;
	struct reg *r = s->private;
	TMU_t *tmu = g_tmu;
	char buf[64] = { 0 };
	ssize_t sz = min_t(size_t, sizeof(buf) - 1, count);
	u32 m[TMU_SCH_Q_ALLOC_CNT] = { 0 };

	if (!g_tmu)
		return -ENODEV;

	if (copy_from_user(buf, ubuf, sz))
		return -EINVAL;

	if (8 != sscanf(buf, "%u %u %u %u %u %u %u %u",
	                &m[0], &m[1], &m[2], &m[3],
	                &m[4], &m[5], &m[6], &m[7])) {
		return -EINVAL;
	}

	tmu_sched_queue_map_set(tmu, r->port, r->u.sched, m);

	return count;
}

static const struct file_operations sched_q_map_rw_fops = {
	.open		= sched_q_map_open,
	.write		= sched_q_map_write,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

REG_RW_DEC_ATTR(q_type);
REG_RO_DEC_ATTR(q_head);
REG_RO_DEC_ATTR(q_tail);
REG_RW_DEC_ATTR(qlen_min);
REG_RW_DEC_ATTR(qlen_max);
REG_RW_DEC_ATTR(buf_max);
REG_RO_DEC_ATTR(buf_cnt);
REG_RO_DEC_ATTR(pkt_cnt);

struct dbg_desc queue_descs[] = {
	Q_REG_RW(q_type,   TMU_PORT_QUEUE_CFG0, TMU_DROP_TYPE,      TMU_DROP_TYPE_SHIFT),
	Q_REG_RO(q_head,   TMU_PORT_QUEUE_STS0, TMU_QUEUE_HEAD_PTR, TMU_QUEUE_HEAD_PTR_SHIFT),
	Q_REG_RO(q_tail,   TMU_PORT_QUEUE_STS0, TMU_QUEUE_TAIL_PTR, TMU_QUEUE_TAIL_PTR_SHIFT),
	Q_REG_RW(qlen_min, TMU_PORT_QUEUE_CFG0, TMU_QUEUE_MIN,      TMU_QUEUE_MIN_SHIFT),
	Q_REG_RW(qlen_max, TMU_PORT_QUEUE_CFG0, TMU_QUEUE_MAX,      TMU_QUEUE_MAX_SHIFT),
	Q_REG_RO(pkt_cnt,  TMU_PORT_QUEUE_STS1, TMU_QUEUE_PKT_CNT,  TMU_QUEUE_PKT_CNT_SHIFT),
	Q_REG_RO(buf_cnt,  TMU_PORT_QUEUE_STS2, TMU_QUEUE_BUF_CNT,  TMU_QUEUE_BUF_CNT_SHIFT),
	Q_REG_RW(buf_max,  TMU_PORT_QUEUE_CFG3, TMU_QUEUE_BUF_MAX,  TMU_QUEUE_BUF_MAX_SHIFT),
	{ .name = "wred_drop_probs", .mode = 0600, .fops = &wred_drop_probs_rw_fops },
};

REG_RW_DEC_ATTR(deq_algo);
REG_RW_DEC_ATTR(sch_bitrate);
REG_RW_DEC_ATTR(sch_pos);

struct dbg_desc sched_descs[] = {
	SCH_REG_RW(deq_algo,    TMU_SCH_CTRL,     TMU_SCH_ALGO,          TMU_SCH_ALGO_SHIFT),
	SCH_REG_RW(sch_bitrate, TMU_SCH_BIT_RATE, TMU_SCH_BIT_RATE_MASK, TMU_SCH_BIT_RATE_SHIFT),
	SCH_REG_RW(sch_pos,     TMU_SCH0_POS,     TMU_SCH0_POS_MASK,     TMU_SCH0_POS_SHIFT),
	{ .name = "q_weight", .mode = 0600, .fops = &sched_q_weight_rw_fops },
	{ .name = "q_map",    .mode = 0600, .fops = &sched_q_map_rw_fops },
};

REG_RW_DEC_ATTR(shp_enabled);
REG_RW_DEC_ATTR(credit_rate);
REG_RW_DEC_ATTR(credit_max);
REG_RW_DEC_ATTR(credit_min);
REG_RW_DEC_ATTR(credit_clear);
REG_RO_DEC_ATTR(credit_cntr);
REG_RW_DEC_ATTR(weight_int);
REG_RW_DEC_ATTR(weight_frac);
REG_RW_DEC_ATTR(shp_bitrate);
REG_RW_DEC_ATTR(shp_pos);
REG_RO_DEC_ATTR(shp_working);

struct dbg_desc shaper_descs[] = {
	SHP_REG_RW(credit_rate,   TMU_SHP_CTRL,       TMU_SHP_CLK_DIV,          TMU_SHP_CLK_DIV_SHIFT),
	SHP_REG_RW(credit_max,    TMU_SHP_MAX_CREDIT, TMU_SHP_MAX_CREDIT_MASK,  TMU_SHP_MAX_CREDIT_SHIFT),
	SHP_REG_RW(credit_min,    TMU_SHP_MIN_CREDIT, TMU_SHP_MIN_CREDIT_MASK,  TMU_SHP_MIN_CREDIT_SHIFT),
	SHP_REG_RW(credit_clear,  TMU_SHP_CTRL2,      TMU_SHP_MODE,             TMU_SHP_MODE_SHIFT),
	SHP_REG_RO(credit_cntr,   TMU_SHP_STATUS,     TMU_SHP_CREDIT_CNTR,      TMU_SHP_CREDIT_CNTR_SHIFT),
        SHP_REG_RW(weight_int,    TMU_SHP_WEIGHT,     TMU_SHP_WEIGHT_INT_MASK,  TMU_SHP_WEIGHT_INT_SHIFT),
        SHP_REG_RW(weight_frac,   TMU_SHP_WEIGHT,     TMU_SHP_WEIGHT_FRAC_MASK, TMU_SHP_WEIGHT_FRAC_SHIFT),
	SHP_REG_RW(shp_enabled,   TMU_SHP_CTRL,       TMU_SHP_EN,               TMU_SHP_EN_SHIFT),
	SHP_REG_RW(shp_pos,       TMU_SHP_CTRL2,      TMU_SHP_POS,              TMU_SHP_POS_SHIFT),
	SHP_REG_RW(shp_bitrate,   TMU_SHP_CTRL2,      TMU_SHP_BIT_RATE,         TMU_SHP_BIT_RATE_SHIFT),
	SHP_REG_RO(shp_working,   TMU_SHP_STATUS,     TMU_SHP_CURR_STATUS,      TMU_SHP_CURR_STATUS_SHIFT),
};

REG_RW_DEC_ATTR(ifg_cfg);
REG_RW_DEC_ATTR(shaper_clk_en);
REG_RW_DEC_ATTR(tdq_en);
REG_RW_DEC_ATTR(sched0_en);
REG_RW_DEC_ATTR(sched1_en);
REG_RO_DEC_ATTR(allow_cfg);
REG_RW_DEC_ATTR(pkt_left_ignore);

struct dbg_desc tdq_descs[] = {
	PORT_REG_RW(ifg_cfg,         TMU_TDQ_IFG,  TMU_TDQ_IIF_CFG,     TMU_TDQ_IIF_CFG_SHIFT),
	PORT_REG_RW(shaper_clk_en,   TMU_TDQ_CTRL, TMU_SHP_CLK_CNT_EN,  TMU_SHP_CLK_CNT_EN_SHIFT),
	PORT_REG_RW(tdq_en,          TMU_TDQ_CTRL, TMU_TDQ_HW_EN,       TMU_TDQ_HW_EN_SHIFT),
	PORT_REG_RW(sched0_en,       TMU_TDQ_CTRL, TMU_SCH0_EN,         TMU_SCH0_EN_SHIFT),
	PORT_REG_RW(sched1_en,       TMU_TDQ_CTRL, TMU_SCH1_EN,         TMU_SCH1_EN_SHIFT),
	PORT_REG_RO(allow_cfg,       TMU_TDQ_CTRL, TMU_TDQ_ALLOW_CFG,   TMU_TDQ_ALLOW_CFG_SHIFT),
	PORT_REG_RW(pkt_left_ignore, TMU_TDQ_CTRL, TMU_PKT_LEFT_IGNORE, TMU_PKT_LEFT_IGNORE_SHIFT),
};

struct dbg_desc port_descs[] = {
	{ .name = "reset", .mode = 0600, .fops = &port_reset_fops },
};

static struct reg *debug_desc_create(struct dentry *p, struct reg *r,
                                     struct dbg_desc *d, size_t cnt,
                                     u32 port, u32 comp)
{
	int k;

	for (k = 0; k < cnt; k++) {
		memcpy(r, &d->reg, sizeof(struct reg));
		r->port = port;
		r->u.nr_comp = comp;

		debugfs_create_file(d->name, d->mode, p, r, d->fops);

		d++;
		r++;
	}

	return r;
}

int tmu_debugfs_init(TMU_t *tmu)
{
	struct dentry *root, *port, *comp;
	struct reg *r;
	char name[20];
	u32 i, j, k;

	struct {
		const char 	*name;
		size_t 		cnt;
		struct dbg_desc *descs;
		size_t 		desc_cnt;
	} comps[] = {
		{ "queue%d",     QUE_MAX_NUM_PER_PORT,     queue_descs,  ARRAY_SIZE(queue_descs)  },
		{ "scheduler%d", QUE_SCH_NUM_PER_PORT, sched_descs,  ARRAY_SIZE(sched_descs)  },
		{ "shaper%d",    QUE_SHAPER_NUM_PER_PORT,    shaper_descs, ARRAY_SIZE(shaper_descs) },
		{ "tdq",	 1,                       tdq_descs,    ARRAY_SIZE(tdq_descs)    },
	};

	u32 decs_per_port = ARRAY_SIZE(queue_descs)  * QUE_MAX_NUM_PER_PORT     +
			    ARRAY_SIZE(sched_descs)  * QUE_SCH_NUM_PER_PORT +
			    ARRAY_SIZE(shaper_descs) * QUE_SHAPER_NUM_PER_PORT    +
			    ARRAY_SIZE(tdq_descs) + ARRAY_SIZE(port_descs);

	desc_regs = devm_kzalloc(&tmu->cpriv->pdev->dev, sizeof(struct reg) * decs_per_port * TMU_MAX_PORT_CNT, GFP_KERNEL);
	if (!desc_regs)
		return -ENOMEM;

	root = debugfs_create_dir("npu_tmu", NULL);
	if (!root) {
		return -ENOENT;
	}

	r = desc_regs;

	for (i = 0; i < TMU_MAX_PORT_CNT; i++) {
		snprintf(name, sizeof(name), "port%d", i);
		port = debugfs_create_dir(name, root);

		for (j = 0; j < ARRAY_SIZE(comps); j++) {
			for (k = 0; k < comps[j].cnt; k++) {
				if (!comps[j].name)
					break;

				snprintf(name, sizeof(name), comps[j].name, k);
				comp = debugfs_create_dir(name, port);

				if (!comps[j].descs)
					break;

				// @r is increased inside
				r = debug_desc_create(comp, r, comps[j].descs, comps[j].desc_cnt, i, k);
			}
		}

		r = debug_desc_create(port, r, port_descs, ARRAY_SIZE(port_descs), i, 0);
	}

	debugfs_create_file("reg_dump",  0400, root, NULL, &reg_dump_fops);

	tmu_dir = root;

	return 0;
}

int tmu_debugfs_deinit(TMU_t *tmu)
{
	if (tmu_dir)
		debugfs_remove_recursive(tmu_dir);

	return 0;
}
