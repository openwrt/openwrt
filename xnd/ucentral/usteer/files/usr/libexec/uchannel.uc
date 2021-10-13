#!/usr/bin/ucode
{%
let fs = require("fs");
let ubus = require("ubus");
let conn = ubus.connect();
let phys = [];
let block_list = {
	"2G": {},
	"5G": {}
};
let channel_masks = {
	"40": [ 5180, 5220, 5260, 5300, 5500, 5540, 5580, 5620, 5660, 5700, 5745, 5785, 5825 ],
	"80": [ 5180, 5260, 5500, 5580, 5660, 5745 ],
	"160": [ 5180, 5500, ],
};

function uptime_get() {
	let info = conn.call("system", "info");

	return info.uptime;
}

function remote_info() {
	let info = conn.call("usteer", "remote_info");

	return info || {};
}

function local_info() {
	let info = conn.call("usteer", "local_info");

	return info || {};
}

function remote_hosts() {
	let hosts = conn.call("usteer", "remote_hosts");

	return hosts || {};
}

let uptime = uptime_get();
let info = local_info();
let remote = remote_info();
let hosts = remote_hosts();

function state_get() {
	let file = fs.open("/tmp/uchannel.json", "r");
	let state = file ? json(file.read("all")) : {};

	if (file)
		file.close();

	return state;
}

function state_set(state) {
	let file = fs.open("/tmp/uchannel.json", "w");

	state.uptime = uptime;
	file.write(state);
	file.close();

	conn.call("usteer", "set_node_data", {
		node: "*",
		data: {
			status: state.status,
			uptime: state.uptime,
		}
	});
	printf("entering %s state\n", state.status);
}

function freq2chan(freq) {
        if (freq == 2484)
                return 14;
        else if (freq < 2484)
                return (freq - 2407) / 5;
        else if (freq >= 4910 && freq <= 4980)
                return (freq - 4000) / 5;
        else if(freq >= 56160 + 2160 * 1 && freq <= 56160 + 2160 * 6)
                return (freq - 56160) / 2160;
        else
                return (freq - 5000) / 5;
}

function freq2band(freq) {
	if (freq < 2500)
		return "2G";
	return "5G";
}

function chan2freq(band, channel) {
	if (band == '2G' && channel >= 1 && channel <= 13)
		return 2407 + channel * 5;
	else if (band == '2G' && channel == 14)
		return 2484;
	else if (band == '5G' && channel >= 36 && channel <= 177)
		return 5000 + channel * 5;
	else if (band == '5G' && channel >= 183 && channel <= 196)
		return 4000 + channel * 5;
	else if (band == '60G' && channel >= 1 && channel <= 6)
		return 56160 + channel * 2160;

	return null;
}

function center_freq(freq, bandwidth) {
	if (bandwidth == 40)
		return +freq + 10;
	if (bandwidth == 80)
		return +freq + 30;
	if (bandwidth == 160)
		return +freq + 70;
	return +freq;
}

function channel_overlap() {
	let overlap = {};
	let peers = {
		"local": {}
	};

	for (let node, r in remote)
		peers[split(node, "#")] = {};

	for (let id, i in info) {
		peers.local[i.freq] = true;
		block_list[freq2band(freq)][i.freq] = 0;
	}
	for (let node, r in remote) {
		peers[split(node, "#")][r.freq] = true;
		block_list[freq2band(freq)][r.freq] = 0;
	}

	for (let id, peer in peers)
		for (let freq, val in peer)
			block_list[freq2band(freq)][freq]++;

	for (let id, i in info)
		for (let node, r in remote)
			if (i.freq == r.freq) {
				overlap[i.freq] = id;
				break;
			}

	return overlap;
}

function phy_lookup() {
	let status = conn.call("network.wireless", "status");

	for (let id, radio in status) {
		let htmode = match(radio.config.htmode, /^([A-Z]+)(.+)$/);
		let phy = {
			path: radio.config.path,
			htmode: lc(htmode[1]),
			bandwidth: htmode[2],
			iface: [],
			sta: false,
		};

		for (let i, iface in radio.interfaces) {

			push(phy.iface, iface.ifname);
			if (iface.config.mode != 'ap')
				phy.sta = true;
		}
		push(phys, phy);
	}
}

function phy_find(iface) {
	for (let idx, phy in phys)
		if (index(phy.iface, iface) >= 0)
			return phy;
	return {};
}

function channel_mask(band, bandwidth) {
	if (band == "2G")
		return [ 2412, 2437, 2462 ];
	return channel_masks[bandwidth];
}

function channel_scan(band) {
	conn.call("wifi", "scan", { band });
	sleep(5000);

	let survey_data = conn.call("wifi", "survey", { band });
	let scan_data = conn.call("wifi", "scan_dump", { band });

	let channels = {};

	for (let survey in survey_data.survey) {
		channels[survey.channel] = survey;
		channels[survey.channel].bss = 0;
	}

	for (let scan in scan_data.scan)
		channels[scan.channel].bss++;

	return channels;
}

function channel_new(band, channels, mask) {
	let new = [];

	for (let chan, data in channels) {
		if (data.in_use)
			continue;
		let freq = chan2freq(band, chan);
		if (length(mask) && index(mask, freq) < 0)
			continue;
		if (block_list[band][freq])
			continue;
		push(new, {
			freq,
			bss: data.bss,
			airtime: data.busy_ms * 100 / data.active_ms,
		});
	}
	print("available free channels :" + new + "\n");

	let best;
	for (let id, data in new) {
		if (!length(best))
			best = data;
		if (best.bss > data.bss)
			best = data;
		else if (best.bss == data.bss &&
			 best.airtime > data.airtime)
			best = data;
	}
	return best || {};
}

function channel_balance(band, mask) {
	let lowest = {
		freq: mask[0],
		count: 1000,
	};
	let highest = {
		freq: mask[0],
		count: 0,
	};

	for (let freq, count in block_list[band]) {
		if (lowest.count > count)
			lowest = { freq, count };
		if (highest.count < count)
			highest = { freq, count };
	}

	if (highest.count - lowest.cont >= 2)
		return lowest;

	return {};
}

function youngest() {
	for (let ip, host in hosts) {
		if (host.host_info.status == "overlap" &&
		    host.host_info.uptime < uptime) {
			print("Found a younger host\n");
			return 1;
		}
	}
	print("We are the youngest host\n");
	return 0;
}

let state = state_get();

if (state.status == "waiting" &&
    (uptime - state.uptime < (12 * 60 * 60))) {
	state_set(state);
	return;
}

phy_lookup();
print("discovered devices: " + phys + "\n");
let overlap = channel_overlap();
print("list of blocked channels: " + block_list + "\n");
print("list of overlapping channels: " + overlap + "\n");

if (!length(overlap)) {
	state.status = "happy";
	state_set(state);
	return;
}

if (state.status != "overlap" || youngest()) {
	state.status = "overlap";
	state_set(state);
	return;
}

for (let freq, obj in overlap) {
	let phy = phy_find(split(obj, ".")[1]);
	let band = freq2band(freq);
	let channels = channel_scan(band);
	let mask = channel_mask(band, phy.bandwidth);
	let new;

	if (phy.sta) {
		print("phy has a STA interface cannot change channel\n");
		return;
	}

	new = channel_new(band, channels, mask);

	if (!length(new))
		new = channel_balance(band, mask);

	if (!length(new)) {
		print("no alternative channel found\n");
		continue;
	}
	printf("selected channel: " + new + " for %s\n", obj);
	conn.call(obj, "switch_chan",  {
		freq: +new.freq,
		center_freq1: center_freq(new.freq, phy.bandwidth),
		bcn_count: 10,
		force: true
	});
}

state.status = "waiting";
state.changed = uptime;
state_set(state);
%}
