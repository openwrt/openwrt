#!/usr/bin/env ucode
'use strict';
import { readfile, basename } from "fs";
let uht = require("uht");

let signatures = {};

function parse_category(str) {
	let items = split(str, "|");
	let data = {};
	for (let item in items) {
		item = split(item, "=", 2);
		if (item[1] == "%")
			data["%val"] = item[0];
		else
			data[item[0]] = item[1];
	}

	return data;
}

function get_device(meta, name)
{
	let dev = {};

	dev[meta["%val"] ?? "device"] = name;
	for (let type in meta) {
		if (substr(type, 0, 1) == "%")
			continue;

		dev[type] = meta[type];
	}

	return dev;
}

let out = shift(ARGV);
if (!out) {
	warn(`Syntax: ${basename(sourcepath())} <output> <jsonfile> [<jsonfile> ...]\n`);
	exit(1);
}

for (let file in ARGV) {
	let data = json(readfile(file));
	for (let category_str in data) {
		let category = parse_category(category_str);
		let devices = data[category_str];
		for (let dev in devices) {
			for (let sig in devices[dev]) {
				signatures[sig] ??= [];
				push(signatures[sig], get_device(category, dev));
			}
		}
	}
}

uht.mark_hashtable(signatures);
uht.save(out, signatures);
