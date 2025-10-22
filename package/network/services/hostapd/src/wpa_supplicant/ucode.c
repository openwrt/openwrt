#include "utils/includes.h"
#include "utils/common.h"
#include "utils/ucode.h"
#include "drivers/driver.h"
#include "ap/hostapd.h"
#include "wpa_supplicant_i.h"
#include "wps_supplicant.h"
#include "ctrl_iface.h"
#include "config.h"
#include "bss.h"
#include "ucode.h"

static struct wpa_global *wpa_global;
static uc_resource_type_t *global_type, *iface_type;
static uc_value_t *global, *iface_registry;
static uc_vm_t *vm;

static uc_value_t *
wpas_ucode_iface_get_uval(struct wpa_supplicant *wpa_s)
{
	uc_value_t *val;

	if (wpa_s->ucode.idx)
		return ucv_get(wpa_ucode_registry_get(iface_registry, wpa_s->ucode.idx));

	val = uc_resource_new(iface_type, wpa_s);
	wpa_s->ucode.idx = wpa_ucode_registry_add(iface_registry, val);

	return val;
}

static void
wpas_ucode_update_interfaces(void)
{
	uc_value_t *ifs = ucv_object_new(vm);
	struct wpa_supplicant *wpa_s;
	int i;

	for (wpa_s = wpa_global->ifaces; wpa_s; wpa_s = wpa_s->next)
		ucv_object_add(ifs, wpa_s->ifname, wpas_ucode_iface_get_uval(wpa_s));

	ucv_object_add(ucv_prototype_get(global), "interfaces", ifs);
}

static uc_value_t *
wpas_ucode_bss_get_uval(struct wpa_bss *bss)
{
	uc_value_t *val;

	val = ucv_object_new(vm);
	ucv_object_add(val, "freq", ucv_int64_new(bss->freq));
	ucv_object_add(val, "ssid", ucv_string_new_length(bss->ssid, bss->ssid_len));
	ucv_object_add(val, "snr", ucv_int64_new(bss->snr));
	ucv_object_add(val, "signal", ucv_int64_new(bss->level));
	ucv_object_add(val, "noise", ucv_int64_new(bss->noise));

	return val;
}

void wpas_ucode_add_bss(struct wpa_supplicant *wpa_s)
{
	uc_value_t *val;

	if (wpa_ucode_call_prepare("iface_add"))
		return;

	uc_value_push(ucv_string_new(wpa_s->ifname));
	uc_value_push(wpas_ucode_iface_get_uval(wpa_s));
	ucv_put(wpa_ucode_call(2));
}

void wpas_ucode_free_bss(struct wpa_supplicant *wpa_s)
{
	uc_value_t *val;

	val = wpa_ucode_registry_remove(iface_registry, wpa_s->ucode.idx);
	if (!val)
		return;

	wpa_s->ucode.idx = 0;
	if (wpa_ucode_call_prepare("iface_remove"))
		return;

	uc_value_push(ucv_string_new(wpa_s->ifname));
	uc_value_push(ucv_get(val));
	ucv_put(wpa_ucode_call(2));
	ucv_put(val);
}

bool wpas_ucode_bss_allowed(struct wpa_supplicant *wpa_s, struct wpa_bss *bss)
{
	uc_value_t *val;
	bool ret = true;

	if (wpa_ucode_call_prepare("bss_allowed"))
		return true;

	uc_value_push(ucv_string_new(wpa_s->ifname));
	uc_value_push(wpas_ucode_bss_get_uval(bss));
	val = wpa_ucode_call(2);

	if (ucv_type(val) == UC_BOOLEAN)
		ret = ucv_boolean_get(val);
	ucv_put(val);

	return ret;
}

void wpas_ucode_update_state(struct wpa_supplicant *wpa_s)
{
	const char *state;
	uc_value_t *val;

	val = wpa_ucode_registry_get(iface_registry, wpa_s->ucode.idx);
	if (!val)
		return;

	if (wpa_ucode_call_prepare("state"))
		return;

	state = wpa_supplicant_state_txt(wpa_s->wpa_state);
	uc_value_push(ucv_string_new(wpa_s->ifname));
	uc_value_push(ucv_get(val));
	uc_value_push(ucv_string_new(state));
	ucv_put(wpa_ucode_call(3));
}

void wpas_ucode_event(struct wpa_supplicant *wpa_s, int event, union wpa_event_data *data)
{
	const char *state;
	uc_value_t *val;

	if (event != EVENT_CH_SWITCH_STARTED)
		return;

	val = wpa_ucode_registry_get(iface_registry, wpa_s->ucode.idx);
	if (!val)
		return;

	if (wpa_ucode_call_prepare("event"))
		return;

	uc_value_push(ucv_string_new(wpa_s->ifname));
	uc_value_push(ucv_get(val));
	uc_value_push(ucv_string_new(event_to_string(event)));
	val = ucv_object_new(vm);
	uc_value_push(ucv_get(val));

	if (event == EVENT_CH_SWITCH_STARTED) {
		ucv_object_add(val, "csa_count", ucv_int64_new(data->ch_switch.count));
		ucv_object_add(val, "frequency", ucv_int64_new(data->ch_switch.freq));
		ucv_object_add(val, "sec_chan_offset", ucv_int64_new(data->ch_switch.ch_offset));
		ucv_object_add(val, "center_freq1", ucv_int64_new(data->ch_switch.cf1));
		ucv_object_add(val, "center_freq2", ucv_int64_new(data->ch_switch.cf2));
	}

	ucv_put(wpa_ucode_call(4));
}

static const char *obj_stringval(uc_value_t *obj, const char *name)
{
	uc_value_t *val = ucv_object_get(obj, name, NULL);

	return ucv_string_get(val);
}

static uc_value_t *
uc_wpas_add_iface(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *info = uc_fn_arg(0);
	uc_value_t *driver = ucv_object_get(info, "driver", NULL);
	uc_value_t *ifname = ucv_object_get(info, "iface", NULL);
	uc_value_t *bridge = ucv_object_get(info, "bridge", NULL);
	uc_value_t *config = ucv_object_get(info, "config", NULL);
	uc_value_t *ctrl = ucv_object_get(info, "ctrl", NULL);
	struct wpa_interface iface;
	int ret = -1;

	if (ucv_type(info) != UC_OBJECT)
		goto out;

	iface = (struct wpa_interface){
		.driver = "nl80211",
		.ifname = ucv_string_get(ifname),
		.bridge_ifname = ucv_string_get(bridge),
		.confname = ucv_string_get(config),
		.ctrl_interface = ucv_string_get(ctrl),
	};

	if (driver) {
		const char *drvname;
		if (ucv_type(driver) != UC_STRING)
			goto out;

		iface.driver = NULL;
		drvname = ucv_string_get(driver);
		for (int i = 0; wpa_drivers[i]; i++) {
			if (!strcmp(drvname, wpa_drivers[i]->name))
				iface.driver = wpa_drivers[i]->name;
		}

		if (!iface.driver)
			goto out;
	}

	if (!iface.ifname || !iface.confname)
		goto out;

	ret = wpa_supplicant_add_iface(wpa_global, &iface, 0) ? 0 : -1;
	wpas_ucode_update_interfaces();

out:
	return ucv_int64_new(ret);
}

static uc_value_t *
uc_wpas_remove_iface(uc_vm_t *vm, size_t nargs)
{
	struct wpa_supplicant *wpa_s = NULL;
	uc_value_t *ifname_arg = uc_fn_arg(0);
	const char *ifname = ucv_string_get(ifname_arg);
	int ret = -1;

	if (!ifname)
		goto out;

	for (wpa_s = wpa_global->ifaces; wpa_s; wpa_s = wpa_s->next)
		if (!strcmp(wpa_s->ifname, ifname))
			break;

	if (!wpa_s)
		goto out;

	ret = wpa_supplicant_remove_iface(wpa_global, wpa_s, 0);
	wpas_ucode_update_interfaces();

out:
	return ucv_int64_new(ret);
}

static void
uc_wpas_iface_status_bss(uc_value_t *ret, struct wpa_bss *bss)
{
	int sec_chan = 0;
	const u8 *ie;

	ie = wpa_bss_get_ie(bss, WLAN_EID_HT_OPERATION);
	if (ie && ie[1] >= 2) {
		const struct ieee80211_ht_operation *ht_oper;
		int sec;

		ht_oper = (const void *) (ie + 2);
		sec = ht_oper->ht_param & HT_INFO_HT_PARAM_SECONDARY_CHNL_OFF_MASK;
		if (sec == HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE)
			sec_chan = 1;
		else if (sec == HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW)
			sec_chan = -1;
	}

	ucv_object_add(ret, "sec_chan_offset", ucv_int64_new(sec_chan));
	ucv_object_add(ret, "frequency", ucv_int64_new(bss->freq));
}

static void
uc_wpas_iface_status_multi_ap(uc_vm_t *vm, uc_value_t *ret, struct wpa_supplicant *wpa_s)
{
	struct multi_ap_params *multi_ap = &wpa_s->multi_ap;
	uc_value_t *obj;

	if (!wpa_s->multi_ap_ie)
		return;

	obj = ucv_object_new(vm);
	ucv_object_add(ret, "multi_ap", obj);

	ucv_object_add(obj, "profile", ucv_int64_new(multi_ap->profile));
	ucv_object_add(obj, "capability", ucv_int64_new(multi_ap->capability));
	ucv_object_add(obj, "vlanid", ucv_int64_new(multi_ap->vlanid));
}

static uc_value_t *
uc_wpas_iface_status(uc_vm_t *vm, size_t nargs)
{
	struct wpa_supplicant *wpa_s = uc_fn_thisval("wpas.iface");
	struct wpa_bss *bss;
	uc_value_t *ret;

	if (!wpa_s)
		return NULL;

	ret = ucv_object_new(vm);

	ucv_object_add(ret, "state", ucv_string_new(wpa_supplicant_state_txt(wpa_s->wpa_state)));

	bss = wpa_s->current_bss;
	if (bss)
		uc_wpas_iface_status_bss(ret, bss);

	if (wpa_s->valid_links) {
		unsigned int valid_links = wpa_s->valid_links;
		uc_value_t *link, *links;

		links = ucv_array_new(vm);

		for (size_t i = 0;
		     valid_links && i < ARRAY_SIZE(wpa_s->links);
			 i++, valid_links >>= 1) {
			bss = wpa_s->links[i].bss;

			if (!(valid_links & 1) || !bss)
				continue;

			link = ucv_object_new(vm);
			uc_wpas_iface_status_bss(link, bss);
			ucv_array_set(links, i, link);
		}

		ucv_object_add(ret, "links", links);
	}

	if (wpa_s->wpa_state == WPA_COMPLETED)
		uc_wpas_iface_status_multi_ap(vm, ret, wpa_s);

#ifdef CONFIG_MESH
	if (wpa_s->ifmsh) {
		struct hostapd_iface *ifmsh = wpa_s->ifmsh;

		ucv_object_add(ret, "sec_chan_offset", ucv_int64_new(ifmsh->conf->secondary_channel));
		ucv_object_add(ret, "frequency", ucv_int64_new(ifmsh->freq));
	}
#endif

	return ret;
}

static uc_value_t *
uc_wpas_iface_ctrl(uc_vm_t *vm, size_t nargs)
{
	struct wpa_supplicant *wpa_s = uc_fn_thisval("wpas.iface");
	uc_value_t *arg = uc_fn_arg(0);
	size_t reply_len;
	uc_value_t *ret;
	char *reply;

	if (!wpa_s || ucv_type(arg) != UC_STRING)
		return NULL;

	reply = wpa_supplicant_ctrl_iface_process(wpa_s, ucv_string_get(arg), &reply_len);
	if (reply_len < 0)
		return NULL;

	if (reply_len && reply[reply_len - 1] == '\n')
		reply_len--;

	ret = ucv_string_new_length(reply, reply_len);
	free(reply);

	return ret;
}

static uc_value_t *
uc_wpas_iface_config(uc_vm_t *vm, size_t nargs)
{
	struct wpa_supplicant *wpa_s = uc_fn_thisval("wpas.iface");
	uc_value_t *arg = uc_fn_arg(0);
	uc_value_t *val = uc_fn_arg(1);
	uc_value_t *ret = NULL;
	bool get = nargs == 1;
	const char *name;
	size_t len = 0;

	if (!wpa_s || ucv_type(arg) != UC_STRING)
		return NULL;

	name = ucv_string_get(arg);
	if (!strcmp(name, "freq_list")) {
		if (get) {
			int *cur = wpa_s->conf->freq_list;
			if (!cur)
				return NULL;

			ret = ucv_array_new(vm);
			while (*cur)
				ucv_array_set(ret, len++, ucv_int64_new(*(cur++)));
		} else {
			size_t len = ucv_array_length(val);
			int *freq_list;

			if (ucv_type(val) != UC_ARRAY)
				return NULL;

			freq_list = calloc(len + 1, sizeof(*freq_list));
			for (size_t i = 0; i < len; i++) {
				uc_value_t *cur = ucv_array_get(val, i);

				if (ucv_type(cur) != UC_INTEGER) {
					free(freq_list);
					return NULL;
				}

				freq_list[i] = ucv_int64_get(cur);
			}

			free(wpa_s->conf->freq_list);
			wpa_s->conf->freq_list = freq_list;
			ret = ucv_boolean_new(true);
		}
	}

	return ret;
}

int wpas_ucode_init(struct wpa_global *gl)
{
	static const uc_function_list_t global_fns[] = {
		{ "printf",	uc_wpa_printf },
		{ "getpid", uc_wpa_getpid },
		{ "add_iface", uc_wpas_add_iface },
		{ "remove_iface", uc_wpas_remove_iface },
		{ "udebug_set", uc_wpa_udebug_set },
	};
	static const uc_function_list_t iface_fns[] = {
		{ "status", uc_wpas_iface_status },
		{ "ctrl", uc_wpas_iface_ctrl },
		{ "config", uc_wpas_iface_config },
	};
	uc_value_t *data, *proto;

	wpa_global = gl;
	vm = wpa_ucode_create_vm();

	global_type = uc_type_declare(vm, "wpas.global", global_fns, NULL);
	iface_type = uc_type_declare(vm, "wpas.iface", iface_fns, NULL);

	iface_registry = ucv_array_new(vm);
	uc_vm_registry_set(vm, "wpas.iface_registry", iface_registry);

	global = wpa_ucode_global_init("wpas", global_type);

	if (wpa_ucode_run(HOSTAPD_UC_PATH "wpa_supplicant.uc"))
		goto free_vm;

	wpas_ucode_update_interfaces();

	return 0;

free_vm:
	wpa_ucode_free_vm();
	return -1;
}

void wpas_ucode_free(void)
{
	if (wpa_ucode_call_prepare("shutdown") == 0)
		ucv_put(wpa_ucode_call(0));
	wpa_ucode_free_vm();
}
