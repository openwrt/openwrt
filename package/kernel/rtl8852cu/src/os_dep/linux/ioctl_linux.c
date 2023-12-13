/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _IOCTL_LINUX_C_

#include <drv_types.h>
#include <rtw_mp.h>


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27))
#define  iwe_stream_add_event(a, b, c, d, e)  iwe_stream_add_event(b, c, d, e)
#define  iwe_stream_add_point(a, b, c, d, e)  iwe_stream_add_point(b, c, d, e)
#endif

#ifdef CONFIG_80211N_HT
extern int rtw_ht_enable;
#endif


#define RTL_IOCTL_WPA_SUPPLICANT	(SIOCIWFIRSTPRIV+30)

#define SCAN_ITEM_SIZE 768
#define MAX_CUSTOM_LEN 64
#define RATE_COUNT 4
#define MAX_SCAN_BUFFER_LEN 65535

#ifdef CONFIG_GLOBAL_UI_PID
extern int ui_pid[3];
#endif

/* combo scan */
#define WEXT_CSCAN_AMOUNT 9
#define WEXT_CSCAN_BUF_LEN		360
#define WEXT_CSCAN_HEADER		"CSCAN S\x01\x00\x00S\x00"
#define WEXT_CSCAN_HEADER_SIZE		12
#define WEXT_CSCAN_SSID_SECTION		'S'
#define WEXT_CSCAN_CHANNEL_SECTION	'C'
#define WEXT_CSCAN_NPROBE_SECTION	'N'
#define WEXT_CSCAN_ACTV_DWELL_SECTION	'A'
#define WEXT_CSCAN_PASV_DWELL_SECTION	'P'
#define WEXT_CSCAN_HOME_DWELL_SECTION	'H'
#define WEXT_CSCAN_TYPE_SECTION		'T'


extern u8 key_2char2num(u8 hch, u8 lch);
extern u8 str_2char2num(u8 hch, u8 lch);
extern void macstr2num(u8 *dst, u8 *src);
extern u8 convert_ip_addr(u8 hch, u8 mch, u8 lch);

u32 rtw_rates[] = {1000000, 2000000, 5500000, 11000000,
	6000000, 9000000, 12000000, 18000000, 24000000, 36000000, 48000000, 54000000};

/**
 * hwaddr_aton - Convert ASCII string to MAC address
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */
static int hwaddr_aton_i(const char *txt, u8 *addr)
{
	int i;

	for (i = 0; i < 6; i++) {
		int a, b;

		a = hex2num_i(*txt++);
		if (a < 0)
			return -1;
		b = hex2num_i(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}

	return 0;
}
#ifdef CONFIG_RTW_ANDROID
static void indicate_wx_custom_event(_adapter *padapter, char *msg)
{
	u8 *buff;
	union iwreq_data wrqu;

	if (strlen(msg) > IW_CUSTOM_MAX) {
		RTW_INFO("%s strlen(msg):%zu > IW_CUSTOM_MAX:%u\n", __FUNCTION__ , strlen(msg), IW_CUSTOM_MAX);
		return;
	}

	buff = rtw_zmalloc(IW_CUSTOM_MAX + 1);
	if (!buff)
		return;

	_rtw_memcpy(buff, msg, strlen(msg));

	_rtw_memset(&wrqu, 0, sizeof(wrqu));
	wrqu.data.length = strlen(msg);

	RTW_INFO("%s %s\n", __FUNCTION__, buff);
#ifndef CONFIG_IOCTL_CFG80211
	wireless_send_event(padapter->pnetdev, IWEVCUSTOM, &wrqu, buff);
#endif

	rtw_mfree(buff, IW_CUSTOM_MAX + 1);

}
#endif

#if 0
static void request_wps_pbc_event(_adapter *padapter)
{
	u8 *buff, *p;
	union iwreq_data wrqu;


	buff = rtw_malloc(IW_CUSTOM_MAX);
	if (!buff)
		return;

	_rtw_memset(buff, 0, IW_CUSTOM_MAX);

	p = buff;

	p += sprintf(p, "WPS_PBC_START.request=TRUE");

	_rtw_memset(&wrqu, 0, sizeof(wrqu));

	wrqu.data.length = p - buff;

	wrqu.data.length = (wrqu.data.length < IW_CUSTOM_MAX) ? wrqu.data.length : IW_CUSTOM_MAX;

	RTW_INFO("%s\n", __FUNCTION__);

#ifndef CONFIG_IOCTL_CFG80211
	wireless_send_event(padapter->pnetdev, IWEVCUSTOM, &wrqu, buff);
#endif

	if (buff)
		rtw_mfree(buff, IW_CUSTOM_MAX);

}
#endif

#ifdef CONFIG_SUPPORT_HW_WPS_PBC
void rtw_request_wps_pbc_event(_adapter *padapter)
{
#ifdef RTK_DMP_PLATFORM
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 12))
	kobject_uevent(&padapter->pnetdev->dev.kobj, KOBJ_NET_PBC);
#else
	kobject_hotplug(&padapter->pnetdev->class_dev.kobj, KOBJ_NET_PBC);
#endif
#else

	if (padapter->pid[0] == 0) {
		/*	0 is the default value and it means the application monitors the HW PBC doesn't privde its pid to driver. */
		return;
	}

	rtw_signal_process(padapter->pid[0], SIGUSR1);

#endif

	rtw_led_control(padapter, LED_CTL_START_WPS_BOTTON);
}
#endif/* #ifdef CONFIG_SUPPORT_HW_WPS_PBC */

void indicate_wx_scan_complete_event(_adapter *padapter)
{
	union iwreq_data wrqu;

	_rtw_memset(&wrqu, 0, sizeof(union iwreq_data));

	/* RTW_INFO("+rtw_indicate_wx_scan_complete_event\n"); */
#ifndef CONFIG_IOCTL_CFG80211
	wireless_send_event(padapter->pnetdev, SIOCGIWSCAN, &wrqu, NULL);
#endif
}


void rtw_indicate_wx_assoc_event(_adapter *padapter)
{
	union iwreq_data wrqu;
	struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX		*pnetwork = (WLAN_BSSID_EX *)(&(pmlmeinfo->dev_network));

	_rtw_memset(&wrqu, 0, sizeof(union iwreq_data));

	wrqu.ap_addr.sa_family = ARPHRD_ETHER;

	if (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)
		_rtw_memcpy(wrqu.ap_addr.sa_data, pnetwork->MacAddress, ETH_ALEN);
	else
		_rtw_memcpy(wrqu.ap_addr.sa_data, pmlmepriv->dev_cur_network.network.MacAddress, ETH_ALEN);

	RTW_PRINT("assoc success\n");
#ifndef CONFIG_IOCTL_CFG80211
	wireless_send_event(padapter->pnetdev, SIOCGIWAP, &wrqu, NULL);
#endif
}

void rtw_indicate_wx_disassoc_event(_adapter *padapter)
{
	union iwreq_data wrqu;

	_rtw_memset(&wrqu, 0, sizeof(union iwreq_data));

	wrqu.ap_addr.sa_family = ARPHRD_ETHER;
	_rtw_memset(wrqu.ap_addr.sa_data, 0, ETH_ALEN);

#ifndef CONFIG_IOCTL_CFG80211
	RTW_PRINT("indicate disassoc\n");
	wireless_send_event(padapter->pnetdev, SIOCGIWAP, &wrqu, NULL);
#endif
}

/*
uint	rtw_is_cckrates_included(u8 *rate)
{
		u32	i = 0;

		while(rate[i]!=0)
		{
			if  (  (((rate[i]) & 0x7f) == 2)	|| (((rate[i]) & 0x7f) == 4) ||
			(((rate[i]) & 0x7f) == 11)  || (((rate[i]) & 0x7f) == 22) )
			return _TRUE;
			i++;
		}

		return _FALSE;
}

uint	rtw_is_cckratesonly_included(u8 *rate)
{
	u32 i = 0;

	while(rate[i]!=0)
	{
			if  (  (((rate[i]) & 0x7f) != 2) && (((rate[i]) & 0x7f) != 4) &&
				(((rate[i]) & 0x7f) != 11)  && (((rate[i]) & 0x7f) != 22) )
			return _FALSE;
			i++;
	}

	return _TRUE;
}
*/

static inline char *iwe_stream_mac_addr_proess(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	/*  AP MAC address */
	iwe->cmd = SIOCGIWAP;
	iwe->u.ap_addr.sa_family = ARPHRD_ETHER;

	_rtw_memcpy(iwe->u.ap_addr.sa_data, pnetwork->network.MacAddress, ETH_ALEN);
	start = iwe_stream_add_event(info, start, stop, iwe, IW_EV_ADDR_LEN);
	return start;
}
static inline char *iwe_stream_essid_proess(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{

	/* Add the ESSID */
	iwe->cmd = SIOCGIWESSID;
	iwe->u.data.flags = 1;
	iwe->u.data.length = min((u16)pnetwork->network.Ssid.SsidLength, (u16)32);
	start = iwe_stream_add_point(info, start, stop, iwe, pnetwork->network.Ssid.Ssid);
	return start;
}

static inline char *iwe_stream_chan_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	if (pnetwork->network.Configuration.DSConfig < 1 /*|| pnetwork->network.Configuration.DSConfig>14*/)
		pnetwork->network.Configuration.DSConfig = 1;

	/* Add frequency/channel */
	iwe->cmd = SIOCGIWFREQ;
	iwe->u.freq.m = rtw_ch2freq(pnetwork->network.Configuration.DSConfig) * 100000;
	iwe->u.freq.e = 1;
	iwe->u.freq.i = pnetwork->network.Configuration.DSConfig;
	start = iwe_stream_add_event(info, start, stop, iwe, IW_EV_FREQ_LEN);
	return start;
}
static inline char *iwe_stream_mode_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe, u16 cap)
{
	/* Add mode */
	if (cap & (WLAN_CAPABILITY_IBSS | WLAN_CAPABILITY_BSS)) {
		iwe->cmd = SIOCGIWMODE;
		if (cap & WLAN_CAPABILITY_BSS)
			iwe->u.mode = IW_MODE_MASTER;
		else
			iwe->u.mode = IW_MODE_ADHOC;

		start = iwe_stream_add_event(info, start, stop, iwe, IW_EV_UINT_LEN);
	}
	return start;
}
static inline char *iwe_stream_encryption_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe, u16 cap)
{

	/* Add encryption capability */
	iwe->cmd = SIOCGIWENCODE;
	if (cap & WLAN_CAPABILITY_PRIVACY)
		iwe->u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
	else
		iwe->u.data.flags = IW_ENCODE_DISABLED;
	iwe->u.data.length = 0;
	start = iwe_stream_add_point(info, start, stop, iwe, pnetwork->network.Ssid.Ssid);
	return start;

}

static inline char *iwe_stream_protocol_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	u16 ht_cap = _FALSE, vht_cap = _FALSE;
	u32 ht_ielen = 0, vht_ielen = 0;
	char *p;
	u8 ie_offset = (pnetwork->network.Reserved[0] == BSS_TYPE_PROB_REQ ? 0 : 12); /* Probe Request	 */

#ifdef CONFIG_80211N_HT
	/* parsing HT_CAP_IE	 */
	if(padapter->registrypriv.ht_enable && is_supported_ht(padapter->registrypriv.wireless_mode)) {
		p = rtw_get_ie(&pnetwork->network.IEs[ie_offset], _HT_CAPABILITY_IE_, &ht_ielen, pnetwork->network.IELength - ie_offset);
		if (p && ht_ielen > 0)
			ht_cap = _TRUE;
	}
#endif

#ifdef CONFIG_80211AC_VHT
	/* parsing VHT_CAP_IE */
	if(is_supported_vht(padapter->registrypriv.wireless_mode)) {
		p = rtw_get_ie(&pnetwork->network.IEs[ie_offset], EID_VHTCapability, &vht_ielen, pnetwork->network.IELength - ie_offset);
		if (p && vht_ielen > 0)
			vht_cap = _TRUE;
	}
#endif
	/* Add the protocol name */
	iwe->cmd = SIOCGIWNAME;
	if ((rtw_is_cckratesonly_included((u8 *)&pnetwork->network.SupportedRates)) == _TRUE) {
		if (ht_cap == _TRUE)
			snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11bn");
		else
			snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11b");
	} else if ((rtw_is_cckrates_included((u8 *)&pnetwork->network.SupportedRates)) == _TRUE) {
		if (ht_cap == _TRUE)
			snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11bgn");
		else
			snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11bg");
	} else {
		if (pnetwork->network.Configuration.DSConfig > 14) {
			#ifdef CONFIG_80211AC_VHT
			if (vht_cap == _TRUE)
				snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11AC");
			else
			#endif
			{
				if (ht_cap == _TRUE)
					snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11an");
				else
					snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11a");
			}
		} else {
			if (ht_cap == _TRUE)
				snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11gn");
			else
				snprintf(iwe->u.name, IFNAMSIZ, "IEEE 802.11g");
		}
	}
	start = iwe_stream_add_event(info, start, stop, iwe, IW_EV_CHAR_LEN);
	return start;
}

static inline char *iwe_stream_rate_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	u32 ht_ielen = 0, vht_ielen = 0;
	char *p;
	u16 max_rate = 0, rate, ht_cap = _FALSE, vht_cap = _FALSE;
	u32 i = 0;
	u8 bw_40MHz = 0, short_GI = 0, bw_160MHz = 0, vht_highest_rate = 0;
	u16 mcs_rate = 0, vht_data_rate = 0;
	char custom[MAX_CUSTOM_LEN] = {0};
	u8 ie_offset = (pnetwork->network.Reserved[0] == BSS_TYPE_PROB_REQ ? 0 : 12); /* Probe Request	 */

	/* parsing HT_CAP_IE	 */
	if(is_supported_ht(padapter->registrypriv.wireless_mode)) {
		p = rtw_get_ie(&pnetwork->network.IEs[ie_offset], _HT_CAPABILITY_IE_, &ht_ielen, pnetwork->network.IELength - ie_offset);
		if (p && ht_ielen > 0) {
			struct rtw_ieee80211_ht_cap *pht_capie;
			ht_cap = _TRUE;
			pht_capie = (struct rtw_ieee80211_ht_cap *)(p + 2);
			_rtw_memcpy(&mcs_rate , pht_capie->supp_mcs_set, 2);
			bw_40MHz = (pht_capie->cap_info & IEEE80211_HT_CAP_SUP_WIDTH) ? 1 : 0;
			short_GI = (pht_capie->cap_info & (IEEE80211_HT_CAP_SGI_20 | IEEE80211_HT_CAP_SGI_40)) ? 1 : 0;
		}
	}
#ifdef CONFIG_80211AC_VHT
	/* parsing VHT_CAP_IE */
	if(is_supported_vht(padapter->registrypriv.wireless_mode)){
		p = rtw_get_ie(&pnetwork->network.IEs[ie_offset], EID_VHTCapability, &vht_ielen, pnetwork->network.IELength - ie_offset);
		if (p && vht_ielen > 0) {
			u8	mcs_map[2];

			vht_cap = _TRUE;
			bw_160MHz = GET_VHT_CAPABILITY_ELE_CHL_WIDTH(p + 2);
			if (bw_160MHz)
				short_GI = GET_VHT_CAPABILITY_ELE_SHORT_GI160M(p + 2);
			else
				short_GI = GET_VHT_CAPABILITY_ELE_SHORT_GI80M(p + 2);

			_rtw_memcpy(mcs_map, GET_VHT_CAPABILITY_ELE_TX_MCS(p + 2), 2);

			vht_highest_rate = rtw_get_vht_highest_rate(mcs_map);
			vht_data_rate = rtw_vht_mcs_to_data_rate(CHANNEL_WIDTH_80, short_GI, vht_highest_rate);
		}
	}
#endif

	/*Add basic and extended rates */
	p = custom;
	p += snprintf(p, MAX_CUSTOM_LEN - (p - custom), " Rates (Mb/s): ");
	while (pnetwork->network.SupportedRates[i] != 0) {
		rate = pnetwork->network.SupportedRates[i] & 0x7F;
		if (rate > max_rate)
			max_rate = rate;
		p += snprintf(p, MAX_CUSTOM_LEN - (p - custom),
			      "%d%s ", rate >> 1, (rate & 1) ? ".5" : "");
		i++;
	}
#ifdef CONFIG_80211AC_VHT
	if (vht_cap == _TRUE)
		max_rate = vht_data_rate;
	else
#endif
		if (ht_cap == _TRUE) {
			if (mcs_rate & 0x8000) /* MCS15 */
				max_rate = (bw_40MHz) ? ((short_GI) ? 300 : 270) : ((short_GI) ? 144 : 130);

			else if (mcs_rate & 0x0080) /* MCS7 */
				max_rate = (bw_40MHz) ? ((short_GI) ? 150 : 135) : ((short_GI) ? 72 : 65);
			else { /* default MCS7 */
				/* RTW_INFO("wx_get_scan, mcs_rate_bitmap=0x%x\n", mcs_rate); */
				max_rate = (bw_40MHz) ? ((short_GI) ? 150 : 135) : ((short_GI) ? 72 : 65);
			}

			max_rate = max_rate * 2; /* Mbps/2;		 */
		}

	iwe->cmd = SIOCGIWRATE;
	iwe->u.bitrate.fixed = iwe->u.bitrate.disabled = 0;
	iwe->u.bitrate.value = max_rate * 500000;
	start = iwe_stream_add_event(info, start, stop, iwe, IW_EV_PARAM_LEN);
	return start ;
}

static inline char *iwe_stream_wpa_wpa2_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	int buf_size = MAX_WPA_IE_LEN * 2;
	/* u8 pbuf[buf_size]={0};	 */
	u8 *pbuf = rtw_zmalloc(buf_size);

	u8 wpa_ie[255] = {0}, rsn_ie[255] = {0};
	u16 i, wpa_len = 0, rsn_len = 0;
	u8 *p;
	sint out_len = 0;


	if (pbuf) {
		p = pbuf;

		/* parsing WPA/WPA2 IE */
		if (pnetwork->network.Reserved[0] != BSS_TYPE_PROB_REQ) { /* Probe Request */
			out_len = rtw_get_sec_ie(pnetwork->network.IEs , pnetwork->network.IELength, rsn_ie, &rsn_len, wpa_ie, &wpa_len);

			if (wpa_len > 0) {

				_rtw_memset(pbuf, 0, buf_size);
				p += sprintf(p, "wpa_ie=");
				for (i = 0; i < wpa_len; i++)
					p += sprintf(p, "%02x", wpa_ie[i]);

				if (wpa_len > 100) {
					printk("-----------------Len %d----------------\n", wpa_len);
					for (i = 0; i < wpa_len; i++)
						printk("%02x ", wpa_ie[i]);
					printk("\n");
					printk("-----------------Len %d----------------\n", wpa_len);
				}

				_rtw_memset(iwe, 0, sizeof(*iwe));
				iwe->cmd = IWEVCUSTOM;
				iwe->u.data.length = strlen(pbuf);
				start = iwe_stream_add_point(info, start, stop, iwe, pbuf);

				_rtw_memset(iwe, 0, sizeof(*iwe));
				iwe->cmd = IWEVGENIE;
				iwe->u.data.length = wpa_len;
				start = iwe_stream_add_point(info, start, stop, iwe, wpa_ie);
			}
			if (rsn_len > 0) {

				_rtw_memset(pbuf, 0, buf_size);
				p += sprintf(p, "rsn_ie=");
				for (i = 0; i < rsn_len; i++)
					p += sprintf(p, "%02x", rsn_ie[i]);
				_rtw_memset(iwe, 0, sizeof(*iwe));
				iwe->cmd = IWEVCUSTOM;
				iwe->u.data.length = strlen(pbuf);
				start = iwe_stream_add_point(info, start, stop, iwe, pbuf);

				_rtw_memset(iwe, 0, sizeof(*iwe));
				iwe->cmd = IWEVGENIE;
				iwe->u.data.length = rsn_len;
				start = iwe_stream_add_point(info, start, stop, iwe, rsn_ie);
			}
		}

		rtw_mfree(pbuf, buf_size);
	}
	return start;
}

static inline char *iwe_stream_wps_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	/* parsing WPS IE */
	uint cnt = 0, total_ielen;
	u8 *wpsie_ptr = NULL;
	uint wps_ielen = 0;
	u8 ie_offset = (pnetwork->network.Reserved[0] == BSS_TYPE_PROB_REQ ? 0 : 12);

	u8 *ie_ptr = pnetwork->network.IEs + ie_offset;
	total_ielen = pnetwork->network.IELength - ie_offset;

	if (pnetwork->network.Reserved[0] == BSS_TYPE_PROB_REQ) { /* Probe Request */
		ie_ptr = pnetwork->network.IEs;
		total_ielen = pnetwork->network.IELength;
	} else { /* Beacon or Probe Respones */
		ie_ptr = pnetwork->network.IEs + _FIXED_IE_LENGTH_;
		total_ielen = pnetwork->network.IELength - _FIXED_IE_LENGTH_;
	}
	while (cnt < total_ielen) {
		if (rtw_is_wps_ie(&ie_ptr[cnt], &wps_ielen) && (wps_ielen > 2)) {
			wpsie_ptr = &ie_ptr[cnt];
			iwe->cmd = IWEVGENIE;
			iwe->u.data.length = (u16)wps_ielen;
			start = iwe_stream_add_point(info, start, stop, iwe, wpsie_ptr);
		}
		cnt += ie_ptr[cnt + 1] + 2; /* goto next */
	}
	return start;
}

static inline char *iwe_stream_wapi_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
#ifdef CONFIG_WAPI_SUPPORT
	char *p;

	if (pnetwork->network.Reserved[0] != BSS_TYPE_PROB_REQ) { /* Probe Request */
		sint out_len_wapi = 0;
		/* here use static for stack size */
		static u8 buf_wapi[MAX_WAPI_IE_LEN * 2] = {0};
		static u8 wapi_ie[MAX_WAPI_IE_LEN] = {0};
		u16 wapi_len = 0;
		u16  i;

		out_len_wapi = rtw_get_wapi_ie(pnetwork->network.IEs , pnetwork->network.IELength, wapi_ie, &wapi_len);

		RTW_INFO("rtw_wx_get_scan: %s ", pnetwork->network.Ssid.Ssid);
		RTW_INFO("rtw_wx_get_scan: ssid = %d ", wapi_len);


		if (wapi_len > 0) {
			p = buf_wapi;
			/* _rtw_memset(buf_wapi, 0, MAX_WAPI_IE_LEN*2); */
			p += sprintf(p, "wapi_ie=");
			for (i = 0; i < wapi_len; i++)
				p += sprintf(p, "%02x", wapi_ie[i]);

			_rtw_memset(iwe, 0, sizeof(*iwe));
			iwe->cmd = IWEVCUSTOM;
			iwe->u.data.length = strlen(buf_wapi);
			start = iwe_stream_add_point(info, start, stop, iwe, buf_wapi);

			_rtw_memset(iwe, 0, sizeof(*iwe));
			iwe->cmd = IWEVGENIE;
			iwe->u.data.length = wapi_len;
			start = iwe_stream_add_point(info, start, stop, iwe, wapi_ie);
		}
	}
#endif/* #ifdef CONFIG_WAPI_SUPPORT */
	return start;
}

static inline char   *iwe_stream_rssi_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	u8 ss, sq;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	/* Add quality statistics */
	iwe->cmd = IWEVQUAL;
	iwe->u.qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_UPDATED
#if 0 /*def CONFIG_BACKGROUND_NOISE_MONITOR*/
			      | IW_QUAL_NOISE_UPDATED
#else
			      | IW_QUAL_NOISE_INVALID
#endif
#ifdef CONFIG_SIGNAL_DISPLAY_DBM
			      | IW_QUAL_DBM
#endif
			      ;

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE &&
	    is_same_network(&pmlmepriv->dev_cur_network.network, &pnetwork->network)) {
		ss = padapter->recvinfo.signal_strength;
		sq = padapter->recvinfo.signal_qual;
	} else {
		ss = pnetwork->network.PhyInfo.SignalStrength;
		sq = pnetwork->network.PhyInfo.SignalQuality;
	}


#ifdef CONFIG_SIGNAL_DISPLAY_DBM
	iwe->u.qual.level = (u8) rtw_phl_rssi_to_dbm(ss); /* dbm */
#else
	iwe->u.qual.level = (u8)ss; /* % */
#endif

	iwe->u.qual.qual = (u8)sq;   /* signal quality */

#ifdef CONFIG_PLATFORM_ROCKCHIPS
	iwe->u.qual.noise = -100; /* noise level suggest by zhf@rockchips */
#else
	iwe->u.qual.noise = 0; /* noise level */
#endif /* CONFIG_PLATFORM_ROCKCHIPS */

	/* RTW_INFO("iqual=%d, ilevel=%d, inoise=%d, iupdated=%d\n", iwe.u.qual.qual, iwe.u.qual.level , iwe.u.qual.noise, iwe.u.qual.updated); */

	start = iwe_stream_add_event(info, start, stop, iwe, IW_EV_QUAL_LEN);
	return start;
}

static inline char   *iwe_stream_net_rsv_process(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop, struct iw_event *iwe)
{
	u8 buf[32] = {0};
	u8 *p, *pos;
	p = buf;
	pos = pnetwork->network.Reserved;

	p += sprintf(p, "fm=%02X%02X", pos[1], pos[0]);
	_rtw_memset(iwe, 0, sizeof(*iwe));
	iwe->cmd = IWEVCUSTOM;
	iwe->u.data.length = strlen(buf);
	start = iwe_stream_add_point(info, start, stop, iwe, buf);
	return start;
}

static char *translate_scan(_adapter *padapter,
		struct iw_request_info *info, struct wlan_network *pnetwork,
		char *start, char *stop)
{
	struct iw_event iwe;
	u16 cap = 0;
	_rtw_memset(&iwe, 0, sizeof(iwe));

	start = iwe_stream_mac_addr_proess(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_essid_proess(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_protocol_process(padapter, info, pnetwork, start, stop, &iwe);
	if (pnetwork->network.Reserved[0] == BSS_TYPE_PROB_REQ) /* Probe Request */
		cap = 0;
	else {
		_rtw_memcpy((u8 *)&cap, rtw_get_capability_from_ie(pnetwork->network.IEs), 2);
		cap = le16_to_cpu(cap);
	}

	start = iwe_stream_mode_process(padapter, info, pnetwork, start, stop, &iwe, cap);
	start = iwe_stream_chan_process(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_encryption_process(padapter, info, pnetwork, start, stop, &iwe, cap);
	start = iwe_stream_rate_process(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_wpa_wpa2_process(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_wps_process(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_wapi_process(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_rssi_process(padapter, info, pnetwork, start, stop, &iwe);
	start = iwe_stream_net_rsv_process(padapter, info, pnetwork, start, stop, &iwe);

	return start;
}

static int wpa_set_auth_algs(struct net_device *dev, u32 value)
{
	_adapter *padapter = (_adapter *) rtw_netdev_priv(dev);
	int ret = 0;

	if ((value & AUTH_ALG_SHARED_KEY) && (value & AUTH_ALG_OPEN_SYSTEM)) {
		RTW_INFO("wpa_set_auth_algs, AUTH_ALG_SHARED_KEY and  AUTH_ALG_OPEN_SYSTEM [value:0x%x]\n", value);
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
		padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeAutoSwitch;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
	} else if (value & AUTH_ALG_SHARED_KEY) {
		RTW_INFO("wpa_set_auth_algs, AUTH_ALG_SHARED_KEY  [value:0x%x]\n", value);
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;

#ifdef CONFIG_PLATFORM_MT53XX
		padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeAutoSwitch;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
#else
		padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeShared;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
#endif
	} else if (value & AUTH_ALG_OPEN_SYSTEM) {
		RTW_INFO("wpa_set_auth_algs, AUTH_ALG_OPEN_SYSTEM\n");
		/* padapter->securitypriv.ndisencryptstatus = Ndis802_11EncryptionDisabled; */
		if (padapter->securitypriv.ndisauthtype < Ndis802_11AuthModeWPAPSK) {
#ifdef CONFIG_PLATFORM_MT53XX
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeAutoSwitch;
			padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
#else
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeOpen;
			padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
#endif
		}

	} else if (value & AUTH_ALG_LEAP)
		RTW_INFO("wpa_set_auth_algs, AUTH_ALG_LEAP\n");
	else {
		RTW_INFO("wpa_set_auth_algs, error!\n");
		ret = -EINVAL;
	}

	return ret;

}

static int wpa_set_encryption(struct net_device *dev, struct ieee_param *param, u32 param_len)
{
	int ret = 0;
	u32 wep_key_idx, wep_key_len;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &padapter->wdinfo;
#endif /* CONFIG_P2P */
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_security_priv *lsecuritypriv = &padapter_link->securitypriv;
	struct link_mlme_priv *lmlmepriv = &padapter_link->mlmepriv;


	param->u.crypt.err = 0;
	param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

	if (param_len < (u32)((u8 *) param->u.crypt.key - (u8 *) param) + param->u.crypt.key_len) {
		ret =  -EINVAL;
		goto exit;
	}

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {

		if (param->u.crypt.idx >= WEP_KEYS
#ifdef CONFIG_IEEE80211W
		    && param->u.crypt.idx > BIP_MAX_KEYID
#endif /* CONFIG_IEEE80211W */
		   ) {
			ret = -EINVAL;
			goto exit;
		}
	} else {
#ifdef CONFIG_WAPI_SUPPORT
		if (strcmp(param->u.crypt.alg, "SMS4"))
#endif
		{
			ret = -EINVAL;
			goto exit;
		}
	}

	if (strcmp(param->u.crypt.alg, "WEP") == 0) {
		RTW_INFO("wpa_set_encryption, crypt.alg = WEP\n");

		wep_key_idx = param->u.crypt.idx;
		wep_key_len = param->u.crypt.key_len;

		if ((wep_key_idx >= WEP_KEYS) || (wep_key_len <= 0)) {
			ret = -EINVAL;
			goto exit;
		}

		if (psecuritypriv->bWepDefaultKeyIdxSet == 0) {
			/* wep default key has not been set, so use this key index as default key.*/

			wep_key_len = wep_key_len <= 5 ? 5 : 13;

			psecuritypriv->ndisencryptstatus = Ndis802_11Encryption1Enabled;
			psecuritypriv->dot11PrivacyAlgrthm = _WEP40_;
			psecuritypriv->dot118021XGrpPrivacy = _WEP40_;

			if (wep_key_len == 13) {
				psecuritypriv->dot11PrivacyAlgrthm = _WEP104_;
				psecuritypriv->dot118021XGrpPrivacy = _WEP104_;
			}

			psecuritypriv->dot11PrivacyKeyIndex = wep_key_idx;
		}

		_rtw_memcpy(&(psecuritypriv->dot11DefKey[wep_key_idx].skey[0]), param->u.crypt.key, wep_key_len);

		psecuritypriv->dot11DefKeylen[wep_key_idx] = wep_key_len;

		psecuritypriv->key_mask |= BIT(wep_key_idx);

		padapter->mlmeextpriv.mlmext_info.key_index = wep_key_idx;
		goto exit;
	}

	if (padapter->securitypriv.dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) { /* 802_1x */
		struct sta_info *psta, *pbcmc_sta;
		struct sta_priv *pstapriv = &padapter->stapriv;

		if (MLME_IS_STA(padapter) || MLME_IS_MP(padapter)) { /* sta mode */
			psta = rtw_get_stainfo(pstapriv, get_link_bssid(lmlmepriv));
			if (psta == NULL) {
				/* DEBUG_ERR( ("Set wpa_set_encryption: Obtain Sta_info fail\n")); */
			} else {
				/* Jeff: don't disable ieee8021x_blocked while clearing key */
				if (strcmp(param->u.crypt.alg, "none") != 0)
					psta->ieee8021x_blocked = _FALSE;

				if ((padapter->securitypriv.ndisencryptstatus == Ndis802_11Encryption2Enabled) ||
				    (padapter->securitypriv.ndisencryptstatus ==  Ndis802_11Encryption3Enabled))
					psta->dot118021XPrivacy = padapter->securitypriv.dot11PrivacyAlgrthm;

				if (param->u.crypt.set_tx == 1) { /* pairwise key */
					RTW_INFO(FUNC_ADPT_FMT" set %s PTK idx:%u, len:%u\n"
						, FUNC_ADPT_ARG(padapter), param->u.crypt.alg, param->u.crypt.idx, param->u.crypt.key_len);
					_rtw_memcpy(psta->dot118021x_UncstKey.skey,  param->u.crypt.key, (param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));
					if (strcmp(param->u.crypt.alg, "TKIP") == 0) { /* set mic key */
						_rtw_memcpy(psta->dot11tkiptxmickey.skey, &(param->u.crypt.key[16]), 8);
						_rtw_memcpy(psta->dot11tkiprxmickey.skey, &(param->u.crypt.key[24]), 8);
						padapter->securitypriv.busetkipkey = _FALSE;
					}
					psta->dot11txpn.val = RTW_GET_LE64(param->u.crypt.seq);
					psta->dot11rxpn.val = RTW_GET_LE64(param->u.crypt.seq);
					psta->bpairwise_key_installed = _TRUE;
					rtw_setstakey_cmd(padapter, psta, UNICAST_KEY, _TRUE);

				} else { /* group key */
					if (strcmp(param->u.crypt.alg, "TKIP") == 0 || strcmp(param->u.crypt.alg, "CCMP") == 0) {
						RTW_INFO(FUNC_ADPT_FMT" set %s GTK idx:%u, len:%u\n"
							, FUNC_ADPT_ARG(padapter), param->u.crypt.alg, param->u.crypt.idx, param->u.crypt.key_len);
						_rtw_memcpy(lsecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,  param->u.crypt.key,
							(param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));
						/* only TKIP group key need to install this */
						if (param->u.crypt.key_len > 16) {
							_rtw_memcpy(lsecuritypriv->dot118021XGrptxmickey[param->u.crypt.idx].skey, &(param->u.crypt.key[16]), 8);
							_rtw_memcpy(lsecuritypriv->dot118021XGrprxmickey[param->u.crypt.idx].skey, &(param->u.crypt.key[24]), 8);
						}
						lsecuritypriv->binstallGrpkey = _TRUE;
						if (param->u.crypt.idx < 4)
							_rtw_memcpy(lsecuritypriv->iv_seq[param->u.crypt.idx], param->u.crypt.seq, 8);
						lsecuritypriv->dot118021XGrpKeyid = param->u.crypt.idx;
						rtw_set_key(padapter, padapter_link, param->u.crypt.idx, 1, _TRUE);
					#ifdef CONFIG_IEEE80211W
					} else if (strcmp(param->u.crypt.alg, "BIP") == 0) {
						RTW_INFO(FUNC_ADPT_FMT" set IGTK idx:%u, len:%u\n"
							, FUNC_ADPT_ARG(padapter), param->u.crypt.idx, param->u.crypt.key_len);
						_rtw_memcpy(lsecuritypriv->dot11wBIPKey[param->u.crypt.idx].skey,  param->u.crypt.key,
							(param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));
						lsecuritypriv->dot11wBIPKeyid = param->u.crypt.idx;
						lsecuritypriv->dot11wBIPrxpn.val = RTW_GET_LE64(param->u.crypt.seq);
						lsecuritypriv->binstallBIPkey = _TRUE;
						rtw_set_key(padapter, padapter_link, param->u.crypt.idx, 1, _TRUE);
					#endif /* CONFIG_IEEE80211W */

					}

					/* WPA/WPA2 key-handshake has completed */
					clr_fwstate(pmlmepriv, WIFI_UNDER_KEY_HANDSHAKE);
				}
			}

			pbcmc_sta = rtw_get_bcmc_stainfo(padapter, padapter_link);
			if (pbcmc_sta == NULL) {
				/* DEBUG_ERR( ("Set OID_802_11_ADD_KEY: bcmc stainfo is null\n")); */
			} else {
				/* Jeff: don't disable ieee8021x_blocked while clearing key */
				if (strcmp(param->u.crypt.alg, "none") != 0)
					pbcmc_sta->ieee8021x_blocked = _FALSE;

				if ((padapter->securitypriv.ndisencryptstatus == Ndis802_11Encryption2Enabled) ||
				    (padapter->securitypriv.ndisencryptstatus ==  Ndis802_11Encryption3Enabled))
					pbcmc_sta->dot118021XPrivacy = padapter->securitypriv.dot11PrivacyAlgrthm;
			}
		} else if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)) { /* adhoc mode */
		}
	}

#ifdef CONFIG_WAPI_SUPPORT
	if (strcmp(param->u.crypt.alg, "SMS4") == 0)
		rtw_wapi_set_set_encryption(padapter, param);
#endif

exit:


	return ret;
}

static int rtw_set_wpa_ie(_adapter *padapter, char *pie, unsigned short ielen)
{
	u8 *buf = NULL, *pos = NULL;
	int group_cipher = 0, pairwise_cipher = 0;
	u8 mfp_opt = MFP_NO;
	int ret = 0;
	u8 null_addr[] = {0, 0, 0, 0, 0, 0};
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &padapter->wdinfo;
#endif /* CONFIG_P2P */

	if ((ielen > MAX_WPA_IE_LEN) || (pie == NULL)) {
		_clr_fwstate_(&padapter->mlmepriv, WIFI_UNDER_WPS);
		if (pie == NULL)
			return ret;
		else
			return -EINVAL;
	}

	if (ielen) {
		buf = rtw_zmalloc(ielen);
		if (buf == NULL) {
			ret =  -ENOMEM;
			goto exit;
		}

		_rtw_memcpy(buf, pie , ielen);

		/* dump */
		{
			int i;
			RTW_INFO("\n wpa_ie(length:%d):\n", ielen);
			for (i = 0; i < ielen; i = i + 8)
				RTW_INFO("0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x\n", buf[i], buf[i + 1], buf[i + 2], buf[i + 3], buf[i + 4], buf[i + 5], buf[i + 6], buf[i + 7]);
		}

		pos = buf;
		if (ielen < RSN_HEADER_LEN) {
			ret  = -1;
			goto exit;
		}

		if (rtw_parse_wpa_ie(buf, ielen, &group_cipher, &pairwise_cipher, NULL) == _SUCCESS) {
			padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeWPAPSK;
			_rtw_memcpy(padapter->securitypriv.supplicant_ie, &buf[0], ielen);
		}

		if (rtw_parse_wpa2_ie(buf, ielen, &group_cipher, &pairwise_cipher, NULL, NULL, &mfp_opt, NULL) == _SUCCESS) {
			padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeWPA2PSK;
			_rtw_memcpy(padapter->securitypriv.supplicant_ie, &buf[0], ielen);
		}

		if (group_cipher == 0)
			group_cipher = WPA_CIPHER_NONE;
		if (pairwise_cipher == 0)
			pairwise_cipher = WPA_CIPHER_NONE;

		switch (group_cipher) {
		case WPA_CIPHER_NONE:
			padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11EncryptionDisabled;
			break;
		case WPA_CIPHER_WEP40:
			padapter->securitypriv.dot118021XGrpPrivacy = _WEP40_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
			break;
		case WPA_CIPHER_TKIP:
			padapter->securitypriv.dot118021XGrpPrivacy = _TKIP_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption2Enabled;
			break;
		case WPA_CIPHER_CCMP:
			padapter->securitypriv.dot118021XGrpPrivacy = _AES_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption3Enabled;
			break;
		case WPA_CIPHER_WEP104:
			padapter->securitypriv.dot118021XGrpPrivacy = _WEP104_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
			break;
		}

		switch (pairwise_cipher) {
		case WPA_CIPHER_NONE:
			padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11EncryptionDisabled;
			break;
		case WPA_CIPHER_WEP40:
			padapter->securitypriv.dot11PrivacyAlgrthm = _WEP40_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
			break;
		case WPA_CIPHER_TKIP:
			padapter->securitypriv.dot11PrivacyAlgrthm = _TKIP_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption2Enabled;
			break;
		case WPA_CIPHER_CCMP:
			padapter->securitypriv.dot11PrivacyAlgrthm = _AES_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption3Enabled;
			break;
		case WPA_CIPHER_WEP104:
			padapter->securitypriv.dot11PrivacyAlgrthm = _WEP104_;
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
			break;
		}

		if (mfp_opt == MFP_INVALID) {
			RTW_INFO(FUNC_ADPT_FMT" invalid MFP setting\n", FUNC_ADPT_ARG(padapter));
			ret = -EINVAL;
			goto exit;
		}
		padapter->securitypriv.mfp_opt = mfp_opt;

		_clr_fwstate_(&padapter->mlmepriv, WIFI_UNDER_WPS);
		{/* set wps_ie	 */
			u16 cnt = 0;
			u8 eid, wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};

			while (cnt < ielen) {
				eid = buf[cnt];

				if ((eid == _VENDOR_SPECIFIC_IE_) && (_rtw_memcmp(&buf[cnt + 2], wps_oui, 4) == _TRUE)) {
					RTW_INFO("SET WPS_IE\n");

					padapter->securitypriv.wps_ie_len = ((buf[cnt + 1] + 2) < MAX_WPS_IE_LEN) ? (buf[cnt + 1] + 2) : MAX_WPS_IE_LEN;

					_rtw_memcpy(padapter->securitypriv.wps_ie, &buf[cnt], padapter->securitypriv.wps_ie_len);

					set_fwstate(&padapter->mlmepriv, WIFI_UNDER_WPS);

					cnt += buf[cnt + 1] + 2;

					break;
				} else {
					cnt += buf[cnt + 1] + 2; /* goto next	 */
				}
			}
		}

		#ifdef CONFIG_RTW_MULTI_AP
		padapter->multi_ap = rtw_get_multi_ap_ie_ext(buf, ielen) & MULTI_AP_BACKHAUL_STA;
		if (padapter->multi_ap)
			adapter_set_use_wds(padapter, 1);
		#endif
	}

	/* TKIP and AES disallow multicast packets until installing group key */
	if (padapter->securitypriv.dot11PrivacyAlgrthm == _TKIP_
	    || padapter->securitypriv.dot11PrivacyAlgrthm == _TKIP_WTMIC_
	    || padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)
		/* WPS open need to enable multicast
		 * || check_fwstate(&padapter->mlmepriv, WIFI_UNDER_WPS) == _TRUE) */
		rtw_hal_set_hwreg(padapter, HW_VAR_OFF_RCR_AM, null_addr);


exit:

	if (buf)
		rtw_mfree(buf, ielen);

	return ret;
}

static int rtw_wx_get_name(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u32 ht_ielen = 0;
	char *p;
	u8 ht_cap = _FALSE, vht_cap = _FALSE, he_cap = _FALSE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct	link_mlme_priv	*pmlmepriv = &(padapter_link->mlmepriv);
	WLAN_BSSID_EX  *pcur_bss = &pmlmepriv->cur_network.network;
	NDIS_802_11_RATES_EX *prates = NULL;



	if (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE | WIFI_ADHOC_MASTER_STATE) == _TRUE) {
		/* parsing HT_CAP_IE */
		if( is_supported_ht(padapter->registrypriv.wireless_mode)&&(padapter->registrypriv.ht_enable)) {
			p = rtw_get_ie(&pcur_bss->IEs[12], _HT_CAPABILITY_IE_, &ht_ielen, pcur_bss->IELength - 12);
			if (p && ht_ielen > 0 )
				ht_cap = _TRUE;
		}
#ifdef CONFIG_80211AC_VHT
		if (is_supported_vht(padapter->registrypriv.wireless_mode) &&
			(pmlmepriv->vhtpriv.vht_option == _TRUE))
			vht_cap = _TRUE;
#endif
#ifdef CONFIG_80211AX_HE
		if (is_supported_he(padapter->registrypriv.wireless_mode) &&
			(pmlmepriv->hepriv.he_option == _TRUE))
			he_cap = _TRUE;
#endif

		prates = &pcur_bss->SupportedRates;
#ifdef CONFIG_80211AX_HE
		if (he_cap == _TRUE)
			snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11AX");
		else
#endif
		if (rtw_is_cckratesonly_included((u8 *)prates) == _TRUE) {
			if (ht_cap == _TRUE)
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bn");
			else
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11b");
		} else if ((rtw_is_cckrates_included((u8 *)prates)) == _TRUE) {
			if (ht_cap == _TRUE)
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bgn");
			else {
				if(padapter->registrypriv.wireless_mode & WLAN_MD_11G)
					snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bg");
				else
					snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11b");
			}
		} else {
			if (pcur_bss->Configuration.DSConfig > 14) {
#ifdef CONFIG_80211AC_VHT
				if (vht_cap == _TRUE)
					snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11AC");
				else
#endif
				{
					if (ht_cap == _TRUE)
						snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11an");
					else
						snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11a");
				}
			} else {
				if (ht_cap == _TRUE)
					snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11gn");
				else
					snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11g");
			}
		}
	} else {
		/* prates = &padapter->registrypriv.dev_network.SupportedRates; */
		/* snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11g"); */
		snprintf(wrqu->name, IFNAMSIZ, "unassociated");
	}


	return 0;
}

static int rtw_wx_set_freq(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	int exp = 1, freq = 0, div = 0;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (wrqu->freq.m <= 1000) {
		if (wrqu->freq.flags == IW_FREQ_AUTO) {
			if (rtw_chset_search_ch(adapter_to_chset(padapter), wrqu->freq.m) > 0) {
				padapter_link->mlmeextpriv.chandef.chan = wrqu->freq.m;
				RTW_INFO("%s: channel is auto, set to channel %d\n", __func__, wrqu->freq.m);
			} else {
				padapter_link->mlmeextpriv.chandef.chan = 1;
				RTW_INFO("%s: channel is auto, Channel Plan don't match just set to channel 1\n", __func__);
			}
		} else {
			padapter_link->mlmeextpriv.chandef.chan = wrqu->freq.m;
			RTW_INFO("%s: set to channel %d\n", __func__, padapter_link->mlmeextpriv.chandef.chan);
		}
	} else {
		while (wrqu->freq.e) {
			exp *= 10;
			wrqu->freq.e--;
		}

		freq = wrqu->freq.m;

		while (!(freq % 10)) {
			freq /= 10;
			exp *= 10;
		}

		/* freq unit is MHz here */
		div = 1000000 / exp;

		if (div)
			freq /= div;
		else {
			div = exp / 1000000;
			freq *= div;
		}

		/* If freq is invalid, rtw_freq2ch() will return channel 1 */
		padapter_link->mlmeextpriv.chandef.band = rtw_freq2band(freq);
		padapter_link->mlmeextpriv.chandef.chan = rtw_freq2ch(freq);
		RTW_INFO("%s: set to channel %d\n", __func__, padapter_link->mlmeextpriv.chandef.chan);
	}
	set_bch_bwmode(padapter,
				padapter_link,
				padapter_link->mlmeextpriv.chandef.band,
				padapter_link->mlmeextpriv.chandef.chan,
				CHAN_OFFSET_NO_EXT,
				CHANNEL_WIDTH_20,
				_FALSE);

	return 0;
}

static int rtw_wx_get_freq(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	WLAN_BSSID_EX  *pcur_bss = &padapter_link->mlmepriv.cur_network.network;

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE && check_fwstate(pmlmepriv, WIFI_MONITOR_STATE) != _TRUE) {
		wrqu->freq.m = rtw_bch2freq(pcur_bss->Configuration.Band, pcur_bss->Configuration.DSConfig) * 100000;
		wrqu->freq.e = 1;
		wrqu->freq.i = pcur_bss->Configuration.DSConfig;

	} else {
		wrqu->freq.m = rtw_bch2freq(padapter_link->mlmeextpriv.chandef.band, padapter_link->mlmeextpriv.chandef.chan) * 100000;
		wrqu->freq.e = 1;
		wrqu->freq.i = padapter_link->mlmeextpriv.chandef.chan;
	}

	return 0;
}

u32 wext_mode_to_rtw_mlme_state(union iwreq_data *wrqu)
{
	switch (wrqu->mode) {
	#ifdef CONFIG_WIFI_MONITOR
	case IW_MODE_MONITOR:
		return WIFI_MONITOR_STATE;
	#endif
	case IW_MODE_ADHOC:
		return WIFI_ADHOC_STATE;
	#ifdef CONFIG_AP_MODE
	case IW_MODE_MASTER:
		return WIFI_AP_STATE;
	#endif
	case IW_MODE_INFRA:
		return WIFI_STATION_STATE;

	#ifdef CONFIG_RTW_MESH
	case IW_MODE_MESH:
		return WIFI_MESH_STATE;
	#endif

	case IW_MODE_AUTO:
	default:
		return WIFI_STATION_STATE;
	}
}

static int rtw_wx_set_mode(struct net_device *dev, struct iw_request_info *a,
			   union iwreq_data *wrqu, char *b)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	NDIS_802_11_NETWORK_INFRASTRUCTURE networkType ;
	int ret = 0;

	if (!rtw_hw_is_init_completed(adapter_to_dvobj(padapter))) {
		ret = -EPERM;
		goto exit;
	}

	/* initial default type */
	dev->type = ARPHRD_ETHER;

	switch (wrqu->mode) {
#ifdef CONFIG_WIFI_MONITOR
	case IW_MODE_MONITOR:
		networkType = Ndis802_11Monitor;
#if 0
		dev->type = ARPHRD_IEEE80211; /* IEEE 802.11 : 801 */
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
		dev->type = ARPHRD_IEEE80211_RADIOTAP; /* IEEE 802.11 + radiotap header : 803 */
		RTW_INFO("set_mode = IW_MODE_MONITOR\n");
#else
		RTW_INFO("kernel version < 2.6.24 not support IW_MODE_MONITOR\n");
#endif
		break;
#endif /* CONFIG_WIFI_MONITOR */
	case IW_MODE_AUTO:
		networkType = Ndis802_11AutoUnknown;
		RTW_INFO("set_mode = IW_MODE_AUTO\n");
		break;
	case IW_MODE_ADHOC:
		networkType = Ndis802_11IBSS;
		RTW_INFO("set_mode = IW_MODE_ADHOC\n");
		break;
	case IW_MODE_MASTER:
		networkType = Ndis802_11APMode;
		RTW_INFO("set_mode = IW_MODE_MASTER\n");
		break;
	case IW_MODE_INFRA:
		networkType = Ndis802_11Infrastructure;
		RTW_INFO("set_mode = IW_MODE_INFRA\n");
		break;

	default:
		ret = -EINVAL;;
		goto exit;
	}

	if (rtw_set_802_11_infrastructure_mode(padapter, networkType, 0) == _FALSE) {

		ret = -EPERM;
		goto exit;

	}

	rtw_setopmode_cmd(padapter, networkType, RTW_CMDF_WAIT_ACK);

	if (check_fwstate(pmlmepriv, WIFI_MONITOR_STATE) == _TRUE)
		rtw_indicate_connect(padapter);

exit:


	return ret;

}

static int rtw_wx_get_mode(struct net_device *dev, struct iw_request_info *a,
			   union iwreq_data *wrqu, char *b)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct	mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	if (MLME_IS_STA(padapter))
		wrqu->mode = IW_MODE_INFRA;
	else if (MLME_IS_ADHOC(padapter) || MLME_IS_ADHOC_MASTER(padapter))
		wrqu->mode = IW_MODE_ADHOC;
	else if (MLME_IS_AP(padapter))
		wrqu->mode = IW_MODE_MASTER;
	else if (MLME_IS_MONITOR(padapter))
		wrqu->mode = IW_MODE_MONITOR;
	else
		wrqu->mode = IW_MODE_AUTO;


	return 0;

}


static int rtw_wx_set_pmkid(struct net_device *dev,
			    struct iw_request_info *a,
			    union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8          j, blInserted = _FALSE;
	int         intReturn = _FALSE;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct iw_pmksa  *pPMK = (struct iw_pmksa *) extra;
	u8     strZeroMacAddress[ETH_ALEN] = { 0x00 };
	u8     strIssueBssid[ETH_ALEN] = { 0x00 };

#if 0
	struct iw_pmksa {
		__u32   cmd;
		struct sockaddr bssid;
		__u8    pmkid[IW_PMKID_LEN];   /* IW_PMKID_LEN=16 */
	}
	There are the BSSID information in the bssid.sa_data array.
	If cmd is IW_PMKSA_FLUSH, it means the wpa_suppplicant wants to clear all the PMKID information.
	If cmd is IW_PMKSA_ADD, it means the wpa_supplicant wants to add a PMKID / BSSID to driver.
	If cmd is IW_PMKSA_REMOVE, it means the wpa_supplicant wants to remove a PMKID / BSSID from driver.
#endif

	_rtw_memcpy(strIssueBssid, pPMK->bssid.sa_data, ETH_ALEN);
	if (pPMK->cmd == IW_PMKSA_ADD) {
		RTW_INFO("[rtw_wx_set_pmkid] IW_PMKSA_ADD!\n");
		if (_rtw_memcmp(strIssueBssid, strZeroMacAddress, ETH_ALEN) == _TRUE)
			return intReturn ;
		else
			intReturn = _TRUE;
		blInserted = _FALSE;

		/* overwrite PMKID */
		for (j = 0 ; j < NUM_PMKID_CACHE; j++) {
			if (_rtw_memcmp(psecuritypriv->PMKIDList[j].Bssid, strIssueBssid, ETH_ALEN) == _TRUE) {
				/* BSSID is matched, the same AP => rewrite with new PMKID. */

				RTW_INFO("[rtw_wx_set_pmkid] BSSID exists in the PMKList.\n");

				_rtw_memcpy(psecuritypriv->PMKIDList[j].PMKID, pPMK->pmkid, IW_PMKID_LEN);
				psecuritypriv->PMKIDList[j].bUsed = _TRUE;
				blInserted = _TRUE;
				break;
			}
		}

		if (!blInserted) {
			/* Find a new entry */
			RTW_INFO("[rtw_wx_set_pmkid] Use the new entry index = %d for this PMKID.\n",
				 psecuritypriv->PMKIDIndex);

			_rtw_memcpy(psecuritypriv->PMKIDList[psecuritypriv->PMKIDIndex].Bssid, strIssueBssid, ETH_ALEN);
			_rtw_memcpy(psecuritypriv->PMKIDList[psecuritypriv->PMKIDIndex].PMKID, pPMK->pmkid, IW_PMKID_LEN);

			psecuritypriv->PMKIDList[psecuritypriv->PMKIDIndex].bUsed = _TRUE;
			psecuritypriv->PMKIDIndex++ ;
			if (psecuritypriv->PMKIDIndex == 16)
				psecuritypriv->PMKIDIndex = 0;
		}
	} else if (pPMK->cmd == IW_PMKSA_REMOVE) {
		RTW_INFO("[rtw_wx_set_pmkid] IW_PMKSA_REMOVE!\n");
		intReturn = _TRUE;
		for (j = 0 ; j < NUM_PMKID_CACHE; j++) {
			if (_rtw_memcmp(psecuritypriv->PMKIDList[j].Bssid, strIssueBssid, ETH_ALEN) == _TRUE) {
				/* BSSID is matched, the same AP => Remove this PMKID information and reset it. */
				_rtw_memset(psecuritypriv->PMKIDList[j].Bssid, 0x00, ETH_ALEN);
				psecuritypriv->PMKIDList[j].bUsed = _FALSE;
				break;
			}
		}
	} else if (pPMK->cmd == IW_PMKSA_FLUSH) {
		RTW_INFO("[rtw_wx_set_pmkid] IW_PMKSA_FLUSH!\n");
		_rtw_memset(&psecuritypriv->PMKIDList[0], 0x00, sizeof(RT_PMKID_LIST) * NUM_PMKID_CACHE);
		psecuritypriv->PMKIDIndex = 0;
		intReturn = _TRUE;
	}
	return intReturn ;
}

static int rtw_wx_get_sens(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
#ifdef CONFIG_PLATFORM_ROCKCHIPS
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	/*
	*  20110311 Commented by Jeff
	*  For rockchip platform's wpa_driver_wext_get_rssi
	*/
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
		/* wrqu->sens.value=-padapter->recvinfo.signal_strength; */
		wrqu->sens.value = -padapter->recvinfo.rssi;
		/* RTW_INFO("%s: %d\n", __FUNCTION__, wrqu->sens.value); */
		wrqu->sens.fixed = 0; /* no auto select */
	} else
#endif
	{
		wrqu->sens.value = 0;
		wrqu->sens.fixed = 0;	/* no auto select */
		wrqu->sens.disabled = 1;
	}
	return 0;
}

static int rtw_wx_get_range(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	struct iw_range *range = (struct iw_range *)extra;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rtw_chset *chset = adapter_to_chset(padapter);
	u16 val;
	int i;



	wrqu->data.length = sizeof(*range);
	_rtw_memset(range, 0, sizeof(*range));

	/* Let's try to keep this struct in the same order as in
	 * linux/include/wireless.h
	 */

	/* TODO: See what values we can set, and remove the ones we can't
	 * set, or fill them with some default data.
	 */

	/* ~5 Mb/s real (802.11b) */
	range->throughput = 5 * 1000 * 1000;

	/* TODO: Not used in 802.11b?
	*	range->min_nwid;	 Minimal NWID we are able to set  */
	/* TODO: Not used in 802.11b?
	*	range->max_nwid;	 Maximal NWID we are able to set  */

	/* Old Frequency (backward compat - moved lower ) */
	/*	range->old_num_channels;
	 *	range->old_num_frequency;
	 * 	range->old_freq[6];  Filler to keep "version" at the same offset  */

	/* signal level threshold range */

	/* Quality of link & SNR stuff */
	/* Quality range (link, level, noise)
	 * If the quality is absolute, it will be in the range [0 ; max_qual],
	 * if the quality is dBm, it will be in the range [max_qual ; 0].
	 * Don't forget that we use 8 bit arithmetics...
	 *
	 * If percentage range is 0~100
	 * Signal strength dbm range logical is -100 ~ 0
	 * but usually value is -90 ~ -20
	 */
	range->max_qual.qual = 100;
#ifdef CONFIG_SIGNAL_DISPLAY_DBM
	range->max_qual.level = (u8)-100;
	range->max_qual.noise = (u8)-100;
	range->max_qual.updated = IW_QUAL_ALL_UPDATED; /* Updated all three */
	range->max_qual.updated |= IW_QUAL_DBM;
#else /* !CONFIG_SIGNAL_DISPLAY_DBM */
	/* percent values between 0 and 100. */
	range->max_qual.level = 100;
	range->max_qual.noise = 100;
	range->max_qual.updated = IW_QUAL_ALL_UPDATED; /* Updated all three */
#endif /* !CONFIG_SIGNAL_DISPLAY_DBM */

	/* This should contain the average/typical values of the quality
	 * indicator. This should be the threshold between a "good" and
	 * a "bad" link (example : monitor going from green to orange).
	 * Currently, user space apps like quality monitors don't have any
	 * way to calibrate the measurement. With this, they can split
	 * the range between 0 and max_qual in different quality level
	 * (using a geometric subdivision centered on the average).
	 * I expect that people doing the user space apps will feedback
	 * us on which value we need to put in each driver... */
	range->avg_qual.qual = 92; /* > 8% missed beacons is 'bad' */
#ifdef CONFIG_SIGNAL_DISPLAY_DBM
	/* TODO: Find real 'good' to 'bad' threshold value for RSSI */
	range->avg_qual.level = (u8)-70;
	range->avg_qual.noise = 0;
	range->avg_qual.updated = IW_QUAL_ALL_UPDATED; /* Updated all three */
	range->avg_qual.updated |= IW_QUAL_DBM;
#else /* !CONFIG_SIGNAL_DISPLAY_DBM */
	/* TODO: Find real 'good' to 'bad' threshol value for RSSI */
	range->avg_qual.level = 30;
	range->avg_qual.noise = 100;
	range->avg_qual.updated = IW_QUAL_ALL_UPDATED; /* Updated all three */
#endif /* !CONFIG_SIGNAL_DISPLAY_DBM */

	range->num_bitrates = RATE_COUNT;

	for (i = 0; i < RATE_COUNT && i < IW_MAX_BITRATES; i++)
		range->bitrate[i] = rtw_rates[i];

	range->min_frag = MIN_FRAG_THRESHOLD;
	range->max_frag = MAX_FRAG_THRESHOLD;

	range->pm_capa = 0;

	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 16;

	/*	range->retry_capa;	 What retry options are supported
	 *	range->retry_flags;	 How to decode max/min retry limit
	 *	range->r_time_flags;	 How to decode max/min retry life
	 *	range->min_retry;	 Minimal number of retries
	 *	range->max_retry;	 Maximal number of retries
	 *	range->min_r_time;	 Minimal retry lifetime
	 *	range->max_r_time;	 Maximal retry lifetime  */

	for (i = 0, val = 0; i < chset->chs_len; i++) {

		/* Include only legal frequencies for some countries */
		if (chset->chs[i].flags & RTW_CHF_DIS)
			continue;

		range->freq[val].i = chset->chs[i].ChannelNum;
		range->freq[val].m = rtw_ch2freq(chset->chs[i].ChannelNum) * 100000;
		range->freq[val].e = 1;
		val++;

		if (val == IW_MAX_FREQUENCIES)
			break;
	}

	range->num_channels = val;
	range->num_frequency = val;

	/* Commented by Albert 2009/10/13
	 * The following code will proivde the security capability to network manager.
	 * If the driver doesn't provide this capability to network manager,
	 * the WPA/WPA2 routers can't be choosen in the network manager. */

	/*
	#define IW_SCAN_CAPA_NONE		0x00
	#define IW_SCAN_CAPA_ESSID		0x01
	#define IW_SCAN_CAPA_BSSID		0x02
	#define IW_SCAN_CAPA_CHANNEL	0x04
	#define IW_SCAN_CAPA_MODE		0x08
	#define IW_SCAN_CAPA_RATE		0x10
	#define IW_SCAN_CAPA_TYPE		0x20
	#define IW_SCAN_CAPA_TIME		0x40
	*/

#if WIRELESS_EXT > 17
	range->enc_capa = IW_ENC_CAPA_WPA | IW_ENC_CAPA_WPA2 |
			  IW_ENC_CAPA_CIPHER_TKIP | IW_ENC_CAPA_CIPHER_CCMP;
#endif

#ifdef IW_SCAN_CAPA_ESSID /* WIRELESS_EXT > 21 */
	range->scan_capa = IW_SCAN_CAPA_ESSID | IW_SCAN_CAPA_TYPE | IW_SCAN_CAPA_BSSID |
		   IW_SCAN_CAPA_CHANNEL | IW_SCAN_CAPA_MODE | IW_SCAN_CAPA_RATE;
#endif



	return 0;

}

/* set bssid flow
 * s1. rtw_set_802_11_infrastructure_mode()
 * s2. rtw_set_802_11_authentication_mode()
 * s3. set_802_11_encryption_mode()
 * s4. rtw_set_802_11_bssid() */
static int rtw_wx_set_wap(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *awrq,
			  char *extra)
{
	uint ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct sockaddr *temp = (struct sockaddr *)awrq;
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	_list	*phead;
	u8 *dst_bssid, *src_bssid;
	_queue	*queue	= &(pmlmepriv->scanned_queue);
	struct	wlan_network	*pnetwork = NULL;
	NDIS_802_11_AUTHENTICATION_MODE	authmode;

	/*
	#ifdef CONFIG_CONCURRENT_MODE
		if(padapter->adapter_type > PRIMARY_IFACE)
		{
			ret = -EINVAL;
			goto exit;
		}
	#endif
	*/

#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter, WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING) == _TRUE) {
		RTW_INFO("set bssid, but buddy_intf is under scanning or linking\n");
		ret = -EINVAL;
		goto exit;
	}
#endif
	if (!padapter->netif_up) {
		ret = -1;
		goto exit;
	}


	if (temp->sa_family != ARPHRD_ETHER) {
		ret = -EINVAL;
		goto exit;
	}

	authmode = padapter->securitypriv.ndisauthtype;
	_rtw_spinlock_bh(&queue->lock);
	phead = get_list_head(queue);
	pmlmepriv->pscanned = get_next(phead);

	while (1) {

		if ((rtw_end_of_queue_search(phead, pmlmepriv->pscanned)) == _TRUE) {
#if 0
			ret = -EINVAL;
			goto exit;

			if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) {
				rtw_set_802_11_bssid(padapter, temp->sa_data);
				goto exit;
			} else {
				ret = -EINVAL;
				goto exit;
			}
#endif

			break;
		}

		pnetwork = LIST_CONTAINOR(pmlmepriv->pscanned, struct wlan_network, list);

		pmlmepriv->pscanned = get_next(pmlmepriv->pscanned);

		dst_bssid = pnetwork->network.MacAddress;

		src_bssid = temp->sa_data;

		if ((_rtw_memcmp(dst_bssid, src_bssid, ETH_ALEN)) == _TRUE) {
			if (!rtw_set_802_11_infrastructure_mode(padapter, pnetwork->network.InfrastructureMode, 0)) {
				ret = -1;
				_rtw_spinunlock_bh(&queue->lock);
				goto exit;
			}

			break;
		}

	}
	_rtw_spinunlock_bh(&queue->lock);

	rtw_set_802_11_authentication_mode(padapter, authmode);
	/* set_802_11_encryption_mode(padapter, padapter->securitypriv.ndisencryptstatus); */
	if (rtw_set_802_11_bssid(padapter, temp->sa_data) == _FALSE) {
		ret = -1;
	}

exit:
	return ret;
}

static int rtw_wx_get_wap(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	WLAN_BSSID_EX  *pcur_bss = &pmlmepriv->dev_cur_network.network;

	wrqu->ap_addr.sa_family = ARPHRD_ETHER;

	_rtw_memset(wrqu->ap_addr.sa_data, 0, ETH_ALEN);



	if (((check_fwstate(pmlmepriv, WIFI_ASOC_STATE)) == _TRUE) ||
	    ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) == _TRUE) ||
	    ((check_fwstate(pmlmepriv, WIFI_AP_STATE)) == _TRUE))

		_rtw_memcpy(wrqu->ap_addr.sa_data, pcur_bss->MacAddress, ETH_ALEN);
	else
		_rtw_memset(wrqu->ap_addr.sa_data, 0, ETH_ALEN);


	return 0;

}

static int rtw_wx_set_mlme(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
#if 0
	/* SIOCSIWMLME data */
	struct	iw_mlme {
		__u16		cmd; /* IW_MLME_* */
		__u16		reason_code;
		struct sockaddr	addr;
	};
#endif

	int ret = 0;
	u16 reason;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct iw_mlme *mlme = (struct iw_mlme *) extra;


	if (mlme == NULL)
		return -1;

	RTW_INFO("%s\n", __FUNCTION__);

	reason = cpu_to_le16(mlme->reason_code);


	RTW_INFO("%s, cmd=%d, reason=%d\n", __FUNCTION__, mlme->cmd, reason);


	switch (mlme->cmd) {
	case IW_MLME_DEAUTH:
		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;
		break;

	case IW_MLME_DISASSOC:
		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;

		break;

	default:
		return -EOPNOTSUPP;
	}
	return ret;
}

static int rtw_wx_set_scan(struct net_device *dev, struct iw_request_info *a,
			   union iwreq_data *wrqu, char *extra)
{
	u8 _status = _FALSE;
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/*struct mlme_priv *pmlmepriv = &padapter->mlmepriv;*/
	struct sitesurvey_parm *parm = NULL;
	u8 ssc_chk;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P */

#ifdef DBG_IOCTL
	RTW_INFO("DBG_IOCTL %s:%d\n", __FUNCTION__, __LINE__);
#endif

#if 1
	ssc_chk = rtw_sitesurvey_condition_check(padapter, _FALSE);

	#ifdef CONFIG_DOSCAN_IN_BUSYTRAFFIC
	if ((ssc_chk != SS_ALLOW) && (ssc_chk != SS_DENY_BUSY_TRAFFIC))
	#else
	/* When Busy Traffic, driver do not site survey. So driver return success. */
	/* wpa_supplicant will not issue SIOCSIWSCAN cmd again after scan timeout. */
	/* modify by thomas 2011-02-22. */
	if (ssc_chk != SS_ALLOW)
	#endif
	{
		if (ssc_chk == SS_DENY_MP_MODE)
			ret = -EPERM;
		#ifdef DBG_LA_MODE
		else if (ssc_chk == SS_DENY_LA_MODE)
			ret = -EPERM;
		#endif
		else
			indicate_wx_scan_complete_event(padapter);

		goto exit;
	} else
		RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	if (!rtw_is_adapter_up(padapter)) {
		ret = -1;
		goto exit;
	}
#else

#ifdef CONFIG_MP_INCLUDED
	if (rtw_mp_mode_check(padapter)) {
		RTW_INFO("MP mode block Scan request\n");
		ret = -EPERM;
		goto exit;
	}
#endif
	if (rtw_is_scan_deny(padapter)) {
		indicate_wx_scan_complete_event(padapter);
		goto exit;
	}

	if (!rtw_is_adapter_up(padapter)) {
		ret = -1;
		goto exit;
	}

#ifndef CONFIG_DOSCAN_IN_BUSYTRAFFIC
	/* When Busy Traffic, driver do not site survey. So driver return success. */
	/* wpa_supplicant will not issue SIOCSIWSCAN cmd again after scan timeout. */
	/* modify by thomas 2011-02-22. */
	if (rtw_mi_busy_traffic_check(padapter)) {
		indicate_wx_scan_complete_event(padapter);
		goto exit;
	}
#endif
	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) && check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
		RTW_INFO("AP mode process WPS\n");
		indicate_wx_scan_complete_event(padapter);
		goto exit;
	}

	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING) == _TRUE) {
		indicate_wx_scan_complete_event(padapter);
		goto exit;
	}

#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter,
		       WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING | WIFI_UNDER_WPS)) {

		indicate_wx_scan_complete_event(padapter);
		goto exit;
	}
#endif
#endif

	parm = rtw_malloc(sizeof(*parm));
	if (parm == NULL) {
		ret = -ENOMEM;
		goto exit;
	}

#if WIRELESS_EXT >= 17
	if (wrqu->data.length == sizeof(struct iw_scan_req)) {
		struct iw_scan_req *req = (struct iw_scan_req *)extra;

		if (wrqu->data.flags & IW_SCAN_THIS_ESSID) {
			int len = min((int)req->essid_len, IW_ESSID_MAX_SIZE);

			rtw_init_sitesurvey_parm(padapter, parm);
			_rtw_memcpy(&parm->ssid[0].Ssid, &req->essid, len);
			parm->ssid[0].SsidLength = len;
			parm->ssid_num = 1;

			RTW_INFO("IW_SCAN_THIS_ESSID, ssid=%s, len=%d\n", req->essid, req->essid_len);

			_status = rtw_sitesurvey_cmd(padapter, parm);

		} else if (req->scan_type == IW_SCAN_TYPE_PASSIVE)
			RTW_INFO("rtw_wx_set_scan, req->scan_type == IW_SCAN_TYPE_PASSIVE\n");

	} else
#endif

		if (wrqu->data.length >= WEXT_CSCAN_HEADER_SIZE
		    && _rtw_memcmp(extra, WEXT_CSCAN_HEADER, WEXT_CSCAN_HEADER_SIZE) == _TRUE
		   ) {
			int len = wrqu->data.length - WEXT_CSCAN_HEADER_SIZE;
			char *pos = extra + WEXT_CSCAN_HEADER_SIZE;
			char section;
			char sec_len;
			int ssid_index = 0;

			/* RTW_INFO("%s COMBO_SCAN header is recognized\n", __FUNCTION__); */
			rtw_init_sitesurvey_parm(padapter, parm);

			while (len >= 1) {
				section = *(pos++);
				len -= 1;

				switch (section) {
				case WEXT_CSCAN_SSID_SECTION:
					/* RTW_INFO("WEXT_CSCAN_SSID_SECTION\n"); */
					if (len < 1) {
						len = 0;
						break;
					}

					sec_len = *(pos++);
					len -= 1;

					if (sec_len > 0 && sec_len <= len) {

						parm->ssid[ssid_index].SsidLength = sec_len;
						_rtw_memcpy(&parm->ssid[ssid_index].Ssid, pos, sec_len);

						/* RTW_INFO("%s COMBO_SCAN with specific parm->ssid:%s, %d\n", __FUNCTION__ */
						/*	, parm->ssid[ssid_index].Ssid, parm->ssid[ssid_index].SsidLength); */
						ssid_index++;
					}

					pos += sec_len;
					len -= sec_len;
					break;


				case WEXT_CSCAN_CHANNEL_SECTION:
					/* RTW_INFO("WEXT_CSCAN_CHANNEL_SECTION\n"); */
					pos += 1;
					len -= 1;
					break;
				case WEXT_CSCAN_ACTV_DWELL_SECTION:
					/* RTW_INFO("WEXT_CSCAN_ACTV_DWELL_SECTION\n"); */
					pos += 2;
					len -= 2;
					break;
				case WEXT_CSCAN_PASV_DWELL_SECTION:
					/* RTW_INFO("WEXT_CSCAN_PASV_DWELL_SECTION\n"); */
					pos += 2;
					len -= 2;
					break;
				case WEXT_CSCAN_HOME_DWELL_SECTION:
					/* RTW_INFO("WEXT_CSCAN_HOME_DWELL_SECTION\n"); */
					pos += 2;
					len -= 2;
					break;
				case WEXT_CSCAN_TYPE_SECTION:
					/* RTW_INFO("WEXT_CSCAN_TYPE_SECTION\n"); */
					pos += 1;
					len -= 1;
					break;
#if 0
				case WEXT_CSCAN_NPROBE_SECTION:
					RTW_INFO("WEXT_CSCAN_NPROBE_SECTION\n");
					break;
#endif

				default:
					/* RTW_INFO("Unknown CSCAN section %c\n", section); */
					len = 0; /* stop parsing */
				}
				/* RTW_INFO("len:%d\n", len); */

			}
			parm->ssid_num = ssid_index;

			/* jeff: it has still some scan paramater to parse, we only do this now... */
			_status = rtw_sitesurvey_cmd(padapter, parm);

		} else

			_status = rtw_sitesurvey_cmd(padapter, NULL);

	if (_status == _FALSE)
		ret = -1;

	rtw_mfree(parm, sizeof(*parm));

exit:
#ifdef DBG_IOCTL
	RTW_INFO("DBG_IOCTL %s:%d return %d\n", __FUNCTION__, __LINE__, ret);
#endif

	return ret;
}

static int rtw_wx_get_scan(struct net_device *dev, struct iw_request_info *a,
			   union iwreq_data *wrqu, char *extra)
{
	_list					*plist, *phead;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rtw_chset *chset = adapter_to_chset(padapter);
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	_queue				*queue	= &(pmlmepriv->scanned_queue);
	struct	wlan_network	*pnetwork = NULL;
	char *ev = extra;
	char *stop = ev + wrqu->data.length;
	u32 ret = 0;
	u32 wait_for_surveydone;
	sint wait_status;
	enum band_type band;
	u8 ch;

#ifdef CONFIG_P2P
	struct	wifidirect_info	*pwdinfo = &padapter->wdinfo;
#endif /* CONFIG_P2P */


#ifdef DBG_IOCTL
	RTW_INFO("DBG_IOCTL %s:%d\n", __FUNCTION__, __LINE__);
#endif

	if (dev_is_drv_stopped(adapter_to_dvobj(padapter))) {
		ret = -EINVAL;
		goto exit;
	}

	wait_for_surveydone = 100;

#if 1 /* Wireless Extension use EAGAIN to try */
	wait_status = WIFI_UNDER_SURVEY
#ifndef CONFIG_RTW_ANDROID
		      | WIFI_UNDER_LINKING
#endif
		      ;

	while (check_fwstate(pmlmepriv, wait_status) == _TRUE)
		return -EAGAIN;
#else
	wait_status = WIFI_UNDER_SURVEY
#ifndef CONFIG_RTW_ANDROID
		      | WIFI_UNDER_LINKING
#endif
		      ;

	while (check_fwstate(pmlmepriv, wait_status) == _TRUE) {
		rtw_msleep_os(30);
		cnt++;
		if (cnt > wait_for_surveydone)
			break;
	}
#endif
	_rtw_spinlock_bh(&(pmlmepriv->scanned_queue.lock));

	phead = get_list_head(queue);
	plist = get_next(phead);

	while (1) {
		if (rtw_end_of_queue_search(phead, plist) == _TRUE)
			break;

		if ((stop - ev) < SCAN_ITEM_SIZE) {
			if(wrqu->data.length == MAX_SCAN_BUFFER_LEN){ /*max buffer len defined by iwlist*/
				ret = 0;
				RTW_INFO("%s: Scan results incomplete\n", __FUNCTION__);
				break;
			}
			ret = -E2BIG;
			break;
		}

		pnetwork = LIST_CONTAINOR(plist, struct wlan_network, list);
		band = pnetwork->network.Configuration.Band;
		ch = pnetwork->network.Configuration.DSConfig;

		/* report network only if the current channel set contains the channel to which this network belongs */
		if (rtw_chset_search_bch(chset, band, ch) >= 0
			&& rtw_mlme_band_check(padapter, ch) == _TRUE
			&& _TRUE == rtw_validate_ssid(&(pnetwork->network.Ssid))
			&& !rtw_chset_is_bch_non_ocp(chset, band, ch)
		)
			ev = translate_scan(padapter, a, pnetwork, ev, stop);

		plist = get_next(plist);

	}

	_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));

	wrqu->data.length = ev - extra;
	wrqu->data.flags = 0;

exit:


#ifdef DBG_IOCTL
	RTW_INFO("DBG_IOCTL %s:%d return %d\n", __FUNCTION__, __LINE__, ret);
#endif

	return ret ;

}

/* set ssid flow
 * s1. rtw_set_802_11_infrastructure_mode()
 * s2. set_802_11_authenticaion_mode()
 * s3. set_802_11_encryption_mode()
 * s4. rtw_set_802_11_ssid() */
static int rtw_wx_set_essid(struct net_device *dev,
			    struct iw_request_info *a,
			    union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	_queue *queue = &pmlmepriv->scanned_queue;
	_list *phead;
	struct wlan_network *pnetwork = NULL;
	NDIS_802_11_AUTHENTICATION_MODE authmode;
	NDIS_802_11_SSID ndis_ssid;
	u8 *dst_ssid, *src_ssid;

	uint ret = 0, len;


#ifdef DBG_IOCTL
	RTW_INFO("DBG_IOCTL %s:%d\n", __FUNCTION__, __LINE__);
#endif
#ifdef CONFIG_WEXT_DONT_JOIN_BYSSID
	RTW_INFO("%s: CONFIG_WEXT_DONT_JOIN_BYSSID be defined!! only allow bssid joining\n", __func__);
	return -EPERM;
#endif

#if WIRELESS_EXT <= 20
	if ((wrqu->essid.length - 1) > IW_ESSID_MAX_SIZE) {
#else
	if (wrqu->essid.length > IW_ESSID_MAX_SIZE) {
#endif
		ret = -E2BIG;
		goto exit;
	}

	if (!padapter->netif_up) {
		ret = -1;
		goto exit;
	}

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		ret = -1;
		goto exit;
	}

#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter, WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING)) {
		RTW_INFO("set ssid, but buddy_intf is under scanning or linking\n");
		ret = -EINVAL;
		goto exit;
	}
#endif
	authmode = padapter->securitypriv.ndisauthtype;
	RTW_INFO("=>%s\n", __FUNCTION__);
	if (wrqu->essid.flags && wrqu->essid.length) {
		/* Commented by Albert 20100519 */
		/* We got the codes in "set_info" function of iwconfig source code. */
		/*	========================================= */
		/*	wrq.u.essid.length = strlen(essid) + 1; */
		/*	if(we_kernel_version > 20) */
		/*		wrq.u.essid.length--; */
		/*	========================================= */
		/*	That means, if the WIRELESS_EXT less than or equal to 20, the correct ssid len should subtract 1. */
#if WIRELESS_EXT <= 20
		len = ((wrqu->essid.length - 1) < IW_ESSID_MAX_SIZE) ? (wrqu->essid.length - 1) : IW_ESSID_MAX_SIZE;
#else
		len = (wrqu->essid.length < IW_ESSID_MAX_SIZE) ? wrqu->essid.length : IW_ESSID_MAX_SIZE;
#endif

		if (wrqu->essid.length != 33)
			RTW_INFO("ssid=%s, len=%d\n", extra, wrqu->essid.length);

		_rtw_memset(&ndis_ssid, 0, sizeof(NDIS_802_11_SSID));
		ndis_ssid.SsidLength = len;
		_rtw_memcpy(ndis_ssid.Ssid, extra, len);
		src_ssid = ndis_ssid.Ssid;

		_rtw_spinlock_bh(&queue->lock);
		phead = get_list_head(queue);
		pmlmepriv->pscanned = get_next(phead);

		while (1) {
			if (rtw_end_of_queue_search(phead, pmlmepriv->pscanned) == _TRUE) {
#if 0
				if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) {
					rtw_set_802_11_ssid(padapter, &ndis_ssid);

					goto exit;
				} else {
					ret = -EINVAL;
					goto exit;
				}
#endif

				break;
			}

			pnetwork = LIST_CONTAINOR(pmlmepriv->pscanned, struct wlan_network, list);

			pmlmepriv->pscanned = get_next(pmlmepriv->pscanned);

			dst_ssid = pnetwork->network.Ssid.Ssid;


			if ((_rtw_memcmp(dst_ssid, src_ssid, ndis_ssid.SsidLength) == _TRUE) &&
			    (pnetwork->network.Ssid.SsidLength == ndis_ssid.SsidLength)) {

				if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE) {
					if (pnetwork->network.InfrastructureMode != pmlmepriv->dev_cur_network.network.InfrastructureMode)
						continue;
				}

				if (rtw_set_802_11_infrastructure_mode(padapter, pnetwork->network.InfrastructureMode, 0) == _FALSE) {
					ret = -1;
					_rtw_spinunlock_bh(&queue->lock);
					goto exit;
				}

				break;
			}
		}
		_rtw_spinunlock_bh(&queue->lock);
		rtw_set_802_11_authentication_mode(padapter, authmode);
		/* set_802_11_encryption_mode(padapter, padapter->securitypriv.ndisencryptstatus); */
		if (rtw_set_802_11_ssid(padapter, &ndis_ssid) == _FALSE) {
			ret = -1;
			goto exit;
		}
	}

exit:
	RTW_INFO("<=%s, ret %d\n", __FUNCTION__, ret);

#ifdef DBG_IOCTL
	RTW_INFO("DBG_IOCTL %s:%d return %d\n", __FUNCTION__, __LINE__, ret);
#endif


	return ret;
}

static int rtw_wx_get_essid(struct net_device *dev,
			    struct iw_request_info *a,
			    union iwreq_data *wrqu, char *extra)
{
	u32 len, ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	WLAN_BSSID_EX  *pcur_bss = &pmlmepriv->dev_cur_network.network;



	if ((check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)) {
		len = pcur_bss->Ssid.SsidLength;

		wrqu->essid.length = len;

		_rtw_memcpy(extra, pcur_bss->Ssid.Ssid, len);

		wrqu->essid.flags = 1;
	} else {
		ret = -1;
		goto exit;
	}

exit:


	return ret;

}

static int rtw_wx_set_rate(struct net_device *dev,
			   struct iw_request_info *a,
			   union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
#if 0
	int i;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8	datarates[NumRates];
	u32	target_rate = wrqu->bitrate.value;
	u32	fixed = wrqu->bitrate.fixed;
	u32	ratevalue = 0;
	u8 mpdatarate[NumRates] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff};



	if (target_rate == -1) {
		ratevalue = 11;
		goto set_rate;
	}
	target_rate = target_rate / 100000;

	switch (target_rate) {
	case 10:
		ratevalue = 0;
		break;
	case 20:
		ratevalue = 1;
		break;
	case 55:
		ratevalue = 2;
		break;
	case 60:
		ratevalue = 3;
		break;
	case 90:
		ratevalue = 4;
		break;
	case 110:
		ratevalue = 5;
		break;
	case 120:
		ratevalue = 6;
		break;
	case 180:
		ratevalue = 7;
		break;
	case 240:
		ratevalue = 8;
		break;
	case 360:
		ratevalue = 9;
		break;
	case 480:
		ratevalue = 10;
		break;
	case 540:
		ratevalue = 11;
		break;
	default:
		ratevalue = 11;
		break;
	}

set_rate:

	for (i = 0; i < NumRates; i++) {
		if (ratevalue == mpdatarate[i]) {
			datarates[i] = mpdatarate[i];
			if (fixed == 0)
				break;
		} else
			datarates[i] = 0xff;

	}

	if (rtw_setdatarate_cmd(padapter, datarates) != _SUCCESS) {
		ret = -1;
	}

#endif
	return ret;
}

static int rtw_wx_get_rate(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	u16 max_rate = 0;

	max_rate = rtw_get_cur_max_rate((_adapter *)rtw_netdev_priv(dev));

	if (max_rate == 0)
		return -EPERM;

	wrqu->bitrate.fixed = 0;	/* no auto select */
	wrqu->bitrate.value = max_rate * 100000;
	/*RTW_INFO("%s, max_rate=%d, wrqu->bitrate.value=%d\n", __func__, max_rate, wrqu->bitrate.value);*/
	/*Note: wrqu->bitrate.value was s32 , so 160M 2SS max rate will overflow*/
	if (wrqu->bitrate.value < 0)
		wrqu->bitrate.value = 0x7FFFFFFF;

	return 0;
}

static int rtw_wx_set_rts(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);


	if (wrqu->rts.disabled)
		padapter->registrypriv.rts_thresh = 2347;
	else {
		if (wrqu->rts.value < 0 ||
		    wrqu->rts.value > 2347)
			return -EINVAL;

		padapter->registrypriv.rts_thresh = wrqu->rts.value;
	}

	RTW_INFO("%s, rts_thresh=%d\n", __func__, padapter->registrypriv.rts_thresh);


	return 0;

}

static int rtw_wx_get_rts(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);


	RTW_INFO("%s, rts_thresh=%d\n", __func__, padapter->registrypriv.rts_thresh);

	wrqu->rts.value = padapter->registrypriv.rts_thresh;
	wrqu->rts.fixed = 0;	/* no auto select */
	/* wrqu->rts.disabled = (wrqu->rts.value == DEFAULT_RTS_THRESHOLD); */


	return 0;
}

static int rtw_wx_set_frag(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);


	if (wrqu->frag.disabled)
		padapter->xmitpriv.frag_len = MAX_FRAG_THRESHOLD;
	else {
		if (wrqu->frag.value < MIN_FRAG_THRESHOLD ||
		    wrqu->frag.value > MAX_FRAG_THRESHOLD)
			return -EINVAL;

		padapter->xmitpriv.frag_len = wrqu->frag.value & ~0x1;
	}

	RTW_INFO("%s, frag_len=%d\n", __func__, padapter->xmitpriv.frag_len);


	return 0;

}

static int rtw_wx_get_frag(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);


	RTW_INFO("%s, frag_len=%d\n", __func__, padapter->xmitpriv.frag_len);

	wrqu->frag.value = padapter->xmitpriv.frag_len;
	wrqu->frag.fixed = 0;	/* no auto select */
	/* wrqu->frag.disabled = (wrqu->frag.value == DEFAULT_FRAG_THRESHOLD); */


	return 0;
}

static int rtw_wx_get_retry(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	/* _adapter *padapter = (_adapter *)rtw_netdev_priv(dev); */


	wrqu->retry.value = 7;
	wrqu->retry.fixed = 0;	/* no auto select */
	wrqu->retry.disabled = 1;

	return 0;

}

#if 0
	#define IW_ENCODE_INDEX		0x00FF	/* Token index (if needed) */
	#define IW_ENCODE_FLAGS		0xFF00	/* Flags defined below */
	#define IW_ENCODE_MODE		0xF000	/* Modes defined below */
	#define IW_ENCODE_DISABLED	0x8000	/* Encoding disabled */
	#define IW_ENCODE_ENABLED	0x0000	/* Encoding enabled */
	#define IW_ENCODE_RESTRICTED	0x4000	/* Refuse non-encoded packets */
	#define IW_ENCODE_OPEN		0x2000	/* Accept non-encoded packets */
	#define IW_ENCODE_NOKEY		0x0800  /* Key is write only, so not present */
	#define IW_ENCODE_TEMP		0x0400  /* Temporary key */
	/*
	iwconfig wlan0 key on->flags = 0x6001->maybe it means auto
	iwconfig wlan0 key off->flags = 0x8800
	iwconfig wlan0 key open->flags = 0x2800
	iwconfig wlan0 key open 1234567890->flags = 0x2000
	iwconfig wlan0 key restricted->flags = 0x4800
	iwconfig wlan0 key open [3] 1234567890->flags = 0x2003
	iwconfig wlan0 key restricted [2] 1234567890->flags = 0x4002
	iwconfig wlan0 key open [3] -> flags = 0x2803
	iwconfig wlan0 key restricted [2] -> flags = 0x4802
	*/
#endif

static int rtw_wx_set_enc(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *keybuf)
{
	u32 key, ret = 0;
	u32 keyindex_provided;
	NDIS_802_11_WEP	 wep;
	NDIS_802_11_AUTHENTICATION_MODE authmode;

	struct iw_point *erq = &(wrqu->encoding);
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	RTW_INFO("+rtw_wx_set_enc, flags=0x%x\n", erq->flags);

	_rtw_memset(&wep, 0, sizeof(NDIS_802_11_WEP));

	key = erq->flags & IW_ENCODE_INDEX;


	if (erq->flags & IW_ENCODE_DISABLED) {
		RTW_INFO("EncryptionDisabled\n");
		padapter->securitypriv.ndisencryptstatus = Ndis802_11EncryptionDisabled;
		padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open; /* open system */
		authmode = Ndis802_11AuthModeOpen;
		padapter->securitypriv.ndisauthtype = authmode;

		goto exit;
	}

	if (key) {
		if (key > WEP_KEYS)
			return -EINVAL;
		key--;
		keyindex_provided = 1;
	} else {
		keyindex_provided = 0;
		key = padapter->securitypriv.dot11PrivacyKeyIndex;
		RTW_INFO("rtw_wx_set_enc, key=%d\n", key);
	}

	/* set authentication mode	 */
	if (erq->flags & IW_ENCODE_OPEN) {
		RTW_INFO("rtw_wx_set_enc():IW_ENCODE_OPEN\n");
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;/* Ndis802_11EncryptionDisabled; */

#ifdef CONFIG_PLATFORM_MT53XX
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
#else
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
#endif

		padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
		authmode = Ndis802_11AuthModeOpen;
		padapter->securitypriv.ndisauthtype = authmode;
	} else if (erq->flags & IW_ENCODE_RESTRICTED) {
		RTW_INFO("rtw_wx_set_enc():IW_ENCODE_RESTRICTED\n");
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;

#ifdef CONFIG_PLATFORM_MT53XX
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
#else
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
#endif

		padapter->securitypriv.dot11PrivacyAlgrthm = _WEP40_;
		padapter->securitypriv.dot118021XGrpPrivacy = _WEP40_;
		authmode = Ndis802_11AuthModeShared;
		padapter->securitypriv.ndisauthtype = authmode;
	} else {
		RTW_INFO("rtw_wx_set_enc():erq->flags=0x%x\n", erq->flags);

		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;/* Ndis802_11EncryptionDisabled; */
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open; /* open system */
		padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
		authmode = Ndis802_11AuthModeOpen;
		padapter->securitypriv.ndisauthtype = authmode;
	}

	wep.KeyIndex = key;
	if (erq->length > 0) {
		wep.KeyLength = erq->length <= 5 ? 5 : 13;

		wep.Length = wep.KeyLength + FIELD_OFFSET(NDIS_802_11_WEP, KeyMaterial);
	} else {
		wep.KeyLength = 0 ;

		if (keyindex_provided == 1) { /* set key_id only, no given KeyMaterial(erq->length==0). */
			padapter->securitypriv.dot11PrivacyKeyIndex = key;

			RTW_INFO("(keyindex_provided == 1), keyid=%d, key_len=%d\n", key, padapter->securitypriv.dot11DefKeylen[key]);

			switch (padapter->securitypriv.dot11DefKeylen[key]) {
			case 5:
				padapter->securitypriv.dot11PrivacyAlgrthm = _WEP40_;
				break;
			case 13:
				padapter->securitypriv.dot11PrivacyAlgrthm = _WEP104_;
				break;
			default:
				padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
				break;
			}

			goto exit;

		}

	}

	wep.KeyIndex |= 0x80000000;

	_rtw_memcpy(wep.KeyMaterial, keybuf, wep.KeyLength);

	if (rtw_set_802_11_add_wep(padapter, &wep) == _FALSE) {
		ret = -EOPNOTSUPP;
		goto exit;
	}
	

exit:


	return ret;

}

static int rtw_wx_get_enc(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *keybuf)
{
	uint key, ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct iw_point *erq = &(wrqu->encoding);
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);


	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) != _TRUE) {
		if (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) != _TRUE) {
			erq->length = 0;
			erq->flags |= IW_ENCODE_DISABLED;
			return 0;
		}
	}


	key = erq->flags & IW_ENCODE_INDEX;

	if (key) {
		if (key > WEP_KEYS)
			return -EINVAL;
		key--;
	} else
		key = padapter->securitypriv.dot11PrivacyKeyIndex;

	erq->flags = key + 1;

	/* if(padapter->securitypriv.ndisauthtype == Ndis802_11AuthModeOpen) */
	/* { */
	/* erq->flags |= IW_ENCODE_OPEN; */
	/* }	  */

	switch (padapter->securitypriv.ndisencryptstatus) {
	case Ndis802_11EncryptionNotSupported:
	case Ndis802_11EncryptionDisabled:

		erq->length = 0;
		erq->flags |= IW_ENCODE_DISABLED;

		break;

	case Ndis802_11Encryption1Enabled:

		erq->length = padapter->securitypriv.dot11DefKeylen[key];

		if (erq->length) {
			_rtw_memcpy(keybuf, padapter->securitypriv.dot11DefKey[key].skey, padapter->securitypriv.dot11DefKeylen[key]);

			erq->flags |= IW_ENCODE_ENABLED;

			if (padapter->securitypriv.ndisauthtype == Ndis802_11AuthModeOpen)
				erq->flags |= IW_ENCODE_OPEN;
			else if (padapter->securitypriv.ndisauthtype == Ndis802_11AuthModeShared)
				erq->flags |= IW_ENCODE_RESTRICTED;
		} else {
			erq->length = 0;
			erq->flags |= IW_ENCODE_DISABLED;
		}

		break;

	case Ndis802_11Encryption2Enabled:
	case Ndis802_11Encryption3Enabled:

		erq->length = 16;
		erq->flags |= (IW_ENCODE_ENABLED | IW_ENCODE_OPEN | IW_ENCODE_NOKEY);

		break;

	default:
		erq->length = 0;
		erq->flags |= IW_ENCODE_DISABLED;

		break;

	}


	return ret;

}

static int rtw_wx_get_power(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	/* _adapter *padapter = (_adapter *)rtw_netdev_priv(dev); */

	wrqu->power.value = 0;
	wrqu->power.fixed = 0;	/* no auto select */
	wrqu->power.disabled = 1;

	return 0;

}

static int rtw_wx_set_gen_ie(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	int ret;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	ret = rtw_set_wpa_ie(padapter, extra, wrqu->data.length);

	return ret;
}

static int rtw_wx_set_auth(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct iw_param *param = (struct iw_param *)&(wrqu->param);
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
#ifdef CONFIG_WAPI_SUPPORT
#ifndef CONFIG_IOCTL_CFG80211
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	u32 value = param->value;
#endif
#endif
	int ret = 0;

	switch (param->flags & IW_AUTH_INDEX) {

	case IW_AUTH_WPA_VERSION:
#ifdef CONFIG_WAPI_SUPPORT
#ifndef CONFIG_IOCTL_CFG80211
		padapter->wapiInfo.bWapiEnable = false;
		if (value == IW_AUTH_WAPI_VERSION_1) {
			padapter->wapiInfo.bWapiEnable = true;
			psecuritypriv->dot11PrivacyAlgrthm = _SMS4_;
			psecuritypriv->dot118021XGrpPrivacy = _SMS4_;
			psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_WAPI;
			pmlmeinfo->auth_algo = psecuritypriv->dot11AuthAlgrthm;
			padapter->wapiInfo.extra_prefix_len = WAPI_EXT_LEN;
			padapter->wapiInfo.extra_postfix_len = SMS4_MIC_LEN;
		}
#endif
#endif
		break;
	case IW_AUTH_CIPHER_PAIRWISE:

		break;
	case IW_AUTH_CIPHER_GROUP:

		break;
	case IW_AUTH_KEY_MGMT:
#ifdef CONFIG_WAPI_SUPPORT
#ifndef CONFIG_IOCTL_CFG80211
		RTW_INFO("rtw_wx_set_auth: IW_AUTH_KEY_MGMT case\n");
		if (value == IW_AUTH_KEY_MGMT_WAPI_PSK)
			padapter->wapiInfo.bWapiPSK = true;
		else
			padapter->wapiInfo.bWapiPSK = false;
		RTW_INFO("rtw_wx_set_auth: IW_AUTH_KEY_MGMT bwapipsk %d\n", padapter->wapiInfo.bWapiPSK);
#endif
#endif
		/*
		 *  ??? does not use these parameters
		 */
		break;

	case IW_AUTH_TKIP_COUNTERMEASURES: {
		if (param->value) {
			/* wpa_supplicant is enabling the tkip countermeasure. */
			padapter->securitypriv.btkip_countermeasure = _TRUE;
		} else {
			/* wpa_supplicant is disabling the tkip countermeasure. */
			padapter->securitypriv.btkip_countermeasure = _FALSE;
		}
		break;
	}
	case IW_AUTH_DROP_UNENCRYPTED: {
		/* HACK:
		 *
		 * wpa_supplicant calls set_wpa_enabled when the driver
		 * is loaded and unloaded, regardless of if WPA is being
		 * used.  No other calls are made which can be used to
		 * determine if encryption will be used or not prior to
		 * association being expected.  If encryption is not being
		 * used, drop_unencrypted is set to false, else true -- we
		 * can use this to determine if the CAP_PRIVACY_ON bit should
		 * be set.
		 */

		if (padapter->securitypriv.ndisencryptstatus == Ndis802_11Encryption1Enabled) {
			break;/* it means init value, or using wep, ndisencryptstatus = Ndis802_11Encryption1Enabled, */
			/* then it needn't reset it; */
		}

		if (param->value) {
			padapter->securitypriv.ndisencryptstatus = Ndis802_11EncryptionDisabled;
			padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
			padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
			padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open; /* open system */
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeOpen;
		}

		break;
	}

	case IW_AUTH_80211_AUTH_ALG:

#if defined(CONFIG_RTW_ANDROID) || 1
		/*
		 *  It's the starting point of a link layer connection using wpa_supplicant
		*/
		if (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE)) {
			rtw_disassoc_cmd(padapter, 500, RTW_CMDF_WAIT_ACK);
			if (1
#ifdef CONFIG_STA_CMD_DISPR
			    && (MLME_IS_STA(padapter) == _FALSE)
#endif /* CONFIG_STA_CMD_DISPR */
			    )
				rtw_free_assoc_resources_cmd(padapter, _TRUE, RTW_CMDF_WAIT_ACK);
			RTW_INFO("%s...call rtw_indicate_disconnect\n ", __FUNCTION__);
			rtw_indicate_disconnect(padapter, 0, _FALSE);

			pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_SYSTEM_DUE_TO_HIGH_LAYER_COMMAND;
			pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
		}
#endif


		ret = wpa_set_auth_algs(dev, (u32)param->value);

		break;

	case IW_AUTH_WPA_ENABLED:

		/* if(param->value) */
		/* padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_8021X; */ /* 802.1x */
		/* else */
		/* padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open; */ /* open system */

		/* _disassociate(priv); */

		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		/* ieee->ieee802_1x = param->value; */
		break;

	case IW_AUTH_PRIVACY_INVOKED:
		/* ieee->privacy_invoked = param->value; */
		break;

#ifdef CONFIG_WAPI_SUPPORT
#ifndef CONFIG_IOCTL_CFG80211
	case IW_AUTH_WAPI_ENABLED:
		break;
#endif
#endif

	default:
		return -EOPNOTSUPP;

	}

	return ret;

}

static int rtw_wx_set_enc_ext(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
	char *alg_name;
	u32 param_len;
	struct ieee_param *param = NULL;
	struct iw_point *pencoding = &wrqu->encoding;
	struct iw_encode_ext *pext = (struct iw_encode_ext *)extra;
	int ret = 0;

	param_len = sizeof(struct ieee_param) + pext->key_len;
	param = (struct ieee_param *)rtw_malloc(param_len);
	if (param == NULL)
		return -1;

	_rtw_memset(param, 0, param_len);

	param->cmd = IEEE_CMD_SET_ENCRYPTION;
	_rtw_memset(param->sta_addr, 0xff, ETH_ALEN);


	switch (pext->alg) {
	case IW_ENCODE_ALG_NONE:
		/* todo: remove key */
		/* remove = 1;	 */
		alg_name = "none";
		break;
	case IW_ENCODE_ALG_WEP:
		alg_name = "WEP";
		break;
	case IW_ENCODE_ALG_TKIP:
		alg_name = "TKIP";
		break;
	case IW_ENCODE_ALG_CCMP:
		alg_name = "CCMP";
		break;
#ifdef CONFIG_IEEE80211W
	case IW_ENCODE_ALG_AES_CMAC:
		alg_name = "BIP";
		break;
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_WAPI_SUPPORT
#ifndef CONFIG_IOCTL_CFG80211
	case IW_ENCODE_ALG_SM4:
		alg_name = "SMS4";
		_rtw_memcpy(param->sta_addr, pext->addr.sa_data, ETH_ALEN);
		RTW_INFO("rtw_wx_set_enc_ext: SMS4 case\n");
		break;
#endif
#endif
	default:
		ret = -1;
		goto exit;
	}

	strncpy((char *)param->u.crypt.alg, alg_name, IEEE_CRYPT_ALG_NAME_LEN);

	if (pext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY)
		param->u.crypt.set_tx = 1;

	/* cliW: WEP does not have group key
	 * just not checking GROUP key setting
	 */
	if ((pext->alg != IW_ENCODE_ALG_WEP) &&
	    ((pext->ext_flags & IW_ENCODE_EXT_GROUP_KEY)
#ifdef CONFIG_IEEE80211W
	     || (pext->ext_flags & IW_ENCODE_ALG_AES_CMAC)
#endif /* CONFIG_IEEE80211W */
	    ))
		param->u.crypt.set_tx = 0;

	param->u.crypt.idx = (pencoding->flags & 0x00FF) - 1 ;

	if (pext->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID) {
#ifdef CONFIG_WAPI_SUPPORT
#ifndef CONFIG_IOCTL_CFG80211
		if (pext->alg == IW_ENCODE_ALG_SM4)
			_rtw_memcpy(param->u.crypt.seq, pext->rx_seq, 16);
		else
#endif /* CONFIG_IOCTL_CFG80211 */
#endif /* CONFIG_WAPI_SUPPORT */
			_rtw_memcpy(param->u.crypt.seq, pext->rx_seq, 8);
	}

	if (pext->key_len) {
		param->u.crypt.key_len = pext->key_len;
		/* _rtw_memcpy(param + 1, pext + 1, pext->key_len); */
		_rtw_memcpy(param->u.crypt.key, pext + 1, pext->key_len);
	}

	if (pencoding->flags & IW_ENCODE_DISABLED) {
		/* todo: remove key */
		/* remove = 1; */
	}

	ret =  wpa_set_encryption(dev, param, param_len);

exit:
	if (param)
		rtw_mfree((u8 *)param, param_len);

	return ret;
}


static int rtw_wx_get_nick(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	/* _adapter *padapter = (_adapter *)rtw_netdev_priv(dev); */
	/* struct mlme_priv *pmlmepriv = &(padapter->mlmepriv); */
	/* struct security_priv *psecuritypriv = &padapter->securitypriv; */

	if (extra) {
		wrqu->data.length = 14;
		wrqu->data.flags = 1;
		_rtw_memcpy(extra, "<WIFI@REALTEK>", 14);
	}

	/* rtw_signal_process(pid, SIGUSR1); */ /* for test */

	/* dump debug info here	 */
#if 0
	u32 dot11AuthAlgrthm;		/*  802.11 auth, could be open, shared, and 8021x */
	u32 dot11PrivacyAlgrthm;	/*  This specify the privacy for shared auth. algorithm. */
	u32 dot118021XGrpPrivacy;	/*  This specify the privacy algthm. used for Grp key */
	u32 ndisauthtype;
	u32 ndisencryptstatus;
#endif

	/* RTW_INFO("auth_alg=0x%x, enc_alg=0x%x, auth_type=0x%x, enc_type=0x%x\n",  */
	/*		psecuritypriv->dot11AuthAlgrthm, psecuritypriv->dot11PrivacyAlgrthm, */
	/*		psecuritypriv->ndisauthtype, psecuritypriv->ndisencryptstatus); */

	/* RTW_INFO("enc_alg=0x%x\n", psecuritypriv->dot11PrivacyAlgrthm); */
	/* RTW_INFO("auth_type=0x%x\n", psecuritypriv->ndisauthtype); */
	/* RTW_INFO("enc_type=0x%x\n", psecuritypriv->ndisencryptstatus); */

	return 0;

}

static int rtw_wx_read32(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{

	_adapter *padapter;
	struct dvobj_priv *dvobj;
	struct iw_point *p;
	u16 len;
	u32 addr;
	u32 data32;
	u32 bytes;
	u8 *ptmp;
	int ret;


	ret = 0;
	padapter = (_adapter *)rtw_netdev_priv(dev);
	dvobj = adapter_to_dvobj(padapter);
	p = &wrqu->data;
	len = p->length;
	if (0 == len)
		return -EINVAL;

	ptmp = (u8 *)rtw_malloc(len);
	if (NULL == ptmp)
		return -ENOMEM;

	if (copy_from_user(ptmp, p->pointer, len)) {
		ret = -EFAULT;
		goto exit;
	}

	bytes = 0;
	addr = 0;
	sscanf(ptmp, "%d,%x", &bytes, &addr);

	switch (bytes) {
	case 1:
		data32 = rtw_phl_read8(dvobj->phl, addr);
		sprintf(extra, "0x%02X", data32);
		break;
	case 2:
		data32 = rtw_phl_read16(dvobj->phl, addr);
		sprintf(extra, "0x%04X", data32);
		break;
	case 4:
		data32 = rtw_phl_read32(dvobj->phl, addr);
		sprintf(extra, "0x%08X", data32);
		break;
#if 0
	#if defined(CONFIG_SDIO_HCI) && defined(CONFIG_SDIO_INDIRECT_ACCESS) && defined(DBG_SDIO_INDIRECT_ACCESS)
	case 11:
		data32 = rtw_sd_iread8(padapter, addr);
		sprintf(extra, "0x%02X", data32);
		break;
	case 12:
		data32 = rtw_sd_iread16(padapter, addr);
		sprintf(extra, "0x%04X", data32);
		break;
	case 14:
		data32 = rtw_sd_iread32(padapter, addr);
		sprintf(extra, "0x%08X", data32);
		break;
	#endif
#endif
	default:
		RTW_INFO("%s: usage> read [bytes],[address(hex)]\n", __func__);
		ret = -EINVAL;
		goto exit;
	}
	RTW_INFO("%s: addr=0x%08X data=%s\n", __func__, addr, extra);

exit:
	rtw_mfree(ptmp, len);
	return 0;
}

static int rtw_wx_write32(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{


	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	u32 addr;
	u32 data32;
	u32 bytes;


	bytes = 0;
	addr = 0;
	data32 = 0;
	sscanf(extra, "%d,%x,%x", &bytes, &addr, &data32);

	switch (bytes) {
	case 1:
		rtw_phl_write8(dvobj->phl, addr, (u8)data32);
		RTW_INFO("%s: addr=0x%08X data=0x%02X\n", __func__, addr, (u8)data32);
		break;
	case 2:
		rtw_phl_write16(dvobj->phl, addr, (u16)data32);
		RTW_INFO("%s: addr=0x%08X data=0x%04X\n", __func__, addr, (u16)data32);
		break;
	case 4:
		rtw_phl_write32(dvobj->phl, addr, data32);
		RTW_INFO("%s: addr=0x%08X data=0x%08X\n", __func__, addr, data32);
		break;
	default:
		RTW_INFO("%s: usage> write [bytes],[address(hex)],[data(hex)]\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int rtw_wx_read_rf(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u32 path, addr, data32;


	path = *(u32 *)extra;
	addr = *((u32 *)extra + 1);
	data32 = rtw_phl_read_rfreg(GET_PHL_INFO(dvobj), path, addr, 0xFFFFF);
	/*	RTW_INFO("%s: path=%d addr=0x%02x data=0x%05x\n", __func__, path, addr, data32); */
	/*
	 * IMPORTANT!!
	 * Only when wireless private ioctl is at odd order,
	 * "extra" would be copied to user space.
	 */
	sprintf(extra, "0x%05x", data32);

	return 0;
}

static int rtw_wx_write_rf(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u32 path, addr, data32;


	path = *(u32 *)extra;
	addr = *((u32 *)extra + 1);
	data32 = *((u32 *)extra + 2);
	/*	RTW_INFO("%s: path=%d addr=0x%02x data=0x%05x\n", __func__, path, addr, data32); */
	rtw_phl_write_rfreg(GET_PHL_INFO(dvobj), path, addr, 0xFFFFF, data32);

	return 0;
}

static int rtw_wx_priv_null(struct net_device *dev, struct iw_request_info *a,
			    union iwreq_data *wrqu, char *b)
{
	return -1;
}

#ifdef CONFIG_RTW_80211K
extern void rm_dbg_cmd(_adapter *padapter, char *s);
static int rtw_wx_priv_rrm(struct net_device *dev, struct iw_request_info *a,
			    union iwreq_data *wrqu, char *b)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u32 path, addr, data32;


	rm_dbg_cmd(padapter, b);
	wrqu->data.length = strlen(b);

	return 0;
}
#endif

static int dummy(struct net_device *dev, struct iw_request_info *a,
		 union iwreq_data *wrqu, char *b)
{
	/* _adapter *padapter = (_adapter *)rtw_netdev_priv(dev);	 */
	/* struct mlme_priv *pmlmepriv = &(padapter->mlmepriv); */

	/* RTW_INFO("cmd_code=%x, fwstate=0x%x\n", a->cmd, get_fwstate(pmlmepriv)); */

	return -1;

}

static int rtw_wx_set_channel_plan(struct net_device *dev,
				   struct iw_request_info *info,
				   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u16 channel_plan_req = (u16)(*((int *)wrqu));

	rtw_chplan_ioctl_input_mapping(&channel_plan_req, NULL);

	if (_SUCCESS != rtw_set_channel_plan(padapter, channel_plan_req, RTW_CHPLAN_6G_UNSPECIFIED, RTW_REGD_SET_BY_USER))
		return -EPERM;

	return 0;
}

static int rtw_wx_set_mtk_wps_probe_ie(struct net_device *dev,
				       struct iw_request_info *a,
				       union iwreq_data *wrqu, char *b)
{
#ifdef CONFIG_PLATFORM_MT53XX
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

#endif
	return 0;
}

static int rtw_wx_get_sensitivity(struct net_device *dev,
				  struct iw_request_info *info,
				  union iwreq_data *wrqu, char *buf)
{
#ifdef CONFIG_PLATFORM_MT53XX
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	/*	Modified by Albert 20110914 */
	/*	This is in dbm format for MTK platform. */
	wrqu->qual.level = padapter->recvinfo.rssi;
	RTW_INFO(" level = %u\n",  wrqu->qual.level);
#endif
	return 0;
}

static int rtw_wx_set_mtk_wps_ie(struct net_device *dev,
				 struct iw_request_info *info,
				 union iwreq_data *wrqu, char *extra)
{
#ifdef CONFIG_PLATFORM_MT53XX
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	return rtw_set_wpa_ie(padapter, wrqu->data.pointer, wrqu->data.length);
#else
	return 0;
#endif
}

#ifdef MP_IOCTL_HDL
static void rtw_dbg_mode_hdl(_adapter *padapter, u32 id, u8 *pdata, u32 len)
{
	pRW_Reg	RegRWStruct;
	struct rf_reg_param *prfreg;
	struct dvobj_priv dvobj = adapter_to_dvobj(padapter);
	u8 path;
	u8 offset;
	u32 value;

	RTW_INFO("%s\n", __FUNCTION__);

	switch (id) {
	case GEN_MP_IOCTL_SUBCODE(MP_START):
		RTW_INFO("871x_driver is only for normal mode, can't enter mp mode\n");
		break;
	case GEN_MP_IOCTL_SUBCODE(READ_REG):
		RegRWStruct = (pRW_Reg)pdata;
		switch (RegRWStruct->width) {
		case 1:
			RegRWStruct->value = rtw_read8(padapter, RegRWStruct->offset);
			break;
		case 2:
			RegRWStruct->value = rtw_read16(padapter, RegRWStruct->offset);
			break;
		case 4:
			RegRWStruct->value = rtw_read32(padapter, RegRWStruct->offset);
			break;
		default:
			break;
		}

		break;
	case GEN_MP_IOCTL_SUBCODE(WRITE_REG):
		RegRWStruct = (pRW_Reg)pdata;
		switch (RegRWStruct->width) {
		case 1:
			rtw_write8(padapter, RegRWStruct->offset, (u8)RegRWStruct->value);
			break;
		case 2:
			rtw_write16(padapter, RegRWStruct->offset, (u16)RegRWStruct->value);
			break;
		case 4:
			rtw_write32(padapter, RegRWStruct->offset, (u32)RegRWStruct->value);
			break;
		default:
			break;
		}

		break;
	case GEN_MP_IOCTL_SUBCODE(READ_RF_REG):

		prfreg = (struct rf_reg_param *)pdata;

		path = (u8)prfreg->path;
		offset = (u8)prfreg->offset;

		value = rtw_phl_read_rfreg(GET_PHL_INFO(dvobj), path, offset, 0xffffffff);

		prfreg->value = value;

		break;
	case GEN_MP_IOCTL_SUBCODE(WRITE_RF_REG):

		prfreg = (struct rf_reg_param *)pdata;

		path = (u8)prfreg->path;
		offset = (u8)prfreg->offset;
		value = prfreg->value;

		rtw_phl_write_rfreg(GET_PHL_INFO(dvobj), path, offset, 0xffffffff, value);

		break;
	case GEN_MP_IOCTL_SUBCODE(TRIGGER_GPIO):
		RTW_INFO("==> trigger gpio 0\n");
		rtw_hal_set_hwreg(padapter, HW_VAR_TRIGGER_GPIO_0, 0);
		break;
#ifdef CONFIG_BTC
	case GEN_MP_IOCTL_SUBCODE(SET_DM_BT):
		RTW_INFO("==> set dm_bt_coexist:%x\n", *(u8 *)pdata);
		rtw_hal_set_hwreg(padapter, HW_VAR_BT_SET_COEXIST, pdata);
		break;
	case GEN_MP_IOCTL_SUBCODE(DEL_BA):
		RTW_INFO("==> delete ba:%x\n", *(u8 *)pdata);
		rtw_hal_set_hwreg(padapter, HW_VAR_BT_ISSUE_DELBA, pdata);
		break;
#endif
#ifdef DBG_CONFIG_ERROR_DETECT
	case GEN_MP_IOCTL_SUBCODE(GET_WIFI_STATUS):
		*pdata = rtw_hal_sreset_get_wifi_status(padapter);
		break;
#endif

	default:
		break;
	}

}
static int rtw_mp_ioctl_hdl(struct net_device *dev, struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
	u32 BytesRead, BytesWritten, BytesNeeded;
	struct oid_par_priv	oid_par;
	struct mp_ioctl_handler	*phandler;
	struct mp_ioctl_param	*poidparam;
	uint status = 0;
	u16 len;
	u8 *pparmbuf = NULL, bset;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct iw_point *p = &wrqu->data;

	/* RTW_INFO("+rtw_mp_ioctl_hdl\n"); */

	/* mutex_lock(&ioctl_mutex); */

	if ((!p->length) || (!p->pointer)) {
		ret = -EINVAL;
		goto _rtw_mp_ioctl_hdl_exit;
	}

	pparmbuf = NULL;
	bset = (u8)(p->flags & 0xFFFF);
	len = p->length;
	pparmbuf = (u8 *)rtw_malloc(len);
	if (pparmbuf == NULL) {
		ret = -ENOMEM;
		goto _rtw_mp_ioctl_hdl_exit;
	}

	if (copy_from_user(pparmbuf, p->pointer, len)) {
		ret = -EFAULT;
		goto _rtw_mp_ioctl_hdl_exit;
	}

	poidparam = (struct mp_ioctl_param *)pparmbuf;

	if (poidparam->subcode >= MAX_MP_IOCTL_SUBCODE) {
		ret = -EINVAL;
		goto _rtw_mp_ioctl_hdl_exit;
	}

	/* RTW_INFO("%s: %d\n", __func__, poidparam->subcode); */
#ifdef CONFIG_MP_INCLUDED
	if (padapter->registrypriv.mp_mode == 1) {
		phandler = mp_ioctl_hdl + poidparam->subcode;

		if ((phandler->paramsize != 0) && (poidparam->len < phandler->paramsize)) {
			ret = -EINVAL;
			goto _rtw_mp_ioctl_hdl_exit;
		}

		if (phandler->handler) {
			oid_par.adapter_context = padapter;
			oid_par.oid = phandler->oid;
			oid_par.information_buf = poidparam->data;
			oid_par.information_buf_len = poidparam->len;
			oid_par.dbg = 0;

			BytesWritten = 0;
			BytesNeeded = 0;

			if (bset) {
				oid_par.bytes_rw = &BytesRead;
				oid_par.bytes_needed = &BytesNeeded;
				oid_par.type_of_oid = SET_OID;
			} else {
				oid_par.bytes_rw = &BytesWritten;
				oid_par.bytes_needed = &BytesNeeded;
				oid_par.type_of_oid = QUERY_OID;
			}

			status = phandler->handler(&oid_par);

			/* todo:check status, BytesNeeded, etc. */
		} else {
			RTW_INFO("rtw_mp_ioctl_hdl(): err!, subcode=%d, oid=%d, handler=%p\n",
				poidparam->subcode, phandler->oid, phandler->handler);
			ret = -EFAULT;
			goto _rtw_mp_ioctl_hdl_exit;
		}
	} else
#endif
	{
		rtw_dbg_mode_hdl(padapter, poidparam->subcode, poidparam->data, poidparam->len);
	}

	if (bset == 0x00) {/* query info */
		if (copy_to_user(p->pointer, pparmbuf, len))
			ret = -EFAULT;
	}

	if (status) {
		ret = -EFAULT;
		goto _rtw_mp_ioctl_hdl_exit;
	}

_rtw_mp_ioctl_hdl_exit:

	if (pparmbuf)
		rtw_mfree(pparmbuf, len);

	/* mutex_unlock(&ioctl_mutex); */

	return ret;
}
#endif /*MP_IOCTL_HDL*/
static int rtw_get_ap_info(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
	u32 cnt = 0, wpa_ielen;
	_list	*plist, *phead;
	unsigned char *pbuf;
	u8 bssid[ETH_ALEN];
	char data[32];
	struct wlan_network *pnetwork = NULL;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	_queue *queue = &(pmlmepriv->scanned_queue);
	struct iw_point *pdata = &wrqu->data;

	RTW_INFO("+rtw_get_aplist_info\n");

	if (dev_is_drv_stopped(adapter_to_dvobj(padapter)) || (pdata == NULL)) {
		ret = -EINVAL;
		goto exit;
	}

	while ((check_fwstate(pmlmepriv, (WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING))) == _TRUE) {
		rtw_msleep_os(30);
		cnt++;
		if (cnt > 100)
			break;
	}


	/* pdata->length = 0; */ /* ?	 */
	pdata->flags = 0;
	if (pdata->length >= 32) {
		if (copy_from_user(data, pdata->pointer, 32)) {
			ret = -EINVAL;
			goto exit;
		}
	} else {
		ret = -EINVAL;
		goto exit;
	}

	_rtw_spinlock_bh(&(pmlmepriv->scanned_queue.lock));

	phead = get_list_head(queue);
	plist = get_next(phead);

	while (1) {
		if (rtw_end_of_queue_search(phead, plist) == _TRUE)
			break;


		pnetwork = LIST_CONTAINOR(plist, struct wlan_network, list);

		/* if(hwaddr_aton_i(pdata->pointer, bssid)) */
		if (hwaddr_aton_i(data, bssid)) {
			RTW_INFO("Invalid BSSID '%s'.\n", (u8 *)data);
			_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));
			return -EINVAL;
		}


		if (_rtw_memcmp(bssid, pnetwork->network.MacAddress, ETH_ALEN) == _TRUE) { /* BSSID match, then check if supporting wpa/wpa2 */
			RTW_INFO("BSSID:" MAC_FMT "\n", MAC_ARG(bssid));

			pbuf = rtw_get_wpa_ie(&pnetwork->network.IEs[12], &wpa_ielen, pnetwork->network.IELength - 12);
			if (pbuf && (wpa_ielen > 0)) {
				pdata->flags = 1;
				break;
			}

			pbuf = rtw_get_wpa2_ie(&pnetwork->network.IEs[12], &wpa_ielen, pnetwork->network.IELength - 12);
			if (pbuf && (wpa_ielen > 0)) {
				pdata->flags = 2;
				break;
			}

		}

		plist = get_next(plist);

	}

	_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));

	if (pdata->length >= 34) {
		if (copy_to_user((u8 *)pdata->pointer + 32, (u8 *)&pdata->flags, 1)) {
			ret = -EINVAL;
			goto exit;
		}
	}

exit:

	return ret;

}

static int rtw_set_pid(struct net_device *dev,
		       struct iw_request_info *info,
		       union iwreq_data *wrqu, char *extra)
{

	int ret = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	int *pdata = (int *)wrqu;
	int selector;

	if (dev_is_drv_stopped(adapter_to_dvobj(padapter)) || (pdata == NULL)) {
		ret = -EINVAL;
		goto exit;
	}

	selector = *pdata;
	if (selector < 3 && selector >= 0) {
		padapter->pid[selector] = *(pdata + 1);
#ifdef CONFIG_GLOBAL_UI_PID
		ui_pid[selector] = *(pdata + 1);
#endif
		RTW_INFO("%s set pid[%d]=%d\n", __FUNCTION__, selector , padapter->pid[selector]);
	} else
		RTW_INFO("%s selector %d error\n", __FUNCTION__, selector);

exit:

	return ret;

}

static int rtw_wps_start(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{

	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct iw_point *pdata = &wrqu->data;
	u32   u32wps_start = 0;
	unsigned int uintRet = 0;

	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)) || (NULL == pdata)) {
		ret = -EINVAL;
		goto exit;
	}

	uintRet = copy_from_user((void *) &u32wps_start, pdata->pointer, 4);
	if (u32wps_start == 0)
		u32wps_start = *extra;

	RTW_INFO("[%s] wps_start = %d\n", __FUNCTION__, u32wps_start);

	if (u32wps_start == 1)   /* WPS Start */
		rtw_led_control(padapter, LED_CTL_START_WPS);
	else if (u32wps_start == 2)   /* WPS Stop because of wps success */
		rtw_led_control(padapter, LED_CTL_STOP_WPS);
	else if (u32wps_start == 3)   /* WPS Stop because of wps fail */
		rtw_led_control(padapter, LED_CTL_STOP_WPS_FAIL);

exit:

	return ret;

}

extern int rtw_change_ifname(_adapter *padapter, const char *ifname);
static int rtw_rereg_nd_name(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rereg_nd_name_data *rereg_priv = &padapter->rereg_nd_name_priv;
	char new_ifname[IFNAMSIZ];

	if (rereg_priv->old_ifname[0] == 0) {
		char *reg_ifname;
#ifdef CONFIG_CONCURRENT_MODE
		if (padapter->isprimary)
			reg_ifname = padapter->registrypriv.ifname;
		else
#endif
			reg_ifname = padapter->registrypriv.if2name;

		strncpy(rereg_priv->old_ifname, reg_ifname, IFNAMSIZ);
		rereg_priv->old_ifname[IFNAMSIZ - 1] = 0;
	}

	/* RTW_INFO("%s wrqu->data.length:%d\n", __FUNCTION__, wrqu->data.length); */
	if (wrqu->data.length > IFNAMSIZ)
		return -EFAULT;

	if (copy_from_user(new_ifname, wrqu->data.pointer, IFNAMSIZ))
		return -EFAULT;

	if (0 == strcmp(rereg_priv->old_ifname, new_ifname))
		return ret;

	RTW_INFO("%s new_ifname:%s\n", __FUNCTION__, new_ifname);
	rtw_set_rtnl_lock_holder(dvobj, current);
	ret = rtw_change_ifname(padapter, new_ifname);
	rtw_set_rtnl_lock_holder(dvobj, NULL);
	if (0 != ret)
		goto exit;

	if (_rtw_memcmp(rereg_priv->old_ifname, "disable%d", 9) == _TRUE) {
		/* rtw_ips_mode_req(&padapter->pwrctrlpriv, rereg_priv->old_ips_mode); */
	}

	strncpy(rereg_priv->old_ifname, new_ifname, IFNAMSIZ);
	rereg_priv->old_ifname[IFNAMSIZ - 1] = 0;

	if (_rtw_memcmp(new_ifname, "disable%d", 9) == _TRUE) {

		RTW_INFO("%s disable\n", __FUNCTION__);
		/* free network queue for Android's timming issue */
		rtw_free_network_queue(padapter, _TRUE);
		rtw_free_mld_network_queue(padapter, _TRUE);

		/* the interface is being "disabled", we can do deeper IPS */
		/* rereg_priv->old_ips_mode = rtw_get_ips_mode_req(&padapter->pwrctrlpriv); */
		/* rtw_ips_mode_req(&padapter->pwrctrlpriv, IPS_NORMAL); */
	}
exit:
	return ret;

}

#ifdef DBG_CMD_QUEUE
u8 dump_cmd_id = 0;
#endif

#if 1
static int rtw_dbg_port(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

	return ret;
}
#else
static int rtw_dbg_port(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
	u8 major_cmd, minor_cmd;
	u16 arg;
	u32 extra_arg, *pdata, val32;
	struct sta_info *psta;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct wlan_network *cur_network = &(pmlmepriv->cur_network);
	struct sta_priv *pstapriv = &padapter->stapriv;


	pdata = (u32 *)&wrqu->data;

	val32 = *pdata;
	arg = (u16)(val32 & 0x0000ffff);
	major_cmd = (u8)(val32 >> 24);
	minor_cmd = (u8)((val32 >> 16) & 0x00ff);

	extra_arg = *(pdata + 1);

	switch (major_cmd) {
	case 0x70: /* read_reg */
		switch (minor_cmd) {
		case 1:
			RTW_INFO("rtw_read8(0x%x)=0x%02x\n", arg, rtw_read8(padapter, arg));
			break;
		case 2:
			RTW_INFO("rtw_read16(0x%x)=0x%04x\n", arg, rtw_read16(padapter, arg));
			break;
		case 4:
			RTW_INFO("rtw_read32(0x%x)=0x%08x\n", arg, rtw_read32(padapter, arg));
			break;
		}
		break;
	case 0x71: /* write_reg */
		switch (minor_cmd) {
		case 1:
			rtw_write8(padapter, arg, extra_arg);
			RTW_INFO("rtw_write8(0x%x)=0x%02x\n", arg, rtw_read8(padapter, arg));
			break;
		case 2:
			rtw_write16(padapter, arg, extra_arg);
			RTW_INFO("rtw_write16(0x%x)=0x%04x\n", arg, rtw_read16(padapter, arg));
			break;
		case 4:
			rtw_write32(padapter, arg, extra_arg);
			RTW_INFO("rtw_write32(0x%x)=0x%08x\n", arg, rtw_read32(padapter, arg));
			break;
		}
		break;
	case 0x72: /* read_bb */
		RTW_INFO("read_bbreg(0x%x)=0x%x\n", arg, rtw_phl_read_bbreg(padapter, arg, 0xffffffff));
		break;
	case 0x73: /* write_bb */
		rtw_phl_write_bbreg(padapter, arg, 0xffffffff, extra_arg);
		RTW_INFO("write_bbreg(0x%x)=0x%x\n", arg, rtw_phl_read_bbreg(padapter, arg, 0xffffffff));
		break;
	case 0x74: /* read_rf */
		RTW_INFO("read RF_reg path(0x%02x),offset(0x%x),value(0x%08x)\n", minor_cmd, arg, rtw_hal_read_rfreg(padapter, minor_cmd, arg, 0xffffffff));
		break;
	case 0x75: /* write_rf */
		rtw_phl_write_rfreg(GET_PHL_INFO(dvobj), minor_cmd, arg, 0xffffffff, extra_arg);
		RTW_INFO("write RF_reg path(0x%02x),offset(0x%x),value(0x%08x)\n", minor_cmd, arg, rtw_hal_read_rfreg(padapter, minor_cmd, arg, 0xffffffff));
		break;

	case 0x76:
		switch (minor_cmd) {
		case 0x00: /* normal mode, */
			padapter->recvinfo.is_signal_dbg = 0;
			break;
		case 0x01: /* dbg mode */
			padapter->recvinfo.is_signal_dbg = 1;
			extra_arg = extra_arg > 100 ? 100 : extra_arg;
			padapter->recvinfo.signal_strength_dbg = extra_arg;
			break;
		}
		break;
	case 0x78:
		break;
	case 0x79: {
		/*
		* dbg 0x79000000 [value], set RESP_TXAGC to + value, value:0~15
		* dbg 0x79010000 [value], set RESP_TXAGC to - value, value:0~15
		*/
		u8 value =  extra_arg & 0x0f;
		u8 sign = minor_cmd;
		u16 write_value = 0;

		RTW_INFO("%s set RESP_TXAGC to %s %u\n", __func__, sign ? "minus" : "plus", value);

		if (sign)
			value = value | 0x10;

		write_value = value | (value << 5);
		rtw_write16(padapter, 0x6d9, write_value);
	}
		break;
	case 0x7a:
		receive_disconnect(padapter, pmlmeinfo->network.MacAddress
				   , WLAN_REASON_EXPIRATION_CHK, _FALSE);
		pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
		pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_IOCTL_DBG_PORT;
		pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
		break;
	case 0x7F:
		switch (minor_cmd) {
		case 0x0:
			RTW_INFO("fwstate=0x%x\n", get_fwstate(pmlmepriv));
			break;
		case 0x01:
			RTW_INFO("auth_alg=0x%x, enc_alg=0x%x, auth_type=0x%x, enc_type=0x%x\n",
				psecuritypriv->dot11AuthAlgrthm, psecuritypriv->dot11PrivacyAlgrthm,
				psecuritypriv->ndisauthtype, psecuritypriv->ndisencryptstatus);
			break;
		case 0x03:
			RTW_INFO("qos_option=%d\n", pmlmepriv->qospriv.qos_option);
#ifdef CONFIG_80211N_HT
			RTW_INFO("ht_option=%d\n", pmlmepriv->htpriv.ht_option);
#endif /* CONFIG_80211N_HT */
			break;
		case 0x04:
			RTW_INFO("cur_ch=%d\n", pmlmeext->chandef.chan);
			RTW_INFO("cur_bw=%d\n", pmlmeext->chandef.bw);
			RTW_INFO("cur_ch_off=%d\n", pmlmeext->chandef.offset);

			RTW_INFO("oper_ch=%d\n", rtw_get_oper_ch(padapter));
			RTW_INFO("oper_bw=%d\n", rtw_get_oper_bw(padapter));
			RTW_INFO("oper_ch_offet=%d\n", rtw_get_oper_choffset(padapter));

			break;
		case 0x05:
			psta = rtw_get_stainfo(pstapriv, cur_network->network.MacAddress);
			if (psta) {
				RTW_INFO("SSID=%s\n", cur_network->network.Ssid.Ssid);
				RTW_INFO("sta's macaddr:" MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
				RTW_INFO("cur_channel=%d, cur_bwmode=%d, cur_ch_offset=%d\n",
					pmlmeext->chandef.chan, pmlmeext->chandef.bw, pmlmeext->chandef.offset);
				RTW_INFO("rtsen=%d, cts2slef=%d\n", psta->rtsen, psta->cts2self);
				RTW_INFO("state=0x%x, aid=%d, macid=%d, raid=%d\n",
					psta->state, psta->phl_sta->aid, psta->phl_sta->macid, psta->phl_sta->ra_info.rate_id);
#ifdef CONFIG_80211N_HT
				RTW_INFO("qos_en=%d, ht_en=%d, init_rate=%d\n", psta->qos_option, psta->htpriv.ht_option, psta->init_rate);
				RTW_INFO("bwmode=%d, ch_offset=%d, sgi_20m=%d,sgi_40m=%d\n"
					, psta->phl_sta->chandef.bw, psta->htpriv.ch_offset, psta->htpriv.sgi_20m, psta->htpriv.sgi_40m);
				RTW_INFO("ampdu_enable = %d\n", psta->ampdu_priv.ampdu_enable);
				RTW_INFO("agg_enable_bitmap=%x, candidate_tid_bitmap=%x\n", psta->ampdu_priv.agg_enable_bitmap, psta->ampdu_priv.candidate_tid_bitmap);
#endif /* CONFIG_80211N_HT */

				sta_rx_reorder_ctl_dump(RTW_DBGDUMP, psta);
			} else
				RTW_INFO("can't get sta's macaddr, cur_network's macaddr:" MAC_FMT "\n", MAC_ARG(cur_network->network.MacAddress));
			break;
		case 0x06: {
				u64 tsf = 0;

				tsf = rtw_hal_get_tsftr_by_port(padapter, extra_arg);
				RTW_INFO(" PORT-%d TSF :%21lld\n", extra_arg, tsf);
		}
			break;
		case 0x07:
			RTW_INFO("bSurpriseRemoved=%s, bDriverStopped=%s\n"
				, dev_is_surprise_removed(adapter_to_dvobj(padapter)) ? "True" : "False"
				, dev_is_drv_stopped(adapter_to_dvobj(padapter)) ? "True" : "False");
			break;
		case 0x08: {
			struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
			struct recv_priv  *precvpriv = &adapter_to_dvobj(padapter)->recvpriv;

			RTW_INFO("free_xmitbuf_cnt=%d, free_xmitframe_cnt=%d"
				", free_xmit_extbuf_cnt=%d, free_xframe_ext_cnt=%d"
				 ", free_recvframe_cnt=%d\n",
				pxmitpriv->free_xmitbuf_cnt, pxmitpriv->free_xmitframe_cnt,
				pxmitpriv->free_xmit_extbuf_cnt, pxmitpriv->free_xframe_ext_cnt,
				 precvpriv->free_recvframe_cnt);
		}
			break;
		case 0x09: {
			int i;
			_list	*plist, *phead;

#ifdef CONFIG_AP_MODE
			RTW_INFO_DUMP("sta_dz_bitmap:", pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len);
			RTW_INFO_DUMP("tim_bitmap:", pstapriv->tim_bitmap, pstapriv->aid_bmp_len);
#endif
			_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

			for (i = 0; i < NUM_STA; i++) {
				phead = &(pstapriv->sta_hash[i]);
				plist = get_next(phead);

				while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
					psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

					plist = get_next(plist);

					if (extra_arg == psta->phl_sta->aid) {
						RTW_INFO("sta's macaddr:" MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
						RTW_INFO("rtsen=%d, cts2slef=%d\n", psta->rtsen, psta->cts2self);
						RTW_INFO("state=0x%x, aid=%d, macid=%d, raid=%d\n",
							psta->state, psta->phl_sta->aid, psta->phl_sta->macid, psta->phl_sta->ra_info.rate_id);
#ifdef CONFIG_80211N_HT
						RTW_INFO("qos_en=%d, ht_en=%d, init_rate=%d\n", psta->qos_option, psta->htpriv.ht_option, psta->init_rate);
						RTW_INFO("bwmode=%d, ch_offset=%d, sgi_20m=%d,sgi_40m=%d\n",
							psta->phl_sta->chandef.bw, psta->htpriv.ch_offset, psta->htpriv.sgi_20m,
							psta->htpriv.sgi_40m);
						RTW_INFO("ampdu_enable = %d\n", psta->ampdu_priv.ampdu_enable);
						RTW_INFO("agg_enable_bitmap=%x, candidate_tid_bitmap=%x\n", psta->ampdu_priv.agg_enable_bitmap, psta->ampdu_priv.candidate_tid_bitmap);
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_AP_MODE
						RTW_INFO("capability=0x%x\n", psta->capability);
						RTW_INFO("flags=0x%x\n", psta->flags);
						RTW_INFO("wpa_psk=0x%x\n", psta->wpa_psk);
						RTW_INFO("wpa2_group_cipher=0x%x\n", psta->wpa2_group_cipher);
						RTW_INFO("wpa2_pairwise_cipher=0x%x\n", psta->wpa2_pairwise_cipher);
						RTW_INFO("qos_info=0x%x\n", psta->qos_info);
#endif
						RTW_INFO("dot118021XPrivacy=0x%x\n", psta->dot118021XPrivacy);

						sta_rx_reorder_ctl_dump(RTW_DBGDUMP, psta);
					}

				}
			}

			_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

		}
			break;

		case 0x0b: { /* Enable=1, Disable=0 driver control vrtl_carrier_sense. */
			/* u8 driver_vcs_en; */ /* Enable=1, Disable=0 driver control vrtl_carrier_sense. */
			/* u8 driver_vcs_type; */ /* force 0:disable VCS, 1:RTS-CTS, 2:CTS-to-self when vcs_en=1. */

			if (arg == 0) {
				RTW_INFO("disable driver ctrl vcs\n");
				padapter->driver_vcs_en = 0;
			} else if (arg == 1) {
				RTW_INFO("enable driver ctrl vcs = %d\n", extra_arg);
				padapter->driver_vcs_en = 1;

				if (extra_arg > 2)
					padapter->driver_vcs_type = 1;
				else
					padapter->driver_vcs_type = extra_arg;
			}
		}
			break;
		case 0x0c: { /* dump rx/tx packet */
			if (arg == 0) {
				RTW_INFO("dump rx packet (%d)\n", extra_arg);
				/* pHalData->bDumpRxPkt =extra_arg;						 */
				rtw_hal_set_def_var(padapter, HAL_DEF_DBG_DUMP_RXPKT, &(extra_arg));
			} else if (arg == 1) {
				RTW_INFO("dump tx packet (%d)\n", extra_arg);
				rtw_hal_set_def_var(padapter, HAL_DEF_DBG_DUMP_TXPKT, &(extra_arg));
			}
		}
			break;
		case 0x0e: {
			if (arg == 0) {
				RTW_INFO("disable driver ctrl rx_ampdu_factor\n");
				padapter->driver_rx_ampdu_factor = 0xFF;
			} else if (arg == 1) {

				RTW_INFO("enable driver ctrl rx_ampdu_factor = %d\n", extra_arg);

				if (extra_arg > 0x03)
					padapter->driver_rx_ampdu_factor = 0xFF;
				else
					padapter->driver_rx_ampdu_factor = extra_arg;
			}
		}
			break;
		#ifdef DBG_CONFIG_ERROR_DETECT
		case 0x0f: {
			if (extra_arg == 0) {
				RTW_INFO("###### silent reset test.......#####\n");
				rtw_hal_sreset_reset(padapter);
			} else {
				struct rtw_phl_com_t *phl_com = GET_PHL_COM(padapter);
				struct sreset_priv *psrtpriv = &pHalData->srestpriv;
				psrtpriv->dbg_trigger_point = extra_arg;
			}

		}
			break;
		case 0x15: {
			struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
			RTW_INFO("==>silent resete cnts:%d\n", pwrpriv->ips_enter_cnts);
		}
			break;

		#endif

		case 0x10: /* driver version display */
			dump_drv_version(RTW_DBGDUMP);
			break;
		case 0x11: { /* dump linked status */
			int pre_mode;
			pre_mode = padapter->bLinkInfoDump;
			/* rtw_hal_linked_info_dump(padapter,extra_arg); */
			if (extra_arg == 1 || (extra_arg == 0 && pre_mode == 1)) /* not consider pwr_saving 0: */
				padapter->bLinkInfoDump = extra_arg;

			else if ((extra_arg == 2) || (extra_arg == 0 && pre_mode == 2)) { /* consider power_saving */
				/* RTW_INFO("rtw_hal_linked_info_dump =%s\n", (padapter->bLinkInfoDump)?"enable":"disable") */
				rtw_hal_linked_info_dump(padapter, extra_arg);
			}



		}
			break;
#ifdef CONFIG_80211N_HT
		case 0x12: { /* set rx_stbc */
			struct registry_priv	*pregpriv = &padapter->registrypriv;
			/* 0: disable, bit(0):enable 2.4g, bit(1):enable 5g, 0x3: enable both 2.4g and 5g */
			/* default is set to enable 2.4GHZ for IOT issue with bufflao's AP at 5GHZ */
			if (extra_arg == 0 || extra_arg == 1 || extra_arg == 2 || extra_arg == 3) {
				pregpriv->rx_stbc = extra_arg;
				RTW_INFO("set rx_stbc=%d\n", pregpriv->rx_stbc);
			} else {
				RTW_INFO("get rx_stbc=%d\n", pregpriv->rx_stbc);
			}
		}
			break;
		case 0x13: { /* set ampdu_enable */
			struct registry_priv	*pregpriv = &padapter->registrypriv;
			/* 0: disable, 0x1:enable */
			if (extra_arg < 2) {
				pregpriv->ampdu_enable = extra_arg;
				RTW_INFO("set ampdu_enable=%d\n", pregpriv->ampdu_enable);
			} else {
				RTW_INFO("get ampdu_enable=%d\n", pregpriv->ampdu_enable);
			}
		}
			break;
#endif
		case 0x14: { /* get wifi_spec */
			struct registry_priv	*pregpriv = &padapter->registrypriv;
			RTW_INFO("get wifi_spec=%d\n", pregpriv->wifi_spec);

		}
			break;

#ifdef DBG_FIXED_CHAN
		case 0x17: {
			struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
			printk("===>  Fixed channel to %d\n", extra_arg);
			pmlmeext->fixed_chan = extra_arg;

		}
			break;
#endif
#ifdef CONFIG_80211N_HT
		case 0x19: {
			struct registry_priv	*pregistrypriv = &padapter->registrypriv;
			/* extra_arg : */
			/* BIT0: Enable VHT LDPC Rx, BIT1: Enable VHT LDPC Tx, */
			/* BIT4: Enable HT LDPC Rx, BIT5: Enable HT LDPC Tx */
			if (arg == 0) {
				RTW_INFO("driver disable LDPC\n");
				pregistrypriv->ldpc_cap = 0x00;
			} else if (arg == 1) {
				RTW_INFO("driver set LDPC cap = 0x%x\n", extra_arg);
				pregistrypriv->ldpc_cap = (u8)(extra_arg & 0x33);
			}
		}
			break;
		case 0x1a: {
			struct registry_priv	*pregistrypriv = &padapter->registrypriv;
			/* extra_arg : */
			/* BIT0: Enable VHT STBC Rx, BIT1: Enable VHT STBC Tx, */
			/* BIT4: Enable HT STBC Rx, BIT5: Enable HT STBC Tx */
			if (arg == 0) {
				RTW_INFO("driver disable STBC\n");
				pregistrypriv->stbc_cap = 0x00;
			} else if (arg == 1) {
				RTW_INFO("driver set STBC cap = 0x%x\n", extra_arg);
				pregistrypriv->stbc_cap = (u8)(extra_arg & 0x33);
			}
		}
			break;
#endif /* CONFIG_80211N_HT */
		case 0x1b: {
			struct registry_priv	*pregistrypriv = &padapter->registrypriv;

			if (arg == 0) {
				RTW_INFO("disable driver ctrl max_rx_rate, reset to default_rate_set\n");
				init_mlme_default_rate_set(padapter);
#ifdef CONFIG_80211N_HT
				pregistrypriv->ht_enable = (u8)rtw_ht_enable;
#endif /* CONFIG_80211N_HT */
			} else if (arg == 1) {

				int i;
				u8 max_rx_rate;

				RTW_INFO("enable driver ctrl max_rx_rate = 0x%x\n", extra_arg);

				max_rx_rate = (u8)extra_arg;

				if (max_rx_rate < 0xc) { /* max_rx_rate < MSC0->B or G -> disable HT */
#ifdef CONFIG_80211N_HT
					pregistrypriv->ht_enable = 0;
#endif /* CONFIG_80211N_HT */
					for (i = 0; i < NumRates; i++) {
						if (pmlmeext->datarate[i] > max_rx_rate)
							pmlmeext->datarate[i] = 0xff;
					}

				}
#ifdef CONFIG_80211N_HT
				else if (max_rx_rate < 0x1c) { /* mcs0~mcs15 */
					u32 mcs_bitmap = 0x0;

					for (i = 0; i < ((max_rx_rate + 1) - 0xc); i++)
						mcs_bitmap |= BIT(i);

					set_mcs_rate_by_mask(pmlmeext->default_supported_mcs_set, mcs_bitmap);
				}
#endif /* CONFIG_80211N_HT							 */
			}
		}
			break;
		case 0x1c: { /* enable/disable driver control AMPDU Density for peer sta's rx */
			if (arg == 0) {
				RTW_INFO("disable driver ctrl ampdu density\n");
				padapter->driver_ampdu_spacing = 0xFF;
			} else if (arg == 1) {

				RTW_INFO("enable driver ctrl ampdu density = %d\n", extra_arg);

				if (extra_arg > 0x07)
					padapter->driver_ampdu_spacing = 0xFF;
				else
					padapter->driver_ampdu_spacing = extra_arg;
			}
		}
			break;


#if defined(CONFIG_SDIO_HCI) && defined(CONFIG_SDIO_INDIRECT_ACCESS) && defined(DBG_SDIO_INDIRECT_ACCESS)
		case 0x1f:
			{
				int i, j = 0, test_cnts = 0;
				static u8 test_code = 0x5A;
				static u32 data_misatch_cnt = 0, d_acc_err_cnt = 0;

				u32 d_data, i_data;
				u32 imr;

				test_cnts = extra_arg;
				for (i = 0; i < test_cnts; i++) {
					if (RTW_CANNOT_IO(adapter_to_dvobj(padapter)))
						break;

					rtw_write8(padapter, 0x07, test_code);

					d_data = rtw_read32(padapter, 0x04);
					imr =  rtw_read32(padapter, 0x10250014);
					rtw_write32(padapter, 0x10250014, 0);
					rtw_msleep_os(50);

					i_data = rtw_sd_iread32(padapter, 0x04);

					rtw_write32(padapter, 0x10250014, imr);

					if (d_data != i_data) {
						data_misatch_cnt++;
						RTW_ERR("d_data :0x%08x, i_data : 0x%08x\n", d_data, i_data);
					}

					if (test_code != (i_data >> 24)) {
						d_acc_err_cnt++;
						rtw_write8(padapter, 0x07, 0xAA);
						RTW_ERR("test_code :0x%02x, i_data : 0x%08x\n", test_code, i_data);
					}
					if ((j++) == 100) {
						rtw_msleep_os(2000);
						RTW_INFO(" Indirect access testing..........%d/%d\n", i, test_cnts);
						j = 0;
					}

					test_code = ~test_code;
					rtw_msleep_os(50);
				}
				RTW_INFO("========Indirect access test=========\n");
				RTW_INFO(" test_cnts = %d\n", test_cnts);
				RTW_INFO(" direct & indirect read32 data missatch cnts = %d\n", data_misatch_cnt);
				RTW_INFO(" indirect rdata is not equal to wdata cnts = %d\n", d_acc_err_cnt);
				RTW_INFO("========Indirect access test=========\n\n");
				data_misatch_cnt = d_acc_err_cnt = 0;

			}
			break;
#endif
		case 0x20:
			{
				if (arg == 0xAA) {
					u8 page_offset, page_num;

					page_offset = (u8)(extra_arg >> 16);
					page_num = (u8)(extra_arg & 0xFF);
					rtw_hal_dump_rsvd_page(RTW_DBGDUMP, padapter, page_offset, page_num);
				}
#ifdef CONFIG_SUPPORT_FIFO_DUMP
				else {
					u8 fifo_sel;
					u32 addr, size;

					fifo_sel = (u8)(arg & 0x0F);
					addr = (extra_arg >> 16) & 0xFFFF;
					size = extra_arg & 0xFFFF;
					rtw_dump_fifo(RTW_DBGDUMP, padapter, fifo_sel, addr, size);
				}
#endif
			}
			break;

		case 0x23: {
			RTW_INFO("turn %s the bNotifyChannelChange Variable\n", (extra_arg == 1) ? "on" : "off");
			padapter->bNotifyChannelChange = extra_arg;
			break;
		}
		case 0x24: {
#ifdef CONFIG_P2P
			RTW_INFO("turn %s the bShowGetP2PState Variable\n", (extra_arg == 1) ? "on" : "off");
			padapter->bShowGetP2PState = extra_arg;
#endif /* CONFIG_P2P */
			break;
		}
#ifdef CONFIG_GPIO_API
		case 0x25: { /* Get GPIO register */
			/*
			* dbg 0x7f250000 [gpio_num], Get gpio value, gpio_num:0~7
			*/

			u8 value;
			RTW_INFO("Read GPIO Value  extra_arg = %d\n", extra_arg);
			value = rtw_hal_get_gpio(padapter, extra_arg);
			RTW_INFO("Read GPIO Value = %d\n", value);
			break;
		}
		case 0x26: { /* Set GPIO direction */

			/* dbg 0x7f26000x [y], Set gpio direction,
			* x: gpio_num,4~7  y: indicate direction, 0~1
			*/

			int value;
			RTW_INFO("Set GPIO Direction! arg = %d ,extra_arg=%d\n", arg , extra_arg);
			value = rtw_hal_config_gpio(padapter, arg, extra_arg);
			RTW_INFO("Set GPIO Direction %s\n", (value == -1) ? "Fail!!!" : "Success");
			break;
		}
		case 0x27: { /* Set GPIO output direction value */
			/*
			* dbg 0x7f27000x [y], Set gpio output direction value,
			* x: gpio_num,4~7  y: indicate direction, 0~1
			*/

			int value;
			RTW_INFO("Set GPIO Value! arg = %d ,extra_arg=%d\n", arg , extra_arg);
			value = rtw_hal_set_gpio_output_value(padapter, arg, extra_arg);
			RTW_INFO("Set GPIO Value %s\n", (value == -1) ? "Fail!!!" : "Success");
			break;
		}
#endif
#ifdef DBG_CMD_QUEUE
		case 0x28: {
			dump_cmd_id = extra_arg;
			RTW_INFO("dump_cmd_id:%d\n", dump_cmd_id);
		}
			break;
#endif /* DBG_CMD_QUEUE */
		case 0xaa: {
			if ((extra_arg & 0x7F) > 0x3F)
				extra_arg = 0xFF;
			RTW_INFO("chang data rate to :0x%02x\n", extra_arg);
			padapter->fix_rate = extra_arg;
		}
			break;
		case 0xdd: { /* registers dump , 0 for mac reg,1 for bb reg, 2 for rf reg */
			if (extra_arg == 0)
				mac_reg_dump(RTW_DBGDUMP, padapter);
			else if (extra_arg == 1)
				bb_reg_dump(RTW_DBGDUMP, padapter);
			else if (extra_arg == 2)
				rf_reg_dump(RTW_DBGDUMP, padapter);
			else if (extra_arg == 11)
				bb_reg_dump_ex(RTW_DBGDUMP, padapter);
		}
			break;

		case 0xee: {
			RTW_INFO(" === please control /proc  to trun on/off PHYDM func ===\n");
		}
			break;

		case 0xfd:
			rtw_write8(padapter, 0xc50, arg);
			RTW_INFO("wr(0xc50)=0x%x\n", rtw_read8(padapter, 0xc50));
			rtw_write8(padapter, 0xc58, arg);
			RTW_INFO("wr(0xc58)=0x%x\n", rtw_read8(padapter, 0xc58));
			break;
		case 0xfe:
			RTW_INFO("rd(0xc50)=0x%x\n", rtw_read8(padapter, 0xc50));
			RTW_INFO("rd(0xc58)=0x%x\n", rtw_read8(padapter, 0xc58));
			break;
		case 0xff: {
			RTW_INFO("dbg(0x210)=0x%x\n", rtw_read32(padapter, 0x210));
			RTW_INFO("dbg(0x608)=0x%x\n", rtw_read32(padapter, 0x608));
			RTW_INFO("dbg(0x280)=0x%x\n", rtw_read32(padapter, 0x280));
			RTW_INFO("dbg(0x284)=0x%x\n", rtw_read32(padapter, 0x284));
			RTW_INFO("dbg(0x288)=0x%x\n", rtw_read32(padapter, 0x288));

			RTW_INFO("dbg(0x664)=0x%x\n", rtw_read32(padapter, 0x664));


			RTW_INFO("\n");

			RTW_INFO("dbg(0x430)=0x%x\n", rtw_read32(padapter, 0x430));
			RTW_INFO("dbg(0x438)=0x%x\n", rtw_read32(padapter, 0x438));

			RTW_INFO("dbg(0x440)=0x%x\n", rtw_read32(padapter, 0x440));

			RTW_INFO("dbg(0x458)=0x%x\n", rtw_read32(padapter, 0x458));

			RTW_INFO("dbg(0x484)=0x%x\n", rtw_read32(padapter, 0x484));
			RTW_INFO("dbg(0x488)=0x%x\n", rtw_read32(padapter, 0x488));

			RTW_INFO("dbg(0x444)=0x%x\n", rtw_read32(padapter, 0x444));
			RTW_INFO("dbg(0x448)=0x%x\n", rtw_read32(padapter, 0x448));
			RTW_INFO("dbg(0x44c)=0x%x\n", rtw_read32(padapter, 0x44c));
			RTW_INFO("dbg(0x450)=0x%x\n", rtw_read32(padapter, 0x450));
		}
			break;
		}
		break;
	default:
		RTW_INFO("error dbg cmd!\n");
		break;
	}


	return ret;

}
#endif

static int wpa_set_param(struct net_device *dev, u8 name, u32 value)
{
	uint ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	switch (name) {
	case IEEE_PARAM_WPA_ENABLED:

		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_8021X; /* 802.1x */

		/* ret = ieee80211_wpa_enable(ieee, value); */

		switch ((value) & 0xff) {
		case 1: /* WPA */
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeWPAPSK; /* WPA_PSK */
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption2Enabled;
			break;
		case 2: /* WPA2 */
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeWPA2PSK; /* WPA2_PSK */
			padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption3Enabled;
			break;
		}


		break;

	case IEEE_PARAM_TKIP_COUNTERMEASURES:
		/* ieee->tkip_countermeasures=value; */
		break;

	case IEEE_PARAM_DROP_UNENCRYPTED: {
		/* HACK:
		 *
		 * wpa_supplicant calls set_wpa_enabled when the driver
		 * is loaded and unloaded, regardless of if WPA is being
		 * used.  No other calls are made which can be used to
		 * determine if encryption will be used or not prior to
		 * association being expected.  If encryption is not being
		 * used, drop_unencrypted is set to false, else true -- we
		 * can use this to determine if the CAP_PRIVACY_ON bit should
		 * be set.
		 */

#if 0
		struct ieee80211_security sec = {
			.flags = SEC_ENABLED,
			.enabled = value,
		};
		ieee->drop_unencrypted = value;
		/* We only change SEC_LEVEL for open mode. Others
		 * are set by ipw_wpa_set_encryption.
		 */
		if (!value) {
			sec.flags |= SEC_LEVEL;
			sec.level = SEC_LEVEL_0;
		} else {
			sec.flags |= SEC_LEVEL;
			sec.level = SEC_LEVEL_1;
		}
		if (ieee->set_security)
			ieee->set_security(ieee->dev, &sec);
#endif
		break;

	}
	case IEEE_PARAM_PRIVACY_INVOKED:

		/* ieee->privacy_invoked=value; */

		break;

	case IEEE_PARAM_AUTH_ALGS:

		ret = wpa_set_auth_algs(dev, value);

		break;

	case IEEE_PARAM_IEEE_802_1X:

		/* ieee->ieee802_1x=value;		 */

		break;

	case IEEE_PARAM_WPAX_SELECT:

		/* added for WPA2 mixed mode */
		/*RTW_WARN("------------------------>wpax value = %x\n", value);*/
		/*
		spin_lock_irqsave(&ieee->wpax_suitlist_lock,flags);
		ieee->wpax_type_set = 1;
		ieee->wpax_type_notify = value;
		spin_unlock_irqrestore(&ieee->wpax_suitlist_lock,flags);
		*/

		break;

	default:



		ret = -EOPNOTSUPP;


		break;

	}

	return ret;

}

static int wpa_mlme(struct net_device *dev, u32 command, u32 reason)
{
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	switch (command) {
	case IEEE_MLME_STA_DEAUTH:

		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;

		break;

	case IEEE_MLME_STA_DISASSOC:

		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;

		break;

	default:
		ret = -EOPNOTSUPP;
		break;
	}
	return ret;

}

static int wpa_supplicant_ioctl(struct net_device *dev, struct iw_point *p)
{
	struct ieee_param *param;
	uint ret = 0;

	/* down(&ieee->wx_sem);	 */

	if (p->length < sizeof(struct ieee_param) || !p->pointer) {
		ret = -EINVAL;
		goto out;
	}

	param = (struct ieee_param *)rtw_malloc(p->length);
	if (param == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	if (copy_from_user(param, p->pointer, p->length)) {
		rtw_mfree((u8 *)param, p->length);
		ret = -EFAULT;
		goto out;
	}

	switch (param->cmd) {

	case IEEE_CMD_SET_WPA_PARAM:
		ret = wpa_set_param(dev, param->u.wpa_param.name, param->u.wpa_param.value);
		break;

	case IEEE_CMD_SET_WPA_IE:
		/* ret = wpa_set_wpa_ie(dev, param, p->length); */
		ret =  rtw_set_wpa_ie((_adapter *)rtw_netdev_priv(dev), (char *)param->u.wpa_ie.data, (u16)param->u.wpa_ie.len);
		break;

	case IEEE_CMD_SET_ENCRYPTION:
		ret = wpa_set_encryption(dev, param, p->length);
		break;

	case IEEE_CMD_MLME:
		ret = wpa_mlme(dev, param->u.mlme.command, param->u.mlme.reason_code);
		break;

	default:
		RTW_INFO("Unknown WPA supplicant request: %d\n", param->cmd);
		ret = -EOPNOTSUPP;
		break;

	}

	if (ret == 0 && copy_to_user(p->pointer, param, p->length))
		ret = -EFAULT;

	rtw_mfree((u8 *)param, p->length);

out:

	/* up(&ieee->wx_sem); */

	return ret;

}

#ifdef CONFIG_AP_MODE
static int rtw_set_encryption(struct net_device *dev, struct ieee_param *param, u32 param_len)
{
	int ret = 0;
	u32 wep_key_idx, wep_key_len, wep_total_len = 0;
	NDIS_802_11_WEP	*pwep = NULL;
	struct sta_info *psta = NULL, *pbcmc_sta = NULL;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &(padapter->securitypriv);
	struct sta_priv *pstapriv = &padapter->stapriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv	*lmlmepriv = &padapter_link->mlmepriv;
	struct link_security_priv *lsecuritypriv = &(padapter_link->securitypriv);

	RTW_INFO("%s\n", __FUNCTION__);

	param->u.crypt.err = 0;
	param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

	/* sizeof(struct ieee_param) = 64 bytes; */
	/* if (param_len !=  (u32) ((u8 *) param->u.crypt.key - (u8 *) param) + param->u.crypt.key_len) */
	if (param_len !=  sizeof(struct ieee_param) + param->u.crypt.key_len) {
		ret =  -EINVAL;
		goto exit;
	}

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {
		if (param->u.crypt.idx >= WEP_KEYS
#ifdef CONFIG_IEEE80211W
		    && param->u.crypt.idx > BIP_MAX_KEYID
#endif /* CONFIG_IEEE80211W */
		   ) {
			ret = -EINVAL;
			goto exit;
		}
	} else {
		psta = rtw_get_stainfo(pstapriv, param->sta_addr);
		if (!psta) {
			/* ret = -EINVAL; */
			RTW_INFO("rtw_set_encryption(), sta has already been removed or never been added\n");
			goto exit;
		}
	}

	if (strcmp(param->u.crypt.alg, "none") == 0 && (psta == NULL)) {
		/* todo:clear default encryption keys */

		psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
		psecuritypriv->ndisencryptstatus = Ndis802_11EncryptionDisabled;
		psecuritypriv->dot11PrivacyAlgrthm = _NO_PRIVACY_;
		psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;

		RTW_INFO("clear default encryption keys, keyid=%d\n", param->u.crypt.idx);

		goto exit;
	}


	if (strcmp(param->u.crypt.alg, "WEP") == 0 && (psta == NULL)) {
		RTW_INFO("r871x_set_encryption, crypt.alg = WEP\n");

		wep_key_idx = param->u.crypt.idx;
		wep_key_len = param->u.crypt.key_len;

		RTW_INFO("r871x_set_encryption, wep_key_idx=%d, len=%d\n", wep_key_idx, wep_key_len);

		if ((wep_key_idx >= WEP_KEYS) || (wep_key_len <= 0)) {
			ret = -EINVAL;
			goto exit;
		}


		if (wep_key_len > 0) {
			wep_key_len = wep_key_len <= 5 ? 5 : 13;
			wep_total_len = wep_key_len + FIELD_OFFSET(NDIS_802_11_WEP, KeyMaterial);
			pwep = (NDIS_802_11_WEP *)rtw_malloc(wep_total_len);
			if (pwep == NULL) {
				RTW_INFO(" r871x_set_encryption: pwep allocate fail !!!\n");
				goto exit;
			}

			_rtw_memset(pwep, 0, wep_total_len);

			pwep->KeyLength = wep_key_len;
			pwep->Length = wep_total_len;

		}

		pwep->KeyIndex = wep_key_idx;

		_rtw_memcpy(pwep->KeyMaterial,  param->u.crypt.key, pwep->KeyLength);

		if (param->u.crypt.set_tx) {
			RTW_INFO("wep, set_tx=1\n");

			psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
			psecuritypriv->ndisencryptstatus = Ndis802_11Encryption1Enabled;
			psecuritypriv->dot11PrivacyAlgrthm = _WEP40_;
			psecuritypriv->dot118021XGrpPrivacy = _WEP40_;

			if (pwep->KeyLength == 13) {
				psecuritypriv->dot11PrivacyAlgrthm = _WEP104_;
				psecuritypriv->dot118021XGrpPrivacy = _WEP104_;
			}


			psecuritypriv->dot11PrivacyKeyIndex = wep_key_idx;

			_rtw_memcpy(&(psecuritypriv->dot11DefKey[wep_key_idx].skey[0]), pwep->KeyMaterial, pwep->KeyLength);

			psecuritypriv->dot11DefKeylen[wep_key_idx] = pwep->KeyLength;

			rtw_ap_set_wep_key(padapter,padapter_link,
					pwep->KeyMaterial, pwep->KeyLength, wep_key_idx, 1);
		} else {
			RTW_INFO("wep, set_tx=0\n");

			/* don't update "psecuritypriv->dot11PrivacyAlgrthm" and  */
			/* "psecuritypriv->dot11PrivacyKeyIndex=keyid", but can rtw_set_key to cam */

			_rtw_memcpy(&(psecuritypriv->dot11DefKey[wep_key_idx].skey[0]), pwep->KeyMaterial, pwep->KeyLength);

			psecuritypriv->dot11DefKeylen[wep_key_idx] = pwep->KeyLength;

			rtw_ap_set_wep_key(padapter, padapter_link,
					pwep->KeyMaterial, pwep->KeyLength, wep_key_idx, 0);
		}

		goto exit;

	}


	if (!psta && check_fwstate(pmlmepriv, WIFI_AP_STATE)) /*  */ { /* group key */
		if (param->u.crypt.set_tx == 1) {
			if (strcmp(param->u.crypt.alg, "WEP") == 0) {
				RTW_INFO(FUNC_ADPT_FMT" set WEP TX GTK idx:%u, len:%u\n"
					, FUNC_ADPT_ARG(padapter), param->u.crypt.idx, param->u.crypt.key_len);
				psecuritypriv->dot118021XGrpPrivacy = _WEP40_;
				_rtw_memcpy(lsecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,  param->u.crypt.key, (param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));
				if (param->u.crypt.key_len == 13)
					psecuritypriv->dot118021XGrpPrivacy = _WEP104_;

			} else if (strcmp(param->u.crypt.alg, "TKIP") == 0) {
				RTW_INFO(FUNC_ADPT_FMT" set TKIP TX GTK idx:%u, len:%u\n"
					, FUNC_ADPT_ARG(padapter), param->u.crypt.idx, param->u.crypt.key_len);
				psecuritypriv->dot118021XGrpPrivacy = _TKIP_;
				_rtw_memcpy(lsecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,  param->u.crypt.key, (param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));
				/* set mic key */
				_rtw_memcpy(lsecuritypriv->dot118021XGrptxmickey[param->u.crypt.idx].skey, &(param->u.crypt.key[16]), 8);
				_rtw_memcpy(lsecuritypriv->dot118021XGrprxmickey[param->u.crypt.idx].skey, &(param->u.crypt.key[24]), 8);
				psecuritypriv->busetkipkey = _TRUE;

			} else if (strcmp(param->u.crypt.alg, "CCMP") == 0) {
				RTW_INFO(FUNC_ADPT_FMT" set CCMP TX GTK idx:%u, len:%u\n"
					, FUNC_ADPT_ARG(padapter), param->u.crypt.idx, param->u.crypt.key_len);
				psecuritypriv->dot118021XGrpPrivacy = _AES_;
				_rtw_memcpy(lsecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,  param->u.crypt.key, (param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));

			#ifdef CONFIG_IEEE80211W
			} else if (strcmp(param->u.crypt.alg, "BIP") == 0) {
				RTW_INFO(FUNC_ADPT_FMT" set TX IGTK idx:%u, len:%u\n"
					, FUNC_ADPT_ARG(padapter), param->u.crypt.idx, param->u.crypt.key_len);
				_rtw_memcpy(lsecuritypriv->dot11wBIPKey[param->u.crypt.idx].skey, param->u.crypt.key, (param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));
				lsecuritypriv->dot11wBIPKeyid = param->u.crypt.idx;
				lsecuritypriv->dot11wBIPtxpn.val = RTW_GET_LE64(param->u.crypt.seq);
				lsecuritypriv->binstallBIPkey = _TRUE;
				goto exit;
			#endif /* CONFIG_IEEE80211W */

			} else if (strcmp(param->u.crypt.alg, "none") == 0) {
				RTW_INFO(FUNC_ADPT_FMT" clear group key, idx:%u\n"
					, FUNC_ADPT_ARG(padapter), param->u.crypt.idx);
				psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;
			} else {
				RTW_WARN(FUNC_ADPT_FMT" set group key, not support\n"
					, FUNC_ADPT_ARG(padapter));
				goto exit;
			}

			lsecuritypriv->dot118021XGrpKeyid = param->u.crypt.idx;
			pbcmc_sta = rtw_get_bcmc_stainfo(padapter, padapter_link);
			if (pbcmc_sta) {
				pbcmc_sta->dot11txpn.val = RTW_GET_LE64(param->u.crypt.seq);
				pbcmc_sta->ieee8021x_blocked = _FALSE;
				pbcmc_sta->dot118021XPrivacy = psecuritypriv->dot118021XGrpPrivacy; /* rx will use bmc_sta's dot118021XPrivacy			 */
			}
			lsecuritypriv->binstallGrpkey = _TRUE;
			psecuritypriv->dot11PrivacyAlgrthm = psecuritypriv->dot118021XGrpPrivacy;/* !!! */

			rtw_ap_set_group_key(padapter, padapter_link, param->u.crypt.key, psecuritypriv->dot118021XGrpPrivacy, param->u.crypt.idx);
		}

		goto exit;

	}

	if (psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X && psta) { /* psk/802_1x */
		if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
			if (param->u.crypt.set_tx == 1) {
				_rtw_memcpy(psta->dot118021x_UncstKey.skey,  param->u.crypt.key, (param->u.crypt.key_len > 16 ? 16 : param->u.crypt.key_len));

				if (strcmp(param->u.crypt.alg, "WEP") == 0) {
					RTW_INFO(FUNC_ADPT_FMT" set WEP PTK of "MAC_FMT" idx:%u, len:%u\n"
						, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr)
						, param->u.crypt.idx, param->u.crypt.key_len);
					psta->dot118021XPrivacy = _WEP40_;
					if (param->u.crypt.key_len == 13)
						psta->dot118021XPrivacy = _WEP104_;

				} else if (strcmp(param->u.crypt.alg, "TKIP") == 0) {
					RTW_INFO(FUNC_ADPT_FMT" set TKIP PTK of "MAC_FMT" idx:%u, len:%u\n"
						, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr)
						, param->u.crypt.idx, param->u.crypt.key_len);
					psta->dot118021XPrivacy = _TKIP_;
					/* set mic key */
					_rtw_memcpy(psta->dot11tkiptxmickey.skey, &(param->u.crypt.key[16]), 8);
					_rtw_memcpy(psta->dot11tkiprxmickey.skey, &(param->u.crypt.key[24]), 8);
					psecuritypriv->busetkipkey = _TRUE;

				} else if (strcmp(param->u.crypt.alg, "CCMP") == 0) {
					RTW_INFO(FUNC_ADPT_FMT" set CCMP PTK of "MAC_FMT" idx:%u, len:%u\n"
						, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr)
						, param->u.crypt.idx, param->u.crypt.key_len);
					psta->dot118021XPrivacy = _AES_;

				} else if (strcmp(param->u.crypt.alg, "none") == 0) {
					RTW_INFO(FUNC_ADPT_FMT" clear pairwise key of "MAC_FMT" idx:%u\n"
						, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr)
						, param->u.crypt.idx);
					psta->dot118021XPrivacy = _NO_PRIVACY_;

				} else {
					RTW_WARN(FUNC_ADPT_FMT" set pairwise key of "MAC_FMT", not support\n"
						, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr));
					goto exit;
				}

				psta->dot11txpn.val = RTW_GET_LE64(param->u.crypt.seq);
				psta->dot11rxpn.val = RTW_GET_LE64(param->u.crypt.seq);
				psta->ieee8021x_blocked = _FALSE;

				if (psta->dot118021XPrivacy != _NO_PRIVACY_) {
					psta->bpairwise_key_installed = _TRUE;

					/* WPA2 key-handshake has completed */
					if (psecuritypriv->ndisauthtype == Ndis802_11AuthModeWPA2PSK)
						psta->state &= (~WIFI_UNDER_KEY_HANDSHAKE);
				}

				rtw_ap_set_pairwise_key(padapter, psta);
			} else {
				RTW_WARN(FUNC_ADPT_FMT" set group key of "MAC_FMT", not support\n"
					, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr));
				goto exit;
			}

		}

	}

exit:

	if (pwep)
		rtw_mfree((u8 *)pwep, wep_total_len);

	return ret;

}

static int rtw_set_beacon(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct sta_priv *pstapriv = &padapter->stapriv;
	unsigned char *pbuf = param->u.bcn_ie.buf;


	RTW_INFO("%s, len=%d\n", __FUNCTION__, len);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;

	_rtw_memcpy(&pstapriv->max_num_sta, param->u.bcn_ie.reserved, 2);

	if ((pstapriv->max_num_sta > NUM_STA) || (pstapriv->max_num_sta <= 0))
		pstapriv->max_num_sta = NUM_STA;


	if (rtw_check_beacon_data(padapter, pbuf, (len - 12 - 2)) == _SUCCESS) /* 12 = param header, 2:no packed */
		ret = 0;
	else
		ret = -EINVAL;


	return ret;

}

static int rtw_hostapd_sta_flush(struct net_device *dev)
{
	/* _list	*phead, *plist; */
	int ret = 0;
	/* struct sta_info *psta = NULL; */
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* struct sta_priv *pstapriv = &padapter->stapriv; */

	RTW_INFO("%s\n", __FUNCTION__);

	flush_all_cam_entry(padapter, PHL_CMD_WAIT, 50);	/* clear CAM */
#ifdef CONFIG_AP_MODE
	ret = rtw_sta_flush(padapter, _TRUE);
#endif
	return ret;

}

static int rtw_add_sta(struct net_device *dev, struct ieee_param *param)
{
	int ret = 0;
	struct sta_info *psta = NULL;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct link_mlme_priv *pmlmepriv;

	RTW_INFO("rtw_add_sta(aid=%d)=" MAC_FMT "\n", param->u.add_sta.aid, MAC_ARG(param->sta_addr));

	if (check_fwstate(&padapter->mlmepriv, (WIFI_ASOC_STATE | WIFI_AP_STATE)) != _TRUE)
		return -EINVAL;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff)
		return -EINVAL;

#if 0
	psta = rtw_get_stainfo(pstapriv, param->sta_addr);
	if (psta) {
		RTW_INFO("rtw_add_sta(), free has been added psta=%p\n", psta);
		/* _rtw_spinlock_bh(&(pstapriv->sta_hash_lock));		 */
		rtw_free_stainfo(padapter,  psta);
		/* _rtw_spinunlock_bh(&(pstapriv->sta_hash_lock)); */

		psta = NULL;
	}
#endif
	/* psta = rtw_alloc_stainfo(pstapriv, param->sta_addr); */
	psta = rtw_get_stainfo(pstapriv, param->sta_addr);
	if (psta) {
		int flags = param->u.add_sta.flags;
		pmlmepriv = &psta->padapter_link->mlmepriv;

		/* RTW_INFO("rtw_add_sta(), init sta's variables, psta=%p\n", psta); */

		psta->phl_sta->aid = param->u.add_sta.aid;/* aid=1~2007 */

		_rtw_memcpy(psta->bssrateset, param->u.add_sta.tx_supp_rates, 16);


		/* check wmm cap. */
		if (WLAN_STA_WME & flags)
			psta->qos_option = 1;
		else
			psta->qos_option = 0;

		if (pmlmepriv->qospriv.qos_option == 0)
			psta->qos_option = 0;


#ifdef CONFIG_80211N_HT
		/* chec 802.11n ht cap. */
		if (padapter->registrypriv.ht_enable &&
			is_supported_ht(padapter->registrypriv.wireless_mode) &&
			(WLAN_STA_HT & flags)) {
			psta->htpriv.ht_option = _TRUE;
			psta->qos_option = 1;
			_rtw_memcpy((void *)&psta->htpriv.ht_cap, (void *)&param->u.add_sta.ht_cap, sizeof(struct rtw_ieee80211_ht_cap));
		} else
			psta->htpriv.ht_option = _FALSE;

		if (pmlmepriv->htpriv.ht_option == _FALSE)
			psta->htpriv.ht_option = _FALSE;

#endif


		update_sta_info_apmode(padapter, psta);


	} else
		ret = -ENOMEM;

	return ret;

}

static int rtw_del_sta(struct net_device *dev, struct ieee_param *param)
{
	int ret = 0;
	struct sta_info *psta = NULL;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct sta_priv *pstapriv = &padapter->stapriv;

	RTW_INFO("rtw_del_sta=" MAC_FMT "\n", MAC_ARG(param->sta_addr));

	if (check_fwstate(pmlmepriv, (WIFI_ASOC_STATE | WIFI_AP_STATE)) != _TRUE)
		return -EINVAL;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff)
		return -EINVAL;

	psta = rtw_get_stainfo(pstapriv, param->sta_addr);
	if (psta) {
		u8 updated = _FALSE;

		/* RTW_INFO("free psta=%p, aid=%d\n", psta, psta->phl_sta->aid); */

		_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
		if (rtw_is_list_empty(&psta->asoc_list) == _FALSE) {
			rtw_list_delete(&psta->asoc_list);
			pstapriv->asoc_list_cnt--;
			#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
			if (psta->tbtx_enable)
				pstapriv->tbtx_asoc_list_cnt--;
			#endif
			updated = ap_free_sta(padapter, psta, _TRUE, WLAN_REASON_DEAUTH_LEAVING, _TRUE, _FALSE);

		}
		_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

		associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);

		psta = NULL;

	} else {
		RTW_INFO("rtw_del_sta(), sta has already been removed or never been added\n");

		/* ret = -1; */
	}


	return ret;

}

static int rtw_ioctl_get_sta_data(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	struct sta_info *psta = NULL;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct ieee_param_ex *param_ex = (struct ieee_param_ex *)param;
	struct sta_data *psta_data = (struct sta_data *)param_ex->data;

	RTW_INFO("rtw_ioctl_get_sta_info, sta_addr: " MAC_FMT "\n", MAC_ARG(param_ex->sta_addr));

	if (check_fwstate(pmlmepriv, (WIFI_ASOC_STATE | WIFI_AP_STATE)) != _TRUE)
		return -EINVAL;

	if (param_ex->sta_addr[0] == 0xff && param_ex->sta_addr[1] == 0xff &&
	    param_ex->sta_addr[2] == 0xff && param_ex->sta_addr[3] == 0xff &&
	    param_ex->sta_addr[4] == 0xff && param_ex->sta_addr[5] == 0xff)
		return -EINVAL;

	psta = rtw_get_stainfo(pstapriv, param_ex->sta_addr);
	if (psta) {
#if 0
		struct {
			u16 aid;
			u16 capability;
			int flags;
			u32 sta_set;
			u8 tx_supp_rates[16];
			u32 tx_supp_rates_len;
			struct rtw_ieee80211_ht_cap ht_cap;
			u64	rx_pkts;
			u64	rx_bytes;
			u64	rx_drops;
			u64	tx_pkts;
			u64	tx_bytes;
			u64	tx_drops;
		} get_sta;
#endif
		psta_data->aid = (u16)psta->phl_sta->aid;
		psta_data->capability = psta->capability;
		psta_data->flags = psta->flags;

		/*
				nonerp_set : BIT(0)
				no_short_slot_time_set : BIT(1)
				no_short_preamble_set : BIT(2)
				no_ht_gf_set : BIT(3)
				no_ht_set : BIT(4)
				ht_20mhz_set : BIT(5)
		*/

		psta_data->sta_set = ((psta->nonerp_set) |
				      (psta->no_short_slot_time_set << 1) |
				      (psta->no_short_preamble_set << 2) |
				      (psta->no_ht_gf_set << 3) |
				      (psta->no_ht_set << 4) |
				      (psta->ht_20mhz_set << 5));

		psta_data->tx_supp_rates_len =  psta->bssratelen;
		_rtw_memcpy(psta_data->tx_supp_rates, psta->bssrateset, psta->bssratelen);
#ifdef CONFIG_80211N_HT
		if(padapter->registrypriv.ht_enable && is_supported_ht(padapter->registrypriv.wireless_mode))
			_rtw_memcpy(&psta_data->ht_cap, &psta->htpriv.ht_cap, sizeof(struct rtw_ieee80211_ht_cap));
#endif /* CONFIG_80211N_HT */
		psta_data->rx_pkts = psta->sta_stats.rx_data_pkts;
		psta_data->rx_bytes = psta->sta_stats.rx_bytes;
		psta_data->rx_drops = psta->sta_stats.rx_drops;

		psta_data->tx_pkts = psta->sta_stats.tx_pkts;
		psta_data->tx_bytes = psta->sta_stats.tx_bytes;
		psta_data->tx_drops = psta->sta_stats.tx_drops;


	} else
		ret = -1;

	return ret;

}

static int rtw_get_sta_wpaie(struct net_device *dev, struct ieee_param *param)
{
	int ret = 0;
	struct sta_info *psta = NULL;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct sta_priv *pstapriv = &padapter->stapriv;

	RTW_INFO("rtw_get_sta_wpaie, sta_addr: " MAC_FMT "\n", MAC_ARG(param->sta_addr));

	if (check_fwstate(pmlmepriv, (WIFI_ASOC_STATE | WIFI_AP_STATE)) != _TRUE)
		return -EINVAL;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff)
		return -EINVAL;

	psta = rtw_get_stainfo(pstapriv, param->sta_addr);
	if (psta) {
		if ((psta->wpa_ie[0] == WLAN_EID_RSN) || (psta->wpa_ie[0] == WLAN_EID_GENERIC)) {
			int wpa_ie_len;
			int copy_len;

			wpa_ie_len = psta->wpa_ie[1];

			copy_len = ((wpa_ie_len + 2) > sizeof(psta->wpa_ie)) ? (sizeof(psta->wpa_ie)) : (wpa_ie_len + 2);

			param->u.wpa_ie.len = copy_len;

			_rtw_memcpy(param->u.wpa_ie.reserved, psta->wpa_ie, copy_len);
		} else {
			/* ret = -1; */
			RTW_INFO("sta's wpa_ie is NONE\n");
		}
	} else
		ret = -1;

	return ret;

}

static int rtw_set_wps_beacon(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	unsigned char wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	int ie_len;

	RTW_INFO("%s, len=%d\n", __FUNCTION__, len);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;

	ie_len = len - 12 - 2; /* 12 = param header, 2:no packed */


	if (pmlmepriv->wps_beacon_ie) {
		rtw_mfree(pmlmepriv->wps_beacon_ie, pmlmepriv->wps_beacon_ie_len);
		pmlmepriv->wps_beacon_ie = NULL;
	}

	if (ie_len > 0) {
		pmlmepriv->wps_beacon_ie = rtw_malloc(ie_len);
		pmlmepriv->wps_beacon_ie_len = ie_len;
		if (pmlmepriv->wps_beacon_ie == NULL) {
			RTW_INFO("%s()-%d: rtw_malloc() ERROR!\n", __FUNCTION__, __LINE__);
			return -EINVAL;
		}

		_rtw_memcpy(pmlmepriv->wps_beacon_ie, param->u.bcn_ie.buf, ie_len);

		rtw_update_beacon(padapter, padapter_link,
				_VENDOR_SPECIFIC_IE_, wps_oui, _TRUE, 0);

		pmlmeext->bstart_bss = _TRUE;

	}


	return ret;

}

static int rtw_set_wps_probe_resp(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	int ie_len;

	RTW_INFO("%s, len=%d\n", __FUNCTION__, len);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;

	ie_len = len - 12 - 2; /* 12 = param header, 2:no packed */


	if (pmlmepriv->wps_probe_resp_ie) {
		rtw_mfree(pmlmepriv->wps_probe_resp_ie, pmlmepriv->wps_probe_resp_ie_len);
		pmlmepriv->wps_probe_resp_ie = NULL;
	}

	if (ie_len > 0) {
		pmlmepriv->wps_probe_resp_ie = rtw_malloc(ie_len);
		pmlmepriv->wps_probe_resp_ie_len = ie_len;
		if (pmlmepriv->wps_probe_resp_ie == NULL) {
			RTW_INFO("%s()-%d: rtw_malloc() ERROR!\n", __FUNCTION__, __LINE__);
			return -EINVAL;
		}
		_rtw_memcpy(pmlmepriv->wps_probe_resp_ie, param->u.bcn_ie.buf, ie_len);
	}


	return ret;

}

static int rtw_set_wps_assoc_resp(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	int ie_len;

	RTW_INFO("%s, len=%d\n", __FUNCTION__, len);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;

	ie_len = len - 12 - 2; /* 12 = param header, 2:no packed */


	if (pmlmepriv->wps_assoc_resp_ie) {
		rtw_mfree(pmlmepriv->wps_assoc_resp_ie, pmlmepriv->wps_assoc_resp_ie_len);
		pmlmepriv->wps_assoc_resp_ie = NULL;
	}

	if (ie_len > 0) {
		pmlmepriv->wps_assoc_resp_ie = rtw_malloc(ie_len);
		pmlmepriv->wps_assoc_resp_ie_len = ie_len;
		if (pmlmepriv->wps_assoc_resp_ie == NULL) {
			RTW_INFO("%s()-%d: rtw_malloc() ERROR!\n", __FUNCTION__, __LINE__);
			return -EINVAL;
		}

		_rtw_memcpy(pmlmepriv->wps_assoc_resp_ie, param->u.bcn_ie.buf, ie_len);
	}


	return ret;

}

static int rtw_set_hidden_ssid(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlmepriv = &(adapter->mlmepriv);
	struct mlme_ext_priv	*mlmeext = &(adapter->mlmeextpriv);
	struct mlme_ext_info	*mlmeinfo = &(mlmeext->mlmext_info);
	int ie_len;
	u8 *ssid_ie;
	char ssid[NDIS_802_11_LENGTH_SSID + 1];
	sint ssid_len = 0;
	u8 ignore_broadcast_ssid;

	if (check_fwstate(mlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EPERM;

	if (param->u.bcn_ie.reserved[0] != 0xea)
		return -EINVAL;

	mlmeinfo->hidden_ssid_mode = ignore_broadcast_ssid = param->u.bcn_ie.reserved[1];

	ie_len = len - 12 - 2; /* 12 = param header, 2:no packed */
	ssid_ie = rtw_get_ie(param->u.bcn_ie.buf,  WLAN_EID_SSID, &ssid_len, ie_len);

	if (ssid_ie && ssid_len > 0 && ssid_len <= NDIS_802_11_LENGTH_SSID) {
		WLAN_BSSID_EX *pbss_network = &mlmepriv->dev_cur_network.network;
		WLAN_BSSID_EX *pbss_network_ext = &mlmeinfo->dev_network;

		_rtw_memcpy(ssid, ssid_ie + 2, ssid_len);
		ssid[ssid_len] = 0x0;

		if (0)
			RTW_INFO(FUNC_ADPT_FMT" ssid:(%s,%d), from ie:(%s,%d), (%s,%d)\n", FUNC_ADPT_ARG(adapter),
				ssid, ssid_len,
				pbss_network->Ssid.Ssid, pbss_network->Ssid.SsidLength,
				pbss_network_ext->Ssid.Ssid, pbss_network_ext->Ssid.SsidLength);

		_rtw_memcpy(pbss_network->Ssid.Ssid, (void *)ssid, ssid_len);
		pbss_network->Ssid.SsidLength = ssid_len;
		_rtw_memcpy(pbss_network_ext->Ssid.Ssid, (void *)ssid, ssid_len);
		pbss_network_ext->Ssid.SsidLength = ssid_len;

		if (0)
			RTW_INFO(FUNC_ADPT_FMT" after ssid:(%s,%d), (%s,%d)\n", FUNC_ADPT_ARG(adapter),
				pbss_network->Ssid.Ssid, pbss_network->Ssid.SsidLength,
				pbss_network_ext->Ssid.Ssid, pbss_network_ext->Ssid.SsidLength);
	}

	RTW_INFO(FUNC_ADPT_FMT" ignore_broadcast_ssid:%d, %s,%d\n", FUNC_ADPT_ARG(adapter),
		ignore_broadcast_ssid, ssid, ssid_len);

	return ret;
}

#if CONFIG_RTW_MACADDR_ACL
static int rtw_ioctl_acl_remove_sta(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff)
		return -EINVAL;

	ret = rtw_acl_remove_sta(padapter, RTW_ACL_PERIOD_BSS, param->sta_addr);

	return ret;

}

static int rtw_ioctl_acl_add_sta(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff)
		return -EINVAL;

	ret = rtw_acl_add_sta(padapter, RTW_ACL_PERIOD_BSS, param->sta_addr);

	return ret;

}

static int rtw_ioctl_set_macaddr_acl(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;

	rtw_set_macaddr_acl(padapter, RTW_ACL_PERIOD_BSS, param->u.mlme.command);

	return ret;
}
#endif /* CONFIG_RTW_MACADDR_ACL */

static int rtw_hostapd_ioctl(struct net_device *dev, struct iw_point *p)
{
	struct ieee_param *param;
	int ret = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	/* RTW_INFO("%s\n", __FUNCTION__); */

	/*
	* this function is expect to call in master mode, which allows no power saving
	* so, we just check hw_init_completed
	*/

	if (!rtw_hw_is_init_completed(dvobj)) {
		ret = -EPERM;
		goto out;
	}


	/* if (p->length < sizeof(struct ieee_param) || !p->pointer){ */
	if (!p->pointer) {
		ret = -EINVAL;
		goto out;
	}

	param = (struct ieee_param *)rtw_malloc(p->length);
	if (param == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	if (copy_from_user(param, p->pointer, p->length)) {
		rtw_mfree((u8 *)param, p->length);
		ret = -EFAULT;
		goto out;
	}

	/* RTW_INFO("%s, cmd=%d\n", __FUNCTION__, param->cmd); */

	switch (param->cmd) {
	case RTL871X_HOSTAPD_FLUSH:

		ret = rtw_hostapd_sta_flush(dev);

		break;

	case RTL871X_HOSTAPD_ADD_STA:

		ret = rtw_add_sta(dev, param);

		break;

	case RTL871X_HOSTAPD_REMOVE_STA:

		ret = rtw_del_sta(dev, param);

		break;

	case RTL871X_HOSTAPD_SET_BEACON:

		ret = rtw_set_beacon(dev, param, p->length);

		break;

	case RTL871X_SET_ENCRYPTION:

		ret = rtw_set_encryption(dev, param, p->length);

		break;

	case RTL871X_HOSTAPD_GET_WPAIE_STA:

		ret = rtw_get_sta_wpaie(dev, param);

		break;

	case RTL871X_HOSTAPD_SET_WPS_BEACON:

		ret = rtw_set_wps_beacon(dev, param, p->length);

		break;

	case RTL871X_HOSTAPD_SET_WPS_PROBE_RESP:

		ret = rtw_set_wps_probe_resp(dev, param, p->length);

		break;

	case RTL871X_HOSTAPD_SET_WPS_ASSOC_RESP:

		ret = rtw_set_wps_assoc_resp(dev, param, p->length);

		break;

	case RTL871X_HOSTAPD_SET_HIDDEN_SSID:

		ret = rtw_set_hidden_ssid(dev, param, p->length);

		break;

	case RTL871X_HOSTAPD_GET_INFO_STA:

		ret = rtw_ioctl_get_sta_data(dev, param, p->length);

		break;

#if CONFIG_RTW_MACADDR_ACL
	case RTL871X_HOSTAPD_SET_MACADDR_ACL:
		ret = rtw_ioctl_set_macaddr_acl(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_ACL_ADD_STA:
		ret = rtw_ioctl_acl_add_sta(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_ACL_REMOVE_STA:
		ret = rtw_ioctl_acl_remove_sta(dev, param, p->length);
		break;
#endif /* CONFIG_RTW_MACADDR_ACL */

	default:
		RTW_INFO("Unknown hostapd request: %d\n", param->cmd);
		ret = -EOPNOTSUPP;
		break;

	}

	if (ret == 0 && copy_to_user(p->pointer, param, p->length))
		ret = -EFAULT;


	rtw_mfree((u8 *)param, p->length);

out:

	return ret;

}
#endif

static int rtw_wx_set_priv(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *awrq,
			   char *extra)
{

#ifdef CONFIG_DEBUG_RTW_WX_SET_PRIV
	char *ext_dbg;
#endif

	int ret = 0;
	int len = 0;
	char *ext;
#ifdef CONFIG_RTW_ANDROID
	int i;
#endif

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct iw_point *dwrq = (struct iw_point *)awrq;

	if (dwrq->length == 0)
		return -EFAULT;

	len = dwrq->length;
	ext = rtw_vmalloc(len);
	if (!ext)
		return -ENOMEM;

	if (copy_from_user(ext, dwrq->pointer, len)) {
		rtw_vmfree(ext, len);
		return -EFAULT;
	}



#ifdef CONFIG_DEBUG_RTW_WX_SET_PRIV
	ext_dbg = rtw_vmalloc(len);
	if (!ext_dbg) {
		rtw_vmfree(ext, len);
		return -ENOMEM;
	}

	_rtw_memcpy(ext_dbg, ext, len);
#endif

	/* added for wps2.0 @20110524 */
	if (dwrq->flags == 0x8766 && len > 8) {
		u32 cp_sz;
		struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
		u8 *probereq_wpsie = ext;
		int probereq_wpsie_len = len;
		u8 wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};

		if ((_VENDOR_SPECIFIC_IE_ == probereq_wpsie[0]) &&
		    (_rtw_memcmp(&probereq_wpsie[2], wps_oui, 4) == _TRUE)) {
			cp_sz = probereq_wpsie_len > MAX_WPS_IE_LEN ? MAX_WPS_IE_LEN : probereq_wpsie_len;

			if (pmlmepriv->wps_probe_req_ie) {
				u32 free_len = pmlmepriv->wps_probe_req_ie_len;
				pmlmepriv->wps_probe_req_ie_len = 0;
				rtw_mfree(pmlmepriv->wps_probe_req_ie, free_len);
				pmlmepriv->wps_probe_req_ie = NULL;
			}

			pmlmepriv->wps_probe_req_ie = rtw_malloc(cp_sz);
			if (pmlmepriv->wps_probe_req_ie == NULL) {
				printk("%s()-%d: rtw_malloc() ERROR!\n", __FUNCTION__, __LINE__);
				ret =  -EINVAL;
				goto FREE_EXT;

			}

			_rtw_memcpy(pmlmepriv->wps_probe_req_ie, probereq_wpsie, cp_sz);
			pmlmepriv->wps_probe_req_ie_len = cp_sz;

		}

		goto FREE_EXT;

	}

	if (len >= WEXT_CSCAN_HEADER_SIZE
		&& _rtw_memcmp(ext, WEXT_CSCAN_HEADER, WEXT_CSCAN_HEADER_SIZE) == _TRUE
	) {
		ret = rtw_wx_set_scan(dev, info, awrq, ext);
		goto FREE_EXT;
	}

#ifdef CONFIG_RTW_ANDROID
	/* RTW_INFO("rtw_wx_set_priv: %s req=%s\n", dev->name, ext); */

	i = rtw_android_cmdstr_to_num(ext);

	switch (i) {
	case ANDROID_WIFI_CMD_START:
		indicate_wx_custom_event(padapter, "START");
		break;
	case ANDROID_WIFI_CMD_STOP:
		indicate_wx_custom_event(padapter, "STOP");
		break;
	case ANDROID_WIFI_CMD_RSSI: {
		struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
		struct	wlan_network	*pcur_network = &pmlmepriv->dev_cur_network;

		if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
			sprintf(ext, "%s rssi %d", pcur_network->network.Ssid.Ssid, padapter->recvinfo.rssi);
		else
			sprintf(ext, "OK");
	}
		break;
	case ANDROID_WIFI_CMD_LINKSPEED: {
		u16 mbps = rtw_get_cur_max_rate(padapter) / 10;
		sprintf(ext, "LINKSPEED %d", mbps);
	}
		break;
	case ANDROID_WIFI_CMD_MACADDR:
		sprintf(ext, "MACADDR = " MAC_FMT, MAC_ARG(dev->dev_addr));
		break;
	case ANDROID_WIFI_CMD_SCAN_ACTIVE: {
		/* rtw_set_scan_mode(padapter, SCAN_ACTIVE); */
		sprintf(ext, "OK");
	}
		break;
	case ANDROID_WIFI_CMD_SCAN_PASSIVE: {
		/* rtw_set_scan_mode(padapter, SCAN_PASSIVE); */
		sprintf(ext, "OK");
	}
		break;

	case ANDROID_WIFI_CMD_COUNTRY: {
		char country_code[10];
		sscanf(ext, "%*s %s", country_code);
		rtw_set_country(padapter, country_code, RTW_REGD_SET_BY_USER);
		sprintf(ext, "OK");
	}
		break;
	default:
		#ifdef CONFIG_DEBUG_RTW_WX_SET_PRIV
		RTW_INFO("%s: %s unknowned req=%s\n", __FUNCTION__,
			dev->name, ext_dbg);
		#endif

		sprintf(ext, "OK");

	}

	if (copy_to_user(dwrq->pointer, ext, min(dwrq->length, (u16)(strlen(ext) + 1))))
		ret = -EFAULT;

#ifdef CONFIG_DEBUG_RTW_WX_SET_PRIV
	RTW_INFO("%s: %s req=%s rep=%s dwrq->length=%d, strlen(ext)+1=%d\n", __FUNCTION__,
		dev->name, ext_dbg , ext, dwrq->length, (u16)(strlen(ext) + 1));
#endif
#endif /* end of CONFIG_ANDROID */


FREE_EXT:

	rtw_vmfree(ext, len);
	#ifdef CONFIG_DEBUG_RTW_WX_SET_PRIV
	rtw_vmfree(ext_dbg, len);
	#endif

	/* RTW_INFO("rtw_wx_set_priv: (SIOCSIWPRIV) %s ret=%d\n",  */
	/*		dev->name, ret); */

	return ret;

}
#ifdef CONFIG_WOWLAN
static int rtw_wowlan_ctrl(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);
	struct wow_priv *wowpriv = adapter_to_wowlan(padapter);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	int ret = 0;
	systime start_time = rtw_get_current_time();

	RTW_INFO("+rtw_wowlan_ctrl: %s\n", extra);

	if (!check_fwstate(pmlmepriv, WIFI_ASOC_STATE) &&
		MLME_IS_STA(padapter) &&
		!WOWLAN_IS_STA_MIX_MODE(padapter)) {
#ifdef CONFIG_PNO_SUPPORT
		if (wowpriv->wow_nlo.nlo_en) {
			pwrctrlpriv->wowlan_pno_enable = _TRUE;
		} else
#endif
		{
			RTW_INFO("[%s] WARNING: Please Connect With AP First!!\n",
				 __func__);
			goto _rtw_wowlan_ctrl_exit_free;
		}
	}

	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY))
		rtw_scan_abort(padapter, 0);

	if (_rtw_memcmp(extra, "enable", 6))


		rtw_suspend_common(padapter);

	else if (_rtw_memcmp(extra, "disable", 7)) {
#ifdef CONFIG_USB_HCI
		RTW_ENABLE_FUNC(adapter_to_dvobj(padapter), DF_RX_BIT);
		RTW_ENABLE_FUNC(adapter_to_dvobj(padapter), DF_TX_BIT);
#endif
		rtw_resume_common(padapter);

#ifdef CONFIG_PNO_SUPPORT
		pwrctrlpriv->wowlan_pno_enable = _FALSE;
#endif /* CONFIG_PNO_SUPPORT */

	} else {
		RTW_INFO("[%s] Invalid Parameter.\n", __func__);
		goto _rtw_wowlan_ctrl_exit_free;
	}
	/* mutex_lock(&ioctl_mutex); */
_rtw_wowlan_ctrl_exit_free:
	RTW_PRINT("%s in %d ms\n", __func__,
		  rtw_get_passing_time_ms(start_time));
	return ret;
}

/*
 * IP filter This pattern if for a frame containing a ip packet:
 * AA:AA:AA:AA:AA:AA:BB:BB:BB:BB:BB:BB:CC:CC:DD:-:-:-:-:-:-:-:-:EE:-:-:FF:FF:FF:FF:GG:GG:GG:GG:HH:HH:II:II
 *
 * A: Ethernet destination address
 * B: Ethernet source address
 * C: Ethernet protocol type
 * D: IP header VER+Hlen, use: 0x45 (4 is for ver 4, 5 is for len 20)
 * E: IP protocol
 * F: IP source address ( 192.168.0.4: C0:A8:00:2C )
 * G: IP destination address ( 192.168.0.4: C0:A8:00:2C )
 * H: Source port (1024: 04:00)
 * I: Destination port (1024: 04:00)
 */

static int rtw_wowlan_set_pattern(struct net_device *dev,
				  struct iw_request_info *info,
				  union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct registry_priv  *registry_par = &padapter->registrypriv;
	struct wow_priv *wowpriv = adapter_to_wowlan(padapter);
	struct rtw_wowcam_upd_info wowcam_info = {0};
	u8 input[MAX_IN_PATTERN_SIZE];
	int ret = 0;

	if (!(registry_par->wakeup_event & BIT(3))) {
		ret = -EOPNOTSUPP;
		RTW_INFO("%s: customized pattern disabled, wakeup_event: %#2x\n",
			 __func__, registry_par->wakeup_event);
		goto _rtw_wowlan_set_pattern_exit;
	}

	if (!check_fwstate(pmlmepriv, WIFI_ASOC_STATE) &&
	    MLME_IS_STA(padapter)) {
		ret = -EOPNOTSUPP;
		RTW_INFO("Please Connect With AP First!!\n");
		goto _rtw_wowlan_set_pattern_exit;
	}

	if ((wrqu->data.length <= 0) || (wrqu->data.length > MAX_IN_PATTERN_SIZE)) {
		ret = -EINVAL;
		RTW_INFO("ERROR: parameter length error, len=%d\n", wrqu->data.length);
		goto _rtw_wowlan_set_pattern_exit;
	} else {
		/* set pattern */
		if (copy_from_user(input, wrqu->data.pointer, wrqu->data.length)) {
			ret -EFAULT;
			goto _rtw_wowlan_set_pattern_exit;
		}

		if (strncmp(input, "pattern=", 8) == 0) {
			if (rtw_wowlan_parser_pattern_cmd(input,
							  wowcam_info.ptrn,
							  &wowcam_info.ptrn_len,
							  wowcam_info.mask)) {
				if (_FAIL == rtw_wow_pattern_set(padapter,
								 &wowcam_info,
								 RTW_CUSTOMIZED_PATTERN))
					ret = -EFAULT;
			} else {
				ret = -EINVAL;
			}
		} else if (strncmp(input, "clean", 5) == 0) {
			rtw_wow_pattern_clean(padapter, RTW_CUSTOMIZED_PATTERN);
		} else if (strncmp(input, "show", 4) == 0) {
			#if 0 /* WOW_ToDo */
			rtw_wow_pattern_cam_dump(padapter);
			rtw_wow_pattern_sw_dump(padapter);
			#endif
		} else {
			RTW_INFO("ERROR: incorrect parameter!\n");
			ret = -EINVAL;
		}
	}
_rtw_wowlan_set_pattern_exit:
	return ret;
}
#endif /* CONFIG_WOWLAN */

#ifdef CONFIG_AP_WOWLAN
static int rtw_ap_wowlan_ctrl(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct wowlan_ioctl_param poidparam;
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sta_info	*psta = NULL;
	int ret = 0;
	systime start_time = rtw_get_current_time();
	poidparam.subcode = 0;

	RTW_INFO("+rtw_ap_wowlan_ctrl: %s\n", extra);

	if (!check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		RTW_INFO("[%s] It is not AP mode!!\n", __func__);
		goto _rtw_ap_wowlan_ctrl_exit_free;
	}

	if (_rtw_memcmp(extra, "enable", 6)) {

		pwrctrlpriv->wowlan_ap_mode = _TRUE;

		rtw_suspend_common(padapter);
	} else if (_rtw_memcmp(extra, "disable", 7)) {
#ifdef CONFIG_USB_HCI
		RTW_ENABLE_FUNC(adapter_to_dvobj(padapter), DF_RX_BIT);
		RTW_ENABLE_FUNC(adapter_to_dvobj(padapter), DF_TX_BIT);
#endif
		rtw_resume_common(padapter);
	} else {
		RTW_INFO("[%s] Invalid Parameter.\n", __func__);
		goto _rtw_ap_wowlan_ctrl_exit_free;
	}
	/* mutex_lock(&ioctl_mutex); */
_rtw_ap_wowlan_ctrl_exit_free:
	RTW_INFO("-rtw_ap_wowlan_ctrl( subcode = %d)\n", poidparam.subcode);
	RTW_PRINT("%s in %d ms\n", __func__,
		  rtw_get_passing_time_ms(start_time));
	return ret;
}
#endif /* CONFIG_AP_WOWLAN */

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE

int rtw_vendor_ie_get_raw_data(struct net_device *dev, u32 vendor_ie_num,
							   char *extra, u32 length)
{
	int j;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u32 vendor_ie_mask = 0;
	char *pstring;

	if (vendor_ie_num >= WLAN_MAX_VENDOR_IE_NUM) {
		RTW_INFO("[%s] only support %d vendor ie\n", __func__ ,
				 WLAN_MAX_VENDOR_IE_NUM);
		return -EFAULT;
	}

	if (pmlmepriv->vendor_ielen[vendor_ie_num] == 0) {
		RTW_INFO("[%s]  Fail, vendor_ie_num: %d is not set\n", __func__,
				 vendor_ie_num);
		return -EFAULT;
	}

	if (length < 2 * pmlmepriv->vendor_ielen[vendor_ie_num] + 5) {
		RTW_INFO("[%s]  Fail, buffer size is too small\n", __func__);
		return -EFAULT;
	}

	vendor_ie_mask = pmlmepriv->vendor_ie_mask[vendor_ie_num];
	_rtw_memset(extra, 0, length);

	pstring = extra;
	pstring += sprintf(pstring, "%d,%x,", vendor_ie_num, vendor_ie_mask);

	for (j = 0; j < pmlmepriv->vendor_ielen[vendor_ie_num]; j++)
		pstring += sprintf(pstring, "%02x", pmlmepriv->vendor_ie[vendor_ie_num][j]);

	length = pstring - extra;
	return length;
}

int rtw_vendor_ie_get_data(struct net_device *dev, int vendor_ie_num, char *extra)
{
	int j;
	char *pstring;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u32 vendor_ie_mask = 0;
	__u16 length = 0;

	vendor_ie_mask = pmlmepriv->vendor_ie_mask[vendor_ie_num];
	pstring = extra;
	pstring += sprintf(pstring , "\nVendor IE num %d , Mask:%x " , vendor_ie_num , vendor_ie_mask);

	if (vendor_ie_mask & WIFI_BEACON_VENDOR_IE_BIT)
		pstring += sprintf(pstring , "[Beacon]");
	if (vendor_ie_mask & WIFI_PROBEREQ_VENDOR_IE_BIT)
		pstring += sprintf(pstring , "[Probe Req]");
	if (vendor_ie_mask & WIFI_PROBERESP_VENDOR_IE_BIT)
		pstring += sprintf(pstring , "[Probe Resp]");
	if (vendor_ie_mask & WIFI_ASSOCREQ_VENDOR_IE_BIT)
		pstring += sprintf(pstring , "[Assoc Req]");
	if (vendor_ie_mask & WIFI_ASSOCRESP_VENDOR_IE_BIT)
		pstring += sprintf(pstring , "[Assoc Resp]");
#ifdef CONFIG_P2P
	if (vendor_ie_mask & WIFI_P2P_PROBEREQ_VENDOR_IE_BIT)
		pstring += sprintf(pstring , "[P2P_Probe Req]");
	if (vendor_ie_mask & WIFI_P2P_PROBERESP_VENDOR_IE_BIT)
		pstring += sprintf(pstring , "[P2P_Probe Resp]");
#endif

	pstring += sprintf(pstring , "\nVendor IE:\n");
	for (j = 0 ; j < pmlmepriv->vendor_ielen[vendor_ie_num]  ; j++)
		pstring += sprintf(pstring , "%02x" , pmlmepriv->vendor_ie[vendor_ie_num][j]);

	length = pstring - extra;
	return length;

}

int rtw_vendor_ie_get(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
	int ret = 0, vendor_ie_num = 0, cmdlen;
	struct iw_point *p;
	u8 *ptmp;

	p = &wrqu->data;
	cmdlen = p->length;
	if (0 == cmdlen)
		return -EINVAL;

	ptmp = (u8 *)rtw_malloc(cmdlen);
	if (NULL == ptmp)
		return -ENOMEM;

	if (copy_from_user(ptmp, p->pointer, cmdlen)) {
		ret = -EFAULT;
		goto exit;
	}
	ret = sscanf(ptmp , "%d", &vendor_ie_num);
	if (vendor_ie_num > WLAN_MAX_VENDOR_IE_NUM - 1) {
		ret = -EFAULT;
		goto exit;
	}

	wrqu->data.length = rtw_vendor_ie_get_data(dev, vendor_ie_num, extra);

exit:
	rtw_mfree(ptmp, cmdlen);

	return 0;
}

int rtw_vendor_ie_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
	int ret = 0, i , len = 0 , totoal_ie_len = 0 , total_ie_len_byte = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u32 vendor_ie_mask = 0;
	u32 vendor_ie_num = 0;
	u32 vendor_ie_mask_max = BIT(WLAN_MAX_VENDOR_IE_MASK_MAX) - 1;
	u32 id, elen;

	ret = sscanf(extra, "%d,%x,%*s", &vendor_ie_num , &vendor_ie_mask);
	if (strrchr(extra , ','))
		extra = strrchr(extra , ',') + 1;
	else
		return -EINVAL;
	totoal_ie_len = strlen(extra);
	RTW_INFO("[%s] vendor_ie_num = %d , vendor_ie_mask = 0x%x , vendor_ie = %s , len = %d\n", __func__ , vendor_ie_num , vendor_ie_mask , extra  , totoal_ie_len);

	if (vendor_ie_num  > WLAN_MAX_VENDOR_IE_NUM - 1) {
		RTW_INFO("[%s] Fail, only support %d vendor ie\n", __func__ , WLAN_MAX_VENDOR_IE_NUM);
		return -EFAULT;
	}

	if (totoal_ie_len > WLAN_MAX_VENDOR_IE_LEN) {
		RTW_INFO("[%s] Fail , not support ie length extend %d\n", __func__ , WLAN_MAX_VENDOR_IE_LEN);
		return -EFAULT;
	}

	if (vendor_ie_mask > vendor_ie_mask_max) {
		RTW_INFO("[%s] Fail, not support vendor_ie_mask more than 0x%x\n", __func__ , vendor_ie_mask_max);
		return -EFAULT;
	}

	if (vendor_ie_mask == 0) {
		RTW_INFO("[%s] Clear vendor_ie_num %d group\n", __func__ , vendor_ie_num);
		goto _clear_path;
	}

	if (totoal_ie_len % 2 != 0) {
		RTW_INFO("[%s]  Fail , IE length = %zu is odd\n" , __func__ , strlen(extra));
		return -EFAULT;
	}

	if (totoal_ie_len > 0) {
		for (i = 0  ; i < strlen(extra) ; i += 2) {
			pmlmepriv->vendor_ie[vendor_ie_num][len] = key_2char2num(extra[i] , extra[i + 1]);
			if (len == 0) {
				id = pmlmepriv->vendor_ie[vendor_ie_num][len];
				if (id != WLAN_EID_VENDOR_SPECIFIC) {
					RTW_INFO("[%s] Fail , VENDOR SPECIFIC IE ID \"%x\" was not correct\n", __func__ , id);
					goto _clear_path;
				}
			} else if (len == 1) {
				total_ie_len_byte = (totoal_ie_len / 2) - 2;
				elen = pmlmepriv->vendor_ie[vendor_ie_num][len];
				if (elen != total_ie_len_byte) {
					RTW_INFO("[%s] Fail , Input IE length = \"%d\"(hex:%x) bytes , not match input total IE context length \"%d\" bytes\n", __func__ , elen , elen ,
						 total_ie_len_byte);
					goto _clear_path;
				}
			}
			len++;
		}
		pmlmepriv->vendor_ielen[vendor_ie_num] = len;
	} else
		pmlmepriv->vendor_ielen[vendor_ie_num] = 0;



	if (vendor_ie_mask & WIFI_BEACON_VENDOR_IE_BIT)
		RTW_INFO("[%s] Beacon append vendor ie\n", __func__);
	if (vendor_ie_mask & WIFI_PROBEREQ_VENDOR_IE_BIT)
		RTW_INFO("[%s] Probe Req append vendor ie\n", __func__);
	if (vendor_ie_mask & WIFI_PROBERESP_VENDOR_IE_BIT)
		RTW_INFO("[%s] Probe Resp append vendor ie\n", __func__);
	if (vendor_ie_mask & WIFI_ASSOCREQ_VENDOR_IE_BIT)
		RTW_INFO("[%s] Assoc Req append vendor ie\n", __func__);
	if (vendor_ie_mask & WIFI_ASSOCRESP_VENDOR_IE_BIT)
		RTW_INFO("[%s] Assoc Resp append vendor ie\n", __func__);
#ifdef CONFIG_P2P
	if (vendor_ie_mask & WIFI_P2P_PROBEREQ_VENDOR_IE_BIT)
		RTW_INFO("[%s] P2P Probe Req append vendor ie\n", __func__);
	if (vendor_ie_mask & WIFI_P2P_PROBERESP_VENDOR_IE_BIT)
		RTW_INFO("[%s] P2P Probe Resp append vendor ie\n", __func__);
#endif

	pmlmepriv->vendor_ie_mask[vendor_ie_num] = vendor_ie_mask;

	return ret;

_clear_path:
	_rtw_memset(pmlmepriv->vendor_ie[vendor_ie_num] , 0 , sizeof(u32) * WLAN_MAX_VENDOR_IE_LEN);
	pmlmepriv->vendor_ielen[vendor_ie_num] = 0;
	pmlmepriv->vendor_ie_mask[vendor_ie_num] = 0;
	return -EFAULT;
}
#endif

#if defined(RTW_PHL_TX) || defined(RTW_PHL_RX) || defined(CONFIG_PHL_TEST_SUITE)
int rtw_phl_test_set(struct net_device *dev,
	struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u32 mode = 0;
	u32 bytes = 0;
	int ret = 0;

#if defined(RTW_PHL_DBG_CMD)
	core_cmd_phl_handler(padapter, extra);
#endif

exit:
	return 0;
}
#endif


#ifdef CONFIG_RTW_CUSTOMER_STR
static int rtw_mp_customer_str(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu, char *extra)
{
	_adapter *adapter = rtw_netdev_priv(dev);
	u32 len;
	u8 *pbuf = NULL, *pch;
	char *ptmp;
	u8 param[RTW_CUSTOMER_STR_LEN];
	u8 count = 0;
	u8 tmp;
	u8 i;
	u32 pos;
	u8 ret;
	u8 read = 0;

	if (adapter->registrypriv.mp_mode != 1
		|| !adapter->registrypriv.mp_customer_str)
		return -EFAULT;

	len = wrqu->data.length + 1;

	pbuf = (u8 *)rtw_zmalloc(len);
	if (pbuf == NULL) {
		RTW_WARN("%s: no memory!\n", __func__);
		return -ENOMEM;
	}

	if (copy_from_user(pbuf, wrqu->data.pointer, wrqu->data.length)) {
		rtw_mfree(pbuf, len);
		RTW_WARN("%s: copy from user fail!\n", __func__);
		return -EFAULT;
	}
	RTW_INFO("%s: string=\"%s\"\n", __func__, pbuf);

	ptmp = (char *)pbuf;
	pch = strsep(&ptmp, ",");
	if ((pch == NULL) || (strlen(pch) == 0)) {
		rtw_mfree(pbuf, len);
		RTW_INFO("%s: parameter error(no cmd)!\n", __func__);
		return -EFAULT;
	}

	_rtw_memset(param, 0xFF, RTW_CUSTOMER_STR_LEN);

	if (strcmp(pch, "read") == 0) {
		read = 1;
		ret = rtw_hal_customer_str_read(adapter, param);

	} else if (strcmp(pch, "write") == 0) {
		do {
			pch = strsep(&ptmp, ":");
			if ((pch == NULL) || (strlen(pch) == 0))
				break;
			if (strlen(pch) != 2
				|| IsHexDigit(*pch) == _FALSE
				|| IsHexDigit(*(pch + 1)) == _FALSE
				|| sscanf(pch, "%hhx", &tmp) != 1
			) {
				RTW_WARN("%s: invalid 8-bit hex!\n", __func__);
				rtw_mfree(pbuf, len);
				return -EFAULT;
			}

			param[count++] = tmp;

		} while (count < RTW_CUSTOMER_STR_LEN);

		if (count == 0) {
			rtw_mfree(pbuf, len);
			RTW_WARN("%s: no input!\n", __func__);
			return -EFAULT;
		}
		ret = rtw_hal_customer_str_write(adapter, param);
	} else {
		rtw_mfree(pbuf, len);
		RTW_INFO("%s: parameter error(unknown cmd)!\n", __func__);
		return -EFAULT;
	}

	pos = sprintf(extra, "%s: ", read ? "read" : "write");
	if (read == 0 || ret == _SUCCESS) {
		for (i = 0; i < RTW_CUSTOMER_STR_LEN; i++)
			pos += sprintf(extra + pos, "%02x:", param[i]);
		extra[pos] = 0;
		pos--;
	}
	pos += sprintf(extra + pos, " %s", ret == _SUCCESS ? "OK" : "FAIL");

	wrqu->data.length = strlen(extra) + 1;

	rtw_mfree(pbuf, len);
	return 0;
}
#endif /* CONFIG_RTW_CUSTOMER_STR */


#ifdef CONFIG_SDIO_INDIRECT_ACCESS
#define DBG_MP_SDIO_INDIRECT_ACCESS 1
static int rtw_mp_sd_iread(struct net_device *dev
			   , struct iw_request_info *info
			   , struct iw_point *wrqu
			   , char *extra)
{
	char input[16];
	u8 width;
	unsigned long addr;
	u32 ret = 0;
	_adapter *padapter = rtw_netdev_priv(dev);

	if (wrqu->length > 16) {
		RTW_INFO(FUNC_ADPT_FMT" wrqu->length:%d\n", FUNC_ADPT_ARG(padapter), wrqu->length);
		ret = -EINVAL;
		goto exit;
	}

	if (copy_from_user(input, wrqu->pointer, wrqu->length)) {
		RTW_INFO(FUNC_ADPT_FMT" copy_from_user fail\n", FUNC_ADPT_ARG(padapter));
		ret = -EFAULT;
		goto exit;
	}

	_rtw_memset(extra, 0, wrqu->length);

	if (sscanf(input, "%hhu,%lx", &width, &addr) != 2) {
		RTW_INFO(FUNC_ADPT_FMT" sscanf fail\n", FUNC_ADPT_ARG(padapter));
		ret = -EINVAL;
		goto exit;
	}

	if (addr > 0x3FFF) {
		RTW_INFO(FUNC_ADPT_FMT" addr:0x%lx\n", FUNC_ADPT_ARG(padapter), addr);
		ret = -EINVAL;
		goto exit;
	}

	if (DBG_MP_SDIO_INDIRECT_ACCESS)
		RTW_INFO(FUNC_ADPT_FMT" width:%u, addr:0x%lx\n", FUNC_ADPT_ARG(padapter), width, addr);

	switch (width) {
	case 1:
		sprintf(extra, "0x%02x", rtw_sd_iread8(padapter, addr));
		wrqu->length = strlen(extra);
		break;
	case 2:
		sprintf(extra, "0x%04x", rtw_sd_iread16(padapter, addr));
		wrqu->length = strlen(extra);
		break;
	case 4:
		sprintf(extra, "0x%08x", rtw_sd_iread32(padapter, addr));
		wrqu->length = strlen(extra);
		break;
	default:
		wrqu->length = 0;
		ret = -EINVAL;
		break;
	}

exit:
	return ret;
}

static int rtw_mp_sd_iwrite(struct net_device *dev
			    , struct iw_request_info *info
			    , struct iw_point *wrqu
			    , char *extra)
{
	char width;
	unsigned long addr, data;
	int ret = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	char input[32];

	if (wrqu->length > 32) {
		RTW_INFO(FUNC_ADPT_FMT" wrqu->length:%d\n", FUNC_ADPT_ARG(padapter), wrqu->length);
		ret = -EINVAL;
		goto exit;
	}

	if (copy_from_user(input, wrqu->pointer, wrqu->length)) {
		RTW_INFO(FUNC_ADPT_FMT" copy_from_user fail\n", FUNC_ADPT_ARG(padapter));
		ret = -EFAULT;
		goto exit;
	}

	_rtw_memset(extra, 0, wrqu->length);

	if (sscanf(input, "%hhu,%lx,%lx", &width, &addr, &data) != 3) {
		RTW_INFO(FUNC_ADPT_FMT" sscanf fail\n", FUNC_ADPT_ARG(padapter));
		ret = -EINVAL;
		goto exit;
	}

	if (addr > 0x3FFF) {
		RTW_INFO(FUNC_ADPT_FMT" addr:0x%lx\n", FUNC_ADPT_ARG(padapter), addr);
		ret = -EINVAL;
		goto exit;
	}

	if (DBG_MP_SDIO_INDIRECT_ACCESS)
		RTW_INFO(FUNC_ADPT_FMT" width:%u, addr:0x%lx, data:0x%lx\n", FUNC_ADPT_ARG(padapter), width, addr, data);

	switch (width) {
	case 1:
		if (data > 0xFF) {
			ret = -EINVAL;
			break;
		}
		rtw_sd_iwrite8(padapter, addr, data);
		break;
	case 2:
		if (data > 0xFFFF) {
			ret = -EINVAL;
			break;
		}
		rtw_sd_iwrite16(padapter, addr, data);
		break;
	case 4:
		rtw_sd_iwrite32(padapter, addr, data);
		break;
	default:
		wrqu->length = 0;
		ret = -EINVAL;
		break;
	}

exit:
	return ret;
}
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */

#if defined(CONFIG_FPGA_INCLUDED)
static int rtw_fpga_set(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
	_adapter *padapter = rtw_netdev_priv(dev);

	if (_rtw_memcmp(extra, "start", 5))
		rtw_fpga_start(padapter);

	if (_rtw_memcmp(extra, "stop", 4))
		rtw_fpga_stop(padapter);

	return ret;
}
#endif
static int rtw_priv_set(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wdata, char *extra)
{
	struct iw_point *wrqu = (struct iw_point *)wdata;
	u32 subcmd = wrqu->flags;
	_adapter *padapter = rtw_netdev_priv(dev);

	if (padapter == NULL)
		return -ENETDOWN;

	if (padapter->netif_up == _FALSE) {
		RTW_INFO(" %s fail =>(padapter->netif_up == _FALSE )\n", __FUNCTION__);
		return -ENETDOWN;
	}

	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		RTW_INFO("%s fail =>(bSurpriseRemoved == _TRUE) || ( bDriverStopped == _TRUE)\n", __func__);
		return -ENETDOWN;
	}

	if (extra == NULL) {
		wrqu->length = 0;
		return -EIO;
	}

	if (subcmd < MP_NULL) {
#ifdef CONFIG_MP_INCLUDED
		rtw_priv_mp_set(dev, info, wdata, extra);
#endif
		return 0;
	}

	switch (subcmd) {
#ifdef CONFIG_WOWLAN
	case MP_WOW_ENABLE:
		RTW_INFO("set case MP_WOW_ENABLE: %s\n", extra);

		rtw_wowlan_ctrl(dev, info, wdata, extra);
		break;
	case MP_WOW_SET_PATTERN:
		RTW_INFO("set case MP_WOW_SET_PATTERN: %s\n", extra);
		rtw_wowlan_set_pattern(dev, info, wdata, extra);
		break;
#endif
#ifdef CONFIG_AP_WOWLAN
	case MP_AP_WOW_ENABLE:
		RTW_INFO("set case MP_AP_WOW_ENABLE: %s\n", extra);
		rtw_ap_wowlan_ctrl(dev, info, wdata, extra);
		break;
#endif
#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	case VENDOR_IE_SET:
		RTW_INFO("set case VENDOR_IE_SET\n");
		rtw_vendor_ie_set(dev , info , wdata , extra);
		break;
#endif
#if defined(RTW_PHL_TX) || defined(RTW_PHL_RX) || defined(CONFIG_PHL_TEST_SUITE)
	case PHL_TEST_SET:
		RTW_INFO("set case PHL_TEST_SET\n");
		rtw_phl_test_set(dev , info , wdata , extra);
		break;
#endif
#ifdef CONFIG_FPGA_INCLUDED
	case FPGA_SET:
		RTW_INFO("set case FPGA_SET\n");
		rtw_fpga_set(dev , info , wdata , extra);
		break;
#endif
	default:
		return -EIO;
	}

	return 0;
}

static int rtw_priv_get(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wdata, char *extra)
{
	struct iw_point *wrqu = (struct iw_point *)wdata;
	u32 subcmd = wrqu->flags;
	_adapter *padapter = rtw_netdev_priv(dev);
	int status = 0 ;
#ifndef CONFIG_MP_INCLUDED

	if (padapter == NULL)
		return -ENETDOWN;

	if (padapter->netif_up == _FALSE) {
		RTW_INFO(" %s fail =>(padapter->netif_up == _FALSE )\n", __FUNCTION__);
		return -ENETDOWN;
	}

	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		RTW_INFO("%s fail =>(padapter->bSurpriseRemoved == _TRUE) || ( padapter->bDriverStopped == _TRUE)\n", __func__);
		return -ENETDOWN;
	}

	if (extra == NULL) {
		wrqu->length = 0;
		return -EIO;
	}
#endif
	if (subcmd < MP_NULL) {
#ifdef CONFIG_MP_INCLUDED
		status = rtw_priv_mp_get(dev, info, wdata, extra);
		rtw_msleep_os(10); /* delay 5ms for sending pkt before exit adb shell operation */
#endif

	} else {
			switch (subcmd) {
#ifdef CONFIG_SDIO_INDIRECT_ACCESS
			case MP_SD_IREAD:
				status = rtw_mp_sd_iread(dev, info, wrqu, extra);
				break;
			case MP_SD_IWRITE:
				status = rtw_mp_sd_iwrite(dev, info, wrqu, extra);
				break;
#endif
#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
			case VENDOR_IE_GET:
				RTW_INFO("get case VENDOR_IE_GET\n");
				status = rtw_vendor_ie_get(dev , info , wdata , extra);
				break;
#endif
			default:
				return -EIO;
			}
		}

	return status;
}


#ifdef CONFIG_TDLS
static int rtw_wx_tdls_wfd_enable(struct net_device *dev,
				  struct iw_request_info *info,
				  union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_WFD

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	if (extra[0] == '0')
		rtw_tdls_wfd_enable(padapter, 0);
	else
		rtw_tdls_wfd_enable(padapter, 1);

#endif /* CONFIG_WFD */

	return ret;
}

static int rtw_tdls_weaksec(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS

	u8 i, j;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	if (extra[0] == '0')
		padapter->wdinfo.wfd_tdls_weaksec = 0;
	else
		padapter->wdinfo.wfd_tdls_weaksec = 1;

#endif /* CONFIG_TDLS */

	return ret;
}


static int rtw_tdls_enable(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	if (extra[0] == '0')
		rtw_disable_tdls_func(padapter, _TRUE);
	else if (extra[0] == '1')
		rtw_enable_tdls_func(padapter);
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_setup(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
#ifdef CONFIG_TDLS
	u8 i, j;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_txmgmt txmgmt;
#ifdef CONFIG_WFD
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_WFD */

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	if (wrqu->data.length - 1 != 17) {
		RTW_INFO("[%s] length:%d != 17\n", __FUNCTION__, (wrqu->data.length - 1));
		return ret;
	}

	_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));
	for (i = 0, j = 0 ; i < ETH_ALEN; i++, j += 3)
		txmgmt.peer[i] = key_2char2num(*(extra + j), *(extra + j + 1));

#ifdef CONFIG_WFD
	if (_AES_ != padapter->securitypriv.dot11PrivacyAlgrthm) {
		/* Weak Security situation with AP. */
		if (0 == pwdinfo->wfd_tdls_weaksec)	{
			/* Can't send the tdls setup request out!! */
			RTW_INFO("[%s] Current link is not AES, "
				"SKIP sending the tdls setup request!!\n", __FUNCTION__);
		} else
			issue_tdls_setup_req(padapter, &txmgmt, _TRUE);
	} else
#endif /* CONFIG_WFD */
	{
		issue_tdls_setup_req(padapter, &txmgmt, _TRUE);
	}
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_teardown(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS

	u8 i, j;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_info *ptdls_sta = NULL;
	struct tdls_txmgmt txmgmt;

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	if (wrqu->data.length - 1 != 17 && wrqu->data.length - 1 != 19) {
		RTW_INFO("[%s] length:%d != 17 or 19\n",
			 __FUNCTION__, (wrqu->data.length - 1));
		return ret;
	}

	_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));
	for (i = 0, j = 0; i < ETH_ALEN; i++, j += 3)
		txmgmt.peer[i] = key_2char2num(*(extra + j), *(extra + j + 1));

	ptdls_sta = rtw_get_stainfo(&(padapter->stapriv), txmgmt.peer);

	if (ptdls_sta != NULL) {
		txmgmt.status_code = _RSON_TDLS_TEAR_UN_RSN_;
		if (wrqu->data.length - 1 == 19)
			issue_tdls_teardown(padapter, &txmgmt, _FALSE);
		else
			issue_tdls_teardown(padapter, &txmgmt, _TRUE);
	} else
		RTW_INFO("TDLS peer not found\n");
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_discovery(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_txmgmt	txmgmt;
	int i = 0, j = 0;

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));
	for (i = 0, j = 0 ; i < ETH_ALEN; i++, j += 3)
		txmgmt.peer[i] = key_2char2num(*(extra + j), *(extra + j + 1));

	issue_tdls_dis_req(padapter, &txmgmt);

#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_ch_switch(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_ch_switch *pchsw_info = &padapter->tdlsinfo.chsw_info;
	u8 i, j;
	struct sta_info *ptdls_sta = NULL;
	u8 take_care_iqk;

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	if (rtw_tdls_is_chsw_allowed(padapter) == _FALSE) {
		RTW_INFO("TDLS channel switch is not allowed\n");
		return ret;
	}

	for (i = 0, j = 0 ; i < ETH_ALEN; i++, j += 3)
		pchsw_info->addr[i] = key_2char2num(*(extra + j), *(extra + j + 1));

	ptdls_sta = rtw_get_stainfo(&padapter->stapriv, pchsw_info->addr);
	if (ptdls_sta == NULL)
		return ret;

	pchsw_info->ch_sw_state |= TDLS_CH_SW_INITIATOR_STATE;

	if (ptdls_sta != NULL) {
		if (pchsw_info->off_ch_num == 0)
			pchsw_info->off_ch_num = 11;
	} else
		RTW_INFO("TDLS peer not found\n");

	rtw_pm_set_lps(padapter, PM_PS_MODE_ACTIVE);

	rtw_hal_get_hwreg(padapter, HW_VAR_CH_SW_NEED_TO_TAKE_CARE_IQK_INFO, &take_care_iqk);
	if (take_care_iqk == _TRUE) {
		u8 central_chnl;
		struct rtw_chan_def chdef = {0};

		chdef.band = rtw_get_band_type(pchsw_info->off_ch_num);
		chdef.chan = pchsw_info->off_ch_num;
		chdef.bw = (pchsw_info->ch_offset) ? CHANNEL_WIDTH_40 : CHANNEL_WIDTH_20;
		chdef.offset = pchsw_info->ch_offset;

		central_chnl = rtw_phl_get_center_ch(&chdef);
		if (rtw_hal_ch_sw_iqk_info_search(padapter, central_chnl, chdef.bw) >= 0)
			rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_START);
		else
			rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_PREPARE);
	} else
		rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_START);

	/* issue_tdls_ch_switch_req(padapter, ptdls_sta); */
	/* RTW_INFO("issue tdls ch switch req\n"); */

#endif /* CONFIG_TDLS_CH_SW */
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_ch_switch_off(struct net_device *dev,
				  struct iw_request_info *info,
				  union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_ch_switch *pchsw_info = &padapter->tdlsinfo.chsw_info;
	u8 i, j, mac_addr[ETH_ALEN];
	struct sta_info *ptdls_sta = NULL;
	struct tdls_txmgmt txmgmt;

	_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	if (rtw_tdls_is_chsw_allowed(padapter) == _FALSE) {
		RTW_INFO("TDLS channel switch is not allowed\n");
		return ret;
	}

	if (wrqu->data.length >= 17) {
		for (i = 0, j = 0 ; i < ETH_ALEN; i++, j += 3)
			mac_addr[i] = key_2char2num(*(extra + j), *(extra + j + 1));
		ptdls_sta = rtw_get_stainfo(&padapter->stapriv, mac_addr);
	}

	if (ptdls_sta == NULL)
		return ret;

	rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_END_TO_BASE_CHNL);

	pchsw_info->ch_sw_state &= ~(TDLS_CH_SW_INITIATOR_STATE |
				     TDLS_CH_SWITCH_ON_STATE |
				     TDLS_PEER_AT_OFF_STATE);
	_rtw_memset(pchsw_info->addr, 0x00, ETH_ALEN);

	ptdls_sta->ch_switch_time = 0;
	ptdls_sta->ch_switch_timeout = 0;
	_cancel_timer_ex(&ptdls_sta->ch_sw_timer);
	_cancel_timer_ex(&ptdls_sta->delay_timer);
	_cancel_timer_ex(&ptdls_sta->stay_on_base_chnl_timer);
	_cancel_timer_ex(&ptdls_sta->ch_sw_monitor_timer);

	rtw_pm_set_lps(padapter, PM_PS_MODE_MAX);
#endif /* CONFIG_TDLS_CH_SW */
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_dump_ch(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	RTW_INFO("[%s] dump_stack:%s\n", __FUNCTION__, extra);

	extra[wrqu->data.length] = 0x00;
	ptdlsinfo->chsw_info.dump_stack = rtw_atoi(extra);

	return ret;

#endif
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_off_ch_num(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	RTW_INFO("[%s] off_ch_num:%s\n", __FUNCTION__, extra);

	extra[wrqu->data.length] = 0x00;
	ptdlsinfo->chsw_info.off_ch_num = rtw_atoi(extra);

	return ret;

#endif
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_ch_offset(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	RTW_INFO("[%s] ch_offset:%s\n", __FUNCTION__, extra);

	extra[wrqu->data.length] = 0x00;
	switch (rtw_atoi(extra)) {
	case IEEE80211_SCA:
		ptdlsinfo->chsw_info.ch_offset = CHAN_OFFSET_UPPER;
		break;

	case IEEE80211_SCB:
		ptdlsinfo->chsw_info.ch_offset = CHAN_OFFSET_LOWER;
		break;

	default:
		ptdlsinfo->chsw_info.ch_offset = CHAN_OFFSET_NO_EXT;
		break;
	}

	return ret;

#endif
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_pson(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8 i, j, mac_addr[ETH_ALEN];
	struct sta_info *ptdls_sta = NULL;

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	for (i = 0, j = 0; i < ETH_ALEN; i++, j += 3)
		mac_addr[i] = key_2char2num(*(extra + j), *(extra + j + 1));

	ptdls_sta = rtw_get_stainfo(&padapter->stapriv, mac_addr);

	issue_nulldata_to_TDLS_peer_STA(padapter, ptdls_sta->padapter_link, ptdls_sta->phl_sta->mac_addr, 1, 3, 500);

#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_psoff(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8 i, j, mac_addr[ETH_ALEN];
	struct sta_info *ptdls_sta = NULL;

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	for (i = 0, j = 0; i < ETH_ALEN; i++, j += 3)
		mac_addr[i] = key_2char2num(*(extra + j), *(extra + j + 1));

	ptdls_sta = rtw_get_stainfo(&padapter->stapriv, mac_addr);

	if (ptdls_sta)
		issue_nulldata_to_TDLS_peer_STA(padapter, ptdls_sta->padapter_link, ptdls_sta->phl_sta->mac_addr, 0, 3, 500);

#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_setip(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_WFD

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	struct wifi_display_info *pwfd_info = ptdlsinfo->wfd_info;
	u8 i = 0, j = 0, k = 0, tag = 0;

	RTW_INFO("[%s] %s %d\n", __FUNCTION__, extra, wrqu->data.length - 1);

	while (i < 4) {
		for (j = 0; j < 4; j++) {
			if (*(extra + j + tag) == '.' || *(extra + j + tag) == '\0') {
				if (j == 1)
					pwfd_info->ip_address[i] = convert_ip_addr('0', '0', *(extra + (j - 1) + tag));
				if (j == 2)
					pwfd_info->ip_address[i] = convert_ip_addr('0', *(extra + (j - 2) + tag), *(extra + (j - 1) + tag));
				if (j == 3)
					pwfd_info->ip_address[i] = convert_ip_addr(*(extra + (j - 3) + tag), *(extra + (j - 2) + tag), *(extra + (j - 1) + tag));

				tag += j + 1;
				break;
			}
		}
		i++;
	}

	RTW_INFO("[%s] Set IP = %u.%u.%u.%u\n", __FUNCTION__,
		 ptdlsinfo->wfd_info->ip_address[0],
		 ptdlsinfo->wfd_info->ip_address[1],
		 ptdlsinfo->wfd_info->ip_address[2],
		 ptdlsinfo->wfd_info->ip_address[3]);

#endif /* CONFIG_WFD */
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_getip(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_WFD

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	struct wifi_display_info *pwfd_info = ptdlsinfo->wfd_info;

	RTW_INFO("[%s]\n", __FUNCTION__);

	sprintf(extra, "\n\n%u.%u.%u.%u\n",
		pwfd_info->peer_ip_address[0], pwfd_info->peer_ip_address[1],
		pwfd_info->peer_ip_address[2], pwfd_info->peer_ip_address[3]);

	RTW_INFO("[%s] IP=%u.%u.%u.%u\n", __FUNCTION__,
		 pwfd_info->peer_ip_address[0], pwfd_info->peer_ip_address[1],
		 pwfd_info->peer_ip_address[2], pwfd_info->peer_ip_address[3]);

	wrqu->data.length = strlen(extra);

#endif /* CONFIG_WFD */
#endif /* CONFIG_TDLS */

	return ret;
}

static int rtw_tdls_getport(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{

	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_WFD

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	struct wifi_display_info *pwfd_info = ptdlsinfo->wfd_info;

	RTW_INFO("[%s]\n", __FUNCTION__);

	sprintf(extra, "\n\n%d\n", pwfd_info->peer_rtsp_ctrlport);
	RTW_INFO("[%s] remote port = %d\n",
		 __FUNCTION__, pwfd_info->peer_rtsp_ctrlport);

	wrqu->data.length = strlen(extra);

#endif /* CONFIG_WFD */
#endif /* CONFIG_TDLS */

	return ret;

}

/* WFDTDLS, for sigma test */
static int rtw_tdls_dis_result(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{

	int ret = 0;

#ifdef CONFIG_TDLS
#ifdef CONFIG_WFD

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	RTW_INFO("[%s]\n", __FUNCTION__);

	if (ptdlsinfo->dev_discovered == _TRUE) {
		sprintf(extra, "\n\nDis=1\n");
		ptdlsinfo->dev_discovered = _FALSE;
	}

	wrqu->data.length = strlen(extra);

#endif /* CONFIG_WFD */
#endif /* CONFIG_TDLS */

	return ret;

}

/* WFDTDLS, for sigma test */
static int rtw_wfd_tdls_status(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{

	int ret = 0;

#ifdef CONFIG_TDLS

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	RTW_INFO("[%s]\n", __FUNCTION__);

	sprintf(extra, "\nlink_established:%d\n"
		"sta_cnt:%d\n"
		"sta_maximum:%d\n"
		"cur_channel:%d\n"
		"tdls_enable:%d"
#ifdef CONFIG_TDLS_CH_SW
		"ch_sw_state:%08x\n"
		"chsw_on:%d\n"
		"off_ch_num:%d\n"
		"cur_time:%d\n"
		"ch_offset:%d\n"
		"delay_swtich_back:%d"
#endif
		,
		ptdlsinfo->link_established, ptdlsinfo->sta_cnt,
		ptdlsinfo->sta_maximum, ptdlsinfo->cur_channel,
		rtw_is_tdls_enabled(padapter)
#ifdef CONFIG_TDLS_CH_SW
		,
		ptdlsinfo->chsw_info.ch_sw_state,
		ATOMIC_READ(&padapter->tdlsinfo.chsw_info.chsw_on),
		ptdlsinfo->chsw_info.off_ch_num,
		ptdlsinfo->chsw_info.cur_time,
		ptdlsinfo->chsw_info.ch_offset,
		ptdlsinfo->chsw_info.delay_switch_back
#endif
	       );

	wrqu->data.length = strlen(extra);

#endif /* CONFIG_TDLS */

	return ret;

}

static int rtw_tdls_getsta(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{

	int ret = 0;
#ifdef CONFIG_TDLS
	u8 i, j;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8 addr[ETH_ALEN] = {0};
	char charmac[17];
	struct sta_info *ptdls_sta = NULL;

	RTW_INFO("[%s] %s %d\n", __FUNCTION__,
		 (char *)wrqu->data.pointer, wrqu->data.length - 1);

	if (copy_from_user(charmac, wrqu->data.pointer + 9, 17)) {
		ret = -EFAULT;
		goto exit;
	}

	RTW_INFO("[%s] %d, charmac:%s\n", __FUNCTION__, __LINE__, charmac);
	for (i = 0, j = 0 ; i < ETH_ALEN; i++, j += 3)
		addr[i] = key_2char2num(*(charmac + j), *(charmac + j + 1));

	RTW_INFO("[%s] %d, charmac:%s, addr:"MAC_FMT"\n",
		 __FUNCTION__, __LINE__, charmac, MAC_ARG(addr));
	ptdls_sta = rtw_get_stainfo(&padapter->stapriv, addr);
	if (ptdls_sta) {
		sprintf(extra, "\n\ntdls_sta_state=0x%08x\n", ptdls_sta->tdls_sta_state);
		RTW_INFO("\n\ntdls_sta_state=%d\n", ptdls_sta->tdls_sta_state);
	} else {
		sprintf(extra, "\n\nNot found this sta\n");
		RTW_INFO("\n\nNot found this sta\n");
	}
	wrqu->data.length = strlen(extra);

exit:
#endif /* CONFIG_TDLS */
	return ret;

}

static int rtw_tdls_get_best_ch(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
#ifdef CONFIG_FIND_BEST_CHANNEL
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rtw_chset *chset = adapter_to_chset(padapter);
	u32 i, best_channel_24G = 1, best_channel_5G = 36, index_24G = 0, index_5G = 0;

	for (i = 0; i < chset->chs_len; i++) {
		if (chset->chs[i].flags & RTW_CHF_DIS)
			continue;
		if (chset->chs[i].ChannelNum == 1)
			index_24G = i;
		if (chset->chs[i].ChannelNum == 36)
			index_5G = i;
	}

	for (i = 0; i < chset->chs_len; i++) {
		if (chset->chs[i].flags & RTW_CHF_DIS)
			continue;

		/* 2.4G */
		if (chset->chs[i].ChannelNum == 6 || chset->chs[i].ChannelNum == 11) {
			if (chset->chs[i].rx_count < chset->chs[index_24G].rx_count) {
				index_24G = i;
				best_channel_24G = chset->chs[i].ChannelNum;
			}
		}

		/* 5G */
		if (chset->chs[i].ChannelNum >= 36
		    && chset->chs[i].ChannelNum < 140) {
			/* Find primary channel */
			if (((chset->chs[i].ChannelNum - 36) % 8 == 0)
			    && (chset->chs[i].rx_count < chset->chs[index_5G].rx_count)) {
				index_5G = i;
				best_channel_5G = chset->chs[i].ChannelNum;
			}
		}

		if (chset->chs[i].ChannelNum >= 149
		    && chset->chs[i].ChannelNum < 165) {
			/* Find primary channel */
			if (((chset->chs[i].ChannelNum - 149) % 8 == 0)
			    && (chset->chs[i].rx_count < chset->chs[index_5G].rx_count)) {
				index_5G = i;
				best_channel_5G = chset->chs[i].ChannelNum;
			}
		}
#if 1 /* debug */
		RTW_INFO("The rx cnt of channel %3d = %d\n",
			 chset->chs[i].ChannelNum,
			 chset->chs[i].rx_count);
#endif
	}

	sprintf(extra, "\nbest_channel_24G = %d\n", best_channel_24G);
	RTW_INFO("best_channel_24G = %d\n", best_channel_24G);

	if (index_5G != 0) {
		sprintf(extra, "best_channel_5G = %d\n", best_channel_5G);
		RTW_INFO("best_channel_5G = %d\n", best_channel_5G);
	}

	wrqu->data.length = strlen(extra);

#endif

	return 0;

}
#endif /*#ifdef CONFIG_TDLS*/
static int rtw_tdls(struct net_device *dev,
		    struct iw_request_info *info,
		    union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_INFO("[%s] extra = %s\n", __FUNCTION__, extra);

	if (rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_TDLS) == _FALSE) {
		RTW_INFO("Discard tdls oper since hal doesn't support tdls\n");
		return 0;
	}

	if (rtw_is_tdls_enabled(padapter) == _FALSE) {
		RTW_INFO("TDLS is not enabled\n");
		return 0;
	}

	/* WFD Sigma will use the tdls enable command to let the driver know we want to test the tdls now! */

	if (rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST)) {
		if (_rtw_memcmp(extra, "wfdenable=", 10)) {
			wrqu->data.length -= 10;
			rtw_wx_tdls_wfd_enable(dev, info, wrqu, &extra[10]);
			return ret;
		}
	}

	if (_rtw_memcmp(extra, "weaksec=", 8)) {
		wrqu->data.length -= 8;
		rtw_tdls_weaksec(dev, info, wrqu, &extra[8]);
		return ret;
	} else if (_rtw_memcmp(extra, "tdlsenable=", 11)) {
		wrqu->data.length -= 11;
		rtw_tdls_enable(dev, info, wrqu, &extra[11]);
		return ret;
	}

	if (_rtw_memcmp(extra, "setup=", 6)) {
		wrqu->data.length -= 6;
		rtw_tdls_setup(dev, info, wrqu, &extra[6]);
	} else if (_rtw_memcmp(extra, "tear=", 5)) {
		wrqu->data.length -= 5;
		rtw_tdls_teardown(dev, info, wrqu, &extra[5]);
	} else if (_rtw_memcmp(extra, "dis=", 4)) {
		wrqu->data.length -= 4;
		rtw_tdls_discovery(dev, info, wrqu, &extra[4]);
	} else if (_rtw_memcmp(extra, "swoff=", 6)) {
		wrqu->data.length -= 6;
		rtw_tdls_ch_switch_off(dev, info, wrqu, &extra[6]);
	} else if (_rtw_memcmp(extra, "sw=", 3)) {
		wrqu->data.length -= 3;
		rtw_tdls_ch_switch(dev, info, wrqu, &extra[3]);
	} else if (_rtw_memcmp(extra, "dumpstack=", 10)) {
		wrqu->data.length -= 10;
		rtw_tdls_dump_ch(dev, info, wrqu, &extra[10]);
	} else if (_rtw_memcmp(extra, "offchnum=", 9)) {
		wrqu->data.length -= 9;
		rtw_tdls_off_ch_num(dev, info, wrqu, &extra[9]);
	} else if (_rtw_memcmp(extra, "choffset=", 9)) {
		wrqu->data.length -= 9;
		rtw_tdls_ch_offset(dev, info, wrqu, &extra[9]);
	} else if (_rtw_memcmp(extra, "pson=", 5)) {
		wrqu->data.length -= 5;
		rtw_tdls_pson(dev, info, wrqu, &extra[5]);
	} else if (_rtw_memcmp(extra, "psoff=", 6)) {
		wrqu->data.length -= 6;
		rtw_tdls_psoff(dev, info, wrqu, &extra[6]);
	}

#ifdef CONFIG_WFD
	if (rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST)) {
		if (_rtw_memcmp(extra, "setip=", 6)) {
			wrqu->data.length -= 6;
			rtw_tdls_setip(dev, info, wrqu, &extra[6]);
		} else if (_rtw_memcmp(extra, "tprobe=", 6))
			issue_tunneled_probe_req((_adapter *)rtw_netdev_priv(dev));
	}
#endif /* CONFIG_WFD */

#endif /* CONFIG_TDLS */

	return ret;
}


static int rtw_tdls_get(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	int ret = 0;

#ifdef CONFIG_TDLS

	RTW_INFO("[%s] extra = %s\n", __FUNCTION__, (char *) wrqu->data.pointer);

	if (_rtw_memcmp(wrqu->data.pointer, "ip", 2))
		rtw_tdls_getip(dev, info, wrqu, extra);
	else if (_rtw_memcmp(wrqu->data.pointer, "port", 4))
		rtw_tdls_getport(dev, info, wrqu, extra);
	/* WFDTDLS, for sigma test */
	else if (_rtw_memcmp(wrqu->data.pointer, "dis", 3))
		rtw_tdls_dis_result(dev, info, wrqu, extra);
	else if (_rtw_memcmp(wrqu->data.pointer, "status", 6))
		rtw_wfd_tdls_status(dev, info, wrqu, extra);
	else if (_rtw_memcmp(wrqu->data.pointer, "tdls_sta=", 9))
		rtw_tdls_getsta(dev, info, wrqu, extra);
	else if (_rtw_memcmp(wrqu->data.pointer, "best_ch", 7))
		rtw_tdls_get_best_ch(dev, info, wrqu, extra);
#endif /* CONFIG_TDLS */

	return ret;
}

#if 0 /*#ifdef CONFIG_MAC_LOOPBACK_DRIVER*/
static s32 initLoopback(_adapter *padapter)
{
	PLOOPBACKDATA ploopback;


	if (padapter->ploopback == NULL) {
		ploopback = (PLOOPBACKDATA)rtw_zmalloc(sizeof(LOOPBACKDATA));
		if (ploopback == NULL)
			return -ENOMEM;

		_rtw_init_sema(&ploopback->sema, 0);
		ploopback->bstop = _TRUE;
		ploopback->cnt = 0;
		ploopback->size = 300;
		_rtw_memset(ploopback->msg, 0, sizeof(ploopback->msg));

		padapter->ploopback = ploopback;
	}

	return 0;
}

static void freeLoopback(_adapter *padapter)
{
	PLOOPBACKDATA ploopback;


	ploopback = padapter->ploopback;
	if (ploopback) {
		rtw_mfree((u8 *)ploopback, sizeof(LOOPBACKDATA));
		padapter->ploopback = NULL;
	}
}

static s32 initpseudoadhoc(_adapter *padapter)
{
	NDIS_802_11_NETWORK_INFRASTRUCTURE networkType;
	s32 err;

	networkType = Ndis802_11IBSS;
	err = rtw_set_802_11_infrastructure_mode(padapter, networkType, 0);
	if (err == _FALSE)
		return _FAIL;

	err = rtw_setopmode_cmd(padapter, networkType, RTW_CMDF_WAIT_ACK);
	if (err == _FAIL)
		return _FAIL;

	return _SUCCESS;
}

static s32 createpseudoadhoc(_adapter *padapter)
{
	NDIS_802_11_AUTHENTICATION_MODE authmode;
	struct mlme_priv *pmlmepriv;
	NDIS_802_11_SSID *passoc_ssid;
	WLAN_BSSID_EX *pdev_network;
	u8 *pibss;
	u8 ssid[] = "pseduo_ad-hoc";
	s32 err;


	pmlmepriv = &padapter->mlmepriv;

	authmode = Ndis802_11AuthModeOpen;
	err = rtw_set_802_11_authentication_mode(padapter, authmode);
	if (err == _FALSE)
		return _FAIL;

	passoc_ssid = &pmlmepriv->assoc_ssid;
	_rtw_memset(passoc_ssid, 0, sizeof(NDIS_802_11_SSID));
	passoc_ssid->SsidLength = sizeof(ssid) - 1;
	_rtw_memcpy(passoc_ssid->Ssid, ssid, passoc_ssid->SsidLength);

	pdev_network = &padapter->registrypriv.dev_network;
	pibss = padapter->registrypriv.dev_network.MacAddress;
	_rtw_memcpy(&pdev_network->Ssid, passoc_ssid, sizeof(NDIS_802_11_SSID));

	rtw_update_registrypriv_dev_network(padapter);
	rtw_generate_random_ibss(pibss);

	_rtw_spinlock_bh(&pmlmepriv->lock);
	/*pmlmepriv->fw_state = WIFI_ADHOC_MASTER_STATE;*/
	init_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE);

	_rtw_spinunlock_bh(&pmlmepriv->lock);

#if 0
	err = rtw_create_ibss_cmd(padapter, 0);
	if (err == _FAIL)
		return _FAIL;
#else
	{
		struct wlan_network *pcur_network;
		struct sta_info *psta;

		/* 3  create a new psta */
		pcur_network = &pmlmepriv->cur_network;

		/* clear psta in the cur_network, if any */
		psta = rtw_get_stainfo(&padapter->stapriv, pcur_network->network.MacAddress);
		if (psta)
			rtw_free_stainfo(padapter, psta);

		psta = rtw_alloc_stainfo(&padapter->stapriv, pibss);
		if (psta == NULL)
			return _FAIL;

		/* 3  join psudo AdHoc */
		pcur_network->join_res = 1;
		pcur_network->aid = psta->phl_sta->aid = 1;
		_rtw_memcpy(&pcur_network->network, pdev_network, get_WLAN_BSSID_EX_sz(pdev_network));

	}
#endif

	return _SUCCESS;
}
/*GEORGIA_TODO_FIXIT_MOVE_TO_HAL*/
static struct xmit_frame *createloopbackpkt(_adapter *padapter, u32 size)
{
	struct xmit_priv *pxmitpriv;
	struct xmit_frame *pframe;
	struct xmit_buf *pxmitbuf;
	struct pkt_attrib *pattrib;
	struct tx_desc *desc;
	u8 *pkt_start, *pkt_end, *ptr;
	struct rtw_ieee80211_hdr *hdr;
	s32 bmcast;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u16 xmitbuf_sz = GET_HAL_XMITBUF_SZ(dvobj);

	if ((TXDESC_SIZE + WLANHDR_OFFSET + size) > xmitbuf_sz)
		return NULL;

	pxmitpriv = &padapter->xmitpriv;
	pframe = NULL;

	/* 2 1. allocate xmit frame */
	pframe = rtw_alloc_xmitframe(pxmitpriv);
	if (pframe == NULL)
		return NULL;
	pframe->padapter = padapter;

	/* 2 2. allocate xmit buffer */
	_rtw_spinlock_bh(&pxmitpriv->lock);
	pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
	_rtw_spinunlock_bh(&pxmitpriv->lock);
	if (pxmitbuf == NULL) {
		rtw_free_xmitframe(pxmitpriv, pframe);
		return NULL;
	}

	pframe->pxmitbuf = pxmitbuf;
	pframe->buf_addr = pxmitbuf->pbuf;
	pxmitbuf->priv_data = pframe;

	/* 2 3. update_attrib() */
	pattrib = &pframe->attrib;

	/* init xmitframe attribute */
	_rtw_memset(pattrib, 0, sizeof(struct pkt_attrib));

	pattrib->ether_type = 0x8723;
	_rtw_memcpy(pattrib->src, adapter_mac_addr(padapter), ETH_ALEN);
	_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
	_rtw_memset(pattrib->dst, 0xFF, ETH_ALEN);
	_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);

	/*	pattrib->dhcp_pkt = 0;
	 *	pattrib->pktlen = 0; */
	pattrib->ack_policy = 0;
	/*	pattrib->pkt_hdrlen = ETH_HLEN; */
	pattrib->hdrlen = WLAN_HDR_A3_LEN;
	pattrib->subtype = WIFI_DATA;
	pattrib->priority = 0;
	pattrib->qsel = pattrib->priority;
	/*	do_queue_select(padapter, pattrib); */
	pattrib->nr_frags = 1;
	pattrib->encrypt = 0;
	pattrib->bswenc = _FALSE;
	pattrib->qos_en = _FALSE;

	bmcast = IS_MCAST(pattrib->ra);
	if (bmcast)
		pattrib->psta = rtw_get_bcmc_stainfo(padapter);
	else
		pattrib->psta = rtw_get_stainfo(&padapter->stapriv, get_bssid(&padapter->mlmepriv));

	pattrib->mac_id = pattrib->psta->phl_sta->macid;
	pattrib->pktlen = size;
	pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->pktlen;

	/* 2 4. fill TX descriptor */
	desc = (struct tx_desc *)pframe->buf_addr;
	_rtw_memset(desc, 0, TXDESC_SIZE);

	fill_default_txdesc(pframe, (u8 *)desc);

	/* Hw set sequence number */
	((PTXDESC)desc)->hwseq_en = 0; /* HWSEQ_EN, 0:disable, 1:enable
 * ((PTXDESC)desc)->hwseq_sel = 0;  */ /* HWSEQ_SEL */

	((PTXDESC)desc)->disdatafb = 1;

	/* convert to little endian */
	desc->txdw0 = cpu_to_le32(desc->txdw0);
	desc->txdw1 = cpu_to_le32(desc->txdw1);
	desc->txdw2 = cpu_to_le32(desc->txdw2);
	desc->txdw3 = cpu_to_le32(desc->txdw3);
	desc->txdw4 = cpu_to_le32(desc->txdw4);
	desc->txdw5 = cpu_to_le32(desc->txdw5);
	desc->txdw6 = cpu_to_le32(desc->txdw6);
	desc->txdw7 = cpu_to_le32(desc->txdw7);
#ifdef CONFIG_PCI_HCI
	desc->txdw8 = cpu_to_le32(desc->txdw8);
	desc->txdw9 = cpu_to_le32(desc->txdw9);
	desc->txdw10 = cpu_to_le32(desc->txdw10);
	desc->txdw11 = cpu_to_le32(desc->txdw11);
	desc->txdw12 = cpu_to_le32(desc->txdw12);
	desc->txdw13 = cpu_to_le32(desc->txdw13);
	desc->txdw14 = cpu_to_le32(desc->txdw14);
	desc->txdw15 = cpu_to_le32(desc->txdw15);
#endif

	cal_txdesc_chksum(desc);

	/* 2 5. coalesce */
	pkt_start = pframe->buf_addr + TXDESC_SIZE;
	pkt_end = pkt_start + pattrib->last_txcmdsz;

	/* 3 5.1. make wlan header, make_wlanhdr() */
	hdr = (struct rtw_ieee80211_hdr *)pkt_start;
	set_frame_sub_type(&hdr->frame_ctl, pattrib->subtype);
	_rtw_memcpy(hdr->addr1, pattrib->dst, ETH_ALEN); /* DA */
	_rtw_memcpy(hdr->addr2, pattrib->src, ETH_ALEN); /* SA */
	_rtw_memcpy(hdr->addr3, get_bssid(&padapter->mlmepriv), ETH_ALEN); /* RA, BSSID */

	/* 3 5.2. make payload */
	ptr = pkt_start + pattrib->hdrlen;
	get_random_bytes(ptr, pkt_end - ptr);

	pxmitbuf->len = TXDESC_SIZE + pattrib->last_txcmdsz;
	pxmitbuf->ptail += pxmitbuf->len;

	return pframe;
}

static void freeloopbackpkt(_adapter *padapter, struct xmit_frame *pframe)
{
	struct xmit_priv *pxmitpriv;
	struct xmit_buf *pxmitbuf;


	pxmitpriv = &padapter->xmitpriv;
	pxmitbuf = pframe->pxmitbuf;

	rtw_free_xmitframe(pxmitpriv, pframe);
	rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
}

static void printdata(u8 *pbuf, u32 len)
{
	u32 i, val;


	for (i = 0; (i + 4) <= len; i += 4) {
		printk("%08X", *(u32 *)(pbuf + i));
		if ((i + 4) & 0x1F)
			printk(" ");
		else
			printk("\n");
	}

	if (i < len) {
#ifdef CONFIG_BIG_ENDIAN
		for (; i < len, i++)
			printk("%02X", pbuf + i);
#else /* CONFIG_LITTLE_ENDIAN */
#if 0
		val = 0;
		_rtw_memcpy(&val, pbuf + i, len - i);
		printk("%8X", val);
#else
		u8 str[9];
		u8 n;
		val = 0;
		n = len - i;
		_rtw_memcpy(&val, pbuf + i, n);
		sprintf(str, "%08X", val);
		n = (4 - n) * 2;
		printk("%8s", str + n);
#endif
#endif /* CONFIG_LITTLE_ENDIAN */
	}
	printk("\n");
}

static u8 pktcmp(_adapter *padapter, u8 *txbuf, u32 txsz, u8 *rxbuf, u32 rxsz)
{
	struct recv_stat *prxstat;
	struct recv_stat report;
	PRXREPORT prxreport;
	u32 drvinfosize;
	u32 rxpktsize;
	u8 fcssize;
	u8 ret = _FALSE;

	prxstat = (struct recv_stat *)rxbuf;
	report.rxdw0 = le32_to_cpu(prxstat->rxdw0);
	report.rxdw1 = le32_to_cpu(prxstat->rxdw1);
	report.rxdw2 = le32_to_cpu(prxstat->rxdw2);
	report.rxdw3 = le32_to_cpu(prxstat->rxdw3);
	report.rxdw4 = le32_to_cpu(prxstat->rxdw4);
	report.rxdw5 = le32_to_cpu(prxstat->rxdw5);

	prxreport = (PRXREPORT)&report;
	drvinfosize = prxreport->drvinfosize << 3;
	rxpktsize = prxreport->pktlen;

	if (rtw_hal_rcr_check(padapter, RCR_APPFCS))
		fcssize = IEEE80211_FCS_LEN;
	else
		fcssize = 0;

	if ((txsz - TXDESC_SIZE) != (rxpktsize - fcssize)) {
		RTW_INFO("%s: ERROR! size not match tx/rx=%d/%d !\n",
			 __func__, txsz - TXDESC_SIZE, rxpktsize - fcssize);
		ret = _FALSE;
	} else {
		ret = _rtw_memcmp(txbuf + TXDESC_SIZE, \
				  rxbuf + RXDESC_SIZE + drvinfosize, \
				  txsz - TXDESC_SIZE);
		if (ret == _FALSE)
			RTW_INFO("%s: ERROR! pkt content mismatch!\n", __func__);
	}

	if (ret == _FALSE) {
		RTW_INFO("\n%s: TX PKT total=%d, desc=%d, content=%d\n",
			 __func__, txsz, TXDESC_SIZE, txsz - TXDESC_SIZE);
		RTW_INFO("%s: TX DESC size=%d\n", __func__, TXDESC_SIZE);
		printdata(txbuf, TXDESC_SIZE);
		RTW_INFO("%s: TX content size=%d\n", __func__, txsz - TXDESC_SIZE);
		printdata(txbuf + TXDESC_SIZE, txsz - TXDESC_SIZE);

		RTW_INFO("\n%s: RX PKT read=%d offset=%d(%d,%d) content=%d\n",
			__func__, rxsz, RXDESC_SIZE + drvinfosize, RXDESC_SIZE, drvinfosize, rxpktsize);
		if (rxpktsize != 0) {
			RTW_INFO("%s: RX DESC size=%d\n", __func__, RXDESC_SIZE);
			printdata(rxbuf, RXDESC_SIZE);
			RTW_INFO("%s: RX drvinfo size=%d\n", __func__, drvinfosize);
			printdata(rxbuf + RXDESC_SIZE, drvinfosize);
			RTW_INFO("%s: RX content size=%d\n", __func__, rxpktsize);
			printdata(rxbuf + RXDESC_SIZE + drvinfosize, rxpktsize);
		} else {
			RTW_INFO("%s: RX data size=%d\n", __func__, rxsz);
			printdata(rxbuf, rxsz);
		}
	}

	return ret;
}

thread_return lbk_thread(thread_context context)
{
#if 0
	s32 err;
	_adapter *padapter;
	PLOOPBACKDATA ploopback;
	struct xmit_frame *pxmitframe;
	u32 cnt, ok, fail, headerlen;
	u32 pktsize;
	u32 ff_hwaddr;


	padapter = (_adapter *)context;
	ploopback = padapter->ploopback;
	if (ploopback == NULL)
		return -1;
	cnt = 0;
	ok = 0;
	fail = 0;

	daemonize("%s", "RTW_LBK_THREAD");
	allow_signal(SIGTERM);

	do {
		if (ploopback->size == 0) {
			get_random_bytes(&pktsize, 4);
			pktsize = (pktsize % 1535) + 1; /* 1~1535 */
		} else
			pktsize = ploopback->size;

		pxmitframe = createloopbackpkt(padapter, pktsize);
		if (pxmitframe == NULL) {
			sprintf(ploopback->msg, "loopback FAIL! 3. create Packet FAIL!");
			break;
		}

		ploopback->txsize = TXDESC_SIZE + pxmitframe->attrib.last_txcmdsz;
		_rtw_memcpy(ploopback->txbuf, pxmitframe->buf_addr, ploopback->txsize);
		ff_hwaddr = rtw_get_ff_hwaddr(pxmitframe);
		cnt++;
		RTW_INFO("%s: wirte port cnt=%d size=%d\n", __func__, cnt, ploopback->txsize);
		pxmitframe->pxmitbuf->pdata = ploopback->txbuf;
		rtw_write_port(padapter, ff_hwaddr, ploopback->txsize, (u8 *)pxmitframe->pxmitbuf);

		/* wait for rx pkt */
		_rtw_down_sema(&ploopback->sema);

		err = pktcmp(padapter, ploopback->txbuf, ploopback->txsize, ploopback->rxbuf, ploopback->rxsize);
		if (err == _TRUE)
			ok++;
		else
			fail++;

		ploopback->txsize = 0;
		_rtw_memset(ploopback->txbuf, 0, 0x8000);
		ploopback->rxsize = 0;
		_rtw_memset(ploopback->rxbuf, 0, 0x8000);

		freeloopbackpkt(padapter, pxmitframe);
		pxmitframe = NULL;

		flush_signals_thread();

		if ((ploopback->bstop == _TRUE) ||
		    ((ploopback->cnt != 0) && (ploopback->cnt == cnt))) {
			u32 ok_rate, fail_rate, all;
			all = cnt;
			ok_rate = (ok * 100) / all;
			fail_rate = (fail * 100) / all;
			sprintf(ploopback->msg, \
				"loopback result: ok=%d%%(%d/%d),error=%d%%(%d/%d)", \
				ok_rate, ok, all, fail_rate, fail, all);
			break;
		}
	} while (1);

	ploopback->bstop = _TRUE;

	thread_exit(NULL);
#endif
	return 0;
}

static void loopbackTest(_adapter *padapter, u32 cnt, u32 size, u8 *pmsg)
{
	PLOOPBACKDATA ploopback;
	u32 len;
	s32 err;


	ploopback = padapter->ploopback;

	if (ploopback) {
		if (ploopback->bstop == _FALSE) {
			ploopback->bstop = _TRUE;
			_rtw_up_sema(&ploopback->sema);
		}
		len = 0;
		do {
			len = strlen(ploopback->msg);
			if (len)
				break;
			rtw_msleep_os(1);
		} while (1);
		_rtw_memcpy(pmsg, ploopback->msg, len + 1);
		freeLoopback(padapter);

		return;
	}

	/* disable dynamic algorithm	 */
	rtw_phydm_ability_backup(padapter);
	rtw_phydm_func_disable_all(padapter);

	/* create pseudo ad-hoc connection */
	err = initpseudoadhoc(padapter);
	if (err == _FAIL) {
		sprintf(pmsg, "loopback FAIL! 1.1 init ad-hoc FAIL!");
		return;
	}

	err = createpseudoadhoc(padapter);
	if (err == _FAIL) {
		sprintf(pmsg, "loopback FAIL! 1.2 create ad-hoc master FAIL!");
		return;
	}

	err = initLoopback(padapter);
	if (err) {
		sprintf(pmsg, "loopback FAIL! 2. init FAIL! error code=%d", err);
		return;
	}

	ploopback = padapter->ploopback;

	ploopback->bstop = _FALSE;
	ploopback->cnt = cnt;
	ploopback->size = size;
	ploopback->lbkthread = rtw_thread_start(lbk_thread, padapter, "RTW_LBK_THREAD");
	if (ploopback->lbkthread == NULL)) {
		freeLoopback(padapter);
		sprintf(pmsg, "loopback start FAIL! cnt=%d", cnt);
		return;
	}

	sprintf(pmsg, "loopback start! cnt=%d", cnt);
}
#endif /* CONFIG_MAC_LOOPBACK_DRIVER */

static int rtw_test(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu, char *extra)
{
	u32 len;
	u8 *pbuf, *pch;
	char *ptmp;
	u8 *delim = ",";
	_adapter *padapter = rtw_netdev_priv(dev);


	RTW_INFO("+%s\n", __func__);
	len = wrqu->data.length;

	pbuf = (u8 *)rtw_zmalloc(len + 1);
	if (pbuf == NULL) {
		RTW_INFO("%s: no memory!\n", __func__);
		return -ENOMEM;
	}

	if (copy_from_user(pbuf, wrqu->data.pointer, len)) {
		rtw_mfree(pbuf, len + 1);
		RTW_INFO("%s: copy from user fail!\n", __func__);
		return -EFAULT;
	}

	pbuf[len] = '\0';

	RTW_INFO("%s: string=\"%s\"\n", __func__, pbuf);

	ptmp = (char *)pbuf;
	pch = strsep(&ptmp, delim);
	if ((pch == NULL) || (strlen(pch) == 0)) {
		rtw_mfree(pbuf, len);
		RTW_INFO("%s: parameter error(level 1)!\n", __func__);
		return -EFAULT;
	}

#if 0 /*#ifdef CONFIG_MAC_LOOPBACK_DRIVER*/
	if (strcmp(pch, "loopback") == 0) {
		s32 cnt = 0;
		u32 size = 64;

		pch = strsep(&ptmp, delim);
		if ((pch == NULL) || (strlen(pch) == 0)) {
			rtw_mfree(pbuf, len);
			RTW_INFO("%s: parameter error(level 2)!\n", __func__);
			return -EFAULT;
		}

		sscanf(pch, "%d", &cnt);
		RTW_INFO("%s: loopback cnt=%d\n", __func__, cnt);

		pch = strsep(&ptmp, delim);
		if ((pch == NULL) || (strlen(pch) == 0)) {
			rtw_mfree(pbuf, len);
			RTW_INFO("%s: parameter error(level 2)!\n", __func__);
			return -EFAULT;
		}

		sscanf(pch, "%d", &size);
		RTW_INFO("%s: loopback size=%d\n", __func__, size);

		loopbackTest(padapter, cnt, size, extra);
		wrqu->data.length = strlen(extra) + 1;

		goto free_buf;
	}
#endif

	if (strcmp(pch, "h2c") == 0) {
		u8 param[8];
		u8 count = 0;
		u32 tmp;
		u8 i;
		u32 pos;
		u8 ret;

		do {
			pch = strsep(&ptmp, delim);
			if ((pch == NULL) || (strlen(pch) == 0))
				break;

			sscanf(pch, "%x", &tmp);
			param[count++] = (u8)tmp;
		} while (count < 8);

		if (count == 0) {
			rtw_mfree(pbuf, len);
			RTW_INFO("%s: parameter error(level 2)!\n", __func__);
			return -EFAULT;
		}

		ret = rtw_test_h2c_cmd(padapter, param, count);

		pos = sprintf(extra, "H2C ID=0x%02x content=", param[0]);
		for (i = 1; i < count; i++)
			pos += sprintf(extra + pos, "%02x,", param[i]);
		extra[pos] = 0;
		pos--;
		pos += sprintf(extra + pos, " %s", ret == _FAIL ? "FAIL" : "OK");

		wrqu->data.length = strlen(extra) + 1;

		goto free_buf;
	}

free_buf:
	rtw_mfree(pbuf, len);
	return 0;
}

static iw_handler rtw_handlers[] = {
	NULL,					/* SIOCSIWCOMMIT */
	rtw_wx_get_name,		/* SIOCGIWNAME */
	dummy,					/* SIOCSIWNWID */
	dummy,					/* SIOCGIWNWID */
	rtw_wx_set_freq,		/* SIOCSIWFREQ */
	rtw_wx_get_freq,		/* SIOCGIWFREQ */
	rtw_wx_set_mode,		/* SIOCSIWMODE */
	rtw_wx_get_mode,		/* SIOCGIWMODE */
	dummy,					/* SIOCSIWSENS */
	rtw_wx_get_sens,		/* SIOCGIWSENS */
	NULL,					/* SIOCSIWRANGE */
	rtw_wx_get_range,		/* SIOCGIWRANGE */
	rtw_wx_set_priv,		/* SIOCSIWPRIV */
	NULL,					/* SIOCGIWPRIV */
	NULL,					/* SIOCSIWSTATS */
	NULL,					/* SIOCGIWSTATS */
	dummy,					/* SIOCSIWSPY */
	dummy,					/* SIOCGIWSPY */
	NULL,					/* SIOCGIWTHRSPY */
	NULL,					/* SIOCWIWTHRSPY */
	rtw_wx_set_wap,		/* SIOCSIWAP */
	rtw_wx_get_wap,		/* SIOCGIWAP */
	rtw_wx_set_mlme,		/* request MLME operation; uses struct iw_mlme */
	dummy,					/* SIOCGIWAPLIST -- depricated */
	rtw_wx_set_scan,		/* SIOCSIWSCAN */
	rtw_wx_get_scan,		/* SIOCGIWSCAN */
	rtw_wx_set_essid,		/* SIOCSIWESSID */
	rtw_wx_get_essid,		/* SIOCGIWESSID */
	dummy,					/* SIOCSIWNICKN */
	rtw_wx_get_nick,		/* SIOCGIWNICKN */
	NULL,					/* -- hole -- */
	NULL,					/* -- hole -- */
	rtw_wx_set_rate,		/* SIOCSIWRATE */
	rtw_wx_get_rate,		/* SIOCGIWRATE */
	rtw_wx_set_rts,			/* SIOCSIWRTS */
	rtw_wx_get_rts,			/* SIOCGIWRTS */
	rtw_wx_set_frag,		/* SIOCSIWFRAG */
	rtw_wx_get_frag,		/* SIOCGIWFRAG */
	dummy,					/* SIOCSIWTXPOW */
	dummy,					/* SIOCGIWTXPOW */
	dummy,					/* SIOCSIWRETRY */
	rtw_wx_get_retry,		/* SIOCGIWRETRY */
	rtw_wx_set_enc,			/* SIOCSIWENCODE */
	rtw_wx_get_enc,			/* SIOCGIWENCODE */
	dummy,					/* SIOCSIWPOWER */
	rtw_wx_get_power,		/* SIOCGIWPOWER */
	NULL,					/*---hole---*/
	NULL,					/*---hole---*/
	rtw_wx_set_gen_ie,		/* SIOCSIWGENIE */
	NULL,					/* SIOCGWGENIE */
	rtw_wx_set_auth,		/* SIOCSIWAUTH */
	NULL,					/* SIOCGIWAUTH */
	rtw_wx_set_enc_ext,		/* SIOCSIWENCODEEXT */
	NULL,					/* SIOCGIWENCODEEXT */
	rtw_wx_set_pmkid,		/* SIOCSIWPMKSA */
	NULL,					/*---hole---*/
};


static const struct iw_priv_args rtw_private_args[] = {
	{
		SIOCIWFIRSTPRIV + 0x0,
		IW_PRIV_TYPE_CHAR | 0x7FF, 0, "write"
	},
	{
		SIOCIWFIRSTPRIV + 0x1,
		IW_PRIV_TYPE_CHAR | 0x7FF,
		IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_FIXED | IFNAMSIZ, "read"
	},
	{
		SIOCIWFIRSTPRIV + 0x2, 0, 0, "driver_ext"
	},
	{
		SIOCIWFIRSTPRIV + 0x3, 0, 0, "mp_ioctl"
	},
	{
		SIOCIWFIRSTPRIV + 0x4,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "apinfo"
	},
	{
		SIOCIWFIRSTPRIV + 0x5,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "setpid"
	},
	{
		SIOCIWFIRSTPRIV + 0x6,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wps_start"
	},
	/* for PLATFORM_MT53XX	 */
	{
		SIOCIWFIRSTPRIV + 0x7,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "get_sensitivity"
	},
	{
		SIOCIWFIRSTPRIV + 0x8,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wps_prob_req_ie"
	},
	{
		SIOCIWFIRSTPRIV + 0x9,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wps_assoc_req_ie"
	},

	/* for RTK_DMP_PLATFORM	 */
	{
		SIOCIWFIRSTPRIV + 0xA,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "channel_plan"
	},

	{
		SIOCIWFIRSTPRIV + 0xB,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "dbg"
	},
	{
		SIOCIWFIRSTPRIV + 0xC,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 3, 0, "rfw"
	},
	{
		SIOCIWFIRSTPRIV + 0xD,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_FIXED | IFNAMSIZ, "rfr"
	},
#if 0
	{
		SIOCIWFIRSTPRIV + 0xE, 0, 0, "wowlan_ctrl"
	},
#endif
	{
		SIOCIWFIRSTPRIV + 0x10,
		IW_PRIV_TYPE_CHAR | 1024, 0, "p2p_set"
	},
	{
		SIOCIWFIRSTPRIV + 0x11,
		IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK , "p2p_get"
	},
	{
		SIOCIWFIRSTPRIV + 0x12, 0, 0, "NULL"
	},
	{
		SIOCIWFIRSTPRIV + 0x13,
		IW_PRIV_TYPE_CHAR | 64, IW_PRIV_TYPE_CHAR | 64 , "p2p_get2"
	},
	{
		SIOCIWFIRSTPRIV + 0x14,
		IW_PRIV_TYPE_CHAR  | 64, 0, "tdls"
	},
	{
		SIOCIWFIRSTPRIV + 0x15,
		IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 , "tdls_get"
	},
	{
		SIOCIWFIRSTPRIV + 0x16,
		IW_PRIV_TYPE_CHAR | 64, 0, "pm_set"
	},
#ifdef CONFIG_RTW_80211K
	{
		SIOCIWFIRSTPRIV + 0x17,
		IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 , "rrm"
	},
#else
	{SIOCIWFIRSTPRIV + 0x17, IW_PRIV_TYPE_CHAR | 1024 , 0 , "NULL"},
#endif
	{SIOCIWFIRSTPRIV + 0x18, IW_PRIV_TYPE_CHAR | IFNAMSIZ , 0 , "rereg_nd_name"},
#ifdef CONFIG_MP_INCLUDED
	{SIOCIWFIRSTPRIV + 0x1A, IW_PRIV_TYPE_CHAR | 1024, 0,  "NULL"},
	{SIOCIWFIRSTPRIV + 0x1B, IW_PRIV_TYPE_CHAR | 128, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "NULL"},
#else
	{SIOCIWFIRSTPRIV + 0x1A, IW_PRIV_TYPE_CHAR | 1024, 0,  "NULL"},
	{SIOCIWFIRSTPRIV + 0x1B, IW_PRIV_TYPE_CHAR | 128, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "efuse_get"},
#endif
	{
		SIOCIWFIRSTPRIV + 0x1D,
		IW_PRIV_TYPE_CHAR | 40, IW_PRIV_TYPE_CHAR | 0x7FF, "test"
	},

	{ SIOCIWFIRSTPRIV + 0x0E, IW_PRIV_TYPE_CHAR | 1024, 0 , ""},  /* set  */
	{ SIOCIWFIRSTPRIV + 0x0F, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK , ""},/* get
 * --- sub-ioctls definitions --- */

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	{ VENDOR_IE_SET, IW_PRIV_TYPE_CHAR | 1024 , 0 , "vendor_ie_set" },
	{ VENDOR_IE_GET, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "vendor_ie_get" },
#endif
#if defined(RTW_PHL_TX) || defined(RTW_PHL_RX) || defined(CONFIG_PHL_TEST_SUITE)
	{ PHL_TEST_SET, IW_PRIV_TYPE_CHAR | 1024 , 0 , "phl_test" },
	{ PHL_TEST_GET, IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "" },
#endif
#ifdef CONFIG_WOWLAN
	{ MP_WOW_ENABLE , IW_PRIV_TYPE_CHAR | 1024, 0, "wow_mode" },
	{ MP_WOW_SET_PATTERN , IW_PRIV_TYPE_CHAR | 1024, 0, "wow_set_pattern" },
#endif
#ifdef CONFIG_AP_WOWLAN
	{ MP_AP_WOW_ENABLE , IW_PRIV_TYPE_CHAR | 1024, 0, "ap_wow_mode" }, /* set  */
#endif
#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	{ MP_SD_IREAD, IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "sd_iread" },
	{ MP_SD_IWRITE, IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "sd_iwrite" },
#endif
#ifdef CONFIG_MP_INCLUDED
	{ MP_GET_PHL_TEST, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "phl_get_io" },
	{ MP_SET_PHL_TEST, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "phl_set_io" },
#endif
#ifdef CONFIG_FPGA_INCLUDED
	{ FPGA_SET, IW_PRIV_TYPE_CHAR | 1024 , 0 , "fpga_set" },
#endif
};

/* --- sub-ioctls definitions --- */
static const struct iw_priv_args rtw_mp_private_args[] = {
#ifdef CONFIG_MP_INCLUDED
	{ MP_START , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_start" },
	{ MP_PHYPARA, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_phypara" },
	{ MP_STOP , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_stop" },
	{ MP_CHANNEL , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK ,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_channel" },
	{ MP_TRXSC_OFFSET , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK ,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_trxsc" },
	{ MP_BANDWIDTH , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_bandwidth"},
	{ MP_RATE , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_rate" },
	{ MP_RESET_STATS , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_reset_stats"},
	{ MP_QUERY , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK , "mp_query"},
	{ READ_REG , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "read_reg" },
	{ MP_RATE , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_rate" },
	{ READ_RF , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "read_rf" },
	{ MP_PSD , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_psd"},
	{ MP_DUMP, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_dump" },
	{ MP_TXPOWER , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_txpower"},
	{ MP_ANT_TX , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_ant_tx"},
	{ MP_ANT_RX , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, 
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_ant_rx"},
	{ WRITE_REG , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "write_reg" },
	{ WRITE_RF , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "write_rf" },
	{ MP_CTX , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_ctx"},
	{ MP_ARX , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_arx"},
	{ MP_THER , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_ther"},
	{ EFUSE_SET, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "efuse_set" },
	{ EFUSE_GET, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "efuse_get" },
	{ MP_PWRTRK , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_pwrtrk"},
	{ MP_QueryDrvStats, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_drvquery" },
	{ MP_SetRFPathSwh, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_setrfpath" },
	{ MP_PwrCtlDM, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_pwrctldm" },
	{ MP_GET_TXPOWER_INX, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_get_txpower" },
	{ MP_GETVER, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_priv_ver" },
	{ MP_MON, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_mon" },
	{ EFUSE_BT_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "efuse_bt_mask" },
	{ EFUSE_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "efuse_mask" },
	{ EFUSE_FILE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "efuse_file" },
	{ EFUSE_FILE_STORE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "efuse_store" },
	{ MP_TX, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_tx" },
	{ MP_RX, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_rx" },
	{ MP_HW_TX_MODE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_hxtx" },
	{ MP_PWRLMT, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, 
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_pwrlmt" },
	{ MP_PWRBYRATE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_pwrbyrate" },
	{ CTA_TEST, IW_PRIV_TYPE_CHAR | 1024, 0, "cta_test"},
	{ MP_IQK, IW_PRIV_TYPE_CHAR | 1024, 0, "mp_iqk"},
	{ MP_LCK, IW_PRIV_TYPE_CHAR | 1024, 0, "mp_lck"},
	{ BT_EFUSE_FILE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "bt_efuse_file" },
	{ MP_SWRFPath, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, 
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_swrfpath" },
	{ MP_LINK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, 
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_link" },
	{ MP_DPK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_dpk"},
	{ MP_DPK_TRK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, 
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_dpk_trk" },
	{ MP_GET_TSSIDE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_get_tsside" },
	{ MP_SET_TSSIDE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_set_tsside" },
	{ MP_GET_PHL_TEST, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "phl_get_io" },
	{ MP_SET_PHL_TEST, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "phl_set_io" },
	{ MP_SET_PHL_TX_PATTERN, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_tx_patt" },
	{ MP_SET_PHL_TX_METHOD, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "tx_method" },
	{ MP_SET_PHL_CONIFG_PHY_NUM, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_phy" },
	{ MP_SET_PHL_PLCP_TX_DATA, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_plcp_data" },
	{ MP_SET_PHL_PLCP_TX_USER, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_plcp_user" },
	{ MP_PHL_RFK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_rfk" },
	{ MP_PHL_BTC_PATH, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_btc_path" },
	{ MP_GET_HE, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_get_he" },
	{ MP_BAND, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_band" },
#ifdef CONFIG_RTW_CUSTOMER_STR
	{ MP_CUSTOMER_STR, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "customer_str" },
#endif
	{ MP_UUID , IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
				IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mp_uuid"},
#endif /* CONFIG_MP_INCLUDED */
};

static iw_handler rtw_private_handler[] = {
	rtw_wx_write32,					/* 0x00 */
	rtw_wx_read32,					/* 0x01 */
	NULL,					/* 0x02 */
#ifdef MP_IOCTL_HDL
	rtw_mp_ioctl_hdl,				/* 0x03 */
#else
	rtw_wx_priv_null,
#endif
	/* for MM DTV platform */
	rtw_get_ap_info,					/* 0x04 */

	rtw_set_pid,						/* 0x05 */
	rtw_wps_start,					/* 0x06 */

	/* for PLATFORM_MT53XX */
	rtw_wx_get_sensitivity,			/* 0x07 */
	rtw_wx_set_mtk_wps_probe_ie,	/* 0x08 */
	rtw_wx_set_mtk_wps_ie,			/* 0x09 */

	/* for RTK_DMP_PLATFORM
	 * Set Channel depend on the country code */
	rtw_wx_set_channel_plan,		/* 0x0A */

	rtw_dbg_port,					/* 0x0B */
	rtw_wx_write_rf,					/* 0x0C */
	rtw_wx_read_rf,					/* 0x0D */

	rtw_priv_set,					/*0x0E*/
	rtw_priv_get,					/*0x0F*/

	NULL, 						/* 0x10 */
	NULL,						/* 0x11 */
	NULL,						/* 0x12 */
	NULL,						/* 0x13 */

	rtw_tdls,						/* 0x14 */
	rtw_tdls_get,					/* 0x15 */

#ifdef CONFIG_RTW_80211K
	rtw_wx_priv_rrm,				/* 0x17 */
#else
	rtw_wx_priv_null,				/* 0x17 */
#endif
	rtw_rereg_nd_name,				/* 0x18 */
	rtw_wx_priv_null,				/* 0x19 */
#ifdef CONFIG_MP_INCLUDED
	rtw_wx_priv_null,				/* 0x1A */
	rtw_wx_priv_null,				/* 0x1B */
#else
	rtw_wx_priv_null,				/* 0x1A */
	/*rtw_mp_efuse_get,*/				/* 0x1B */
#endif
	NULL,							/* 0x1C is reserved for hostapd */
	rtw_test,						/* 0x1D */
};

#if WIRELESS_EXT >= 17
static struct iw_statistics *rtw_get_wireless_stats(struct net_device *dev)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct iw_statistics *piwstats = &padapter->iwstats;
	int tmp_level = 0;
	int tmp_qual = 0;
	int tmp_noise = 0;

	if (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) != _TRUE) {
		piwstats->qual.qual = 0;
		piwstats->qual.level = 0;
		piwstats->qual.noise = 0;
		/* RTW_INFO("No link  level:%d, qual:%d, noise:%d\n", tmp_level, tmp_qual, tmp_noise); */
	} else {
#ifdef CONFIG_SIGNAL_DISPLAY_DBM
		tmp_level = rtw_phl_rssi_to_dbm(padapter->recvinfo.signal_strength);
#else
		tmp_level = padapter->recvinfo.signal_strength;
#endif

		tmp_qual = padapter->recvinfo.signal_qual;
		/* RTW_INFO("level:%d, qual:%d, noise:%d, rssi (%d)\n", tmp_level, tmp_qual, tmp_noise,padapter->recvinfo.rssi); */

		piwstats->qual.level = tmp_level;
		piwstats->qual.qual = tmp_qual;
		piwstats->qual.noise = tmp_noise;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14))
	piwstats->qual.updated = IW_QUAL_ALL_UPDATED ;/* |IW_QUAL_DBM; */
#else
#ifdef RTK_DMP_PLATFORM
	/* IW_QUAL_DBM= 0x8, if driver use this flag, wireless extension will show value of dbm. */
	/* remove this flag for show percentage 0~100 */
	piwstats->qual.updated = 0x07;
#else
	piwstats->qual.updated = 0x0f;
#endif
#endif

#ifdef CONFIG_SIGNAL_DISPLAY_DBM
	piwstats->qual.updated = piwstats->qual.updated | IW_QUAL_DBM;
#endif

	return &padapter->iwstats;
}
#endif

#ifdef CONFIG_WIRELESS_EXT
struct iw_handler_def rtw_handlers_def = {
	.standard = rtw_handlers,
	.num_standard = sizeof(rtw_handlers) / sizeof(iw_handler),
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)) || defined(CONFIG_WEXT_PRIV)
	.private = rtw_private_handler,
	.private_args = (struct iw_priv_args *)rtw_private_args,
	.num_private = sizeof(rtw_private_handler) / sizeof(iw_handler),
	.num_private_args = sizeof(rtw_private_args) / sizeof(struct iw_priv_args),
#endif
#if WIRELESS_EXT >= 17
	.get_wireless_stats = rtw_get_wireless_stats,
#endif
};
#endif

/* copy from net/wireless/wext.c start
 * ----------------------------------------------------------------
 *
 * Calculate size of private arguments
 */
static const char iw_priv_type_size[] = {
	0,                              /* IW_PRIV_TYPE_NONE */
	1,                              /* IW_PRIV_TYPE_BYTE */
	1,                              /* IW_PRIV_TYPE_CHAR */
	0,                              /* Not defined */
	sizeof(__u32),                  /* IW_PRIV_TYPE_INT */
	sizeof(struct iw_freq),         /* IW_PRIV_TYPE_FLOAT */
	sizeof(struct sockaddr),        /* IW_PRIV_TYPE_ADDR */
	0,                              /* Not defined */
};

static int get_priv_size(__u16 args)
{
	int num = args & IW_PRIV_SIZE_MASK;
	int type = (args & IW_PRIV_TYPE_MASK) >> 12;

	return num * iw_priv_type_size[type];
}
/* copy from net/wireless/wext.c end */


static int _rtw_ioctl_wext_private(struct net_device *dev, union iwreq_data *wrq_data)
{
	int err = 0;
	u8 *input = NULL;
	u32 input_len = 0;
	const char delim[] = " ";
	u8 *output = NULL;
	u32 output_len = 0;
	u32 count = 0;
	u8 *buffer = NULL;
	u32 buffer_len = 0;
	char *ptr = NULL;
	u8 cmdname[17] = {0}; /* IFNAMSIZ+1 */
	u32 cmdlen;
	s32 len;
	u8 *extra = NULL;
	u32 extra_size = 0;

	s32 k;
	const iw_handler *priv;		/* Private ioctl */
	const struct iw_priv_args *priv_args;	/* Private ioctl description */
	const struct iw_priv_args *mp_priv_args;	/*MP Private ioctl description */
	const struct iw_priv_args *sel_priv_args;	/*Selected Private ioctl description */
	u32 num_priv;				/* Number of ioctl */
	u32 num_priv_args;			/* Number of descriptions */
	u32 num_mp_priv_args;			/*Number of MP descriptions */
	u32 num_sel_priv_args;			/*Number of Selected descriptions */
	iw_handler handler;
	int temp;
	int subcmd = 0;				/* sub-ioctl index */
	int offset = 0;				/* Space for sub-ioctl index */

	union iwreq_data wdata;

	_rtw_memcpy(&wdata, wrq_data, sizeof(wdata));

	input_len = wdata.data.length;
	if (!input_len)
		return -EINVAL;
	input = rtw_zmalloc(input_len);

	if (input == NULL) {
		err = -EOPNOTSUPP;
		goto exit;
	}

	if (copy_from_user(input, wdata.data.pointer, input_len)) {
		err = -EFAULT;
		goto exit;
	}
	input[input_len - 1] = '\0';
	ptr = input;
	len = input_len;

	sscanf(ptr, "%16s", cmdname);
	cmdlen = strlen(cmdname);
	RTW_DBG("%s: cmd=%s\n", __func__, cmdname);

	/* skip command string */
	if (cmdlen > 0)
		cmdlen += 1; /* skip one space */
	ptr += cmdlen;
	len -= cmdlen;
	RTW_DBG("%s: parameters=%s\n", __func__, ptr);

	priv = rtw_private_handler;
	priv_args = rtw_private_args;
	mp_priv_args = rtw_mp_private_args;
	num_priv = sizeof(rtw_private_handler) / sizeof(iw_handler);
	num_priv_args = sizeof(rtw_private_args) / sizeof(struct iw_priv_args);
	num_mp_priv_args = sizeof(rtw_mp_private_args) / sizeof(struct iw_priv_args);

	if (num_priv_args == 0) {
		err = -EOPNOTSUPP;
		goto exit;
	}

	/* Search the correct ioctl */
	k = -1;
	sel_priv_args = priv_args;
	num_sel_priv_args = num_priv_args;
	while
	((++k < num_sel_priv_args) && strcmp(sel_priv_args[k].name, cmdname))
		;

	/* If not found... */
	if (k == num_sel_priv_args) {
		k = -1;
		sel_priv_args = mp_priv_args;
		num_sel_priv_args = num_mp_priv_args;
		while
		((++k < num_sel_priv_args) && strcmp(sel_priv_args[k].name, cmdname))
			;

		if (k == num_sel_priv_args) {
			err = -EOPNOTSUPP;
			goto exit;
		}
	}

	/* Watch out for sub-ioctls ! */
	if (sel_priv_args[k].cmd < SIOCDEVPRIVATE) {
		int j = -1;

		/* Find the matching *real* ioctl */
		while ((++j < num_priv_args) && ((priv_args[j].name[0] != '\0') ||
			 (priv_args[j].set_args != sel_priv_args[k].set_args) ||
			 (priv_args[j].get_args != sel_priv_args[k].get_args)))
			;

		/* If not found... */
		if (j == num_priv_args) {
			err = -EINVAL;
			goto exit;
		}

		/* Save sub-ioctl number */
		subcmd = sel_priv_args[k].cmd;
		/* Reserve one int (simplify alignment issues) */
		offset = sizeof(__u32);
		/* Use real ioctl definition from now on */
		k = j;
	}

	buffer = rtw_zmalloc(4096);
	if (NULL == buffer) {
		err = -ENOMEM;
		goto exit;
	}

	if (k >= num_priv_args) {
		err = -EINVAL;
		goto exit;
	}

	/* If we have to set some data */
	if ((priv_args[k].set_args & IW_PRIV_TYPE_MASK) &&
	    (priv_args[k].set_args & IW_PRIV_SIZE_MASK)) {
		u8 *str;

		switch (priv_args[k].set_args & IW_PRIV_TYPE_MASK) {
		case IW_PRIV_TYPE_BYTE:
			/* Fetch args */
			count = 0;
			do {
				str = strsep(&ptr, delim);
				if (NULL == str)
					break;
				sscanf(str, "%i", &temp);
				buffer[count++] = (u8)temp;
			} while (1);
			buffer_len = count;

			/* Number of args to fetch */
			wdata.data.length = count;
			if (wdata.data.length > (priv_args[k].set_args & IW_PRIV_SIZE_MASK))
				wdata.data.length = priv_args[k].set_args & IW_PRIV_SIZE_MASK;

			break;

		case IW_PRIV_TYPE_INT:
			/* Fetch args */
			count = 0;
			do {
				str = strsep(&ptr, delim);
				if (NULL == str)
					break;
				sscanf(str, "%i", &temp);
				((s32 *)buffer)[count++] = (s32)temp;
			} while (1);
			buffer_len = count * sizeof(s32);

			/* Number of args to fetch */
			wdata.data.length = count;
			if (wdata.data.length > (priv_args[k].set_args & IW_PRIV_SIZE_MASK))
				wdata.data.length = priv_args[k].set_args & IW_PRIV_SIZE_MASK;

			break;

		case IW_PRIV_TYPE_CHAR:
			if (len > 0) {
				/* Size of the string to fetch */
				wdata.data.length = len;
				if (wdata.data.length > (priv_args[k].set_args & IW_PRIV_SIZE_MASK))
					wdata.data.length = priv_args[k].set_args & IW_PRIV_SIZE_MASK;

				/* Fetch string */
				_rtw_memcpy(buffer, ptr, wdata.data.length);
			} else {
				wdata.data.length = 1;
				buffer[0] = '\0';
			}
			buffer_len = wdata.data.length;
			break;

		default:
			RTW_INFO("%s: Not yet implemented...\n", __func__);
			err = -1;
			goto exit;
		}

		if ((priv_args[k].set_args & IW_PRIV_SIZE_FIXED) &&
		    (wdata.data.length != (priv_args[k].set_args & IW_PRIV_SIZE_MASK))) {
			RTW_INFO("%s: The command %s needs exactly %d argument(s)...\n",
				__func__, cmdname, priv_args[k].set_args & IW_PRIV_SIZE_MASK);
			err = -EINVAL;
			goto exit;
		}
	}   /* if args to set */
	else
		wdata.data.length = 0L;

	/* Those two tests are important. They define how the driver
	* will have to handle the data */
	if ((priv_args[k].set_args & IW_PRIV_SIZE_FIXED) &&
	    ((get_priv_size(priv_args[k].set_args) + offset) <= IFNAMSIZ)) {
		/* First case : all SET args fit within wrq */
		if (offset)
			wdata.mode = subcmd;
		_rtw_memcpy(wdata.name + offset, buffer, IFNAMSIZ - offset);
	} else {
		if ((priv_args[k].set_args == 0) &&
		    (priv_args[k].get_args & IW_PRIV_SIZE_FIXED) &&
		    (get_priv_size(priv_args[k].get_args) <= IFNAMSIZ)) {
			/* Second case : no SET args, GET args fit within wrq */
			if (offset)
				wdata.mode = subcmd;
		} else {
			/* Third case : args won't fit in wrq, or variable number of args */
			if (copy_to_user(wdata.data.pointer, buffer, buffer_len)) {
				err = -EFAULT;
				goto exit;
			}
			wdata.data.flags = subcmd;
		}
	}

	rtw_mfree(input, input_len);
	input = NULL;

	extra_size = 0;
	if (IW_IS_SET(priv_args[k].cmd)) {
		/* Size of set arguments */
		extra_size = get_priv_size(priv_args[k].set_args);

		/* Does it fits in iwr ? */
		if ((priv_args[k].set_args & IW_PRIV_SIZE_FIXED) &&
		    ((extra_size + offset) <= IFNAMSIZ))
			extra_size = 0;
	} else {
		/* Size of get arguments */
		extra_size = get_priv_size(priv_args[k].get_args);

		/* Does it fits in iwr ? */
		if ((priv_args[k].get_args & IW_PRIV_SIZE_FIXED) &&
		    (extra_size <= IFNAMSIZ))
			extra_size = 0;
	}

	if (extra_size == 0) {
		extra = (u8 *)&wdata;
		rtw_mfree(buffer, 4096);
		buffer = NULL;
	} else
		extra = buffer;

	handler = priv[priv_args[k].cmd - SIOCIWFIRSTPRIV];
	err = handler(dev, NULL, &wdata, extra);

	/* If we have to get some data */
	if ((priv_args[k].get_args & IW_PRIV_TYPE_MASK) &&
	    (priv_args[k].get_args & IW_PRIV_SIZE_MASK)) {
		int j;
		int n = 0;	/* number of args */
		u8 str[20] = {0};

		/* Check where is the returned data */
		if ((priv_args[k].get_args & IW_PRIV_SIZE_FIXED) &&
		    (get_priv_size(priv_args[k].get_args) <= IFNAMSIZ))
			n = priv_args[k].get_args & IW_PRIV_SIZE_MASK;
		else
			n = wdata.data.length;

		output = rtw_zmalloc(4096);
		if (NULL == output) {
			err =  -ENOMEM;
			goto exit;
		}

		switch (priv_args[k].get_args & IW_PRIV_TYPE_MASK) {
		case IW_PRIV_TYPE_BYTE:
			/* Display args */
			for (j = 0; j < n; j++) {
				sprintf(str, "%d  ", extra[j]);
				len = strlen(str);
				output_len = strlen(output);
				if ((output_len + len + 1) > 4096) {
					err = -E2BIG;
					goto exit;
				}
				_rtw_memcpy(output + output_len, str, len);
			}
			break;

		case IW_PRIV_TYPE_INT:
			/* Display args */
			for (j = 0; j < n; j++) {
				sprintf(str, "%d  ", ((__s32 *)extra)[j]);
				len = strlen(str);
				output_len = strlen(output);
				if ((output_len + len + 1) > 4096) {
					err = -E2BIG;
					goto exit;
				}
				_rtw_memcpy(output + output_len, str, len);
			}
			break;

		case IW_PRIV_TYPE_CHAR:
			/* Display args */
			_rtw_memcpy(output, extra, n);
			output_len = n;
			break;

		default:
			RTW_INFO("%s: Not yet implemented...\n", __func__);
			err = -1;
			goto exit;
		}

		output_len ++;
		wrq_data->data.length = output_len;
		if (copy_to_user(wrq_data->data.pointer, output, output_len)) {
			err = -EFAULT;
			goto exit;
		}
	}   /* if args to set */
	else
		wrq_data->data.length = 0;

exit:
	if (input)
		rtw_mfree(input, input_len);
	if (buffer)
		rtw_mfree(buffer, 4096);
	if (output)
		rtw_mfree(output, 4096);

	return err;
}

#ifdef CONFIG_COMPAT
static int rtw_ioctl_compat_wext_private(struct net_device *dev, struct ifreq *rq)
{
	struct compat_iw_point iwp_compat;
	union iwreq_data wrq_data;
	int err = 0;
	RTW_DBG("%s:...\n", __func__);
	if (copy_from_user(&iwp_compat, rq->ifr_ifru.ifru_data, sizeof(struct compat_iw_point)))
		return -EFAULT;

	wrq_data.data.pointer = compat_ptr(iwp_compat.pointer);
	wrq_data.data.length = iwp_compat.length;
	wrq_data.data.flags = iwp_compat.flags;

	err = _rtw_ioctl_wext_private(dev, &wrq_data);

	iwp_compat.pointer = ptr_to_compat(wrq_data.data.pointer);
	iwp_compat.length = wrq_data.data.length;
	iwp_compat.flags = wrq_data.data.flags;
	if (copy_to_user(rq->ifr_ifru.ifru_data, &iwp_compat, sizeof(struct compat_iw_point)))
		return -EFAULT;

	return err;
}
#endif /* CONFIG_COMPAT */

static int rtw_ioctl_standard_wext_private(struct net_device *dev, struct ifreq *rq)
{
	struct iw_point *iwp;
	union iwreq_data wrq_data;
	int err = 0;
	iwp = &wrq_data.data;
	RTW_DBG("%s:...\n", __func__);
	if (copy_from_user(iwp, rq->ifr_ifru.ifru_data, sizeof(struct iw_point)))
		return -EFAULT;

	err = _rtw_ioctl_wext_private(dev, &wrq_data);

	if (copy_to_user(rq->ifr_ifru.ifru_data, iwp, sizeof(struct iw_point)))
		return -EFAULT;

	return err;
}

static int rtw_ioctl_wext_private(struct net_device *dev, struct ifreq *rq)
{
#ifdef CONFIG_COMPAT
#if (KERNEL_VERSION(4, 6, 0) > LINUX_VERSION_CODE)
	if (is_compat_task())
#else
	if (in_compat_syscall())
#endif
		return rtw_ioctl_compat_wext_private(dev, rq);
	else
#endif /* CONFIG_COMPAT */
		return rtw_ioctl_standard_wext_private(dev, rq);
}

int rtw_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct iwreq *wrq = (struct iwreq *)rq;
	int ret = 0;

	switch (cmd) {
	case RTL_IOCTL_WPA_SUPPLICANT:
		ret = wpa_supplicant_ioctl(dev, &wrq->u.data);
		break;
#ifdef CONFIG_AP_MODE
	case RTL_IOCTL_HOSTAPD:
		ret = rtw_hostapd_ioctl(dev, &wrq->u.data);
		break;
#ifdef CONFIG_WIRELESS_EXT
	case SIOCSIWMODE:
		ret = rtw_wx_set_mode(dev, NULL, &wrq->u, NULL);
		break;
#endif
#endif /* CONFIG_AP_MODE */
	case SIOCDEVPRIVATE:
		ret = rtw_ioctl_wext_private(dev, rq);
		break;
	case (SIOCDEVPRIVATE+1):
		ret = rtw_android_priv_cmd(dev, rq, cmd);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	return ret;
}
