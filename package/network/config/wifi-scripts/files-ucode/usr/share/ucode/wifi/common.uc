'use strict';

import * as libubus from 'ubus';
import * as fs from 'fs';

global.ubus = libubus.connect();

let config_data = '';
let network_data = '';

export function log(msg) {
	printf(`wifi-scripts: ${msg}\n`);
};

export function append_raw(value) {
	config_data += value + '\n';
};

export function append(key, value) {
	if (value == null)
		return;

	switch (type(value)) {
	case 'array':
		value = join(' ', value);
		break;
	case 'bool':
		value = value ? 1 : 0;
		break;
	}

	append_raw(key + '=' + value);
};

export function append_vars(dict, keys) {
	for (let key in keys)
		append(key, dict[key]);
};

export function network_append_raw(value) {
	network_data += value + '\n';
};

export function network_append(key, value) {
	if (value == null)
		return;

	switch (type(value)) {
	case 'array':
		value = join(' ', value);
		break;
	case 'bool':
		value = value ? 1 : 0;
		break;
	}

	network_append_raw('\t' + key + '=' + value);
};

export function network_append_vars(dict, keys) {
	for (let key in keys)
		network_append(key, dict[key]);
};

export function set_default(dict, key, value) {
	if (dict[key] == null)
		dict[key] = value;
};

export function push_config(dict, key, option, value) {
	if (!dict[option])
		return;

	dict[key] ??= [];
	push(dict[key], value);
};

export function touch_file(filename) {
	let file = fs.open(filename, "a");
	if (file)
		file.close();
	else
		log('Failed to touch ' + filename);
};

export function append_value(config, key, value) {
	if (!config[key])
		config[key] = value;
	else
		config[key] += ' ' + value;
};

export function comment(comment) {
	append_raw('\n# ' + comment);
};

export function dump_config(file) {
	if (file)
		fs.writefile(file, config_data);

	return config_data;
};

export function dump_network(file) {
	config_data += 'network={\n';
	config_data += network_data;;
	config_data += '}\n';

	if (file)
		fs.writefile(file, config_data);

	return config_data;
};

export function flush_config() {
	config_data = '';
};

export function flush_network() {
	config_data = '';
	network_data = '';
};
