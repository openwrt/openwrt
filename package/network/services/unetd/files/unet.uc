// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

import { readfile, writefile, mkstemp, mkdir, unlink } from "fs";
import { time_format } from "cli.utils";
import * as editor from "cli.object-editor";
import * as rtnl from "rtnl";
import * as uci from "uci";

const supported_service_types = [
	"vxlan", "uconfig", "unetacl",
];

function get_networks()
{
	let ret = [];

	uci.cursor().foreach("network", "interface", (s) => {
		if (s.proto != "unet")
			return;
		push(ret, s[".name"]);
	});

	return ret;
}

function get_network_status()
{
	let data = model.ubus.call("unetd", "network_get");
	if (!data)
		return {};

	return data.networks;
}

function network_get_string_file(str)
{
	let f = mkstemp();
	f.write(str);
	f.flush();
	f.seek();
	return f;
}

function network_get_file_string(f)
{
	f.seek();
	let str = trim(f.read("all"));
	f.close();
	return str;
}

function network_keygen(pw_file, args, config, out_file, extra_args)
{
	let rounds = config.rounds;
	let salt = config.salt;
	let out, output, xorkey;

	if (!out_file) {
		output = mkstemp();
		out_file = "/dev/fd/" + output.fileno();
	}

	if (extra_args)
		extra_args = '"' + extra_args + '"';
	else
		extra_args = "";
	args += ` -s ${rounds},${salt} -o ${out_file}`;

	if (config.xorkey) {
		xorkey = network_get_string_file(config.xorkey);
		args += " -x /dev/fd/" + xorkey.fileno();
	}

	pw_file.seek();
	args += " <&" + pw_file.fileno() + " " + extra_args;
	let rc = system("unet-tool " + args);

	if (xorkey)
		xorkey.close();

	if (output)
		out = network_get_file_string(output);
	else
		out = true;

	if (rc != 0)
		return;

	return out;
}

function network_get_pubkey(pw_file, network)
{
	let key = network_keygen(pw_file, '-P', network.config);
	if (!key)
		return ctx.command_failed("Failed to generate public key");
	return key;
}

function __network_fetch_password(ctx, named, confirm)
{
	if (named.password)
		return true;

	if (!model.cb.getpass) {
		if (ctx.invalid_argument)
			ctx.invalid_argument("Could not get network config password");
		return;
	}

	let pw = model.cb.getpass((confirm ? "Set new" : "Network") + " config password: ");
	if (length(pw) < 12) {
		if (ctx.invalid_argument)
			ctx.invalid_argument("Password must be at least 12 characters long");
		return;
	}

	if (confirm) {
		let pw2 = model.cb.getpass("Confirm config password: ");
		if (pw != pw2) {
			if (ctx.invalid_argument)
				ctx.invalid_argument("Password mismatch");
			return;
		}
	}

	named.password = pw;

	return true;
}

function network_fetch_password(ctx, named, confirm)
{
	if (ctx.data.netdata)
		named.password ??= ctx.data.netdata.password;

	if (!__network_fetch_password(ctx, named, confirm))
		return;

	let pw_file = network_get_string_file(named.password);

	return pw_file;
}

function network_generate_salt()
{
	let salt = readfile("/dev/urandom", 16);
	if (length(salt) != 16)
		return;
	salt = map(split(salt, ""), (v) => ord(v));
	salt = join("", map(salt, (v) => sprintf("%02x", v)));
	return salt;
}

function network_sign_data(ctx, name, network, pw_file, upload)
{
	let rounds = network.config.rounds;
	let salt = network.config.salt;

	mkdir("/etc/unetd", 0700);
	let json_file = "/etc/unetd/" + name + ".json";
	let bin_file = "/etc/unetd/" + name + ".bin";
	if (upload)
		bin_file += "." + time();

	writefile(json_file, sprintf("%.J\n", network));
	let ret = network_keygen(pw_file, '-S', network.config, bin_file, json_file);
	unlink(json_file);
	if (!ret) {
		if (ctx.command_failed)
			ctx.command_failed("Failed to sign network configuration");
		return false;
	}

	if (!upload)
		return true;

	ret = system(`unet-tool -U 127.0.0.1 "${bin_file}"`);
	unlink(bin_file);
	if (ret) {
		if (ctx.command_failed)
			ctx.command_failed("Failed to upload network configuration");
		return false;
	}

	pw_file.close();
	return true;
}

function network_create_uci(model, name, iface)
{
	model.run_hook("unet_create", name, iface);
	let cur = uci.cursor(null, null, "");
	cur.set("network", name, "interface");
	for (let key, val in iface)
		cur.set("network", name, key, val);
	cur.commit();

	system("reload_config");
}

const config_editor = {
	change_cb: function(ctx, argv) {
		ctx.data.netdata.changed = true;
	},
	add: {
		help: "Add configuration parameter value",
	},
	set: {
		help: "Set configuration parameters",
	},
	remove: {
		help: "Remove configuration parameter value",
	},
	named_args: {
		port: {
			help: "wireguard port",
			default: 51830,
			required: true,
			args: {
				type: "int",
				min: 1,
				max: 65535,
			}
		},
		"unet-port": {
			help: "unet protocol port",
			default: 51831,
			required: true,
			attribute: "peer-exchange-port",
			args: {
				type: "int",
				min: 1,
				max: 65535,
			}
		},
		keepalive: {
			help: "keepalive interval (seconds)",
			default: 10,
			args: {
				type: "int",
				min: 0,
			}
		},
		"stun-server": {
			help: "STUN server",
			multiple: true,
			args: {
				type: "host",
			}
		}
	}
};

const UnetConfigEdit = editor.new(config_editor);

const iface_editor = {
	change_cb: function(ctx, argv) {
		ctx.data.netdata.iface_changed = true;
	},
	add: {
		help: "Add interface parameter value",
	},
	set: {
		help: "Set interface parameters",
	},
	remove: {
		help: "Remove interface parameter value",
	},
	named_args: {
		metric: {
			help: "Interface metric",
			allow_empty: true,
			default: 100,
			args: {
				type: "int",
			}
		},
		zone: {
			help: "Firewall zone",
			allow_empty: true,
			default: "lan",
			args: {
				type: "string",
			}
		},
		domain: {
			help: "Local DNS domain for unet hosts",
			default: "unet",
			allow_empty: true,
			args: {
				type: "host"
			}
		},
		"local-network": {
			help: "Local network interface for discovering peers",
			default: [ "lan" ],
			attribute: "local_network",
			allow_empty: true,
			multiple: true,
			args: {
				type: "string",
			}
		},
		connect: {
			help: "Connect to remote IP or broadcast address",
			allow_empty: true,
			multiple: true,
			args: {
				type: "string",
			},
		},
	},
};

const network_local_args = {
	...iface_editor.named_args,
	network: {
		help: "network name",
		default: "unet",
		required: true,
		args: {
			type: "string",
		}
	},
};

const UnetIfaceEdit = editor.new(iface_editor);

function network_create(ctx, argv, named) {
	ctx.apply_defaults();

	if (!named.network || index(named.network, "/") >= 0)
		return ctx.error("Invalid network name: %s", named.network);

	let pw_file = network_fetch_password(ctx, named, true);
	if (!pw_file)
		return;

	let salt = network_generate_salt();
	if (!salt)
		return ctx.unknown_error();

	let rounds = 10000;

	let xorkey_file = mkstemp();
	system(`unet-tool -G >&${xorkey_file.fileno()}`);
	let xorkey = network_get_file_string(xorkey_file);

	let network = {
		config: {
			salt, rounds, xorkey,
		},
		hosts: {},
	};
	for (let name, spec in config_editor.named_args) {
		let val = named[name];
		if (val == null)
			continue;
		name = spec.attribute ?? name;
		network.config[name] = val;
	}

	let pubkey = network_get_pubkey(pw_file, network);

	let hostkey_file = mkstemp();
	if (system(`unet-tool -G >&${hostkey_file.fileno()}`))
		return ctx.command_failed("Failed to generate host key");

	hostkey_file.seek();
	let host_pubkey_file = mkstemp();
	if (system(`unet-tool -H -K - <&${hostkey_file.fileno()} >&${host_pubkey_file.fileno()}`))
		return ctx.command_failed("Failed to generate host public key");

	let host_key = network_get_file_string(hostkey_file);
	let host_pubkey = network_get_file_string(host_pubkey_file);
	network.config.id = pubkey;

	network.hosts[named.host] = {
		key: host_pubkey,
	};

	if (!network_sign_data(ctx, named.network, network, pw_file))
		return;

	network_create_uci(ctx.model, named.network, {
		proto: "unet",
		metric: named.metric,
		zone: named.zone,
		domain: named.domain,
		key: host_key,
		auth_key: pubkey,
		local_network: named["local-network"],
		connect: named["connect"],
	});

	return ctx.ok("Created network "+ named.network);
}

function network_delete(ctx, argv) {
	let name = argv[0];
	let cur = uci.cursor(null, null, "");
	model.run_hook("unet_delete", name);
	if (!cur.delete("network", name))
		return ctx.command_failed("Command failed");

	cur.commit();
	system("reload_config");
	return ctx.ok("Network deleted");
}

function network_iface_save(ctx)
{
	let netdata = ctx.data.netdata;
	let network = ctx.data.network;
	let changed;

	if (!netdata.iface_changed)
		return;

	model.run_hook("unet_update", network, netdata.iface);
	let cur = uci.cursor(null, null, "");
	let iface_orig = cur.get_all("network", network);
	for (let name, val in netdata.iface) {
		if (iface_orig[name] == val)
			continue;

		if (val == null)
			cur.delete("network", network, name);
		else
			cur.set("network", network, name, val);
		changed = true;
	}

	if (changed)
		cur.commit();

	netdata.iface_changed = false;

	return changed;
}

function network_apply(ctx, argv, named)
{
	let name = ctx.data.network;
	let netdata = ctx.data.netdata;
	let data = netdata.json;

	if (!netdata.changed)
		return;

	let pw_file = network_fetch_password(ctx, named);
	if (!pw_file)
		return;

	let id = network_get_pubkey(pw_file, data);
	if (id != data.config.id) {
		pw_file.close();
		return ctx.invalid_argument("Invalid password");
	}

	if (!network_sign_data(ctx, name, data, pw_file, true))
		return;

	netdata.changed = false;
	return true;
}

function __network_enroll_cancel(model, ctx)
{
	let req = ctx.data.enroll;
	if (!req)
		return false;

	req.sub.remove();
	model.ubus.call("unetd", "enroll_stop");
	delete ctx.data.enroll;
	return true;
}

function network_enroll_accept(ctx, argv, named)
{
	let req = ctx.data.enroll;
	let id = argv[0];
	if (!req || !id)
		return ctx.invalid_argument();

	let peer = req.peers[id];
	if (!peer)
		return ctx.invalid_argument("Session not found: %s", id);

	model.ubus.call("unetd", "enroll_accept", {
		session: id
	});

	return ctx.ok("Network peer accepted");
}

function network_handle_enroll_update(model, ctx, msg)
{
	let invite = ctx.data.enroll;
	if (!invite)
		return;

	let data = msg.data;
	let peer = invite.peers[data.session];
	let ret;

	if (!peer)
		model.status_msg("New device detected at " + data.address + ", session id " + data.session);

	peer ??= {};
	if (data.accepted && !peer.accepted)
		model.status_msg("Accepted peer at " + data.address + ", session id " + data.session);
	if (!data.accepted)
		data.confirmed = false;

	if (data.confirmed && !peer.confirmed) {
		model.status_msg("Confirmed peer at " + data.address + ", session id " + data.session);
		ret = data;
	}

	invite.peers[data.session] = data;

	return ret;
}

function network_invite_peer_update(model, ctx, msg)
{
	let name = ctx.data.network;
	let netdata = ctx.data.netdata;
	let invite = ctx.data.enroll;
	if (!invite)
		return;

	let data = network_handle_enroll_update(model, ctx, msg);
	if (!data)
		return;

	netdata.json.hosts[invite.name] ??= {};
	netdata.json.hosts[invite.name].key = data.enroll_key;
	netdata.changed = true;

	let pw_file = network_get_string_file(netdata.password);
	if (network_sign_data(ctx, name, netdata.json, pw_file, true)) {
		netdata.changed = false;
		model.status_msg("Updated configuration");
	}

	__network_enroll_cancel(model, ctx);
}

function network_invite(ctx, argv, named)
{
	let network = ctx.data.network;
	let netdata = ctx.data.netdata;
	let data = netdata.json;

	let pw_file = network_fetch_password(ctx, named);
	if (!pw_file)
		return;

	let id = network_get_pubkey(pw_file, data);
	pw_file.close();
	if (id != data.config.id)
		return ctx.invalid_argument("Invalid password");

	netdata.password = named.password;
	let invite = {
		name: argv[0],
		peers: {},
	};

	invite.sub = model.ubus.subscriber((msg) => {
		if (msg.type == "enroll_peer_update")
			network_invite_peer_update(ctx.model, ctx, msg);
		else if (msg.type == "enroll_timeout")
			__network_enroll_cancel(ctx.model, ctx);
	});

	let req = {
		network,
		timeout: named.timeout,
	};

	if (named["access-key"]) {
		req.enroll_secret = named["access-key"];
		req.enroll_auto = true;
	}

	if (named.connect)
		req.connect = named.connect;

	invite.sub.subscribe("unetd");
	model.ubus.call("unetd", "enroll_start", req);
	ctx.data.enroll = invite;

	return ctx.ok("Invite started");
}

function network_join_peer_update(model, ctx, msg)
{
	let joinreq = ctx.data.enroll;
	let name = joinreq.name;

	let data = network_handle_enroll_update(model, ctx, msg);
	if (!data)
		return;

	let iface = {
		proto: "unet",
		metric: joinreq.metric,
		zone: joinreq.zone,
		domain: joinreq.domain,
		connect: joinreq.connect,
		local_network: joinreq.local_network,
		key: data.local_key,
		auth_key: data.enroll_key,
	};

	if (joinreq.connect)
		iface.connect = joinreq.connect;

	network_create_uci(model, name, iface);

	model.status_msg("Configuration added for interface " + name);

	__network_enroll_cancel(model, ctx);
}

function resolve_network_broadcast_addr(list, net)
{
	let data = model.ubus.call("network.interface." + net, "status");
	if (!data)
		return;

	let dev = data.l3_device;
	if (!dev)
		return;

	let req = rtnl.request(rtnl.const.RTM_GETADDR, rtnl.const.NLM_F_DUMP);
	for (let addr in req)
		if (addr.family == 2 && addr.dev == dev && addr.broadcast)
			push(list, addr.broadcast);
}

function network_join(ctx, argv, named)
{
	__network_enroll_cancel(model, ctx);
	ctx.apply_defaults();

	let data = {
		name: named.network,
		metric: named.metric,
		zone: named.zone,
		domain: named.domain,
		connect: named.connect,
		local_network: named["local-network"],
		peers: {},
	};

	let req = {
		timeout: named.timeout,
	};

	if (named["access-key"]) {
		req.enroll_secret = named["access-key"];
		req.enroll_auto = true;
	}

	if (data.connect)
		req.connect = [ ...data.connect ];
	if (length(data.local_network) > 0) {
		req.connect ??= [];
		for (let net in data.local_network)
			resolve_network_broadcast_addr(req.connect, net);
	}

	data.sub = model.ubus.subscriber((msg) => {
		if (msg.type == "enroll_peer_update")
			network_join_peer_update(ctx.model, ctx, msg);
		else if (msg.type == "enroll_timeout")
			__network_enroll_cancel(ctx.model, ctx);
	});
	data.sub.subscribe("unetd");
	model.ubus.call("unetd", "enroll_start", req);

	ctx.data.enroll = data;

	return ctx.ok("Join request started");
}

function network_edit_exit_hook()
{
	let ctx = this;
	let netdata = ctx.data.netdata;

	network_iface_save(ctx);
	__network_enroll_cancel(model, ctx);
	if (!netdata.changed)
		return true;

	if (!model.cb.poll_key)
		return true;

	let key = model.poll_key(['c', 'r', 'a'], `You have uncommitted changes. [a]pply, [r]evert or [c]ancel? `);
	if (!key)
		return true;

	switch (key) {
	case 'c':
		warn("cancel\n");
		return false;
	case 'r':
		warn("revert\n");
		return true;
	case 'a':
		warn("apply\n");
		break;
	}

	let name = ctx.data.network;
	let data = netdata.json;

	let pw_file = network_fetch_password(ctx, {});
	if (!pw_file)
		return;

	let id = network_get_pubkey(pw_file, data);
	if (id != data.config.id) {
		warn("Invalid password\n");
		return false;
	}

	if (!network_sign_data(ctx, name, data, pw_file, true)) {
		warn("Failed to apply network configuration\n");
		return false;
	}

	return true;
}


function network_set_password(ctx, argv, named)
{
	let netdata = ctx.data.netdata;
	let network = netdata.json;

	let pw_file = network_fetch_password(ctx, named);
	if (!pw_file)
		return;

	let salt = network_generate_salt();
	if (!salt)
		return ctx.unknown_error();

	let rounds = 10000;
	let config = { ...network.config, salt };

	let key = network_keygen(pw_file, '-G', network.config);
	pw_file.close();

	named.password = named["new-password"];
	pw_file = network_fetch_password(ctx, named, true);
	if (!pw_file)
		return;

	let key_file = network_get_string_file(key);
	delete config.xorkey;
	config.xorkey = network_keygen(pw_file, '-G -x /dev/fd/' + key_file.fileno(), config);
	key_file.close();

	if (!config.xorkey) {
		delete named.password;
		return ctx.unknown_error("Error generating key");
	}

	network.config = config;
	netdata.changed = true;
	netdata.password = named.password;

	return ctx.ok();
}

function network_edit(ctx, argv) {
	let network = argv[0];
	if (!network) {
		network = "unet";
		if (!get_network_status()[network])
			return ctx.invalid_argument('no valid network name provided');
	}

	let iface_data = uci.cursor().get_all("network", network);
	for (let name in keys(iface_data))
		if (substr(name, 0, 1) == ".")
			delete iface_data[name];

	let json_file = mkstemp();
	if (system(`unet-tool -T -b /etc/unetd/${network}.bin >&${json_file.fileno()}`))
		return;

	let json_data;
	try {
		json_data = network_get_file_string(json_file);
		json_data = json(json_data);
	} catch (e) {
		json_data = null;
	}

	if (!json_data)
		return;

	let netdata = {
		json: json_data,
		iface: iface_data,
		changed: false,
	};

	json_data.hosts ??= {};
	json_data.services ??= {};

	ctx.add_hook("exit", network_edit_exit_hook);

	return ctx.set('edit "' + network + '"', {
		network, netdata,
		object_edit: json_data,
	});
}

const network_args = [
	{
		name: "network",
		help: "Network name",
		type: "enum",
		value: () => get_networks(),
		required: true,
	}
];

const network_status_args = [
	{
		name: "network",
		help: "Network name",
		type: "enum",
		value: () => keys(get_network_status())
	}
];

const network_password_arg = {
	password: {
		help: "Network configuration password",
		no_complete: true,
		args: {
			type: "string",
			min: 12,
		}
	},
};

const network_new_password_arg = {
	"new-password": {
		help: "New network configuration password",
		no_complete: true,
		args: {
			type: "string",
			min: 12,
		}
	},
};

const network_config_args = editor.object_create_params(UnetConfigEdit);

const network_create_args = {
	...network_password_arg,
	...network_config_args,
	...network_local_args,
	host: {
		help: "local host name",
		default: "main",
		required: true,
		args: {
			type: "string",
		}
	},
};

const network_invite_name_arg = [
	{
		name: "name",
		help: "Name of the invited device",
		type: "string",
	}
];

const network_enroll_args = {
	"access-key": {
		help: "Access key for allowing the device into the network",
		args: {
			type: "string",
		}
	},
	timeout: {
		help: "Timeout for invite",
		required: true,
		default: 120,
		args: {
			type: "int",
		}
	},
};

const enroll_accept_arg = [{
	name: "session_id",
	help: "Session id of the network peer",
	type: "string",
	required: true,
	type: "enum",
	value: (ctx) => keys(ctx.data.enroll.peers),
}];

const network_join_args = {
	...network_enroll_args,
	...network_local_args,
};

const network_invite_args = {
	...network_enroll_args,
	...network_password_arg,
};

const host_editor = {
	change_cb: function(ctx, argv) {
		ctx.data.netdata.changed = true;
	},
	named_args: {
		name: {
			help: "Host name",
			get: (ctx) => ctx.data.name,
			set: (ctx, val) => {
				let name = ctx.data.name;
				let hosts = ctx.data.netdata.json.hosts;
				hosts[val] = hosts[name];
				delete hosts[name];
				ctx.data.name = val;
			},
			change_only: true,
			args: {
				type: "string",
			}
		},
		key: {
			help: "Wireguard key",
			required: true,
			args: {
				type: "string",
			}
		},
		port: {
			help: "Wireguard port",
			args: {
				type: "int",
				min: 1,
				max: 65535,
			}
		},
		"unet-port": {
			help: "unet protocol port (0: wireguard only)",
			args: {
				type: "int",
				min: 0,
				max: 65535,
			}
		},
		endpoint: {
			help: "Wireguard endpoint IP address",
			args: {
				type: "string",
			}
		},
		ipaddr: {
			help: "IP address",
			multiple: true,
			args: {
				type: "ipv4",
			}
		},
		subnet: {
			help: "IP subnet",
			multiple: true,
			args: {
				type: "cidr4",
			}
		},
		gateway: {
			help: "Other host to be used as gateway",
			args: {
				type: "enum",
				value: function(ctx, argv) {
					return filter(keys(ctx.data.netdata.json.hosts),
					              (v) => v != ctx.data.name);
				}
			}
		},
		group: {
			help: "Host group membership",
			attribute: "groups",
			multiple: true,
			args: {
				type: "enum",
				no_validate: true,
				value: function(ctx) {
					let groups = {};
					for (let name, host in ctx.data.netdata.json.hosts)
						for (let group in host.groups)
							groups[group] = true;
					return keys(groups);
				}
			}
		}
	},
};

const UnetHostEdit = editor.new(host_editor);

function has_service_type(ctx, named, name)
{
	let type = named.type;
	if (ctx.data.edit)
		type ??= ctx.data.edit.type;

	return type == name;
}

function is_vxlan_service(ctx, argv, named, spec)
{
	return has_service_type(ctx, named, "vxlan");
}

function is_unetmsg_service(ctx, argv, named, spec)
{
	return has_service_type(ctx, named, "unetmsg");
}

function get_config_object(ctx, spec, obj, argv)
{
	obj.config ??= {};
	return obj.config;
}

const service_editor = {
	change_cb: function(ctx, argv) {
		ctx.data.netdata.changed = true;
	},
	named_args: {
		type: {
			help: "Service type",
			required: true,
			args: {
				type: "enum",
				no_validate: true,
				value: supported_service_types,
			}
		},
		member: {
			help: "Service member",
			attribute: "members",
			multiple: true,
			args: {
				type: "enum",
				value: (ctx) => [ "@all", ...keys(ctx.data.netdata.json.hosts) ]
			}
		},
		"vxlan-id": {
			help: "VXLAN ID",
			attribute: "id",
			available: is_vxlan_service,
			get_object: get_config_object,
			args: {
				type: "int",
				min: 0,
				max: (1 << 24) - 1,
			}
		},
		"vxlan-port": {
			help: "VXLAN port",
			attribute: "port",
			available: is_vxlan_service,
			get_object: get_config_object,
			args: {
				type: "int",
				min: 1,
				max: 65535,
			}
		},
		"vxlan-mtu": {
			help: "VXLAN tunnel MTU",
			attribute: "mtu",
			available: is_vxlan_service,
			get_object: get_config_object,
			args: {
				type: "int",
				min: 1280,
				max: 9000,
			}
		},
		"vxlan-forwarding-port": {
			help: "Member allowed to receive broad-/multicast and unknown unicast",
			attribute: "forward_ports",
			available: is_vxlan_service,
			get_object: get_config_object,
			multiple: true,
			args: {
				type: "enum",
				value: (ctx) => keys(ctx.data.netdata.json.hosts)
			}
		},
		"unetmsg-allowed": {
			help: "Allowed topics for this unetmsg service group",
			attribute: "allowed",
			available: is_unetmsg_service,
			get_object: get_config_object,
			multiple: true,
			args: {
				type: "string"
			}
		},
	}
};

const UnetServiceEdit = editor.new(service_editor);

const edit_create_destroy = {
	change_cb: function(ctx, argv) {
		ctx.data.netdata.changed = true;
	},
	types: {
		host: {
			node_name: "UnetHostEdit",
			node: UnetHostEdit,
			object: "hosts",
		},
		service: {
			node_name: "UnetServiceEdit",
			node: UnetServiceEdit,
			object: "services",
		},
	},
};

let UnetEdit = {
	config: {
		help: "Edit network global configuration",
		select_node: "UnetConfigEdit",
		select: function(ctx) {
			return ctx.set("config", {
				edit: ctx.data.object_edit.config,
			});
		}
	},
	iface: {
		help: "Edit interface configuration",
		select_node: "UnetIfaceEdit",
		select: function(ctx) {
			return ctx.set("iface", {
				edit: ctx.data.netdata.iface,
			});
		}
	},
	accept: {
		help: "Accept invited network peer",
		args: enroll_accept_arg,
		available: (ctx) => ctx.data.enroll && length(ctx.data.enroll.peers) > 0,
		call: network_enroll_accept,
	},
	invite: {
		help: "Invite another device to the network",
		args: network_invite_name_arg,
		named_args: network_invite_args,
		call: network_invite,
	},
	cancel: {
		help: "Cancel device invitation",
		available: (ctx) => ctx.data.enroll,
		call: function(ctx) {
			__network_enroll_cancel(model, ctx);
			return ctx.ok("Invitation cancelled");
		}
	},
	dump: {
		help: "Show network json data",
		call: function(ctx) {
			return ctx.json("Network data", ctx.data.netdata.json);
		}
	},
	password: {
		help: "Edit network password",
		call: network_set_password,
		named_args: {
			...network_password_arg,
			...network_new_password_arg
		}
	},
	save: {
		help: "Save network data to json file",
		args: [
			{
				name: "file",
				help: "Destination path",
				type: "path",
				required: true,
				new_path: true,
			},
		],
		call: function(ctx, argv) {
			if (!writefile(argv[0], sprintf("%.J\n", ctx.data.netdata.json)))
				return ctx.command_failed("Could not write to %s", argv[0]);

			return ctx.ok("Configuration saved to "+argv[0]);
		}
	},
	restore: {
		help: "Restore network data from json file",
		args: [
			{
				name: "file",
				help: "Source path",
				type: "path",
				required: true,
			},
		],
		call: function(ctx, argv) {
			let config, data;
			try {
				data = json(readfile(argv[0]));
				config = data.config;
			} catch (e) {
				return ctx.command_failed("Could not read JSON data from %s", argv[0]);
			}

			if (!config)
				return ctx.command_failed("Invalid network json file");

			let json = ctx.data.netdata.json;
			let prev_config = {};
			for (let field in [ "salt", "rounds", "id" ]) {
				prev_config[field] = json.config[field];
				delete config[field];
			}

			ctx.data.netdata.changed = true;
			data.config = { ...prev_config, ...config };
			ctx.data.netdata.json = data;

			return ctx.ok("Configuration restored from "+argv[0]);
		}
	},
	apply: {
		help: "Apply changes",
		named_args: network_password_arg,
		call: function(ctx, argv, named) {
			let netdata = ctx.data.netdata;

			let changed = network_iface_save(ctx);
			if (network_apply(ctx, argv, named))
				changed = true;

			if (!changed)
				return ctx.ok("No changes");

			return ctx.ok("Changes applied");
		}
	}
};
editor.edit_create_destroy(edit_create_destroy, UnetEdit);

const Unet = {
	status: {
		help: "Show unet network information",
		args: network_status_args,
		call: function(ctx, argv) {
			let name = argv[0];
			let status = get_network_status();
			if (!status)
				return ctx.command_failed();

			if (!name)
				return ctx.list("Networks", keys(status));

			status = status[name];
			if (!status)
				return ctx.not_found();

			let data = {};
			for (let name, host in status.peers) {
				let cur = [];

				let key = name;
				if (model.cb.opt_pretty_print) {
					data[`Host '${name}'`] = cur;
					push(cur, [ "State", host.connected ? "connected" : "disconnected" ]);
					if (!host.connected)
						continue;

					if (host.endpoint)
						push(cur, [ "IP address", host.endpoint ]);

					push(cur, [ "Idle time", time_format(host.idle) ]);
					push(cur, [ "Sent bytes", host.tx_bytes ]);
					push(cur, [ "Received bytes", host.rx_bytes ]);
					push(cur, [ "Last handshake", time_format(host.last_handshake_sec) + " ago" ]);
				} else {
					data[name] = host;
				}
			}
			return ctx.multi_table("Status of network " + name, data);
		}
	},
	join: {
		help: "Join existing network",
		named_args: network_join_args,
		call: network_join,
	},
	accept: {
		help: "Accept network peer",
		args: enroll_accept_arg,
		available: (ctx) => ctx.data.enroll && length(ctx.data.enroll.peers) > 0,
		call: network_enroll_accept,
	},
	cancel: {
		help: "Cancel join request",
		available: (ctx) => ctx.data.enroll,
		call: function(ctx) {
			__network_enroll_cancel(model, ctx);
			return ctx.ok("Join request cancelled");
		},
	},
	create: {
		help: "Create network",
		named_args: network_create_args,
		call: network_create,
	},
	delete: {
		help: "Delete network",
		args: network_args,
		call: network_delete,
	},
	edit: {
		help: "Edit network",
		args: network_status_args,
		no_subcommands: true,
		select_node: "UnetEdit",
		select: network_edit,
	},
};

const Root = {
	unet: {
		help: "unetd network management",
		select_node: "Unet",
	}
};

model.add_nodes({ Root, Unet, UnetEdit, UnetConfigEdit, UnetIfaceEdit, UnetHostEdit, UnetServiceEdit });
