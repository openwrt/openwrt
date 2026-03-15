// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
 */
'use strict';
import * as struct from "struct";
import * as fs from "fs";

const MAGIC = 0xf09f8697;
const HDR_LEN = 9;

let hdr = struct.new(">LLc");

const ubi_proto = {
	read: function() {
		let file = fs.open(this.dev);
		if (!file)
			return;

		let hdr_data = file.read(HDR_LEN);
		if (!hdr_data)
			return;

		hdr_data = hdr.unpack(hdr_data);
		if (!hdr_data)
			return;

		if (hdr_data[0] != MAGIC)
			return;

		if (hdr_data[1] > 131072 || hdr_data[2] != 0)
			return;

		let data = file.read(hdr_data[1]);
		if (length(data) != hdr_data[1])
			return;

		return data;
	},
	commit: function(data) {
		let len = HDR_LEN + length(data);

		let file = fs.popen(`ubiupdatevol ${this.dev} -s ${len} -`, "w");
		file.write(hdr.pack(MAGIC, length(data), 0));
		file.write(data);

		return file.close() == 0;
	},
	destroy: function() {
		let dev = replace(this.dev, /_\d+$/, "");
		return system(`ubirmvol ${dev} -N provisioning`) == 0;
	}
};

function open_ubi()
{
	let found = fs.glob("/sys/class/ubi/*/name");
	found = filter(found, (v) => trim(fs.readfile(v)) == "provisioning");
	if (!length(found))
		return;

	let dev_name = fs.basename(fs.dirname(found[0]));

	return proto({
		dev: "/dev/" + dev_name,
	}, ubi_proto);
}

function create_ubi()
{
	let ctx = open_ubi();
	if (ctx)
		return ctx;

	let found = fs.glob("/sys/class/ubi/*/name");
	found = filter(found, (v) => substr(fs.readfile(v), 0, 6) == "rootfs");
	if (!length(found))
		return;

	let dev = fs.basename(fs.dirname(found[0]));
	dev = "/dev/" + replace(dev, /_\d+$/, "");
	if (system(`ubimkvol ${dev} -N provisioning -s 131072`) != 0)
		return;

	return open_ubi();
}

function data_path_get(data, path, create)
{
	if (!data)
		return;

	if (!length(path))
		return data;

	if (type(path) == "string")
		path = split(path, ".");

	let last = data;
	let last_name;
	for (let name in path) {
		switch (type(data)) {
		case "object":
			last = data;
			last_name = name;
			data = data[name];
			break;
		case "array":
			last = data;
			last_name = name;
			data = data[+name];
			break;
		default:
			return;
		}

		if (data == null && create)
			data = last[last_name] = {};
	}

	return data;
}

const provision_proto = {
	init: function() {
		this.data = this.backend.read();
		try {
			this.data = json(this.data);
		} catch(e) {
			this.data = null;
		}
		if (!this.data)
			this.reset();
		return true;
	},
	get: function(path) {
		return data_path_get(this.data, path);
	},
	set: function(path, value) {
		if (!length(path))
			return;

		if (type(path) == "string")
			path = split(path, ".");
		let name = pop(path);
		let data = data_path_get(this.data, path, true);
		if (type(data) != "object")
			return;

		if (value == null)
			delete data[name];
		else
			data[name] = value;
		return true;
	},
	reset: function() {
		this.data = {};
		return true;
	},
	commit: function() {
		if (!this.data)
			return;

		return this.backend.commit("" + this.data);
	},
	destroy: function() {
		return this.backend.destroy();
	}
};

function __open(backend)
{
	if (!backend)
		return;

	return proto({
		backend,
	}, provision_proto);
}

export function create()
{
	return __open(create_ubi());
};

export function open()
{
	return __open(open_ubi());
};
