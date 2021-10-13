/*
 **************************************************************************
 * Copyright (c) 2013-2021, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/**
 * @file nss_api_if.h
 *	NSS driver APIs and Declarations.
 *
 * This file declares all the public interfaces for NSS driver.
 */

#ifndef __NSS_API_IF_H
#define __NSS_API_IF_H

#ifdef __KERNEL__ /* only kernel will use. */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include "nss_arch.h"
#include "nss_def.h"
#include "nss_cmn.h"
#include "nss_tun6rd.h"
#include "nss_l2tpv2.h"
#include "nss_pptp.h"
#include "nss_map_t.h"
#include "nss_tunipip6.h"
#include "nss_lag.h"
#include "nss_stats_public.h"
#include "nss_ipv4.h"
#include "nss_ipv6.h"
#include "nss_shaper.h"
#include "nss_if.h"
#include "nss_phy_if.h"
#include "nss_virt_if.h"
#include "nss_pppoe.h"
#include "nss_crypto.h"
#include "nss_crypto_cmn.h"
#include "nss_dma.h"

#include "nss_profiler.h"
#include "nss_dynamic_interface.h"
#include "nss_ipsec.h"
#include "nss_ipsec_cmn.h"
#include "nss_gre.h"
#include "nss_gre_redir.h"
#include "nss_gre_redir_lag.h"
#include "nss_gre_tunnel.h"
#include "nss_sjack.h"
#include "nss_capwap.h"
#include "nss_wifi.h"
#include "nss_wifi_vdev.h"
#include "nss_n2h.h"
#include "nss_rps.h"
#include "nss_portid.h"
#include "nss_oam.h"
#include "nss_dtls.h"
#include "nss_dtls_cmn.h"
#include "nss_tls.h"
#include "nss_edma.h"
#include "nss_bridge.h"
#include "nss_ppe.h"
#include "nss_trustsec_tx.h"
#include "nss_vlan.h"
#include "nss_igs.h"
#include "nss_mirror.h"
#include "nss_wifili_if.h"
#include "nss_project.h"
#include "nss_qrfs.h"
#include "nss_c2c_tx.h"
#include "nss_qvpn.h"
#include "nss_unaligned.h"
#include "nss_pvxlan.h"
#include "nss_vxlan.h"
#include "nss_pm.h"
#include "nss_freq.h"
#include "nss_tstamp.h"
#include "nss_gre_redir_mark.h"
#include "nss_clmap.h"
#include "nss_rmnet_rx.h"
#include "nss_match.h"
#include "nss_eth_rx.h"
#include "nss_c2c_rx.h"
#include "nss_ipv6_reasm.h"
#include "nss_ipv4_reasm.h"
#include "nss_lso_rx.h"
#include "nss_wifi_mac_db_if.h"
#include "nss_wifi_ext_vdev_if.h"
#include "nss_wifili_if.h"
#include "nss_ppe_vp.h"
#include "nss_wifi_mesh.h"
#include "nss_udp_st.h"
#endif

#endif /*__KERNEL__ */

/**
 * @addtogroup nss_driver_subsystem
 * @{
 */

#define NSS_MAX_CORES 2		/**< Maximum number of core interfaces. */

#define NSS_MAX_DEVICE_INTERFACES (NSS_MAX_PHYSICAL_INTERFACES + NSS_MAX_VIRTUAL_INTERFACES + NSS_MAX_TUNNEL_INTERFACES + NSS_MAX_DYNAMIC_INTERFACES)
		/**< Maximum number of device interfaces. */

#define NSS_MAX_NET_INTERFACES (NSS_MAX_DEVICE_INTERFACES + NSS_MAX_SPECIAL_INTERFACES)
		/**< Maximum number of network interfaces. */

#define NSS_MAX_PHYSICAL_INTERFACES 8	/**< Maximum number of physical interfaces. */
#define NSS_MAX_VIRTUAL_INTERFACES 16	/**< Maximum number of virtual interfaces. */
#define NSS_MAX_TUNNEL_INTERFACES 4	/**< Maximum number of tunnel interfaces. */
#define NSS_MAX_SPECIAL_INTERFACES 69	/**< Maximum number of special interfaces. */
#define NSS_MAX_WIFI_RADIO_INTERFACES 3	/**< Maximum number of radio interfaces. */

/*
 * Start of individual interface groups
 */
#define NSS_PHYSICAL_IF_START 0
		/**< Beginning of the physical interfaces. */

#define NSS_VIRTUAL_IF_START (NSS_PHYSICAL_IF_START + NSS_MAX_PHYSICAL_INTERFACES)
		/**< Beginning of the virtual interfaces. */

#define NSS_TUNNEL_IF_START (NSS_VIRTUAL_IF_START + NSS_MAX_VIRTUAL_INTERFACES)
		/**< Beginning of the tunnel interfaces. */

#define NSS_DYNAMIC_IF_START (NSS_TUNNEL_IF_START + NSS_MAX_TUNNEL_INTERFACES)
		/**< Beginning of the dynamic interfaces. */

#define NSS_SPECIAL_IF_START (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)
		/**< Beginning of the special interfaces. */

/*
 * Tunnel interface numbers
 */
#define NSS_IPSEC_ENCAP_IF_NUMBER (NSS_TUNNEL_IF_START + 0)
		/**< Tunnel interface number for IPsec encapsulation interfaces. */
#define NSS_IPSEC_DECAP_IF_NUMBER (NSS_TUNNEL_IF_START + 1)
		/**< Tunnel interface number for IPsec decapsulation interfaces. */
#define NSS_TUN6RD_INTERFACE (NSS_TUNNEL_IF_START + 2)
		/**< Tunnel interface number for TUN6RD interfaces. */
#define NSS_TUNIPIP6_INTERFACE (NSS_TUNNEL_IF_START + 3)
		/**< Tunnel interface number for TUNIPIP6 interfaces. */

/*
 * Special interface numbers
 */
#define NSS_N2H_INTERFACE (NSS_SPECIAL_IF_START + 0)
		/**< Special interface number for N2H. */
#define NSS_ETH_RX_INTERFACE (NSS_SPECIAL_IF_START + 2)
		/**< Special interface number for Ethernet Rx. */
#define NSS_PPPOE_INTERFACE (NSS_SPECIAL_IF_START + 3)
		/**< Special interface number for PPPoE. */
#define NSS_IPV4_RX_INTERFACE (NSS_SPECIAL_IF_START + 5)
		/**< Special interface number for IPv4. */
#define NSS_IPV6_RX_INTERFACE (NSS_SPECIAL_IF_START + 7)
		/**< Special interface number for IPv6. */
#define NSS_PROFILER_INTERFACE (NSS_SPECIAL_IF_START + 8)
		/**< Special interface number for profile. */
#define NSS_CRYPTO_INTERFACE (NSS_SPECIAL_IF_START + 9)
		/**< Special interface number for crypto CE5. */
#define NSS_DTLS_INTERFACE (NSS_SPECIAL_IF_START + 10)
		/**< Special interface number for DTLS. */
#define NSS_CRYPTO_CMN_INTERFACE (NSS_SPECIAL_IF_START + 11)
		/**< Special interface number for crypto common. */
#define NSS_C2C_TX_INTERFACE (NSS_SPECIAL_IF_START + 12)
		/**< Virtual interface number for core-to-core transmissions. */
#define NSS_C2C_RX_INTERFACE (NSS_SPECIAL_IF_START + 13)
		/**< Virtual interface number for core-to-core reception. */
#define NSS_IPSEC_CMN_INTERFACE (NSS_SPECIAL_IF_START + 18)
		/**< Virtual interface number for IPSec rule. */
#define NSS_COREFREQ_INTERFACE (NSS_SPECIAL_IF_START + 19)
		/**< Virtual interface number for core frequency. */
#define NSS_DYNAMIC_INTERFACE (NSS_SPECIAL_IF_START + 20)
		/**< Special interface number for dynamic interfaces. */
#define NSS_GRE_REDIR_INTERFACE (NSS_SPECIAL_IF_START + 21)
		/**< Special interface number for GRE redirect base interfaces. */
#define NSS_LSO_RX_INTERFACE (NSS_SPECIAL_IF_START + 22)
		/**< Special interface number for LSO. */
#define NSS_SJACK_INTERFACE (NSS_SPECIAL_IF_START + 23)
		/**< Special interface number for GRE REDIR base interfaces. */
#define NSS_IPV4_REASM_INTERFACE (NSS_SPECIAL_IF_START + 24)
		/**< Special interface number for IPv4 reassembly interfaces. */
#define NSS_DEBUG_INTERFACE (NSS_SPECIAL_IF_START + 25)
		/**< Special interface number for debug. */
#define NSS_WIFI_INTERFACE0 (NSS_SPECIAL_IF_START + 26)
		/**< Special interface number for Wi-Fi radio 0. */
#define NSS_WIFI_INTERFACE1 (NSS_SPECIAL_IF_START + 27)
		/**< Special interface number for Wi-Fi radio 1. */
#define NSS_WIFI_INTERFACE2 (NSS_SPECIAL_IF_START + 28)
		/**< Special interface number for Wi-Fi radio 2. */
#define NSS_IPV6_REASM_INTERFACE (NSS_SPECIAL_IF_START + 29)
		/**< Special interface number for IPv6 reassembly. */
#define NSS_LAG0_INTERFACE_NUM (NSS_SPECIAL_IF_START + 30)
		/**< Special interface number for LAG0. */
#define NSS_LAG1_INTERFACE_NUM (NSS_SPECIAL_IF_START + 31)
		/**< Special interface number for LAG1. */
#define NSS_LAG2_INTERFACE_NUM (NSS_SPECIAL_IF_START + 32)
		/**< Special interface number for LAG2. */
#define NSS_LAG3_INTERFACE_NUM (NSS_SPECIAL_IF_START + 33)
		/**< Special interface number for LAG3. */
#define NSS_L2TPV2_INTERFACE (NSS_SPECIAL_IF_START + 34)
		/**< Special interface number for L2TPv2 UDP encapsulation. */
#define NSS_PPTP_INTERFACE (NSS_SPECIAL_IF_START + 36)
		/**< Special interface number for PPTP-to-decapsulation. */
#define NSS_PORTID_INTERFACE (NSS_SPECIAL_IF_START + 37)
		/**< Special interface number for port ID. */
#define NSS_OAM_INTERFACE (NSS_SPECIAL_IF_START + 38)
		/**< Special interface number for OAM. */
#define NSS_MAP_T_INTERFACE (NSS_SPECIAL_IF_START + 39)
		/**< Special interface number for MAP-T. */
#define NSS_PPE_INTERFACE (NSS_SPECIAL_IF_START + 40)
		/**< Special interface number for PPE. */
#define NSS_EDMA_INTERFACE (NSS_SPECIAL_IF_START + 41)
		/**< Special interface number for EDMA. */
#define NSS_GRE_TUNNEL_INTERFACE (NSS_SPECIAL_IF_START + 42)
		/**< Special interface number for NSS GRE tunnel. */
#define NSS_TRUSTSEC_TX_INTERFACE (NSS_SPECIAL_IF_START + 43)
		/**< Special interface number for TrustSec Tx. */
#define NSS_VAP_INTERFACE (NSS_SPECIAL_IF_START + 44)
		/**< Special interface number for NSS Wi-Fi VAPs base interfaces. */
#define NSS_VLAN_INTERFACE (NSS_SPECIAL_IF_START + 45)
		/**< Special interface number for VLAN. */
#define NSS_GRE_INTERFACE (NSS_SPECIAL_IF_START + 46)
		/**< Special interface number for GRE. */
#define NSS_WIFILI_INTERNAL_INTERFACE (NSS_SPECIAL_IF_START + 47)
		/**< Special interface number for wifili internal instance. */
#define NSS_PROJECT_INTERFACE (NSS_SPECIAL_IF_START + 48)
		/**< Special interface number for project node. */
#define NSS_PBUF_MGR_FREE_INTERFACE (NSS_SPECIAL_IF_START + 49)
		/**< Special interface number for PBUF_MGR_FREE node. */
#define NSS_REDIR_RX_INTERFACE (NSS_SPECIAL_IF_START + 50)
		/**< Special interface number for 802.3 redirect node. */
#define NSS_QRFS_INTERFACE (NSS_SPECIAL_IF_START + 51)
		/**< Special interface number for QRFS. */
#define NSS_GRE_REDIR_LAG_INTERFACE (NSS_SPECIAL_IF_START + 52)
		/**< Special interface number for GRE redirect link aggregation interface. */
#define NSS_UNALIGNED_INTERFACE (NSS_SPECIAL_IF_START + 53)
		/**< Special interface number for unaligned handler. */
#define NSS_TSTAMP_TX_INTERFACE (NSS_SPECIAL_IF_START + 54)
		/**< Special interface number for timestamp transmit. */
#define NSS_TSTAMP_RX_INTERFACE (NSS_SPECIAL_IF_START + 55)
		/**< Special interface number for timestamp receive. */
#define NSS_GRE_REDIR_MARK_INTERFACE (NSS_SPECIAL_IF_START + 56)
		/**< Special interface number for GRE redirect mark. */
#define NSS_VXLAN_INTERFACE (NSS_SPECIAL_IF_START + 57)
		/**< Special interface number for VxLAN handler. */
#define NSS_RMNET_RX_INTERFACE (NSS_SPECIAL_IF_START + 58)
		/**< Special interface number for remote wireless wide area network receive handler. */
#define NSS_WIFILI_EXTERNAL_INTERFACE0 (NSS_SPECIAL_IF_START + 59)
		/**< Special interface number for first external radio instance. */
#define NSS_WIFILI_EXTERNAL_INTERFACE1 (NSS_SPECIAL_IF_START + 60)
		/**< Special interface number for second external radio instance. */
#define NSS_TLS_INTERFACE (NSS_SPECIAL_IF_START + 61)
		/**< Special interface number for TLS. */
#define NSS_PPE_VP_INTERFACE (NSS_SPECIAL_IF_START + 62)
		/**< Special interface number for the virtual port (62, 63, 64) interface. */
#define NSS_WIFI_MAC_DB_INTERFACE (NSS_SPECIAL_IF_START + 65)
		/**< Special interface number for the Wi-Fi MAC database. */
#define NSS_DMA_INTERFACE (NSS_SPECIAL_IF_START + 66)
		/**< Special interface number for the DMA interface. */
#define NSS_WIFI_EXT_VDEV_INTERFACE (NSS_SPECIAL_IF_START + 67)
		/**< Special interface number for the Wi-Fi extended virtual interface. */
#define NSS_UDP_ST_INTERFACE (NSS_SPECIAL_IF_START + 68)
		/**< Special interface number for the UDP speed test interface. */

#ifdef __KERNEL__ /* only kernel will use. */

/*
 * General utilities
 */

/**
 * General callback function for all interface messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_if_rx_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_get_state
 *	Gets the NSS state.
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * NSS state.
 */
extern nss_state_t nss_get_state(void *nss_ctx);

#endif /*__KERNEL__ */

/*
 * Once Everything is arranged correctly, will be placed at top
 */

/**
 *@}
 */

#endif /** __NSS_API_IF_H */
