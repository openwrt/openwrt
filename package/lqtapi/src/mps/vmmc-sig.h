#ifndef __VMMC_SIG_H__
#define __VMMC_SIG_H__

#include "vmmc-module.h"

struct vmmc_sig {
	struct vmmc *vmmc;
	unsigned int id;
	struct vmmc_module module;

	uint32_t sig_cache;
	uint32_t dtmfr_cache;
};

int vmmc_sig_init(struct vmmc_sig *sig, struct vmmc *vmmc, unsigned int id);

void vmmc_sig_dtmf_event_handler(struct vmmc *vmmc, uint32_t event, uint32_t data);

#endif
