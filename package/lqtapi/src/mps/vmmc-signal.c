#include <linux/kernel.h>

#include "vmmc.h"
#include "vmmc-sig.h"
#include "vmmc-cmds.h"

static struct vmmc_sig *vmmc_module_to_sig(struct vmmc_module *module)
{
	return container_of(module, struct vmmc_sig, module);
}

static int vmmc_sig_enable(struct vmmc_module *module, bool enabled)
{
	struct vmmc_sig *sig = vmmc_module_to_sig(module);

	return 0;

	sig->sig_cache = VMMC_CMD_SIG_SET_ENABLE(sig->sig_cache, enabled);

	return vmmc_command_write(sig->vmmc, VMMC_CMD_SIG(sig->id), &sig->sig_cache);
}

static int vmmc_sig_sync(struct vmmc_module *module)
{
	struct vmmc_sig *sig = vmmc_module_to_sig(module);

	sig->sig_cache = VMMC_CMD_SIG_SET_INPUTS(sig->sig_cache,
		module->pins[0], module->pins[1]);

	vmmc_command_write(sig->vmmc, VMMC_CMD_SIG(sig->id),
		&sig->sig_cache);

	return vmmc_command_write(sig->vmmc, VMMC_CMD_DTMFR(sig->id),
		&sig->dtmfr_cache);
}

static const struct vmmc_module_ops vmmc_sig_ops = {
	.enable = vmmc_sig_enable,
	.sync = vmmc_sig_sync,
};

int vmmc_sig_init(struct vmmc_sig *sig, struct vmmc *vmmc, unsigned int id)
{
	int ret;

	ret = vmmc_module_init(&sig->module, 2, &vmmc_sig_ops);
	if (ret)
		return ret;

	sig->id = id;
	sig->module.id = id + 0x1e;
	sig->vmmc = vmmc;
	sig->sig_cache = VMMC_CMD_SIG_DATA(1, 1, 0, 0, 0, 0, 0);
	sig->dtmfr_cache = VMMC_CMD_DTMFR_DATA(1, 1, id);

	vmmc_register_module(vmmc, &sig->module);

	vmmc_command_write(sig->vmmc, VMMC_CMD_SIG(sig->id),
		&sig->sig_cache);
	vmmc_command_write(sig->vmmc, VMMC_CMD_DTMFR(sig->id),
		&sig->dtmfr_cache);

	return ret;
}

void vmmc_sig_dtmf_event_handler(struct vmmc *vmmc, uint32_t id, uint32_t data)
{
	tapi_report_dtmf_event(&vmmc->tdev, &vmmc->tdev.ports[id], data & 0xf);
}
