#ifndef _GSW_SWCONFIG_H_
#define _GSW_SWCONFIG_H_

#include "gsw_sw_init.h"

extern int intel_swconfig_init(struct intel_gsw *gsw);
extern void intel_swconfig_destroy(struct intel_gsw *gsw);

extern void intel_init(struct intel_gsw *gsw);
extern void intel_deinit(struct intel_gsw *gsw);

#endif
