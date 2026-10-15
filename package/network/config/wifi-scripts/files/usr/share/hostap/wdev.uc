#!/usr/bin/env ucode
'use strict';
import { vlist_new, is_equal, wdev_set_mesh_params, wdev_remove, wdev_set_up, phy_open, macaddr_keep, macaddr_sync } from "/usr/share/hostap/common.uc";
import { readfile, writefile, basename, readlink, glob } from "fs";
let libubus = require("ubus");

let keep_devices = {};
let macaddr_options = {};
let phy_name = shift(ARGV);
let command = shift(ARGV);
let phydev;
let ubus;

function iface_stop(wdev)
{
	if (keep_devices[wdev.ifname])
		return;

	wdev_remove(wdev.ifname);
}

function netdev_macaddr(ifname)
{
	let addr = readfile(`/sys/class/net/${ifname}/address`);

	return addr ? trim(addr) : null;
}

function iface_macaddr(wdev)
{
	let ifname = wdev.ifname;
	let ret = { error: "ubus is not reachable", transport: true };

	if (ubus)
		ret = phydev.macaddr_get(ubus, "wdev", ifname, {
			...macaddr_options,
			group: phydev.name,
			ifname,
			macaddr: wdev.macaddr,
			static: !!wdev.macaddr,
		});

	if (ret.macaddr)
		return ret.macaddr;

	let prev = ret.transport ? (wdev.macaddr ?? netdev_macaddr(ifname)) : null;
	warn(`No MAC address from netifd for ${ifname}: ${ret.error}${prev ? ", using " + prev : ""}\n`);

	return prev;
}

function iface_start(wdev)
{
	let ifname = wdev.ifname;
	let macaddr;

	if (wdev.mode != "monitor")
		macaddr = iface_macaddr(wdev);

	if (readfile(`/sys/class/net/${ifname}/ifindex`)) {
		wdev_set_up(ifname, false);
		wdev_remove(ifname);
	}
	if (!macaddr && wdev.mode != "monitor")
		return;

	let wdev_config = {};
	for (let key in wdev)
		wdev_config[key] = wdev[key];
	if (macaddr)
		wdev_config.macaddr = macaddr;
	let err = phydev.wdev_add(ifname, wdev_config);
	if (err) {
		warn(`Failed to create ${ifname}: ${err}\n`);
		return;
	}

	err = wdev_set_up(ifname, true);
	if (err) {
		warn(`Failed to bring up ${ifname}: ${err}\n`);
		wdev_remove(ifname);
		return;
	}

	let htmode = wdev.htmode || "NOHT";
	if (wdev.freq)
		system(`iw dev ${ifname} set freq ${wdev.freq} ${htmode}`);
	if (wdev.mode == "adhoc") {
		let cmd = ["iw", "dev", ifname, "ibss", "join", wdev.ssid, wdev.freq, htmode, "fixed-freq" ];
		if (wdev.bssid)
			push(cmd, wdev.bssid);
		for (let key in [ "beacon-interval", "basic-rates", "mcast-rate", "keys" ])
			if (wdev[key])
				push(cmd, key, wdev[key]);
		system(cmd);
	} else if (wdev.mode == "mesh") {
		let cmd = [ "iw", "dev", ifname, "mesh", "join", wdev.ssid ];
		if (wdev.freq) {
			push(cmd, "freq", wdev.freq);
			if (htmode && htmode != "NOHT")
				push(cmd, htmode);
		}
		for (let key in [ "basic-rates", "mcast-rate", "beacon-interval" ])
			if (wdev[key])
				push(cmd, key, wdev[key]);
		system(cmd);

		wdev_set_mesh_params(ifname, wdev);
	}
}

function iface_cb(new_if, old_if)
{
	if (old_if && new_if && is_equal(old_if, new_if))
		return;

	if (old_if)
		iface_stop(old_if);
	if (new_if)
		iface_start(new_if);
}

function wdev_on_radio(wdev)
{
	let mask = wdev.vif_radio_mask;

	return phydev.radio == null || !mask || (mask & (1 << phydev.radio));
}

// A name in the state file can belong to another owner by now, so match
// the wdev ids. A state file of an older version has names only.
function drop_inactive(config, wdev_ids)
{
	let wdevs = {};
	for (let wdev in phydev.wdev_list())
		wdevs[wdev.ifname] = wdev;

	for (let key in config) {
		let wdev = wdevs[key];
		if (!wdev || !wdev_on_radio(wdev) || (wdev_ids && wdev_ids[key] != wdev.wdev))
			delete config[key];
	}
}

function wdev_ids_get(config)
{
	let wdev_ids = {};
	for (let wdev in phydev.wdev_list())
		if (config[wdev.ifname])
			wdev_ids[wdev.ifname] = wdev.wdev;

	return wdev_ids;
}

function add_ifname(config)
{
	for (let key in config)
		config[key].ifname = key;
}

function delete_ifname(config)
{
	for (let key in config)
		delete config[key].ifname;
}

function usage()
{
	warn(`Usage: ${basename(sourcepath())} <phy> <command> [<arguments>]

Commands:
	set_config <config> [<option>=<value>|<device>]...
					  - set phy configuration; options:
					    num_global, macaddr_base
	reset				  - remove the interfaces of the phy
`);
	exit(1);
}

const statefile = `/var/run/wdev-${phy_name}.json`;
const idfile = `/var/run/wdev-${phy_name}.id.json`;

function state_load()
{
	let config = readfile(statefile);
	if (config)
		config = json(config);
	if (type(config) != "object")
		config = {};

	let wdev_ids = readfile(idfile);
	if (wdev_ids)
		wdev_ids = json(wdev_ids);

	add_ifname(config);
	drop_inactive(config, type(wdev_ids) == "object" ? wdev_ids : null);

	return config;
}

function state_save(config)
{
	writefile(statefile, sprintf("%J", config));
	writefile(idfile, sprintf("%J", wdev_ids_get(config)));
}

function macaddr_prune(config)
{
	let names = filter(keys(config), (ifname) => config[ifname].mode != "monitor");

	if (!macaddr_sync(ubus, "wdev", phydev.name, macaddr_keep(names), macaddr_options))
		warn(`Could not release the MAC addresses of removed interfaces: ${ubus.error()}\n`);
}

const commands = {
	set_config: function(args) {
		let new_config = shift(args);
		for (let arg in args) {
			let val = split(arg, "=", 2);
			if (length(val) < 2)
				keep_devices[arg] = true;
			else if (index([ "num_global", "macaddr_base" ], val[0]) >= 0 && val[1] != "")
				macaddr_options[val[0]] = val[1];
		}

		if (!new_config)
			usage();

		new_config = json(new_config);
		if (!new_config) {
			warn("Invalid configuration\n");
			exit(1);
		}

		let config = vlist_new(iface_cb);
		config.data = state_load();

		ubus = libubus.connect();
		macaddr_prune(new_config);

		add_ifname(new_config);
		config.update(new_config);
		ubus?.disconnect();

		drop_inactive(config.data);
		delete_ifname(config.data);
		state_save(config.data);
	},
	// netifd waits for this at startup and cannot answer ubus calls
	reset: function(args) {
		for (let ifname in state_load())
			wdev_remove(ifname);

		state_save({});
	},
};

if (!phy_name || !command || !commands[command])
	usage();

let phy_split = split(phy_name, ":");
phydev = phy_open(phy_split[0], phy_split[1]);
if (!phydev) {
	warn(`PHY ${phy_name} does not exist\n`);
	exit(1);
}

commands[command](ARGV);
