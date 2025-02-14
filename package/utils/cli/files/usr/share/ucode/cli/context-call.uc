// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

function default_result()
{
	return {
		errors: [],
		ok: false
	};
}

function context_clone()
{
	let ret = { ...this };
	ret.result = default_result();
	ret.data = { ...ret.data };
	return proto(ret, proto(this));
}

function call_select(...args)
{
	this.result.ctx = this.node_ctx.select(args);
}

function call_ok(msg)
{
	this.result.ok = true;
	if (msg)
		this.result.status_msg = msg;
	return true;
}

function call_error(code, msg, ...args)
{
	msg ??= "Unknown error";
	msg = sprintf(msg, ...args);
	let error = {
		code, msg, args
	};
	push(this.result.errors, error);
}

function call_generic(ctx, name, type, val)
{
	ctx.result.type = type;
	ctx.result.name = name;
	ctx.result.data = val;
	return ctx.ok();
}

function call_multi_table(name, val)
{
	return call_generic(this, name, "multi_table", val);
}

function call_table(name, val)
{
	return call_generic(this, name, "table", val);
}

function call_list(name, val)
{
	return call_generic(this, name, "list", val);
}

function call_string(name, val)
{
	return call_generic(this, name, "string", val);
}

function call_json(name, val)
{
	return call_generic(this, name, "json", val);
}

function call_apply_defaults(named_args, args)
{
	let entry = this.entry;
	named_args ??= entry.named_args;
	args ??= this.named_args;
	for (let name, arg in named_args)
		if (arg.default != null && !(name in args))
			args[name] ??= arg.default;
}

export const callctx_error_proto = {
	missing_argument: function(msg, ...args) {
		return this.error("MISSING_ARGUMENT", msg ?? "Missing argument", ...args);
	},
	invalid_argument: function(msg, ...args) {
		return this.error("INVALID_ARGUMENT", msg ?? "Invalid argument", ...args);
	},
	unknown_error: function(msg, ...args) {
		return this.error("UNKNOWN_ERROR", msg ?? "Unknown error", ...args);
	},
	not_found: function(msg, ...args) {
		return this.error("NOT_FOUND", msg ?? "Not found", ...args);
	},
	command_failed: function(msg, ...args) {
		return this.error("COMMAND_FAILEDu", msg ?? "Command failed", ...args);
	},
};

const callctx_proto = {
	clone: context_clone,
	select: call_select,
	apply_defaults: call_apply_defaults,
	ok: call_ok,
	list: call_list,
	table: call_table,
	multi_table: call_multi_table,
	string: call_string,
	json: call_json,

	error: call_error,
	...callctx_error_proto,
};

export function new(model, ctx) {
	let node_ctx = ctx;
	let data = ctx.data;
	model.callctx_proto ??= { model, ...callctx_proto };
	let result = default_result();
	return proto({ node_ctx, data, result }, model.callctx_proto);
};
