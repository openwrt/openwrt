#ifndef __VMMC_H__

#include <linux/list.h>
#include <linux/tapi/tapi.h>

#include "mps.h"
#include "vmmc-module.h"

struct vmmc
{
	struct mps *mps;
	struct device *dev;

	struct vmmc_port *ports;

	struct completion firmware_loaded_completion;
	struct completion cmd_completion;

	struct mps_mailbox mbox_cmd;
	struct mps_mailbox mbox_data;
	struct mps_fifo fifo_event;

	int irq_fw_loaded;
	int irq_mbox_cmd;
	int irq_mbox_data;
	int irq_event_fifo;
	int irq_cmd_error;

	unsigned int num_coders;
	struct vmmc_coder *coder;
	unsigned long coder_used;

	struct list_head modules;

	struct tapi_device tdev;

	struct sk_buff_head recv_queue;
	struct sk_buff_head send_queue;
};

static inline struct vmmc *tdev_to_vmmc(struct tapi_device *tdev)
{
	return container_of(tdev, struct vmmc, tdev);
}

static inline void vmmc_register_module(struct vmmc *vmmc,
	struct vmmc_module *module)
{
	list_add_tail(&module->head, &vmmc->modules);
}

static inline void vmmc_unregister_module(struct vmmc *vmmc,
	struct vmmc_module *module)
{
	list_del(&module->head);
}

int vmmc_command_write(struct vmmc *vmmc, uint32_t cmd,
	const uint32_t *data);
int vmmc_command_read(struct vmmc *vmmc, uint32_t cmd, uint32_t *result);

struct vmmc_coder *vmmc_coder_get(struct vmmc *);
void vmmc_coder_put(struct vmmc *, struct vmmc_coder *);
void vmmc_init_coders(struct vmmc *);

void vmmc_send_paket(struct vmmc *vmmc, unsigned int chan, struct sk_buff *skb);

#endif
