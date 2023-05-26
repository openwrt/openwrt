#include <sys/un.h>

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/ucode.h"
#include "hostapd.h"
#include "ap_drv_ops.h"
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
		return wpa_ucode_registry_get(bss_registry, hapd->ucode.idx);

	val = uc_resource_new(bss_type, hapd);
	wpa_ucode_registry_add(bss_registry, val, &hapd->ucode.idx);

	return val;
}

static uc_value_t *
hostapd_ucode_iface_get_uval(struct hostapd_iface *hapd)
{
	uc_value_t *val;

	if (hapd->ucode.idx)
		return wpa_ucode_registry_get(iface_registry, hapd->ucode.idx);

	val = uc_resource_new(iface_type, hapd);
	wpa_ucode_registry_add(iface_registry, val, &hapd->ucode.idx);

	return val;
}

static void
hostapd_ucode_update_bss_list(struct hostapd_iface *iface)
{
	uc_value_t *ifval, *list;
	int i;

	list = ucv_array_new(vm);
	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *hapd = iface->bss[i];
		uc_value_t *val = hostapd_ucode_bss_get_uval(hapd);
		uc_value_t *proto = ucv_prototype_get(val);

		ucv_object_add(proto, "name", ucv_get(ucv_string_new(hapd->conf->iface)));
		ucv_object_add(proto, "index", ucv_int64_new(i));
		ucv_array_set(list, i, ucv_get(val));
	}

	ifval = hostapd_ucode_iface_get_uval(iface);
	ucv_object_add(ucv_prototype_get(ifval), "bss", ucv_get(list));
}

static void
hostapd_ucode_update_interfaces(void)
{
	uc_value_t *ifs = ucv_object_new(vm);
	int i;

	for (i = 0; i < interfaces->count; i++) {
		struct hostapd_iface *iface = interfaces->iface[i];

		ucv_object_add(ifs, iface->phy, ucv_get(hostapd_ucode_iface_get_uval(iface)));
		hostapd_ucode_update_bss_list(iface);
	}

	ucv_object_add(ucv_prototype_get(global), "interfaces", ucv_get(ifs));
	ucv_gc(vm);
}

static uc_value_t *
uc_hostapd_add_iface(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *iface = uc_fn_arg(0);
	int ret;

	if (ucv_type(iface) != UC_STRING)
		return ucv_int64_new(-1);

	ret = hostapd_add_iface(interfaces, ucv_string_get(iface));
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

static uc_value_t *
uc_hostapd_bss_set_config(uc_vm_t *vm, size_t nargs)
{
	struct hostapd_data *hapd = uc_fn_thisval("hostapd.bss");
	struct hostapd_bss_config *old_bss;
	struct hostapd_iface *iface;
	struct hostapd_config *conf;
	uc_value_t *file = uc_fn_arg(0);
	uc_value_t *index = uc_fn_arg(1);
	unsigned int i, idx = 0;
	int ret = -1;

	if (!hapd || ucv_type(file) != UC_STRING)
		goto out;

	if (ucv_type(index) == UC_INTEGER)
		idx = ucv_int64_get(index);

	iface = hapd->iface;
	conf = interfaces->config_read_cb(ucv_string_get(file));
	if (!conf || idx > conf->num_bss || !conf->bss[idx])
		goto out;

	hostapd_bss_deinit_no_free(hapd);
	hostapd_drv_stop_ap(hapd);
	hostapd_free_hapd_data(hapd);

	old_bss = hapd->conf;
	for (i = 0; i < iface->conf->num_bss; i++)
		if (iface->conf->bss[i] == hapd->conf)
			iface->conf->bss[i] = conf->bss[idx];
	hapd->conf = conf->bss[idx];
	conf->bss[idx] = old_bss;
	hostapd_config_free(conf);

	hostapd_setup_bss(hapd, hapd == iface->bss[0], !iface->conf->mbssid);

	ret = 0;

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

	if (!hapd || hapd == hapd->iface->bss[0])
		return NULL;

	iface = hapd->iface;
	for (idx = 0; idx < iface->num_bss; idx++)
		if (iface->bss[idx] == hapd)
			break;

	if (idx == iface->num_bss)
		return NULL;

	for (i = idx + 1; i < iface->num_bss; i++)
		iface->bss[i - 1] = iface->bss[i];
	iface->num_bss--;

	hostapd_drv_stop_ap(hapd);
	hostapd_bss_deinit(hapd);
	hostapd_remove_iface_bss_conf(iface->conf, hapd->conf);
	hostapd_config_free_bss(hapd->conf);
	os_free(hapd);

	hostapd_ucode_update_bss_list(iface);
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
	hostapd_ucode_update_bss_list(iface);
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

int hostapd_ucode_init(struct hapd_interfaces *ifaces)
{
	static const uc_function_list_t global_fns[] = {
		{ "printf",	uc_wpa_printf },
		{ "getpid", uc_wpa_getpid },
		{ "sha1", uc_wpa_sha1 },
		{ "add_iface", uc_hostapd_add_iface },
		{ "remove_iface", uc_hostapd_remove_iface },
	};
	static const uc_function_list_t bss_fns[] = {
		{ "ctrl", uc_hostapd_bss_ctrl },
		{ "set_config", uc_hostapd_bss_set_config },
		{ "delete", uc_hostapd_bss_delete },
	};
	static const uc_function_list_t iface_fns[] = {
		{ "add_bss", uc_hostapd_iface_add_bss }
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
	wpa_ucode_registry_remove(iface_registry, iface->ucode.idx);
}

void hostapd_ucode_add_bss(struct hostapd_data *hapd)
{
	uc_value_t *val;

	if (wpa_ucode_call_prepare("bss_add"))
		return;

	val = hostapd_ucode_bss_get_uval(hapd);
	uc_value_push(ucv_get(ucv_string_new(hapd->conf->iface)));
	uc_value_push(ucv_get(val));
	ucv_put(wpa_ucode_call(2));
	ucv_gc(vm);
}

void hostapd_ucode_reload_bss(struct hostapd_data *hapd, int reconf)
{
	uc_value_t *val;

	if (wpa_ucode_call_prepare("bss_reload"))
		return;

	val = hostapd_ucode_bss_get_uval(hapd);
	uc_value_push(ucv_get(ucv_string_new(hapd->conf->iface)));
	uc_value_push(ucv_get(val));
	uc_value_push(ucv_int64_new(reconf));
	ucv_put(wpa_ucode_call(3));
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
	ucv_gc(vm);
}
