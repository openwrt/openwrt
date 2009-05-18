
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/io.h>

#include <linux/glamo-gpio.h>

#include "glamo-core.h"
#include "glamo-regs.h"

void glamo_gpio_setpin(struct glamo_core *glamo, unsigned int pin,
		       unsigned int value)
{
	unsigned int reg = REG_OF_GPIO(pin);
	u_int16_t tmp;

	spin_lock(&glamo->lock);
	tmp = readw(glamo->base + reg);
	if (value)
		tmp |= OUTPUT_BIT(pin);
	else
		tmp &= ~OUTPUT_BIT(pin);
	writew(tmp, glamo->base + reg);
	spin_unlock(&glamo->lock);
}
EXPORT_SYMBOL(glamo_gpio_setpin);

int glamo_gpio_getpin(struct glamo_core *glamo, unsigned int pin)
{
	return readw(REG_OF_GPIO(pin)) & INPUT_BIT(pin) ? 1 : 0;
}
EXPORT_SYMBOL(glamo_gpio_getpin);

void glamo_gpio_cfgpin(struct glamo_core *glamo, unsigned int pinfunc)
{
	unsigned int reg = REG_OF_GPIO(pinfunc);
	u_int16_t tmp;

	spin_lock(&glamo->lock);
	tmp = readw(glamo->base + reg);

	if ((pinfunc & 0x00f0) == GLAMO_GPIO_F_FUNC) {
		/* pin is a function pin: clear gpio bit */
		tmp &= ~FUNC_BIT(pinfunc);
	} else {
		/* pin is gpio: set gpio bit */
		tmp |= FUNC_BIT(pinfunc);

		if (pinfunc & GLAMO_GPIO_F_IN) {
			/* gpio input: set bit to disable output mode */
			tmp |= GPIO_OUT_BIT(pinfunc);
		} else if (pinfunc & GLAMO_GPIO_F_OUT) {
			/* gpio output: clear bit to enable output mode */
			tmp &= ~GPIO_OUT_BIT(pinfunc);
		}
	}
	writew(tmp, glamo->base + reg);
	spin_unlock(&glamo->lock);
}
EXPORT_SYMBOL(glamo_gpio_cfgpin);

