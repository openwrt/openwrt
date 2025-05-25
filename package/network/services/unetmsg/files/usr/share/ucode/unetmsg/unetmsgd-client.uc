// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
'use strict';
import * as libubus from "ubus";
import { gen_id } from "./utils.uc";

let core;
let clients = {};

const pubsub_proto = {
	get_channel: function() {
		let cl = clients[this.client];
		if (!cl)
			return;

		return cl.chan;
	}
};

function client_pubsub(kind, cl, names)
{
	if (type(names) != "array")
		return libubus.STATUS_INVALID_ARGUMENT;

	for (let cur in names) {
		if (type(cur) != "string")
			return libubus.STATUS_INVALID_ARGUMENT;
	}

	if (!core.acl_check(kind, cl.acl, names))
		return libubus.STATUS_PERMISSION_DENIED;

	let cl_list = cl[kind];
	for (let name in names) {
		if (cl_list[name])
			continue;

		cl_list[name] = core.pubsub_add(kind, name, proto({
			client: cl.id,
		}, pubsub_proto));

		if (kind == "publish")
			core.handle_publish(cl_list[name], name);
	}

	return 0;
}

function prepare_data(args)
{
	return {
		name: args.name,
		type: args.type,
		data: args.data,
	};
}

function client_request(cl, req)
{
	let args = req.args;
	let name = args.name;

	if (type(name) != "string" || type(args.type) != "string" || type(args.data) != "object")
		return libubus.STATUS_INVALID_ARGUMENT;

	let data = prepare_data(args);
	let handle;
	switch (req.type) {
	case "message":
		handle = cl.publish[name];
	    if (!handle)
			return libubus.STATUS_INVALID_ARGUMENT;
		return core.handle_message(handle, data, true, args.host);
	case "request":
		handle = cl.subscribe[name];
		if (!handle &&
		    !core.acl_check("subscribe", cl.acl, [ name ]))
			return libubus.STATUS_PERMISSION_DENIED;

		handle ??= { client: cl.id };
		return core.handle_request(handle, req, data, true, args.host);
	}
}

function client_cb(cl, req)
{
	let args = req.args;
	switch (req.type) {
	case "publish":
	case "subscribe":
		return client_pubsub(req.type, cl, args.name);
	case "message":
	case "request":
		return client_request(cl, req);
	}
}

function client_disconnect(id)
{
	let cl = clients[id];
	if (!cl)
		return;

	for (let kind in [ "publish", "subscribe" ])
		for (let name, data in cl[kind]) {
			if (kind == "publish")
				core.handle_publish(data, name);
			core.pubsub_del(kind, name, data);
		}

	delete clients[id];
}

export function new(req)
{
	let id = gen_id();
	let acl = req.info.acl;
	let client = {
		id, acl,
		publish: {},
		subscribe: {},
	};
	let cb = (req) => client_cb(client, req);
	let disconnect_cb = () => client_disconnect(id);
	client.chan = req.new_channel(cb, disconnect_cb);
	clients[id] = client;

	return client;
};

export function set_core(_core)
{
	core = _core;
};
