// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
'use strict';
import * as libubus from "ubus";
import * as uloop from "uloop";
import * as socket from "socket";
import { gen_id, is_equal } from "./utils.uc";

let core, ubus;
let local_id = gen_id();
let ev_listener, sub;

let networks = {};

const USYNC_PORT = 51818;

const pubsub_proto = {
	get_channel: function() {
		let net = networks[this.network];
		if (!net)
			return;

		let sock_data = net.tx_channels[this.name];
		if (!sock_data)
			return;

		return sock_data.channel;
	},
	get_response_data: function(data) {
		data.network = this.network,
		data.host = this.name;
		return data;
	}
};

function network_socket_close(data)
{
	if (!data)
		return;

	if (data.timer)
		data.timer.cancel();
	data.channel.disconnect();
	data.socket.close();
}

function network_rx_socket_close(data)
{
	if (!data)
		return;

	core.dbg(`Incoming connection from ${data.name} closed\n`);
	let net = networks[data.network];
	if (net && net.rx_channels[data.name] == data)
		delete net.rx_channels[data.name];

	for (let name, sub in core.remote_subscribe)
		delete sub[data.name];

	for (let name, sub in core.remote_publish)
		delete sub[data.name];

	network_socket_close(data);
}

function network_tx_socket_close(data)
{
	if (!data)
		return;

	core.dbg(`Outgoing connection to ${data.name} closed\n`);
	let net = networks[data.network];
	if (net && net.tx_channels[data.name] == data)
		delete net.tx_channels[data.name];

	network_socket_close(data);
}

function network_socket_handle_request(sock_data, req)
{
	let net = networks[sock_data.network];
	if (!net)
		return;

	let msgtype = req.type;
	let host = sock_data.name;
	let network = sock_data.network;
	let args = { ...req.args, host, network };
	switch (msgtype) {
	case "publish":
	case "subscribe":
		let list = sock_data[msgtype];
		let name = args.name;
		if (!name)
			return;
		if (args.enabled) {
			if (list[name]) {
				core.handle_publish(null, name);
				return 0;
			}

			let allowed = net.peers[host].allowed == null;
			for (let cur in net.peers[host].allowed) {
				if (!wildcard(name, cur))
					continue;
				allowed = true;
				break;
			}
			if (!allowed)
				return 0;

			core["remote_" + msgtype][name] ??= {};
			core["remote_" + msgtype][name][host] = proto({
				network: sock_data.network,
				name: host,
			}, pubsub_proto);
			core.handle_publish(null, name);
			list[name] = true;
		} else {
			if (!list[name])
				return 0;
			core.handle_publish(null, name);
			delete core["remote_" + msgtype][name][host];
			delete list[name];
		}
		break;
	case "request":
		return core.handle_request(null, req, args);
	case "message":
		core.handle_message(null, args);
		return 0;
	}

	return 0;
}

function network_auth_token(net, host, id)
{
	let auth_data = ubus.call("unetd", "token_create", {
		network: net,
		target: host,
		data: { id }
	});

	if (!auth_data)
		return;

	return auth_data.token;
}

function network_auth_valid(host, id, token)
{
	if (!token)
		return;

	let data = ubus.call("unetd", "token_parse", { token });
	if (!data)
		return;

	if (data.host != host)
		return;

	if (data.user != "root")
		return;

	data = data.data;
	if (data.id != id)
		return;

	return true;
}


function network_check_auth(sock_data, info)
{
	if (!network_auth_valid(sock_data.name, sock_data.id, info.token))
		return;

	let net = networks[sock_data.network];
	if (!net)
		return;

	if (!net.peers[sock_data.name])
		return;

	network_rx_socket_close(net.rx_channels[sock_data.name]);
	if (sock_data.timer)
		sock_data.timer.cancel();
	sock_data.auth = true;
	net.rx_channels[sock_data.name] = sock_data;
	core.dbg(`Incoming connection from ${sock_data.name} established\n`);
	if (!net.tx_channels[sock_data.name])
		net.timer.set(100);
}

function network_accept(net, sock, addr)
{
	let src = addr.address;
	let name;

	for (let cur_name, data in net.peers)
		if (data.address == src)
			name = cur_name;

	if (!name) {
		core.dbg(`No peer found for address ${src}\n`);
		sock.close();
		return;
	}

	let sock_data = {
		network: net.name,
		socket: sock,
		publish: {},
		subscribe: {},
		name,
	};

	let cb = (req) => {
		if (!sock_data.auth) {
			if (req.type == "hello") {
				sock_data.id = req.args.id;
				return;
			}
			if (req.type == "auth")
				network_check_auth(sock_data, req.args);

			if (!sock_data.auth) {
				warn(`Auth failed\n`);
				network_rx_socket_close(sock_data);
				return 0;
			}

			let token = network_auth_token(net.name, name, req.args.id);
			if (!token) {
				warn(`Failed to generate auth reply token\n`);
				return 0;
			}

			req.reply({ token }, -1);

			return 0;
		}

		return network_socket_handle_request(sock_data, req);
	};

	let disconnect_cb = (req) => {
		network_rx_socket_close(sock_data);
	};

	sock_data.id = gen_id();
	sock_data.timer = uloop.timer(10 * 1000, () => {
		network_socket_close(sock_data);
	});
	sock_data.channel = libubus.open_channel(sock, cb, disconnect_cb);
	sock_data.channel.request({
		method: "hello",
		data: { id: sock_data.id },
		return: "ignore",
	});
}

function network_open_channel(net, name, peer)
{
	network_tx_socket_close(net.tx_channels[name]);

	let sock_data = {
		network: net.name,
		name
	};

	let addr = socket.sockaddr({
		address: peer.address,
		port: USYNC_PORT
	});
	if (!addr)
		return;

	let sock = socket.create(socket.AF_INET6, socket.SOCK_STREAM | socket.SOCK_NONBLOCK);
	if (!sock)
		return;

	core.dbg(`Try to connect to ${name}\n`);
	sock.connect(addr);
	let auth_data_cb = (msg) => {
		if (!network_auth_valid(sock_data.name, sock_data.id, msg.token))
			return;

		sock_data.auth = true;
		core.dbg(`Outgoing connection to ${name} established\n`);

		for (let kind in [ "publish", "subscribe" ])
			for (let name in core[kind])
				sock_data.channel.request({
					method: kind,
					data: { name, enabled: true },
					return: "ignore",
				});
	};
	let auth_cb = () => {
		if (!sock_data.auth)
			network_tx_socket_close(sock_data);
	};

	let cb = (req) => {
		if (sock_data.auth)
			return 0;

		if (req.type != "hello") {
			network_tx_socket_close(sock_data);
			return 0;
		}

		let token = network_auth_token(net.name, name, req.args.id);
		if (!token) {
			network_tx_socket_close(sock_data);
			return 0;
		}

		sock_data.request = sock_data.channel.defer({
			method: "auth",
			data: { token },
			data_cb: auth_data_cb,
			cb: auth_cb,
		});

		return 0;
	};

	let disconnect_cb = (req) => {
		let net = networks[sock_data.network];
		let cur_data = net.tx_channels[sock_data.name];
		if (cur_data == sock_data)
			delete net.rx_channels[sock_data.name];

		network_tx_socket_close(sock_data);
	};

	sock_data.socket = sock;
	sock_data.channel = libubus.open_channel(sock, cb, disconnect_cb);
	net.tx_channels[name] = sock_data;
}

function network_connect_peers(net)
{
	let n_pending = 0;

	for (let name, data in net.peers) {
		let chan = net.tx_channels[name];
		if (chan && chan.auth)
			continue;

		network_open_channel(net, name, data);
		n_pending++;
	}

	for (let name, sock_data in net.tx_channels)
		if (!net.peers[name])
			network_tx_socket_close(sock_data);

	for (let name, sock_data in net.rx_channels)
		if (!net.peers[name])
			network_rx_socket_close(sock_data);

	if (n_pending)
		net.timer.set(10 * 1000);
}

function network_open(name, info)
{
	let net = info;

	net.socket = socket.listen(net.local_address, USYNC_PORT, {
		family: socket.AF_INET6,
		socktype: socket.SOCK_STREAM,
		flags: socket.AI_NUMERICHOST,
	}, null, true);

	if (!net.socket) {
		warn(`Failed to open socket: ${socket.error()}\n`);
		return;
	}

	net.name = name;
	net.rx_channels = {};
	net.tx_channels = {};

	net.socket.setopt(socket.SOL_TCP, socket.TCP_USER_TIMEOUT, 30 * 1000);

	let cb = () => {
		let addr = {};
		let sock = net.socket.accept(addr);
		if (sock)
			network_accept(net, sock, addr);
	};

	net.handle = uloop.handle(net.socket.fileno(), cb, uloop.ULOOP_READ);
	net.timer = uloop.timer(100, () => network_connect_peers(net));

	networks[name] = net;
}

function network_close(name)
{
	let net = networks[name];
	net.timer.cancel();
	net.handle.delete();
	net.socket.close();
	delete networks[name];
}

function network_update(name, info)
{
	let net = networks[name];
	if (!net)
		return;

	if (net.local_host != info.local_host ||
	    net.local_address != info.local_address) {
		network_close(name);
		network_open(name, info);
		return;
	}

	for (let name, peer in net.peers) {
		let allowed;
		if (info.peers[name])
			allowed = info.peers[name].allowed;
		if (is_equal(peer.allowed, allowed))
			continue;
		network_rx_socket_close(net.rx_channels[name]);
		network_tx_socket_close(net.tx_channels[name]);
	}
	net.peers = info.peers;
	net.timer.set(100);
}

function unetd_network_check_peers(info)
{
	let services = [];

	for (let name, data in info.services) {
		if (data.type == "unetmsg")
			push(services, data);
	}

	if (!length(services))
		return;

	services = filter(services, (v) => index(v.members, info.local_host) >= 0);
	for (let name in keys(info.peers)) {
		let allowed = [];

		for (let data in services) {
			if (index(data.members, name) < 0)
				continue;

			let cur_allowed = [ "*" ];
			if (data.config && data.config.allowed)
				cur_allowed = data.config.allowed;

			for (let cur in cur_allowed)
				if (index(allowed, cur) < 0)
					push(allowed, cur);
		}

		if (!length(allowed))
			delete info.peers[name];
		else
			info.peers[name].allowed = allowed;
	}
}

function unetd_network_update()
{
	let data = ubus.call("unetd", "network_get");
	if (!data || !data.networks)
		return;

	for (let name, info in data.networks) {
		if (!info.local_host)
			continue;

		unetd_network_check_peers(info);

		if (networks[name])
			network_update(name, info);
		else
			network_open(name, info);
	}

	for (let name in networks)
		if (!data.networks)
			network_close(name);
}

function unetd_cb(msg)
{
	if (msg.type == "network_update")
		unetd_network_update();
	return 0;
}

export function pubsub_set(kind, name, enabled)
{
	for (let net_name, net in networks) {
		for (let host_name, chan in net.tx_channels) {
			if (!chan.auth)
				continue;

			chan.channel.request({
				method: kind,
				data: { name, enabled },
				return: "ignore",
			});
		}
	}
};

export function init(_core)
{
	core = _core;
	ubus = core.ubus;
	sub = ubus.subscriber(unetd_cb);
	unetd_network_update();
	ev_listener = ubus.listener("ubus.object.add", (event, msg) => {
		if (msg.path == "unetd")
			sub.subscribe(msg.path);
	});
	sub.subscribe("unetd");
};
