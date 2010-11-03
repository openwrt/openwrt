#ifndef __MPS_H__
#define __MPS_H__

#include <linux/platform_device.h>
#include <linux/completion.h>

struct mps_fifo_config
{
	size_t head_addr;
	size_t tail_addr;
	size_t base_addr;
	size_t size_addr;

	size_t base;
	size_t size;
};

struct mps_fifo {
	void __iomem *base;
	void __iomem *head_addr;
	void __iomem *tail_addr;
	uint32_t size;
};

struct mps_mailbox {
	struct mps_fifo upstream;
	struct mps_fifo downstream;
};

enum mps_boot_config
{
	MPS_BOOT_LEGACY = 1,
	MPS_BOOT_ENCRYPTED = 2,
};

struct mps {
	struct resource *res;
	void __iomem *base;

	struct resource *mbox_res;
	void __iomem *mbox_base;

	struct resource *cp1_res;
	void __iomem *cp1_base;

	struct device *dev;

	int irq_ad0;
	int irq_ad1;
	int irq_base;
};

void mps_configure_fifo(struct mps *mps, struct mps_fifo *fifo,
	const struct mps_fifo_config *config);

void mps_configure_mailbox(struct mps *mps, struct mps_mailbox *mbox,
	const struct mps_fifo_config *upstream_config,
	const struct mps_fifo_config *downstream_config);

void mps_load_firmware(struct mps *mps, const void *data, size_t size,
	enum mps_boot_config config);

static inline struct mps *device_to_mps(struct device *dev)
{
	return (struct mps *)dev_get_drvdata(dev);
}

/* fifo */
void mps_fifo_init(struct mps_fifo *fifo, void __iomem *data_addr,
	void __iomem *head_addr, void __iomem *tail_addr, uint32_t size);
void mps_fifo_in(struct mps_fifo *fifo, const uint32_t *from, size_t len);
void mps_fifo_out(struct mps_fifo *fifo, uint32_t *to, size_t len);
uint32_t mps_fifo_peek(struct mps_fifo *fifo);
void mps_fifo_reset(struct mps_fifo *fifo);
size_t mps_fifo_len(struct mps_fifo *fifo);

/* Mailbox */
int mps_mailbox_init(struct mps_mailbox *mbox, const char *name, int irq);
int mps_mailbox_command_read(struct mps_mailbox *mbox, uint32_t cmd,
	uint32_t *result);
int mps_mailbox_command_write(struct mps_mailbox *mbox, uint32_t cmd,
	const uint32_t *data);


#endif
