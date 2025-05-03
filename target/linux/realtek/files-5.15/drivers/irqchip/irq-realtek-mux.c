// SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-2-Clause)
/*
 * Realtek IRQ mux
 *
 * Copyright (c) 2017 - 2021 Realtek Semiconductor Corporation
 *
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/irqchip.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/mfd/syscon.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#define RTK_MUX_IRQ_ALWAYS_ENABLED  (-1)

struct realtek_irq_mux_data;

/**
 * struct realtek_irq_mux_subset_cfg
 *
 * @ints_mask: handler of a interrupt source only handles
 *             ints bits in the mask.
 */
struct realtek_irq_mux_subset_cfg {
	unsigned int ints_mask;
};

struct realtek_irq_mux_info {
	unsigned int isr_offset;
	unsigned int umsk_isr_offset;
	unsigned int scpu_int_en_offset;
	const u32 *isr_to_scpu_int_en_mask;
	const struct realtek_irq_mux_subset_cfg *cfg;
	int cfg_num;
};

/**
 * struct realtek_irq_mux_subset_data
 *
 * @cfg: cfg of the subset
 * @common: common data
 * @parent_irq: interrupt source
 */
struct realtek_irq_mux_subset_data {
	const struct realtek_irq_mux_subset_cfg *cfg;
	struct realtek_irq_mux_data *common;
	int parent_irq;
};

struct realtek_irq_mux_data {
	struct regmap *base;
	const struct realtek_irq_mux_info *info;
	struct irq_domain *domain;
	spinlock_t lock;
	int subset_data_num;
	struct realtek_irq_mux_subset_data subset_data[0];
};

static unsigned int realtek_mux_irq_get_ints(struct realtek_irq_mux_data *data)
{
	unsigned int val;

	regmap_read(data->base, data->info->isr_offset, &val);
	return val;
}

static void realtek_mux_irq_clear_ints_bit(struct realtek_irq_mux_data *data, int bit)
{
	regmap_write(data->base, data->info->isr_offset, BIT(bit) & ~1);
}

static unsigned int realtek_mux_irq_get_inte(struct realtek_irq_mux_data *data)
{
	unsigned int val;
	unsigned long flags;

	spin_lock_irqsave(&data->lock, flags);
	regmap_read(data->base, data->info->scpu_int_en_offset, &val);
	spin_unlock_irqrestore(&data->lock, flags);

	return val;
}

static void realtek_mux_irq_handle(struct irq_desc *desc)
{
	struct realtek_irq_mux_subset_data *subset_data = irq_desc_get_handler_data(desc);
	struct realtek_irq_mux_data *data = subset_data->common;
	struct irq_chip *chip = irq_desc_get_chip(desc);
	u32 ints, inte, mask;
	int i;

	chained_irq_enter(chip, desc);

	ints = realtek_mux_irq_get_ints(data) & subset_data->cfg->ints_mask;
	inte = realtek_mux_irq_get_inte(data);

	while (ints)
	{
		i = __ffs(ints);
		ints &= ~BIT(i);

		mask = data->info->isr_to_scpu_int_en_mask[i];
		if (mask != RTK_MUX_IRQ_ALWAYS_ENABLED && !(inte & mask))
			continue;

		generic_handle_irq(irq_find_mapping(data->domain, i));
		realtek_mux_irq_clear_ints_bit(data, i);
	}

	chained_irq_exit(chip, desc);
}

static void realtek_mux_mask_irq(struct irq_data *data)
{
	struct realtek_irq_mux_data *mux_data = irq_data_get_irq_chip_data(data);

	regmap_write(mux_data->base, mux_data->info->isr_offset, BIT(data->hwirq));
}

static void realtek_mux_unmask_irq(struct irq_data *data)
{
	struct realtek_irq_mux_data *mux_data = irq_data_get_irq_chip_data(data);

	regmap_write(mux_data->base, mux_data->info->umsk_isr_offset, BIT(data->hwirq));
}

static void realtek_mux_enable_irq(struct irq_data *data)
{
	struct realtek_irq_mux_data *mux_data = irq_data_get_irq_chip_data(data);
	unsigned long flags;
	u32 scpu_int_en, mask;

	mask = mux_data->info->isr_to_scpu_int_en_mask[data->hwirq];
	if (!mask)
		return;

	spin_lock_irqsave(&mux_data->lock, flags);

	regmap_read(mux_data->base, mux_data->info->scpu_int_en_offset, &scpu_int_en);
	scpu_int_en |= mask;
	regmap_write(mux_data->base, mux_data->info->umsk_isr_offset, scpu_int_en);

	spin_unlock_irqrestore(&mux_data->lock, flags);
}

static void realtek_mux_disable_irq(struct irq_data *data)
{
	struct realtek_irq_mux_data *mux_data = irq_data_get_irq_chip_data(data);
	unsigned long flags;
	u32 scpu_int_en, mask;

	mask = mux_data->info->isr_to_scpu_int_en_mask[data->hwirq];
	if (!mask)
		return;

	spin_lock_irqsave(&mux_data->lock, flags);

	regmap_read(mux_data->base, mux_data->info->scpu_int_en_offset, &scpu_int_en);
	scpu_int_en &= ~mask;
	regmap_write(mux_data->base, mux_data->info->umsk_isr_offset, scpu_int_en);

	spin_unlock_irqrestore(&mux_data->lock, flags);
}

static int lookup_parent_irq(struct realtek_irq_mux_data *mux_data, struct irq_data *d)
{
	unsigned int mask = BIT(d->hwirq);
	int i;

	for (i = 0; i < mux_data->subset_data_num; i++)
		if (mux_data->subset_data[i].cfg->ints_mask & mask)
			return mux_data->subset_data[i].parent_irq;

	return -EINVAL;
}

static int realtek_mux_set_affinity(struct irq_data *d,
			const struct cpumask *mask_val, bool force)
{
	struct realtek_irq_mux_data *mux_data = irq_data_get_irq_chip_data(d);
	int irq;
	struct irq_chip *chip;
	struct irq_data *data;

	irq = lookup_parent_irq(mux_data, d);
	if (irq < 0)
		return irq;

	chip = irq_get_chip(irq);
	data = irq_get_irq_data(irq);

	irq_data_update_effective_affinity(d, cpu_online_mask);
	if (chip && chip->irq_set_affinity)
		return chip->irq_set_affinity(data, mask_val, force);
	else
		return -EINVAL;
}

static struct irq_chip realtek_mux_irq_chip = {
	.name			= "realtek-irq-mux",
	.irq_mask		= realtek_mux_mask_irq,
	.irq_unmask		= realtek_mux_unmask_irq,
	.irq_enable		= realtek_mux_enable_irq,
	.irq_disable		= realtek_mux_disable_irq,
	.irq_set_affinity	= realtek_mux_set_affinity,
};

static int realtek_mux_irq_domain_map(struct irq_domain *d,
		unsigned int irq, irq_hw_number_t hw)
{
	struct realtek_irq_mux_data *data = d->host_data;

	irq_set_chip_and_handler(irq, &realtek_mux_irq_chip, handle_level_irq);
	irq_set_chip_data(irq, data);
	irq_set_probe(irq);

	return 0;
}

static const struct irq_domain_ops realtek_mux_irq_domain_ops = {
	.xlate	= irq_domain_xlate_onecell,
	.map	= realtek_mux_irq_domain_map,
};

enum rtd13xx_iso_isr_bits {
	RTD13XX_ISO_ISR_TC3_SHIFT =		1,
	RTD13XX_ISO_ISR_UR0_SHIFT =		2,
	RTD13XX_ISO_ISR_LSADC0_SHIFT =		3,
	RTD13XX_ISO_ISR_IRDA_SHIFT =		5,
	RTD13XX_ISO_ISR_SPI1_SHIFT =		6,
	RTD13XX_ISO_ISR_WDOG_NMI_SHIFT =	7,
	RTD13XX_ISO_ISR_I2C0_SHIFT =		8,
	RTD13XX_ISO_ISR_TC4_SHIFT =		9,
	RTD13XX_ISO_ISR_TC7_SHIFT =		10,
	RTD13XX_ISO_ISR_I2C1_SHIFT =		11,
	RTD13XX_ISO_ISR_RTC_HSEC_SHIFT =	12,
	RTD13XX_ISO_ISR_RTC_ALARM_SHIFT =	13,
	RTD13XX_ISO_ISR_GPIOA_SHIFT =		19,
	RTD13XX_ISO_ISR_GPIODA_SHIFT =		20,
	RTD13XX_ISO_ISR_ISO_MISC_SHIFT =	21,
	RTD13XX_ISO_ISR_CBUS_SHIFT =		22,
	RTD13XX_ISO_ISR_ETN_SHIFT =		23,
	RTD13XX_ISO_ISR_USB_HOST_SHIFT =	24,
	RTD13XX_ISO_ISR_USB_U3_DRD_SHIFT =	25,
	RTD13XX_ISO_ISR_USB_U2_DRD_SHIFT =	26,
	RTD13XX_ISO_ISR_PORB_HV_SHIFT =		28,
	RTD13XX_ISO_ISR_PORB_DV_SHIFT =		29,
	RTD13XX_ISO_ISR_PORB_AV_SHIFT =		30,
	RTD13XX_ISO_ISR_I2C1_REQ_SHIFT =	31,
};

static const u32 rtd13xx_iso_isr_to_scpu_int_en_mask[32] = {
	[RTD13XX_ISO_ISR_SPI1_SHIFT]		= BIT(1),
	[RTD13XX_ISO_ISR_UR0_SHIFT]		= BIT(2),
	[RTD13XX_ISO_ISR_LSADC0_SHIFT]		= BIT(3),
	[RTD13XX_ISO_ISR_IRDA_SHIFT]		= BIT(5),
	[RTD13XX_ISO_ISR_I2C0_SHIFT]		= BIT(8),
	[RTD13XX_ISO_ISR_I2C1_SHIFT]		= BIT(11),
	[RTD13XX_ISO_ISR_RTC_HSEC_SHIFT]	= BIT(12),
	[RTD13XX_ISO_ISR_RTC_ALARM_SHIFT]	= BIT(13),
	[RTD13XX_ISO_ISR_GPIOA_SHIFT]		= BIT(19),
	[RTD13XX_ISO_ISR_GPIODA_SHIFT]		= BIT(20),
	[RTD13XX_ISO_ISR_PORB_HV_SHIFT]		= BIT(28),
	[RTD13XX_ISO_ISR_PORB_DV_SHIFT]		= BIT(29),
	[RTD13XX_ISO_ISR_PORB_AV_SHIFT]		= BIT(30),
	[RTD13XX_ISO_ISR_I2C1_REQ_SHIFT]	= BIT(31),
};

enum rtd13xx_misc_isr_bits {
	RTD13XX_ISR_WDOG_NMI_SHIFT =		2,
	RTD13XX_ISR_UR1_SHIFT =			3,
	RTD13XX_ISR_TC5_SHIFT =			4,
	RTD13XX_ISR_UR1_TO_SHIFT =		5,
	RTD13XX_ISR_TC0_SHIFT =			6,
	RTD13XX_ISR_TC1_SHIFT =			7,
	RTD13XX_ISR_UR2_SHIFT =			8,
	RTD13XX_ISR_RTC_HSEC_SHIFT =		9,
	RTD13XX_ISR_RTC_MIN_SHIFT =		10,
	RTD13XX_ISR_RTC_HOUR_SHIFT =		11,
	RTD13XX_ISR_RTC_DATE_SHIFT =		12,
	RTD13XX_ISR_UR2_TO_SHIFT =		13,
	RTD13XX_ISR_I2C5_SHIFT =		14,
	RTD13XX_ISR_I2C3_SHIFT =		23,
	RTD13XX_ISR_SC0_SHIFT =			24,
	RTD13XX_ISR_SC1_SHIFT =			25,
	RTD13XX_ISR_SPI_SHIFT =			27,
	RTD13XX_ISR_FAN_SHIFT =			29,
};

static const u32 rtd13xx_misc_isr_to_scpu_int_en_mask[32] = {
	[RTD13XX_ISR_UR1_SHIFT]			= BIT(3),
	[RTD13XX_ISR_UR1_TO_SHIFT]		= BIT(5),
	[RTD13XX_ISR_UR2_TO_SHIFT]		= BIT(6),
	[RTD13XX_ISR_UR2_SHIFT]			= BIT(7),
	[RTD13XX_ISR_RTC_MIN_SHIFT]		= BIT(10),
	[RTD13XX_ISR_RTC_HOUR_SHIFT]		= BIT(11),
	[RTD13XX_ISR_RTC_DATE_SHIFT]		= BIT(12),
	[RTD13XX_ISR_I2C5_SHIFT]		= BIT(14),
	[RTD13XX_ISR_SC0_SHIFT]			= BIT(24),
	[RTD13XX_ISR_SC1_SHIFT]			= BIT(25),
	[RTD13XX_ISR_SPI_SHIFT]			= BIT(27),
	[RTD13XX_ISR_I2C3_SHIFT]		= BIT(28),
	[RTD13XX_ISR_FAN_SHIFT]			= BIT(29),
	[RTD13XX_ISR_WDOG_NMI_SHIFT]		= RTK_MUX_IRQ_ALWAYS_ENABLED,
};

enum rtd16xxb_iso_isr_bits {
	RTD16XXB_ISO_ISR_TC3_SHIFT =		1,
	RTD16XXB_ISO_ISR_UR0_SHIFT =		2,
	RTD16XXB_ISO_ISR_LSADC0_SHIFT =		3,
	RTD16XXB_ISO_ISR_WDOG1_NMI_SHIFT =	4,
	RTD16XXB_ISO_ISR_IRDA_SHIFT =		5,
	RTD16XXB_ISO_ISR_SPI1_SHIFT =		6,
	RTD16XXB_ISO_ISR_WDOG2_NMI_SHIFT =	7,
	RTD16XXB_ISO_ISR_I2C0_SHIFT =		8,
	RTD16XXB_ISO_ISR_TC4_SHIFT =		9,
	RTD16XXB_ISO_ISR_TC7_SHIFT =		10,
	RTD16XXB_ISO_ISR_I2C1_SHIFT =		11,
	RTD16XXB_ISO_ISR_HIFI_WAKEUP_SHIFT =	14,
	RTD16XXB_ISO_ISR_WDOG4_NMI_SHIFT =	15,
	RTD16XXB_ISO_ISR_TC8_SHIFT =		16,
	RTD16XXB_ISO_ISR_VFD_SHIFT =		17,
	RTD16XXB_ISO_ISR_VTC_SHIFT =		18,
	RTD16XXB_ISO_ISR_GPIOA_SHIFT =		19,
	RTD16XXB_ISO_ISR_GPIODA_SHIFT =		20,
	RTD16XXB_ISO_ISR_ISO_MISC_SHIFT =	21,
	RTD16XXB_ISO_ISR_CBUS_SHIFT =		22,
	RTD16XXB_ISO_ISR_ETN_SHIFT =		23,
	RTD16XXB_ISO_ISR_USB_HOST_SHIFT =	24,
	RTD16XXB_ISO_ISR_USB_U3_DRD_SHIFT =	25,
	RTD16XXB_ISO_ISR_USB_U2_DRD_SHIFT =	26,
	RTD16XXB_ISO_ISR_WDOG3_NMI_SHIFT =	27,
	RTD16XXB_ISO_ISR_PORB_HV_CEN_SHIFT =	28,
	RTD16XXB_ISO_ISR_PORB_DV_CEN_SHIFT =	29,
	RTD16XXB_ISO_ISR_PORB_AV_CEN_SHIFT =	30,
	RTD16XXB_ISO_ISR_I2C1_REQ_SHIFT =	31,
};
static const u32 rtd16xxb_iso_isr_to_scpu_int_en_mask[32] = {
	[RTD16XXB_ISO_ISR_SPI1_SHIFT]		= BIT(1),
	[RTD16XXB_ISO_ISR_UR0_SHIFT]		= BIT(2),
	[RTD16XXB_ISO_ISR_LSADC0_SHIFT]		= BIT(3),
	[RTD16XXB_ISO_ISR_IRDA_SHIFT]		= BIT(5),
	[RTD16XXB_ISO_ISR_I2C0_SHIFT]		= BIT(8),
	[RTD16XXB_ISO_ISR_I2C1_SHIFT]		= BIT(11),
	[RTD16XXB_ISO_ISR_VFD_SHIFT]		= BIT(17),
	[RTD16XXB_ISO_ISR_GPIOA_SHIFT]		= BIT(19),
	[RTD16XXB_ISO_ISR_GPIODA_SHIFT]		= BIT(20),
	[RTD16XXB_ISO_ISR_PORB_HV_CEN_SHIFT]	= BIT(28),
	[RTD16XXB_ISO_ISR_PORB_DV_CEN_SHIFT]	= BIT(29),
	[RTD16XXB_ISO_ISR_PORB_AV_CEN_SHIFT]	= BIT(30),
	[RTD16XXB_ISO_ISR_I2C1_REQ_SHIFT]	= BIT(31),
	[RTD16XXB_ISO_ISR_WDOG1_NMI_SHIFT]      = RTK_MUX_IRQ_ALWAYS_ENABLED,
	[RTD16XXB_ISO_ISR_WDOG2_NMI_SHIFT]      = RTK_MUX_IRQ_ALWAYS_ENABLED,
	[RTD16XXB_ISO_ISR_WDOG3_NMI_SHIFT]      = RTK_MUX_IRQ_ALWAYS_ENABLED,
	[RTD16XXB_ISO_ISR_WDOG4_NMI_SHIFT]      = RTK_MUX_IRQ_ALWAYS_ENABLED,
};
enum rtd16xxb_misc_isr_bits {
	RTD16XXB_ISR_UR1_SHIFT =		3,
	RTD16XXB_ISR_TC5_SHIFT =		4,
	RTD16XXB_ISR_UR1_TO_SHIFT =		5,
	RTD16XXB_ISR_TC0_SHIFT =		6,
	RTD16XXB_ISR_TC1_SHIFT =		7,
	RTD16XXB_ISR_UR2_SHIFT =		8,
	RTD16XXB_ISR_UR2_TO_SHIFT =		13,
	RTD16XXB_ISR_I2C5_SHIFT =		14,
	RTD16XXB_ISR_I2C4_SHIFT =		15,
	RTD16XXB_ISR_I2C3_SHIFT =		23,
	RTD16XXB_ISR_SC0_SHIFT =		24,
	RTD16XXB_ISR_SC1_SHIFT =		25,
	RTD16XXB_ISR_SPI_SHIFT =		27,
	RTD16XXB_ISR_FAN_SHIFT =		29,
};
static const u32 rtd16xxb_misc_isr_to_scpu_int_en_mask[32] = {
	[RTD16XXB_ISR_UR1_SHIFT]		= BIT(3),
	[RTD16XXB_ISR_UR1_TO_SHIFT]		= BIT(5),
	[RTD16XXB_ISR_UR2_TO_SHIFT]		= BIT(6),
	[RTD16XXB_ISR_UR2_SHIFT]		= BIT(7),
	[RTD16XXB_ISR_I2C5_SHIFT]		= BIT(14),
	[RTD16XXB_ISR_I2C4_SHIFT]		= BIT(15),
	[RTD16XXB_ISR_SC0_SHIFT]		= BIT(24),
	[RTD16XXB_ISR_SC1_SHIFT]		= BIT(25),
	[RTD16XXB_ISR_SPI_SHIFT]		= BIT(27),
	[RTD16XXB_ISR_I2C3_SHIFT]		= BIT(28),
	[RTD16XXB_ISR_FAN_SHIFT]		= BIT(29),
};

enum rtd129x_iso_isr_bits {
        RTD129X_ISO_ISR_UR0_SHIFT               = 2,
        RTD129X_ISO_ISR_IRDA_SHIFT              = 5,
        RTD129X_ISO_ISR_I2C0_SHIFT              = 8,
        RTD129X_ISO_ISR_I2C1_SHIFT              = 11,
        RTD129X_ISO_ISR_RTC_HSEC_SHIFT          = 12,
        RTD129X_ISO_ISR_RTC_ALARM_SHIFT         = 13,
        RTD129X_ISO_ISR_GPIOA_SHIFT             = 19,
        RTD129X_ISO_ISR_GPIODA_SHIFT            = 20,
        RTD129X_ISO_ISR_GPHY_DV_SHIFT           = 29,
        RTD129X_ISO_ISR_GPHY_AV_SHIFT           = 30,
        RTD129X_ISO_ISR_I2C1_REQ_SHIFT          = 31,
};

static const u32 rtd129x_iso_isr_to_scpu_int_en_mask[32] = {
        [RTD129X_ISO_ISR_UR0_SHIFT]             = BIT(2),
        [RTD129X_ISO_ISR_IRDA_SHIFT]            = BIT(5),
        [RTD129X_ISO_ISR_I2C0_SHIFT]            = BIT(8),
        [RTD129X_ISO_ISR_I2C1_SHIFT]            = BIT(11),
        [RTD129X_ISO_ISR_RTC_HSEC_SHIFT]        = BIT(12),
        [RTD129X_ISO_ISR_RTC_ALARM_SHIFT]       = BIT(13),
        [RTD129X_ISO_ISR_GPIOA_SHIFT]           = BIT(19),
        [RTD129X_ISO_ISR_GPIODA_SHIFT]          = BIT(20),
        [RTD129X_ISO_ISR_GPHY_DV_SHIFT]         = BIT(29),
        [RTD129X_ISO_ISR_GPHY_AV_SHIFT]         = BIT(30),
        [RTD129X_ISO_ISR_I2C1_REQ_SHIFT]        = BIT(31),
};

enum rtd129x_misc_isr_bits {
	RTD129X_MIS_ISR_WDOG_NMI_SHIFT		= 2,
	RTD129X_MIS_ISR_UR1_SHIFT		= 3,
	RTD129X_MIS_ISR_UR1_TO_SHIFT		= 5,
	RTD129X_MIS_ISR_UR2_SHIFT		= 8,
	RTD129X_MIS_ISR_RTC_MIN_SHIFT		= 10,
	RTD129X_MIS_ISR_RTC_HOUR_SHIFT		= 11,
	RTD129X_MIS_ISR_RTC_DATA_SHIFT		= 12,
	RTD129X_MIS_ISR_UR2_TO_SHIFT		= 13,
	RTD129X_MIS_ISR_I2C5_SHIFT		= 14,
	RTD129X_MIS_ISR_I2C4_SHIFT		= 15,
	RTD129X_MIS_ISR_GPIOA_SHIFT		= 19,
	RTD129X_MIS_ISR_GPIODA_SHIFT		= 20,
	RTD129X_MIS_ISR_LSADC0_SHIFT		= 21,
	RTD129X_MIS_ISR_LSADC1_SHIFT		= 22,
	RTD129X_MIS_ISR_I2C3_SHIFT		= 23,
	RTD129X_MIS_ISR_SC0_SHIFT		= 24,
	RTD129X_MIS_ISR_I2C2_SHIFT		= 26,
	RTD129X_MIS_ISR_GSPI_SHIFT		= 27,
	RTD129X_MIS_ISR_FAN_SHIFT		= 29,
};

static const u32 rtd129x_misc_isr_to_scpu_int_en_mask[32] = {
	[RTD129X_MIS_ISR_UR1_SHIFT]		= BIT(3),
	[RTD129X_MIS_ISR_UR1_TO_SHIFT]		= BIT(5),
	[RTD129X_MIS_ISR_UR2_TO_SHIFT]		= BIT(6),
	[RTD129X_MIS_ISR_UR2_SHIFT]		= BIT(7),
	[RTD129X_MIS_ISR_RTC_MIN_SHIFT]		= BIT(10),
	[RTD129X_MIS_ISR_RTC_HOUR_SHIFT]	= BIT(11),
	[RTD129X_MIS_ISR_RTC_DATA_SHIFT]	= BIT(12),
	[RTD129X_MIS_ISR_I2C5_SHIFT]		= BIT(14),
	[RTD129X_MIS_ISR_I2C4_SHIFT]		= BIT(15),
	[RTD129X_MIS_ISR_GPIOA_SHIFT]		= BIT(19),
	[RTD129X_MIS_ISR_GPIODA_SHIFT]		= BIT(20),
	[RTD129X_MIS_ISR_LSADC0_SHIFT]		= BIT(21),
	[RTD129X_MIS_ISR_LSADC1_SHIFT]		= BIT(22),
	[RTD129X_MIS_ISR_SC0_SHIFT]		= BIT(24),
	[RTD129X_MIS_ISR_I2C2_SHIFT]		= BIT(26),
	[RTD129X_MIS_ISR_GSPI_SHIFT]		= BIT(27),
	[RTD129X_MIS_ISR_I2C3_SHIFT]		= BIT(28),
	[RTD129X_MIS_ISR_FAN_SHIFT]		= BIT(29),
	[RTD129X_MIS_ISR_WDOG_NMI_SHIFT]	= RTK_MUX_IRQ_ALWAYS_ENABLED,
};

static struct realtek_irq_mux_subset_cfg rtd13xx_iso_irq_cfgs[] = {
	{ 0xffffcffe, },
	{ 0x00003001, }, /* rtc */
};

static const struct realtek_irq_mux_info rtd13xx_iso_irq_mux_info = {
	.isr_offset		= 0x0,
	.umsk_isr_offset	= 0x4,
	.scpu_int_en_offset	= 0x40,
	.isr_to_scpu_int_en_mask = rtd13xx_iso_isr_to_scpu_int_en_mask,
	.cfg = rtd13xx_iso_irq_cfgs,
	.cfg_num = ARRAY_SIZE(rtd13xx_iso_irq_cfgs),
};

static struct realtek_irq_mux_subset_cfg rtd13xx_misc_irq_cfgs[] = {
	{ 0xffffc0d2, },
	{ 0x00000004, }, /* wdt */
	{ 0x00000028, }, /* ur1 */
	{ 0x00002100, }, /* ur2 */
};

static const struct realtek_irq_mux_info rtd13xx_misc_irq_mux_info = {
	.umsk_isr_offset	= 0x8,
	.isr_offset		= 0xc,
	.scpu_int_en_offset	= 0x80,
	.isr_to_scpu_int_en_mask = rtd13xx_misc_isr_to_scpu_int_en_mask,
	.cfg = rtd13xx_misc_irq_cfgs,
	.cfg_num = ARRAY_SIZE(rtd13xx_misc_irq_cfgs),
};

static struct realtek_irq_mux_subset_cfg rtd16xxb_iso_irq_cfgs[] = {
	{ 0xf7ff7f6e, },
	{ 0x08008090, }, /* wdt */
};

static const struct realtek_irq_mux_info rtd16xxb_iso_irq_mux_info = {
	.isr_offset		= 0x0,
	.umsk_isr_offset	= 0x4,
	.scpu_int_en_offset	= 0x40,
	.isr_to_scpu_int_en_mask = rtd16xxb_iso_isr_to_scpu_int_en_mask,
	.cfg = rtd16xxb_iso_irq_cfgs,
	.cfg_num = ARRAY_SIZE(rtd16xxb_iso_irq_cfgs),
};

static struct realtek_irq_mux_subset_cfg rtd16xxb_misc_irq_cfgs[] = {
	{ 0xffffded6, },
	{ 0x00000028, }, /* ur1 */
	{ 0x00002100, }, /* ur2 */
};

static const struct realtek_irq_mux_info rtd16xxb_misc_irq_mux_info = {
	.umsk_isr_offset	= 0x8,
	.isr_offset		= 0xc,
	.scpu_int_en_offset	= 0x80,
	.isr_to_scpu_int_en_mask = rtd16xxb_misc_isr_to_scpu_int_en_mask,
	.cfg = rtd16xxb_misc_irq_cfgs,
	.cfg_num = ARRAY_SIZE(rtd16xxb_misc_irq_cfgs),
};

static struct realtek_irq_mux_subset_cfg rtd129x_iso_irq_cfgs[] = {
	{ 0xffffcffe, },
	{ 0x00003001, }, /* rtc */
};

static const struct realtek_irq_mux_info rtd129x_iso_irq_mux_info = {
	.isr_offset		= 0x0,
	.umsk_isr_offset	= 0x4,
	.scpu_int_en_offset	= 0x40,
	.isr_to_scpu_int_en_mask = rtd129x_iso_isr_to_scpu_int_en_mask,
	.cfg = rtd129x_iso_irq_cfgs,
	.cfg_num = ARRAY_SIZE(rtd129x_iso_irq_cfgs),
};

static struct realtek_irq_mux_subset_cfg rtd129x_misc_irq_cfgs[] = {
	{ 0xfffffff2, },
	{ 0x00000004, }, /* wdt */
};

static const struct realtek_irq_mux_info rtd129x_misc_irq_mux_info = {
	.umsk_isr_offset	= 0x8,
	.isr_offset		= 0xc,
	.scpu_int_en_offset	= 0x80,
	.isr_to_scpu_int_en_mask = rtd129x_misc_isr_to_scpu_int_en_mask,
	.cfg = rtd129x_misc_irq_cfgs,
	.cfg_num = ARRAY_SIZE(rtd129x_misc_irq_cfgs),
};


static const struct of_device_id realtek_irq_mux_dt_matches[] = {
	{
		.compatible = "realtek,rtd13xx-iso-irq-mux",
		.data = &rtd13xx_iso_irq_mux_info,
	}, {
		.compatible = "realtek,rtd13xx-misc-irq-mux",
		.data = &rtd13xx_misc_irq_mux_info,
	}, {
		.compatible = "realtek,rtd16xxb-iso-irq-mux",
		.data = &rtd16xxb_iso_irq_mux_info,
	}, {
		.compatible = "realtek,rtd16xxb-misc-irq-mux",
		.data = &rtd16xxb_misc_irq_mux_info,
	}, {
		.compatible = "realtek,rtd129x-iso-irq-mux",
		.data = &rtd129x_iso_irq_mux_info,
	}, {
		.compatible = "realtek,rtd129x-misc-irq-mux",
		.data = &rtd129x_iso_irq_mux_info,
	},
	{ /* sentinel */ }
};

static int __init realtek_irq_mux_init_subset(struct device_node *node,
		struct realtek_irq_mux_data *data, int index)
{
	int irq;
	struct realtek_irq_mux_subset_data *subset_data = &data->subset_data[index];
	const struct realtek_irq_mux_subset_cfg *cfg = &data->info->cfg[index];

	irq = irq_of_parse_and_map(node, index);
	if (irq <= 0)
		return irq;

	subset_data->common     = data;
	subset_data->cfg        = cfg;
	subset_data->parent_irq = irq;
	irq_set_chained_handler_and_data(irq, realtek_mux_irq_handle, subset_data);
	return 0;
}

static int __init realtek_irq_mux_init(struct device_node *node,
				       struct device_node *parent)
{
	const struct of_device_id *match;
	const struct realtek_irq_mux_info *info;
	struct realtek_irq_mux_data *data;
	int ret, i;

	match = of_match_node(realtek_irq_mux_dt_matches, node);
	if (!match)
		return -EINVAL;

	info = match->data;
	if (!info)
		return -EINVAL;

	data = kzalloc(struct_size(data, subset_data, info->cfg_num) , GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->base = syscon_regmap_lookup_by_phandle(node, "syscon");
	if (IS_ERR_OR_NULL(data->base)) {
		return -EINVAL;
	}

	data->info = info;

	spin_lock_init(&data->lock);

	data->domain = irq_domain_add_linear(node, 32, &realtek_mux_irq_domain_ops, data);
	if (!data->domain) {
		kfree(data);
		return -ENOMEM;
	}

	data->subset_data_num = info->cfg_num;
	for (i = 0; i < info->cfg_num; i++) {
		ret = realtek_irq_mux_init_subset(node, data, i);
		WARN(ret, "failed to init subset %d: %d", i, ret);
	}

	return 0;
}

IRQCHIP_PLATFORM_DRIVER_BEGIN(realtek_irq_mux)
IRQCHIP_MATCH("realtek,rtd13xx-iso-irq-mux", realtek_irq_mux_init)
IRQCHIP_MATCH("realtek,rtd13xx-misc-irq-mux", realtek_irq_mux_init)
IRQCHIP_MATCH("realtek,rtd16xxb-iso-irq-mux", realtek_irq_mux_init)
IRQCHIP_MATCH("realtek,rtd16xxb-misc-irq-mux", realtek_irq_mux_init)
IRQCHIP_MATCH("realtek,rtd129x-iso-irq-mux", realtek_irq_mux_init)
IRQCHIP_MATCH("realtek,rtd129x-misc-irq-mux", realtek_irq_mux_init)
IRQCHIP_PLATFORM_DRIVER_END(realtek_irq_mux)
MODULE_DESCRIPTION("Realtek DHC SoC Family interrupt controller");
MODULE_LICENSE("GPL v2");
