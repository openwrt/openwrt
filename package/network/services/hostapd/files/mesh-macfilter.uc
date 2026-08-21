'use strict';
import * as nl80211 from "nl80211";
import * as ubus from "ubus";
import * as uloop from "uloop";
import { open } from "fs";

let rad = ARGV[0];
let dev = ARGV[1];
let mode_prev = "";
const monitor = ubus.connect();
if (!monitor) die();

system(["logger", "starting ucode 802.11s mesh peer link macfilter for interface", dev]);

function grep_i_file(search, filename) {
	let file = open(filename, "r");
	if (!file) return 2;

	for (let line = file.read('line'); length(line); line = file.read('line')) {
		if (trim(lc(line)) == lc(search)) {
			file.close();
			return 0;
		}
	}
	file.close();
	return 1;
}

function wpa_maclist_cmp(dev, station, mode) {
	let modes = {};

	modes[mode] = grep_i_file(station.mac, `/var/run/wpa-supplicant-${dev}.maclist`);
	return (modes.deny == 0 || modes.block == 0 || modes.allow == 1 || modes.open == 1) ? 1 : 0;
}

function ubus_config_get_wifi_iface(conf, phy, iface) {
	let i = 0;
	const status = ubus.call("network.wireless", "status", {});

	while (status[phy].interfaces[i] != null) {
		if (status[phy].interfaces[i].ifname && status[phy].interfaces[i].ifname == iface)
			break;
		i++;
	}
	if (status[phy].interfaces[i] && status[phy].interfaces[i].config[conf])
		return status[phy].interfaces[i].config[conf];
	return null;
}

function station_dump(dev) {
	return nl80211.request(nl80211.const.NL80211_CMD_GET_STATION, nl80211.const.NLM_F_DUMP, {
		dev: dev
	});
}

function plink_action_block(dev, station) {
	nl80211.request(nl80211.const.NL80211_CMD_SET_STATION, 0, {
		dev: dev,
		mac: station.mac,
		sta_plink_action: nl80211.const.NL80211_PLINK_ACTION_BLOCK
	});
}

function mesh_macfilter(mode) {
	let stations = station_dump(dev);
	for (let station in stations) {
		if (station.sta_info.plink_state != nl80211.const.NL80211_PLINK_ESTAB)
			continue;
		if (wpa_maclist_cmp(dev, station, mode))
			plink_action_block(dev, station);
	}
}

uloop.init();

const check = uloop.timer(-1, () => {
	let mode = ubus_config_get_wifi_iface("macfilter", rad, dev);
	mesh_macfilter(mode);
	if (mode_prev != mode && mode_prev != "")
		system(["/sbin/wifi", "up", rad]);
	mode_prev = mode;
});

nl80211.listener(() => {
	check.set(10000);
});

monitor.listener('*', (type, data) => {
	if (type == "wifi-scripts" && data.action && data.action == "config_done")
		check.set(10000);
	if (type == "network" && data.action && data.action == "reload")
		check.set(10000);
});

uloop.run();
