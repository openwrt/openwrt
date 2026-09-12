// SPDX-License-Identifier: GPL-2.0
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/bitfield.h>
#include <linux/bits.h>

#include "cs752x_ne.h"

#define SCH_CONTROL				0x000
#define SCH_CONTROL_SOFT_RESET			BIT(0)

#define SCH_STATUS				0x004
#define SCH_STATUS_SCH_MEM_INIT_DONE		BIT(0)
#define SCH_STATUS_CURRENT_SET_TDM_CONTROL	BIT(1)
#define SCH_STATUS_CPU_CMD_STATUS		GENMASK(4, 2)

#define SCH_SWITCH_TDM_CONTROL_SET		0x008
#define SCH_SWITCH_TDM_CONTROL_SET_SWITCH	BIT(0)

#define SCH_MAIN_TDM_CONTROL_SETA		0x00c
#define SCH_MAIN_TDM_CONTROL_SETB		0x010
#define SCH_MAIN_TDM_CONTROL_CALENDAR_RETURN	GENMASK(4, 0)

#define SCH_MAIN_TDM_PORT_CALENDAR0_SETA	0x014
#define SCH_MAIN_TDM_PORT_CALENDAR1_SETA	0x018
#define SCH_MAIN_TDM_PORT_CALENDAR0_SETB	0x01c
#define SCH_MAIN_TDM_PORT_CALENDAR1_SETB	0x020

#define SCH_SHAPER_CONFIGURATION		0x024
#define SCH_SHAPER_CONFIGURATION_CLOCKS_PER_ROUND_ST	GENMASK(9, 0)
#define SCH_SHAPER_CONFIGURATION_GLOBAL_SCRUB_ENABLE	BIT(27)

#define SCH_ATOMIC_COMMAND_CONTROL		0x028
#define SCH_ATOMIC_COMMAND_CONTROL_START_WRITE	BIT(0)

#define SCH_ATOMIC_COMMAND_DATA1		0x02c
#define SCH_ATOMIC_COMMAND_DATA1_CPU_PORT	GENMASK(2, 0)
#define SCH_ATOMIC_COMMAND_DATA1_PORT		GENMASK(3, 0)
#define SCH_ATOMIC_COMMAND_DATA1_TMAX		GENMASK(21, 4)
#define SCH_ATOMIC_COMMAND_DATA1_RPT		GENMASK(26, 22)

#define SCH_ATOMIC_COMMAND_DATA0		0x030
#define SCH_ATOMIC_COMMAND_DATA0_CMD		GENMASK(3, 0)
#define SCH_ATOMIC_COMMAND_DATA0_MASK		BIT(4)
#define SCH_ATOMIC_COMMAND_DATA0_VOQ		GENMASK(10, 4)
#define SCH_ATOMIC_COMMAND_DATA0_SHAPER		GENMASK(11, 4)
#define SCH_ATOMIC_COMMAND_DATA0_BURST		GENMASK(11, 4)
#define SCH_ATOMIC_COMMAND_DATA0_QUANTA		GENMASK(29, 12)
#define SCH_ATOMIC_COMMAND_DATA0_TSIZE		GENMASK(29, 12)

#define SCH_INTERRUPT_0				0x034
#define SCH_INTERRUPT_0_CPU_CMD_EXECUTED	BIT(0)

#define SCH_DEBUG_CONTROL			0x084
#define SCH_DEBUG_CONTROL_SWITCH_EXPR_MODE_LOWLVL	GENMASK(24, 20)

#define SCH_CMD_VOQ_AS_DRR			1
#define SCH_CMD_CHANGE_SHAPER_PARA		5
#define SCH_CMD_SET_BURST_SIZE			7
#define SCH_CMD_CHANGE_PORT_LEVEL_MASK		9

#define SCH_ENABLE_PORT				0

#define SCH_PORT_SCH_ID				0
#define SCH_ROOT_SCH_ID				1
#define SCH_CPU_SCH_ID				2

#define SCH_MAX_ETH_PORT			5
#define SCH_MAX_ROOT_PORT			1
#define SCH_MAX_CPU_PORT			8
#define SCH_MAX_PORT_QUEUE			8
#define SCH_PORT_NO				(SCH_MAX_ETH_PORT + \
						 SCH_MAX_ROOT_PORT + \
						 SCH_MAX_CPU_PORT)
#define SCH_VOQ_NO				(SCH_PORT_NO * \
						 SCH_MAX_PORT_QUEUE)
#define SCH_DRR_BASE_ID				SCH_VOQ_NO
#define SCH_LT_BASE_ID				(SCH_VOQ_NO + SCH_PORT_NO)
#define SCH_ST_BASE_ID				(SCH_LT_BASE_ID + SCH_PORT_NO)
#define SCH_CPU_PORT0				(SCH_MAX_ETH_PORT + \
						 SCH_MAX_ROOT_PORT)
#define SCH_MAX_CALENDAR_LENGTH			32
#define SCH_DEFAULT_QUANTA			9132
#define SCH_CLOCKS_PER_ROUND_ST			0x100
#define SCH_EXPR_MODE_LOWLVL			3

#define SCH_CMD_TIMEOUT_US			10000
#define SCH_INIT_TIMEOUT_US			10000
#define SCH_RESET_DELAY_US			100

#define TM_BM_CONFIG_0				0x000
#define TM_BM_CONFIG_0_INIT			BIT(0)
#define TM_BM_CONFIG_0_ENQUE_HDR_ADJUST		BIT(1)
#define TM_BM_CONFIG_0_BM1_BYPASS_VOQ_PROCESS	BIT(4)
#define TM_BM_CONFIG_0_BM1_BYPASS_DEST_PORT_PROCESS	BIT(5)
#define TM_BM_CONFIG_0_BM1_BYPASS_CA_PROCESS	BIT(6)
#define TM_BM_CONFIG_0_BM2_BYPASS_VOQ_PROCESS	BIT(8)
#define TM_BM_CONFIG_0_BM2_BYPASS_DEST_PORT_PROCESS	BIT(9)
#define TM_BM_CONFIG_0_BM2_BYPASS_CA_PROCESS	BIT(10)
#define TM_BM_CONFIG_0_RECALCULATE_GLOBAL_BUFFERS_USED	BIT(29)
#define TM_BM_CONFIG_0_DISABLE			BIT(30)
#define TM_BM_CONFIG_0_CPU_FULL_ACCESS		BIT(31)

#define TM_BM_STATUS_0				0x004
#define TM_BM_STATUS_0_INIT_DONE		BIT(0)

#define TM_BM_WRED_CONFIG_1			0x02c
#define TM_BM_WRED_CONFIG_1_WRED_PROFILE_SELECT_MODE	GENMASK(15, 14)

#define TM_BM_TRAFFIC_CONTROL_2			0x074
#define TM_BM_TRAFFIC_CONTROL_2_WARNING_THRESHOLD_INT	GENMASK(11, 0)

#define TM_BM_TRAFFIC_CONTROL_3			0x078
#define TM_BM_TRAFFIC_CONTROL_3_LIVING_THRESHOLD_INT	GENMASK(11, 0)
#define TM_BM_TRAFFIC_CONTROL_3_DYING_GASP_THRESHOLD_INT	GENMASK(27, 16)

#define TM_BM_VOQ_MEM_ACCESS			0x080
#define TM_BM_VOQ_MEM_ACCESS_ADDR		GENMASK(6, 0)

#define TM_BM_VOQ_MEM_DATA			0x084
#define TM_BM_VOQ_MEM_DATA_VOQ_PROFILE		GENMASK(4, 0)
#define TM_BM_VOQ_MEM_DATA_WRED_PROFILE		GENMASK(9, 5)
#define TM_BM_VOQ_MEM_DATA_DEST_PORT		GENMASK(12, 10)
#define TM_BM_VOQ_MEM_DATA_VOQ_CNTR_ENABLE	BIT(13)
#define TM_BM_VOQ_MEM_DATA_VOQ_CNTR		GENMASK(18, 14)
#define TM_BM_VOQ_MEM_DATA_WRED_ENABLE		BIT(19)

#define TM_BM_VOQ_PROFILE_MEM_ACCESS		0x0c0
#define TM_BM_VOQ_PROFILE_MEM_ACCESS_ADDR	GENMASK(4, 0)

#define TM_BM_VOQ_PROFILE_MEM_DATA1		0x0c4
#define TM_BM_VOQ_PROFILE_MEM_DATA0		0x0c8
#define TM_BM_VOQ_PROFILE_MEM_DATA0_VOQ_MIN_DEPTH	GENMASK(15, 0)
#define TM_BM_VOQ_PROFILE_MEM_DATA0_VOQ_MAX_DEPTH	GENMASK(31, 16)

#define TM_POL_CONFIG_0				0x100
#define TM_POL_CONFIG_0_INIT			BIT(0)
#define TM_POL_CONFIG_0_POL_COLOR_BLIND		BIT(4)
#define TM_POL_CONFIG_0_POL_BYPASS_YELLOW	BIT(5)
#define TM_POL_CONFIG_0_POL_BYPASS_RED		BIT(6)
#define TM_POL_CONFIG_0_POL_DISABLE		BIT(30)
#define TM_POL_CONFIG_0_CPU_FULL_ACCESS		BIT(31)

#define TM_POL_CONFIG_SPID			0x104
#define TM_POL_CONFIG_FLOW			0x108
#define TM_POL_CONFIG_PKT_TYPE			0x10c
#define TM_POL_CONFIG_CPU			0x110
#define TM_POL_CONFIG_BYPASS_YELLOW		BIT(0)
#define TM_POL_CONFIG_BYPASS_RED		BIT(1)
#define TM_POL_CONFIG_DISABLE			BIT(2)
#define TM_POL_CONFIG_COLOR_BLIND		BIT(3)
#define TM_POL_CONFIG_BYPASS_YELLOW_DEFINE	BIT(4)
#define TM_POL_CONFIG_UPDATE_MODE		BIT(5)
#define TM_POL_CONFIG_NEST_LEVEL		GENMASK(7, 6)
#define TM_POL_CONFIG_COMMIT_OVERRIDE_ENABLE	BIT(9)
#define TM_POL_CONFIG_COMMON_MASK		(GENMASK(7, 0) | BIT(9))
#define TM_POL_CONFIG_PKT_TYPE_LSPID0		GENMASK(19, 16)
#define TM_POL_CONFIG_PKT_TYPE_LSPID1		GENMASK(23, 20)
#define TM_POL_CONFIG_PKT_TYPE_LSPID2		GENMASK(27, 24)
#define TM_POL_CONFIG_PKT_TYPE_LSPID3		GENMASK(31, 28)
#define TM_POL_CONFIG_CPU_VOQ0			GENMASK(22, 16)
#define TM_POL_CONFIG_CPU_VOQ1			GENMASK(30, 24)

#define TM_POL_STATUS_0				0x124
#define TM_POL_STATUS_0_INIT_DONE		BIT(0)

#define TM_POL_FLOW_PROFILE_MEM_ACCESS		0x190
#define TM_POL_FLOW_PROFILE_MEM_ACCESS_ADDR	GENMASK(6, 0)
#define TM_POL_FLOW_PROFILE_MEM_DATA1		0x194
#define TM_POL_FLOW_PROFILE_MEM_DATA0		0x198

#define TM_PM_CONFIG_0				0x400
#define TM_PM_CONFIG_0_INIT			BIT(0)
#define TM_PM_CONFIG_0_AUTO_CLEAR_ON_READ_MODE	GENMASK(5, 4)
#define TM_PM_CONFIG_0_DISABLE			BIT(30)

#define TM_PM_STATUS_0				0x408
#define TM_PM_STATUS_0_INIT_DONE		BIT(0)

#define TM_MEM_ACCESS_RBW			BIT(30)
#define TM_MEM_ACCESS_ACCESS			BIT(31)

#define TM_BM_VOQ_NO				112
#define TM_BM_CPU_VOQ_PROFILE_FIRST		48
#define TM_BM_CPU_VOQ_PROFILE_END		72
#define TM_BM_DEF_VOQ_MIN_DEPTH			48
#define TM_BM_DEF_VOQ_MAX_DEPTH_INT_BUFF	2048
#define TM_BM_DEF_VOQ_MAX_DEPTH_EXT_BUFF	65530
#define TM_BM_DEF_CPU_VOQ_MIN_DEPTH		128
#define TM_BM_DEF_CPU_VOQ_MAX_DEPTH		32768
#define TM_BM_DEF_DYING_GASP_INT		(6 * 24)
#define TM_BM_DEF_LIVING_THSHLD_INT		(TM_BM_DEF_DYING_GASP_INT + \
						 6 * 3)
#define TM_BM_WRED_PROFILE_SELECT_DIRECT	2

#define TM_POL_FLOW_POL_NO			128
#define TM_POL_NEST_LEVEL_SPID			0
#define TM_POL_NEST_LEVEL_FLOW			1
#define TM_POL_NEST_LEVEL_PKT_TYPE		2

#define TM_PM_READ_MODE_CLEAR_ALL		1

#define TM_INIT_TIMEOUT_US			200000
#define TM_ACCESS_TIMEOUT_US			1000

struct cs752x_tm_mem_word {
	u32 off;
	u32 val;
};

static int cs752x_sch_cmd(struct device *dev, void __iomem *sch, u32 data0,
			  u32 data1)
{
	u32 val;
	int ret;

	writel(data0, sch + SCH_ATOMIC_COMMAND_DATA0);
	writel(data1, sch + SCH_ATOMIC_COMMAND_DATA1);
	writel(SCH_ATOMIC_COMMAND_CONTROL_START_WRITE,
	       sch + SCH_ATOMIC_COMMAND_CONTROL);

	ret = readl_poll_timeout(sch + SCH_INTERRUPT_0, val,
				 val & SCH_INTERRUPT_0_CPU_CMD_EXECUTED, 1,
				 SCH_CMD_TIMEOUT_US);
	if (ret) {
		dev_err(dev, "sch command %#010x %#010x not executed\n",
			data0, data1);
		return ret;
	}

	writel(SCH_INTERRUPT_0_CPU_CMD_EXECUTED, sch + SCH_INTERRUPT_0);

	val = FIELD_GET(SCH_STATUS_CPU_CMD_STATUS, readl(sch + SCH_STATUS));
	if (val) {
		dev_err(dev, "sch command %#010x %#010x failed, status %u\n",
			data0, data1, val);
		return -EIO;
	}

	return 0;
}

static int cs752x_sch_enable_port(struct device *dev, void __iomem *sch,
				  unsigned int port)
{
	u32 data0, data1;

	data0 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_CMD,
			   SCH_CMD_CHANGE_PORT_LEVEL_MASK) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_MASK, SCH_ENABLE_PORT);
	data1 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA1_PORT, port);

	return cs752x_sch_cmd(dev, sch, data0, data1);
}

static int cs752x_sch_set_port_burst(struct device *dev, void __iomem *sch,
				     unsigned int port, unsigned int burst)
{
	u32 data0, data1;

	data0 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_CMD,
			   SCH_CMD_SET_BURST_SIZE) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_BURST, burst);
	data1 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA1_PORT, port);

	return cs752x_sch_cmd(dev, sch, data0, data1);
}

static int cs752x_sch_bypass_shaper(struct device *dev, void __iomem *sch,
				    unsigned int shaper, unsigned int cpu_port)
{
	u32 data0, data1;

	data0 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_CMD,
			   SCH_CMD_CHANGE_SHAPER_PARA) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_SHAPER, shaper) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_TSIZE, 0);
	data1 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA1_CPU_PORT, cpu_port) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA1_TMAX, 1) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA1_RPT, 0);

	return cs752x_sch_cmd(dev, sch, data0, data1);
}

static int cs752x_sch_enable_voq_drr(struct device *dev, void __iomem *sch,
				     unsigned int voq, unsigned int quanta,
				     unsigned int cpu_port)
{
	u32 data0, data1;

	data0 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_CMD, SCH_CMD_VOQ_AS_DRR) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_VOQ, voq) |
		FIELD_PREP(SCH_ATOMIC_COMMAND_DATA0_QUANTA, quanta);
	data1 = FIELD_PREP(SCH_ATOMIC_COMMAND_DATA1_CPU_PORT, cpu_port);

	return cs752x_sch_cmd(dev, sch, data0, data1);
}

static int cs752x_sch_reset(struct device *dev, void __iomem *sch)
{
	u32 val;
	int ret;

	writel(SCH_CONTROL_SOFT_RESET, sch + SCH_CONTROL);
	udelay(SCH_RESET_DELAY_US);
	writel(0, sch + SCH_CONTROL);
	udelay(SCH_RESET_DELAY_US);

	ret = readl_poll_timeout(sch + SCH_STATUS, val,
				 val & SCH_STATUS_SCH_MEM_INIT_DONE, 10,
				 SCH_INIT_TIMEOUT_US);
	if (ret)
		dev_err(dev, "sch memory init failed, status %#010x\n", val);

	return ret;
}

static void cs752x_sch_set_calendar(void __iomem *sch)
{
	static const u8 calendar[SCH_MAX_CALENDAR_LENGTH] = {
		SCH_PORT_SCH_ID, SCH_PORT_SCH_ID, SCH_PORT_SCH_ID,
		SCH_CPU_SCH_ID, SCH_CPU_SCH_ID,
		SCH_PORT_SCH_ID, SCH_PORT_SCH_ID, SCH_PORT_SCH_ID,
		SCH_CPU_SCH_ID, SCH_CPU_SCH_ID,
		SCH_PORT_SCH_ID, SCH_PORT_SCH_ID, SCH_PORT_SCH_ID,
		SCH_ROOT_SCH_ID,
		SCH_PORT_SCH_ID, SCH_PORT_SCH_ID, SCH_PORT_SCH_ID,
		SCH_CPU_SCH_ID, SCH_CPU_SCH_ID,
		SCH_PORT_SCH_ID, SCH_PORT_SCH_ID, SCH_PORT_SCH_ID,
		SCH_CPU_SCH_ID, SCH_CPU_SCH_ID,
		SCH_PORT_SCH_ID, SCH_PORT_SCH_ID, SCH_PORT_SCH_ID,
		SCH_ROOT_SCH_ID, SCH_ROOT_SCH_ID,
		SCH_PORT_SCH_ID, SCH_PORT_SCH_ID, SCH_PORT_SCH_ID,
	};
	u32 cal0 = 0, cal1 = 0;
	unsigned int i;

	for (i = 0; i < SCH_MAX_CALENDAR_LENGTH / 2; i++)
		cal0 |= (u32)(calendar[i] & 0x3) << (2 * i);
	for (; i < SCH_MAX_CALENDAR_LENGTH; i++)
		cal1 |= (u32)(calendar[i] & 0x3) <<
			(2 * (i - SCH_MAX_CALENDAR_LENGTH / 2));

	if (readl(sch + SCH_STATUS) & SCH_STATUS_CURRENT_SET_TDM_CONTROL) {
		writel(cal0, sch + SCH_MAIN_TDM_PORT_CALENDAR0_SETA);
		writel(cal1, sch + SCH_MAIN_TDM_PORT_CALENDAR1_SETA);
		writel(FIELD_PREP(SCH_MAIN_TDM_CONTROL_CALENDAR_RETURN,
				  SCH_MAX_CALENDAR_LENGTH - 1),
		       sch + SCH_MAIN_TDM_CONTROL_SETA);
	} else {
		writel(cal0, sch + SCH_MAIN_TDM_PORT_CALENDAR0_SETB);
		writel(cal1, sch + SCH_MAIN_TDM_PORT_CALENDAR1_SETB);
		writel(FIELD_PREP(SCH_MAIN_TDM_CONTROL_CALENDAR_RETURN,
				  SCH_MAX_CALENDAR_LENGTH - 1),
		       sch + SCH_MAIN_TDM_CONTROL_SETB);
	}

	writel(SCH_SWITCH_TDM_CONTROL_SET_SWITCH,
	       sch + SCH_SWITCH_TDM_CONTROL_SET);
}

int cs752x_sch_init(struct device *dev, void __iomem *sch)
{
	unsigned int port, q, cpu_port;
	u32 val;
	int ret;

	ret = cs752x_sch_reset(dev, sch);
	if (ret)
		return ret;

	cs752x_sch_set_calendar(sch);

	for (port = 0; port < SCH_PORT_NO; port++) {
		cpu_port = port >= SCH_CPU_PORT0 ? port - SCH_CPU_PORT0 : 0;

		cs752x_sch_enable_port(dev, sch, port);
		cs752x_sch_set_port_burst(dev, sch, port, 0);
		cs752x_sch_bypass_shaper(dev, sch, port + SCH_LT_BASE_ID, 0);
		cs752x_sch_bypass_shaper(dev, sch, port + SCH_ST_BASE_ID, 0);

		for (q = 0; q < SCH_MAX_PORT_QUEUE; q++) {
			cs752x_sch_enable_voq_drr(dev, sch,
						  port * SCH_MAX_PORT_QUEUE + q,
						  SCH_DEFAULT_QUANTA, cpu_port);
			cs752x_sch_bypass_shaper(dev, sch,
						 port + SCH_DRR_BASE_ID,
						 cpu_port);
		}
	}

	val = readl(sch + SCH_SHAPER_CONFIGURATION);
	val &= ~SCH_SHAPER_CONFIGURATION_CLOCKS_PER_ROUND_ST;
	val |= FIELD_PREP(SCH_SHAPER_CONFIGURATION_CLOCKS_PER_ROUND_ST,
			  SCH_CLOCKS_PER_ROUND_ST);
	val |= SCH_SHAPER_CONFIGURATION_GLOBAL_SCRUB_ENABLE;
	writel(val, sch + SCH_SHAPER_CONFIGURATION);

	val = readl(sch + SCH_DEBUG_CONTROL);
	val &= ~SCH_DEBUG_CONTROL_SWITCH_EXPR_MODE_LOWLVL;
	val |= FIELD_PREP(SCH_DEBUG_CONTROL_SWITCH_EXPR_MODE_LOWLVL,
			  SCH_EXPR_MODE_LOWLVL);
	writel(val, sch + SCH_DEBUG_CONTROL);

	return 0;
}

static int cs752x_tm_wait_init_done(struct device *dev, void __iomem *tm,
				    u32 status_off, u32 init_done,
				    const char *name)
{
	u32 val;
	int ret;

	ret = readl_poll_timeout(tm + status_off, val, val & init_done, 10,
				 TM_INIT_TIMEOUT_US);
	if (ret)
		dev_dbg(dev, "tm %s init not done, status %#010x\n", name,
			val);

	return ret;
}

static int cs752x_tm_mem_write(struct device *dev, void __iomem *tm,
			       u32 cfg_off, u32 cpu_full_access, u32 access_off,
			       u32 addr, const struct cs752x_tm_mem_word *words,
			       unsigned int nwords)
{
	unsigned int i;
	u32 val;
	int ret;

	writel(readl(tm + cfg_off) | cpu_full_access, tm + cfg_off);

	for (i = 0; i < nwords; i++)
		writel(words[i].val, tm + words[i].off);

	writel(TM_MEM_ACCESS_ACCESS | TM_MEM_ACCESS_RBW | addr,
	       tm + access_off);
	ret = readl_poll_timeout(tm + access_off, val,
				 !(val & TM_MEM_ACCESS_ACCESS), 0,
				 TM_ACCESS_TIMEOUT_US);

	writel(readl(tm + cfg_off) & ~cpu_full_access, tm + cfg_off);

	if (ret)
		dev_err(dev, "tm memory write %#03x entry %#x timed out\n",
			access_off, addr);

	return ret;
}

static int cs752x_tm_bm_set_voq_profile(struct device *dev, void __iomem *tm,
					unsigned int profile,
					unsigned int min_depth,
					unsigned int max_depth)
{
	struct cs752x_tm_mem_word words[2];

	words[0].off = TM_BM_VOQ_PROFILE_MEM_DATA0;
	words[0].val = FIELD_PREP(TM_BM_VOQ_PROFILE_MEM_DATA0_VOQ_MIN_DEPTH,
				  min_depth) |
		       FIELD_PREP(TM_BM_VOQ_PROFILE_MEM_DATA0_VOQ_MAX_DEPTH,
				  max_depth);
	words[1].off = TM_BM_VOQ_PROFILE_MEM_DATA1;
	words[1].val = 0;

	return cs752x_tm_mem_write(dev, tm, TM_BM_CONFIG_0,
				   TM_BM_CONFIG_0_CPU_FULL_ACCESS,
				   TM_BM_VOQ_PROFILE_MEM_ACCESS,
				   FIELD_PREP(TM_BM_VOQ_PROFILE_MEM_ACCESS_ADDR,
					      profile),
				   words, 2);
}

static int cs752x_tm_bm_set_voq_mem(struct device *dev, void __iomem *tm,
				    unsigned int voq)
{
	struct cs752x_tm_mem_word word;
	unsigned int profile, wred_profile, dest_port;

	profile = voq >= TM_BM_CPU_VOQ_PROFILE_FIRST &&
		  voq < TM_BM_CPU_VOQ_PROFILE_END ? 1 : 0;
	wred_profile = voq & 0x1f;
	dest_port = (voq >> 3) & 0x7;

	word.off = TM_BM_VOQ_MEM_DATA;
	word.val = FIELD_PREP(TM_BM_VOQ_MEM_DATA_VOQ_PROFILE, profile) |
		   FIELD_PREP(TM_BM_VOQ_MEM_DATA_WRED_PROFILE, wred_profile) |
		   FIELD_PREP(TM_BM_VOQ_MEM_DATA_DEST_PORT, dest_port) |
		   FIELD_PREP(TM_BM_VOQ_MEM_DATA_VOQ_CNTR_ENABLE, 0) |
		   FIELD_PREP(TM_BM_VOQ_MEM_DATA_VOQ_CNTR, 0) |
		   FIELD_PREP(TM_BM_VOQ_MEM_DATA_WRED_ENABLE, 0);

	return cs752x_tm_mem_write(dev, tm, TM_BM_CONFIG_0,
				   TM_BM_CONFIG_0_CPU_FULL_ACCESS,
				   TM_BM_VOQ_MEM_ACCESS,
				   FIELD_PREP(TM_BM_VOQ_MEM_ACCESS_ADDR, voq),
				   &word, 1);
}

static int cs752x_tm_bm_init(struct device *dev, void __iomem *tm,
			     bool internal_buf)
{
	unsigned int voq;
	u32 val, cfg;
	int ret;

	val = readl(tm + TM_BM_WRED_CONFIG_1);
	val &= ~TM_BM_WRED_CONFIG_1_WRED_PROFILE_SELECT_MODE;
	val |= FIELD_PREP(TM_BM_WRED_CONFIG_1_WRED_PROFILE_SELECT_MODE,
			  TM_BM_WRED_PROFILE_SELECT_DIRECT);
	writel(val, tm + TM_BM_WRED_CONFIG_1);

	cs752x_tm_wait_init_done(dev, tm, TM_BM_STATUS_0,
				 TM_BM_STATUS_0_INIT_DONE, "bm");

	cfg = readl(tm + TM_BM_CONFIG_0);
	cfg &= ~(TM_BM_CONFIG_0_DISABLE |
		 TM_BM_CONFIG_0_RECALCULATE_GLOBAL_BUFFERS_USED |
		 TM_BM_CONFIG_0_BM2_BYPASS_CA_PROCESS |
		 TM_BM_CONFIG_0_BM2_BYPASS_DEST_PORT_PROCESS |
		 TM_BM_CONFIG_0_BM2_BYPASS_VOQ_PROCESS |
		 TM_BM_CONFIG_0_BM1_BYPASS_CA_PROCESS |
		 TM_BM_CONFIG_0_BM1_BYPASS_DEST_PORT_PROCESS |
		 TM_BM_CONFIG_0_BM1_BYPASS_VOQ_PROCESS |
		 TM_BM_CONFIG_0_ENQUE_HDR_ADJUST |
		 TM_BM_CONFIG_0_INIT);

	writel(cfg | TM_BM_CONFIG_0_BM1_BYPASS_CA_PROCESS |
	       TM_BM_CONFIG_0_BM2_BYPASS_DEST_PORT_PROCESS |
	       TM_BM_CONFIG_0_BM2_BYPASS_VOQ_PROCESS |
	       TM_BM_CONFIG_0_INIT, tm + TM_BM_CONFIG_0);
	cs752x_tm_wait_init_done(dev, tm, TM_BM_STATUS_0,
				 TM_BM_STATUS_0_INIT_DONE, "bm");

	writel(cfg, tm + TM_BM_CONFIG_0);
	ret = cs752x_tm_wait_init_done(dev, tm, TM_BM_STATUS_0,
				       TM_BM_STATUS_0_INIT_DONE, "bm");
	if (ret)
		return ret;

	writel(FIELD_PREP(TM_BM_TRAFFIC_CONTROL_2_WARNING_THRESHOLD_INT, 0),
	       tm + TM_BM_TRAFFIC_CONTROL_2);
	writel(FIELD_PREP(TM_BM_TRAFFIC_CONTROL_3_LIVING_THRESHOLD_INT,
			  TM_BM_DEF_LIVING_THSHLD_INT) |
	       FIELD_PREP(TM_BM_TRAFFIC_CONTROL_3_DYING_GASP_THRESHOLD_INT,
			  TM_BM_DEF_DYING_GASP_INT),
	       tm + TM_BM_TRAFFIC_CONTROL_3);

	ret = cs752x_tm_bm_set_voq_profile(dev, tm, 0, TM_BM_DEF_VOQ_MIN_DEPTH,
					   internal_buf ?
					   TM_BM_DEF_VOQ_MAX_DEPTH_INT_BUFF :
					   TM_BM_DEF_VOQ_MAX_DEPTH_EXT_BUFF);
	if (ret)
		return ret;

	ret = cs752x_tm_bm_set_voq_profile(dev, tm, 1,
					   TM_BM_DEF_CPU_VOQ_MIN_DEPTH,
					   TM_BM_DEF_CPU_VOQ_MAX_DEPTH);
	if (ret)
		return ret;

	for (voq = 0; voq < TM_BM_VOQ_NO; voq++) {
		ret = cs752x_tm_bm_set_voq_mem(dev, tm, voq);
		if (ret)
			return ret;
	}

	return 0;
}

static int cs752x_tm_pol_set_flow_policer_disabled(struct device *dev,
						   void __iomem *tm,
						   unsigned int id)
{
	struct cs752x_tm_mem_word words[2];

	words[0].off = TM_POL_FLOW_PROFILE_MEM_DATA0;
	words[0].val = 0;
	words[1].off = TM_POL_FLOW_PROFILE_MEM_DATA1;
	words[1].val = 0;

	return cs752x_tm_mem_write(dev, tm, TM_POL_CONFIG_0,
				   TM_POL_CONFIG_0_CPU_FULL_ACCESS,
				   TM_POL_FLOW_PROFILE_MEM_ACCESS,
				   FIELD_PREP(TM_POL_FLOW_PROFILE_MEM_ACCESS_ADDR,
					      id),
				   words, 2);
}

static int cs752x_tm_pol_init(struct device *dev, void __iomem *tm)
{
	unsigned int id;
	u32 val, cfg;
	int ret;

	val = readl(tm + TM_POL_CONFIG_SPID);
	val &= ~TM_POL_CONFIG_COMMON_MASK;
	val |= FIELD_PREP(TM_POL_CONFIG_NEST_LEVEL, TM_POL_NEST_LEVEL_SPID);
	writel(val, tm + TM_POL_CONFIG_SPID);

	val = readl(tm + TM_POL_CONFIG_FLOW);
	val &= ~TM_POL_CONFIG_COMMON_MASK;
	val |= FIELD_PREP(TM_POL_CONFIG_NEST_LEVEL, TM_POL_NEST_LEVEL_FLOW);
	writel(val, tm + TM_POL_CONFIG_FLOW);

	val = readl(tm + TM_POL_CONFIG_CPU);
	val &= ~(TM_POL_CONFIG_COMMON_MASK | TM_POL_CONFIG_CPU_VOQ0 |
		 TM_POL_CONFIG_CPU_VOQ1);
	val |= TM_POL_CONFIG_DISABLE;
	writel(val, tm + TM_POL_CONFIG_CPU);

	val = readl(tm + TM_POL_CONFIG_PKT_TYPE);
	val &= ~(TM_POL_CONFIG_COMMON_MASK | TM_POL_CONFIG_PKT_TYPE_LSPID0 |
		 TM_POL_CONFIG_PKT_TYPE_LSPID1 | TM_POL_CONFIG_PKT_TYPE_LSPID2 |
		 TM_POL_CONFIG_PKT_TYPE_LSPID3);
	val |= FIELD_PREP(TM_POL_CONFIG_NEST_LEVEL,
			  TM_POL_NEST_LEVEL_PKT_TYPE) |
	       FIELD_PREP(TM_POL_CONFIG_PKT_TYPE_LSPID0, 0) |
	       FIELD_PREP(TM_POL_CONFIG_PKT_TYPE_LSPID1, 1) |
	       FIELD_PREP(TM_POL_CONFIG_PKT_TYPE_LSPID2, 2) |
	       FIELD_PREP(TM_POL_CONFIG_PKT_TYPE_LSPID3, 0);
	writel(val, tm + TM_POL_CONFIG_PKT_TYPE);

	cs752x_tm_wait_init_done(dev, tm, TM_POL_STATUS_0,
				 TM_POL_STATUS_0_INIT_DONE, "pol");

	cfg = readl(tm + TM_POL_CONFIG_0);
	cfg &= ~(TM_POL_CONFIG_0_POL_DISABLE | TM_POL_CONFIG_0_POL_BYPASS_RED |
		 TM_POL_CONFIG_0_POL_BYPASS_YELLOW |
		 TM_POL_CONFIG_0_POL_COLOR_BLIND | TM_POL_CONFIG_0_INIT);

	writel(cfg | TM_POL_CONFIG_0_INIT, tm + TM_POL_CONFIG_0);
	cs752x_tm_wait_init_done(dev, tm, TM_POL_STATUS_0,
				 TM_POL_STATUS_0_INIT_DONE, "pol");

	writel(cfg, tm + TM_POL_CONFIG_0);
	ret = cs752x_tm_wait_init_done(dev, tm, TM_POL_STATUS_0,
				       TM_POL_STATUS_0_INIT_DONE, "pol");
	if (ret)
		return ret;

	for (id = 0; id < TM_POL_FLOW_POL_NO; id++) {
		ret = cs752x_tm_pol_set_flow_policer_disabled(dev, tm, id);
		if (ret)
			return ret;
	}

	return 0;
}

static int cs752x_tm_pm_init(struct device *dev, void __iomem *tm)
{
	u32 cfg;

	cs752x_tm_wait_init_done(dev, tm, TM_PM_STATUS_0,
				 TM_PM_STATUS_0_INIT_DONE, "pm");

	cfg = readl(tm + TM_PM_CONFIG_0);
	cfg &= ~(TM_PM_CONFIG_0_DISABLE | TM_PM_CONFIG_0_AUTO_CLEAR_ON_READ_MODE |
		 TM_PM_CONFIG_0_INIT);
	cfg |= FIELD_PREP(TM_PM_CONFIG_0_AUTO_CLEAR_ON_READ_MODE,
			  TM_PM_READ_MODE_CLEAR_ALL);

	writel(cfg | TM_PM_CONFIG_0_INIT, tm + TM_PM_CONFIG_0);
	cs752x_tm_wait_init_done(dev, tm, TM_PM_STATUS_0,
				 TM_PM_STATUS_0_INIT_DONE, "pm");

	writel(cfg, tm + TM_PM_CONFIG_0);

	return cs752x_tm_wait_init_done(dev, tm, TM_PM_STATUS_0,
					TM_PM_STATUS_0_INIT_DONE, "pm");
}

int cs752x_tm_init(struct device *dev, void __iomem *tm, bool internal_buf)
{
	int ret, err = 0;

	ret = cs752x_tm_pol_init(dev, tm);
	if (ret)
		err = ret;
	ret = cs752x_tm_bm_init(dev, tm, internal_buf);
	if (ret && !err)
		err = ret;
	ret = cs752x_tm_pm_init(dev, tm);
	if (ret && !err)
		err = ret;
	return err;
}
