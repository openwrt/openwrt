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
#define _RTW_P2P_C_

#include <drv_types.h>

#ifdef CONFIG_P2P

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37))
	#error "Linux kernel version is too old to enable CONFIG_P2P"
#endif
#ifndef CONFIG_IOCTL_CFG80211
	#error "Enable CONFIG_P2P without CONFIG_IOCTL_CFG80211"
#endif

int is_any_client_associated(_adapter *padapter)
{
	return padapter->stapriv.asoc_list_cnt ? _TRUE : _FALSE;
}

#ifdef CONFIG_WFD
u32 build_beacon_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u16 val16 = 0;
	u32 len = 0, wfdielen = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110812 */
	/*	According to the WFD Specification, the beacon frame should contain 4 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID */
	/*	3. Coupled Sink Information */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */

	if (P2P_ROLE_GO == pwdinfo->role) {
		if (is_any_client_associated(pwdinfo->padapter)) {
			/*	WFD primary sink + WiFi Direct mode + WSD (WFD Service Discovery) */
			val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_WSD;
			RTW_PUT_BE16(wfdie + wfdielen, val16);
		} else {
			/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD (WFD Service Discovery) */
			val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
			RTW_PUT_BE16(wfdie + wfdielen, val16);
		}

	} else {
		/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
		val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
		RTW_PUT_BE16(wfdie + wfdielen, val16);
	}

	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_probe_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u16 val16 = 0;
	u32 len = 0, wfdielen = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110812 */
	/*	According to the WFD Specification, the probe request frame should contain 4 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID */
	/*	3. Coupled Sink Information */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */

	if (1 == pwdinfo->wfd_tdls_enable) {
		/*	WFD primary sink + available for WFD session + WiFi TDLS mode + WSC ( WFD Service Discovery )	 */
		val16 = pwfd_info->wfd_device_type |
			WFD_DEVINFO_SESSION_AVAIL |
			WFD_DEVINFO_WSD |
			WFD_DEVINFO_PC_TDLS;
		RTW_PUT_BE16(wfdie + wfdielen, val16);
	} else {
		/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSC ( WFD Service Discovery )	 */
		val16 = pwfd_info->wfd_device_type |
			WFD_DEVINFO_SESSION_AVAIL |
			WFD_DEVINFO_WSD;
		RTW_PUT_BE16(wfdie + wfdielen, val16);
	}

	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_probe_resp_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf, u8 tunneled)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;
	u16 v16 = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110812 */
	/*	According to the WFD Specification, the probe response frame should contain 4 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID */
	/*	3. Coupled Sink Information */
	/*	4. WFD Session Information */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + available for WFD session + WiFi Direct mode */

	if (P2P_ROLE_GO == pwdinfo->role) {
		if (is_any_client_associated(pwdinfo->padapter)) {
			if (pwdinfo->wfd_tdls_enable) {
				/*	TDLS mode + WSD ( WFD Service Discovery ) */
				v16 = pwfd_info->wfd_device_type | WFD_DEVINFO_WSD | WFD_DEVINFO_PC_TDLS | WFD_DEVINFO_HDCP_SUPPORT;
				RTW_PUT_BE16(wfdie + wfdielen, v16);
			} else {
				/*	WiFi Direct mode + WSD ( WFD Service Discovery ) */
				v16 =  pwfd_info->wfd_device_type | WFD_DEVINFO_WSD | WFD_DEVINFO_HDCP_SUPPORT;
				RTW_PUT_BE16(wfdie + wfdielen, v16);
			}
		} else {
			if (pwdinfo->wfd_tdls_enable) {
				/*	available for WFD session + TDLS mode + WSD ( WFD Service Discovery ) */
				v16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD | WFD_DEVINFO_PC_TDLS | WFD_DEVINFO_HDCP_SUPPORT;
				RTW_PUT_BE16(wfdie + wfdielen, v16);
			} else {
				/*	available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
				v16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD | WFD_DEVINFO_HDCP_SUPPORT;
				RTW_PUT_BE16(wfdie + wfdielen, v16);
			}
		}
	} else {
		if (pwdinfo->wfd_tdls_enable) {
			/*	available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
			v16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD | WFD_DEVINFO_PC_TDLS | WFD_DEVINFO_HDCP_SUPPORT;
			RTW_PUT_BE16(wfdie + wfdielen, v16);
		} else {
			/*	available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
			v16 =  pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD | WFD_DEVINFO_HDCP_SUPPORT;
			RTW_PUT_BE16(wfdie + wfdielen, v16);
		}
	}

	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		/*	WFD Session Information ATTR */
		/*	Type: */
		wfdie[wfdielen++] = WFD_ATTR_SESSION_INFO;

		/*	Length: */
		/*	Note: In the WFD specification, the size of length field is 2. */
		RTW_PUT_BE16(wfdie + wfdielen, 0x0000);
		wfdielen += 2;

		/*	Todo: to add the list of WFD device info descriptor in WFD group. */

	}
#ifdef CONFIG_CONCURRENT_MODE
#ifdef CONFIG_TDLS
	{
		int i;
		_adapter *iface = NULL;
		struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			if ((iface) && rtw_is_adapter_up(iface)) {
				if (iface == padapter)
					continue;
				if (!rtw_iface_at_same_hwband(padapter, iface))
					continue;
				if ((tunneled == 0) && (iface->wdinfo.wfd_tdls_enable == 1)) {
					/*	Alternative MAC Address ATTR
						Type:					*/
					wfdie[wfdielen++] = WFD_ATTR_ALTER_MAC;

					/*	Length:
						Note: In the WFD specification, the size of length field is 2.*/
					RTW_PUT_BE16(wfdie + wfdielen,  ETH_ALEN);
					wfdielen += 2;

					/*	Value:
						Alternative MAC Address*/
					_rtw_memcpy(wfdie + wfdielen, adapter_mac_addr(iface), ETH_ALEN);
					wfdielen += ETH_ALEN;
				}
			}
		}
	}

#endif /* CONFIG_TDLS*/
#endif /* CONFIG_CONCURRENT_MODE */

	pbuf = rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_assoc_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u16 val16 = 0;
	u32 len = 0, wfdielen = 0;
	_adapter			*padapter = NULL;
	struct mlme_priv			*pmlmepriv = NULL;
	struct wifi_display_info		*pwfd_info = NULL;

	padapter = pwdinfo->padapter;
	pmlmepriv = &padapter->mlmepriv;
	pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/* Only P2P GC need to include the WFD IE in the association request */
	if (!rtw_p2p_chk_role(pwdinfo, P2P_ROLE_CLIENT))
		goto exit;

	/* WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110812 */
	/*	According to the WFD Specification, the probe request frame should contain 4 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID */
	/*	3. Coupled Sink Information */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_assoc_resp_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	u16 val16 = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110812 */
	/*	According to the WFD Specification, the probe request frame should contain 4 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID */
	/*	3. Coupled Sink Information */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_nego_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	u16 val16 = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110825 */
	/*	According to the WFD Specification, the negotiation request frame should contain 3 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID ( Optional ) */
	/*	3. Local IP Adress ( Optional ) */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + WiFi Direct mode + WSD ( WFD Service Discovery ) + WFD Session Available */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_WSD | WFD_DEVINFO_SESSION_AVAIL;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_nego_resp_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	u16 val16 = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110825 */
	/*	According to the WFD Specification, the negotiation request frame should contain 3 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID ( Optional ) */
	/*	3. Local IP Adress ( Optional ) */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + WiFi Direct mode + WSD ( WFD Service Discovery ) + WFD Session Available */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_WSD | WFD_DEVINFO_SESSION_AVAIL;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;


	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_nego_confirm_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	u16 val16 = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110825 */
	/*	According to the WFD Specification, the negotiation request frame should contain 3 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID ( Optional ) */
	/*	3. Local IP Adress ( Optional ) */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + WiFi Direct mode + WSD ( WFD Service Discovery ) + WFD Session Available */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_WSD | WFD_DEVINFO_SESSION_AVAIL;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;


	pbuf = rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_invitation_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	u16 val16 = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110825 */
	/*	According to the WFD Specification, the provision discovery request frame should contain 3 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID ( Optional ) */
	/*	3. Local IP Adress ( Optional ) */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	if (P2P_ROLE_GO == pwdinfo->role) {
		/*	WFD Session Information ATTR */
		/*	Type: */
		wfdie[wfdielen++] = WFD_ATTR_SESSION_INFO;

		/*	Length: */
		/*	Note: In the WFD specification, the size of length field is 2. */
		RTW_PUT_BE16(wfdie + wfdielen, 0x0000);
		wfdielen += 2;

		/*	Todo: to add the list of WFD device info descriptor in WFD group. */

	}

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_invitation_resp_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u16 val16 = 0;
	u32 len = 0, wfdielen = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110825 */
	/*	According to the WFD Specification, the provision discovery request frame should contain 3 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID ( Optional ) */
	/*	3. Local IP Adress ( Optional ) */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	if (P2P_ROLE_GO == pwdinfo->role) {
		/*	WFD Session Information ATTR */
		/*	Type: */
		wfdie[wfdielen++] = WFD_ATTR_SESSION_INFO;

		/*	Length: */
		/*	Note: In the WFD specification, the size of length field is 2. */
		RTW_PUT_BE16(wfdie + wfdielen, 0x0000);
		wfdielen += 2;

		/*	Todo: to add the list of WFD device info descriptor in WFD group. */

	}

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_provdisc_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	u16 val16 = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110825 */
	/*	According to the WFD Specification, the provision discovery request frame should contain 3 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID ( Optional ) */
	/*	3. Local IP Adress ( Optional ) */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;


	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}

u32 build_provdisc_resp_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
	u32 len = 0, wfdielen = 0;
	u16 val16 = 0;
	_adapter *padapter = pwdinfo->padapter;
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info	*pwfd_info = padapter->wdinfo.wfd_info;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
		goto exit;

	/*	WFD OUI */
	wfdielen = 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;	/*	WFA WFD v1.0 */

	/*	Commented by Albert 20110825 */
	/*	According to the WFD Specification, the provision discovery response frame should contain 3 WFD attributes */
	/*	1. WFD Device Information */
	/*	2. Associated BSSID ( Optional ) */
	/*	3. Local IP Adress ( Optional ) */


	/*	WFD Device Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value1: */
	/*	WFD device information */
	/*	WFD primary sink + available for WFD session + WiFi Direct mode + WSD ( WFD Service Discovery ) */
	val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;

	/*	Value2: */
	/*	Session Management Control Port */
	/*	Default TCP port for RTSP messages is 554 */
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;

	/*	Value3: */
	/*	WFD Device Maximum Throughput */
	/*	300Mbps is the maximum throughput */
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	/*	Associated BSSID ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	/*	Value: */
	/*	Associated BSSID */
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		_rtw_memcpy(wfdie + wfdielen, &pmlmepriv->assoc_bssid[0], ETH_ALEN);
	else
		_rtw_memset(wfdie + wfdielen, 0x00, ETH_ALEN);

	wfdielen += ETH_ALEN;

	/*	Coupled Sink Information ATTR */
	/*	Type: */
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

	/*	Length: */
	/*	Note: In the WFD specification, the size of length field is 2. */
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;

	/*	Value: */
	/*	Coupled Sink Status bitmap */
	/*	Not coupled/available for Coupling */
	wfdie[wfdielen++] = 0;
	/* MAC Addr. */
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;
	wfdie[wfdielen++] = 0;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, (unsigned char *) wfdie, &len);

exit:
	return len;
}
#endif /* CONFIG_WFD */

u32 process_probe_req_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pframe, uint len)
{
	u8 *p;
	u32 ret = _FALSE;
	u8 *p2pie;
	u32	p2pielen = 0;
	int ssid_len = 0, rate_cnt = 0;

	p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_, _SUPPORTEDRATES_IE_, (int *)&rate_cnt,
		       len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_);

	if (rate_cnt <= 4) {
		int i, g_rate = 0;

		for (i = 0; i < rate_cnt; i++) {
			if (((*(p + 2 + i) & 0xff) != 0x02) &&
			    ((*(p + 2 + i) & 0xff) != 0x04) &&
			    ((*(p + 2 + i) & 0xff) != 0x0B) &&
			    ((*(p + 2 + i) & 0xff) != 0x16))
				g_rate = 1;
		}

		if (g_rate == 0) {
			/*	There is no OFDM rate included in SupportedRates IE of this probe request frame */
			/*	The driver should response this probe request. */
			return ret;
		}
	} else {
		/*	rate_cnt > 4 means the SupportRates IE contains the OFDM rate because the count of CCK rates are 4. */
		/*	We should proceed the following check for this probe request. */
	}

	/*	Added comments by Albert 20100906 */
	/*	There are several items we should check here. */
	/*	1. This probe request frame must contain the P2P IE. (Done) */
	/*	2. This probe request frame must contain the wildcard SSID. (Done) */
	/*	3. Wildcard BSSID. (Todo) */
	/*	4. Destination Address. ( Done in mgt_dispatcher function ) */
	/*	5. Requested Device Type in WSC IE. (Todo) */
	/*	6. Device ID attribute in P2P IE. (Todo) */

	p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_, _SSID_IE_, (int *)&ssid_len,
		       len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_);

	ssid_len &= 0xff;	/*	Just last 1 byte is valid for ssid len of the probe request */
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DEVICE) || rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		p2pie = rtw_get_p2p_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_ , len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_ , NULL, &p2pielen);
		if (p2pie) {
			if ((p != NULL) && _rtw_memcmp((void *)(p + 2), (void *) pwdinfo->p2p_wildcard_ssid , 7)) {
				/* todo: */
				/* Check Requested Device Type attributes in WSC IE. */
				/* Check Device ID attribute in P2P IE */

				ret = _TRUE;
			} else if ((p != NULL) && (ssid_len == 0))
				ret = _TRUE;
		} else {
			/* non -p2p device */
		}

	}


	return ret;

}

u32 process_assoc_req_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pframe, uint len, struct sta_info *psta)
{
	u8 status_code = P2P_STATUS_SUCCESS;
	u8 *pbuf, *pattr_content = NULL;
	u32 attr_contentlen = 0;
	u16 cap_attr = 0;
	unsigned short	frame_type, ie_offset = 0;
	u8 *ies;
	u32 ies_len;
	u8 *p2p_ie;
	u32	p2p_ielen = 0;

	if (!rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO))
		return P2P_STATUS_FAIL_REQUEST_UNABLE;

	frame_type = get_frame_sub_type(pframe);
	if (frame_type == WIFI_ASSOCREQ)
		ie_offset = _ASOCREQ_IE_OFFSET_;
	else /* WIFI_REASSOCREQ */
		ie_offset = _REASOCREQ_IE_OFFSET_;

	ies = pframe + WLAN_HDR_A3_LEN + ie_offset;
	ies_len = len - WLAN_HDR_A3_LEN - ie_offset;

	p2p_ie = rtw_get_p2p_ie(ies , ies_len , NULL, &p2p_ielen);

	if (!p2p_ie) {
		RTW_INFO("[%s] P2P IE not Found!!\n", __FUNCTION__);
		status_code =  P2P_STATUS_FAIL_INVALID_PARAM;
	} else
		RTW_INFO("[%s] P2P IE Found!!\n", __FUNCTION__);

	while (p2p_ie) {
		/* Check P2P Capability ATTR */
		attr_contentlen = sizeof(cap_attr);
		if (rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_CAPABILITY, (u8 *)&cap_attr, (uint *) &attr_contentlen)) {
			RTW_INFO("[%s] Got P2P Capability Attr!!\n", __FUNCTION__);
			cap_attr = le16_to_cpu(cap_attr);
			psta->dev_cap = cap_attr & 0xff;
		}

		/* Check Extended Listen Timing ATTR */


		/* Check P2P Device Info ATTR */
		if (rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_DEVICE_INFO, NULL, (uint *)&attr_contentlen)) {
			RTW_INFO("[%s] Got P2P DEVICE INFO Attr!!\n", __FUNCTION__);
			pattr_content = pbuf = rtw_zmalloc(attr_contentlen);
			if (pattr_content) {
				u8 num_of_secdev_type;
				u16 dev_name_len;


				rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_DEVICE_INFO , pattr_content, (uint *)&attr_contentlen);

				_rtw_memcpy(psta->dev_addr, 	pattr_content, ETH_ALEN);/* P2P Device Address */

				pattr_content += ETH_ALEN;

				_rtw_memcpy(&psta->config_methods, pattr_content, 2);/* Config Methods */
				psta->config_methods = be16_to_cpu(psta->config_methods);

				pattr_content += 2;

				_rtw_memcpy(psta->primary_dev_type, pattr_content, 8);

				pattr_content += 8;

				num_of_secdev_type = *pattr_content;
				pattr_content += 1;

				if (num_of_secdev_type == 0)
					psta->num_of_secdev_type = 0;
				else {
					u32 len;

					psta->num_of_secdev_type = num_of_secdev_type;

					len = (sizeof(psta->secdev_types_list) < (num_of_secdev_type * 8)) ? (sizeof(psta->secdev_types_list)) : (num_of_secdev_type * 8);

					_rtw_memcpy(psta->secdev_types_list, pattr_content, len);

					pattr_content += (num_of_secdev_type * 8);
				}


				/* dev_name_len = attr_contentlen - ETH_ALEN - 2 - 8 - 1 - (num_of_secdev_type*8); */
				psta->dev_name_len = 0;
				if (WPS_ATTR_DEVICE_NAME == be16_to_cpu(*(u16 *)pattr_content)) {
					dev_name_len = be16_to_cpu(*(u16 *)(pattr_content + 2));

					psta->dev_name_len = (sizeof(psta->dev_name) < dev_name_len) ? sizeof(psta->dev_name) : dev_name_len;

					_rtw_memcpy(psta->dev_name, pattr_content + 4, psta->dev_name_len);
				}

				rtw_mfree(pbuf, attr_contentlen);

			}

		}

		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);

	}

	return status_code;

}

#ifdef CONFIG_IOCTL_CFG80211
#if 0
static void rtw_change_p2pie_op_ch(_adapter *padapter, const u8 *frame_body, u32 len, u8 ch)
{
	u8 *ies, *p2p_ie;
	u32 ies_len, p2p_ielen;

	ies = (u8 *)(frame_body + _PUBLIC_ACTION_IE_OFFSET_);
	ies_len = len - _PUBLIC_ACTION_IE_OFFSET_;

	p2p_ie = rtw_get_p2p_ie(ies, ies_len, NULL, &p2p_ielen);

	while (p2p_ie) {
		u32	attr_contentlen = 0;
		u8 *pattr = NULL;

		/* Check P2P_ATTR_OPERATING_CH */
		attr_contentlen = 0;
		pattr = NULL;
		pattr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, (uint *)&attr_contentlen);
		if (pattr != NULL)
			*(pattr + 4) = ch;

		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);
	}
}
#endif

#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
static void rtw_change_p2pie_ch_list(_adapter *padapter, const u8 *frame_body, u32 len, u8 ch)
{
	u8 *ies, *p2p_ie;
	u32 ies_len, p2p_ielen;

	ies = (u8 *)(frame_body + _PUBLIC_ACTION_IE_OFFSET_);
	ies_len = len - _PUBLIC_ACTION_IE_OFFSET_;

	p2p_ie = rtw_get_p2p_ie(ies, ies_len, NULL, &p2p_ielen);

	while (p2p_ie) {
		u32	attr_contentlen = 0;
		u8 *pattr = NULL;

		/* Check P2P_ATTR_CH_LIST */
		pattr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_CH_LIST, NULL, (uint *)&attr_contentlen);
		if (pattr != NULL) {
			int i;
			u32 num_of_ch;
			u8 *pattr_temp = pattr + 3 ;

			attr_contentlen -= 3;

			while (attr_contentlen > 0) {
				num_of_ch = *(pattr_temp + 1);

				for (i = 0; i < num_of_ch; i++)
					*(pattr_temp + 2 + i) = ch;

				pattr_temp += (2 + num_of_ch);
				attr_contentlen -= (2 + num_of_ch);
			}
		}

		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);
	}
}
#endif

#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
static bool rtw_chk_p2pie_ch_list_with_buddy(_adapter *padapter, const u8 *frame_body, u32 len)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	bool fit = _FALSE;
	u8 *ies, *p2p_ie;
	u32 ies_len, p2p_ielen;
	struct rtw_chan_def u_chdef = {0};
	u8 union_ch = 0;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role,
				padapter_link->wrlink, &u_chdef)
			!= RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s get union chandef failed\n", __func__);
		rtw_warn_on(1);
	}
	union_ch = u_chdef.chan;

	ies = (u8 *)(frame_body + _PUBLIC_ACTION_IE_OFFSET_);
	ies_len = len - _PUBLIC_ACTION_IE_OFFSET_;

	p2p_ie = rtw_get_p2p_ie(ies, ies_len, NULL, &p2p_ielen);

	while (p2p_ie) {
		u32	attr_contentlen = 0;
		u8 *pattr = NULL;

		/* Check P2P_ATTR_CH_LIST */
		pattr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_CH_LIST, NULL, (uint *)&attr_contentlen);
		if (pattr != NULL) {
			int i;
			u32 num_of_ch;
			u8 *pattr_temp = pattr + 3 ;

			attr_contentlen -= 3;

			while (attr_contentlen > 0) {
				num_of_ch = *(pattr_temp + 1);

				for (i = 0; i < num_of_ch; i++) {
					if (*(pattr_temp + 2 + i) == union_ch) {
						RTW_INFO(FUNC_ADPT_FMT" ch_list fit buddy_ch:%u\n", FUNC_ADPT_ARG(padapter), union_ch);
						fit = _TRUE;
						break;
					}
				}

				pattr_temp += (2 + num_of_ch);
				attr_contentlen -= (2 + num_of_ch);
			}
		}

		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);
	}

	return fit;
}

#if defined(CONFIG_P2P_INVITE_IOT)
static bool rtw_chk_p2pie_op_ch_with_buddy(_adapter *padapter, const u8 *frame_body, u32 len)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	bool fit = _FALSE;
	u8 *ies, *p2p_ie;
	u32 ies_len, p2p_ielen;
	struct rtw_chan_def u_chdef = {0};
	u8 union_ch = 0;

	if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role, &u_chdef)
							!= RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s get union chandef failed\n", __func__);
		rtw_warn_on(1);
	}
	union_ch = u_chdef.chan;
	ies = (u8 *)(frame_body + _PUBLIC_ACTION_IE_OFFSET_);
	ies_len = len - _PUBLIC_ACTION_IE_OFFSET_;

	p2p_ie = rtw_get_p2p_ie(ies, ies_len, NULL, &p2p_ielen);

	while (p2p_ie) {
		u32	attr_contentlen = 0;
		u8 *pattr = NULL;

		/* Check P2P_ATTR_OPERATING_CH */
		attr_contentlen = 0;
		pattr = NULL;
		pattr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, (uint *)&attr_contentlen);
		if (pattr != NULL) {
			if (*(pattr + 4) == union_ch) {
				RTW_INFO(FUNC_ADPT_FMT" op_ch fit buddy_ch:%u\n", FUNC_ADPT_ARG(padapter), union_ch);
				fit = _TRUE;
				break;
			}
		}

		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);
	}

	return fit;
}
#endif

static void rtw_cfg80211_adjust_p2pie_channel(_adapter *padapter, const u8 *frame_body, u32 len)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u8 *ies, *p2p_ie;
	u32 ies_len, p2p_ielen;
	struct rtw_chan_def u_chdef = {0};
	u8 union_ch = 0;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role,
					padapter_link->wrlink, &u_chdef)
						!= RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s get union chandef failed\n", __func__);
		rtw_warn_on(1);
	}
	union_ch = u_chdef.chan;

	ies = (u8 *)(frame_body + _PUBLIC_ACTION_IE_OFFSET_);
	ies_len = len - _PUBLIC_ACTION_IE_OFFSET_;

	p2p_ie = rtw_get_p2p_ie(ies, ies_len, NULL, &p2p_ielen);

	while (p2p_ie) {
		u32	attr_contentlen = 0;
		u8 *pattr = NULL;

		/* Check P2P_ATTR_CH_LIST */
		pattr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_CH_LIST, NULL, (uint *)&attr_contentlen);
		if (pattr != NULL) {
			int i;
			u32 num_of_ch;
			u8 *pattr_temp = pattr + 3 ;

			attr_contentlen -= 3;

			while (attr_contentlen > 0) {
				num_of_ch = *(pattr_temp + 1);

				for (i = 0; i < num_of_ch; i++) {
					if (*(pattr_temp + 2 + i) && *(pattr_temp + 2 + i) != union_ch) {
						RTW_ERR("replace ch_list:%u with:%u\n", *(pattr_temp + 2 + i), union_ch);
						*(pattr_temp + 2 + i) = union_ch; /*forcing to the same channel*/
					}
				}

				pattr_temp += (2 + num_of_ch);
				attr_contentlen -= (2 + num_of_ch);
			}
		}

		/* Check P2P_ATTR_OPERATING_CH */
		attr_contentlen = 0;
		pattr = NULL;
		pattr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, (uint *)&attr_contentlen);
		if (pattr != NULL) {
			if (*(pattr + 4) && *(pattr + 4) != union_ch) {
				RTW_ERR("replace op_ch:%u with:%u\n", *(pattr + 4), union_ch);
				*(pattr + 4) = union_ch; /*forcing to the same channel	*/
			}
		}

		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);

	}

}
#endif

#ifdef CONFIG_WFD
u32 rtw_xframe_build_wfd_ie(struct xmit_frame *xframe)
{
	_adapter *adapter = xframe->padapter;
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	u8 *frame = xframe->buf_addr + TXDESC_OFFSET;
	u8 *frame_body = frame + sizeof(struct rtw_ieee80211_hdr_3addr);
	u8 *frame_tail = frame + xframe->attrib.pktlen;
	u8 category, action, OUI_Subtype, dialogToken = 0;
	u32	wfdielen = 0;

	category = frame_body[0];
	if (category == RTW_WLAN_CATEGORY_PUBLIC) {
		action = frame_body[1];
		if (action == ACT_PUBLIC_VENDOR
		    && _rtw_memcmp(frame_body + 2, P2P_OUI, 4) == _TRUE
		   ) {
			OUI_Subtype = frame_body[6];
			dialogToken = frame_body[7];

			switch (OUI_Subtype) {
			case P2P_GO_NEGO_REQ:
				wfdielen = build_nego_req_wfd_ie(wdinfo, frame_tail);
				break;
			case P2P_GO_NEGO_RESP:
				wfdielen = build_nego_resp_wfd_ie(wdinfo, frame_tail);
				break;
			case P2P_GO_NEGO_CONF:
				wfdielen = build_nego_confirm_wfd_ie(wdinfo, frame_tail);
				break;
			case P2P_INVIT_REQ:
				wfdielen = build_invitation_req_wfd_ie(wdinfo, frame_tail);
				break;
			case P2P_INVIT_RESP:
				wfdielen = build_invitation_resp_wfd_ie(wdinfo, frame_tail);
				break;
			case P2P_PROVISION_DISC_REQ:
				wfdielen = build_provdisc_req_wfd_ie(wdinfo, frame_tail);
				break;
			case P2P_PROVISION_DISC_RESP:
				wfdielen = build_provdisc_resp_wfd_ie(wdinfo, frame_tail);
				break;
			case P2P_DEVDISC_REQ:
			case P2P_DEVDISC_RESP:
			default:
				break;
			}

		}
	} else if (category == RTW_WLAN_CATEGORY_P2P) {
		OUI_Subtype = frame_body[5];
		dialogToken = frame_body[6];

#ifdef CONFIG_DEBUG_CFG80211
		RTW_INFO("ACTION_CATEGORY_P2P: OUI=0x%x, OUI_Subtype=%d, dialogToken=%d\n"
			, cpu_to_be32(*((u32 *)(frame_body + 1))), OUI_Subtype, dialogToken);
#endif

		switch (OUI_Subtype) {
		case P2P_NOTICE_OF_ABSENCE:
			break;
		case P2P_PRESENCE_REQUEST:
			break;
		case P2P_PRESENCE_RESPONSE:
			break;
		case P2P_GO_DISC_REQUEST:
			break;
		default:
			break;
		}
	} else
		RTW_INFO("%s, action frame category=%d\n", __func__, category);

	xframe->attrib.pktlen += wfdielen;

	return wfdielen;
}
#endif /* CONFIG_WFD */

bool rtw_xframe_del_wfd_ie(struct xmit_frame *xframe)
{
#define DBG_XFRAME_DEL_WFD_IE 0
	u8 *frame = xframe->buf_addr + TXDESC_OFFSET;
	u8 *frame_body = frame + sizeof(struct rtw_ieee80211_hdr_3addr);
	u8 *frame_tail = frame + xframe->attrib.pktlen;
	u8 category, action, OUI_Subtype;
	u8 *ies = NULL;
	uint ies_len_ori = 0;
	uint ies_len = 0;

	category = frame_body[0];
	if (category == RTW_WLAN_CATEGORY_PUBLIC) {
		action = frame_body[1];
		if (action == ACT_PUBLIC_VENDOR
		    && _rtw_memcmp(frame_body + 2, P2P_OUI, 4) == _TRUE
		   ) {
			OUI_Subtype = frame_body[6];

			switch (OUI_Subtype) {
			case P2P_GO_NEGO_REQ:
			case P2P_GO_NEGO_RESP:
			case P2P_GO_NEGO_CONF:
			case P2P_INVIT_REQ:
			case P2P_INVIT_RESP:
			case P2P_PROVISION_DISC_REQ:
			case P2P_PROVISION_DISC_RESP:
				ies = frame_body + 8;
				ies_len_ori = frame_tail - (frame_body + 8);
				break;
			}
		}
	}

	if (ies && ies_len_ori) {
		ies_len = rtw_del_wfd_ie(ies, ies_len_ori, DBG_XFRAME_DEL_WFD_IE ? __func__ : NULL);
		xframe->attrib.pktlen -= (ies_len_ori - ies_len);
	}

	return ies_len_ori != ies_len;
}

/*
* rtw_xframe_chk_wfd_ie -
*
*/
void rtw_xframe_chk_wfd_ie(struct xmit_frame *xframe)
{
	_adapter *adapter = xframe->padapter;
#ifdef CONFIG_IOCTL_CFG80211
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
#endif
	u8 build = 0;
	u8 del = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		del = 1;

#ifdef CONFIG_IOCTL_CFG80211
	if (wdinfo->wfd_info->wfd_enable == _TRUE)
#endif
		del = build = 1;

	if (del)
		rtw_xframe_del_wfd_ie(xframe);

#ifdef CONFIG_WFD
	if (build)
		rtw_xframe_build_wfd_ie(xframe);
#endif
}

u8 *dump_p2p_attr_ch_list(u8 *p2p_ie, uint p2p_ielen, u8 *buf, u32 buf_len)
{
	uint attr_contentlen = 0;
	u8 *pattr = NULL;
	int w_sz = 0;
	u8 ch_cnt = 0;
	u8 ch_list[MAX_CHANNEL_NUM];

	pattr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_CH_LIST, NULL, &attr_contentlen);
	if (pattr != NULL) {
		int i, j;
		u32 num_of_ch;
		u8 *pattr_temp = pattr + 3 ;

		attr_contentlen -= 3;

		_rtw_memset(ch_list, 0, 40);

		while (attr_contentlen > 0) {
			num_of_ch = *(pattr_temp + 1);

			for (i = 0; i < num_of_ch; i++) {
				for (j = 0; j < ch_cnt; j++) {
					if (ch_list[j] == *(pattr_temp + 2 + i))
						break;
				}
				if (j >= ch_cnt)
					ch_list[ch_cnt++] = *(pattr_temp + 2 + i);
				if (ch_cnt == MAX_CHANNEL_NUM) {
					RTW_INFO("channel list array is used up, may need to increase array size\n");
					goto make_str;
				}
			}
			pattr_temp += (2 + num_of_ch);
			attr_contentlen -= (2 + num_of_ch);
		}
make_str:
		for (j = 0; j < ch_cnt; j++) {
			if (j == 0)
				w_sz += snprintf(buf + w_sz, buf_len - w_sz, "%u", ch_list[j]);
			else if (ch_list[j] - ch_list[j - 1] != 1)
				w_sz += snprintf(buf + w_sz, buf_len - w_sz, ", %u", ch_list[j]);
			else if (j != ch_cnt - 1 && ch_list[j + 1] - ch_list[j] == 1) {
				/* empty */
			} else
				w_sz += snprintf(buf + w_sz, buf_len - w_sz, "-%u", ch_list[j]);
		}
	}
	return buf;
}

/*
 * return _TRUE if requester is GO, _FALSE if responder is GO
 */
bool rtw_p2p_nego_intent_compare(u8 req, u8 resp)
{
	if (req >> 1 == resp >> 1)
		return  req & 0x01 ? _TRUE : _FALSE;
	else if (req >> 1 > resp >> 1)
		return _TRUE;
	else
		return _FALSE;
}

int rtw_p2p_check_frames(_adapter *padapter, const u8 *buf, u32 len, u8 tx)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	int is_p2p_frame = (-1);
	unsigned char	*frame_body;
	u8 category, action, OUI_Subtype, dialogToken = 0;
	u8 *p2p_ie = NULL;
	uint p2p_ielen = 0;
	struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);
	int status = -1;
	u8 ch_list_buf[128] = {'\0'};
	int op_ch = -1;
	int listen_ch = -1;
	u8 intent = 0;
	u8 *iaddr = NULL;
	u8 *gbssid = NULL;

	frame_body = (unsigned char *)(buf + sizeof(struct rtw_ieee80211_hdr_3addr));
	category = frame_body[0];
	/* just for check */
	if (category == RTW_WLAN_CATEGORY_PUBLIC) {
		action = frame_body[1];
		if (action == ACT_PUBLIC_VENDOR
			&& _rtw_memcmp(frame_body + 2, P2P_OUI, 4) == _TRUE
		) {
			OUI_Subtype = frame_body[6];
			dialogToken = frame_body[7];
			is_p2p_frame = OUI_Subtype;

			#ifdef CONFIG_DEBUG_CFG80211
			RTW_INFO("ACTION_CATEGORY_PUBLIC: ACT_PUBLIC_VENDOR, OUI=0x%x, OUI_Subtype=%d, dialogToken=%d\n",
				cpu_to_be32(*((u32 *)(frame_body + 2))), OUI_Subtype, dialogToken);
			#endif

			p2p_ie = rtw_get_p2p_ie(
				(u8 *)buf + sizeof(struct rtw_ieee80211_hdr_3addr) + _PUBLIC_ACTION_IE_OFFSET_
				, len - sizeof(struct rtw_ieee80211_hdr_3addr) - _PUBLIC_ACTION_IE_OFFSET_
				, NULL, &p2p_ielen);

			switch (OUI_Subtype) { /* OUI Subtype */
				u8 *cont;
				uint cont_len;
			case P2P_GO_NEGO_REQ: {
				struct rtw_wdev_nego_info *nego_info = &pwdev_priv->nego_info;

				if (tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
					if (rtw_mi_check_status(padapter, MI_LINKED) && padapter->registrypriv.full_ch_in_p2p_handshake == 0)
						rtw_cfg80211_adjust_p2pie_channel(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr));
					#endif
				}

				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, &cont_len);
				if (cont)
					op_ch = *(cont + 4);
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_LISTEN_CH, NULL, &cont_len);
				if (cont)
					listen_ch = *(cont + 4);
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_GO_INTENT, NULL, &cont_len);
				if (cont)
					intent = *cont;
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_INTENDED_IF_ADDR, NULL, &cont_len);
				if (cont && cont_len == 6)
					iaddr = cont;

				if (nego_info->token != dialogToken)
					rtw_wdev_nego_info_init(nego_info);

				_rtw_memcpy(nego_info->peer_mac, tx ? GetAddr1Ptr(buf) : get_addr2_ptr(buf), ETH_ALEN);
				if (iaddr)
					_rtw_memcpy(tx ? nego_info->iface_addr : nego_info->peer_iface_addr, iaddr, ETH_ALEN);
				nego_info->active = tx ? 1 : 0;
				nego_info->token = dialogToken;
				nego_info->req_op_ch = op_ch;
				nego_info->req_listen_ch = listen_ch;
				nego_info->req_intent = intent;
				nego_info->state = 0;

				dump_p2p_attr_ch_list(p2p_ie, p2p_ielen, ch_list_buf, 128);
				RTW_INFO("RTW_%s:P2P_GO_NEGO_REQ, dialogToken=%d, intent:%u%s, listen_ch:%d, op_ch:%d, ch_list:%s"
					, (tx == _TRUE) ? "Tx" : "Rx" , dialogToken , (intent >> 1) , intent & 0x1 ? "+" : "-" , listen_ch , op_ch , ch_list_buf);
				if (iaddr)
					_RTW_INFO(", iaddr:"MAC_FMT, MAC_ARG(iaddr));
				_RTW_INFO("\n");

				if (!tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
					{
						if (rtw_mi_check_status(padapter, MI_LINKED)
						    && rtw_chk_p2pie_ch_list_with_buddy(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr)) == _FALSE
						    && padapter->registrypriv.full_ch_in_p2p_handshake == 0) {
							RTW_INFO(FUNC_ADPT_FMT" ch_list has no intersect with buddy\n", FUNC_ADPT_ARG(padapter));
							rtw_change_p2pie_ch_list(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr), 0);
						}
					}
					#endif
				}

				break;
			}
			case P2P_GO_NEGO_RESP: {
				struct rtw_wdev_nego_info *nego_info = &pwdev_priv->nego_info;

				if (tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
						if (rtw_mi_check_status(padapter, MI_LINKED) && padapter->registrypriv.full_ch_in_p2p_handshake == 0)
							rtw_cfg80211_adjust_p2pie_channel(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr));
					#endif
				}

				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, &cont_len);
				if (cont)
					op_ch = *(cont + 4);
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_GO_INTENT, NULL, &cont_len);
				if (cont)
					intent = *cont;
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_STATUS, NULL, &cont_len);
				if (cont)
					status = *cont;
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_INTENDED_IF_ADDR, NULL, &cont_len);
				if (cont && cont_len == 6)
					iaddr = cont;

				if (nego_info->token == dialogToken && nego_info->state == 0
					&& _rtw_memcmp(nego_info->peer_mac, tx ? GetAddr1Ptr(buf) : get_addr2_ptr(buf), ETH_ALEN) == _TRUE
				) {
					if (iaddr)
						_rtw_memcpy(tx ? nego_info->iface_addr : nego_info->peer_iface_addr, iaddr, ETH_ALEN);
					nego_info->status = (status == -1) ? 0xff : status;
					nego_info->rsp_op_ch = op_ch;
					nego_info->rsp_intent = intent;
					nego_info->state = 1;
					if (status != 0)
						nego_info->token = 0; /* init */
				}

				dump_p2p_attr_ch_list(p2p_ie, p2p_ielen, ch_list_buf, 128);
				RTW_INFO("RTW_%s:P2P_GO_NEGO_RESP, dialogToken=%d, intent:%u%s, status:%d, op_ch:%d, ch_list:%s"
					, (tx == _TRUE) ? "Tx" : "Rx", dialogToken, (intent >> 1), intent & 0x1 ? "+" : "-", status, op_ch, ch_list_buf);
				if (iaddr)
					_RTW_INFO(", iaddr:"MAC_FMT, MAC_ARG(iaddr));
				_RTW_INFO("\n");

				if (!tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
					{
						if (rtw_mi_check_status(padapter, MI_LINKED)
						    && rtw_chk_p2pie_ch_list_with_buddy(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr)) == _FALSE
						    && padapter->registrypriv.full_ch_in_p2p_handshake == 0) {
							RTW_INFO(FUNC_ADPT_FMT" ch_list has no intersect with buddy\n", FUNC_ADPT_ARG(padapter));
							rtw_change_p2pie_ch_list(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr), 0);
						}
					}
					#endif
				}

				break;
			}
			case P2P_GO_NEGO_CONF: {
				struct rtw_wdev_nego_info *nego_info = &pwdev_priv->nego_info;
				bool is_go = _FALSE;

				if (tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
						if (rtw_mi_check_status(padapter, MI_LINKED) && padapter->registrypriv.full_ch_in_p2p_handshake == 0)
							rtw_cfg80211_adjust_p2pie_channel(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr));
					#endif
				}

				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, &cont_len);
				if (cont)
					op_ch = *(cont + 4);
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_STATUS, NULL, &cont_len);
				if (cont)
					status = *cont;

				if (nego_info->token == dialogToken && nego_info->state == 1
				    && _rtw_memcmp(nego_info->peer_mac, tx ? GetAddr1Ptr(buf) : get_addr2_ptr(buf), ETH_ALEN) == _TRUE
				   ) {
					nego_info->status = (status == -1) ? 0xff : status;
					nego_info->conf_op_ch = (op_ch == -1) ? 0 : op_ch;
					nego_info->state = 2;

					if (status == 0) {
						if (rtw_p2p_nego_intent_compare(nego_info->req_intent, nego_info->rsp_intent) ^ !tx)
							is_go = _TRUE;
					}

					nego_info->token = 0; /* init */
				}

				dump_p2p_attr_ch_list(p2p_ie, p2p_ielen, ch_list_buf, 128);
				RTW_INFO("RTW_%s:P2P_GO_NEGO_CONF, dialogToken=%d, status:%d, op_ch:%d, ch_list:%s\n"
					, (tx == _TRUE) ? "Tx" : "Rx", dialogToken, status, op_ch, ch_list_buf);

				if (!tx) {
				}

				break;
			}
			case P2P_INVIT_REQ: {
				struct rtw_wdev_invit_info *invit_info = &pwdev_priv->invit_info;
				int flags = -1;

				if (tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
						if (rtw_mi_check_status(padapter, MI_LINKED)
						    && padapter->registrypriv.full_ch_in_p2p_handshake == 0)
							rtw_cfg80211_adjust_p2pie_channel(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr));
					#endif
				}

				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_INVITATION_FLAGS, NULL, &cont_len);
				if (cont)
					flags = *cont;
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, &cont_len);
				if (cont)
					op_ch = *(cont + 4);
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_GROUP_BSSID, NULL, &cont_len);
				if (cont && cont_len == 6)
					gbssid = cont;

				if (invit_info->token != dialogToken)
					rtw_wdev_invit_info_init(invit_info);

				_rtw_memcpy(invit_info->peer_mac, tx ? GetAddr1Ptr(buf) : get_addr2_ptr(buf), ETH_ALEN);
				if (gbssid)
					_rtw_memcpy(invit_info->group_bssid, gbssid, ETH_ALEN);
				invit_info->active = tx ? 1 : 0;
				invit_info->token = dialogToken;
				invit_info->flags = (flags == -1) ? 0x0 : flags;
				invit_info->req_op_ch = op_ch;
				invit_info->state = 0;

				dump_p2p_attr_ch_list(p2p_ie, p2p_ielen, ch_list_buf, 128);
				RTW_INFO("RTW_%s:P2P_INVIT_REQ, dialogToken=%d, flags:0x%02x, op_ch:%d, ch_list:%s"
					, (tx == _TRUE) ? "Tx" : "Rx", dialogToken, flags, op_ch, ch_list_buf);
				if (gbssid)
					_RTW_INFO(", gbssid:"MAC_FMT, MAC_ARG(gbssid));
				_RTW_INFO("\n");

				if (!tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
					{
						if (rtw_mi_check_status(padapter, MI_LINKED) && padapter->registrypriv.full_ch_in_p2p_handshake == 0) {
							#if defined(CONFIG_P2P_INVITE_IOT)
							if (op_ch != -1 && rtw_chk_p2pie_op_ch_with_buddy(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr)) == _FALSE) {
								RTW_INFO(FUNC_ADPT_FMT" op_ch:%u has no intersect with buddy\n", FUNC_ADPT_ARG(padapter), op_ch);
								rtw_change_p2pie_ch_list(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr), 0);
							} else
							#endif
							if (rtw_chk_p2pie_ch_list_with_buddy(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr)) == _FALSE) {
								RTW_INFO(FUNC_ADPT_FMT" ch_list has no intersect with buddy\n", FUNC_ADPT_ARG(padapter));
								rtw_change_p2pie_ch_list(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr), 0);
							}
						}
					}
					#endif
				}

				break;
			}
			case P2P_INVIT_RESP: {
				struct rtw_wdev_invit_info *invit_info = &pwdev_priv->invit_info;

				if (tx) {
					#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT)
					#ifdef CONFIG_DBCC_SUPPORT
					if (GET_PHL_COM(dvobj)->dev_cap.dbcc_sup == false)
					#endif /*CONFIG_DBCC_SUPPORT*/
						if (rtw_mi_check_status(padapter, MI_LINKED) && padapter->registrypriv.full_ch_in_p2p_handshake == 0)
							rtw_cfg80211_adjust_p2pie_channel(padapter, frame_body, len - sizeof(struct rtw_ieee80211_hdr_3addr));
					#endif
				}

				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_STATUS, NULL, &cont_len);
				if (cont) {
					#ifdef CONFIG_P2P_INVITE_IOT
					if (tx && *cont == 7) {
						RTW_INFO("TX_P2P_INVITE_RESP, status is no common channel, change to unknown group\n");
						*cont = 8; /* unknow group status */
					}
					#endif /* CONFIG_P2P_INVITE_IOT */
					status = *cont;
				}
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_OPERATING_CH, NULL, &cont_len);
				if (cont)
					op_ch = *(cont + 4);
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_GROUP_BSSID, NULL, &cont_len);
				if (cont && cont_len == 6)
					gbssid = cont;

				if (invit_info->token == dialogToken && invit_info->state == 0
				    && _rtw_memcmp(invit_info->peer_mac, tx ? GetAddr1Ptr(buf) : get_addr2_ptr(buf), ETH_ALEN) == _TRUE
				   ) {
					invit_info->status = (status == -1) ? 0xff : status;
					invit_info->rsp_op_ch = op_ch;
					invit_info->state = 1;
					invit_info->token = 0; /* init */
				}

				dump_p2p_attr_ch_list(p2p_ie, p2p_ielen, ch_list_buf, 128);
				RTW_INFO("RTW_%s:P2P_INVIT_RESP, dialogToken=%d, status:%d, op_ch:%d, ch_list:%s"
					, (tx == _TRUE) ? "Tx" : "Rx", dialogToken, status, op_ch, ch_list_buf);
				if (gbssid)
					_RTW_INFO(", gbssid:"MAC_FMT, MAC_ARG(gbssid));
				_RTW_INFO("\n");

				if (!tx) {
				}

				break;
			}
			case P2P_DEVDISC_REQ:
				RTW_INFO("RTW_%s:P2P_DEVDISC_REQ, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken);
				break;
			case P2P_DEVDISC_RESP:
				cont = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_STATUS, NULL, &cont_len);
				RTW_INFO("RTW_%s:P2P_DEVDISC_RESP, dialogToken=%d, status:%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken, cont ? *cont : -1);
				break;
			case P2P_PROVISION_DISC_REQ:
				RTW_INFO("RTW_%s:P2P_PROVISION_DISC_REQ, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken);
				break;
			case P2P_PROVISION_DISC_RESP:
				RTW_INFO("RTW_%s:P2P_PROVISION_DISC_RESP, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken);
				break;
			default:
				RTW_INFO("RTW_%s:OUI_Subtype=%d, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", OUI_Subtype, dialogToken);
				break;
			}

		}

	} else if (category == RTW_WLAN_CATEGORY_P2P) {
		OUI_Subtype = frame_body[5];
		dialogToken = frame_body[6];

		#ifdef CONFIG_DEBUG_CFG80211
		RTW_INFO("ACTION_CATEGORY_P2P: OUI=0x%x, OUI_Subtype=%d, dialogToken=%d\n",
			cpu_to_be32(*((u32 *)(frame_body + 1))), OUI_Subtype, dialogToken);
		#endif

		is_p2p_frame = OUI_Subtype;

		switch (OUI_Subtype) {
		case P2P_NOTICE_OF_ABSENCE:
			RTW_INFO("RTW_%s:P2P_NOTICE_OF_ABSENCE, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken);
			break;
		case P2P_PRESENCE_REQUEST:
			RTW_INFO("RTW_%s:P2P_PRESENCE_REQUEST, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken);
			break;
		case P2P_PRESENCE_RESPONSE:
			RTW_INFO("RTW_%s:P2P_PRESENCE_RESPONSE, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken);
			break;
		case P2P_GO_DISC_REQUEST:
			RTW_INFO("RTW_%s:P2P_GO_DISC_REQUEST, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", dialogToken);
			break;
		default:
			RTW_INFO("RTW_%s:OUI_Subtype=%d, dialogToken=%d\n", (tx == _TRUE) ? "Tx" : "Rx", OUI_Subtype, dialogToken);
			break;
		}

	}

	return is_p2p_frame;
}
#endif /* CONFIG_IOCTL_CFG80211	 */

int process_p2p_cross_connect_ie(_adapter *padapter, u8 *IEs, u32 IELength)
{
	int ret = _TRUE;
	u8 *ies;
	u32 ies_len;
	u8 *p2p_ie;
	u32	p2p_ielen = 0;
	u8	p2p_attr[MAX_P2P_IE_LEN] = { 0x00 };/* NoA length should be n*(13) + 2 */
	u32	attr_contentlen = 0;



	if (IELength <= _BEACON_IE_OFFSET_)
		return ret;

	ies = IEs + _BEACON_IE_OFFSET_;
	ies_len = IELength - _BEACON_IE_OFFSET_;

	p2p_ie = rtw_get_p2p_ie(ies, ies_len, NULL, &p2p_ielen);

	while (p2p_ie) {
		/* Get P2P Manageability IE. */
		attr_contentlen = sizeof(p2p_attr);
		if (rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_MANAGEABILITY, p2p_attr, &attr_contentlen)) {
			if ((p2p_attr[0] & (BIT(0) | BIT(1))) == 0x01)
				ret = _FALSE;
			break;
		}
		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);
	}

	return ret;
}

#ifdef CONFIG_P2P_PS

static void _build_p2p_go_noa_ie(struct wifidirect_info *wdinfo,
				 u8 *ie, u16 *ie_len)
{
	u8 *p2p_noa_attr_ie;
	u8 p2p_noa_attr_pos = 0;
	u8 noa_desc_num = 1;
	u8 opp_ps = 0; /* Disable OppPS */
	u8 noa_count = 0;
	u32 noa_duration = 0;
	u32 noa_interval = 0;
	u32 noa_start_time = 0;
	u8 noa_index = 0;
	/* support 1 NoA desc */
	u8 num_noa_desc = 1;
	u8 noa_attr_len = 0;
	u16 noa_ie_len = 0;

	noa_index = wdinfo->noa_index;
	noa_count = wdinfo->noa_count[0];
	noa_duration = wdinfo->noa_duration[0];
	noa_interval = wdinfo->noa_interval[0];
	noa_start_time = wdinfo->noa_start_time[0];
	/* Noa Attrute Body Len */
	noa_attr_len = num_noa_desc * 13 + 2;
	/* P2P NoA IE Len(included P2P_OUI, attrute ID, attrute Len) */
	noa_ie_len = noa_attr_len + 4 + 1 + 2;
	/* Totol IE Len(included Element ID & NoA IE Len) */
	*ie_len = noa_ie_len + 2;
	if (*ie_len > MAX_P2P_IE_LEN) {
		RTW_ERR("%s: ie len = %d\n", __func__, *ie_len);
		*ie_len = 0;
		return;
	}

	/* Element ID:0xDD  */
	*ie = _VENDOR_SPECIFIC_IE_;
	/* NoA IE Len */
	*(ie + 1) = noa_ie_len;
	p2p_noa_attr_ie = (ie + 2);

	/* P2P OUI(4 bytes) */
	_rtw_memcpy(p2p_noa_attr_ie, P2P_OUI, 4);
	p2p_noa_attr_pos = p2p_noa_attr_pos + 4;

	/* attrute ID(1 byte) */
	p2p_noa_attr_ie[p2p_noa_attr_pos] = P2P_ATTR_NOA;
	p2p_noa_attr_pos = p2p_noa_attr_pos + 1;

	/* attrute length(2 bytes) length = noa_desc_num*13 + 2 */
	RTW_PUT_LE16(p2p_noa_attr_ie + p2p_noa_attr_pos,
		     (noa_desc_num * 13 + 2));
	p2p_noa_attr_pos = p2p_noa_attr_pos + 2;

	/* Index (1 byte) */
	p2p_noa_attr_ie[p2p_noa_attr_pos] = noa_index;
	p2p_noa_attr_pos = p2p_noa_attr_pos + 1;

	/* CTWindow and OppPS Parameters (1 byte) */
	p2p_noa_attr_ie[p2p_noa_attr_pos] = opp_ps;
	p2p_noa_attr_pos = p2p_noa_attr_pos+ 1;

	/* NoA Count (1 byte) */
	p2p_noa_attr_ie[p2p_noa_attr_pos] = noa_count;
	p2p_noa_attr_pos = p2p_noa_attr_pos + 1;

	/* NoA Duration (4 bytes) unit: microseconds */
	RTW_PUT_LE32(p2p_noa_attr_ie + p2p_noa_attr_pos, noa_duration);
	p2p_noa_attr_pos = p2p_noa_attr_pos + 4;

	/* NoA Interval (4 bytes) unit: microseconds */
	RTW_PUT_LE32(p2p_noa_attr_ie + p2p_noa_attr_pos, noa_interval);
	p2p_noa_attr_pos = p2p_noa_attr_pos + 4;

	/* NoA Start Time (4 bytes) unit: microseconds */
	RTW_PUT_LE32(p2p_noa_attr_ie + p2p_noa_attr_pos, noa_start_time);
}

u8 *rtw_append_p2p_go_noa_ie(struct _ADAPTER *adapter, u8 *frame, u32 *len)
{
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	u16 p2p_go_noa_ie_len = 0;

	if (!MLME_IS_GO(adapter))
		return frame;

	if (wdinfo->noa_num == 0)
		return frame;

	_build_p2p_go_noa_ie(wdinfo, frame, &p2p_go_noa_ie_len);
	*len = *len + p2p_go_noa_ie_len;

	return frame + p2p_go_noa_ie_len;
}

static void _tsf32_tog_update_single_noa(void *priv,
					 struct rtw_wifi_role_t *w_role,
					 struct rtw_phl_noa_desc *desc)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *) priv;
	struct _ADAPTER *a = NULL;
	u8 id = 0;
	u8 ret = _SUCCESS;
	struct wifidirect_info *wdinfo;
	struct _ADAPTER_LINK *alink;

	if (w_role == NULL) {
		RTW_ERR("%s wrole is NULL\n", __func__);
		ret = _FAIL;
		goto exit;
	}

	id = w_role->id;
	if (id >= CONFIG_IFACE_NUMBER) {
		RTW_ERR("%s error id (%d)\n", __func__, id);
		ret = _FAIL;
		goto exit;
	}

	a = dvobj->padapters[id];
	if (a == NULL) {
		RTW_ERR("%s adapter(%d) is NULL\n", __func__, id);
		ret = _FAIL;
		goto exit;
	}

	/* by pass non-GO case */
	if (!MLME_IS_GO(a))
		goto exit;

	RTW_INFO(FUNC_ADPT_FMT":(%d)\n", FUNC_ADPT_ARG(a), id);
	RTW_INFO("start_t_h=0x%02x,start_t_l=0x%02x\n",
		 desc->start_t_h, desc->start_t_l);
	RTW_INFO("dur=0x%d,cnt=0x%d,interval=0x%d,noa id=%d\n",
		 desc->duration, desc->count, desc->interval, desc->noa_id);

	wdinfo = &a->wdinfo;
	/* update NoA IE */
	wdinfo->noa_index = wdinfo->noa_index + 1;
	wdinfo->noa_num = 1;
	wdinfo->noa_count[0] = desc->count;
	wdinfo->noa_duration[0] =desc->duration;
	wdinfo->noa_interval[0] = desc->interval;
	wdinfo->noa_start_time[0] = desc->start_t_l;

	alink = GET_PRIMARY_LINK(a);
	rtw_update_beacon(a, alink, _VENDOR_SPECIFIC_IE_, P2P_OUI, _TRUE,
			  RTW_CMDF_DIRECTLY);

exit:
	return;
}

struct rtw_phl_p2pps_ops rtw_p2pps_ops = {
	.priv = NULL,
	.tsf32_tog_update_noa = NULL,
	.tsf32_tog_update_single_noa = &_tsf32_tog_update_single_noa,
};

void rtw_core_register_p2pps_ops(struct dvobj_priv *dvobj)
{
	struct rtw_phl_p2pps_ops ops = {0};
	ops.priv = (void *)dvobj;
	rtw_phl_p2pps_init_ops(dvobj->phl, &ops);
}

void process_p2p_ps_ie(_adapter *padapter, u8 *IEs, u32 IELength)
{
	u8 *ies;
	u32 ies_len;
	u8 *p2p_ie;
	u32	p2p_ielen = 0;
	u8 *noa_attr; /* NoA length should be n*(13) + 2 */
	u32	attr_contentlen = 0;

	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	u8	find_p2p = _FALSE, find_p2p_ps = _FALSE;
	u8	noa_offset, noa_num, noa_index;


	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DISABLE) ||
	    rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DEVICE))
		return;

	if (IELength <= _BEACON_IE_OFFSET_)
		return;

	ies = IEs + _BEACON_IE_OFFSET_;
	ies_len = IELength - _BEACON_IE_OFFSET_;

	p2p_ie = rtw_get_p2p_ie(ies, ies_len, NULL, &p2p_ielen);

	while (p2p_ie) {
		find_p2p = _TRUE;
		/* Get Notice of Absence IE. */
		noa_attr = rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_NOA, NULL, &attr_contentlen);
		if (noa_attr) {
			find_p2p_ps = _TRUE;
			noa_index = noa_attr[0];

			if ((pwdinfo->p2p_ps_mode == P2P_PS_NONE) ||
			    (noa_index != pwdinfo->noa_index)) { /* if index change, driver should reconfigure related setting. */
				pwdinfo->noa_index = noa_index;
				pwdinfo->opp_ps = noa_attr[1] >> 7;
				if (pwdinfo->opp_ps != 1)
					pwdinfo->ctwindow = 0;
				else
					pwdinfo->ctwindow = noa_attr[1] & 0x7F;
				noa_offset = 2;
				noa_num = 0;
				/* NoA length should be n*(13) + 2 */
				if (attr_contentlen > 2 && (attr_contentlen - 2) % 13 == 0) {
					while (noa_offset < attr_contentlen && noa_num < P2P_MAX_NOA_NUM) {
						/* _rtw_memcpy(&wifidirect_info->noa_count[noa_num], &noa_attr[noa_offset], 1); */
						pwdinfo->noa_count[noa_num] = noa_attr[noa_offset];
						noa_offset += 1;

						_rtw_memcpy(&pwdinfo->noa_duration[noa_num], &noa_attr[noa_offset], 4);
						noa_offset += 4;

						_rtw_memcpy(&pwdinfo->noa_interval[noa_num], &noa_attr[noa_offset], 4);
						noa_offset += 4;

						_rtw_memcpy(&pwdinfo->noa_start_time[noa_num], &noa_attr[noa_offset], 4);
						noa_offset += 4;

						noa_num++;
					}
				}
				pwdinfo->noa_num = noa_num;

				if (pwdinfo->opp_ps == 1) {
					pwdinfo->p2p_ps_mode = P2P_PS_CTWINDOW;
					/* driver should wait LPS for entering CTWindow */
					/* if (adapter_to_pwrctl(padapter)->bFwCurrentInPSMode == _TRUE)
						p2p_ps_wk_cmd(padapter, P2P_PS_ENABLE, 1); */
				} else if (pwdinfo->noa_num > 0) {
					pwdinfo->p2p_ps_mode = P2P_PS_NOA;
					p2p_ps_wk_cmd(padapter, P2P_PS_ENABLE, 1);
				} else if (pwdinfo->p2p_ps_mode > P2P_PS_NONE)
					p2p_ps_wk_cmd(padapter, P2P_PS_DISABLE, 1);
			}

			break; /* find target, just break. */
		}

		/* Get the next P2P IE */
		p2p_ie = rtw_get_p2p_ie(p2p_ie + p2p_ielen, ies_len - (p2p_ie - ies + p2p_ielen), NULL, &p2p_ielen);

	}

	if (find_p2p == _TRUE) {
		if ((pwdinfo->p2p_ps_mode > P2P_PS_NONE) && (find_p2p_ps == _FALSE))
			p2p_ps_wk_cmd(padapter, P2P_PS_DISABLE, 1);
	}

}

void p2p_ps_wk_hdl(_adapter *padapter, u8 p2p_ps_state)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct wifidirect_info *pwdinfo = &padapter->wdinfo;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	int i;

	/* Pre action for p2p state */
	switch (p2p_ps_state) {
	case P2P_PS_DISABLE:
		RTW_INFO("%s, P2P PS disabled\n", __func__);

		rtw_phl_p2pps_noa_disable_all(GET_PHL_INFO(dvobj),
					      padapter->phl_role);
		pwdinfo->p2p_ps_mode = P2P_PS_NONE;
		pwdinfo->p2p_ps_state = p2p_ps_state;
		pwdinfo->noa_index = 0;
		pwdinfo->ctwindow = 0;
		pwdinfo->opp_ps = 0;
		pwdinfo->noa_num = 0;
		for (i = 0; i < P2P_MAX_NOA_NUM; i++) {
			pwdinfo->noa_count[i] = 0;
			pwdinfo->noa_duration[i] = 0;
			pwdinfo->noa_interval[i] = 0;
			pwdinfo->noa_start_time[i] = 0;
		}

		break;
	case P2P_PS_ENABLE:
		if (pwdinfo->p2p_ps_mode <= P2P_PS_NONE)
			return;

		RTW_INFO("%s, P2P PS enabled\n", __func__);

		/* Disable all NoA desc before update NoA desc */
		rtw_phl_p2pps_noa_disable_all(GET_PHL_INFO(dvobj),
					      padapter->phl_role);

		for (i = 0; i < pwdinfo->noa_num; i++) {
			struct rtw_phl_noa_desc desc = {0};
			enum rtw_phl_status phl_sts;

			desc.enable = _TRUE;
			/* config NOA start time */
			desc.start_t_h = (u32)(pmlmeext->TSFValue >> 32);
			desc.start_t_l = pwdinfo->noa_start_time[i];
			/* config NOA duration */
			desc.duration = pwdinfo->noa_duration[i];
			/* config NOA interval */
			desc.interval = pwdinfo->noa_interval[i];
			/* config NOA count */
			desc.count = pwdinfo->noa_count[i];
			desc.w_role = padapter->phl_role;
			desc.rlink = padapter_link->wrlink;
			if (pwdinfo->noa_count[i] == 255)
				desc.tag = P2PPS_TRIG_GC_255;
			else
				desc.tag = P2PPS_TRIG_GC;

			RTW_INFO("[NoA Desc]- entry: %d\n", i);
			RTW_INFO("[NoA Desc]- duration: %u\n", desc.duration);
			RTW_INFO("[NoA Desc]- interval: %u\n", desc.interval);
			RTW_INFO("[NoA Desc]- high_start_time: %u\n",
				 desc.start_t_h);
			RTW_INFO("[NoA Desc]- low_start_time: %u\n",
				 desc.start_t_l);
			RTW_INFO("[NoA Desc]- count: %u\n",desc.count);

			phl_sts = rtw_phl_p2pps_noa_update(GET_PHL_INFO(dvobj),
							   &desc);
			if (phl_sts != RTW_PHL_STATUS_SUCCESS)
				RTW_ERR("rtw_phl_p2pps_noa_update failed\n");
		}

		break;
	default:
		break;
	}
}

u8 p2p_ps_wk_cmd(_adapter *padapter, u8 p2p_ps_state, u8 enqueue)
{
	struct cmd_obj	*cmd;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u8 res = _SUCCESS;


	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DISABLE) ||
	    rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DEVICE))
		return res;

	if (enqueue) {
		cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (cmd == NULL) {
			res = _FAIL;
			goto exit;
		}
		cmd->padapter = padapter;

		pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (pdrvextra_cmd_parm == NULL) {
			rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}

		pdrvextra_cmd_parm->ec_id = P2P_PS_WK_CID;
		pdrvextra_cmd_parm->type = p2p_ps_state;
		pdrvextra_cmd_parm->size = 0;
		pdrvextra_cmd_parm->pbuf = NULL;

		init_h2fwcmd_w_parm_no_rsp(cmd, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);

		res = rtw_enqueue_cmd(pcmdpriv, cmd);
	} else
		p2p_ps_wk_hdl(padapter, p2p_ps_state);
exit:
	return res;
}

void rtw_append_probe_resp_p2p_go_noa(struct xmit_frame *xframe)
{
	_adapter *adapter = xframe->padapter;
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	u8 *frame = xframe->buf_addr + TXDESC_OFFSET;
	u8 frame_ie_offset = WLAN_HDR_A3_LEN + _PROBERSP_IE_OFFSET_;
	u8 *p2p_ie = NULL;
	u16 p2p_go_noa_ie_len = 0;

	if (!MLME_IS_GO(adapter))
		return;

	if (wdinfo->noa_num == 0)
		return;

	p2p_ie = rtw_get_p2p_ie(frame + frame_ie_offset,
				xframe->attrib.pktlen - frame_ie_offset,
				NULL, NULL);
	if (p2p_ie) {
		_build_p2p_go_noa_ie(wdinfo, frame + xframe->attrib.pktlen,
				     &p2p_go_noa_ie_len);
		xframe->attrib.pktlen += p2p_go_noa_ie_len;
	}
}
#endif /* CONFIG_P2P_PS */

void reset_global_wifidirect_info(_adapter *padapter)
{
	struct wifidirect_info	*pwdinfo;

	pwdinfo = &padapter->wdinfo;
	rtw_tdls_wfd_enable(padapter, 0);
	pwdinfo->wfd_tdls_weaksec = _TRUE;
}

#ifdef CONFIG_WFD
int rtw_init_wifi_display_info(_adapter *padapter)
{
	int	res = _SUCCESS;
	struct wifi_display_info *pwfd_info = &padapter->wfd_info;

	/* Used in P2P and TDLS */
	pwfd_info->init_rtsp_ctrlport = 554;
#ifdef CONFIG_IOCTL_CFG80211
	pwfd_info->rtsp_ctrlport = 0;
#else
	pwfd_info->rtsp_ctrlport = pwfd_info->init_rtsp_ctrlport; /* set non-zero value for legacy wfd */
#endif
	pwfd_info->tdls_rtsp_ctrlport = 0;
	pwfd_info->peer_rtsp_ctrlport = 0;	/*	Reset to 0 */
	pwfd_info->wfd_enable = _FALSE;
	pwfd_info->wfd_device_type = WFD_DEVINFO_PSINK;
	pwfd_info->scan_result_type = SCAN_RESULT_P2P_ONLY;

	/* Used in P2P */
	pwfd_info->peer_session_avail = _TRUE;
	pwfd_info->wfd_pc = _FALSE;

	/* Used in TDLS */
	_rtw_memset(pwfd_info->ip_address, 0x00, 4);
	_rtw_memset(pwfd_info->peer_ip_address, 0x00, 4);
	return res;

}

inline void rtw_wfd_enable(_adapter *adapter, bool on)
{
	struct wifi_display_info *wfdinfo = &adapter->wfd_info;

	if (on) {
		wfdinfo->rtsp_ctrlport = wfdinfo->init_rtsp_ctrlport;
		wfdinfo->wfd_enable = _TRUE;

	} else {
		wfdinfo->wfd_enable = _FALSE;
		wfdinfo->rtsp_ctrlport = 0;
	}
}

inline void rtw_wfd_set_ctrl_port(_adapter *adapter, u16 port)
{
	struct wifi_display_info *wfdinfo = &adapter->wfd_info;

	wfdinfo->init_rtsp_ctrlport = port;
	if (wfdinfo->wfd_enable == _TRUE)
		wfdinfo->rtsp_ctrlport = port;
	if (adapter->wdinfo.wfd_tdls_enable == 1)
		wfdinfo->tdls_rtsp_ctrlport = port;
}

inline void rtw_tdls_wfd_enable(_adapter *adapter, bool on)
{
	struct wifi_display_info *wfdinfo = &adapter->wfd_info;

	if (on) {
		wfdinfo->tdls_rtsp_ctrlport = wfdinfo->init_rtsp_ctrlport;
		adapter->wdinfo.wfd_tdls_enable = 1;

	} else {
		adapter->wdinfo.wfd_tdls_enable = 0;
		wfdinfo->tdls_rtsp_ctrlport = 0;
	}
}

u32 rtw_append_beacon_wfd_ie(_adapter *adapter, u8 *pbuf)
{
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	struct mlme_priv *mlme = &adapter->mlmepriv;
	u8 build_ie_by_self = 0;
	u32 len = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		goto exit;

#ifdef CONFIG_IOCTL_CFG80211
	if (_TRUE == wdinfo->wfd_info->wfd_enable)
#endif
		build_ie_by_self = 1;

	if (build_ie_by_self)
		len = build_beacon_wfd_ie(wdinfo, pbuf);
#ifdef CONFIG_IOCTL_CFG80211
	else if (mlme->wfd_beacon_ie && mlme->wfd_beacon_ie_len > 0) {
		len = mlme->wfd_beacon_ie_len;
		_rtw_memcpy(pbuf, mlme->wfd_beacon_ie, len);
	}
#endif

exit:
	return len;
}

u32 rtw_append_probe_req_wfd_ie(_adapter *adapter, u8 *pbuf)
{
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	struct mlme_priv *mlme = &adapter->mlmepriv;
	u8 build_ie_by_self = 0;
	u32 len = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		goto exit;

#ifdef CONFIG_IOCTL_CFG80211
	if (_TRUE == wdinfo->wfd_info->wfd_enable)
#endif
		build_ie_by_self = 1;

	if (build_ie_by_self)
		len = build_probe_req_wfd_ie(wdinfo, pbuf);
#ifdef CONFIG_IOCTL_CFG80211
	else if (mlme->wfd_probe_req_ie && mlme->wfd_probe_req_ie_len > 0) {
		len = mlme->wfd_probe_req_ie_len;
		_rtw_memcpy(pbuf, mlme->wfd_probe_req_ie, len);
	}
#endif

exit:
	return len;
}

u32 rtw_append_probe_resp_wfd_ie(_adapter *adapter, u8 *pbuf)
{
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	struct mlme_priv *mlme = &adapter->mlmepriv;
	u8 build_ie_by_self = 0;
	u32 len = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		goto exit;

#ifdef CONFIG_IOCTL_CFG80211
	if (_TRUE == wdinfo->wfd_info->wfd_enable)
#endif
		build_ie_by_self = 1;

	if (build_ie_by_self)
		len = build_probe_resp_wfd_ie(wdinfo, pbuf, 0);
#ifdef CONFIG_IOCTL_CFG80211
	else if (mlme->wfd_probe_resp_ie && mlme->wfd_probe_resp_ie_len > 0) {
		len = mlme->wfd_probe_resp_ie_len;
		_rtw_memcpy(pbuf, mlme->wfd_probe_resp_ie, len);
	}
#endif

exit:
	return len;
}

u32 rtw_append_assoc_req_wfd_ie(_adapter *adapter, u8 *pbuf)
{
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	struct mlme_priv *mlme = &adapter->mlmepriv;
	u8 build_ie_by_self = 0;
	u32 len = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		goto exit;

#ifdef CONFIG_IOCTL_CFG80211
	if (_TRUE == wdinfo->wfd_info->wfd_enable)
#endif
		build_ie_by_self = 1;

	if (build_ie_by_self)
		len = build_assoc_req_wfd_ie(wdinfo, pbuf);
#ifdef CONFIG_IOCTL_CFG80211
	else if (mlme->wfd_assoc_req_ie && mlme->wfd_assoc_req_ie_len > 0) {
		len = mlme->wfd_assoc_req_ie_len;
		_rtw_memcpy(pbuf, mlme->wfd_assoc_req_ie, len);
	}
#endif

exit:
	return len;
}

u32 rtw_append_assoc_resp_wfd_ie(_adapter *adapter, u8 *pbuf)
{
	struct wifidirect_info *wdinfo = &adapter->wdinfo;
	struct mlme_priv *mlme = &adapter->mlmepriv;
	u8 build_ie_by_self = 0;
	u32 len = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		goto exit;

#ifdef CONFIG_IOCTL_CFG80211
	if (_TRUE == wdinfo->wfd_info->wfd_enable)
#endif
		build_ie_by_self = 1;

	if (build_ie_by_self)
		len = build_assoc_resp_wfd_ie(wdinfo, pbuf);
#ifdef CONFIG_IOCTL_CFG80211
	else if (mlme->wfd_assoc_resp_ie && mlme->wfd_assoc_resp_ie_len > 0) {
		len = mlme->wfd_assoc_resp_ie_len;
		_rtw_memcpy(pbuf, mlme->wfd_assoc_resp_ie, len);
	}
#endif

exit:
	return len;
}

#endif /* CONFIG_WFD */

void init_wifidirect_info(_adapter *padapter, enum P2P_ROLE role)
{
	struct wifidirect_info	*pwdinfo;
#ifdef CONFIG_WFD
	struct wifi_display_info	*pwfd_info = &padapter->wfd_info;
#endif
	pwdinfo = &padapter->wdinfo;

	pwdinfo->padapter = padapter;

	if (role == P2P_ROLE_DEVICE)
		rtw_p2p_set_role(pwdinfo, P2P_ROLE_DEVICE);
	else if (role == P2P_ROLE_CLIENT)
		rtw_p2p_set_role(pwdinfo, P2P_ROLE_CLIENT);
	else if (role == P2P_ROLE_GO)
		rtw_p2p_set_role(pwdinfo, P2P_ROLE_GO);

	/*	Use the OFDM rate in the P2P probe response frame. ( 6(B), 9(B), 12, 18, 24, 36, 48, 54 )	 */
	pwdinfo->support_rate[0] = 0x8c;	/*	6(B) */
	pwdinfo->support_rate[1] = 0x92;	/*	9(B) */
	pwdinfo->support_rate[2] = 0x18;	/*	12 */
	pwdinfo->support_rate[3] = 0x24;	/*	18 */
	pwdinfo->support_rate[4] = 0x30;	/*	24 */
	pwdinfo->support_rate[5] = 0x48;	/*	36 */
	pwdinfo->support_rate[6] = 0x60;	/*	48 */
	pwdinfo->support_rate[7] = 0x6c;	/*	54 */

	_rtw_memcpy((void *) pwdinfo->p2p_wildcard_ssid, "DIRECT-", 7);

#ifdef CONFIG_WFD
	pwdinfo->wfd_info = pwfd_info;
#endif /* CONFIG_WFD */

	pwdinfo->wfd_tdls_enable = 0;
}

void _rtw_p2p_set_role(struct wifidirect_info *wdinfo, enum P2P_ROLE role)
{
	if (wdinfo->role != role) {
		wdinfo->role = role;
		rtw_mi_update_iface_status(&(wdinfo->padapter->mlmepriv), 0);
	}
}

#ifdef CONFIG_DBG_P2P

/**
 * rtw_p2p_role_txt - Get the p2p role name as a text string
 * @role: P2P role
 * Returns: The state name as a printable text string
 */
const char *rtw_p2p_role_txt(enum P2P_ROLE role)
{
	switch (role) {
	case P2P_ROLE_DISABLE:
		return "P2P_ROLE_DISABLE";
	case P2P_ROLE_DEVICE:
		return "P2P_ROLE_DEVICE";
	case P2P_ROLE_CLIENT:
		return "P2P_ROLE_CLIENT";
	case P2P_ROLE_GO:
		return "P2P_ROLE_GO";
	default:
		return "UNKNOWN";
	}
}

/**
 * rtw_p2p_state_txt - Get the p2p state name as a text string
 * @state: P2P state
 * Returns: The state name as a printable text string
 */
const char *rtw_p2p_state_txt(enum P2P_STATE state)
{
	switch (state) {
	case P2P_STATE_NONE:
		return "P2P_STATE_NONE";
	case P2P_STATE_IDLE:
		return "P2P_STATE_IDLE";
	case P2P_STATE_LISTEN:
		return "P2P_STATE_LISTEN";
	case P2P_STATE_FIND_PHASE_SEARCH:
		return "P2P_STATE_FIND_PHASE_SEARCH";
	case P2P_STATE_GONEGO_OK:
		return "P2P_STATE_GONEGO_OK";
	default:
		return "UNKNOWN";
	}
}

#if 0
void dbg_rtw_p2p_restore_state(struct wifidirect_info *wdinfo, const char *caller, int line)
{
	if (wdinfo->pre_p2p_state != -1) {
		RTW_INFO("[CONFIG_DBG_P2P]%s:%d restore from %s to %s\n", caller, line
			, p2p_state_str[wdinfo->p2p_state], p2p_state_str[wdinfo->pre_p2p_state]
			);
		_rtw_p2p_restore_state(wdinfo);
	} else {
		RTW_INFO("[CONFIG_DBG_P2P]%s:%d restore no pre state, cur state %s\n", caller, line
			 , p2p_state_str[wdinfo->p2p_state]
			);
	}
}
#endif
void dbg_rtw_p2p_set_role(struct wifidirect_info *wdinfo, enum P2P_ROLE role, const char *caller, int line)
{
	if (wdinfo->role != role) {
		enum P2P_ROLE old_role = wdinfo->role;
		_rtw_p2p_set_role(wdinfo, role);
		RTW_INFO("[CONFIG_DBG_P2P]%s:%d set_role from %s to %s\n", caller, line
			, rtw_p2p_role_txt(old_role), rtw_p2p_role_txt(wdinfo->role)
			);
	} else {
		RTW_INFO("[CONFIG_DBG_P2P]%s:%d set_role to same role %s\n", caller, line
			 , rtw_p2p_role_txt(wdinfo->role)
			);
	}
}
#endif /* CONFIG_DBG_P2P */


int rtw_p2p_enable(_adapter *padapter, enum P2P_ROLE role)
{
	int ret = _SUCCESS;
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (role == P2P_ROLE_DEVICE || role == P2P_ROLE_CLIENT || role == P2P_ROLE_GO) {
#ifndef CONFIG_CFG80211_REPORT_PROBE_REQ
#if defined(CONFIG_CONCURRENT_MODE) && !RTW_P2P_GROUP_INTERFACE
		/*	Commented by Albert 2011/12/30 */
		/*	The driver just supports 1 P2P group operation. */
		/*	So, this function will do nothing if the buddy adapter had enabled the P2P function. */
		/*if(!rtw_p2p_chk_state(pbuddy_wdinfo, P2P_STATE_NONE))
			return ret;*/
		/* Only selected interface can be P2P interface */
		if (padapter->iface_id != padapter->registrypriv.sel_p2p_iface) {
			RTW_ERR("%s, iface_id:%d is not P2P interface!\n", __func__, padapter->iface_id);
			ret = _FAIL;
			return ret;
		}
#endif
#endif

		/* The P2P device does not need to remove the CCK rate of the
		 * adapter as it can act as an STA.
		 */
		if (role != P2P_ROLE_DEVICE)
			update_tx_basic_rate(padapter, padapter_link, WLAN_MD_11AGN);

		/* Enable P2P function */
		init_wifidirect_info(padapter, role);

		#ifdef CONFIG_IOCTL_CFG80211
		adapter_wdev_data(padapter)->p2p_enabled = _TRUE;
		#endif

#if 0
		rtw_hal_set_phydm_var(padapter, HAL_PHYDM_P2P_STATE, NULL, _TRUE);
#ifdef CONFIG_WFD
		if (rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
			rtw_hal_set_phydm_var(padapter, HAL_PHYDM_WIFI_DISPLAY_STATE, NULL, _TRUE);
#endif
#endif
	} else if (role == P2P_ROLE_DISABLE) {

		#ifdef CONFIG_IOCTL_CFG80211
		adapter_wdev_data(padapter)->p2p_enabled = _FALSE;
		#endif

		pwdinfo->listen_channel = 0;

		/* Disable P2P function */
		if (!rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DISABLE))
			rtw_p2p_set_role(pwdinfo, P2P_ROLE_DISABLE);
		else
			RTW_ERR("%s, disable P2P when p2p role is P2P_ROLE_DISABLE!\n", __func__);

#if 0
		rtw_hal_set_phydm_var(padapter, HAL_PHYDM_P2P_STATE, NULL, _FALSE);
#ifdef CONFIG_WFD
		if (rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_MIRACAST))
			rtw_hal_set_phydm_var(padapter, HAL_PHYDM_WIFI_DISPLAY_STATE, NULL, _FALSE);
#endif
#endif

		/* Restore to initial setting. */
		update_tx_basic_rate(padapter, padapter_link, padapter->registrypriv.wireless_mode);
	}

exit:
	return ret;
}

#endif /* CONFIG_P2P */