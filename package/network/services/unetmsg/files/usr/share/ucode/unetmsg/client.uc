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

function subscribe(name, message_cb, update_cb)
{
	if (!this.channel)
		this.connect();

	if (type(name) == "string")
		name = [ name ];

	let cb = {
		cb: message_cb,
		update: update_cb
	};
	for (let cur in name)
		this.cb_sub[cur] = cb;

	if (!this.channel)
		return;

	this.channel.request("subscribe", { name });
}

function send_ext(data)
{
	this.channel.request({
		method: "message",
		return: "ignore",
		data
	});
}

function send_host(host, name, type, data)
{
	this.send_ext({
		host, name, type, data
	});
}

function send(name, type, data)
{
	this.send_ext({
		name, type, data
	});
}

function default_complete_cb()
{
}

function request_ext(data, data_cb, complete_cb)
{
	if (!this.channel)
		this.connect();

	if (!this.channel)
		return;

	let req = this.channel.defer({
		method: "request",
		data,
		data_cb,
		cb: complete_cb
	});

	if (complete_cb)
		return req;

	req.await();
}

function request_host(host, name, type, data, data_cb, complete_cb)
{
	return this.request_ext({
		host, name, type, data
	}, data_cb, complete_cb);
}

function request(name, type, data, data_cb, complete_cb)
{
	return this.request_ext({
		name, type, data
	}, data_cb, complete_cb);
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
	connect, publish, subscribe,
	send, send_ext, send_host,
	request, request_ext, request_host,
	close: function() {
		for (let sub in this.sub_cb) {
			if (!sub.timer)
				continue;
			sub.timer.cancel();
			delete sub.timer;
		}
		if (this.channel)
			this.channel.disconnect();
		this.connect_timer.cancel();
		for (let name in keys(this))
			delete this[name];
	}
};

function handle_request(cl, req)
{
	let data, cb;

	switch (req.type) {
	case "publish":
		data = cl.cb_sub[req.args.name];
		if (!data || data.timer)
			break;

		cb = data.update;
		if (!cb)
			return;

		data.timer = uloop.timer(100, () => {
			delete data.timer;
			cb();
		});
		break;
	case "message":
		data = cl.cb_sub[req.args.name];
		if (!data)
			break;

		cb = data.cb;
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
