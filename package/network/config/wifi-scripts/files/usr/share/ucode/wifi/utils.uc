'use strict';

import { readfile, realpath, lsdir } from "fs";
import * as nl80211 from "nl80211";

function phy_filename(phy, name) {
	return `/sys/class/ieee80211/${phy}/${name}`;
}

function phy_file(phy, name) {
	return readfile(phy_filename(phy, name));
}

function phy_index(phy) {
	return +phy_file(phy, "index");
}

function phy_path_match(phy, path) {
	let phy_path = realpath(phy_filename(phy, "device"));
	return substr(phy_path, -length(path)) == path;
}

function __find_phy_by_path(phys, path) {
	if (!path)
		return null;

	path = split(path, "+");
	phys = filter(phys, (phy) => phy_path_match(phy, path[0]));
	phys = sort(phys, (a, b) => phy_index(a) - phy_index(b));

	return phys[+path[1]];
}

function find_phy_by_macaddr(phys, macaddr) {
	macaddr = lc(macaddr);
	return filter(phys, (phy) => phy_file(phy, "macaddr") == macaddr)[0];
}

function rename_phy_by_name(phys, name, rename) {
	let data = json(readfile("/etc/board.json")).wlan;
	if (!data)
		return;

	data = data[name];
	if (!data)
		return;

	let prev_name = __find_phy_by_path(phys, data.path);
	if (!prev_name)
		return;

	if (!rename)
		return true;

	let idx = phy_index(prev_name);
	nl80211.request(nl80211.const.NL80211_CMD_SET_WIPHY, 0, {
		wiphy: idx,
		wiphy_name: name
	});
	return true;
}

function find_phy_by_path(phys, path) {
	let name = __find_phy_by_path(phys, path);
	if (!name)
		return;

	let data = json(readfile("/etc/board.json")).wlan;
	if (!data || data[name])
		return name;

	for (let cur_name, cur_data in data) {
		if (!phy_path_match(name, cur_data.path))
			continue;

		let idx = phy_index(name);
		nl80211.request(nl80211.const.NL80211_CMD_SET_WIPHY, 0, {
			wiphy: idx,
			wiphy_name: cur_name
		});

		return cur_name;
	}

	return name;
}

function find_phy_by_name(phys, name, rename) {
	if (index(phys, name) >= 0)
		return name;

	if (!rename_phy_by_name(phys, name, rename))
		return;

	if (!rename)
		return name;

	return index(phys, name) < 0 ? null : name;
}

export function find_phy(config, rename) {
	let phys = lsdir("/sys/class/ieee80211");

	return find_phy_by_path(phys, config.path) ??
	       find_phy_by_macaddr(phys, config.macaddr) ??
	       find_phy_by_name(phys, config.phy, rename);
};
