#!/usr/bin/env ucode
// SPDX-License-Identifier: GPL-2.0-only
'use strict';

import * as fs from 'fs';
import { cursor } from 'uci';

const JRDCTL = '/usr/bin/jrdctl';
const UBUS = '/bin/ubus';
const PROFILE_CONFIG = 'jrd';

function to_array(value) {
	if (value == null || value == '')
		return [];

	return type(value) == 'array' ? value : [ value ];
}

function load_profile(config) {
	let uci = cursor();
	let profile_name;
	let profile;

	try {
		uci.load(PROFILE_CONFIG);
		profile_name = config.profile ?? uci.get(PROFILE_CONFIG, 'globals', 'default_profile');
		profile = profile_name != null ? uci.get_all(PROFILE_CONFIG, profile_name) : null;
	}
	catch (e) {
		warn(`jrd: unable to load /etc/config/${PROFILE_CONFIG}\n`);
		return {
			...config,
			profile_error: 'profile configuration unavailable'
		};
	}

	if (profile_name == null || profile == null || profile['.type'] != 'profile') {
		warn(`jrd: unknown modem profile '${profile_name ?? ''}'\n`);
		return {
			...config,
			profile: profile_name,
			profile_error: 'unknown modem profile'
		};
	}

	/*
	 * /etc/config/jrd supplies modem-profile defaults. Explicit options on the
	 * network interface always win, allowing a single profile value to be
	 * overridden without modifying the shared profile.
	 */
	return {
		...profile,
		...config,
		profile: profile_name,
		rpc_timeout: config.rpc_timeout ?? profile.rpc_timeout ?? '3',
		timeout: config.timeout ?? profile.timeout ?? '30'
	};
}

function rpc(config, command) {
	let host = config.modem_ip;
	let port = int(config.port);
	let timeout = int(config.rpc_timeout ?? 3);
	let proc = fs.popen([
		JRDCTL,
		'-H', host,
		'-p', '' + port,
		'-T', '' + timeout,
		command
	], 'r');

	if (!proc) {
		warn(`jrd: failed to execute ${JRDCTL}\n`);
		return null;
	}

	let output = trim(proc.read('all'));
	let status = proc.close();

	if (status != 0) {
		warn(`jrd: ${command} failed with exit code ${status}\n`);
		return null;
	}

	try {
		return json(output);
	}
	catch (e) {
		warn(`jrd: invalid JSON returned by ${command}: ${output}\n`);
		return null;
	}
}

function rpc_method(config, method, params) {
	let host = config.modem_ip;
	let port = int(config.port);
	let timeout = int(config.rpc_timeout ?? 3);
	let proc = fs.popen([
		JRDCTL,
		'-H', host,
		'-p', '' + port,
		'-T', '' + timeout,
		'rpc', method, params ?? '{}'
	], 'r');

	if (!proc) {
		warn(`jrd: failed to execute ${JRDCTL} rpc ${method}\n`);
		return null;
	}

	let output = trim(proc.read('all'));
	let status = proc.close();

	if (status != 0) {
		warn(`jrd: rpc ${method} failed with exit code ${status}\n`);
		return null;
	}

	try {
		return json(output);
	}
	catch (e) {
		warn(`jrd: invalid JSON returned by rpc ${method}: ${output}\n`);
		return null;
	}
}

function network_interface_up(name) {
	if (name == null || name == '' || !fs.access(UBUS, 'x'))
		return false;

	let proc = fs.popen([
		UBUS, 'call', `network.interface.${name}`, 'status'
	], 'r');

	if (!proc)
		return false;

	let output = trim(proc.read('all'));
	let status = proc.close();

	if (status != 0 || output == '')
		return false;

	try {
		return json(output)?.up == true;
	}
	catch (e) {
		return false;
	}
}

function sync_ethernet_led(config) {
	let ethernet_interface = config.ethernet_interface;

	if (ethernet_interface == null || ethernet_interface == '')
		return;

	let state = network_interface_up(ethernet_interface) ? 1 : 0;
	let reply = rpc_method(config, 'ethWanStatus',
		`{"ethlan_status":${state}}`);

	/* LED reporting is auxiliary and must never make the JRD data link fail. */
	if (reply == null || reply.error != null)
		warn(`jrd: unable to report ethlan_status=${state} for '${ethernet_interface}'\n`);
}

function connection_state(config) {
	let reply = rpc(config, 'status');
	let state = reply?.result?.ConnectionStatus;

	if (state == null)
		return null;

	return int(state);
}

function wait_for_control(config, timeout_ms) {
	let elapsed = 0;

	while (elapsed <= timeout_ms) {
		let state = connection_state(config);

		if (state != null)
			return state;

		sleep(500);
		elapsed += 500;
	}

	return null;
}

function wait_for_connected(config, timeout_ms) {
	let elapsed = 0;

	while (elapsed <= timeout_ms) {
		if (connection_state(config) == 2)
			return true;

		sleep(500);
		elapsed += 500;
	}

	return false;
}

function management_link(proto) {
	let config = proto.config;

	return {
		ifname: proto.device,
		ipaddr: [ {
			ipaddr: config.local_ip,
			mask: '' + config.prefix
		} ]
	};
}

function connected_link(proto) {
	let config = proto.config;
	let gateway = config.gateway;
	let data = management_link(proto);

	if (config.defaultroute != '0') {
		let route = {
			target: '0.0.0.0',
			netmask: '0',
			gateway: gateway
		};

		if (config.metric != null && config.metric != '')
			route.metric = int(config.metric);

		data.routes = [ route ];
	}

	if (config.peerdns != '0') {
		let dns = to_array(config.dns);

		if (length(dns) == 0)
			dns = [ gateway ];

		data.dns = dns;
	}

	return data;
}

function proto_setup(proto) {
	let config = proto.config;
	let timeout_ms = int(config.timeout ?? 30) * 1000;

	if (config.profile_error != null) {
		warn(`jrd: ${config.profile_error}\n`);
		proto.setup_failed();
		return;
	}

	if (!fs.access(JRDCTL, 'x')) {
		warn(`jrd: ${JRDCTL} is not executable\n`);
		proto.setup_failed();
		return;
	}

	if (!proto.device) {
		warn('jrd: no network device configured\n');
		proto.setup_failed();
		return;
	}

	if (config.modem_ip == null || config.port == null ||
	    config.local_ip == null || config.prefix == null ||
	    config.gateway == null) {
		warn(`jrd: profile '${config.profile ?? ''}' is incomplete\n`);
		proto.setup_failed();
		return;
	}

	/*
	 * The JRD control socket lives behind the modem management link, so give
	 * the configured data device its profile-defined management address before
	 * trying the control endpoint. The default route is added only after the
	 * PDP connection reports connected.
	 */
	proto.update_link(true, management_link(proto));

	let state = wait_for_control(config, timeout_ms);
	if (state == null) {
		warn('jrd: modem control service did not become reachable\n');
		proto.update_link(false);
		proto.setup_failed();
		return;
	}

	/* Synchronize the modem-controlled HH70 LAN indicator at protocol start. */
	sync_ethernet_led(config);

	if (state != 2) {
		let reply = rpc(config, 'connect');

		if (!reply || reply.error || !wait_for_connected(config, timeout_ms)) {
			warn('jrd: Connect did not reach ConnectionStatus=2\n');
			proto.update_link(false);
			proto.setup_failed();
			return;
		}
	}

	proto.update_link(true, connected_link(proto));
}

function proto_teardown(proto) {
	/* Keep the management address present until DisConnect has been sent. */
	if (proto.config.profile_error == null)
		rpc(proto.config, 'disconnect');

	proto.update_link(false);
}

function proto_renew(proto) {
	proto_setup(proto);
}

netifd.add_proto({
	name: 'jrd',
	available: true,
	no_proto_task: true,
	'renew-handler': true,

	config: function(ctx) {
		return load_profile(ctx.data);
	},

	setup: proto_setup,
	teardown: proto_teardown,
	renew: proto_renew
});
