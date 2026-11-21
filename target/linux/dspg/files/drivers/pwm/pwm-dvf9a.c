// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/regmap.h>

#define DVF9A_PWM_CFG2_PRESC_HI	GENMASK(1, 0)
#define DVF9A_PWM_CFG2_MODULO	GENMASK(4, 2)
#define DVF9A_PWM_CFG2_EN	BIT(5)
#define DVF9A_PWM_PRESC_MAX	1024
#define DVF9A_PWM_NUM		2

struct dvf9a_pwm_regs {
	u8 cfg1;
	u8 cfg2;
	u8 cfg3;
	u8 duty;
};

static const struct dvf9a_pwm_regs dvf9a_pwm_regs[DVF9A_PWM_NUM] = {
	{ .cfg1 = 73, .cfg2 = 39, .cfg3 = 74, .duty = 71 },
	{ .cfg1 = 78, .cfg2 = 49, .cfg3 = 79, .duty = 72 },
};

static const u8 dvf9a_pwm_modulo_code[] = {
	[3] = 0, [4] = 1, [5] = 4, [6] = 5, [7] = 6, [8] = 7,
};

struct dvf9a_pwm {
	struct regmap *regmap;
	struct clk *ref;
};

static int dvf9a_pwm_apply(struct pwm_chip *chip, struct pwm_device *pwm,
			   const struct pwm_state *state)
{
	struct dvf9a_pwm *dp = pwmchip_get_drvdata(chip);
	const struct dvf9a_pwm_regs *regs = &dvf9a_pwm_regs[pwm->hwpwm];
	unsigned int shift, modulo, presc, duty;
	u64 cycles, duty_cycles;
	int ret;

	if (state->polarity != PWM_POLARITY_NORMAL)
		return -EINVAL;

	if (!state->enabled || !state->duty_cycle)
		return regmap_write(dp->regmap, regs->cfg2, 0);

	cycles = mul_u64_u64_div_u64(state->period, clk_get_rate(dp->ref),
				     NSEC_PER_SEC);

	for (shift = 8; shift >= 3; shift--)
		if (cycles >> shift)
			break;

	if (shift < 3)
		return -EINVAL;

	modulo = BIT(shift);
	presc = min_t(u64, cycles >> shift, DVF9A_PWM_PRESC_MAX);
	duty_cycles = mul_u64_u64_div_u64(state->duty_cycle,
					  clk_get_rate(dp->ref), NSEC_PER_SEC);
	duty = div_u64(duty_cycles, presc);
	presc--;

	if (!duty)
		return regmap_write(dp->regmap, regs->cfg2, 0);

	if (duty >= modulo)
		duty = 0;

	ret = regmap_write(dp->regmap, regs->cfg1, 0);
	if (ret)
		return ret;

	ret = regmap_write(dp->regmap, regs->cfg3, presc & 0xff);
	if (ret)
		return ret;

	ret = regmap_write(dp->regmap, regs->duty, duty);
	if (ret)
		return ret;

	return regmap_write(dp->regmap, regs->cfg2,
			    FIELD_PREP(DVF9A_PWM_CFG2_PRESC_HI, presc >> 8) |
			    FIELD_PREP(DVF9A_PWM_CFG2_MODULO,
				       dvf9a_pwm_modulo_code[shift]) |
			    DVF9A_PWM_CFG2_EN);
}

static int dvf9a_pwm_get_state(struct pwm_chip *chip, struct pwm_device *pwm,
			       struct pwm_state *state)
{
	struct dvf9a_pwm *dp = pwmchip_get_drvdata(chip);
	const struct dvf9a_pwm_regs *regs = &dvf9a_pwm_regs[pwm->hwpwm];
	unsigned int shift, presc, cfg2, cfg3, duty;
	unsigned long rate;
	int ret;

	ret = regmap_read(dp->regmap, regs->cfg2, &cfg2);
	if (!ret)
		ret = regmap_read(dp->regmap, regs->cfg3, &cfg3);
	if (!ret)
		ret = regmap_read(dp->regmap, regs->duty, &duty);
	if (ret)
		return ret;

	for (shift = 3; shift <= 8; shift++)
		if (dvf9a_pwm_modulo_code[shift] ==
		    FIELD_GET(DVF9A_PWM_CFG2_MODULO, cfg2))
			break;

	if (shift > 8)
		return -EINVAL;

	rate = clk_get_rate(dp->ref);
	presc = (FIELD_GET(DVF9A_PWM_CFG2_PRESC_HI, cfg2) << 8 | cfg3) + 1;

	state->polarity = PWM_POLARITY_NORMAL;
	state->enabled = cfg2 & DVF9A_PWM_CFG2_EN;
	state->period = DIV_ROUND_UP_ULL((u64)presc * NSEC_PER_SEC << shift,
					 rate);
	state->duty_cycle = duty ? DIV_ROUND_UP_ULL(state->period * duty,
						    BIT(shift)) :
				   state->period;

	return 0;
}

static const struct pwm_ops dvf9a_pwm_ops = {
	.apply = dvf9a_pwm_apply,
	.get_state = dvf9a_pwm_get_state,
};

static int dvf9a_pwm_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct pwm_chip *chip;
	struct dvf9a_pwm *dp;

	chip = devm_pwmchip_alloc(dev, DVF9A_PWM_NUM, sizeof(*dp));
	if (IS_ERR(chip))
		return PTR_ERR(chip);

	dp = pwmchip_get_drvdata(chip);

	dp->regmap = dev_get_regmap(dev->parent, NULL);
	if (!dp->regmap)
		return dev_err_probe(dev, -ENODEV, "no DVF9A regmap\n");

	dp->ref = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(dp->ref))
		return dev_err_probe(dev, PTR_ERR(dp->ref), "no ref clock\n");

	if (!clk_get_rate(dp->ref))
		return dev_err_probe(dev, -EINVAL, "ref clock rate is zero\n");

	chip->ops = &dvf9a_pwm_ops;

	return devm_pwmchip_add(dev, chip);
}

static const struct of_device_id dvf9a_pwm_of_match[] = {
	{ .compatible = "dspg,dvf9a-pwm" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf9a_pwm_of_match);

static struct platform_driver dvf9a_pwm_driver = {
	.probe = dvf9a_pwm_probe,
	.driver = {
		.name = "dvf9a-pwm",
		.of_match_table = dvf9a_pwm_of_match,
	},
};
module_platform_driver(dvf9a_pwm_driver);

MODULE_DESCRIPTION("DSPG DVF9A PWM driver");
MODULE_LICENSE("GPL");
