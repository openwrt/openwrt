#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mutex.h>

#include <asm/bitops.h>

#include "vmmc-module.h"

int vmmc_module_init(struct vmmc_module *module, size_t num_pins,
	const struct vmmc_module_ops *ops)
{
	module->pins = kcalloc(num_pins, sizeof(*module->pins), GFP_KERNEL);

	if (!module->pins)
		return -ENOMEM;

	module->num_pins = num_pins;
	module->ops = ops;

	mutex_init(&module->lock);
	module->refcount = 0;

	return 0;
}

int vmmc_module_sync(struct vmmc_module *module)
{
	if (!test_and_clear_bit(VMMC_MODULE_FLAG_MODIFIED, &module->flags))
		return 0;

	return module->ops->sync(module);
}

int vmmc_module_get_pin(struct vmmc_module *module)
{
	size_t i = 0;
	int ret = 0;

	for (i = 0; i < module->num_pins; ++i) {
		if (!test_and_set_bit(VMMC_MODULE_FLAG_PIN_USED(i), &module->flags))
			break;
	}
	if (i == module->num_pins)
		ret = -EBUSY;
	else
		ret = i;

	return ret;
}

void vmmc_module_put_pin(struct vmmc_module *module, unsigned int pin)
{
	module->pins[pin] = 0;
	clear_bit(VMMC_MODULE_FLAG_PIN_USED(pin), &module->flags);
}

void vmmc_module_set_pin_input(struct vmmc_module *module, unsigned int pin,
	struct vmmc_module *input)
{
	if (input)
		module->pins[pin] = input->id;
	else
		module->pins[pin] = 0;

	set_bit(VMMC_MODULE_FLAG_MODIFIED, &module->flags);
}

static void vmmc_module_enable(struct vmmc_module *module)
{
	mutex_lock(&module->lock);

	if (++module->refcount == 1)
		module->ops->enable(module, true);

	mutex_unlock(&module->lock);
}

static void vmmc_module_disable(struct vmmc_module *module)
{
	mutex_lock(&module->lock);

	if (module->refcount <= 0)
		printk(KERN_ERR "vmmc module: unbalanced disable\n");
	else if (--module->refcount == 0)
		module->ops->enable(module, false);

	mutex_unlock(&module->lock);
}


unsigned int vmmc_link_init(struct vmmc_link *link,
	struct vmmc_module *a, struct vmmc_module *b)
{
	link->pins[0] = vmmc_module_get_pin(a);
	link->pins[1] = vmmc_module_get_pin(b);
	link->modules[0] = a;
	link->modules[1] = b;

	return 0;
}

void vmmc_link_put(struct vmmc_link *link)
{
	vmmc_link_disable(link);
	vmmc_module_sync(link->modules[0]);
	vmmc_module_sync(link->modules[1]);
	vmmc_module_put_pin(link->modules[0], link->pins[0]);
	vmmc_module_put_pin(link->modules[1], link->pins[1]);
}

void vmmc_link_enable(struct vmmc_link *link)
{
	vmmc_module_set_pin_input(link->modules[0], link->pins[0],
		link->modules[1]);
	vmmc_module_set_pin_input(link->modules[1], link->pins[1],
		link->modules[0]);

	vmmc_module_enable(link->modules[0]);
	vmmc_module_enable(link->modules[1]);
}

void vmmc_link_disable(struct vmmc_link *link)
{
	vmmc_module_set_pin_input(link->modules[0], link->pins[0], NULL);
	vmmc_module_set_pin_input(link->modules[1], link->pins[1], NULL);

	vmmc_module_disable(link->modules[0]);
	vmmc_module_disable(link->modules[1]);
}
