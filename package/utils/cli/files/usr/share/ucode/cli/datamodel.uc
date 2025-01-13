// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

import * as context from "cli.context";
import * as cache from "cli.cache";
import * as libubus from "ubus";
import * as uloop from "uloop";
import { glob, dirname } from "fs";
let types = require("cli.types");

uloop.init();
let ubus = libubus.connect();

function status_msg(msg)
{
	if (this.cb.status_msg)
		call(this.cb.status_msg, this, this.scope, msg);
}

function poll_key(keys, prompt)
{
	if (!model.cb.poll_key)
		return;

	if (prompt)
		warn(prompt);

    while (1) {
        let key = lc(model.cb.poll_key());
		if (!key || key == "\x03")
			return;

		if (index(keys, key) >= 0)
			return key;
	}
}

function merge_object(obj, add)
{
	for (let name, entry in add)
		obj[name] = entry;
}

function add_node(name, node)
{
	let obj = this.node;

	if (obj[name])
		merge_object(obj[name], node);
	else
		obj[name] = { ...node };

	return obj[name];
}

function add_nodes(add)
{
	for (let name, val in add)
		this.add_node(name, val);
}

function add_hook(name, val)
{
	let obj = this.hooks;

	if (type(val) == "function")
		val = [ val ];
	obj[name] ??= [];
	push(obj[name], ...val);
}

function add_hooks(add)
{
	for (let name, val in add)
		this.add_hook(name, val);
}

function add_type(name, val)
{
	this.type[name] = val;
}

function add_types(add)
{
	for (let name, val in add)
		this.add_type(name, val);
}

function add_module(path)
{
	if (substr(path, 0, 1) != "/")
		path = dirname(sourcepath()) + "/modules/" + path;

	let mod;
	try {
		let fn = loadfile(path, {
			raw_mode: true,
			strict_declarations: true,
		});
		mod = call(fn, this, this.scope);
	} catch (e) {
		this.warn(`${e}\n${e.stacktrace[0].context}\nFailed to open module ${path}.\n`);
		return;
	}
}

function add_modules(path)
{
	path ??= "*.uc";
	if (substr(path, 0, 1) != "/")
		path = dirname(sourcepath()) + "/modules/" + path;

	for (let mod in glob(path))
		this.add_module(mod);
}

function run_hook(name, ...args)
{
	let hooks = this.hooks[name];
	if (!hooks)
		return;

	for (let hook in hooks)
		call(hook, this, {}, ...args);
}

function init()
{
	this.run_hook("init");
}

function context_new()
{
	return context.new(this);
}

function exception(e)
{
	this.warn(`${e}\n${e.stacktrace[0].context}`);
}

const data_proto = {
	warn, exception,
	poll_key,
	add_module,
	add_modules,
	add_node,
	add_nodes,
	add_type,
	add_types,
	add_hook,
	add_hooks,
	run_hook,
	init,
	status_msg,
	context: context_new,
};

export function new(cb) {
	cb ??= {};
	let model = proto({
		libubus, ubus, uloop,
		cb,
		hooks: {},
		node: {
			Root: {}
		},
		warnings: {},
		types: { ...types },
	}, data_proto);
	model.scope = proto({ model }, global);
	model.cache = cache.new(model);
	return model;
};
