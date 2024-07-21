#!/usr/bin/env ucode
import { readfile } from "fs";
import * as uci from 'uci';

const bands_order = [ "6G", "5G", "2G" ];
const htmode_order = [ "HE", "VHT", "HT" ];

let board = json(readfile("/etc/board.json"));
if (!board.wlan)
	exit(0);

let idx = 0;
let commit;

let config = uci.cursor().get_all("wireless") ?? {};

function radio_exists(path, macaddr, phy) {
	for (let name, s in config) {
		if (s[".type"] != "wifi-device")
			continue;
		if (s.macaddr & lc(s.macaddr) == lc(macaddr))
			return true;
		if (s.phy == phy)
			return true;
		if (!s.path || !path)
			continue;
		if (substr(s.path, -length(path)) == path)
			return true;
	}
}

for (let phy_name, phy in board.wlan) {
	let info = phy.info;
	if (!info || !length(info.bands))
		continue;

	while (config[`radio${idx}`])
		idx++;
	let name = "radio" + idx++;

	let s = "wireless." + name;
	let si = "wireless.default_" + name;

	let band_name = filter(bands_order, (b) => info.bands[b])[0];
	if (!band_name)
		continue;

	let band = info.bands[band_name];
	let channel = band.default_channel ?? "auto";

	let width = band.max_width;
	if (band_name == "2G")
		width = 20;
	else if (width > 80)
		width = 80;

	let htmode = filter(htmode_order, (m) => band[lc(m)])[0];
	if (htmode)
		htmode += width;
	else
		htmode = "NOHT";

	if (!phy.path)
		continue;

	let macaddr = trim(readfile(`/sys/class/ieee80211/${phy_name}/macaddress`));
	if (radio_exists(phy.path, macaddr, phy_name))
		continue;

	let id = `phy='${phy_name}'`;
	if (match(phy_name, /^phy[0-9]/))
		id = `path='${phy.path}'`;

	print(`set ${s}=wifi-device
set ${s}.type='mac80211'
set ${s}.${id}
set ${s}.band='${lc(band_name)}'
set ${s}.channel='${channel}'
set ${s}.htmode='${htmode}'
set ${s}.disabled='1'

set ${si}=wifi-iface
set ${si}.device='${name}'
set ${si}.network='lan'
set ${si}.mode='ap'
set ${si}.ssid='OpenWrt'
set ${si}.encryption='none'

`);
	commit = true;
}

if (commit)
	print("commit wireless\n");
