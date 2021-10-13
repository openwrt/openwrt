/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include "nss_tx_rx_common.h"
#include "nss_dynamic_interface.h"

/*
 * nss_dynamic_interface_type_names
 *	Name strings for dynamic interface types
 */
const char *nss_dynamic_interface_type_names[NSS_DYNAMIC_INTERFACE_TYPE_MAX] = {
	"NSS_DYNAMIC_INTERFACE_TYPE_NONE",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR",
	"NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_5",
	"NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_RESERVED",
	"NSS_DYNAMIC_INTERFACE_TYPE_VAP",
	"NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_0",
	"NSS_DYNAMIC_INTERFACE_TYPE_PPPOE",
	"NSS_DYNAMIC_INTERFACE_TYPE_VIRTIF_DEPRECATED",
	"NSS_DYNAMIC_INTERFACE_TYPE_L2TPV2",
	"NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_4",
	"NSS_DYNAMIC_INTERFACE_TYPE_PORTID",
	"NSS_DYNAMIC_INTERFACE_TYPE_DTLS",
	"NSS_DYNAMIC_INTERFACE_TYPE_QVPN_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_QVPN_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_BRIDGE",
	"NSS_DYNAMIC_INTERFACE_TYPE_VLAN",
	"NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_3",
	"NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_INTERNAL",
	"NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_HOST_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_OFFL_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_SJACK_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GENERIC_REDIR_N2H",
	"NSS_DYNAMIC_INTERFACE_TYPE_GENERIC_REDIR_H2N",
	"NSS_DYNAMIC_INTERFACE_TYPE_TUN6RD_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_TUN6RD_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_DS",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER_EXCEPTION",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_EXCEPTION_US",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_EXCEPTION_DS",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_GRE_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_PPTP_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_PPTP_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_PPTP_HOST_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_REDIRECT",
	"NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_HOST_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_IGS",
	"NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_US",
	"NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_DS",
	"NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_OUTER",
	"NSS_DYNAMIC_INTERFACE_TYPE_MATCH",
	"NSS_DYNAMIC_INTERFACE_TYPE_RMNET_RX_N2H",
	"NSS_DYNAMIC_INTERFACE_TYPE_RMNET_RX_H2N",
	"NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_EXTERNAL0",
	"NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_EXTERNAL1",
	"NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_HOST_INNER",
	"NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_OUTER",
};

/*
 * nss_dynamic_interface_type_names_stats_read()
 *	Read and display dynamic interface types names
 */
static ssize_t nss_dynamic_interface_type_names_stats_read(struct file *fp, char __user *ubuf, size_t sz, loff_t *ppos)
{
	int i;
	char *lbuf = NULL;
	size_t size_wr = 0;
	ssize_t bytes_read = 0;
	uint32_t max_output_lines = 2 /* header & footer for stats */
				+ NSS_DYNAMIC_INTERFACE_TYPE_MAX /* maximum number of dynamic interface types */
				+ 2;
	size_t size_al = NSS_STATS_MAX_STR_LENGTH * max_output_lines;

	lbuf = kzalloc(size_al, GFP_KERNEL);
	if (unlikely(lbuf == NULL)) {
		nss_warning("Could not allocate memory for local statistics buffer");
		return 0;
	}

	/*
	 * name strings
	 */
	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n dynamic interface type names start:\n\n");

	for (i = 0; i < NSS_DYNAMIC_INTERFACE_TYPE_MAX; i++) {
		size_wr += scnprintf(lbuf + size_wr, size_al - size_wr,
				"\t%u : %s\n", i, nss_dynamic_interface_type_names[i]);
	}

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n");

	size_wr += scnprintf(lbuf + size_wr, size_al - size_wr, "\n dynamic interface type names end\n");
	bytes_read = simple_read_from_buffer(ubuf, sz, ppos, lbuf, size_wr);

	kfree(lbuf);
	return bytes_read;
}

/*
 * nss_dynamic_interface_type_names_stats_ops
 */
NSS_STATS_DECLARE_FILE_OPERATIONS(dynamic_interface_type_names)

/*
 * nss_dynamic_interface_stats_dentry_create()
 *	Create dynamic-interface statistics debug entry.
 */
void nss_dynamic_interface_stats_dentry_create(void)
{
	struct dentry *di_dentry = NULL;
	struct dentry *di_type_name_d = NULL;

	di_dentry = debugfs_create_dir("dynamic_if", nss_top_main.stats_dentry);
	if (unlikely(di_dentry == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/dynamic_if directory");
		return;
	}

	di_type_name_d = debugfs_create_file("type_names", 0400, di_dentry,
					&nss_top_main, &nss_dynamic_interface_type_names_stats_ops);
	if (unlikely(di_type_name_d == NULL)) {
		nss_warning("Failed to create qca-nss-drv/stats/dynamic_if/type_names file");
		return;
	}
}
