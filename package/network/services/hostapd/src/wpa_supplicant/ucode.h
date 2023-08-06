#ifndef __WPAS_UCODE_H
#define __WPAS_UCODE_H

#include "utils/ucode.h"

struct wpa_global;
struct wpa_supplicant;

struct wpas_ucode_bss {
#ifdef UCODE_SUPPORT
	unsigned int idx;
#endif
};

#ifdef UCODE_SUPPORT
int wpas_ucode_init(struct wpa_global *gl);
void wpas_ucode_free(void);
void wpas_ucode_add_bss(struct wpa_supplicant *wpa_s);
void wpas_ucode_free_bss(struct wpa_supplicant *wpa_s);
#else
static inline int wpas_ucode_init(struct wpa_global *gl)
{
	return -EINVAL;
}
static inline void wpas_ucode_free(void)
{
}
static inline void wpas_ucode_add_bss(struct wpa_supplicant *wpa_s)
{
}

static inline void wpas_ucode_free_bss(struct wpa_supplicant *wpa_s)
{
}

#endif

#endif
