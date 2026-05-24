// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

import * as callctx from "cli.context-call";

function prefix_match(prefix, str, icase)
{
	if (icase) {
		str = lc(str);
		prefix = lc(prefix);
	}
	return substr(str, 0, length(prefix)) == prefix;
}

function context_clone()
{
	let ret = { ...this };
	ret.prompt = [ ...ret.prompt ];
	ret.data = { ...ret.data };
	ret.hooks = {};
	return proto(ret, proto(this));
}

function context_entries()
{
	return keys(this.node)
}

function context_help(entry)
{
	if (entry)
		return this.node[entry].help;

	let ret = {};
	for (let name, val in this.node)
		ret[name] = val.help ?? "";

	return ret;
}

function context_add_hook(type, cb)
{
	this.hooks[type] ??= [];
	push(this.hooks[type], cb);
}

function context_select_error(code, msg, ...args)
{
	msg ??= "Unknown error";
	msg = sprintf(msg, ...args);
	let error = {
		code, msg, args
	};
	this.errors ??= [];
	push(this.errors, error);
}

function context_set(prompt, data)
{
	if (prompt)
		this.cur_prompt = prompt;
	if (data)
		this.data = { ...this.data, ...data };
	return true;
}

const context_select_proto = {
	add_hook: context_add_hook,
	error: context_select_error,
	set: context_set,
	...callctx.callctx_error_proto,
};

function __context_select(ctx, name, args)
{
	let entry = ctx.node[name];
	if (!entry || !entry.select_node)
		return;

	let node = ctx.model.node[entry.select_node];
	if (!node)
		return;

	let ret = proto(ctx.clone(), ctx.model.context_select_proto);
	ret.cur_prompt = name;
	ret.node = node;
	try {
		if (entry.select &&
		    !call(entry.select, entry, ctx.model.scope, ret, args))
			ret.errors ??= [];
	} catch (e) {
		ctx.model.exception(e);
		return;
	}

	push(ret.prompt, ret.cur_prompt);
	ret.prev = ctx;
	proto(ret, proto(ctx));

	return ret;
}

function context_run_hooks(ctx, name)
{
	try {
		while (length(ctx.hooks[name]) > 0) {
			let hook = ctx.hooks[name][0];

			let ret = call(hook, ctx, ctx.model.scope);
			if (!ret)
				return false;

			shift(ctx.hooks.exit);
		}
	} catch (e) {
		ctx.model.exception(e);
		return false;
	}

	return true;
}

function context_prev(ctx, skip_hooks)
{
	if (!skip_hooks && !context_run_hooks(ctx, "exit"))
		return;
	return ctx.prev;
}

function context_top(ctx, skip_hooks)
{
	while (ctx && ctx.prev)
		ctx = context_prev(ctx, skip_hooks);
	return ctx;
}

function prepare_spec(e, ctx, spec, argv)
{
	if (type(spec) != "function")
		return spec;

	return call(spec, e, ctx.model.scope, ctx, argv);
}

function prepare_default(e, ctx, spec, argv, named_args)
{
	if (type(spec) != "object" || type(spec.default) != "function")
		return;

	try {
		spec.default = call(spec.default, e, ctx.model.scope, ctx, argv, named_args, spec);
	} catch (e) {
		model.exception(e);
	}
}

function prepare_attr_spec(e, ctx, spec, argv, named_args)
{
	if (type(spec) != "object")
		return spec;

	let t = ctx.model.types[spec.type];
	if (t)
		spec = { ...t, ...spec };
	else
		spec = { ...spec };

	prepare_default(e, ctx, spec, argv, named_args, spec);
	if (type(spec.value) == "function")
		try {
			spec.value = call(spec.value, e, ctx.model.scope, ctx, argv, named_args, spec);
		} catch (e) {
			ctx.model.exception(e);
			spec.value = [];
		}

	return spec;
}

function parse_arg(ctx, name, spec, val)
{
	let t;

	if (val == null) {
		ctx.invalid_argument("Missing argument %s", name);
		return;
	}

	if (type(spec) == "object" && spec.type)
		t = ctx.model.types[spec.type];
	if (!t) {
		ctx.invalid_argument("Invalid type in argument: %s", name);
		return;
	}

	if (!t.parse)
		return val;

	return call(t.parse, spec, ctx.model.scope, ctx, name, val);
}

const context_defaults = {
	up: [ "Return to previous node", context_prev ],
	exit: [ "Return to previous node", context_prev ],
	main: [ "Return to main node", context_top ],
};

const context_default_order = [ "up", "exit", "main" ];

function context_select(args, completion)
{
	let ctx = this;

	while (length(args) > completion ? 1 : 0) {
		let name = args[0];
		let entry = ctx.node[name];

		if (!entry) {
			let e = context_defaults[name];
			if (!e)
				return ctx;

			shift(args);
			ctx = e[1](ctx, completion);
			if (!ctx)
				return;

			continue;
		}

		if (!entry.select_node)
			return ctx;

		let num_args = length(entry.args);
		if (completion && num_args + 1 >= length(args))
			return ctx;

		shift(args);
		let argv = [];
		let parse_ctx = callctx.new(this.model, ctx);
		if (num_args > 0) {
			let cur_argv = slice(args, 0, num_args);
			for (let i = 0; i < num_args; i++) {
				let arg = shift(args);
				let spec = entry.args[i];

				spec = prepare_attr_spec(entry, ctx, spec, cur_argv, {});
				if (arg != null)
					arg = parse_arg(parse_ctx, spec.name, spec, arg);

				if (arg != null)
					push(argv, arg);
			}

		}

		if (entry.no_subcommands && length(args) > 0)
			parse_ctx.invalid_argument("command %s does not support subcommands", name);

		if (length(parse_ctx.result.errors) > 0) {
			ctx = ctx.clone();
			ctx.errors = parse_ctx.result.errors;
			return ctx;
		}

		ctx = __context_select(ctx, name, argv);
		if (type(ctx) != "object" || ctx.errors)
			break;
	}

	return ctx;
}

function complete_named_params(ctx, entry, obj, name, argv, named_params)
{
	let data = [];
	let empty = "";

	if (substr(name, 0, 1) == "-") {
		empty = "-";
		name = substr(name, 1);
	}

	let defaults = {};
	callctx.new(ctx.model, ctx).apply_defaults(obj, defaults);
	for (let cur_name in sort(keys(obj))) {
		let val = obj[cur_name];

		if (!prefix_match(name, cur_name) || val.no_complete)
			continue;

		if (empty && !(val.allow_empty ?? entry.allow_empty))
			continue;

		if (!val.multiple && named_params[cur_name] != null)
			continue;

		if (type(val.available) == "function" &&
		    !call(val.available, val, ctx.model.scope, ctx, argv, named_params))
			continue;

		val = {
			name: empty + cur_name,
			...val,
		};
		push(data, val);
	}

	return {
		type: "keywords",
		name: "parameter",
		help: "Parameter name",
		value: data
	};
}

function complete_param(e, ctx, cur, val, args, named_args)
{
	cur = prepare_attr_spec(e, ctx, cur, args, named_args);

	if (type(cur.value) == "object") {
		let ret = [];
		for (let key in sort(keys(cur.value)))
			if (prefix_match(val, key, cur.ignore_case))
				push(ret, {
					name: key,
					help: cur.value[key]
				});

		cur.value = ret;
		return cur;
	}

	if (type(cur.value) == "array") {
		cur.value = map(sort(filter(cur.value, (v) => prefix_match(val, v, cur.ignore_case))), (v) => ({ name: v }));
		return cur;
	}

	let type_info = ctx.model.types[cur.type];
	if (!type_info || !type_info.complete)
		return cur;

	cur.value = call(type_info.complete, cur, ctx.model.scope, ctx, val);

	return cur;
}

function complete_arg_list(e, ctx, arg_info, args, base_args, named_args)
{
	let cur_idx = length(args) - 1;
	let cur = arg_info[cur_idx];
	let val;

	for (let i = 0; i <= cur_idx; i++)
		val = shift(args);

	let ret = complete_param(e, ctx, cur, val, base_args, named_args);
	if (!cur.prefix_separator)
		return ret;

	let prefix_len = length(val);
	let vals = [];
	let match_prefix;
	for (let cur_val in ret.value) {
		let cur_str = cur_val.name;
		let cur_suffix = substr(cur_str, prefix_len);
		let idx = index(cur_suffix, cur.prefix_separator);
		if (idx < 0) {
			push(vals, cur_val);
			continue;
		}

		let cur_prefix = substr(cur_str, 0, prefix_len + idx + 1);
		if (cur_prefix == match_prefix)
			continue;

		match_prefix = cur_prefix;
		push(vals, {
			...cur_val,
			name: cur_prefix,
			incomplete: true
		});
	}
	ret.value = vals;

	return ret;
}

function handle_empty_param(entry, spec, name, argv, named_args)
{
	if (substr(name, 0, 1) != "-")
		return;

	name = substr(name, 1);
	let cur = spec[name];
	if (!cur)
		return;

	if (cur.default == null &&
	    !(cur.allow_empty ?? entry.allow_empty))
		return;

	if (cur.required) {
		cur = { ...cur };
		prepare_default(e, ctx, cur, argv, named_args, cur);
		named_args[name] = cur.default;
	} else {
		named_args[name] = null;
	}
	return true;
}


function default_complete(ctx, args)
{
	let num_args = length(this.args);
	let named_args = {};
	let cur_args;

	if (length(args) <= num_args)
		return complete_arg_list(this, ctx, this.args, args, [ ...args ], named_args);

	let spec = prepare_spec(this, ctx, this.named_args, args);
	if (!spec)
		return;

	let base_args = slice(args, 0, num_args);
	for (let i = 0; i < num_args; i++)
		shift(args);

	while (length(args) > 0) {
		let name = args[0];

		if (length(args) == 1)
			return complete_named_params(ctx, this, spec, name, base_args, named_args);

		shift(args);
		let cur = spec[name];
		if (!cur) {
			if (handle_empty_param(this, spec, name, base_args, named_args))
				continue;
			return;
		}

		if (!cur.args) {
			named_args[name] = true;
			continue;
		}

		let val;
		let cur_spec = cur.args;
		if (type(cur_spec) != "array") {
			cur_spec = [{
				name,
				help: cur.help,
				...cur_spec
			}];
			named_args[name] = shift(args);
			val = [ named_args[name] ];
		} else {
			let num_args = length(cur_spec);
			let val = [];
			for (let i = 0; i < num_args; i++)
				push(val, shift(args));
			named_args[name] = val;
		}

		if (!length(args))
			return complete_arg_list(this, ctx, cur_spec, val, base_args, named_args);
	}
}

function context_complete(args)
{
	let ctx = this.select(args, true);
	if (!ctx || ctx.errors)
		return;

	if (ctx != this) {
		ctx = ctx.clone();
		ctx.skip_default_complete = true;
	}

	if (length(args) > 1) {
		let name = shift(args);
		let entry = ctx.node[name];
		if (!entry)
			return;

		try {
			if (!entry.available || call(entry.available, entry, ctx.model.scope, ctx, args))
				return call(entry.complete ?? default_complete, entry, ctx.model.scope, ctx, args);
		} catch (e) {
			this.model.exception(e);
		}
		return;
	}

	let name = shift(args) ?? "";
	let prefix_len = length(name);
	let data = [];
	let default_data = {};
	for (let cur_name in sort(keys(ctx.node))) {
		let val = ctx.node[cur_name];

		if (substr(cur_name, 0, prefix_len) != name)
			continue;

		if (val.available && !call(val.available, val, ctx.model.scope, ctx, args))
			continue;

		let cur = {
			name: cur_name,
			help: val.help,
			category: val.select_node ? "Object" : "Action",
		};
		if (context_defaults[cur_name])
			default_data[cur_name] = cur;
		else
			push(data, cur);
	}

	for (let cur_name in context_default_order) {
		if (substr(cur_name, 0, prefix_len) != name)
			continue;

		let val = default_data[cur_name];
		if (!val) {
			if (!ctx.prev || ctx.skip_default_complete)
				continue;
			val = {
				name: cur_name,
				help: context_defaults[cur_name][0],
				category: "Navigation",
			};
		}

		push(data, val);
	}

	return {
		type: "enum",
		name: "command",
		help: "Command",
		value: data
	};
}

function context_call(args)
{
	let ctx = this.select(args);
	if (!ctx || !length(args))
		return;

	let name = shift(args);
	let entry = ctx.node[name];
	if (!entry)
		return;

	if (!entry.call)
		return;

	let named_args = {};
	let num_args = length(entry.args);
	let cur_argv = slice(args, 0, num_args);
	let argv = [];
	let skip = {};

	ctx = callctx.new(this.model, ctx);
	ctx.entry = entry;
	ctx.named_args = named_args;

	for (let i = 0; i < num_args; i++) {
		let arg = shift(args);
		let spec = entry.args[i];

		spec = prepare_attr_spec(entry, ctx, spec, cur_argv, named_args);
		if (arg != null)
			arg = parse_arg(ctx, spec.name, spec, arg);

		if (spec.required && !length(arg)) {
			if (spec.default)
				arg = spec.default;
			else
				ctx.missing_argument("Missing argument %d: %s", i + 1, spec.name);
		}

		if (arg != null)
			push(argv, arg);
	}

	let spec = prepare_spec(entry, ctx, entry.named_args, argv) ?? {};
	let defaults = {};
	ctx.apply_defaults(spec, defaults);
	while (length(args) > 0) {
		let name = shift(args);
		let cur = spec[name];
		try {
			if (cur && type(cur.available) == "function" &&
				!call(cur.available, cur, ctx.model.scope, ctx, argv, { ...defaults, ...named_args }))
					cur = null;
		} catch (e) {
			ctx.model.exception(e);
			continue;
		}

		if (!cur) {
			if (handle_empty_param(entry, spec, name, argv, named_args))
				continue;
			ctx.invalid_argument("Invalid argument: %s", name);
			return ctx.result;
		}

		if (!cur.args) {
			named_args[name] = true;
			continue;
		}

		let val;
		let cur_spec = cur.args;
		if (type(cur.args) == "array") {
			val = [];
			for (let spec in cur.args) {
				spec = prepare_attr_spec(entry, ctx, spec, argv, named_args);
				let cur = parse_arg(ctx, name, spec, shift(args));
				if (cur == null)
					return ctx.result;

				push(val, cur);
			}
		} else {
			let spec = prepare_attr_spec(entry, ctx, cur.args, argv, named_args);
			val = parse_arg(ctx, name, spec, shift(args));
			if (val == null)
				return ctx.result;
		}
		if (cur.multiple) {
			named_args[name] ??= [];
			push(named_args[name], val);
		} else {
			named_args[name] = val;
		}
	}

	for (let name, arg in spec) {
		if (!arg.required || named_args[name] != null)
			continue;

		try {
			if (type(arg.available) == "function" &&
				!call(arg.available, arg, ctx.model.scope, ctx, argv, named_args))
				continue;
		} catch (e) {
			ctx.model.exception(e);
			continue;
		}

		let spec = { ...arg };
		prepare_default(entry, ctx, spec, argv, named_args);
		if (spec.default != null)
			named_args[name] = spec.default;
		else
			ctx.missing_argument("Missing argument: %s", name);
	}

	if (length(ctx.result.errors) > 0)
		return ctx.result;

	if (entry.available && !call(entry.available, entry, ctx.model.scope, ctx))
		return ctx.result;

	try {
		if (!entry.validate || call(entry.validate, entry, ctx.model.scope, ctx, argv, named_args))
			call(entry.call, entry, ctx.model.scope, ctx, argv, named_args);
	} catch (e) {
		this.model.exception(e);
		return;
	}
	return ctx.result;
}

const context_proto = {
	clone: context_clone,
	entries: context_entries,
	help: context_help,
	select: context_select,
	call: context_call,
	complete: context_complete,
	add_hook: context_add_hook,
};

export function new(model) {
	model.context_proto ??= {
		model,
		...context_proto
	};
	model.context_select_proto ??= {
		model,
		...context_select_proto
	};
	return proto({
		prompt: [],
		node: model.node.Root,
		hooks: {},
		data: {}
	}, model.context_proto);
};
