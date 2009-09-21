/* Smedia Glamo 336x/337x gpio driver
 *
 * (C) 2009 Lars-Peter Clausen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/platform_device.h>

#include <linux/gpio.h>
#include <linux/mfd/glamo.h>

#include "glamo-core.h"
#include "glamo-regs.h"

#define GLAMO_NR_GPIO 21
#define GLAMO_NR_GPIO_REGS DIV_ROUND_UP(GLAMO_NR_GPIO, 4)

#define GLAMO_REG_GPIO(x) (((x) * 2) + GLAMO_REG_GPIO_GEN1)

struct glamo_gpio {
	struct glamo_core *glamo;
	struct gpio_chip chip;
	uint16_t saved_regs[GLAMO_NR_GPIO_REGS];
};

#define REG_OF_GPIO(gpio)   (GLAMO_REG_GPIO(gpio >> 2))
#define NUM_OF_GPIO(gpio)	(gpio & 0x3)
#define DIRECTION_BIT(gpio)	(1 << (NUM_OF_GPIO(gpio) + 0))
#define OUTPUT_BIT(gpio)	(1 << (NUM_OF_GPIO(gpio) + 4))
#define INPUT_BIT(gpio)		(1 << (NUM_OF_GPIO(gpio) + 8))
#define FUNC_BIT(gpio)		(1 << (NUM_OF_GPIO(gpio) + 12))


static inline struct glamo_core *chip_to_glamo(struct gpio_chip *chip)
{
	return container_of(chip, struct glamo_gpio, chip)->glamo;
}

static void glamo_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	struct glamo_core *glamo = chip_to_glamo(chip);
	unsigned int reg = REG_OF_GPIO(offset);
	u_int16_t tmp;

	spin_lock(&glamo->lock);
	tmp = readw(glamo->base + reg);
	if (value)
		tmp |= OUTPUT_BIT(offset);
	else
		tmp &= ~OUTPUT_BIT(offset);
	writew(tmp, glamo->base + reg);
	spin_unlock(&glamo->lock);
}

static int glamo_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	struct glamo_core *glamo = chip_to_glamo(chip);
	return readw(glamo->base + REG_OF_GPIO(offset)) & INPUT_BIT(offset) ? 1 : 0;
}

static int glamo_gpio_request(struct gpio_chip *chip, unsigned offset)
{
	struct glamo_core *glamo = chip_to_glamo(chip);
	unsigned int reg = REG_OF_GPIO(offset);
	u_int16_t tmp;

	spin_lock(&glamo->lock);
	tmp = readw(glamo->base + reg);
	if ((tmp & FUNC_BIT(offset)) == 0) {
		tmp |= FUNC_BIT(offset);
		writew(tmp, glamo->base + reg);
	}
	spin_unlock(&glamo->lock);

	return 0;
}

static void glamo_gpio_free(struct gpio_chip *chip, unsigned offset)
{
	struct glamo_core *glamo = chip_to_glamo(chip);
	unsigned int reg = REG_OF_GPIO(offset);
	u_int16_t tmp;

	spin_lock(&glamo->lock);
	tmp = readw(glamo->base + reg);
	if ((tmp & FUNC_BIT(offset)) == 1) {
		tmp &= ~FUNC_BIT(offset);
		writew(tmp, glamo->base + reg);
	}
	spin_unlock(&glamo->lock);
}

static int glamo_gpio_direction_output(struct gpio_chip *chip, unsigned offset,
						int value)
{
	struct glamo_core *glamo = chip_to_glamo(chip);
	unsigned int reg = REG_OF_GPIO(offset);
	u_int16_t tmp;

	spin_lock(&glamo->lock);
	tmp = readw(glamo->base + reg);
    tmp &= ~DIRECTION_BIT(offset);

	if (value)
		tmp |= OUTPUT_BIT(offset);
	else
		tmp &= ~OUTPUT_BIT(offset);

	writew(tmp, glamo->base + reg);
	spin_unlock(&glamo->lock);

	return 0;
}

static int glamo_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	struct glamo_core *glamo = chip_to_glamo(chip);
	unsigned int reg = REG_OF_GPIO(offset);
	u_int16_t tmp;

	spin_lock(&glamo->lock);
	tmp = readw(glamo->base + reg);
	if ((tmp & DIRECTION_BIT(offset)) == 0) {
		tmp |= DIRECTION_BIT(offset);
		writew(tmp, glamo->base + reg);
	}
	spin_unlock(&glamo->lock);

	return 0;
}

static const struct __devinit gpio_chip glamo_gpio_chip = {
	.label				= "glamo",
	.request			= glamo_gpio_request,
	.free				= glamo_gpio_free,
	.direction_input	= glamo_gpio_direction_input,
	.get				= glamo_gpio_get,
	.direction_output	= glamo_gpio_direction_output,
	.set				= glamo_gpio_set,
	.base				= -1,
	.ngpio				= GLAMO_NR_GPIO,
	.can_sleep			= 0,
	.owner             = THIS_MODULE,
};

static int __devinit glamo_gpio_probe(struct platform_device *pdev)
{
	struct glamo_platform_data *pdata = pdev->dev.parent->platform_data;
	struct glamo_gpio *glamo_gpio;
	int ret;

	glamo_gpio = kzalloc(sizeof(struct glamo_gpio), GFP_KERNEL);
	if (!glamo_gpio)
		return -ENOMEM;

	glamo_gpio->glamo = dev_get_drvdata(pdev->dev.parent);
	glamo_gpio->chip = glamo_gpio_chip;
	glamo_gpio->chip.dev = &pdev->dev;
	if (pdata)
		glamo_gpio->chip.base = pdata->gpio_base;

	ret = gpiochip_add(&glamo_gpio->chip);

	if (ret) {
		dev_err(&pdev->dev, "Could not register gpio chip: %d\n", ret);
		goto err;
	}

	platform_set_drvdata(pdev, glamo_gpio);

    dev_info(&pdev->dev, "Glamo GPIO registered\n");

    if (pdata->registered)
        pdata->registered(&pdev->dev);

	return 0;
err:
	kfree(glamo_gpio);
	return ret;
}

static int __devexit glamo_gpio_remove(struct platform_device *pdev)
{
	struct glamo_gpio *glamo_gpio = platform_get_drvdata(pdev);
	int ret;

	ret = gpiochip_remove(&glamo_gpio->chip);
	if (!ret)
		goto done;

	platform_set_drvdata(pdev, NULL);
	kfree(glamo_gpio);

done:
	return ret;
}

#ifdef CONFIG_PM

static int glamo_gpio_suspend(struct device *dev)
{
	struct glamo_gpio *glamo_gpio = dev_get_drvdata(dev);
	struct glamo_core *glamo = glamo_gpio->glamo;
	uint16_t *saved_regs = glamo_gpio->saved_regs;
	int i;

	spin_lock(&glamo->lock);
	for (i = 0; i < GLAMO_NR_GPIO / 4; ++i)
		saved_regs[i] = readw(glamo->base + GLAMO_REG_GPIO(i));
	spin_unlock(&glamo->lock);

	return 0;
}

static int glamo_gpio_resume(struct device *dev)
{
	struct glamo_gpio *glamo_gpio = dev_get_drvdata(dev);
	struct glamo_core *glamo = glamo_gpio->glamo;
	uint16_t *saved_regs = glamo_gpio->saved_regs;
	int i;

	spin_lock(&glamo->lock);
	for (i = 0; i < GLAMO_NR_GPIO_REGS; ++i)
		writew(saved_regs[i], glamo->base + GLAMO_REG_GPIO(i));
	spin_unlock(&glamo->lock);
	return 0;
}

static const struct dev_pm_ops glamo_pm_ops = {
	.suspend = glamo_gpio_suspend,
	.resume  = glamo_gpio_resume,
	.freeze  = glamo_gpio_suspend,
	.thaw    = glamo_gpio_resume,
};

#define GLAMO_GPIO_PM_OPS (&glamo_pm_ops)

#else
#define GLAMO_GPIO_PM_OPS NULL
#endif

static struct platform_driver glamo_gpio_driver = {
	.driver = {
		.name  = "glamo-gpio",
		.owner = THIS_MODULE,
		.pm    = GLAMO_GPIO_PM_OPS,
	},
	.probe = glamo_gpio_probe,
	.remove = __devexit_p(glamo_gpio_remove),
};

static int __devinit glamo_gpio_init(void)
{
	return platform_driver_register(&glamo_gpio_driver);
}

static void __exit glamo_gpio_exit(void)
{
	platform_driver_unregister(&glamo_gpio_driver);
}

module_init(glamo_gpio_init);
module_exit(glamo_gpio_exit);

MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("GPIO interface for the Glamo multimedia device");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:glamo-gpio");
