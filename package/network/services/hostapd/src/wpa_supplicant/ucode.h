#ifndef __WPAS_UCODE_H
#define __WPAS_UCODE_H

#include "utils/ucode.h"

struct wpa_bss;
struct wpa_global;
union wpa_event_data;
struct wpa_supplicant;
struct wps_credential;

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
void wpas_ucode_update_state(struct wpa_supplicant *wpa_s);
void wpas_ucode_event(struct wpa_supplicant *wpa_s, int event, union wpa_event_data *data);
void wpas_ucode_ctrl_event(struct wpa_supplicant *wpa_s, const char *str, size_t len);
bool wpas_ucode_bss_allowed(struct wpa_supplicant *wpa_s, struct wpa_bss *bss);
void wpas_ucode_wps_complete(struct wpa_supplicant *wpa_s,
			     const struct wps_credential *cred);
int wpas_ucode_wps_m8_rx(struct wpa_supplicant *wpa_s,
			  const u8 *data, size_t data_len);
#ifdef CONFIG_DPP
int wpas_ucode_dpp_rx_action(struct wpa_supplicant *wpa_s, const u8 *src,
			     u8 frame_type, unsigned int freq,
			     const u8 *data, size_t data_len);
int wpas_ucode_dpp_gas_rx(struct wpa_supplicant *wpa_s, const u8 *src,
			  const u8 *data, size_t data_len, unsigned int freq);
#else
static inline int wpas_ucode_dpp_rx_action(struct wpa_supplicant *wpa_s,
					   const u8 *src, u8 frame_type,
					   unsigned int freq, const u8 *data,
					   size_t data_len)
{
	return 0;
}

static inline int wpas_ucode_dpp_gas_rx(struct wpa_supplicant *wpa_s,
					const u8 *src, const u8 *data,
					size_t data_len, unsigned int freq)
{
	return 0;
}
#endif /* CONFIG_DPP */
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

static inline void wpas_ucode_update_state(struct wpa_supplicant *wpa_s)
{
}

static inline void wpas_ucode_event(struct wpa_supplicant *wpa_s, int event, union wpa_event_data *data)
{
}

static inline void wpas_ucode_ctrl_event(struct wpa_supplicant *wpa_s, const char *str, size_t len)
{
}

static inline bool wpas_ucode_bss_allowed(struct wpa_supplicant *wpa_s, struct wpa_bss *bss)
{
	return true;
}

static inline void wpas_ucode_wps_complete(struct wpa_supplicant *wpa_s, const struct wps_credential *cred)
{
}

static inline int wpas_ucode_wps_m8_rx(struct wpa_supplicant *wpa_s,
					const u8 *data, size_t data_len)
{
	return 0;
}

static inline int wpas_ucode_dpp_rx_action(struct wpa_supplicant *wpa_s,
					   const u8 *src, u8 frame_type,
					   unsigned int freq, const u8 *data,
					   size_t data_len)
{
	return 0;
}

static inline int wpas_ucode_dpp_gas_rx(struct wpa_supplicant *wpa_s,
					const u8 *src, const u8 *data,
					size_t data_len, unsigned int freq)
{
	return 0;
}
#endif

#endif
