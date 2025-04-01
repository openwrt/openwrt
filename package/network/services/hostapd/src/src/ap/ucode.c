#include <sys/un.h>

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/ucode.h"
#include "sta_info.h"
#include "beacon.h"
#include "hw_features.h"
#include "ap_drv_ops.h"
#include "dfs.h"
#include "acs.h"
#include "ieee802_11_auth.h"
#include <libubox/uloop.h>

static uc_resource_type_t *global_type, *bss_type, *iface_type;
static struct hapd_interfaces *interfaces;
static uc_value_t *global, *bss_registry, *iface_registry;
static uc_vm_t *vm;

static uc_value_t *
hostapd_ucode_bss_get_uval(struct hostapd_data *hapd)
{
	uc_value_t *val;

	if (hapd->ucode.idx)
		return ucv_get(wpa_ucode_registry_get(bss_registry, hapd->ucode.idx));

	val = uc_resource_new(bss_type, hapd);
	hapd->ucode.idx = wpa_ucode_registry_add(bss_registry, val);

	return val;
}

static uc_value_t *
hostapd_ucode_iface_get_uval(struct hostapd_iface *hapd)
{
	uc_value_t *val;

	if (hapd->ucode.idx)
		return ucv_get(wpa_ucode_registry_get(iface_registry, hapd->ucode.idx));

	val = uc_resource_new(iface_type, hapd);
	hapd->ucode.idx = wpa_ucode_registry_add(iface_registry, val);

	return val;
}

static void
hostapd_ucode_update_bss_list(struct hostapd_iface *iface, uc_value_t *if_bss, uc_value_t *bss)
{
	uc_value_t *list;
	int i;

	list = ucv_array_new(vm);
	for (i = 0; iface->bss && i < iface->num_bss; i++) {
		struct hostapd_data *hapd = iface->bss[i];

		ucv_array_set(list, i, ucv_string_new(hapd->conf->iface));
		ucv_object_add(bss, hapd->conf->iface, hostapd_ucode_bss_get_uval(hapd));
	}
	ucv_object_add(if_bss, iface->phy, list);
}

static void
hostapd_ucode_update_interfaces(void)
{
	uc_value_t *ifs = ucv_object_new(vm);
	uc_value_t *if_bss = ucv_array_new(vm);
	uc_value_t *bss = ucv_object_new(vm);
	int i;

	for (i = 0; i < interfaces->count; i++) {
		struct hostapd_iface *iface = interfaces->iface[i];

		ucv_object_add(ifs, iface->phy, hostapd_ucode_iface_get_uval(iface));
		hostapd_ucode_update_bss_list(iface, if_bss, bss);
	}

	ucv_object_add(ucv_prototype_get(global), "interfaces", ifs);
	ucv_object_add(ucv_prototype_get(global), "interface_bss", if_bss);
	ucv_object_add(ucv_prototype_get(global), "bss", bss);

	ucv_gc(vm);
}

static uc_value_t *
uc_hostapd_add_iface(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *iface = uc_fn_arg(0);
	char *data;
	int ret;

	if (ucv_type(iface) != UC_STRING)
		return ucv_int64_new(-1);

	data = strdup(ucv_string_get(iface));
	ret = hostapd_add_iface(interfaces, data);
	free(data);

	hostapd_ucode_update_interfaces();

	return ucv_int64_new(ret);
}

static uc_value_t *
uc_hostapd_remove_iface(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *iface = uc_fn_arg(0);

	if (ucv_type(iface) != UC_STRING)
		return NULL;

	hostapd_remove_iface(interfaces, ucv_string_get(iface));
	hostapd_ucode_update_interfaces();

	return NULL;
}

static struct hostapd_vlan *
bss_conf_find_vlan(struct hostapd_bss_config *bss, int id)
{
	struct hostapd_vlan *vlan;

	for (vlan = bss->vlan; vlan; vlan = vlan->next)
		if (vlan->vlan_id == id)
			return vlan;

	return NULL;
}

static int
bss_conf_rename_vlan(struct hostapd_data *hapd, struct hostapd_vlan *vlan,
		     const char *ifname)
{
	if (!strcmp(ifname, vlan->ifname))
		return 0;

	hostapd_drv_if_rename(hapd, WPA_IF_AP_VLAN, vlan->ifname, ifname);
	os_strlcpy(vlan->ifname, ifname, sizeof(vlan->ifname));

	return 0;
}

static int
bss_reload_vlans(struct hostapd_data *hapd, struct hostapd_bss_config *bss)
{
	struct hostapd_bss_config *old_bss = hapd->conf;
	struct hostapd_vlan *vlan, *vlan_new, *wildcard;
	char ifname[IFNAMSIZ + 1], vlan_ifname[IFNAMSIZ + 1], *pos;
	int ret;

	vlan = bss_conf_find_vlan(old_bss, VLAN_ID_WILDCARD);
	wildcard = bss_conf_find_vlan(bss, VLAN_ID_WILDCARD);
	if (!!vlan != !!wildcard)
		return -1;

	if (vlan && wildcard && strcmp(vlan->ifname, wildcard->ifname) != 0)
		strcpy(vlan->ifname, wildcard->ifname);
	else
		wildcard = NULL;

	for (vlan = bss->vlan; vlan; vlan = vlan->next) {
		if (vlan->vlan_id == VLAN_ID_WILDCARD ||
		    vlan->dynamic_vlan > 0)
			continue;

		if (!bss_conf_find_vlan(old_bss, vlan->vlan_id))
			return -1;
	}

	for (vlan = old_bss->vlan; vlan; vlan = vlan->next) {
		if (vlan->vlan_id == VLAN_ID_WILDCARD)
			continue;

		if (vlan->dynamic_vlan == 0) {
			vlan_new = bss_conf_find_vlan(bss, vlan->vlan_id);
			if (!vlan_new)
				return -1;

			if (bss_conf_rename_vlan(hapd, vlan, vlan_new->ifname))
				return -1;

			continue;
		}

		if (!wildcard)
			continue;

		os_strlcpy(ifname, wildcard->ifname, sizeof(ifname));
		pos = os_strchr(ifname, '#');
		if (!pos)
			return -1;

		*pos++ = '\0';
		ret = os_snprintf(vlan_ifname, sizeof(vlan_ifname), "%s%d%s",
				  ifname, vlan->vlan_id, pos);
	        if (os_snprintf_error(sizeof(vlan_ifname), ret))
			return -1;

		if (bss_conf_rename_vlan(hapd, vlan, vlan_ifname))
			return -1;
	}

	return 0;
}

static uc_value_t *
uc_hostapd_bss_set_config(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_data *hapd = uc_fn_thisval("hostapd.bss");
	struct hostapd_bss_config *old_bss;
	struct hostapd_iface *iface;
	struct hostapd_config *conf;
	uc_value_t *file = uc_fn_arg(0);
	uc_value_t *index = uc_fn_arg(1);
	uc_value_t *files_only = uc_fn_arg(2);
	unsigned int i, idx = 0;
	int ret = -1;

	if (!hapd || ucv_type(file) != UC_STRING)
		goto out;

	if (ucv_type(index) == UC_INTEGER)
		idx = ucv_int64_get(index);

	iface = hapd->iface;
	conf = interfaces->config_read_cb(ucv_string_get(file));
	if (!conf)
		goto out;

	if (idx > conf->num_bss || !conf->bss[idx])
		goto free;

	if (ucv_boolean_get(files_only)) {
		struct hostapd_bss_config *bss = conf->bss[idx];
		struct hostapd_bss_config *old_bss = hapd->conf;

#define swap_field(name)				\
	do {								\
		void *ptr = old_bss->name;		\
		old_bss->name = bss->name;		\
		bss->name = ptr;				\
	} while (0)

		swap_field(ssid.wpa_psk_file);
		ret = bss_reload_vlans(hapd, bss);
		goto done;
	}

	hostapd_bss_deinit_no_free(hapd);
	hostapd_drv_stop_ap(hapd);
	hostapd_free_hapd_data(hapd);

	old_bss = hapd->conf;
	for (i = 0; i < iface->conf->num_bss; i++)
		if (iface->conf->bss[i] == hapd->conf)
			iface->conf->bss[i] = conf->bss[idx];
	hapd->conf = conf->bss[idx];
	conf->bss[idx] = old_bss;

	hostapd_setup_bss(hapd, hapd == iface->bss[0], true);
	hostapd_ucode_update_interfaces();
	hostapd_owe_update_trans(iface);

done:
	ret = 0;
free:
	hostapd_config_free(conf);
out:
	return ucv_int64_new(ret);
}

static void
hostapd_remove_iface_bss_conf(struct hostapd_config *iconf,
			      struct hostapd_bss_config *conf)
{
	int i;

	for (i = 0; i < iconf->num_bss; i++)
		if (iconf->bss[i] == conf)
			break;

	if (i == iconf->num_bss)
		return;

	for (i++; i < iconf->num_bss; i++)
		iconf->bss[i - 1] = iconf->bss[i];
	iconf->num_bss--;
}


static uc_value_t *
uc_hostapd_bss_delete(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_data *hapd = uc_fn_thisval("hostapd.bss");
	struct hostapd_iface *iface;
	int i, idx;

	if (!hapd)
		return NULL;

	iface = hapd->iface;
	if (iface->num_bss == 1) {
		wpa_printf(MSG_ERROR, "trying to delete last bss of an iface: %s\n", hapd->conf->iface);
		return NULL;
	}

	for (idx = 0; idx < iface->num_bss; idx++)
		if (iface->bss[idx] == hapd)
			break;

	if (idx == iface->num_bss)
		return NULL;

	for (i = idx + 1; i < iface->num_bss; i++)
		iface->bss[i - 1] = iface->bss[i];

	iface->num_bss--;

	iface->bss[0]->interface_added = 0;
	hostapd_drv_set_first_bss(iface->bss[0]);
	hapd->interface_added = 1;

	hostapd_drv_stop_ap(hapd);
	hostapd_bss_deinit(hapd);
	hostapd_remove_iface_bss_conf(iface->conf, hapd->conf);
	hostapd_config_free_bss(hapd->conf);
	os_free(hapd);

	hostapd_ucode_update_interfaces();
	ucv_gc(vm);

	return NULL;
}

static uc_value_t *
uc_hostapd_iface_add_bss(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_iface *iface = uc_fn_thisval("hostapd.iface");
	struct hostapd_bss_config *bss;
	struct hostapd_config *conf;
	struct hostapd_data *hapd;
	uc_value_t *file = uc_fn_arg(0);
	uc_value_t *index = uc_fn_arg(1);
	unsigned int idx = 0;
	uc_value_t *ret = NULL;

	if (!iface || ucv_type(file) != UC_STRING)
		goto out;

	if (ucv_type(index) == UC_INTEGER)
		idx = ucv_int64_get(index);

	conf = interfaces->config_read_cb(ucv_string_get(file));
	if (!conf || idx > conf->num_bss || !conf->bss[idx])
		goto out;

	bss = conf->bss[idx];
	hapd = hostapd_alloc_bss_data(iface, iface->conf, bss);
	if (!hapd)
		goto out;

	hapd->driver = iface->bss[0]->driver;
	hapd->drv_priv = iface->bss[0]->drv_priv;
#ifdef CONFIG_IEEE80211BE
	os_strlcpy(hapd->ctrl_sock_iface, hapd->conf->iface,
		   sizeof(hapd->ctrl_sock_iface));
#endif
	if (interfaces->ctrl_iface_init &&
	    interfaces->ctrl_iface_init(hapd) < 0)
		goto free_hapd;

	if (iface->state == HAPD_IFACE_ENABLED &&
	    hostapd_setup_bss(hapd, -1, true))
		goto deinit_ctrl;

	iface->bss = os_realloc_array(iface->bss, iface->num_bss + 1,
				      sizeof(*iface->bss));
	iface->bss[iface->num_bss++] = hapd;

	iface->conf->bss = os_realloc_array(iface->conf->bss,
					    iface->conf->num_bss + 1,
					    sizeof(*iface->conf->bss));
	iface->conf->bss[iface->conf->num_bss] = bss;
	conf->bss[idx] = NULL;
	ret = hostapd_ucode_bss_get_uval(hapd);
	hostapd_ucode_update_interfaces();
	hostapd_owe_update_trans(iface);
	goto out;

deinit_ctrl:
	if (interfaces->ctrl_iface_deinit)
		interfaces->ctrl_iface_deinit(hapd);
free_hapd:
	hostapd_free_hapd_data(hapd);
	os_free(hapd);
out:
	hostapd_config_free(conf);
	return ret;
}

static uc_value_t *
uc_hostapd_iface_set_bss_order(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_iface *iface = uc_fn_thisval("hostapd.iface");
	uc_value_t *bss_list = uc_fn_arg(0);
	struct hostapd_data **new_bss;
	struct hostapd_bss_config **new_conf;

	if (!iface)
		return NULL;

	if (ucv_type(bss_list) != UC_ARRAY ||
	    ucv_array_length(bss_list) != iface->num_bss)
		return NULL;

	new_bss = calloc(iface->num_bss, sizeof(*new_bss));
	new_conf = calloc(iface->num_bss, sizeof(*new_conf));
	for (size_t i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss;

		bss = ucv_resource_data(ucv_array_get(bss_list, i), "hostapd.bss");
		if (bss->iface != iface)
			goto free;

		for (size_t k = 0; k < i; k++)
			if (new_bss[k] == bss)
				goto free;

		new_bss[i] = bss;
		new_conf[i] = bss->conf;
	}

	new_bss[0]->interface_added = 0;
	for (size_t i = 1; i < iface->num_bss; i++)
		new_bss[i]->interface_added = 1;

	free(iface->bss);
	iface->bss = new_bss;

	free(iface->conf->bss);
	iface->conf->bss = new_conf;
	iface->conf->num_bss = iface->num_bss;
	hostapd_drv_set_first_bss(iface->bss[0]);

	return ucv_boolean_new(true);

free:
	free(new_bss);
	free(new_conf);
	return NULL;
}

static uc_value_t *
uc_hostapd_bss_ctrl(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_data *hapd = uc_fn_thisval("hostapd.bss");
	uc_value_t *arg = uc_fn_arg(0);
	struct sockaddr_storage from = {};
	static char reply[4096];
	int reply_len;

	if (!hapd || !interfaces->ctrl_iface_recv ||
	    ucv_type(arg) != UC_STRING)
		return NULL;

	reply_len = interfaces->ctrl_iface_recv(hapd, ucv_string_get(arg),
						reply, sizeof(reply),
						&from, sizeof(from));
	if (reply_len < 0)
		return NULL;

	if (reply_len && reply[reply_len - 1] == '\n')
		reply_len--;

	return ucv_string_new_length(reply, reply_len);
}

static void
uc_hostapd_disable_iface(struct hostapd_iface *iface)
{
	switch (iface->state) {
	case HAPD_IFACE_DISABLED:
		break;
#ifdef CONFIG_ACS
	case HAPD_IFACE_ACS:
		acs_cleanup(iface);
		iface->scan_cb = NULL;
		/* fallthrough */
#endif
	default:
		hostapd_disable_iface(iface);
		break;
	}
}

static uc_value_t *
uc_hostapd_iface_stop(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_iface *iface = uc_fn_thisval("hostapd.iface");
	int i;

	if (!iface)
		return NULL;

	if (iface->state != HAPD_IFACE_ENABLED)
		uc_hostapd_disable_iface(iface);

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *hapd = iface->bss[i];

		hostapd_drv_stop_ap(hapd);
		hapd->beacon_set_done = 0;
	}

	return NULL;
}

static uc_value_t *
uc_hostapd_iface_start(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_iface *iface = uc_fn_thisval("hostapd.iface");
	uc_value_t *info = uc_fn_arg(0);
	struct hostapd_config *conf;
	bool changed = false;
	uint64_t intval;
	int i;

	if (!iface)
		return NULL;

	if (!info) {
		iface->freq = 0;
		goto out;
	}

	if (ucv_type(info) != UC_OBJECT)
		return NULL;

#define UPDATE_VAL(field, name)							\
	do {									\
		intval = ucv_int64_get(ucv_object_get(info, name, NULL));	\
		if (!errno && intval != conf->field) {				\
			conf->field = intval;					\
			changed = true;						\
		}								\
	} while(0)

	conf = iface->conf;
	UPDATE_VAL(op_class, "op_class");
	UPDATE_VAL(hw_mode, "hw_mode");
	UPDATE_VAL(channel, "channel");
	UPDATE_VAL(secondary_channel, "sec_channel");
	if (!changed &&
	    (iface->bss[0]->beacon_set_done ||
	     iface->state == HAPD_IFACE_DFS))
		return ucv_boolean_new(true);

	intval = ucv_int64_get(ucv_object_get(info, "center_seg0_idx", NULL));
	if (!errno)
		hostapd_set_oper_centr_freq_seg0_idx(conf, intval);

	intval = ucv_int64_get(ucv_object_get(info, "center_seg1_idx", NULL));
	if (!errno)
		hostapd_set_oper_centr_freq_seg1_idx(conf, intval);

	intval = ucv_int64_get(ucv_object_get(info, "oper_chwidth", NULL));
	if (!errno)
		hostapd_set_oper_chwidth(conf, intval);

	intval = ucv_int64_get(ucv_object_get(info, "frequency", NULL));
	if (!errno)
		iface->freq = intval;
	else
		iface->freq = 0;
	conf->acs = 0;

out:
	switch (iface->state) {
	case HAPD_IFACE_ENABLED:
		if (!hostapd_is_dfs_required(iface) ||
			hostapd_is_dfs_chan_available(iface))
			break;
		wpa_printf(MSG_INFO, "DFS CAC required on new channel, restart interface");
		/* fallthrough */
	default:
		uc_hostapd_disable_iface(iface);
		break;
	}

	if (conf->channel && !iface->freq)
		iface->freq = hostapd_hw_get_freq(iface->bss[0], conf->channel);

	if (iface->state != HAPD_IFACE_ENABLED) {
		hostapd_enable_iface(iface);
		return ucv_boolean_new(true);
	}

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *hapd = iface->bss[i];
		int ret;

		hapd->conf->start_disabled = 0;
		hostapd_set_freq(hapd, conf->hw_mode, iface->freq,
				 conf->channel,
				 conf->enable_edmg,
				 conf->edmg_channel,
				 conf->ieee80211n,
				 conf->ieee80211ac,
				 conf->ieee80211ax,
				 conf->ieee80211be,
				 conf->secondary_channel,
				 hostapd_get_oper_chwidth(conf),
				 hostapd_get_oper_centr_freq_seg0_idx(conf),
				 hostapd_get_oper_centr_freq_seg1_idx(conf));

		ieee802_11_set_beacon(hapd);
	}
	hostapd_owe_update_trans(iface);

	return ucv_boolean_new(true);
}

static uc_value_t *
uc_hostapd_iface_switch_channel(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_iface *iface = uc_fn_thisval("hostapd.iface");
	uc_value_t *info = uc_fn_arg(0);
	struct hostapd_config *conf;
	struct csa_settings csa = {};
	uint64_t intval;
	int i, ret = 0;

	if (!iface || ucv_type(info) != UC_OBJECT)
		return NULL;

	conf = iface->conf;
	if ((intval = ucv_int64_get(ucv_object_get(info, "csa_count", NULL))) && !errno)
		csa.cs_count = intval;
	if ((intval = ucv_int64_get(ucv_object_get(info, "sec_channel", NULL))) && !errno)
		csa.freq_params.sec_channel_offset = intval;

	csa.freq_params.ht_enabled = conf->ieee80211n;
	csa.freq_params.vht_enabled = conf->ieee80211ac;
	csa.freq_params.he_enabled = conf->ieee80211ax;
#ifdef CONFIG_IEEE80211BE
	csa.freq_params.eht_enabled = conf->ieee80211be;
#endif
	intval = ucv_int64_get(ucv_object_get(info, "oper_chwidth", NULL));
	if (errno)
		intval = hostapd_get_oper_chwidth(conf);
	if (intval)
		csa.freq_params.bandwidth = 40 << intval;
	else
		csa.freq_params.bandwidth = csa.freq_params.sec_channel_offset ? 40 : 20;

	if ((intval = ucv_int64_get(ucv_object_get(info, "frequency", NULL))) && !errno)
		csa.freq_params.freq = intval;
	if ((intval = ucv_int64_get(ucv_object_get(info, "center_freq1", NULL))) && !errno)
		csa.freq_params.center_freq1 = intval;
	if ((intval = ucv_int64_get(ucv_object_get(info, "center_freq2", NULL))) && !errno)
		csa.freq_params.center_freq2 = intval;

	for (i = 0; i < iface->num_bss; i++)
		ret = hostapd_switch_channel(iface->bss[i], &csa);

	return ucv_boolean_new(!ret);
}

static uc_value_t *
uc_hostapd_bss_rename(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_data *hapd = uc_fn_thisval("hostapd.bss");
	uc_value_t *ifname_arg = uc_fn_arg(0);
	char prev_ifname[IFNAMSIZ + 1];
	struct sta_info *sta;
	const char *ifname;
	int ret;

	if (!hapd || ucv_type(ifname_arg) != UC_STRING)
		return NULL;

	os_strlcpy(prev_ifname, hapd->conf->iface, sizeof(prev_ifname));
	ifname = ucv_string_get(ifname_arg);

	hostapd_ubus_free_bss(hapd);
	if (interfaces->ctrl_iface_deinit)
		interfaces->ctrl_iface_deinit(hapd);

	ret = hostapd_drv_if_rename(hapd, WPA_IF_AP_BSS, NULL, ifname);
	if (ret)
		goto out;

	for (sta = hapd->sta_list; sta; sta = sta->next) {
		char cur_name[IFNAMSIZ + 1], new_name[IFNAMSIZ + 1];

		if (!(sta->flags & WLAN_STA_WDS) || sta->pending_wds_enable)
			continue;

		snprintf(cur_name, sizeof(cur_name), "%s.sta%d", prev_ifname, sta->aid);
		snprintf(new_name, sizeof(new_name), "%s.sta%d", ifname, sta->aid);
		hostapd_drv_if_rename(hapd, WPA_IF_AP_VLAN, cur_name, new_name);
	}

	if (!strncmp(hapd->conf->ssid.vlan, hapd->conf->iface, sizeof(hapd->conf->ssid.vlan)))
		os_strlcpy(hapd->conf->ssid.vlan, ifname, sizeof(hapd->conf->ssid.vlan));
	os_strlcpy(hapd->conf->iface, ifname, sizeof(hapd->conf->iface));
	hostapd_ubus_add_bss(hapd);

	hostapd_ucode_update_interfaces();
	hostapd_owe_update_trans(hapd->iface);
out:
	if (interfaces->ctrl_iface_init)
		interfaces->ctrl_iface_init(hapd);

	return ret ? NULL : ucv_boolean_new(true);
}

int hostapd_ucode_sta_auth(struct hostapd_data *hapd, struct sta_info *sta)
{
	char addr[sizeof(MACSTR)];
	uc_value_t *val, *cur;
	int ret = 0;

	if (wpa_ucode_call_prepare("sta_auth"))
		return 0;

	uc_value_push(ucv_string_new(hapd->conf->iface));

	snprintf(addr, sizeof(addr), MACSTR, MAC2STR(sta->addr));
	uc_value_push(ucv_string_new(addr));

	val = wpa_ucode_call(2);

	cur = ucv_object_get(val, "psk", NULL);
	if (ucv_type(cur) == UC_ARRAY) {
		struct hostapd_sta_wpa_psk_short *p, **next;
		size_t len = ucv_array_length(cur);

		next = &sta->psk;
		hostapd_free_psk_list(*next);
		*next = NULL;

		for (size_t i = 0; i < len; i++) {
			uc_value_t *cur_psk;
			const char *str;
			size_t str_len;

			cur_psk = ucv_array_get(cur, i);
			str = ucv_string_get(cur_psk);
			str_len = strlen(str);
			if (!str || str_len < 8 || str_len > 64)
				continue;

			p = os_zalloc(sizeof(*p));
			if (len == 64) {
				if (hexstr2bin(str, p->psk, PMK_LEN) < 0) {
					free(p);
					continue;
				}
			} else {
				p->is_passphrase = 1;
				memcpy(p->passphrase, str, str_len + 1);
			}

			*next = p;
			next = &p->next;
		}
	}

	cur = ucv_object_get(val, "force_psk", NULL);
	sta->use_sta_psk = ucv_is_truish(cur);

	cur = ucv_object_get(val, "status", NULL);
	if (ucv_type(cur) == UC_INTEGER)
		ret = ucv_int64_get(cur);

	ucv_put(val);
	ucv_gc(vm);

	return ret;
}

void hostapd_ucode_sta_connected(struct hostapd_data *hapd, struct sta_info *sta)
{
	char addr[sizeof(MACSTR)];
	uc_value_t *val, *cur;
	int ret = 0;

	if (wpa_ucode_call_prepare("sta_connected"))
		return;

	uc_value_push(ucv_string_new(hapd->conf->iface));

	snprintf(addr, sizeof(addr), MACSTR, MAC2STR(sta->addr));
	uc_value_push(ucv_string_new(addr));

	val = ucv_object_new(vm);
	if (sta->psk_idx)
		ucv_object_add(val, "psk_idx", ucv_int64_new(sta->psk_idx - 1));
	uc_value_push(val);

	val = wpa_ucode_call(3);
	if (ucv_type(val) != UC_OBJECT)
		goto out;

	cur = ucv_object_get(val, "vlan", NULL);
	if (ucv_type(cur) == UC_INTEGER) {
		struct vlan_description vdesc = {
			.notempty = 1,
			.untagged = ucv_int64_get(cur),
		};

		ap_sta_set_vlan(hapd, sta, &vdesc);
		ap_sta_bind_vlan(hapd, sta);
	}

out:
	ucv_put(val);
}

static uc_value_t *
uc_wpa_rkh_derive_key(uc_vm_t *vm, size_t nargs)
{
#ifdef CONFIG_IEEE80211R_AP
	u8 oldkey[16];
	char *oldkey_hex;
	u8 key[SHA256_MAC_LEN];
	size_t key_len = sizeof(key);
	char key_hex[2 * ARRAY_SIZE(key) + 1];
	uc_value_t *val = uc_fn_arg(0);
	int i;

	if (ucv_type(val) != UC_STRING)
		return NULL;

	oldkey_hex = ucv_string_get(val);

	if (!hexstr2bin(oldkey_hex, key, key_len))
		return ucv_string_new_length(oldkey_hex, 2 * ARRAY_SIZE(key));

	if (hexstr2bin(oldkey_hex, oldkey, sizeof(oldkey))) {
		wpa_printf(MSG_ERROR, "Invalid RxKH key: '%s'", oldkey_hex);
		return NULL;
	}

	if (hmac_sha256_kdf(oldkey, sizeof(oldkey), "FT OLDKEY", NULL, 0, key, key_len) < 0) {
		wpa_printf(MSG_ERROR, "Invalid RxKH key: '%s'", oldkey_hex);
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(key); i++)
		sprintf(key_hex + 2 * i, "%02x", key[i]);

	return ucv_string_new_length(key_hex, 2 * ARRAY_SIZE(key));
#else
	return NULL;
#endif
}

int hostapd_ucode_init(struct hapd_interfaces *ifaces)
{
	static const uc_function_list_t global_fns[] = {
		{ "printf",	uc_wpa_printf },
		{ "getpid", uc_wpa_getpid },
		{ "sha1", uc_wpa_sha1 },
		{ "rkh_derive_key", uc_wpa_rkh_derive_key },
		{ "freq_info", uc_wpa_freq_info },
		{ "add_iface", uc_hostapd_add_iface },
		{ "remove_iface", uc_hostapd_remove_iface },
		{ "udebug_set", uc_wpa_udebug_set },
	};
	static const uc_function_list_t bss_fns[] = {
		{ "ctrl", uc_hostapd_bss_ctrl },
		{ "set_config", uc_hostapd_bss_set_config },
		{ "rename", uc_hostapd_bss_rename },
		{ "delete", uc_hostapd_bss_delete },
	};
	static const uc_function_list_t iface_fns[] = {
		{ "set_bss_order", uc_hostapd_iface_set_bss_order },
		{ "add_bss", uc_hostapd_iface_add_bss },
		{ "stop", uc_hostapd_iface_stop },
		{ "start", uc_hostapd_iface_start },
		{ "switch_channel", uc_hostapd_iface_switch_channel },
	};
	uc_value_t *data, *proto;

	interfaces = ifaces;
	vm = wpa_ucode_create_vm();

	global_type = uc_type_declare(vm, "hostapd.global", global_fns, NULL);
	bss_type = uc_type_declare(vm, "hostapd.bss", bss_fns, NULL);
	iface_type = uc_type_declare(vm, "hostapd.iface", iface_fns, NULL);

	bss_registry = ucv_array_new(vm);
	uc_vm_registry_set(vm, "hostap.bss_registry", bss_registry);

	iface_registry = ucv_array_new(vm);
	uc_vm_registry_set(vm, "hostap.iface_registry", iface_registry);

	global = wpa_ucode_global_init("hostapd", global_type);

	if (wpa_ucode_run(HOSTAPD_UC_PATH "hostapd.uc"))
		goto free_vm;
	ucv_gc(vm);

	return 0;

free_vm:
	wpa_ucode_free_vm();
	return -1;
}

void hostapd_ucode_free(void)
{
	if (wpa_ucode_call_prepare("shutdown") == 0)
		ucv_put(wpa_ucode_call(0));
	wpa_ucode_free_vm();
}

void hostapd_ucode_free_iface(struct hostapd_iface *iface)
{
	ucv_put(wpa_ucode_registry_remove(iface_registry, iface->ucode.idx));
}

void hostapd_ucode_bss_cb(struct hostapd_data *hapd, const char *type)
{
	uc_value_t *val;

	if (wpa_ucode_call_prepare(type))
		return;

	val = hostapd_ucode_bss_get_uval(hapd);
	uc_value_push(ucv_string_new(hapd->iface->phy));
	uc_value_push(ucv_string_new(hapd->conf->iface));
	uc_value_push(ucv_get(val));
	ucv_put(wpa_ucode_call(3));
	ucv_put(val);
	ucv_gc(vm);
}

void hostapd_ucode_free_bss(struct hostapd_data *hapd)
{
	uc_value_t *val;

	val = wpa_ucode_registry_remove(bss_registry, hapd->ucode.idx);
	if (!val)
		return;

	hapd->ucode.idx = 0;
	if (wpa_ucode_call_prepare("bss_remove"))
		return;

	uc_value_push(ucv_string_new(hapd->conf->iface));
	uc_value_push(ucv_get(val));
	ucv_put(wpa_ucode_call(2));

	ucv_put(val);
	ucv_gc(vm);
}

#ifdef CONFIG_APUP
void hostapd_ucode_apup_newpeer(struct hostapd_data *hapd, const char *ifname)
{
	uc_value_t *val;

	if (wpa_ucode_call_prepare("apup_newpeer"))
		return;

	val = hostapd_ucode_bss_get_uval(hapd);
	uc_value_push(ucv_string_new(hapd->conf->iface)); // BSS ifname
	uc_value_push(ucv_get(val));
	uc_value_push(ucv_string_new(ifname)); // APuP peer ifname
	ucv_put(wpa_ucode_call(2));
	ucv_put(val);
	ucv_gc(vm);
}
#endif // def CONFIG_APUP
