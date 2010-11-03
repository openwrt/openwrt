#ifndef __VMMC_MODULE_H__
#define __VMMC_MODULE_H__

#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/tapi/tapi.h>

struct vmmc_module;

struct vmmc_module_ops {
	int (*sync)(struct vmmc_module *);
	int (*enable)(struct vmmc_module *, bool enable);
};

struct vmmc_module
{
	unsigned int id;

	size_t num_pins;
	unsigned int *pins;

	const struct vmmc_module_ops *ops;

	unsigned long flags;
#define VMMC_MODULE_FLAG_PIN_USED(x) (x)
#define VMMC_MODULE_FLAG_MODIFIED 31
	struct mutex lock;
	struct list_head head;

	unsigned int refcount;
};

int vmmc_module_init(struct vmmc_module *module, size_t num_pins,
	const struct vmmc_module_ops *ops);
int vmmc_module_sync(struct vmmc_module *module);

struct vmmc_link {
	struct vmmc_module *modules[2];
	unsigned int pins[2];
};

struct vmmc_endpoint {
	struct tapi_endpoint ep;
	struct vmmc_module *module;
};

void vmmc_link_enable(struct vmmc_link *link);
void vmmc_link_disable(struct vmmc_link *link);
unsigned int vmmc_link_init(struct vmmc_link *link, 
	struct vmmc_module *a, struct vmmc_module *b);
void vmmc_link_put(struct vmmc_link *link);

int vmmc_module_get_pin(struct vmmc_module *module);
void vmmc_module_put_pin(struct vmmc_module *module, unsigned int pin);
void vmmc_module_set_pin_input(struct vmmc_module *module, unsigned int pin,
	struct vmmc_module *input);


static inline struct vmmc_module *tapi_endpoint_to_vmmc_module(struct tapi_endpoint *ep)
{
	return tapi_endpoint_get_data(ep);
}

#endif
