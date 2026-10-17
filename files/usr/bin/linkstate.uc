#!/usr/bin/ucode
// Link state gatherer for QCA9533 Smart Radio — uses nl80211, ubus, batctl.
// One-shot: gathers stats, writes JSON, fires ubus event, exits.
// procd respawn handles the interval (default 2s).
import * as libubus from 'ubus';
let nl = require("nl80211");
let fs = require("fs");
let def = nl.const;
let ubus = require("ubus").connect();

let ifindex = +(ARGV[0] ?? 0);
let ifname = "wlan" + ifindex;
let phy = "phy" + ifindex;
let linkstate_raw = "/tmp/linkstate_raw.json";
let linkstate_current = "/tmp/linkstate_current.json";

function read_output(cmd) {
	let pipe = fs.popen(cmd);
	let result = pipe.read("all");
	let retcode = pipe.close();
	return (retcode == 0) ? trim(result) : null;
}

function iface_assoclist() {
	let params = { dev: ifname };
	let res = nl.request(def.NL80211_CMD_GET_STATION, def.NLM_F_DUMP, params);
	if (res === false) return {};

	let assoc = {};
	for (let sta in res) {
		assoc[sta.mac] = {
			mac: sta.mac,
			connected: +sta.sta_info?.connected_time,
			inactive: +sta.sta_info?.inactive_time,
			tx_packets: +sta.sta_info?.tx_packets,
			tx_bytes: +sta.sta_info?.tx_bytes,
			tx_retries: +sta.sta_info?.tx_retries,
			tx_failed: +sta.sta_info?.tx_failed,
			mcs: +sta.sta_info?.tx_bitrate?.mcs,
			rssi: +sta.sta_info?.signal,
			rssi_ant: [
				+sta.sta_info?.chain_signal?.[0],
				+sta.sta_info?.chain_signal?.[1]
			],
		};
	}
	return assoc;
}

let prev = {};
if (fs.stat(linkstate_raw)) {
	prev = json(fs.readfile(linkstate_raw) || "{}");
}

let rv = {};

// System info
let sysinfo_raw = ubus.call("system", "info");
if (sysinfo_raw) {
	rv.sysinfo = {
		cpu_load: sysinfo_raw.load,
		freemem: sysinfo_raw.memory?.free,
		localtime: sysinfo_raw.localtime,
	};
}

// Channel info from iwinfo
let info = ubus.call("iwinfo", "info", { device: ifname });
if (info) {
	rv.oper_chan = info.channel;
	rv.oper_freq = info.frequency;

	// Channel activity from survey
	let survey = ubus.call("iwinfo", "survey", { device: ifname });
	if (survey?.results) {
		for (let s in survey.results) {
			if (s.mhz == info.frequency && prev.survey) {
				let active = s.active_time - prev.survey.active_time;
				let busy = s.busy_time - prev.survey.busy_time;
				if (active > 0)
					rv.activity = (busy * 100) / active;
			}
			if (s.mhz == info.frequency)
				prev.survey = s;
		}
	}

	rv.noise = info.noise ?? 0;
}

// Channel width from ath9k debugfs
let chanbw = read_output("cat /sys/kernel/debug/ieee80211/" + phy + "/ath9k/chanbw 2>/dev/null");
rv.chan_width = chanbw ? +(split(chanbw, ".")[0]) : 20;

let lna = read_output("rf-ctrl 0 7 2>/dev/null");
rv.lna_status = +(lna ?? 1);

// Per-station stats via nl80211
let stations = iface_assoclist();
rv.sta_stats = [];
for (let mac, info in stations) {
	let sta = { mac, rssi: info.rssi, rssi_ant: info.rssi_ant, mcs: info.mcs, inactive: info.inactive };
	if (prev.stations?.[mac]) {
		let p = prev.stations[mac];
		let tx_packets = info.tx_packets - p.tx_packets;
		let tx_retries = info.tx_retries - p.tx_retries;
		sta.tx_packets = tx_packets;
		sta.tx_bytes = info.tx_bytes - p.tx_bytes;
		sta.tx_retries = tx_retries;
		sta.tx_failed = info.tx_failed - p.tx_failed;
		sta.pl_ratio = (tx_retries + tx_packets > 0) ? (tx_retries * 100.0) / (tx_retries + tx_packets) : 0.0;
	}
	push(rv.sta_stats, sta);
}
prev.stations = stations;

// batman-adv mesh stats
let batcheck = read_output("[[ -d /sys/devices/virtual/net/bat* ]] && echo 1");
if (batcheck == "1") {
	rv.mesh_stats = [];
	let origs_raw = read_output("/usr/sbin/batctl oj 2>/dev/null");
	if (origs_raw) {
		let origs = json(origs_raw);
		if (type(origs) == "array") {
			for (let entry in origs) {
				if (entry.best == true) {
					push(rv.mesh_stats, {
						orig_address: entry.orig_address,
						tq: entry.tq,
						hop_status: (entry.neigh_address == entry.orig_address) ? "direct" : "hop",
						last_seen_msecs: entry.last_seen_msecs,
					});
				}
			}
		}
	}
}

fs.writefile(linkstate_raw, sprintf("%J", prev));
fs.writefile(linkstate_current, sprintf("%J", rv));
ubus.event("linkstate", rv);
ubus.disconnect();
