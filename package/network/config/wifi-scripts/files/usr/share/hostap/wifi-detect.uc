#!/usr/bin/env ucode
'use strict';
import { readfile, writefile, realpath, glob, basename, unlink, open, rename } from "fs";
import { is_equal } from "/usr/share/hostap/common.uc";
let nl = require("nl80211");

let board_file = "/etc/board.json";
let prev_board_data = json(readfile(board_file));
let board_data = json(readfile(board_file));

function phy_idx(name) {
	return +rtrim(readfile(`/sys/class/ieee80211/${name}/index`));
}

function phy_path(name) {
	let devpath = realpath(`/sys/class/ieee80211/${name}/device`);

	devpath = replace(devpath, /^\/sys\/devices\//, "");
	if (match(devpath, /^platform\/.*\/pci/))
		devpath = replace(devpath, /^platform\//, "");
	let dev_phys = map(glob(`/sys/class/ieee80211/${name}/device/ieee80211/*`), basename);
	sort(dev_phys, (a, b) => phy_idx(a) - phy_idx(b));

	let ofs = index(dev_phys, name);
	if (ofs > 0)
		devpath += `+${ofs}`;

	return devpath;
}

function cleanup() {
	let wlan = board_data.wlan;

	for (let name in wlan)
		if (substr(name, 0, 3) == "phy")
			delete wlan[name];
		else
			delete wlan[name].info;
}

function wiphy_get_entry(phy, path) {
	board_data.wlan ??= {};

	let wlan = board_data.wlan;
	for (let name in wlan)
		if (wlan[name].path == path)
			return wlan[name];

	wlan[phy] = {
		path: path
	};

	return wlan[phy];
}

function wiphy_detect() {
	let phys = nl.request(nl.const.NL80211_CMD_GET_WIPHY, nl.const.NLM_F_DUMP, { split_wiphy_dump: true });
	if (!phys)
		return;

	for (let phy in phys) {
		let name = phy.wiphy_name;
		let path = phy_path(name);
		let info = {
			antenna_rx: phy.wiphy_antenna_avail_rx,
			antenna_tx: phy.wiphy_antenna_avail_tx,
			bands: {},
		};

		let bands = info.bands;
		for (let band in phy.wiphy_bands) {
			if (!band || !band.freqs)
				continue;
			let freq = band.freqs[0].freq;
			let band_info = {};
			let band_name;
			if (freq > 50000)
				band_name = "60G";
			else if (freq > 5900)
				band_name = "6G";
			else if (freq > 4000)
				band_name = "5G";
			else
				band_name = "2G";
			bands[band_name] = band_info;
			if (band.ht_capa > 0)
				band_info.ht = true;
			if (band.vht_capa > 0)
				band_info.vht = true;
			let he_phy_cap = 0;

			for (let ift in band.iftype_data) {
				if (!ift.he_cap_phy)
					continue;

				band_info.he = true;
				he_phy_cap |= ift.he_cap_phy[0];
				/* TODO: EHT */
			}

			if (band_name != "2G" &&
			    (he_phy_cap & 0x18) || ((band.vht_capa >> 2) & 0x3))
				band_info.max_width = 160;
			else if (band_name != "2G" &&
			         (he_phy_cap & 4) || band.vht_capa > 0)
				band_info.max_width = 80;
			else if ((band.ht_capa & 0x2) || (he_phy_cap & 0x2))
				band_info.max_width = 40;
			else
				band_info.max_width = 20;

			let modes = band_info.modes = [ "NOHT" ];
			if (band_info.ht)
				push(modes, "HT20");
			if (band_info.vht)
				push(modes, "VHT20");
			if (band_info.he)
				push(modes, "HE20");
			if (band.ht_capa & 0x2) {
				push(modes, "HT40");
				if (band_info.vht)
					push(modes, "VHT40")
			}
			if (he_phy_cap & 0x2)
				push(modes, "HE40");

			if (band_name == "2G")
				continue;
			if (band_info.vht)
				push(modes, "VHT80");
			if (he_phy_cap & 4)
				push(modes, "HE80");
			if ((band.vht_capa >> 2) & 0x3)
				push(modes, "VHT160");
			if (he_phy_cap & 0x18)
				push(modes, "HE160");
		}

		let entry = wiphy_get_entry(name, path);
		entry.info = info;
	}
}

cleanup();
wiphy_detect();
if (!is_equal(prev_board_data, board_data)) {
	let new_file = board_file + ".new";
	unlink(new_file);
	let f = open(new_file, "wx");
	if (!f)
		exit(1);
	f.write(sprintf("%.J\n", board_data));
	f.close();
	rename(new_file, board_file);
}
