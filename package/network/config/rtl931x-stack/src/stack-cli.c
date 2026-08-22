// SPDX-License-Identifier: GPL-2.0-only

#include <errno.h>
#include <limits.h>
#include <net/if.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/rtl931x_stack.h>

#include "stack-netlink.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static const char *stack_state_name(uint8_t state)
{
	static const char * const names[] = {
		[RTL931X_STACK_STATE_DISABLED] = "disabled",
		[RTL931X_STACK_STATE_CONFIGURED] = "configured",
		[RTL931X_STACK_STATE_ERROR] = "error",
		[RTL931X_STACK_STATE_ARMED] = "armed",
		[RTL931X_STACK_STATE_PEER_VERIFIED] = "peer-verified",
	};

	if (state >= ARRAY_SIZE(names) || !names[state])
		return "unknown";
	return names[state];
}

static const char *talk_mode_name(uint8_t mode)
{
	switch (mode) {
	case RTL931X_STACK_TALK_MODE_ONE_HOP:
		return "one-hop";
	case RTL931X_STACK_TALK_MODE_UNICAST:
		return "unicast";
	default:
		return "unknown";
	}
}

static void print_bool(const char *name, bool value)
{
	printf("%s=%s\n", name, value ? "true" : "false");
}

static void print_stack_flags(uint32_t flags)
{
	printf("flags=0x%08x\n", flags);
	print_bool("flag_auto_learn", flags & RTL931X_STACK_F_AUTO_LEARN);
	print_bool("flag_drop_my_dev", flags & RTL931X_STACK_F_DROP_MY_DEV);
}

static void print_status(const struct rtl931x_stack_status *status)
{
	printf("ifindex=%u\n", status->ifindex);
	print_bool("enabled", status->enabled);
	printf("member_id=%u\n", status->member);
	printf("peer_id=%u\n", status->peer);
	printf("master_id=%u\n", status->master);
	printf("generation=%u\n", status->generation);
	printf("state=%s\n", stack_state_name(status->state));
	printf("state_id=%u\n", status->state);
	print_bool("link_up", status->link_up);
	printf("local_port_mask=0x%016llx\n",
	       (unsigned long long)status->local_port_mask);
	printf("local_delegated_port_mask=0x%016llx\n",
	       (unsigned long long)status->local_delegated_port_mask);
	print_bool("peer_netdevs_desired", status->peer_netdevs_desired);
	print_bool("peer_netdevs_active", status->peer_netdevs_active);
	print_bool("peer_netdevs_published", status->peer_netdevs_published);
	print_bool("peer_netdevs_fenced", status->peer_netdevs_fenced);
	print_bool("peer_netdevs_recovering", status->peer_netdevs_recovering);
	printf("peer_netdevs_last_error=%u\n",
	       status->peer_netdevs_last_error);
	print_stack_flags(status->flags);
}

static void print_probe(const struct rtl931x_stack_probe *probe)
{
	printf("ifindex=%u\n", probe->ifindex);
	printf("talk_mode=%s\n", talk_mode_name(probe->mode));
	printf("talk_mode_id=%u\n", probe->mode);
	printf("transaction=0x%016llx\n",
	       (unsigned long long)probe->transaction);
	printf("remote_boot_nonce=0x%016llx\n",
	       (unsigned long long)probe->boot_nonce);
	printf("remote_member_id=%u\n", probe->member);
	printf("remote_master_id=%u\n", probe->master);
	printf("remote_stack_port=%u\n", probe->stack_port);
	printf("remote_generation=%u\n", probe->generation);
	printf("round_trip_us=%u\n", probe->round_trip_us);
	printf("remote_status=0x%08x\n", probe->status);
	print_bool("remote_id_valid",
		   probe->status & RTL931X_STACK_TALK_S_ID_VALID);
	print_bool("remote_is_master",
		   probe->status & RTL931X_STACK_TALK_S_MASTER);
	print_bool("remote_configured",
		   probe->status & RTL931X_STACK_TALK_S_CONFIGURED);
	print_bool("remote_route_ready",
		   probe->status & RTL931X_STACK_TALK_S_ROUTE_READY);
	print_bool("remote_link_up",
		   probe->status & RTL931X_STACK_TALK_S_LINK_UP);
}

static void print_peer_session(uint32_t ifindex, uint64_t boot_nonce,
			       uint8_t member, uint8_t master,
			       uint32_t generation)
{
	printf("ifindex=%u\n", ifindex);
	printf("remote_boot_nonce=0x%016llx\n",
	       (unsigned long long)boot_nonce);
	printf("remote_member_id=%u\n", member);
	printf("remote_master_id=%u\n", master);
	printf("remote_generation=%u\n", generation);
}

static void print_capabilities(uint32_t capabilities)
{
	printf("remote_capabilities=0x%08x\n", capabilities);
	print_bool("remote_cap_get_switch",
		   capabilities & RTL931X_STACK_PEER_CAP_GET_SWITCH);
	print_bool("remote_cap_get_port_state",
		   capabilities & RTL931X_STACK_PEER_CAP_GET_PORT_STATE);
	print_bool("remote_cap_set_delegated",
		   capabilities & RTL931X_STACK_PEER_CAP_SET_DELEGATED);
	print_bool("remote_cap_bridge_vlan",
		   capabilities & RTL931X_STACK_PEER_CAP_BRIDGE_VLAN);
	print_bool("remote_cap_port_status_event",
		   capabilities & RTL931X_STACK_PEER_CAP_PORT_STATUS_EVENT);
	print_bool("remote_cap_set_port_admin",
		   capabilities & RTL931X_STACK_PEER_CAP_SET_PORT_ADMIN);
}

static void print_peer_switch(const struct rtl931x_stack_peer_switch *peer)
{
	print_peer_session(peer->ifindex, peer->boot_nonce, peer->member,
			   peer->master, peer->generation);
	printf("remote_port_mask=0x%016llx\n",
	       (unsigned long long)peer->port_mask);
	printf("remote_admin_up_mask=0x%016llx\n",
	       (unsigned long long)peer->admin_up_mask);
	printf("remote_carrier_mask=0x%016llx\n",
	       (unsigned long long)peer->carrier_mask);
	printf("remote_delegated_port_mask=0x%016llx\n",
	       (unsigned long long)peer->delegated_port_mask);
	printf("remote_port_count=%u\n", peer->port_count);
	printf("remote_cpu_port=%u\n", peer->cpu_port);
	printf("remote_stack_port=%u\n", peer->stack_port);
	printf("remote_max_body_len=%u\n", peer->max_body_len);
	print_capabilities(peer->capabilities);
}

static void print_peer_port(const struct rtl931x_stack_peer_port *port)
{
	print_peer_session(port->ifindex, port->boot_nonce, port->member,
			   port->master, port->generation);
	printf("remote_port=%u\n", port->port);
	printf("remote_port_flags=0x%08x\n", port->flags);
	print_bool("remote_port_admin_up",
		   port->flags & RTL931X_STACK_PEER_PORT_F_ADMIN_UP);
	print_bool("remote_port_carrier",
		   port->flags & RTL931X_STACK_PEER_PORT_F_CARRIER);
	printf("remote_port_mtu=%u\n", port->mtu);
	printf("remote_port_mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
	       port->mac[0], port->mac[1], port->mac[2], port->mac[3],
	       port->mac[4], port->mac[5]);
}

static int parse_number(const char *text, uint64_t maximum, uint64_t *value)
{
	char *end;
	uint64_t parsed;

	errno = 0;
	parsed = strtoull(text, &end, 0);
	if (errno || !text[0] || *end || parsed > maximum)
		return -EINVAL;
	*value = parsed;
	return 0;
}

static int parse_ifindex(const char *name, uint32_t *ifindex)
{
	*ifindex = if_nametoindex(name);
	if (!*ifindex) {
		fprintf(stderr, "unknown interface '%s': %s\n", name,
			strerror(errno));
		return -ENODEV;
	}
	return 0;
}

static int parse_enabled(const char *text, uint8_t *enabled)
{
	if (!strcmp(text, "enable")) {
		*enabled = 1;
		return 0;
	}
	if (!strcmp(text, "disable")) {
		*enabled = 0;
		return 0;
	}
	return -EINVAL;
}

static void usage(FILE *stream)
{
	fprintf(stream,
		"Usage:\n"
		"  rtl931x-stack status IFACE\n"
		"  rtl931x-stack get IFACE\n"
		"  rtl931x-stack talk-arm IFACE\n"
		"  rtl931x-stack talk-disarm IFACE\n"
		"  rtl931x-stack probe IFACE\n"
		"  rtl931x-stack enable IFACE MEMBER PEER MASTER GENERATION FLAGS\n"
		"  rtl931x-stack disable IFACE GENERATION\n"
		"  rtl931x-stack recover-local IFACE GENERATION\n"
		"  rtl931x-stack peer-switch IFACE\n"
		"  rtl931x-stack peer-port IFACE PORT\n"
		"  rtl931x-stack peer-netdevs IFACE enable|disable\n"
		"\n"
		"IDs are 0..15, ports are 0..55, and numbers accept decimal or 0x.\n"
		"FLAGS: bit 0 = auto-learn, bit 1 = drop-my-device (valid mask 0x3).\n");
}

static int parse_command(int argc, char **argv,
			 struct rtl931x_stack_request *request)
{
	uint64_t value;
	const char *command;
	int err;

	if (argc < 3)
		return -EINVAL;
	command = argv[1];
	err = parse_ifindex(argv[2], &request->ifindex);
	if (err)
		return err;

	if ((!strcmp(command, "status") || !strcmp(command, "get")) &&
	    argc == 3) {
		request->cmd = RTL931X_STACK_CMD_GET;
	} else if (!strcmp(command, "talk-arm") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT;
		request->enabled = 1;
	} else if (!strcmp(command, "talk-disarm") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT;
		request->enabled = 0;
	} else if (!strcmp(command, "probe") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_PROBE_PEER;
	} else if (!strcmp(command, "enable") && argc == 8) {
		request->cmd = RTL931X_STACK_CMD_SET_TWO_MEMBER;
		request->enabled = 1;
		if (parse_number(argv[3], 15, &value))
			return -EINVAL;
		request->member = value;
		if (parse_number(argv[4], 15, &value))
			return -EINVAL;
		request->peer = value;
		if (parse_number(argv[5], 15, &value))
			return -EINVAL;
		request->master = value;
		if (parse_number(argv[6], UINT32_MAX, &value))
			return -EINVAL;
		request->generation = value;
		if (parse_number(argv[7], RTL931X_STACK_F_MASK, &value))
			return -EINVAL;
		request->flags = value;
	} else if (!strcmp(command, "disable") && argc == 4) {
		request->cmd = RTL931X_STACK_CMD_SET_TWO_MEMBER;
		if (parse_number(argv[3], UINT32_MAX, &value))
			return -EINVAL;
		request->generation = value;
	} else if (!strcmp(command, "recover-local") && argc == 4) {
		request->cmd = RTL931X_STACK_CMD_RECOVER_LOCAL;
		if (parse_number(argv[3], UINT32_MAX, &value))
			return -EINVAL;
		request->generation = value;
	} else if (!strcmp(command, "peer-switch") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_GET_PEER_SWITCH;
	} else if (!strcmp(command, "peer-port") && argc == 4) {
		request->cmd = RTL931X_STACK_CMD_GET_PEER_PORT;
		if (parse_number(argv[3], 55, &value))
			return -EINVAL;
		request->remote_port = value;
	} else if (!strcmp(command, "peer-netdevs") && argc == 4) {
		request->cmd = RTL931X_STACK_CMD_SET_PEER_PORT_NETDEVS;
		if (parse_enabled(argv[3], &request->enabled))
			return -EINVAL;
	} else {
		return -EINVAL;
	}

	return 0;
}

static void print_reply(const struct rtl931x_stack_request *request,
			const union rtl931x_stack_reply *reply)
{
	switch (request->cmd) {
	case RTL931X_STACK_CMD_GET:
	case RTL931X_STACK_CMD_SET_TWO_MEMBER:
	case RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT:
	case RTL931X_STACK_CMD_RECOVER_LOCAL:
		print_status(&reply->status);
		break;
	case RTL931X_STACK_CMD_PROBE_PEER:
		print_probe(&reply->probe);
		break;
	case RTL931X_STACK_CMD_GET_PEER_SWITCH:
		print_peer_switch(&reply->peer_switch);
		break;
	case RTL931X_STACK_CMD_GET_PEER_PORT:
		print_peer_port(&reply->peer_port);
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	struct rtl931x_stack_request request = {};
	union rtl931x_stack_reply reply;
	struct rtl931x_stack_nl nl;
	int err;

	err = parse_command(argc, argv, &request);
	if (err) {
		usage(stderr);
		return 2;
	}
	err = rtl931x_stack_nl_open(&nl);
	if (err) {
		fprintf(stderr, "cannot connect to %s: %s\n",
			RTL931X_STACK_GENL_NAME, strerror(-err));
		return 1;
	}
	err = rtl931x_stack_nl_request(&nl, &request, &reply);
	if (err) {
		if (nl.kernel_error) {
			fprintf(stderr, "kernel rejected request: %s (%d)",
				strerror(-err), -err);
			if (nl.extack[0])
				fprintf(stderr, ": %s", nl.extack);
			fputc('\n', stderr);
		} else {
			fprintf(stderr, "netlink request failed: %s\n",
				strerror(-err));
		}
		rtl931x_stack_nl_close(&nl);
		return 1;
	}
	print_reply(&request, &reply);
	rtl931x_stack_nl_close(&nl);

	return 0;
}
