// SPDX-License-Identifier: GPL-2.0-only

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <net/if.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include <linux/rtl931x_stack.h>

#include <libubox/blobmsg.h>
#include <libubox/uloop.h>
#include <libubus.h>
#include <uci.h>

#include "stack-netlink.h"

#define STACK_OBJECT_NAME "rtl931x.stack"
#define READY_OBJECT_NAME "rtl931x.stack.ready"
#define RETRY_INTERVAL_MS 1000
#define HEALTH_INTERVAL_MS 2000
#define MAX_PREPARED_PORTS 64
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

enum boot_policy {
	BOOT_POLICY_WAIT,
	BOOT_POLICY_FALLBACK,
	BOOT_POLICY_FAIL,
};

enum daemon_phase {
	PHASE_DISABLED,
	PHASE_STARTING,
	PHASE_WAITING_PEER,
	PHASE_WAITING_DELEGATION,
	PHASE_WAITING_PEER_NETDEVS,
	PHASE_READY,
	PHASE_FALLING_BACK,
	PHASE_FALLBACK,
	PHASE_FAILED,
};

struct stack_config {
	char interface[IFNAMSIZ];
	enum boot_policy policy;
	uint32_t generation;
	uint32_t flags;
	uint32_t ready_timeout;
	uint8_t member;
	uint8_t peer;
	uint8_t master;
	bool enabled;
};

struct prepared_port {
	char name[IFNAMSIZ];
	bool was_up;
};

struct stack_daemon {
	struct rtl931x_stack_nl nl;
	struct rtl931x_stack_status last_status;
	struct stack_config config;
	struct ubus_context *ubus;
	struct ubus_object control_object;
	struct ubus_object ready_object;
	struct uloop_timeout timer;
	struct prepared_port prepared[MAX_PREPARED_PORTS];
	struct timespec started;
	enum daemon_phase phase;
	size_t prepared_count;
	uint32_t attempts;
	int last_error;
	char last_extack[256];
	bool nl_open;
	bool status_valid;
	bool ready_published;
	bool fallback_requested;
	bool readiness_reached;
	bool fabric_recorded;
	bool fabric_was_up;
};

static struct stack_daemon daemon_state;
static struct blob_buf reply_buf;

static const char *phase_name(enum daemon_phase phase)
{
	switch (phase) {
	case PHASE_DISABLED:
		return "disabled";
	case PHASE_STARTING:
		return "starting";
	case PHASE_WAITING_PEER:
		return "waiting-peer";
	case PHASE_WAITING_DELEGATION:
		return "waiting-delegation";
	case PHASE_WAITING_PEER_NETDEVS:
		return "waiting-peer-netdevs";
	case PHASE_READY:
		return "ready";
	case PHASE_FALLING_BACK:
		return "falling-back";
	case PHASE_FALLBACK:
		return "fallback";
	case PHASE_FAILED:
		return "failed";
	default:
		return "unknown";
	}
}

static const char *policy_name(enum boot_policy policy)
{
	switch (policy) {
	case BOOT_POLICY_WAIT:
		return "wait";
	case BOOT_POLICY_FALLBACK:
		return "fallback";
	case BOOT_POLICY_FAIL:
		return "fail";
	default:
		return "unknown";
	}
}

static uint32_t elapsed_seconds(struct stack_daemon *daemon)
{
	struct timespec now;
	uint64_t elapsed;

	clock_gettime(CLOCK_MONOTONIC, &now);
	if (now.tv_sec < daemon->started.tv_sec)
		return 0;
	elapsed = (uint64_t)(now.tv_sec - daemon->started.tv_sec);
	if (elapsed > UINT32_MAX)
		return UINT32_MAX;

	return elapsed;
}

static int parse_u32(const char *value, uint32_t maximum, uint32_t *result)
{
	unsigned long parsed;
	char *end;

	if (!value || !*value)
		return -EINVAL;
	errno = 0;
	parsed = strtoul(value, &end, 0);
	if (errno || *end || parsed > maximum)
		return -EINVAL;
	*result = parsed;

	return 0;
}

static int parse_bool(const char *value, bool fallback, bool *result)
{
	if (!value) {
		*result = fallback;
		return 0;
	}
	if (!strcmp(value, "1") || !strcasecmp(value, "true") ||
	    !strcasecmp(value, "yes") || !strcasecmp(value, "on")) {
		*result = true;
		return 0;
	}
	if (!strcmp(value, "0") || !strcasecmp(value, "false") ||
	    !strcasecmp(value, "no") || !strcasecmp(value, "off")) {
		*result = false;
		return 0;
	}

	return -EINVAL;
}

static const char *uci_option(struct uci_context *ctx,
			      struct uci_section *section, const char *name)
{
	struct uci_option *option = uci_lookup_option(ctx, section, name);

	if (!option || option->type != UCI_TYPE_STRING)
		return NULL;

	return option->v.string;
}

static int load_config(struct stack_config *config)
{
	struct uci_context *ctx;
	struct uci_package *package;
	struct uci_element *element;
	struct uci_section *selected = NULL;
	const char *value;
	uint32_t parsed;
	int enabled_sections = 0;
	int err = -EINVAL;

	memset(config, 0, sizeof(*config));
	config->policy = BOOT_POLICY_WAIT;
	config->ready_timeout = 60;

	ctx = uci_alloc_context();
	if (!ctx)
		return -ENOMEM;
	if (uci_load(ctx, "rtl931x-stack", &package) != UCI_OK) {
		err = -ENOENT;
		goto out;
	}

	uci_foreach_element(&package->sections, element) {
		struct uci_section *section = uci_to_section(element);
		bool enabled;

		if (strcmp(section->type, "stack"))
			continue;
		if (parse_bool(uci_option(ctx, section, "enabled"), false,
			       &enabled))
			goto out_unload;
		if (!enabled)
			continue;
		selected = section;
		enabled_sections++;
	}
	if (!enabled_sections) {
		config->enabled = false;
		err = 0;
		goto out_unload;
	}
	if (enabled_sections != 1) {
		err = -E2BIG;
		goto out_unload;
	}

	value = uci_option(ctx, selected, "interface");
	if (!value || !*value || strlen(value) >= sizeof(config->interface))
		goto out_unload;
	strcpy(config->interface, value);

	value = uci_option(ctx, selected, "member_id");
	if (parse_u32(value, 15, &parsed))
		goto out_unload;
	config->member = parsed;
	value = uci_option(ctx, selected, "peer_id");
	if (parse_u32(value, 15, &parsed))
		goto out_unload;
	config->peer = parsed;
	value = uci_option(ctx, selected, "master_id");
	if (parse_u32(value, 15, &parsed))
		goto out_unload;
	config->master = parsed;
	if (config->member == config->peer ||
	    (config->master != config->member &&
	     config->master != config->peer))
		goto out_unload;

	value = uci_option(ctx, selected, "generation");
	if (parse_u32(value, UINT32_MAX, &config->generation) ||
	    !config->generation)
		goto out_unload;
	value = uci_option(ctx, selected, "flags");
	if (value && parse_u32(value, RTL931X_STACK_F_MASK, &config->flags))
		goto out_unload;

	value = uci_option(ctx, selected, "boot_policy");
	if (value && !strcmp(value, "fallback"))
		config->policy = BOOT_POLICY_FALLBACK;
	else if (value && !strcmp(value, "fail"))
		config->policy = BOOT_POLICY_FAIL;
	else if (value && strcmp(value, "wait"))
		goto out_unload;

	value = uci_option(ctx, selected, "ready_timeout");
	if (value) {
		if (parse_u32(value, UINT32_MAX, &parsed))
			goto out_unload;
		config->ready_timeout = parsed;
	}
	if (config->policy != BOOT_POLICY_WAIT && !config->ready_timeout)
		goto out_unload;

	config->enabled = true;
	err = 0;

out_unload:
	uci_unload(ctx, package);
out:
	uci_free_context(ctx);
	return err;
}

static int interface_flags(const char *name, short *flags)
{
	struct ifreq request = {};
	int socket_fd;
	int err = 0;

	if (strlen(name) >= sizeof(request.ifr_name))
		return -EINVAL;
	strcpy(request.ifr_name, name);
	socket_fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (socket_fd < 0)
		return -errno;
	if (ioctl(socket_fd, SIOCGIFFLAGS, &request) < 0)
		err = -errno;
	else
		*flags = request.ifr_flags;
	close(socket_fd);

	return err;
}

static int interface_set_up(const char *name, bool up)
{
	struct ifreq request = {};
	int socket_fd;
	int err = 0;

	if (strlen(name) >= sizeof(request.ifr_name))
		return -EINVAL;
	strcpy(request.ifr_name, name);
	socket_fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (socket_fd < 0)
		return -errno;
	if (ioctl(socket_fd, SIOCGIFFLAGS, &request) < 0) {
		err = -errno;
		goto out;
	}
	if (up)
		request.ifr_flags |= IFF_UP;
	else
		request.ifr_flags &= ~IFF_UP;
	if (ioctl(socket_fd, SIOCSIFFLAGS, &request) < 0)
		err = -errno;

out:
	close(socket_fd);
	return err;
}

static int record_and_enable_fabric(struct stack_daemon *daemon)
{
	short flags;
	int err;

	if (!daemon->fabric_recorded) {
		err = interface_flags(daemon->config.interface, &flags);
		if (err)
			return err;
		daemon->fabric_was_up = flags & IFF_UP;
		daemon->fabric_recorded = true;
	}

	return interface_set_up(daemon->config.interface, true);
}

static int prepare_follower_ports(struct stack_daemon *daemon)
{
	char fabric_device[PATH_MAX];
	char candidate_device[PATH_MAX];
	char path[PATH_MAX];
	struct dirent *entry;
	DIR *directory;
	int err = 0;

	if (daemon->config.member == daemon->config.master ||
	    daemon->prepared_count)
		return 0;

	snprintf(path, sizeof(path), "/sys/class/net/%s/device",
		 daemon->config.interface);
	if (!realpath(path, fabric_device))
		return -errno;
	directory = opendir("/sys/class/net");
	if (!directory)
		return -errno;

	while ((entry = readdir(directory))) {
		struct prepared_port *prepared;
		struct stat stat_buf;
		short flags;

		if (entry->d_name[0] == '.' ||
		    !strcmp(entry->d_name, daemon->config.interface))
			continue;
		if (strlen(entry->d_name) >= IFNAMSIZ)
			continue;
		snprintf(path, sizeof(path),
			 "/sys/class/net/%s/phys_port_name", entry->d_name);
		if (stat(path, &stat_buf))
			continue;
		snprintf(path, sizeof(path), "/sys/class/net/%s/device",
			 entry->d_name);
		if (!realpath(path, candidate_device) ||
		    strcmp(candidate_device, fabric_device))
			continue;
		if (daemon->prepared_count >= MAX_PREPARED_PORTS) {
			err = -E2BIG;
			break;
		}
		err = interface_flags(entry->d_name, &flags);
		if (err)
			break;
		prepared = &daemon->prepared[daemon->prepared_count++];
		strcpy(prepared->name, entry->d_name);
		prepared->was_up = flags & IFF_UP;
		if (!prepared->was_up) {
			err = interface_set_up(prepared->name, true);
			if (err)
				break;
		}
	}
	closedir(directory);

	if (err) {
		while (daemon->prepared_count) {
			struct prepared_port *prepared =
				&daemon->prepared[--daemon->prepared_count];

			if (!prepared->was_up)
				interface_set_up(prepared->name, false);
		}
	}

	return err;
}

static void restore_prepared_ports(struct stack_daemon *daemon)
{
	while (daemon->prepared_count) {
		struct prepared_port *prepared =
			&daemon->prepared[--daemon->prepared_count];

		if (!prepared->was_up)
			interface_set_up(prepared->name, false);
	}
	if (daemon->fabric_recorded && !daemon->fabric_was_up)
		interface_set_up(daemon->config.interface, false);
	daemon->fabric_recorded = false;
}

static void record_error(struct stack_daemon *daemon, int err)
{
	daemon->last_error = err;
	if (daemon->nl.extack[0])
		snprintf(daemon->last_extack, sizeof(daemon->last_extack),
			 "%s", daemon->nl.extack);
	else
		daemon->last_extack[0] = '\0';
}

static int ensure_netlink(struct stack_daemon *daemon)
{
	int err;

	if (daemon->nl_open)
		return 0;
	err = rtl931x_stack_nl_open(&daemon->nl);
	if (!err)
		daemon->nl_open = true;

	return err;
}

static int stack_request(struct stack_daemon *daemon,
			 const struct rtl931x_stack_request *request,
			 union rtl931x_stack_reply *reply)
{
	int err;

	err = ensure_netlink(daemon);
	if (err)
		return err;
	err = rtl931x_stack_nl_request(&daemon->nl, request, reply);
	if (err == -EIO) {
		rtl931x_stack_nl_close(&daemon->nl);
		daemon->nl_open = false;
	}

	return err;
}

static int stack_get(struct stack_daemon *daemon,
		     struct rtl931x_stack_status *status)
{
	union rtl931x_stack_reply reply;
	struct rtl931x_stack_request request = {
		.cmd = RTL931X_STACK_CMD_GET,
		.ifindex = if_nametoindex(daemon->config.interface),
	};
	int err;

	if (!request.ifindex)
		return -ENODEV;
	err = stack_request(daemon, &request, &reply);
	if (!err) {
		*status = reply.status;
		daemon->last_status = reply.status;
		daemon->status_valid = true;
	}

	return err;
}

static int stack_set_talk_port(struct stack_daemon *daemon, bool enabled)
{
	union rtl931x_stack_reply reply;
	struct rtl931x_stack_request request = {
		.cmd = RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT,
		.ifindex = if_nametoindex(daemon->config.interface),
		.enabled = enabled,
	};

	if (!request.ifindex)
		return -ENODEV;

	return stack_request(daemon, &request, &reply);
}

static int stack_probe(struct stack_daemon *daemon,
		       struct rtl931x_stack_probe *probe)
{
	union rtl931x_stack_reply reply;
	struct rtl931x_stack_request request = {
		.cmd = RTL931X_STACK_CMD_PROBE_PEER,
		.ifindex = if_nametoindex(daemon->config.interface),
	};
	int err;

	if (!request.ifindex)
		return -ENODEV;
	err = stack_request(daemon, &request, &reply);
	if (!err)
		*probe = reply.probe;

	return err;
}

static int stack_set_enabled(struct stack_daemon *daemon, bool enabled,
			     uint32_t generation)
{
	union rtl931x_stack_reply reply;
	struct rtl931x_stack_request request = {
		.cmd = RTL931X_STACK_CMD_SET_TWO_MEMBER,
		.ifindex = if_nametoindex(daemon->config.interface),
		.generation = generation,
		.flags = daemon->config.flags,
		.enabled = enabled,
		.member = daemon->config.member,
		.peer = daemon->config.peer,
		.master = daemon->config.master,
	};

	if (!request.ifindex)
		return -ENODEV;

	return stack_request(daemon, &request, &reply);
}

static int stack_set_peer_netdevs(struct stack_daemon *daemon, bool enabled)
{
	union rtl931x_stack_reply reply;
	struct rtl931x_stack_request request = {
		.cmd = RTL931X_STACK_CMD_SET_PEER_PORT_NETDEVS,
		.ifindex = if_nametoindex(daemon->config.interface),
		.enabled = enabled,
	};

	if (!request.ifindex)
		return -ENODEV;

	return stack_request(daemon, &request, &reply);
}

static void add_status_blob(struct stack_daemon *daemon)
{
	blobmsg_add_string(&reply_buf, "phase", phase_name(daemon->phase));
	blobmsg_add_string(&reply_buf, "policy",
			   policy_name(daemon->config.policy));
	blobmsg_add_string(&reply_buf, "interface", daemon->config.interface);
	blobmsg_add_u8(&reply_buf, "configured", daemon->config.enabled);
	blobmsg_add_u8(&reply_buf, "ready", daemon->ready_published);
	blobmsg_add_u8(&reply_buf, "fallback",
		       daemon->phase == PHASE_FALLBACK);
	blobmsg_add_u32(&reply_buf, "member_id", daemon->config.member);
	blobmsg_add_u32(&reply_buf, "peer_id", daemon->config.peer);
	blobmsg_add_u32(&reply_buf, "master_id", daemon->config.master);
	blobmsg_add_u32(&reply_buf, "generation", daemon->config.generation);
	blobmsg_add_u32(&reply_buf, "flags", daemon->config.flags);
	blobmsg_add_u32(&reply_buf, "ready_timeout",
			daemon->config.ready_timeout);
	blobmsg_add_u32(&reply_buf, "elapsed", elapsed_seconds(daemon));
	blobmsg_add_u32(&reply_buf, "attempts", daemon->attempts);
	if (daemon->status_valid) {
		blobmsg_add_u8(&reply_buf, "stack_enabled",
			       daemon->last_status.enabled);
		blobmsg_add_u8(&reply_buf, "link_up",
			       daemon->last_status.link_up);
		blobmsg_add_u32(&reply_buf, "stack_state",
				daemon->last_status.state);
		blobmsg_add_u64(&reply_buf, "local_port_mask",
				(unsigned long long)
				daemon->last_status.local_port_mask);
		blobmsg_add_u64(&reply_buf, "local_delegated_port_mask",
				(unsigned long long)
				daemon->last_status.local_delegated_port_mask);
		blobmsg_add_u8(&reply_buf, "peer_netdevs_desired",
			       daemon->last_status.peer_netdevs_desired);
		blobmsg_add_u8(&reply_buf, "peer_netdevs_active",
			       daemon->last_status.peer_netdevs_active);
		blobmsg_add_u8(&reply_buf, "peer_netdevs_published",
			       daemon->last_status.peer_netdevs_published);
		blobmsg_add_u8(&reply_buf, "peer_netdevs_fenced",
			       daemon->last_status.peer_netdevs_fenced);
		blobmsg_add_u8(&reply_buf, "peer_netdevs_recovering",
			       daemon->last_status.peer_netdevs_recovering);
		blobmsg_add_u32(&reply_buf, "peer_netdevs_last_error",
				daemon->last_status.peer_netdevs_last_error);
	}
	if (daemon->last_error) {
		blobmsg_add_u32(&reply_buf, "error", -daemon->last_error);
		blobmsg_add_string(&reply_buf, "error_string",
				   strerror(-daemon->last_error));
	}
	if (daemon->last_extack[0])
		blobmsg_add_string(&reply_buf, "extack", daemon->last_extack);
}

static int ubus_status(struct ubus_context *ctx, struct ubus_object *object,
		       struct ubus_request_data *request, const char *method,
		       struct blob_attr *message)
{
	struct stack_daemon *daemon = &daemon_state;

	(void)object;
	(void)method;
	(void)message;
	blob_buf_init(&reply_buf, 0);
	add_status_blob(daemon);
	ubus_send_reply(ctx, request, reply_buf.head);

	return 0;
}

static void schedule_converge(struct stack_daemon *daemon,
			      unsigned int delay_ms)
{
	uloop_timeout_set(&daemon->timer, delay_ms);
}

static void remove_ready_object(struct stack_daemon *daemon)
{
	if (!daemon->ready_published)
		return;
	ubus_remove_object(daemon->ubus, &daemon->ready_object);
	daemon->ready_published = false;
}

static int publish_ready_object(struct stack_daemon *daemon,
				enum daemon_phase phase)
{
	int err;

	daemon->phase = phase;
	if (daemon->ready_published)
		return 0;
	err = ubus_add_object(daemon->ubus, &daemon->ready_object);
	if (err)
		return -EIO;
	daemon->ready_published = true;
	daemon->readiness_reached = true;
	syslog(LOG_NOTICE, "stack reached %s state",
	       phase == PHASE_FALLBACK ? "fallback" : "stacked");

	return 0;
}

static int ubus_retry(struct ubus_context *ctx, struct ubus_object *object,
		      struct ubus_request_data *request, const char *method,
		      struct blob_attr *message)
{
	struct stack_daemon *daemon = &daemon_state;

	(void)object;
	(void)method;
	(void)message;
	if (!daemon->config.enabled || daemon->phase == PHASE_FALLBACK)
		return UBUS_STATUS_NOT_SUPPORTED;
	remove_ready_object(daemon);
	daemon->fallback_requested = false;
	daemon->last_error = 0;
	daemon->last_extack[0] = '\0';
	daemon->phase = PHASE_STARTING;
	clock_gettime(CLOCK_MONOTONIC, &daemon->started);
	schedule_converge(daemon, 1);

	return ubus_status(ctx, object, request, method, message);
}

static int ubus_fallback(struct ubus_context *ctx, struct ubus_object *object,
			 struct ubus_request_data *request, const char *method,
			 struct blob_attr *message)
{
	struct stack_daemon *daemon = &daemon_state;

	(void)object;
	(void)method;
	(void)message;
	if (!daemon->config.enabled)
		return UBUS_STATUS_NOT_SUPPORTED;
	remove_ready_object(daemon);
	daemon->fallback_requested = true;
	daemon->phase = PHASE_FALLING_BACK;
	schedule_converge(daemon, 1);

	return ubus_status(ctx, object, request, method, message);
}

static const struct ubus_method control_methods[] = {
	UBUS_METHOD_NOARG("status", ubus_status),
	UBUS_METHOD_NOARG("retry", ubus_retry),
	UBUS_METHOD_NOARG("fallback", ubus_fallback),
};

static const struct ubus_method ready_methods[] = {
	UBUS_METHOD_NOARG("status", ubus_status),
};

static struct ubus_object_type control_object_type =
	UBUS_OBJECT_TYPE(STACK_OBJECT_NAME, control_methods);
static struct ubus_object_type ready_object_type =
	UBUS_OBJECT_TYPE(READY_OBJECT_NAME, ready_methods);

static void ubus_connection_lost(struct ubus_context *ctx)
{
	(void)ctx;
	syslog(LOG_ERR, "ubus connection lost; restarting");
	uloop_end();
}

static bool status_matches_config(struct stack_daemon *daemon,
				  const struct rtl931x_stack_status *status)
{
	return status->enabled && status->member == daemon->config.member &&
	       status->peer == daemon->config.peer &&
	       status->master == daemon->config.master &&
	       status->generation == daemon->config.generation &&
	       status->flags == daemon->config.flags &&
	       status->state == RTL931X_STACK_STATE_CONFIGURED;
}

static bool peer_netdevs_ready(struct stack_daemon *daemon,
			       const struct rtl931x_stack_status *status)
{
	if (daemon->config.member != daemon->config.master)
		return true;

	return status->peer_netdevs_desired &&
	       status->peer_netdevs_active &&
	       status->peer_netdevs_published &&
	       !status->peer_netdevs_fenced &&
	       !status->peer_netdevs_recovering &&
	       !status->peer_netdevs_last_error;
}

static bool probe_matches_config(struct stack_daemon *daemon,
				 const struct rtl931x_stack_probe *probe)
{
	uint32_t required = RTL931X_STACK_TALK_S_ID_VALID |
		RTL931X_STACK_TALK_S_CONFIGURED |
		RTL931X_STACK_TALK_S_ROUTE_READY |
		RTL931X_STACK_TALK_S_LINK_UP;

	return probe->mode == RTL931X_STACK_TALK_MODE_UNICAST &&
	       (probe->status & required) == required &&
	       probe->member == daemon->config.peer &&
	       probe->master == daemon->config.master &&
	       probe->generation == daemon->config.generation;
}

static void retry_or_apply_policy(struct stack_daemon *daemon, int err,
				  enum daemon_phase retry_phase)
{
	bool changed = daemon->last_error != err ||
		strcmp(daemon->last_extack, daemon->nl.extack);

	record_error(daemon, err);
	daemon->phase = retry_phase;
	if (changed)
		syslog(LOG_WARNING, "stack bootstrap failed: %s%s%s",
		       strerror(-err), daemon->last_extack[0] ? ": " : "",
		       daemon->last_extack);
	if (daemon->fallback_requested) {
		daemon->phase = PHASE_FALLING_BACK;
		schedule_converge(daemon, RETRY_INTERVAL_MS);
		return;
	}

	if (!daemon->readiness_reached &&
	    daemon->config.policy != BOOT_POLICY_WAIT &&
	    elapsed_seconds(daemon) >= daemon->config.ready_timeout) {
		if (daemon->config.policy == BOOT_POLICY_FALLBACK) {
			daemon->fallback_requested = true;
			daemon->phase = PHASE_FALLING_BACK;
			schedule_converge(daemon, 1);
		} else {
			daemon->phase = PHASE_FAILED;
			syslog(LOG_ERR,
			       "stack readiness timed out; automatic retries stopped");
		}
		return;
	}

	schedule_converge(daemon, RETRY_INTERVAL_MS);
}

static int converge_fallback(struct stack_daemon *daemon)
{
	struct rtl931x_stack_status status;
	uint32_t disable_generation;
	int err;

	daemon->phase = PHASE_FALLING_BACK;
	err = stack_get(daemon, &status);
	if (err)
		return err;

	if (status.enabled) {
		if (status.member == status.master) {
			err = stack_set_peer_netdevs(daemon, false);
			if (err)
				return err;
		} else if (status.local_delegated_port_mask) {
			/* The leader must revoke delegation before local teardown. */
			return -EBUSY;
		}

		disable_generation = status.generation;
		if (disable_generation < daemon->config.generation)
			disable_generation = daemon->config.generation;
		if (disable_generation == UINT32_MAX)
			return -EOVERFLOW;
		err = stack_set_enabled(daemon, false, disable_generation + 1);
		if (err)
			return err;
	}

	err = stack_set_talk_port(daemon, false);
	if (err)
		return err;
	restore_prepared_ports(daemon);
	daemon->last_error = 0;
	daemon->last_extack[0] = '\0';

	return publish_ready_object(daemon, PHASE_FALLBACK);
}

static int converge_stack(struct stack_daemon *daemon)
{
	struct rtl931x_stack_status status;
	struct rtl931x_stack_probe probe;
	int err;

	err = stack_get(daemon, &status);
	if (err)
		return err;
	if (status.state == RTL931X_STACK_STATE_ERROR)
		return -EUCLEAN;

	if (status.enabled) {
		if (!status_matches_config(daemon, &status))
			return -ESTALE;
		err = record_and_enable_fabric(daemon);
		if (err)
			return err;
		if (!status.link_up)
			return -ENOLINK;
		if (daemon->config.member != daemon->config.master &&
		    status.local_delegated_port_mask != status.local_port_mask) {
			err = prepare_follower_ports(daemon);
			if (err)
				return err;
		}
		err = stack_probe(daemon, &probe);
		if (err)
			return err;
		if (!probe_matches_config(daemon, &probe))
			return -ESTALE;

		if (daemon->config.member == daemon->config.master) {
			daemon->phase = PHASE_WAITING_PEER_NETDEVS;
			err = stack_set_peer_netdevs(daemon, true);
			if (err)
				return err;
			err = stack_get(daemon, &status);
			if (err)
				return err;
			if (!status_matches_config(daemon, &status) ||
			    !status.link_up)
				return -ESTALE;
			if (!peer_netdevs_ready(daemon, &status)) {
				daemon->phase = PHASE_WAITING_PEER_NETDEVS;
				return -EAGAIN;
			}
		} else {
			err = stack_get(daemon, &status);
			if (err)
				return err;
			if (!status_matches_config(daemon, &status))
				return -ESTALE;
			if (!status.link_up)
				return -ENOLINK;
			if (!status.local_port_mask ||
			    status.local_delegated_port_mask !=
				status.local_port_mask) {
				daemon->phase = PHASE_WAITING_DELEGATION;
				return -EAGAIN;
			}
		}

		daemon->last_error = 0;
		daemon->last_extack[0] = '\0';
		return publish_ready_object(daemon, PHASE_READY);
	}

	if (status.state == RTL931X_STACK_STATE_DISABLED) {
		err = stack_set_talk_port(daemon, true);
		if (err)
			return err;
	}
	err = record_and_enable_fabric(daemon);
	if (err)
		return err;
	err = prepare_follower_ports(daemon);
	if (err)
		return err;
	err = stack_probe(daemon, &probe);
	if (err)
		return err;
	err = stack_set_enabled(daemon, true, daemon->config.generation);
	if (err)
		return err;

	/* Re-read and verify routed Device Talk on the next iteration. */
	return -EAGAIN;
}

static void converge_timeout(struct uloop_timeout *timeout)
{
	struct stack_daemon *daemon =
		container_of(timeout, struct stack_daemon, timer);
	struct rtl931x_stack_status status;
	int err;

	daemon->attempts++;
	if (daemon->fallback_requested) {
		err = converge_fallback(daemon);
		if (err)
			retry_or_apply_policy(daemon, err, PHASE_FALLING_BACK);
		return;
	}

	if (daemon->phase == PHASE_READY) {
		err = stack_get(daemon, &status);
		if (!err && status_matches_config(daemon, &status) &&
		    status.link_up && peer_netdevs_ready(daemon, &status) &&
		    (daemon->config.member == daemon->config.master ||
		     (status.local_port_mask &&
		      status.local_delegated_port_mask ==
			status.local_port_mask))) {
			schedule_converge(daemon, HEALTH_INTERVAL_MS);
			return;
		}
		remove_ready_object(daemon);
		daemon->phase = PHASE_STARTING;
	}

	err = converge_stack(daemon);
	if (err) {
		retry_or_apply_policy(daemon, err,
				      err == -EAGAIN ||
				      daemon->phase == PHASE_WAITING_PEER_NETDEVS ?
				      daemon->phase : PHASE_WAITING_PEER);
		return;
	}
	schedule_converge(daemon, HEALTH_INTERVAL_MS);
}

int main(void)
{
	struct stack_daemon *daemon = &daemon_state;
	int config_err;
	int err;

	openlog("rtl931x-stackd", LOG_PID, LOG_DAEMON);
	clock_gettime(CLOCK_MONOTONIC, &daemon->started);
	daemon->phase = PHASE_STARTING;
	config_err = load_config(&daemon->config);
	if (config_err) {
		daemon->phase = PHASE_FAILED;
		daemon->last_error = config_err;
		syslog(LOG_ERR, "invalid configuration: %s",
		       strerror(-config_err));
	} else if (!daemon->config.enabled) {
		daemon->phase = PHASE_DISABLED;
	}

	err = uloop_init();
	if (err) {
		syslog(LOG_ERR, "failed to initialize event loop: %s",
		       strerror(errno));
		err = EXIT_FAILURE;
		goto out_log;
	}
	daemon->ubus = ubus_connect(NULL);
	if (!daemon->ubus) {
		syslog(LOG_ERR, "failed to connect to ubus");
		err = EXIT_FAILURE;
		goto out_uloop;
	}
	daemon->ubus->connection_lost = ubus_connection_lost;
	ubus_add_uloop(daemon->ubus);

	daemon->control_object.name = STACK_OBJECT_NAME;
	daemon->control_object.type = &control_object_type;
	daemon->control_object.methods = control_methods;
	daemon->control_object.n_methods = ARRAY_SIZE(control_methods);
	err = ubus_add_object(daemon->ubus, &daemon->control_object);
	if (err) {
		syslog(LOG_ERR, "failed to publish %s: %s", STACK_OBJECT_NAME,
		       ubus_strerror(err));
		err = EXIT_FAILURE;
		goto out_ubus;
	}

	daemon->ready_object.name = READY_OBJECT_NAME;
	daemon->ready_object.type = &ready_object_type;
	daemon->ready_object.methods = ready_methods;
	daemon->ready_object.n_methods = ARRAY_SIZE(ready_methods);
	daemon->timer.cb = converge_timeout;
	if (!config_err && daemon->config.enabled)
		schedule_converge(daemon, 1);

	uloop_run();
	remove_ready_object(daemon);
	ubus_remove_object(daemon->ubus, &daemon->control_object);
	err = EXIT_SUCCESS;

out_ubus:
	ubus_free(daemon->ubus);
out_uloop:
	uloop_done();
	blob_buf_free(&reply_buf);
	if (daemon->nl_open)
		rtl931x_stack_nl_close(&daemon->nl);
out_log:
	closelog();
	return err;
}
