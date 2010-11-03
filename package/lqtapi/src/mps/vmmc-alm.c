#include <linux/kernel.h>

#include "vmmc.h"
#include "vmmc-cmds.h"
#include "vmmc-alm.h"
#include "vmmc-module.h"
#include "mps.h"

static inline struct vmmc_alm *vmmc_module_to_alm(struct vmmc_module *module)
{
	return container_of(module, struct vmmc_alm, module);
}

int vmmc_alm_set_state(struct vmmc_alm *alm, enum vmmc_alm_state state)
{
	int ret;

	ret = vmmc_command_write(alm->vmmc, VMMC_CMD_OPMODE(alm->id), &state);
	if (!ret)
		alm->state = state;

	return ret;
}

enum vmmc_alm_state vmmc_alm_get_state(struct vmmc_alm *alm)
{
	return alm->state;
}

static struct vmmc_alm_coef *vmmc_alm_coef_alloc(unsigned int offset, size_t len)
{
	struct vmmc_alm_coef *coef;

	coef = kzalloc(sizeof(*coef) + sizeof(uint32_t) * DIV_ROUND_UP(len, 4),
			GFP_KERNEL);
	coef->offset = offset;
	coef->len = len;

	return coef;
}

int vmcc_alm_set_coefficents(struct vmmc_alm *alm,
	const struct vmmc_alm_coef *coef_list)
{
	int ret = 0;
	uint32_t cmd;
	struct vmmc_alm_coef *coef;
	struct list_head l;

	INIT_LIST_HEAD(&l);

	coef = vmmc_alm_coef_alloc(0x37, 8);
	coef->data[0] = 0x76d7871d;
	coef->data[1] = 0x7fbb7ff4;
	list_add_tail(&coef->list, &l);
	coef = vmmc_alm_coef_alloc(0x5e, 2);
	coef->data[0] = 0x7e000000;
	list_add_tail(&coef->list, &l);
	coef = vmmc_alm_coef_alloc(0x6c, 2);
	coef->data[0] = 0x7e000000;
	list_add_tail(&coef->list, &l);

	list_for_each_entry(coef, &l, list) {
		cmd = VMMC_CMD_ALM_COEF(alm->id, coef->offset, coef->len);
		ret = vmmc_command_write(alm->vmmc, cmd, coef->data);
		if (ret)
			break;
	}

	return ret;
}

static int vmmc_alm_sync(struct vmmc_module *module)
{
	struct vmmc_alm *alm = vmmc_module_to_alm(module);

	alm->cmd_cache[0] = VMMC_CMD_ALI_DATA1(1, 0, 0, 1, 0, module->pins[0], 0x4000);
	alm->cmd_cache[1] = VMMC_CMD_ALI_DATA2(0x4000, module->pins[1], module->pins[2]);
	alm->cmd_cache[2] = VMMC_CMD_ALI_DATA3(module->pins[3], module->pins[4]);

	return vmmc_command_write(alm->vmmc, VMMC_CMD_ALI(alm->id), alm->cmd_cache);
}

static int vmmc_alm_enable(struct vmmc_module *module, bool enable)
{
	struct vmmc_alm *alm = vmmc_module_to_alm(module);

	return vmmc_command_write(alm->vmmc, VMMC_CMD_ALI(alm->id), alm->cmd_cache);
}

static const struct vmmc_module_ops vmmc_alm_module_ops = {
	.sync = vmmc_alm_sync,
	.enable = vmmc_alm_enable,
};

int vmmc_alm_init(struct vmmc_alm *alm, struct vmmc *vmmc, unsigned int id)
{
	int ret;

	ret = vmmc_module_init(&alm->module, 5, &vmmc_alm_module_ops);
	if (ret)
		return ret;

	alm->id = id;
	alm->module.id = id + 0x4;
	alm->vmmc = vmmc;

	alm->cmd_cache[0] = VMMC_CMD_ALI_DATA1(1, 0, 0, 1, 0, 0, 0x2000);
	alm->cmd_cache[1] = VMMC_CMD_ALI_DATA2(0x2000, 0, 0);
	alm->cmd_cache[2] = VMMC_CMD_ALI_DATA3(0, 0);

	vmmc_command_write(alm->vmmc, VMMC_CMD_ALI(alm->id), alm->cmd_cache);
	vmcc_alm_set_coefficents(alm, NULL);
	vmmc_register_module(vmmc, &alm->module);

// disable lec
// write lec coef
// write nlp coef
// enable lec

// ALI_LEC ALI_ES RES_LEC_COEF RES_LEC_NLP_COEF

	return ret;
}

void vmmc_alm_hook_event_handler(struct vmmc *vmmc, uint32_t id, uint32_t data)
{
	tapi_report_hook_event(&vmmc->tdev, &vmmc->tdev.ports[id], data & 1);
}
