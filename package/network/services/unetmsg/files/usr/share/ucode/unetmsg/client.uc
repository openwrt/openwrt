// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
'use strict';
import * as libubus from "ubus";
import * as uloop from "uloop";

function publish(name, request_cb)
{
	if (!this.channel)
		this.connect();

	if (type(name) == "string")
		name = [ name ];

	for (let cur in name)
		this.cb_pub[cur] = request_cb;

	if (!this.channel)
		return;

	this.channel.request("publish", { name });
}

function subscribe(name, message_cb)
{
	if (!this.channel)
		this.connect();

	if (type(name) == "string")
		name = [ name ];

	for (let cur in name)
		this.cb_sub[cur] = message_cb;

	if (!this.channel)
		return;

	this.channel.request("subscribe", { name });
}

function send(name, type, data)
{
	this.channel.request({
		method: "message",
		return: "ignore",
		data: {
			name, type, data
		},
	});
}

function default_complete_cb()
{
}

function request(name, type, data, data_cb, complete_cb)
{
	if (!this.channel)
		this.connect();

	if (!this.channel)
		return;

	let req = this.channel.defer({
		method: "request",
		data: {
			name, type, data
		},
		data_cb,
		cb: complete_cb
	});

	if (!complete_cb)
		req.await();
}

function connect()
{
	if (this.channel)
		return;

	let cl = this;
	let res = cl.ubus.call({
		object: "unetmsg",
		method: "channel",
		fd_cb: (fd) => {
			cl.channel = libubus.open_channel(fd, cl.request_cb, cl.disconnect_cb, cl.timeout);
		}
	});

	if (!this.channel) {
		this.connect_timer.set(1000);
		return;
	}

	if (length(this.cb_pub) > 0)
		this.channel.request("publish", {
			name: keys(this.cb_pub)
		});

	if (length(this.cb_sub) > 0)
		this.channel.request("subscribe", {
			name: keys(this.cb_sub)
		});
}

const client_proto = {
	connect, publish, subscribe, send, request,
	close: function() {
		if (this.channel)
			this.channel.disconnect();
		this.connect_timer.cancel();
		for (let name in keys(this))
			delete this[name];
	}
};

function handle_request(cl, req)
{
	let cb;

	switch (req.type) {
	case "message":
		cb = cl.cb_sub[req.args.name];
		if (cb)
			return cb(req);
		break;
	case "request":
		cb = cl.cb_pub[req.args.name];
		if (cb)
			return cb(req);
	}
	return 0;
}

export function open(ubus_conn, timeout)
{
	let cl = proto({
		cb_sub: {},
		cb_pub: {},
		ubus: ubus_conn,
		timeout,
	}, client_proto);

	cl.request_cb = (req) => {
		return handle_request(cl, req);
	};

	cl.disconnect_cb = () => {
		cl.channel = null;
		cl.connect_timer.set(100);
	};

	cl.connect_timer = uloop.timer(1, () => cl.connect());

	return cl;
};
