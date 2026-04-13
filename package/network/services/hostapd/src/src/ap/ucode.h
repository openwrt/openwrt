#ifndef __HOSTAPD_AP_UCODE_H
#define __HOSTAPD_AP_UCODE_H

#include "utils/ucode.h"

struct hostapd_data;

struct hostapd_ucode_bss {
#ifdef UCODE_SUPPORT
	int idx;
#endif
};

struct hostapd_ucode_iface {
#ifdef UCODE_SUPPORT
	int idx;
#endif
};

#ifdef UCODE_SUPPORT

int hostapd_ucode_init(struct hapd_interfaces *ifaces);

void hostapd_ucode_free(void);
void hostapd_ucode_free_iface(struct hostapd_iface *iface);
void hostapd_ucode_free_bss(struct hostapd_data *hapd);
void hostapd_ucode_bss_cb(struct hostapd_data *hapd, const char *type);
int hostapd_ucode_sta_auth(struct hostapd_data *hapd, struct sta_info *sta);
void hostapd_ucode_sta_connected(struct hostapd_data *hapd, struct sta_info *sta);

#ifdef CONFIG_APUP
void hostapd_ucode_apup_newpeer(struct hostapd_data *hapd, const char *ifname);
#endif // def CONFIG_APUP

void hostapd_ucode_wps_m7_rx(struct hostapd_data *hapd, const u8 *addr,
			      const u8 *data, size_t data_len,
			      struct wpabuf **m8_encr_extra, int *skip_cred);

#ifdef CONFIG_DPP
int hostapd_ucode_dpp_rx_action(struct hostapd_data *hapd, const u8 *src,
				u8 frame_type, unsigned int freq,
				const u8 *data, size_t data_len);
struct wpabuf *hostapd_ucode_dpp_gas_req(struct hostapd_data *hapd,
					 const u8 *sa, u8 dialog_token,
					 const u8 *query, size_t query_len);
#endif /* CONFIG_DPP */

#else

static inline int hostapd_ucode_init(struct hapd_interfaces *ifaces)
{
	return -EINVAL;
}
static inline void hostapd_ucode_free(void)
{
}
static inline void hostapd_ucode_free_iface(struct hostapd_iface *iface)
{
}
static inline void hostapd_ucode_bss_cb(struct hostapd_data *hapd, const char *type)
{
}
static inline int hostapd_ucode_sta_auth(struct hostapd_data *hapd, struct sta_info *sta)
{
	return 0;
}
static inline void hostapd_ucode_sta_connected(struct hostapd_data *hapd, struct sta_info *sta)
{
}
static inline void hostapd_ucode_free_bss(struct hostapd_data *hapd)
{
}
static inline void hostapd_ucode_wps_m7_rx(struct hostapd_data *hapd,
					    const u8 *addr,
					    const u8 *data, size_t data_len,
					    struct wpabuf **m8_encr_extra,
					    int *skip_cred)
{
}

#ifdef CONFIG_DPP
static inline int hostapd_ucode_dpp_rx_action(struct hostapd_data *hapd,
					      const u8 *src, u8 frame_type,
					      unsigned int freq,
					      const u8 *data, size_t data_len)
{
	return 0;
}
static inline struct wpabuf *hostapd_ucode_dpp_gas_req(struct hostapd_data *hapd,
						       const u8 *sa,
						       u8 dialog_token,
						       const u8 *query,
						       size_t query_len)
{
	return NULL;
}
#endif /* CONFIG_DPP */

#endif

static inline void hostapd_ucode_create_bss(struct hostapd_data *hapd)
{
	hostapd_ucode_bss_cb(hapd, "bss_create");
}

static inline void hostapd_ucode_add_bss(struct hostapd_data *hapd)
{
	hostapd_ucode_bss_cb(hapd, "bss_add");
}

static inline void hostapd_ucode_reload_bss(struct hostapd_data *hapd)
{
	hostapd_ucode_bss_cb(hapd, "bss_reload");
}

#endif
