// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

function __get_edit_object(ctx, entry, argv, name)
{
	if (type(entry.edit_object) == "function")
		return call(entry.edit_object, entry, ctx.model.scope, ctx, argv);

	if (name)
		return ctx.data.edit[name];

	return ctx.data.edit;
}

function get_edit_object(ctx, entry, argv, name)
{
	let obj = __get_edit_object(ctx, entry, argv, name);
	if (!obj)
		ctx.invalid_argument();

	return obj;
}

function get_param_object(ctx, obj, spec, argv)
{
	if (type(spec.get_object) != "function")
		return obj;

	return call(spec.get_object, spec, ctx.model.scope, ctx, spec, obj, argv);
}

function call_change_cb(ctx, entry, argv, named)
{
	if (!length(named) || type(entry.change_cb) != "function")
		return;

	call(entry.change_cb, entry, ctx.model.scope, ctx, argv);
}

function check_duplicate(ctx, val, new_val)
{
	for (let i = 0; i < length(new_val); i++) {
		let v = new_val[i];
		if ((val && index(val, v) >= 0) ||
			(i && index(slice(new_val, 0, i), v) >= 0)) {
			ctx.invalid_argument("Duplicate value: %s", v);
			return true;
		}
	}
}

export function add_call(ctx, argv, named)
{
	let spec = this.named_args;
	let obj = get_edit_object(ctx, this, argv);
	if (!obj)
		return;

	for (let name, val in named) {
		let cur = spec[name];
		if (type(cur.add) == "function") {
			call(cur.add, cur, ctx.model.scope, ctx, val);
			continue;
		}

		if (cur.attribute)
			name = cur.attribute;

		let cur_obj = get_param_object(ctx, obj, cur, argv);
		cur_obj[name] ??= [];
		if (!cur.allow_duplicate &&
		    check_duplicate(ctx, obj[name], val))
			return;
		push(cur_obj[name], ...val);
	}
	call_change_cb(ctx, this, argv, named);
	return ctx.ok();
};

export function set_call(ctx, argv, named)
{
	let spec = this.named_args;
	let obj = get_edit_object(ctx, this, argv);
	if (!obj)
		return;

	for (let name, val in named) {
		let cur = spec[name];
		if (!cur)
			continue;

		if (type(cur.set) == "function") {
			call(cur.set, cur, ctx.model.scope, ctx, val);
			continue;
		}

		if (cur.attribute)
			name = cur.attribute;

		let cur_obj = get_param_object(ctx, obj, cur, argv);
		if (val == null) {
			delete cur_obj[name];
			continue;
		}

		if (cur.multiple && !cur.allow_duplicate &&
		    check_duplicate(ctx, obj[name], val))
			return;
		cur_obj[name] = val;
	}
	call_change_cb(ctx, this, argv, named);
	return ctx.ok();
};

export function remove_call(ctx, argv, named)
{
	let spec = this.named_args;
	let obj = get_edit_object(ctx, this, argv);
	if (!obj)
		return;

	for (let name, val in named) {
		let cur = spec[name];
		if (type(cur.remove) == "function") {
			call(cur.remove, cur, ctx.model.scope, ctx, val);
			continue;
		}

		if (cur.attribute)
			name = cur.attribute;

		let cur_obj = get_param_object(ctx, obj, cur, argv);
		let data = cur_obj[name];
		if (!data)
			continue;

		for (let idx in val)
			data[+idx - 1] = null;

		cur_obj[name] = filter(data, (v) => v != null);
		if (cur.attribute_allow_empty && !length(cur_obj[name]))
			delete cur_obj[name];
	}
	call_change_cb(ctx, this, argv, named);
	return ctx.ok();
};

export function show_call(ctx, argv, named)
{
	let obj = get_edit_object(ctx, this, argv);
	if (!obj)
		return;

	let data = {};
	for (let name, spec in this.attribute_info) {
		let val;
		if (type(spec.get) == "function") {
			val = call(spec.get, spec, ctx.model.scope, ctx);
		} else {
			let cur_obj = get_param_object(ctx, obj, spec, argv);
			val = cur_obj[spec.attribute ?? name];
		}
		val ??= spec.default;

		if (val != null)
			data[name] = val;
	}

	return ctx.table("Values", data);
};

function param_values(ctx, argv, named_args, spec)
{
	let obj = get_edit_object(ctx, this, argv);
	if (!obj)
		return;

	let values;
	if (type(spec.get) == "function")
		values = call(spec.get, spec, ctx.model.scope, ctx);
	else {
		let cur_obj = get_param_object(ctx, obj, spec, argv);
		values = cur_obj[spec.attribute];
	}

	let ret = {};
	let idx = 0;
	for (let value in values)
		ret["" + (++idx)] = value;

	return ret;
}

function add_params(orig_params)
{
	let params = {};

	for (let name, val in orig_params) {
		if (!val.multiple)
			continue;

		val = { ...val };
		delete val.required;
		delete val.allow_empty;
		params[name] = val;
	}

	return params;
}

function set_params(orig_params)
{
	let params = {};

	for (let name, val in orig_params) {
		val = { ...val };
		if (!val.required)
			val.allow_empty = true;
		else
			delete val.allow_empty;

		delete val.required;
		params[name] = val;
	}

	return params;
}

function remove_params(orig_params)
{
	let params = {};

	for (let name, val in orig_params) {
		if (!val.multiple)
			continue;

		val = { ...val };
		val.attribute_allow_empty = val.allow_empty;
		delete val.required;
		delete val.allow_empty;
		val.args = {
			type: "enum",
			attribute: val.attribute ?? name,
			value: param_values,
			force_helptext: true,
		};

		params[name] = val;
	}

	return params;
}

export function new(info, node)
{
	let params = info.named_args;
	let ret = {
		add: {
			help: "Add list parameter entries",
			args: info.args,
			named_args: add_params(params),
			call: add_call,
			edit_object: info.edit_object,
			change_cb: info.change_cb,
			...(info.add ?? {}),
		},
		show: {
			help: "Show parameter values",
			args: info.args,
			call: show_call,
			attribute_info: params,
			...(info.show ?? {}),
		},
		set: {
			help: "Set parameter values",
			args: info.args,
			named_args: set_params(params),
			call: set_call,
			edit_object: info.edit_object,
			change_cb: info.change_cb,
			...(info.set ?? {}),
		},
		remove: {
			help: "Remove parameter values",
			args: info.args,
			named_args: remove_params(params),
			call: remove_call,
			edit_object: info.edit_object,
			change_cb: info.change_cb,
			...(info.remove ?? {}),
		}
	};

	if (!length(ret.add.named_args)) {
		delete ret.add;
		delete ret.remove;
	}

	if (node)
		for (let cmd, val in ret)
			node[cmd] = val;

	return ret;
};

export function object_destroy_call(ctx, argv, named)
{
	let type_name = argv[0];
	if (!type_name)
		return ctx.invalid_argument();

	let info = this.object_info;
	let type_info = info.types[type_name];
	if (!type_info)
		return ctx.invalid_argument();

	let obj_name = type_info.object ?? type_name;

	let name = argv[1];
	if (type_info.delete)
		return call(type_info.delete, info, ctx.model.scope, ctx, type, name);

	let obj = ctx.data.object_edit[obj_name];
	if (!obj)
		return ctx.unknown_error();

	if (!obj[name])
		return ctx.not_found();

	delete obj[name];

	if (info.change_cb)
		call(info.change_cb, info, ctx.model.scope, ctx, argv);

	return ctx.ok(`Deleted ${argv[0]} '${name}'`);
};

const create_edit_param = {
	help: "Edit object after creating",
};

export function object_create_params(node)
{
	if (!node.show)
		return {};

	let orig_params = node.show.attribute_info;
	let params = {};

	for (let name, val in orig_params) {
		if (val.change_only)
			continue;

		params[name] = val;
	}
	params.edit ??= create_edit_param;

	return params;
};

export function object_create_call(ctx, argv, named)
{
	let type_name = argv[0];
	if (!type_name)
		return ctx.invalid_argument();

	let info = this.object_info;
	let type_info = info.types[type_name];
	if (!type_info)
		return ctx.invalid_argument();

	let obj_name = type_info.object ?? type_name;

	let name = argv[1];
	let obj, data;
	if (type_info.add) {
		data = call(type_info.add, info, ctx.model.scope, ctx, type, name);
		if (!data)
			return;
	} else {
		data = {};
	}

	ctx.data.object_edit[obj_name] ??= {};
	obj = ctx.data.object_edit[obj_name];

	let entry = type_info.node.set;
	if (entry) {
		ctx.apply_defaults();
		let subctx = ctx.clone();
		subctx.data.edit = data;

		try {
			call(entry.call, entry, ctx.model.scope, subctx, argv, named);
		} catch (e) {
			ctx.model.exception(e);
			return ctx.unknown_error();
		}

		if (!subctx.result.ok) {
			ctx.result = subctx.result;
			return;
		}
	}

	obj[name] = data;

	if (named.edit)
		ctx.select(type_name, name);

	return ctx.ok(`Added ${type_name} '${name}'`);
};

function object_lookup(ctx, entry, type_name)
{
	let info = entry.object_info;
	let type_info = info.types[type_name];
	if (!type_info)
		return [];

	let obj_name = type_info.object ?? type_name;

	return ctx.data.object_edit[obj_name];
}

function object_values(ctx, entry, type_name)
{
	let obj = object_lookup(ctx, entry, type_name);
	if (!obj)
		return [];

	return keys(obj);
}

export function object_list_call(ctx, argv, named)
{
	return ctx.list(argv[0] + " list", object_values(ctx, this, argv[0]));
};

export function edit_create_destroy(info, node)
{
	let type_arg = {
		name: "type",
		help: "Type",
		type: "enum",
		required: true,
		value: keys(info.types),
	};
	let name_arg = {
		name: "name",
		help: "Name",
		type: "string",
		required: true,
	};
	let delete_name_arg = {
		...name_arg,
		type: "enum",
		value: function(ctx, argv) {
			return object_values(ctx, this, argv[0]);
		}
	};

	let create_params = {};
	for (let name, val in info.types)
		create_params[name] = object_create_params(val.node);

	let types_info = " (" + join(", ", keys(info.types)) + ")";
	let cmds = {
		destroy: {
			object_info: info,
			help: "Delete object" + types_info,
			args: [ type_arg, delete_name_arg ],
			call: object_destroy_call,
		},
		list: {
			object_info: info,
			help: "List objects" + types_info,
			args: [ type_arg ],
			call: object_list_call,
		},
		create: {
			object_info: info,
			help: "Create object" + types_info,
			args: [ type_arg, name_arg ],
			type_params: create_params,
			named_args: function(ctx, argv) {
				if (!argv[0])
					return;
				return this.type_params[argv[0]];
			},
			call: object_create_call,
		},
	};

	for (let name, val in info.types) {
		cmds[name] = {
			object_name: name,
			object_info: info,
			help: "Edit " + name,
			args: [
				{
					...name_arg,
					type: "enum",
					value: function(ctx, argv) {
						return object_values(ctx, this, this.object_name);
					}
				}
			],
			select_node: val.node_name,
			select: function(ctx, argv) {
				let name = argv[0];
				if (!name) {
					warn(`Missing argument\n`);
					return;
				}

				let obj = object_lookup(ctx, this, this.object_name);
				if (!obj) {
					warn(`Object not found\n`);
					return;
				}

				let entry = obj[name];
				if (!entry) {
					warn(`${name} not found\n`);
					return;
				}

				let info = this.object_info;
				let type_info = info.types[this.object_name];
				return ctx.set(`${this.object_name} "${name}"`, {
					name,
					edit: entry,
					object_edit: entry,
				});
			}
		};
	}

	if (node)
		for (let cmd, val in cmds)
			node[cmd] = val;

	return cmds;
};
