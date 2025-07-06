// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

import { access, basename, dirname, opendir, stat } from "fs";

function is_directory(path)
{
	let s = stat(path);
	return s && s.type == "directory";
}

const types = {
	bool: {
		value: [ "0", "1" ],
		parse: function(ctx, name, val) {
			if (val == "1")
				return true;
			if (val == "0")
				return false;
			ctx.invalid_argument("value for %s must be 0 or 1", name);
			return;
		},
	},
	int: {
		parse: function(ctx, name, strval) {
			let val = +strval;
			if (substr(strval, 0, 1) == "-")
				strval = substr(strval, 1);
			if (match(strval, /[^0-9]/)) {
				ctx.invalid_argument("value for %s is not a number", name);
				return;
			}
			if ((this.min == null || val >= this.min) &&
			    (this.max == null || val <= this.max))
				return val;
			if (this.min != null && this.max != null)
				ctx.invalid_argument(`value for %s must be between ${this.min} and ${this.max}`, name);
			else if (this.min != null)
				ctx.invalid_argument(`value for %s must be at least ${this.min}`, name);
			else
				ctx.invalid_argument(`value for %s must not be bigger than ${this.max}`, name);
			return;
		}
	},
	string: {
		parse: function(ctx, name, val) {
			let len = length(val);
			if ((this.min == null || len >= this.min) &&
			    (this.max == null || len <= this.max))
				return val;
			if (this.min != null && this.max != null)
				ctx.invalid_argument(`String value %s must be between ${this.min} and ${this.max} characters`, name);
			else if (this.min != null)
				ctx.invalid_argument(`String value %s must be at least ${this.min} characters long`, name);
			else
				ctx.invalid_argument(`String value %s must not be longer than ${this.max} characters`, name);
			return;
		}
	},
	enum: {
		parse: function(ctx, name, val) {
			if (this.no_validate)
				return val;

			let list = this.value;
			if (type(list) == "object")
				list = keys(list);
			if (this.ignore_case) {
				val = lc(val);
				val = filter(list, (v) => val == lc(v))[0];
			} else {
				val = filter(list, (v) => val == v)[0];
			}

			if (val == null)
				ctx.invalid_argument("Invalid value for %s", name);

			return val;
		}
	},
	path: {
		complete: function(ctx, val) {
			let ret = [];

			let dir = split(val, "/");
			let prefix = pop(dir);
			push(dir, "");
			dir = join("/", dir);
			let prefix_len = length(prefix);
			let d = opendir(length(dir) ? dir : ".");
			if (!d)
				return ret;

			let cur;
			while (cur = d.read()) {
				if (cur == "." || cur == "..")
					continue;

				if (substr(cur, 0, prefix_len) != prefix)
					continue;

				let path = dir + cur;
				let incomplete = false;
				if (is_directory(path)) {
					path += "/";
					incomplete = true;
				}

				push(ret, { name: path, incomplete });
			}

			return ret;
		},
		parse: function(ctx, name, val) {
			if (this.new_path) {
				let dir = dirname(val);
				let s = stat(dir);
				if (!is_directory(dir)) {
					ctx.invalid_argument("Path '%s' is not a directory", dir);
					return;
				}
			} else {
				if (!access(val, "r")) {
					ctx.invalid_argument("Path '%s' does not exist", val);
					return;
				}
			}
			return val;
		}
	},
	host: {
		parse: function(ctx, name, val) {
			if (length(iptoarr(val)) != 0)
				return val;
			if (length(val) > 255)
				return;
			let labels = split(val, ".");
			if (length(filter(labels, label => !match(label, /^([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9])$/))) == 0 && length(labels) > 0)
				return val;
			ctx.invalid_argument("value for %s is not an valid IP or hostname", name);
			return;
		}
	},
	macaddr: {
		parse: function(ctx, name, val) {
			val = lc(val);
			let arr = split(val, ":");
			if (length(arr) != 6 || length(filter(arr, (v) => !match(v, /^[0-9a-f][0-9a-f]$/))))
				return ctx.invalid_argument("value for %s is not an MAC address", name);
			return val;
		}
	},
	ipv4: {
		parse: function(ctx, name, val) {
			if (length(iptoarr(val)) == 4)
				return val;
			ctx.invalid_argument("value for %s is not an IPv4", name);
			return;
		}
	},
	ipv6: {
		parse: function(ctx, name, val) {
			if (length(iptoarr(val)) == 16)
				return val;
			ctx.invalid_argument("value for %s is not an IPv6", name);
			return;
		}
	},
	cidr4: {
		parse: function(ctx, name, val) {
			let m = split(val, '/', 2);
			if (m && +m[1] <= 32 &&
			    ((m[0] == "auto" && this.allow_auto) ||
			     length(iptoarr(m[0])) == 4))
				return val;
			ctx.invalid_argument("value for %s is not cidr4 (e.g. 192.168.1.1/24)", name);
			return;
		}
	},
};

return types;
