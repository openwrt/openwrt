#ifndef  __VMMC_ALM_H__
#define  __VMMC_ALM_H__

#include "vmmc-module.h"

struct vmmc;
enum vmmc_alm_state {
	VMMC_ALM_STATE_OFF = 0 << 16,
	VMMC_ALM_STATE_ONHOOK = 1 << 16,
	VMMC_ALM_STATE_RING = 3 << 16,
	VMMC_ALM_STATE_ACTIVE = 2 << 16,
};

struct vmmc_alm {
	struct vmmc *vmmc;
	unsigned int id;

	enum vmmc_alm_state state;

	struct vmmc_module module;

	uint32_t cmd_cache[3];
};

struct vmmc_alm_coef {
	struct list_head list;
	unsigned int offset;
	size_t len;
	uint32_t data[0];
};

int vmmc_alm_init(struct vmmc_alm *alm, struct vmmc *vmmc, unsigned int id);

int vmmc_alm_set_state(struct vmmc_alm *alm, enum vmmc_alm_state state);
enum vmmc_alm_state vmmc_alm_get_state(struct vmmc_alm *alm);

int vmcc_alm_set_coefficents(struct vmmc_alm *alm,
	const struct vmmc_alm_coef *coef_list);

void vmmc_alm_hook_event_handler(struct vmmc *vmmc, uint32_t event, uint32_t data);

#endif
