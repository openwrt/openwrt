#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

#include <linux/firmware.h>

#include <linux/delay.h>

#include <ifxmips_gptu.h>

#include <linux/tapi/tapi.h>

#include "vmmc.h"
#include "mps.h"
#include "mps-msg.h"
#include "mps-irq.h"
#include "vmmc-cmds.h"

#include "vmmc-port.h"
#include "vmmc-stream.h"
#include "vmmc-link.h"
#include "vmmc-coder.h"

struct vmmc_capabilities {
	uint8_t num_alm;
	uint8_t num_pcm;
	uint8_t num_signal;
	uint8_t num_coder;
	uint8_t num_agc;
	uint8_t num_eq;
	uint8_t num_nlec;
	uint8_t num_wlec;
	uint8_t num_nwlec;
	uint8_t num_wwlec;
	uint8_t num_tone_generators;
	uint8_t num_dtmf_generators;
	uint8_t num_caller_id_senders;
	uint8_t num_caller_id_recivers;
};

#define MPS_FIRMWARE_MAGIC 0xcc123456

struct vmmc_firmware_head {
	uint32_t crc;
	uint32_t crc_start_addr;
	uint32_t crc_end_addr;
	uint32_t version;
	uint32_t encrypted;
	uint32_t magic;
	uint32_t mem;
} __packed;

#define VMMC_FIFO_UPSTREAM_CMD_BASE_ADDR	0x00
#define VMMC_FIFO_UPSTREAM_CMD_SIZE_ADDR	0x04
#define VMMC_FIFO_DOWNSTREAM_CMD_BASE_ADDR	0x08
#define VMMC_FIFO_DOWNSTREAM_CMD_SIZE_ADDR	0x0c
#define VMMC_FIFO_UPSTREAM_DATA_BASE_ADDR	0x10
#define VMMC_FIFO_UPSTREAM_DATA_SIZE_ADDR	0x14
#define VMMC_FIFO_DOWNSTREAM_DATA_BASE_ADDR	0x18
#define VMMC_FIFO_DOWNSTREAM_DATA_SIZE_ADDR	0x1c
#define VMMC_FIFO_UPSTREAM_CMD_TAIL_ADDR	0x20
#define VMMC_FIFO_UPSTREAM_CMD_HEAD_ADDR	0x24
#define VMMC_FIFO_DOWNSTREAM_CMD_TAIL_ADDR	0x28
#define VMMC_FIFO_DOWNSTREAM_CMD_HEAD_ADDR	0x2c
#define VMMC_FIFO_UPSTREAM_DATA_TAIL_ADDR	0x30
#define VMMC_FIFO_UPSTREAM_DATA_HEAD_ADDR	0x34
#define VMMC_FIFO_DOWNSTREAM_DATA_TAIL_ADDR	0x38
#define VMMC_FIFO_DOWNSTREAM_DATA_HEAD_ADDR	0x3c

#define VMMC_FIFO_EVENT_BASE_ADDR		0x180
#define VMMC_FIFO_EVENT_SIZE_ADDR		0x184
#define VMMC_FIFO_EVENT_TAIL_ADDR		0x188
#define VMMC_FIFO_EVENT_HEAD_ADDR		0x18c


/* Calculates the base of the fifo behind the given fifo */
#define VMMC_NEXT_FIFO_BASE0(_fifo) \
	(VMMC_FIFO_ ## _fifo ## _BASE + VMMC_FIFO_ ## _fifo ## _SIZE)
#define VMMC_NEXT_FIFO_BASE1(_fifo) \
	(VMMC_FIFO_ ## _fifo ## _BASE + VMMC_FIFO_ ## _fifo ## _SIZE)
#define VMMC_NEXT_FIFO_BASE2(_fifo) \
	(VMMC_FIFO_ ## _fifo ## _BASE + VMMC_FIFO_ ## _fifo ## _SIZE)

/* Fifo sizes */
#define VMMC_FIFO_UPSTREAM_CMD_SIZE	64
#define VMMC_FIFO_DOWNSTREAM_CMD_SIZE	64
#define VMMC_FIFO_UPSTREAM_DATA_SIZE	64
#define VMMC_FIFO_DOWNSTREAM_DATA_SIZE	128
#define VMMC_FIFO_EVENT_SIZE		64

/* Fifo addresses */
#define VMMC_FIFO_UPSTREAM_CMD_BASE	0x40
#define VMMC_FIFO_DOWNSTREAM_CMD_BASE	VMMC_NEXT_FIFO_BASE0(UPSTREAM_CMD)
#define VMMC_FIFO_UPSTREAM_DATA_BASE	VMMC_NEXT_FIFO_BASE1(DOWNSTREAM_CMD)
#define VMMC_FIFO_DOWNSTREAM_DATA_BASE	VMMC_NEXT_FIFO_BASE2(UPSTREAM_DATA)
#define VMMC_FIFO_EVENT_BASE		0x190

#define VMMC_DECLARE_FIFO_CONFIG(_name, _fifo) \
static const struct mps_fifo_config _name = { \
	.tail_addr = VMMC_FIFO_ ## _fifo ## _TAIL_ADDR, \
	.head_addr = VMMC_FIFO_ ## _fifo ## _HEAD_ADDR, \
	.base_addr = VMMC_FIFO_ ## _fifo ## _BASE_ADDR, \
	.size_addr = VMMC_FIFO_ ## _fifo ## _SIZE_ADDR, \
	.base = VMMC_FIFO_ ## _fifo ## _BASE, \
	.size = VMMC_FIFO_ ## _fifo ## _SIZE, \
}

VMMC_DECLARE_FIFO_CONFIG(vmmc_fifo_config_upstream_cmd, UPSTREAM_CMD);
VMMC_DECLARE_FIFO_CONFIG(vmmc_fifo_config_downstream_cmd, DOWNSTREAM_CMD);
VMMC_DECLARE_FIFO_CONFIG(vmmc_fifo_config_upstream_data, UPSTREAM_DATA);
VMMC_DECLARE_FIFO_CONFIG(vmmc_fifo_config_downstream_data, DOWNSTREAM_DATA);
VMMC_DECLARE_FIFO_CONFIG(vmmc_fifo_config_event, EVENT);

static void vmmc_setup_fifos(struct vmmc *vmmc)
{
    mps_configure_mailbox(vmmc->mps, &vmmc->mbox_cmd,
		&vmmc_fifo_config_upstream_cmd, &vmmc_fifo_config_downstream_cmd);
    mps_configure_mailbox(vmmc->mps, &vmmc->mbox_data,
		&vmmc_fifo_config_upstream_data, &vmmc_fifo_config_downstream_data);

    mps_configure_fifo(vmmc->mps, &vmmc->fifo_event, &vmmc_fifo_config_event);
}

static uint32_t cram_data[] = {
0x00200000, 0x00008e59, 0x165235cd, 0x17e2f141, 0xe3eef301, 0x0a431281,
0x04fdf20d, 0x7fe363d5, 0xfd4b7333, 0x7ffffd44, 0xfcf80298, 0xfecd00c9,
0xff900042, 0xfff70003, 0x000923b7, 0xe92a354d, 0xc8981f44, 0x9c0f1257,
0x26aacf33, 0x27db9836, 0x10586f5b, 0x9c167d2d, 0x94b679a7, 0x8c227660,
0x83fa7491, 0x7ce0826a, 0x7ff87ff4, 0x296b4e22, 0x76e67fff, 0x008ffc04,
0x02cbfb36, 0x026afeba, 0x009effc3, 0x0013fffd, 0x23b7e92a, 0x354dc898,
0x1f449c0f, 0x125726aa, 0xcf3327db, 0x98361058, 0x74bc93d6, 0x7ebc8f61,
0x7d068986, 0x7b46833b, 0x7a3b7f00, 0x287a47b1, 0x05800367, 0x20ae2715,
0x0fb5da12, 0x1935f53b, 0x01230240, 0xfc717f00, 0x2000d346,
};

static void vmmc_push_data_paket(struct vmmc *vmmc, int type, unsigned int chan,
	void __iomem *addr, size_t len)
{
	uint32_t data[3];

	data[0] = VMMC_VOICE_DATA(type, chan, 8);
	data[1] = CPHYSADDR(addr);
	data[2] = len;

	mps_fifo_in(&vmmc->mbox_data.downstream, data, 3);
}

static struct sk_buff *vmmc_alloc_data_paket(struct vmmc *vmmc)
{
	struct sk_buff *skb;

	skb = alloc_skb(512, GFP_KERNEL);

	skb_queue_tail(&vmmc->recv_queue, skb);

	return skb;
}

static void vmmc_provide_paket(struct vmmc *vmmc)
{
	struct sk_buff *skb = vmmc_alloc_data_paket(vmmc);
	vmmc_push_data_paket(vmmc, CMD_ADDRESS_PACKET, 0, skb->data, skb->len);
}

static void vmmc_recv_paket(struct vmmc *vmmc, unsigned int chan, void __iomem *addr, size_t len)
{
	struct sk_buff *skb;
	struct sk_buff *tmp;

	skb_queue_walk_safe(&vmmc->recv_queue, skb, tmp) {
		if (skb->data == addr)
			break;
	}

	if (skb == (struct sk_buff *)(&vmmc->recv_queue)) {
		printk("AHHHH\n");
		return;
	}

	dma_cache_inv((u32)addr, len);
	skb_unlink(skb, &vmmc->recv_queue);

	if (!vmmc->coder[chan].stream) {
		kfree_skb(skb);
		return;
	}

	skb_put(skb, len);
	tapi_stream_recv(&vmmc->tdev, vmmc->coder[chan].stream, skb);
}

void vmmc_send_paket(struct vmmc *vmmc, unsigned int chan, struct sk_buff *skb)
{
	skb_queue_tail(&vmmc->send_queue, skb);

	dma_cache_wback((u32)skb->data, skb->len);
	vmmc_push_data_paket(vmmc, CMD_RTP_VOICE_DATA_PACKET, chan, skb->data,
		skb->len);
}

static void vmmc_free_paket(struct vmmc *vmmc, void __iomem *addr, size_t len)
{
	struct sk_buff *skb;
	struct sk_buff *tmp;

	skb_queue_walk_safe(&vmmc->send_queue, skb, tmp) {
		if (skb->data == addr)
			break;
	}

	if (skb == (struct sk_buff *)(&vmmc->send_queue)) {
		printk("AHHHH\n");
	} else {
		skb_unlink(skb, &vmmc->send_queue);
		kfree_skb(skb);
	}
}


static void vmmc_write_cram_data(struct vmmc *vmmc, unsigned int id,
	uint32_t *data, size_t length)
{
	size_t transfer_length;
	size_t offset = 0;
	uint32_t cmd;

	length *= 4;
	offset = 0x5;

	while (length) {
		transfer_length = length > 56 ? 56 : length;
		cmd = VMMC_CMD_ALM_COEF(id, offset, transfer_length);
		vmmc_command_write(vmmc, cmd, data);

		data += transfer_length >> 2;
		offset += transfer_length >> 1;
		length -= transfer_length;
	}
}

int vmmc_command_read(struct vmmc *vmmc, uint32_t cmd, uint32_t *result)
{
	struct mps_mailbox *mbox = &vmmc->mbox_cmd;

	INIT_COMPLETION(vmmc->cmd_completion);

	mps_fifo_in(&mbox->downstream, &cmd, 1);

	wait_for_completion(&vmmc->cmd_completion);

	mps_fifo_out(&mbox->upstream, result, 1);
	mps_fifo_out(&mbox->upstream, result, (*result & 0xff) / 4);

	return 0;
}

int vmmc_command_write(struct vmmc *vmmc, uint32_t cmd,
	const uint32_t *data)
{
	struct mps_mailbox *mbox = &vmmc->mbox_cmd;
/*	int i;

	printk("cmd: %x\n", cmd);
	for (i = 0; i < DIV_ROUND_UP((cmd & 0xff), 4); ++i) {
		printk("data[%d] = %x\n", i, data[i]);
	}
*/
	while (mps_fifo_len(&mbox->downstream) < (cmd & 0xff) + 4)
		mdelay(100);

	mps_fifo_in(&mbox->downstream, &cmd, 1);
	mps_fifo_in(&mbox->downstream, data, DIV_ROUND_UP((cmd & 0xff), 4));

	mdelay(100);

	return 0;
}

static int vmmc_modules_sync(struct tapi_device *tapi)
{
	struct vmmc *vmmc = tdev_to_vmmc(tapi);
	struct vmmc_module *module;

	list_for_each_entry(module, &vmmc->modules, head)
		vmmc_module_sync(module);

	return 0;
}

static const struct tapi_ops vmmc_tapi_ops = {
	.send_dtmf_event = vmmc_port_send_dtmf_event,
	.ring = vmmc_port_ring,

	.sync = vmmc_modules_sync,

	.stream_alloc = vmmc_stream_alloc,
	.stream_free = vmmc_stream_free,
	.stream_send = vmmc_stream_send,

	.link_alloc = vmmc_tapi_link_alloc,
	.link_free = vmmc_tapi_link_free,
	.link_enable = vmmc_tapi_link_enable,
	.link_disable = vmmc_tapi_link_disable,
};

static void setup_alm(struct vmmc *vmmc)
{
	int i;

	vmmc->tdev.ports = kcalloc(2, sizeof(*vmmc->tdev.ports), GFP_KERNEL);
	vmmc->ports = kcalloc(2, sizeof(*vmmc->ports), GFP_KERNEL);

	for (i = 0; i < 2; ++i)
		vmmc_port_init(vmmc, &vmmc->ports[i], &vmmc->tdev.ports[i], i);

	skb_queue_head_init(&vmmc->send_queue);
	skb_queue_head_init(&vmmc->recv_queue);

	for (i = 0; i < 10; ++i)
		vmmc_provide_paket(vmmc);

	vmmc->tdev.num_ports = 2;
	vmmc->tdev.ops = &vmmc_tapi_ops;
	tapi_device_register(&vmmc->tdev, "vmmc", vmmc->dev);
}

static void vmmc_init_timer(struct vmmc *vmmc)
{
	unsigned int timer;
	unsigned int timer_flags;
	int ret;
	unsigned long loops, count;

	timer = TIMER1B;

	timer_flags =
	  TIMER_FLAG_16BIT | TIMER_FLAG_COUNTER | TIMER_FLAG_CYCLIC |
	  TIMER_FLAG_DOWN | TIMER_FLAG_FALL_EDGE | TIMER_FLAG_SYNC |
	  TIMER_FLAG_CALLBACK_IN_IRQ;
	ret = ifxmips_request_timer (timer, timer_flags, 1, 0, 0);
	if (ret < 0) {
		printk("FAILED TO INIT TIMER\n");
		return;
	}
	ret = ifxmips_start_timer (timer, 0);
	if (ret < 0) {
		printk("FAILED TO START TIMER\n");
		return;
	}
	do
	{
		loops++;
		ifxmips_get_count_value(timer, &count);
	} while (count);

	*((volatile uint32_t *) (KSEG1 + 0x1e100a00 + 0x0014)) = 0x000005c5;
}

static void vmmc_free_timer(struct vmmc *vmmc)
{
	ifxmips_free_timer(TIMER1B);
}

static void vmmc_get_capabilities(struct vmmc *vmmc)
{
	uint32_t data[10];
	uint8_t len;

	vmmc_command_read(vmmc,
		MPS_MSG_CMD_EOP_SYSTEM(SYS_CAP_ECMD, sizeof(uint32_t)), data);

	len = ((data[0] >> 16) & 0xff) - sizeof(uint32_t);

	if (len > sizeof(data))
		len = sizeof(data);

	vmmc_command_read(vmmc,
		MPS_MSG_CMD_EOP_SYSTEM(SYS_CAP_ECMD, len), data);

	len /= 4;

/*	for (;len > 0; --len) {
		printk("fw cap(%d): %.2x\n", 10-len, data[10-len]);
	}
*/
	setup_alm(vmmc);
}

static void vmmc_get_firmware_version(struct vmmc *vmmc)
{
	uint32_t data[1];

	vmmc_command_read(vmmc, MPS_CMD_GET_VERSION, data);

	printk("firmware version: %x\n", *data);

	vmmc_get_capabilities(vmmc);
}

static irqreturn_t vmmc_firmware_loaded_irq(int irq, void *devid)
{
	struct vmmc *vmmc = devid;
	complete(&vmmc->firmware_loaded_completion);
	printk("Firmware loaded irq\n");

	return IRQ_HANDLED;
}

static irqreturn_t vmmc_cmd_error_irq(int irq, void *devid)
{
/*	struct vmmc *vmmc = devid;*/

	printk("cmd error!!!!\n");

	return IRQ_HANDLED;
}

static irqreturn_t vmmc_recv_ov_irq(int irq, void *devid)
{
	struct vmmc *vmmc = devid;
	uint32_t data[2] = {
		VMMC_CMD_SERR_ACK(0),
		VMMC_CMD_SERR_ACK_DATA1(1)
	};
	uint32_t voice_data[64];

	return IRQ_HANDLED;

	mps_fifo_in(&vmmc->mbox_cmd.downstream, data, 2);
	mps_fifo_out(&vmmc->mbox_data.upstream, voice_data, 15);

	printk("recv overflow: %x\n", voice_data[0]);

	return IRQ_HANDLED;
}

static irqreturn_t vmmc_event_fifo_irq(int irq, void *devid)
{
	struct vmmc *vmmc = devid;
	uint32_t event, event_id;
	uint32_t data = 0;
	unsigned int chan;

	mps_fifo_out(&vmmc->fifo_event, &event, 1);

	event_id = event & VMMC_EVENT_ID_MASK;
	chan = VMMC_MSG_GET_CHAN(event);

	if (event & 0xff)
		mps_fifo_out(&vmmc->fifo_event, &data, 1);

	switch (event_id) {
	case VMMC_EVENT_HOOK_ID:
		vmmc_alm_hook_event_handler(vmmc, chan, data);
		break;
	case VMMC_EVENT_DTMF_ID:
		vmmc_sig_dtmf_event_handler(vmmc, chan, data);
		break;
	default:
		printk("Ein unbekanntes Event: %x %x\n", event, data);
		break;
	}

	return IRQ_HANDLED;
}

static irqreturn_t vmmc_mbox_data_irq_handler(int irq, void *devid)
{
	struct vmmc *vmmc = devid;
	struct mps_mailbox *mbox = &vmmc->mbox_data;
	unsigned int count, type, chan;
	uint32_t data[2];
	void __iomem *addr;
	size_t len;

	mps_fifo_out(&mbox->upstream, data, 1);

	count = (data[0] & 0xff) / 8;
	type = (data[0] >> 24) & 0xff;
	chan = (data[0] >> 16) & 0xff;

	while (count) {
		mps_fifo_out(&mbox->upstream, data, 2);

		addr = (void __iomem *)CKSEG0ADDR(data[0]);
		len = data[1];

		switch (type) {
		case CMD_ADDRESS_PACKET:
			vmmc_free_paket(vmmc, addr, len);
			break;
		case CMD_RTP_VOICE_DATA_PACKET:
			vmmc_provide_paket(vmmc);
			vmmc_recv_paket(vmmc, chan, addr, len);
			break;
		}
		--count;
	}

	return IRQ_HANDLED;
}

static irqreturn_t vmmc_mbox_cmd_irq_handler(int irq, void *devid)
{
	struct vmmc *vmmc = devid;

	complete(&vmmc->cmd_completion);

	return IRQ_HANDLED;
}

static void vmmc_load_firmware(const struct firmware *fw, void *context)
{
	struct vmmc *vmmc = context;
	struct vmmc_firmware_head *fw_head;
	size_t tail_size;
	enum mps_boot_config config;

	if (!fw) {
		printk("failed to load tapi firmware\n");
//		request_firmware_nowait(THIS_MODULE, 1, "danube_firmware.bin", vmmc->dev,
//			GFP_KERNEL, vmmc, vmmc_load_firmware);
		return;
	}

	if (fw->size < sizeof(*fw_head))
		return;

	fw_head = (struct vmmc_firmware_head *)((uint8_t *)fw->data + fw->size - sizeof(*fw_head));

	if (fw_head->magic != MPS_FIRMWARE_MAGIC) {
		config = MPS_BOOT_LEGACY;
		tail_size = sizeof(uint32_t);
	} else {
		config = MPS_BOOT_ENCRYPTED;
		tail_size = sizeof(*fw_head) - sizeof(uint32_t);
	}

	vmmc_setup_fifos(vmmc);
	init_completion(&vmmc->firmware_loaded_completion);
	mps_load_firmware(vmmc->mps, fw->data, fw->size - tail_size, config);
	wait_for_completion_timeout(&vmmc->firmware_loaded_completion, 5*HZ);
	vmmc_init_timer(vmmc);
	vmmc_write_cram_data(vmmc, 0, cram_data, ARRAY_SIZE(cram_data));
	vmmc_write_cram_data(vmmc, 1, cram_data, ARRAY_SIZE(cram_data));
	vmmc_get_firmware_version(vmmc);
	vmmc_init_coders(vmmc);
}

static int vmmc_request_irqs(struct vmmc *vmmc)
{
	int ret;

	ret = request_irq(vmmc->irq_fw_loaded, vmmc_firmware_loaded_irq, 0, "vmmc fw loaded", vmmc);
	ret = request_irq(vmmc->irq_event_fifo, vmmc_event_fifo_irq, 0, "vmmc event fifo", vmmc);
	ret = request_irq(vmmc->irq_cmd_error, vmmc_cmd_error_irq, 0,
		"cmd error irq", vmmc);
	ret = request_irq(MPS_IRQ_RCV_OVERFLOW, vmmc_recv_ov_irq, 0,
		"recv_ov irq", vmmc);

	ret = request_irq(vmmc->irq_mbox_cmd, vmmc_mbox_cmd_irq_handler, 0,
		"vmmc cmd mailbox irq", vmmc);

	ret = request_irq(vmmc->irq_mbox_data, vmmc_mbox_data_irq_handler, 0,
		"vmmc data mailbox irq", vmmc);

	return ret;
}

static int __devinit vmmc_probe(struct platform_device *pdev)
{
	struct vmmc *vmmc;
	int ret = 0;

	vmmc = kzalloc(sizeof(*vmmc), GFP_KERNEL);

	if (!vmmc)
		return -ENOMEM;

	vmmc->dev = &pdev->dev;
	vmmc->mps = device_to_mps(pdev->dev.parent);

	if (!vmmc->mps) {
		goto err_free;
		ret = -EBUSY;
	}

	INIT_LIST_HEAD(&vmmc->modules);
	init_completion(&vmmc->cmd_completion);

	vmmc->irq_fw_loaded = MPS_IRQ_DOWNLOAD_DONE;
	vmmc->irq_mbox_cmd = MPS_IRQ_CMD_UPSTREAM;
	vmmc->irq_mbox_data = MPS_IRQ_DATA_UPSTREAM;
	vmmc->irq_event_fifo = MPS_IRQ_EVENT;
	vmmc->irq_cmd_error = MPS_IRQ_CMD_ERROR;

	platform_set_drvdata(pdev, vmmc);

	vmmc_request_irqs(vmmc);

	request_firmware_nowait(THIS_MODULE, 1, "danube_firmware.bin", &pdev->dev,
		GFP_KERNEL, vmmc, vmmc_load_firmware);

	return 0;

err_free:
	kfree(vmmc);

	return ret;
}

static int __devexit vmmc_remove(struct platform_device *pdev)
{
	struct vmmc *vmmc = platform_get_drvdata(pdev);
	vmmc_free_timer(vmmc);

	tapi_device_unregister(&vmmc->tdev);

	return 0;
}

static struct platform_driver vmmc_driver = {
	.probe = vmmc_probe,
	.remove = __devexit_p(vmmc_remove),
	.driver = {
		.name = "vmmc",
		.owner = THIS_MODULE
	},
};

static int __init vmmc_init(void)
{
	return platform_driver_register(&vmmc_driver);
}
module_init(vmmc_init);

static void __exit vmmc_exit(void)
{
	platform_driver_unregister(&vmmc_driver);
}
module_exit(vmmc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
