#ifndef M68K_CF_CACHEFLUSH_H
#define M68K_CF_CACHEFLUSH_H

#ifdef CONFIG_M5445X
#include "cf_5445x_cacheflush.h"
#else
#include "cf_548x_cacheflush.h"
#endif

#endif /* M68K_CF_CACHEFLUSH_H */
