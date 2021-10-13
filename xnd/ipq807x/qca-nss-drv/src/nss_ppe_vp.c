/*
 **************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
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

/*
 * Header file for qca-ssdk APIs
 */
#include <ref/ref_vsi.h>

#include "nss_ppe_vp.h"
#include "nss_ppe_vp_stats.h"

#define NSS_PPE_VP_TX_TIMEOUT 1000 /* 1 Second */

static struct nss_vp_mapping *vp_map[NSS_MAX_DYNAMIC_INTERFACES] = {NULL};
unsigned char nss_ppe_vp_cmd[NSS_PPE_VP_MAX_CMD_STR] __read_mostly;

/*
 * Private data structure
 */
static struct nss_ppe_vp_pvt {
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
	nss_ppe_port_t ppe_port_num;
} ppe_vp_pvt;

DEFINE_SPINLOCK(nss_ppe_vp_stats_lock);
DEFINE_SPINLOCK(nss_ppe_vp_map_lock);

struct nss_ppe_vp_stats_debug nss_ppe_vp_debug_stats;
static struct dentry *nss_ppe_vp_dentry;

/*
 * nss_ppe_vp_get_map_index()
 *	Get the index of the NSS-VP number mapping array.
 */
static inline int32_t nss_ppe_vp_get_map_index(nss_if_num_t if_num)
{
	return (if_num - NSS_DYNAMIC_IF_START);
}

/*
 * nss_ppe_vp_verify_ifnum()
 *	Verify PPE VP interface number.
 */
static inline bool nss_ppe_vp_verify_ifnum(int if_num)
{
	return (if_num == NSS_PPE_VP_INTERFACE);
}

/*
 * nss_ppe_vp_map_dealloc()
 *	Deallocate memory for the NSS interface number and PPE VP number mapping.
 */
static inline void nss_ppe_vp_map_dealloc(struct nss_vp_mapping *map)
{
	vfree(map);
}

/*
 * nss_ppe_vp_map_alloc()
 *	Allocate memory for the NSS interface number and PPE VP number mapping.
 */
static inline struct nss_vp_mapping *nss_ppe_vp_map_alloc(void)
{
	struct nss_vp_mapping *nss_vp_info = vzalloc(sizeof(struct nss_vp_mapping));
	if (!nss_vp_info) {
		nss_warning("No memory for allocating NSS-VP mapping instance");
	}

	return nss_vp_info;
}

/*
 * nss_ppe_vp_proc_help()
 *	Print usage information for ppe_vp configure sysctl.
 */
static void nss_ppe_vp_proc_help(void)
{
	nss_info_always("== for dynamic interface types read following file ==");
	nss_info_always("/sys/kernel/debug/qca-nss-drv/stats/dynamic_if/type_names");
	nss_info_always("NSS PPE VP create: echo <interface name> <dynamic interface type> > /proc/sys/nss/ppe_vp/create");
	nss_info_always("NSS PPE VP destroy: echo <interface name> <dynamic interface type> > /proc/sys/nss/ppe_vp/destroy");
}

/*
 * nss_ppe_vp_del_map()
 *	Delete mapping between NSS interface number and VP number.
 */
static bool nss_ppe_vp_del_map(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num)
{
	int32_t idx;
	nss_ppe_port_t ppe_port_num;
	struct nss_vp_mapping *nss_vp_info;
	uint16_t vp_index;

	nss_assert((if_num >= NSS_DYNAMIC_IF_START) && (if_num < (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)));

	idx = nss_ppe_vp_get_map_index(if_num);
	if ((idx < 0) || (idx >= NSS_MAX_DYNAMIC_INTERFACES)) {
		nss_warning("%px: Invalid index. Cannot delete the PPE VP mapping. idx:%u", nss_ctx, idx);
		return false;
	}

	spin_lock_bh(&nss_ppe_vp_map_lock);
	nss_vp_info = vp_map[idx];
	if (!nss_vp_info) {
		spin_unlock_bh(&nss_ppe_vp_map_lock);
		nss_warning("%px: Could not find the vp num in the mapping. NSS if num:%u", nss_ctx, if_num);
		return false;
	}

	ppe_port_num = nss_vp_info->ppe_port_num;

	nss_ppe_vp_map_dealloc(nss_vp_info);
	vp_map[idx] = NULL;
	spin_unlock_bh(&nss_ppe_vp_map_lock);

	/*
	 * Clear the PPE VP stats once PPE VP is deleted
	 */
	vp_index = ppe_port_num - NSS_PPE_VP_START;
	spin_lock_bh(&nss_ppe_vp_stats_lock);
	memset(&nss_ppe_vp_debug_stats.vp_stats[vp_index], 0, sizeof(struct nss_ppe_vp_statistics_debug));
	spin_unlock_bh(&nss_ppe_vp_stats_lock);

	nss_info("%px: Deleted NSS interface number and PPE VP number mapping successfully: NSS if num:%u at index:%u", nss_ctx, if_num, idx);

	return true;
}

/*
 * nss_ppe_vp_add_map()
 *	Add mapping between NSS interface number and VP number.
 */
static bool nss_ppe_vp_add_map(struct nss_ctx_instance *nss_ctx ,nss_if_num_t if_num, struct nss_vp_mapping *nss_vp_info)
{
	uint32_t idx;
	nss_ppe_port_t ppe_port_num;

	nss_assert((if_num >= NSS_DYNAMIC_IF_START) && (if_num < (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)));

	if (!nss_vp_info) {
		nss_warning("%px: Received invalid argument.", nss_ctx);
		return false;
	}

	idx = nss_ppe_vp_get_map_index(if_num);
	if ((idx < 0) || (idx >= NSS_MAX_DYNAMIC_INTERFACES)) {
		nss_warning("%px: Invalid index. Cannot add the PPE VP mapping. idx:%u", nss_ctx, idx);
		return false;
	}

	spin_lock_bh(&nss_ppe_vp_map_lock);
	if (vp_map[idx]) {
		spin_unlock_bh(&nss_ppe_vp_map_lock);
		nss_warning("%px: Mapping exists already. NSS if num:%d index:%u, VP num:%u", nss_ctx, if_num, idx, vp_map[idx]->ppe_port_num);
		return false;
	}

	vp_map[idx] = nss_vp_info;
	ppe_port_num = vp_map[idx]->ppe_port_num;
	spin_unlock_bh(&nss_ppe_vp_map_lock);

	nss_info("%px: Mapping added successfully. NSS if num:%d index:%u, VP num:%u", nss_ctx, if_num, idx, ppe_port_num);

	return true;
}

/*
 * nss_ppe_vp_callback()
 *	Callback to handle the completion of NSS->HLOS messages.
 */
static void nss_ppe_vp_callback(void *app_data, struct nss_ppe_vp_msg *npvm)
{
	if (npvm->cm.response != NSS_CMN_RESPONSE_ACK) {
		nss_warning("ppe_vp error response %d", npvm->cm.response);
		ppe_vp_pvt.response = NSS_TX_FAILURE;
		complete(&ppe_vp_pvt.complete);
		return;
	}

	if (npvm->cm.type == NSS_IF_PPE_PORT_CREATE) {
		ppe_vp_pvt.ppe_port_num = npvm->msg.if_msg.ppe_port_create.ppe_port_num;
		nss_trace("PPE VP callback success VP num: %u", npvm->msg.if_msg.ppe_port_create.ppe_port_num);
	}
	ppe_vp_pvt.response = NSS_TX_SUCCESS;

	complete(&ppe_vp_pvt.complete);
}

/*
 * nss_ppe_vp_parse_vp_cmd()
 *	Parse PPE VP create and destroy message and return the NSS interface number.
 * Command usage:
 * echo <interface name> <dynamic interface type> /proc/sys/nss/ppe_vp/create>
 * echo ath0 6 > /proc/sys/nss/ppe_vp/create
 * Since ath0 has only one type i.e. ath0 is NSS_DYNAMIC_INTERFACE_TYPE_VAP, the above command can be rewritten as
 * echo ath0 > /proc/sys/nss/ppe_vp/create => Here 6 can be ignored.
 */
static nss_if_num_t nss_ppe_vp_parse_vp_cmd(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int32_t if_num;
	struct net_device *dev;
	uint32_t  dynamic_if_type = (uint32_t)NSS_DYNAMIC_INTERFACE_TYPE_NONE;
	struct nss_ctx_instance *nss_ctx = nss_ppe_vp_get_context();
	char *pos;
	char cmd_buf[NSS_PPE_VP_MAX_CMD_STR] = {0}, dev_name[NSS_PPE_VP_MAX_CMD_STR] = {0};
	size_t count = *lenp;
	int ret = proc_dostring(ctl, write, buffer, lenp, ppos);

	if (!write) {
		nss_ppe_vp_proc_help();
		return ret;
	}

	if (!nss_ctx) {
		nss_warning("%px: NSS Context not found.", nss_ctx);
		return -ENODEV;
	}

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if (count >= NSS_PPE_VP_MAX_CMD_STR) {
		nss_ppe_vp_proc_help();
		nss_warning("%px: Input string too big", nss_ctx);
		return -E2BIG;
	}

	if (copy_from_user(cmd_buf, buffer, count)) {
		nss_warning("%px: Cannot copy user's entry to kernel memory", nss_ctx);
		return -EFAULT;
	}

	if ((pos = strrchr(cmd_buf, '\n')) != NULL) {
		*pos = '\0';
	}

	if (sscanf(cmd_buf, "%s %u", dev_name, &dynamic_if_type) < 0) {
		nss_warning("%px: PPE VP command parse failed", nss_ctx);
		return -EFAULT;
	}

	dev = dev_get_by_name(&init_net, dev_name);
	if (!dev) {
		nss_warning("%px: Cannot find the net device", nss_ctx);
		return -ENODEV;
	}

	nss_info("%px: Dynamic interface type: %u", nss_ctx, dynamic_if_type);
	if ((dynamic_if_type < NSS_DYNAMIC_INTERFACE_TYPE_NONE) || (dynamic_if_type >= NSS_DYNAMIC_INTERFACE_TYPE_MAX)) {
		nss_warning("%px: Invalid dynamic interface type: %d", nss_ctx, dynamic_if_type);
		dev_put(dev);
		return -EFAULT;
	}

	if_num = nss_cmn_get_interface_number_by_dev_and_type(dev, dynamic_if_type);
	if (if_num < 0) {
		nss_warning("%px: Invalid interface number:%s", nss_ctx, dev_name);
		dev_put(dev);
		return -EFAULT;
	}

	nss_info("%px: PPE VP create/destroy for, nss_if_num:%d dev_name:%s dynamic_if_type:%u", nss_ctx, if_num, dev_name, dynamic_if_type);
	dev_put(dev);

	return if_num;
}

/*
 * nss_ppe_vp_tx_msg()
 *	Transmit a ppe_vp message to NSS FW
 */
nss_tx_status_t nss_ppe_vp_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_ppe_vp_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;
	nss_if_num_t if_num = ncm->interface;

	/*
	 * Trace messages.
	 */
	nss_ppe_vp_log_tx_msg(msg);

	/*
	 * Sanity check the message
	 */
	if (!((ncm->type == NSS_IF_PPE_PORT_CREATE) || (ncm->type == NSS_IF_PPE_PORT_DESTROY))) {
		nss_warning("%px: Invalid message type: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	if (!(if_num >= NSS_DYNAMIC_IF_START && (if_num < (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)))) {
		nss_warning("%px: invalid interface %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_ppe_vp_tx_msg_sync()
 *	Transmit a ppe_vp message to NSS firmware synchronously.
 */
nss_tx_status_t nss_ppe_vp_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_ppe_vp_msg *npvm)
{
	nss_tx_status_t status;
	int ret = 0;

	down(&ppe_vp_pvt.sem);

	status = nss_ppe_vp_tx_msg(nss_ctx, npvm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: ppe_tx_msg failed", nss_ctx);
		up(&ppe_vp_pvt.sem);
		return status;
	}

	ret = wait_for_completion_timeout(&ppe_vp_pvt.complete, msecs_to_jiffies(NSS_PPE_VP_TX_TIMEOUT));
	if (!ret) {
		nss_warning("%px: ppe_vp msg tx failed due to timeout", nss_ctx);
		ppe_vp_pvt.response = NSS_TX_FAILURE;
	}

	status = ppe_vp_pvt.response;
	up(&ppe_vp_pvt.sem);

	return status;
}

/*
 * nss_ppe_vp_get_context()
 *	Get NSS context instance for ppe_vp
 */
struct nss_ctx_instance *nss_ppe_vp_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.ppe_handler_id];
}
EXPORT_SYMBOL(nss_ppe_vp_get_context);

/*
 * nss_ppe_vp_get_ppe_port_by_nssif()
 *	Get vp number for a given NSS interface number.
 */
nss_ppe_port_t nss_ppe_vp_get_ppe_port_by_nssif(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num)
{
	uint32_t idx;
	nss_ppe_port_t ppe_port_num;

	if (!((if_num >= NSS_DYNAMIC_IF_START) && (if_num < (NSS_DYNAMIC_IF_START + NSS_MAX_DYNAMIC_INTERFACES)))) {
		nss_warning("%px: NSS invalid nss if num: %u", nss_ctx, if_num);
		return -1;
	}

	idx = nss_ppe_vp_get_map_index(if_num);
	if (idx < 0 || idx >= NSS_MAX_DYNAMIC_INTERFACES) {
		nss_warning("%px: NSS invalid index: %d nss if num: %u",nss_ctx, idx, if_num);
		return -1;
	}

	spin_lock_bh(&nss_ppe_vp_map_lock);
	if (!vp_map[idx]) {
		spin_unlock_bh(&nss_ppe_vp_map_lock);
		nss_warning("%px: NSS interface and VP mapping is not present for nss if num: %u",nss_ctx, if_num);
		return -1;
	}
	ppe_port_num = vp_map[idx]->ppe_port_num;
	spin_unlock_bh(&nss_ppe_vp_map_lock);

	nss_info("%px: VP num %d nss_if: %d",nss_ctx, ppe_port_num, if_num);

	return ppe_port_num;
}
EXPORT_SYMBOL(nss_ppe_vp_get_ppe_port_by_nssif);

/*
 * nss_ppe_vp_destroy()
 *	Destroy PPE virtual port for the given nss interface number.
 */
nss_tx_status_t nss_ppe_vp_destroy(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num)
{
	nss_tx_status_t status;
	struct nss_ppe_vp_msg *npvm;
	uint32_t idx;
	int32_t vsi_id_valid = false;
	int32_t vsi_id;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	idx = nss_ppe_vp_get_map_index(if_num);
	if (idx < 0 || idx >= NSS_MAX_DYNAMIC_INTERFACES) {
		nss_warning("%px: Cannot destroy PPE VP. Invalid index: %d. nss_if_num: %u", nss_ctx, idx, if_num);
		return -1;
	}

	spin_lock_bh(&nss_ppe_vp_map_lock);
	if (vp_map[idx]) {
		vsi_id = vp_map[idx]->vsi_id;
		vsi_id_valid = vp_map[idx]->vsi_id_valid;
	}
	spin_unlock_bh(&nss_ppe_vp_map_lock);

	if (vsi_id_valid) {
		/*
		 * Send the dettach VSI message to the Firmware.
		 */
		if (nss_if_vsi_unassign(nss_ctx, if_num, vsi_id)) {
			nss_warning("%px: PPE VP destroy failed. Failed to detach VSI to PPE VP interface %d vsi:%d", nss_ctx, if_num, vsi_id);
			return NSS_TX_FAILURE;
		}

		if (ppe_vsi_free(NSS_PPE_VP_SWITCH_ID, vsi_id)) {
			nss_warning("%px: PPE VP destroy failed. Failed to free PPE VSI. nss_if:%d vsi:%d", nss_ctx, if_num, vsi_id);
			return NSS_TX_FAILURE;
		}

		nss_info("%px: PPE VP VSI detached successfully. VSI ID freed successfully. NSS if num:%u, VSI ID:%u", nss_ctx, if_num, vsi_id);
	}

	npvm = kzalloc(sizeof(struct nss_ppe_vp_msg), GFP_KERNEL);
	if (!npvm) {
		nss_warning("%px: Unable to allocate memeory of PPE VP message", nss_ctx);
		return NSS_TX_FAILURE;
	}

	nss_trace("%px: PPE_VP will be destroyed for an interface: %d", nss_ctx, if_num);

	/*
	 * Destroy PPE VP for a dynamic interface.
	 */
	nss_cmn_msg_init(&npvm->cm, if_num, NSS_IF_PPE_PORT_DESTROY, 0, nss_ppe_vp_callback, NULL);

	status = nss_ppe_vp_tx_msg_sync(nss_ctx, npvm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Unable to send PPE VP destroy message", nss_ctx);
		kfree(npvm);
		return NSS_TX_FAILURE;
	}

	kfree(npvm);

	/*
	 * Delete mapping between the NSS interface number and the VP number.
	 */
	if (!nss_ppe_vp_del_map(nss_ctx, if_num)) {
		nss_warning("%px: Failed to delete the mapping for nss_if:%d", nss_ctx, if_num);
		return NSS_TX_FAILURE;
	}

	return status;
}
EXPORT_SYMBOL(nss_ppe_vp_destroy);

/*
 * nss_ppe_vp_create()
 *	Create PPE virtual port for the given nss interface number.
 */
nss_tx_status_t nss_ppe_vp_create(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num)
{
	uint32_t vsi_id;
	nss_tx_status_t status;
	struct nss_ppe_vp_msg *npvm;
	struct nss_vp_mapping *nss_vp_info;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	/*
	 * TODO: No need to create VSI for tunnel interfaces. Only for VAP interfaces VSI is needed.
	 * Allocate the VSI for the dynamic interface on which VP will be created.
	 */
	if (ppe_vsi_alloc(NSS_PPE_VP_SWITCH_ID, &vsi_id)) {
		nss_warning("%px, Failed to alloc VSI ID, PPE VP create failed. nss_if:%u", nss_ctx, if_num);
		return NSS_TX_FAILURE;
	}

	npvm = kzalloc(sizeof(struct nss_ppe_vp_msg), GFP_KERNEL);
	if (!npvm) {
		nss_warning("%px: Unable to allocate memeory of PPE VP message", nss_ctx);
		goto free_vsi;
	}

	nss_trace("%px: PPE_VP will be created for an interface: %d", nss_ctx, if_num);

	/*
	 * Create PPE VP for a dynamic interface.
	 */
	nss_cmn_msg_init(&npvm->cm, if_num, NSS_IF_PPE_PORT_CREATE,
			sizeof(struct nss_if_ppe_port_create), nss_ppe_vp_callback, NULL);

	status = nss_ppe_vp_tx_msg_sync(nss_ctx, npvm);
	if (status != NSS_TX_SUCCESS) {
		nss_warning("%px: Unable to send ppe_vp create message", nss_ctx);
		goto free_alloc;
	}

	/*
	 * Send the attach VSI message to the Firmware.
	 */
	if (nss_if_vsi_assign(nss_ctx, if_num, vsi_id) != NSS_TX_SUCCESS) {
		nss_warning("%px: Failed to attach VSI to PPE VP interface. nss_if:%u vsi:%u", nss_ctx, if_num, vsi_id);
		goto destroy_vp;
	}

	nss_vp_info = nss_ppe_vp_map_alloc();
	if (!nss_vp_info) {
		nss_warning("%px: No memory for allocating NSS-VP mapping instance", nss_ctx);
		goto detach_vsi;
	}

	nss_vp_info->vsi_id = vsi_id;
	nss_vp_info->vsi_id_valid = true;
	nss_vp_info->if_num = if_num;
	nss_vp_info->ppe_port_num = ppe_vp_pvt.ppe_port_num;

	nss_info("%px: PPE VP allocated VSI ID:%u NSS interface number:%u VP no from Firmware:%u", nss_ctx, vsi_id, if_num, nss_vp_info->ppe_port_num);

	/*
	 * Add mapping between the NSS interface number and the VP number.
	 */
	if (!nss_ppe_vp_add_map(nss_ctx, if_num, nss_vp_info)) {
		nss_warning("%px: Failed to add mapping for NSS interface number: %d", nss_ctx, if_num);
		goto free_nss_vp_info;
	}

	kfree(npvm);

	return status;

free_nss_vp_info:
	nss_ppe_vp_map_dealloc(nss_vp_info);

detach_vsi:
	nss_trace("%px: Detaching VSI ID :%u NSS Interface no:%u", nss_ctx, vsi_id, if_num);
	if (nss_if_vsi_unassign(nss_ctx, if_num, vsi_id)) {
		nss_warning("%px: Failed to free PPE VP VSI. nss_if:%u vsi:%u", nss_ctx, if_num, vsi_id);
	}

destroy_vp:
	nss_trace("%px: Destroy Vp for NSS Interface num:%u VP num:%u", nss_ctx, if_num, npvm->msg.if_msg.ppe_port_create.ppe_port_num);
	if (nss_ppe_vp_destroy(nss_ctx, if_num)) {
		nss_warning("%px: PPE VP destroy failed, nss_if:%u", nss_ctx, if_num);
	}

free_alloc:
	kfree(npvm);

free_vsi:
	nss_trace("%px: Free VSI ID :%u NSS Interface no:%u", nss_ctx, vsi_id, if_num);
	if (ppe_vsi_free(NSS_PPE_VP_SWITCH_ID, vsi_id)) {
		nss_warning("%px: Failed to free PPE VP VSI. NSS if num:%u vsi:%u", nss_ctx, if_num, vsi_id);
	}

	return NSS_TX_FAILURE;
}
EXPORT_SYMBOL(nss_ppe_vp_create);

/*
 * nss_ppe_vp_destroy_notify()
 *	Get PPE VP destroy notification from NSS
 */
static void nss_ppe_vp_destroy_notify(struct nss_ctx_instance *nss_ctx, struct nss_ppe_vp_destroy_notify_msg *destroy_notify)
{
	nss_if_num_t nss_if_num;
	uint32_t i;
	int32_t vsi_id;
	bool vsi_id_valid = false;
	nss_ppe_port_t ppe_port_num = destroy_notify->ppe_port_num;

	/*
	 * Find NSS interface number corresponding to the VP num.
	 */
	spin_lock_bh(&nss_ppe_vp_map_lock);
	for (i = 0; i < NSS_MAX_DYNAMIC_INTERFACES; i++) {
		if (vp_map[i] && (ppe_port_num == vp_map[i]->ppe_port_num)) {
			nss_if_num = vp_map[i]->if_num;
			vsi_id = vp_map[i]->vsi_id;
			vsi_id_valid = vp_map[i]->vsi_id_valid;
			break;
		}
	}
	spin_unlock_bh(&nss_ppe_vp_map_lock);

	if (i == NSS_MAX_DYNAMIC_INTERFACES) {
		nss_warning("%px: Could not find the NSS interface number mapping for VP number: %u\n", nss_ctx, ppe_port_num);
		return;
	}

	/*
	 * Delete the nss_if_num to VP num mapping and reset the stats entry for this VP.
	 */
	if (!nss_ppe_vp_del_map(nss_ctx, nss_if_num)) {
		nss_warning("%px: Failed to delete the mapping for nss_if: %d\n", nss_ctx, nss_if_num);
		return;
	}

	if (vsi_id_valid && ppe_vsi_free(NSS_PPE_VP_SWITCH_ID, vsi_id)) {
		nss_warning("%px: Failed to free PPE VSI. nss_if: %d vsi: %d\n", nss_ctx, nss_if_num, vsi_id);
	}
}

/*
 * nss_ppe_vp_handler()
 *	Handle NSS -> HLOS messages for ppe
 */
static void nss_ppe_vp_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_ppe_vp_msg *msg = (struct nss_ppe_vp_msg *)ncm;
	nss_ppe_vp_msg_callback_t cb;
	void *ctx;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	nss_trace("%px ppe_vp msg: %px\n", nss_ctx, msg);
	BUG_ON(!nss_ppe_vp_verify_ifnum(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_PPE_VP_MSG_MAX) {
		nss_warning("%px: received invalid message %d for PPE_VP interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_ppe_vp_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Trace messages.
	 */
	nss_ppe_vp_log_rx_msg(msg);

	switch (msg->cm.type) {
	case NSS_PPE_VP_MSG_SYNC_STATS:
		/*
		 * Per VP stats msg
		 */
		nss_ppe_vp_stats_sync(nss_ctx, &msg->msg.stats, ncm->interface);
		break;

	case NSS_PPE_VP_MSG_DESTROY_NOTIFY:
		/*
		 * VP destroy notification
		 */
		nss_ppe_vp_destroy_notify(nss_ctx, &msg->msg.destroy_notify);
		break;
	}

	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_core_get_msg_handler(nss_ctx, ncm->interface);
		ncm->app_data = (nss_ptr_t)nss_ctx->nss_rx_interface_handlers[ncm->interface].app_data;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * Callback
	 */
	cb = (nss_ppe_vp_msg_callback_t)ncm->cb;
	ctx = (void *)ncm->app_data;

	cb(ctx, msg);
}

/*
 * nss_ppe_vp_destroy_handler()
 *	PPE VP destroy handler.
 */
static int nss_ppe_vp_destroy_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_vp_get_context();
	int32_t if_num;
	nss_tx_status_t nss_tx_status;

	if (!nss_ctx) {
		nss_warning("%px: NSS Context not found.", nss_ctx);
		return -ENODEV;
	}

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if_num = nss_ppe_vp_parse_vp_cmd(ctl, write, buffer, lenp, ppos);
	if (if_num < 0) {
		nss_warning("%px: Invalid interface number: %d", nss_ctx, if_num);
		return -EFAULT;
	}

	if (nss_ppe_vp_get_ppe_port_by_nssif(nss_ctx, if_num) < 0) {
		nss_warning("%px: VP is not present for interface: %d", nss_ctx, if_num);
		return -EEXIST;
	}

	nss_tx_status = nss_ppe_vp_destroy(nss_ctx, if_num);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: Sending message failed, cannot destroy PPE_VP node nss_if: %u", nss_ctx, if_num);
		return -EBUSY;
	}

	return 0;
}

/*
 * nss_ppe_vp_create_handler()
 *	PPE VP create handler.
 */
static int nss_ppe_vp_create_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int32_t if_num;
	struct nss_ctx_instance *nss_ctx = nss_ppe_vp_get_context();
	nss_tx_status_t nss_tx_status;

	if (!nss_ctx) {
		nss_warning("%px: NSS Context not found.", nss_ctx);
		return -ENODEV;
	}

	NSS_VERIFY_CTX_MAGIC(nss_ctx);

	if_num = nss_ppe_vp_parse_vp_cmd(ctl, write, buffer, lenp, ppos);
	if (if_num < 0) {
		nss_warning("%px: Invalid interface number: %d", nss_ctx, if_num);
		return -EFAULT;
	}

	nss_info("%px: NSS interface number: %d", nss_ctx, if_num);

	if (nss_ppe_vp_get_ppe_port_by_nssif(nss_ctx, if_num) > 0) {
		nss_warning("%px: VP is already present for nss_if_num: %d", nss_ctx, if_num);
		return -EEXIST;
	}

	nss_tx_status = nss_ppe_vp_create(nss_ctx, if_num);
	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: Sending message failed, cannot create PPE VP node for nss_if_num: %u", nss_ctx, if_num);
		return -EBUSY;
	}

	return 0;
}

static struct ctl_table nss_ppe_vp_table[] = {
	{
		.procname       = "create",
		.data           = &nss_ppe_vp_cmd,
		.maxlen         = sizeof(nss_ppe_vp_cmd),
		.mode           = 0644,
		.proc_handler   = &nss_ppe_vp_create_handler,
	},
	{
		.procname       = "destroy",
		.data           = &nss_ppe_vp_cmd,
		.maxlen         = sizeof(nss_ppe_vp_cmd),
		.mode           = 0644,
		.proc_handler   = &nss_ppe_vp_destroy_handler,
	},
	{ }
};

static struct ctl_table nss_ppe_vp_dir[] = {
	{
		.procname	= "ppe_vp",
		.mode		= 0555,
		.child		= nss_ppe_vp_table,
	},
	{ }
};

static struct ctl_table nss_ppe_vp_root_dir[] = {
	{
		.procname	= "nss",
		.mode		= 0555,
		.child		= nss_ppe_vp_dir,
	},
	{ }
};

static struct ctl_table_header *nss_ppe_vp_procfs_header;

/*
 * nss_ppe_vp_procfs_register()
 *	Register sysctl specific to ppe_vp
 */
void nss_ppe_vp_procfs_register(void)
{
	/*
	 * Register sysctl table.
	 */
	nss_ppe_vp_procfs_header = register_sysctl_table(nss_ppe_vp_root_dir);
}

/*
 * uss_ppe_vp_procfs_unregister()
 *	Unregister sysctl specific for ppe_vp
 */
void nss_ppe_vp_procfs_unregister(void)
{
	/*
	 * Unregister sysctl table.
	 */
	if (nss_ppe_vp_procfs_header) {
		unregister_sysctl_table(nss_ppe_vp_procfs_header);
	}
}

/*
 * nss_ppe_vp_register_handler()
 *
 */
void nss_ppe_vp_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_vp_get_context();

	nss_ppe_vp_dentry = nss_ppe_vp_stats_dentry_create();
	if (nss_ppe_vp_dentry == NULL) {
		nss_warning("%px: Not able to create debugfs entry", nss_ctx);
		return;
	}

	nss_core_register_handler(nss_ctx, NSS_PPE_VP_INTERFACE, nss_ppe_vp_handler, NULL);
	nss_ppe_vp_procfs_register();

	sema_init(&ppe_vp_pvt.sem, 1);
	init_completion(&ppe_vp_pvt.complete);
}

/*
 * nss_ppe_vp_unregister_handler()
 *
 */
void nss_ppe_vp_unregister_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_ppe_vp_get_context();

	debugfs_remove_recursive(nss_ppe_vp_dentry);
	nss_ppe_vp_procfs_unregister();
	nss_core_unregister_handler(nss_ctx, NSS_PPE_VP_INTERFACE);
}
