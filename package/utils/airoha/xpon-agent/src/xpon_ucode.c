// SPDX-License-Identifier: GPL-2.0-only
/* Native configuration and inventory access, not an OAM protocol agent. */

#include <limits.h>
#include <net/if.h>
#include <stdlib.h>
#include <ucode/module.h>

#include <linux/xpon.h>
#include "nl_transport.h"

struct field {
	unsigned int attr;
	const char *name;
	unsigned int size;
};

static const struct field fields[] = {
	{ XPON_ATTR_IFINDEX, "ifindex", 4 },
	{ XPON_ATTR_MODE, "mode", 4 },
	{ XPON_ATTR_AVAILABLE_MODES, "available_modes", 4 },
	{ XPON_ATTR_REGISTRATION, "registration", 1 },
	{ XPON_ATTR_CARRIER, "carrier", 1 },
	{ XPON_ATTR_SIGNAL_DETECT, "signal_detect", 1 },
	{ XPON_ATTR_LOS, "los", 1 },
	{ XPON_ATTR_CAPABILITIES, "capabilities", 4 },
	{ XPON_ATTR_MAX_LINKS, "max_links", 2 },
	{ XPON_ATTR_MAX_ENDPOINTS, "max_endpoints", 2 },
	{ XPON_ATTR_MAX_QUEUES, "max_queues", 1 },
	{ XPON_ATTR_REPORT_SETS, "report_sets", 1 },
	{ XPON_ATTR_MAX_REPORT_SETS, "max_report_sets", 1 },
	{ XPON_ATTR_ONU_TYPE, "onu_type", 1 },
	{ XPON_ATTR_UNI_COUNT, "uni_count", 1 },
	{ XPON_ATTR_ID, "id", 4 },
	{ XPON_ATTR_TYPE, "type", 1 },
	{ XPON_ATTR_WIRE_ID, "wire_id", 2 },
	{ XPON_ATTR_SCHEDULER_ID, "scheduler_id", 2 },
	{ XPON_ATTR_LINK_ID, "link_id", 4 },
	{ XPON_ATTR_QUEUE, "queue", 1 },
	{ XPON_ATTR_ENABLED, "enabled", 1 },
	{ XPON_ATTR_NETDEV_IFINDEX, "netdev_ifindex", 4 },
	{ XPON_ATTR_NAME, "name", 0 },
	{ XPON_ATTR_LINK_STATE, "link_state", 1 },
	{ XPON_ATTR_RX_PACKETS, "rx_oam", 8 },
	{ XPON_ATTR_TX_PACKETS, "tx_oam", 8 },
	{ XPON_ATTR_REPORT_SET, "report_set", 1 },
	{ XPON_ATTR_THRESHOLD_TQ, "threshold_tq", 4 },
};

struct reply {
	uc_vm_t *vm;
	uc_value_t *value;
	bool array;
	uint8_t cmd;
	uint32_t ifindex;
};

static uc_value_t *failure(int ret, const char *where)
{
	set_errno_error(ret, where);
	return NULL;
}

static bool number(uc_value_t *v, uint32_t max, uint32_t *out)
{
	uint64_t n;

	if (ucv_type(v) != UC_INTEGER)
		return false;
	errno = 0;
	n = ucv_uint64_get(v);
	if (errno || n > max)
		return false;
	*out = n;
	return true;
}

static int decode_reply(struct nlmsghdr *nlh, void *arg)
{
	struct reply *r = arg;
	struct genlmsghdr *genl;
	struct nlattr *attrs[XPON_ATTR_MAX + 1] = {};
	struct nlattr *attr;
	uc_value_t *obj;
	char name[IF_NAMESIZE];
	int remaining;

	if (nlh->nlmsg_len < NLMSG_LENGTH(GENL_HDRLEN))
		return -EBADMSG;
	genl = NLMSG_DATA(nlh);
	if (genl->version != XPON_GENL_VERSION || genl->cmd != r->cmd)
		return -EPROTO;
	remaining = nlh->nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN;
	attr = (void *)((char *)genl + GENL_HDRLEN);
	while (remaining) {
		unsigned int type;
		int step;

		if (!attr_ok(attr, remaining))
			return -EBADMSG;
		type = attr->nla_type & NLA_TYPE_MASK;
		if (type && type <= XPON_ATTR_MAX && type != XPON_ATTR_PAD) {
			if (attrs[type] || (attr->nla_type & ~NLA_TYPE_MASK))
				return -EBADMSG;
			attrs[type] = attr;
		}
		step = nla_align_len(attr->nla_len);
		if (step > remaining)
			return -EBADMSG;
		remaining -= step;
		attr = (void *)((char *)attr + step);
	}
	if (!attrs[XPON_ATTR_IFINDEX] ||
	    nla_payload_len(attrs[XPON_ATTR_IFINDEX]) != 4 ||
	    attr_u32(attrs[XPON_ATTR_IFINDEX]) != r->ifindex)
		return -EBADMSG;
	/* Validate the complete reply before allocating a ucode object. */
	for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
		const struct field *f = &fields[i];
		int len;

		attr = attrs[f->attr];
		if (!attr)
			continue;
		len = nla_payload_len(attr);
		if (f->size ? len != (int)f->size :
		    (len <= 0 || len > IF_NAMESIZE ||
		     ((char *)nla_data_ptr(attr))[len - 1] != '\0'))
			return -EBADMSG;
	}
	obj = ucv_object_new(r->vm);
	for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
		const struct field *f = &fields[i];
		uc_value_t *value;

		attr = attrs[f->attr];
		if (!attr)
			continue;
		switch (f->size) {
		case 1: value = ucv_uint64_new(attr_u8(attr)); break;
		case 2: value = ucv_uint64_new(attr_u16(attr)); break;
		case 4: value = ucv_uint64_new(attr_u32(attr)); break;
		case 8:
			/* Preserve counters above JS's exact integer range over ubus. */
			{
				char count[32];

				snprintf(count, sizeof(count), "%llu",
					 (unsigned long long)attr_u64(attr));
				value = ucv_string_new(count);
			}
			break;
		default: value = ucv_string_new(nla_data_ptr(attr)); break;
		}
		ucv_object_add(obj, f->name, value);
	}
	if (if_indextoname(r->ifindex, name))
		ucv_object_add(obj, "netdev", ucv_string_new(name));
	if (r->array)
		ucv_array_push(r->value, obj);
	else {
		ucv_put(r->value);
		r->value = obj;
	}
	return 0;
}

static int open_client(struct genl_client *ctx)
{
	int ret = nl_open(ctx, XPON_GENL_NAME);

	if (!ret && ctx->family_version != XPON_GENL_VERSION) {
		nl_close(ctx);
		return -EPROTONOSUPPORT;
	}
	return ret;
}

static int query(struct genl_client *ctx, struct reply *r,
		 uint8_t cmd, uint32_t index, bool dump)
{
	struct nl_request req;
	int ret;

	r->cmd = cmd;
	r->ifindex = index;
	r->array = dump;
	if (dump)
		r->value = ucv_array_new(r->vm);
	request_init(ctx, &req, ctx->family_id, cmd, XPON_GENL_VERSION,
		     dump ? NLM_F_DUMP : 0);
	ret = add_u32(&req, XPON_ATTR_IFINDEX, index);
	if (!ret)
		ret = request_exec(ctx, &req, true, decode_reply, r);
	return ret;
}

static int inventory(struct genl_client *ctx, uc_vm_t *vm, uc_value_t **value)
{
	struct if_nameindex *list = if_nameindex(), *it;
	uc_value_t *array;
	int ret = 0;

	if (!list)
		return -errno;
	array = ucv_array_new(vm);
	for (it = list; it->if_index; it++) {
		struct reply r = { .vm = vm };

		ret = query(ctx, &r, XPON_CMD_GET_CAPS, it->if_index, false);
		if (ret == -ENODEV)
			continue;
		if (ret) {
			ucv_put(r.value);
			break;
		}
		ucv_array_push(array, r.value);
	}
	if_freenameindex(list);
	if (ret && ret != -ENODEV) {
		ucv_put(array);
		return ret;
	}
	*value = array;
	return 0;
}

static int device_index(struct genl_client *ctx, uc_vm_t *vm,
			uc_value_t *value, uint32_t *index)
{
	uc_value_t *list, *item;
	int ret;

	if (ucv_type(value) == UC_INTEGER)
		return number(value, UINT32_MAX, index) && *index ? 0 : -EINVAL;
	if (ucv_type(value) == UC_STRING && ucv_string_length(value)) {
		if (ucv_string_length(value) >= IF_NAMESIZE ||
		    strlen(ucv_string_get(value)) != ucv_string_length(value))
			return -EINVAL;
		*index = if_nametoindex(ucv_string_get(value));
		return *index ? 0 : -ENODEV;
	}
	if (value && ucv_type(value) != UC_STRING)
		return -EINVAL;
	ret = inventory(ctx, vm, &list);
	if (ret)
		return ret;
	ret = ucv_array_length(list) == 1 ? 0 :
		ucv_array_length(list) ? -ENOTUNIQ : -ENODEV;
	if (!ret) {
		item = ucv_array_get(list, 0);
		*index = ucv_uint64_get(ucv_object_get(item, "ifindex", NULL));
	}
	ucv_put(list);
	return ret;
}

static uc_value_t *uc_devices(uc_vm_t *vm, size_t nargs)
{
	struct genl_client ctx;
	uc_value_t *value = NULL;
	int ret;
	(void)nargs;

	ret = open_client(&ctx);
	if (ret)
		return failure(ret, "open xPON netlink");
	ret = inventory(&ctx, vm, &value);
	nl_close(&ctx);
	if (ret)
		return failure(ret, "list xPON devices");
	set_error(0, "success");
	return value;
}

static uc_value_t *get(uc_vm_t *vm, size_t nargs, uint8_t cmd, bool dump)
{
	struct genl_client ctx;
	struct reply r = { .vm = vm };
	uint32_t index;
	int ret = open_client(&ctx);

	if (ret)
		return failure(ret, "open xPON netlink");
	ret = device_index(&ctx, vm, uc_fn_arg(0), &index);
	if (!ret)
		ret = query(&ctx, &r, cmd, index, dump);
	nl_close(&ctx);
	if (ret) {
		ucv_put(r.value);
		return failure(ret, "query xPON device");
	}
	set_error(0, "success");
	return r.value;
}

#define GETTER(name, cmd, dump) \
static uc_value_t *uc_##name(uc_vm_t *vm, size_t nargs) \
{ (void)nargs; return get(vm, nargs, cmd, dump); }

GETTER(status, XPON_CMD_GET_STATE, false)
GETTER(capabilities, XPON_CMD_GET_CAPS, false)
GETTER(links, XPON_CMD_LINK_GET, true)
GETTER(endpoints, XPON_CMD_ENDPOINT_GET, true)
GETTER(unis, XPON_CMD_UNI_GET, true)

static uc_value_t *uc_family(uc_vm_t *vm, size_t nargs)
{
	struct genl_client ctx;
	uc_value_t *value;
	int ret = nl_open(&ctx, XPON_GENL_NAME);
	(void)nargs;

	if (ret)
		return failure(ret, "resolve xPON family");
	value = ucv_object_new(vm);
	ucv_object_add(value, "name", ucv_string_new(XPON_GENL_NAME));
	ucv_object_add(value, "version", ucv_uint64_new(ctx.family_version));
	ucv_object_add(value, "client_version", ucv_uint64_new(XPON_GENL_VERSION));
	ucv_object_add(value, "compatible",
		       ucv_boolean_new(ctx.family_version == XPON_GENL_VERSION));
	nl_close(&ctx);
	set_error(0, "success");
	return value;
}

static uc_value_t *modify(uc_vm_t *vm, size_t nargs, uint8_t cmd)
{
	struct genl_client ctx;
	struct nl_request req;
	struct reply r = { .vm = vm, .cmd = cmd };
	uc_value_t *opts = uc_fn_arg(1), *result;
	uint32_t id, index, enabled, queue, set, sets, threshold;
	bool report = cmd != XPON_CMD_LINK_SET;
	int ret;

	if (ucv_type(opts) != UC_OBJECT ||
	    !number(ucv_object_get(opts, "id", NULL), UINT32_MAX, &id) || !id)
		return failure(-EINVAL, "invalid local link ID");
	if (!report) {
		if (!number(ucv_object_get(opts, "enabled", NULL), 1, &enabled))
			return failure(-EINVAL, "enabled must be 0 or 1");
	} else {
		if (!number(ucv_object_get(opts, "queue", NULL), UINT8_MAX, &queue) ||
		    !number(ucv_object_get(opts, "set", NULL), UINT8_MAX, &set))
			return failure(-EINVAL, "invalid REPORT queue or threshold set");
		if (cmd == XPON_CMD_REPORT_SET &&
		    (!number(ucv_object_get(opts, "sets", NULL), UINT8_MAX, &sets) ||
		     !sets || set >= sets ||
		     !number(ucv_object_get(opts, "threshold_tq", NULL), UINT32_MAX,
			     &threshold)))
			return failure(-EINVAL, "invalid REPORT threshold configuration");
	}
	ret = open_client(&ctx);
	if (ret)
		return failure(ret, "open xPON netlink");
	ret = device_index(&ctx, vm, uc_fn_arg(0), &index);
	if (ret)
		goto out;
	r.ifindex = index;
	request_init(&ctx, &req, ctx.family_id, cmd, XPON_GENL_VERSION, 0);
	ret = add_u32(&req, XPON_ATTR_IFINDEX, index);
	if (!ret) ret = add_u32(&req, XPON_ATTR_ID, id);
	if (!ret && !report) ret = add_u8(&req, XPON_ATTR_ENABLED, enabled);
	if (!ret && report) ret = add_u8(&req, XPON_ATTR_QUEUE, queue);
	if (!ret && report) ret = add_u8(&req, XPON_ATTR_REPORT_SET, set);
	if (!ret && cmd == XPON_CMD_REPORT_SET) {
		ret = add_u8(&req, XPON_ATTR_REPORT_SETS, sets);
		if (!ret) ret = add_u32(&req, XPON_ATTR_THRESHOLD_TQ, threshold);
	}
	if (!ret)
		ret = request_exec(&ctx, &req, cmd == XPON_CMD_REPORT_GET,
				   decode_reply, &r);
out:
	nl_close(&ctx);
	if (ret) {
		ucv_put(r.value);
		return failure(ret, "xPON configuration request");
	}
	set_error(0, "success");
	result = cmd == XPON_CMD_REPORT_GET ? r.value : ucv_boolean_new(true);
	return result;
}

static uc_value_t *uc_link_set(uc_vm_t *vm, size_t nargs)
{ (void)nargs; return modify(vm, nargs, XPON_CMD_LINK_SET); }
static uc_value_t *uc_report_get(uc_vm_t *vm, size_t nargs)
{ (void)nargs; return modify(vm, nargs, XPON_CMD_REPORT_GET); }
static uc_value_t *uc_report_set(uc_vm_t *vm, size_t nargs)
{ (void)nargs; return modify(vm, nargs, XPON_CMD_REPORT_SET); }

static uc_value_t *uc_error(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *value = ucv_object_new(vm);
	(void)nargs;
	ucv_object_add(value, "errno", ucv_int64_new(abs(last_error.code)));
	ucv_object_add(value, "message", ucv_string_new(last_error.message));
	return value;
}

static const uc_function_list_t functions[] = {
	{ "error", uc_error },
	{ "family", uc_family },
	{ "devices", uc_devices },
	{ "status", uc_status },
	{ "capabilities", uc_capabilities },
	{ "links", uc_links },
	{ "endpoints", uc_endpoints },
	{ "unis", uc_unis },
	{ "link_set", uc_link_set },
	{ "report_get", uc_report_get },
	{ "report_set", uc_report_set },
};

void uc_module_init(uc_vm_t *vm, uc_value_t *scope)
{
	(void)vm;
	uc_function_list_register(scope, functions);
}
