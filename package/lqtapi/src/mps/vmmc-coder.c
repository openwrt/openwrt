#include <linux/kernel.h>

#include "vmmc.h"
#include "vmmc-cmds.h"
#include "vmmc-coder.h"
#include "vmmc-module.h"
#include "mps.h"

enum vmmc_coder_encoding {
	VMMC_CODER_ENCODING_ALAW = 2,
	VMMC_CODER_ENCODING_MLAW = 3,
	VMMC_CODER_ENCODING_G726_16 = 4,
	VMMC_CODER_ENCODING_G726_24 = 5,
	VMMC_CODER_ENCODING_G726_32 = 6,
	VMMC_CODER_ENCODING_G726_40 = 7,
	VMMC_CODER_ENCODING_AMR_4_75 = 8,
	VMMC_CODER_ENCODING_AMR_5_15 = 9,
	VMMC_CODER_ENCODING_AMR_5_9 = 10,
	VMMC_CODER_ENCODING_AMR_6_7 = 11,
	VMMC_CODER_ENCODING_AMR_7_4 = 12,
	VMMC_CODER_ENCODING_AMR_7_95 = 13,
	VMMC_CODER_ENCODING_AMR_10_2 = 14,
	VMMC_CODER_ENCODING_AMR_12_2 = 15,
	VMMC_CODER_ENCODING_G728_16 = 16,
	VMMC_CODER_ENCODING_G729AB_8 = 18,
	VMMC_CODER_ENCODING_G729E_11_8 = 19,
	VMMC_CODER_ENCODING_G7221_24 = 20,
	VMMC_CODER_ENCODING_G7221_32 = 21,
	VMMC_CODER_ENCODING_G722_64 = 22,
	VMMC_CODER_ENCODING_L16_8 = 24,
	VMMC_CODER_ENCODING_L16_16 = 25,
	VMMC_CODER_ENCODING_ILBC_15_2 = 26,
	VMMC_CODER_ENCODING_ILBC_13_3 = 27,
	VMMC_CODER_ENCODING_G7231_5_3 = 28,
	VMMC_CODER_ENCODING_G7231_6_3 = 29,
	VMMC_CODER_ENCODING_ALAW_VBD = 30,
	VMMC_CODER_ENCODING_MLAW_VBD = 31,
};

static const uint8_t vmmc_coder_payload_mapping[] = {
	[VMMC_CODER_ENCODING_ALAW] = 8,
	[VMMC_CODER_ENCODING_MLAW] = 0,
	[VMMC_CODER_ENCODING_G726_16] = 35,
	[VMMC_CODER_ENCODING_G726_24] = 36,
	[VMMC_CODER_ENCODING_G726_32] = 37,
	[VMMC_CODER_ENCODING_G726_40] = 38,
	[VMMC_CODER_ENCODING_AMR_4_75] = 39,
	[VMMC_CODER_ENCODING_AMR_5_15] = 40,
	[VMMC_CODER_ENCODING_AMR_5_9] = 41,
	[VMMC_CODER_ENCODING_AMR_6_7] = 42,
	[VMMC_CODER_ENCODING_AMR_7_4] = 43,
	[VMMC_CODER_ENCODING_AMR_7_95] = 44,
	[VMMC_CODER_ENCODING_AMR_10_2] = 45,
	[VMMC_CODER_ENCODING_AMR_12_2] = 46,
	[VMMC_CODER_ENCODING_G728_16] = 47,
	[VMMC_CODER_ENCODING_G729AB_8] = 48,
	[VMMC_CODER_ENCODING_G729E_11_8] = 49,
	[VMMC_CODER_ENCODING_G7221_24] = 50,
	[VMMC_CODER_ENCODING_G7221_32] = 51,
	[VMMC_CODER_ENCODING_G722_64] = 52,
	[VMMC_CODER_ENCODING_L16_8] = 11,
	[VMMC_CODER_ENCODING_L16_16] = 10,
	[VMMC_CODER_ENCODING_ILBC_15_2] = 53,
	[VMMC_CODER_ENCODING_ILBC_13_3] = 54,
	[VMMC_CODER_ENCODING_G7231_5_3] = 4,
	[VMMC_CODER_ENCODING_G7231_6_3] = 4,
	[VMMC_CODER_ENCODING_ALAW_VBD] = 55,
	[VMMC_CODER_ENCODING_MLAW_VBD] = 56,
};

static uint32_t payload_enc[9];
static uint32_t payload_dec[8];

static inline struct vmmc_coder *vmmc_module_to_coder(struct vmmc_module *module)
{
	return container_of(module, struct vmmc_coder, module);
}

static int vmmc_coder_enable(struct vmmc_module *module, bool enable)
{
	struct vmmc_coder *coder = vmmc_module_to_coder(module);

	coder->enabled = enable;

/*	coder->cmd_cache[0] = VMMC_CMD_CODER_DATA1(coder->enabled, 0, 0, 0, coder->id,
		module->pins[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return vmmc_command_write(coder->vmmc, VMMC_CMD_CODER(coder->id),
	coder->cmd_cache);*/
	return 0;
}

static int vmmc_coder_sync(struct vmmc_module *module)
{
	struct vmmc_coder *coder = vmmc_module_to_coder(module);

	coder->cmd_cache[0] = VMMC_CMD_CODER_DATA1(coder->enabled, 0, 1, 1, coder->id,
		module->pins[0], 1, 1, 1, 1, 1, 1, 0, 0, 0, VMMC_CODER_ENCODING_MLAW);
	coder->cmd_cache[1] = VMMC_CMD_CODER_DATA2(0x2000, 0x2000);
	coder->cmd_cache[2] = VMMC_CMD_CODER_DATA3(0, 0, module->pins[1], 0,
		module->pins[2], 0, module->pins[3], module->pins[4]);

	return vmmc_command_write(coder->vmmc, VMMC_CMD_CODER(coder->id), coder->cmd_cache);
}

static const struct vmmc_module_ops vmmc_coder_module_ops = {
	.sync = vmmc_coder_sync,
	.enable = vmmc_coder_enable,
};

#define VMMC_CMD_CODER_JB(_chan) (_VMMC_MSG(6, 3, 18) | _VMMC_CHAN(_chan) | 8)
#define VMMC_CMD_CODER_JB_DATA1(_sf, _prp, _pje, _dvf, _nam, _rad, _adap, _init_pod) \
	(((_sf) << 31) | ((_prp) << 23) | ((_pje) << 22) | ((_dvf) << 21) | \
	((_nam) << 20) | ((_rad) << 17) | ((_adap) << 16) | (_init_pod))
#define VMMC_CMD_CODER_JB_DATA2(_min_pod, _max_pod) \
	(((_min_pod) << 16) | (_max_pod))
#define VMMC_CMD_CODER_JB_DATA3(_adap_factor, _min_margin, _mode) \
	(((_adap_factor) << 24) | ((_min_margin) << 16) | (_mode))

int vmmc_coder_init(struct vmmc_coder *coder, struct vmmc *vmmc, unsigned int id)
{
	int ret;

	ret = vmmc_module_init(&coder->module, 5, &vmmc_coder_module_ops);
	if (ret)
		return ret;

	coder->id = id;
	coder->module.id = id + 0x18;
	coder->vmmc = vmmc;
	coder->enabled = 0;

	coder->cmd_cache[0] = VMMC_CMD_CODER_DATA1(0, 0, 0, 0, id, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	coder->cmd_cache[1] = VMMC_CMD_CODER_DATA2(0, 0);
	coder->cmd_cache[2] = VMMC_CMD_CODER_DATA3(0, 0, 0, 0, 0, 0, 0, 0);
	coder->cmd_cache[3] = VMMC_CMD_CODER_DATA4(0);

	coder->jitter_buffer_cache[0] = VMMC_CMD_CODER_JB_DATA1(0x16, 1, 0, 0, 1, 1,
	1, 0x50);
	coder->jitter_buffer_cache[1] = VMMC_CMD_CODER_JB_DATA2(0x50, 0x05a0);
	coder->jitter_buffer_cache[2] = VMMC_CMD_CODER_JB_DATA3(0x0d, 0x28, 1);

	vmmc_command_write(coder->vmmc, VMMC_CMD_RTP_CFG_US(coder->id), payload_enc);
	vmmc_command_write(coder->vmmc, VMMC_CMD_RTP_CFG_DS(coder->id), payload_dec);
	vmmc_command_write(coder->vmmc, VMMC_CMD_CODER_JB(coder->id),
	coder->jitter_buffer_cache);
	vmmc_command_write(coder->vmmc, VMMC_CMD_CODER(coder->id), coder->cmd_cache);

	vmmc_register_module(vmmc, &coder->module);

	return ret;
}

void vmmc_coder_event(struct vmmc *vmmc, unsigned int chan, unsigned int type)
{
}

/*
void vmmc_coder_event(struct vmmc_coder *coder, unsigned int len)
{
	struct sk_buff *skb;
	provide_mem_region(coder->vmmc, vmmc_coder_alloc_paket(coder)->head, 512);

	skb = skb_dequeue(&coder->paket_queue);
	skb_put(skb, len);

	tapi_stream_recv(&coder->vmmc->tdev, skb);
}
*/
/*
int vmmc_coder_set_codec(struct vmmc_coder *coder, enum vmmc_codec codec)
{
}*/

void vmmc_init_coders(struct vmmc *vmmc)
{
	unsigned int i;

	payload_enc[0] = 0;
	payload_enc[1] = 0x0;
	payload_enc[1] |= vmmc_coder_payload_mapping[2] << 8;
	payload_enc[1] |= vmmc_coder_payload_mapping[3] << 8;
	for (i = 2; i < 9; ++i) {
		payload_enc[i] = vmmc_coder_payload_mapping[i*4 - 4] << 24;
		payload_enc[i] |= vmmc_coder_payload_mapping[i*4 - 3] << 16;
		payload_enc[i] |= vmmc_coder_payload_mapping[i*4 - 2] << 8;
		payload_enc[i] |= vmmc_coder_payload_mapping[i*4 - 1];
		payload_enc[i] |= 0x80808080;
	}
	for (i = 0; i < 7; ++i) {
		payload_dec[i] = vmmc_coder_payload_mapping[i*4 + 2] << 24;
		payload_dec[i] |= vmmc_coder_payload_mapping[i*4 + 3] << 16;
		payload_dec[i] |= vmmc_coder_payload_mapping[i*4 + 4] << 8;
		payload_dec[i] |= vmmc_coder_payload_mapping[i*4 + 5];
	}
	payload_dec[i] = vmmc_coder_payload_mapping[i*4 + 2] << 24;
	payload_dec[i] |= vmmc_coder_payload_mapping[i*4 + 3] << 16;


	vmmc->num_coders = 5;
	vmmc->coder = kcalloc(sizeof(*vmmc->coder), vmmc->num_coders, GFP_KERNEL);

	for (i = 0; i < vmmc->num_coders; ++i)
		vmmc_coder_init(&vmmc->coder[i], vmmc, i);
}

struct vmmc_coder *vmmc_coder_get(struct vmmc *vmmc)
{
	unsigned int i;
	for (i = 0; i < vmmc->num_coders; ++i) {
		if (!test_and_set_bit(i, &vmmc->coder_used))
			return &vmmc->coder[i];
	}

	return NULL;
}

void vmmc_coder_put(struct vmmc *vmmc, struct vmmc_coder *coder)
{
	clear_bit(coder->id, &vmmc->coder_used);
}
