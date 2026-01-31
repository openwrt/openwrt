// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
'use strict';
import * as client from "./unetmsgd-client.uc";
import * as remote from "./unetmsgd-remote.uc";
import { gen_id } from "./utils.uc";

function __acl_check(list, name)
{
	for (let cur in list)
		if (wildcard(name, cur, true))
			return true;
}

function acl_check(acl_type, info, names)
{
	let acl = this.acl;

	if (info.user == "root")
		return true;

	let list = acl[acl_type][info.user] ?? [];
	if (info.group) {
		let list2 = acl[acl_type][":" + info.group];
		if (list2)
			list = [ ...list, ...list2 ];
	}

	for (let name in names)
		if (!__acl_check(list, name))
			return;

	return true;
}

function new_handle(list, name, data)
{
	let id = gen_id();
	data._id = id;
	list[name] ??= {};
	list[name][id] = data;
	return data;
}

function pubsub_add(kind, name, data)
{
	let list = this[kind];
	if (!length(list[name]) || kind == "publish") {
		list[name] ??= {};
		remote.pubsub_set(kind, name, true);
	}
	return new_handle(this[kind], name, data);
}

function pubsub_del(kind, name, data)
{
	let list = this[kind][name];
	delete list[data._id];
	if (!length(list) || kind == "publish")
		remote.pubsub_set(kind, name, length(list) > 0);
}

function get_handles(handle, local, remote, host)
{
	let handles = [];

	if (host == "")
		remote = {};
	else if (host != null)
		local = {};

	for (let cur_id, cur in local) {
		if (handle) {
			if (handle.id == cur_id)
				continue;
			if (handle.client && handle.client == cur.client)
				continue;
		}

		push(handles, cur);
	}

	if (!remote)
		return handles;

	for (let cur_id, cur in remote) {
		if (host != null && cur.name != host)
			continue;
		push(handles, cur);
	}

	return handles;
}

function handle_request(handle, req, data, remote, host)
{
	let name = data.name;
	let local = this.publish[name];
	if (remote)
		remote = this.remote_publish[name];
	let handles = get_handles(handle, local, remote, host);

	let context = {
		pending: length(handles),
		req
	};

	if (!context.pending)
		return 0;

	req.defer();
	let cb = (ret) => {
		if (--context.pending > 0)
			return;
		req.reply();
	};

	for (let cur in handles) {
		if (!cur || !cur.get_channel) {
			cb();
			continue;
		}
		let chan = cur.get_channel();
		if (!chan) {
			cb();
			continue;
		}

		let cur_handle = cur;
		let data_cb = (msg) => {
			if (cur_handle.get_response_data)
				msg = cur_handle.get_response_data(msg);
			req.reply(msg, -1);
		};

		chan.defer({
			method: "request",
			data, cb, data_cb
		});
	}
}

function handle_message(handle, data, remote, host)
{
	let name = data.name;
	let local = this.subscribe[name];
	if (remote)
		remote = this.remote_subscribe[name];
	let handles = get_handles(handle, local, remote, host);
	for (let cur in handles) {
		if (!cur || !cur.get_channel)
			continue;

		let chan = cur.get_channel();
		if (!chan)
			continue;

		chan.request({
			method: "message",
			return: "ignore",
			data,
		});
	}
	return 0;
}

function handle_publish(handle, name)
{
	let local = this.subscribe[name];
	let handles = get_handles(handle, local);

	for (let cur in handles) {
		if (!cur || !cur.get_channel)
			continue;

		let chan = cur.get_channel();
		if (!chan)
			continue;

		chan.request({
			method: "publish",
			return: "ignore",
			data: { name },
		});
	}
}

function add_acl(type, user, data)
{
	if (!data || !user)
		return;

	type[user] ??= [];
	let list = type[user];
	for (let cur in data)
		if (index(list, data) < 0)
			push(list, cur);
}

function acl_set(acl_data)
{
	let acl = this.acl = {
		publish: {},
		subscribe: {},
	};

	for (let cur in acl_data) {
		if (cur.obj != "unetmsg" || !cur.acl)
			continue;

		if (cur.group)
			cur.group = ":" + cur.group;

		for (let user in [ cur.user, cur.group ]) {
			add_acl(acl.publish, user, cur.acl.publish);
			add_acl(acl.subscribe, user, cur.acl.publish);
			add_acl(acl.subscribe, user, cur.acl.subscribe);
		}
	}
};

const core_proto = {
	acl_check,
	acl_set,
	pubsub_add,
	pubsub_del,
	handle_request,
	handle_message,
	handle_publish,
	dbg: function(msg) {
		if (this.debug_enabled)
			warn(msg);
	},
	exception: function(e) {
		this.dbg(`Exception: ${e}\n${e.stacktrace[0].context}`);
	}
};

export function init(ubus, debug_enabled)
{
	let data = proto({
		clients: {},
		publish: {},
		subscribe: {},
		remote_publish: {},
		remote_subscribe: {},
		client,
		remote,
		ubus,
		debug_enabled
	}, core_proto);

	client.set_core(data);
	remote.init(data);

	return data;
};
