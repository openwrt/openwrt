'use strict';

import * as nl80211 from "nl80211";
import { readfile } from "fs";

const MACADDR_IDS = 32;
const MACADDR_RADIO_IDS = 16;

// see identical_mac_addr_allowed() in mac80211
const shared_iftypes = [
	nl80211.const.NL80211_IFTYPE_AP_VLAN,
	nl80211.const.NL80211_IFTYPE_MONITOR,
	nl80211.const.NL80211_IFTYPE_P2P_DEVICE,
];

let entries = {};
let reserved = {};

function log_warn(msg)
{
	netifd.log(netifd.L_WARNING, `wireless: ${msg}\n`);
}

function phy_sysfs_file(phy, name)
{
	let data = readfile(`/sys/class/ieee80211/${phy}/${name}`);

	return data != null ? trim(data) : null;
}

function phy_index(phy)
{
	let idx = phy_sysfs_file(phy, "index");

	return idx != null ? int(idx) : null;
}

function phy_info(phy)
{
	let macaddress = phy_sysfs_file(phy, "macaddress");
	let mask = phy_sysfs_file(phy, "address_mask");
	if (!macaddress || !mask)
		return null;

	return {
		macaddress, mask,
		addrs: split(phy_sysfs_file(phy, "addresses") ?? "", "\n"),
	};
}

function macaddr_split(str)
{
	return map(split(str, ":"), (val) => hex(val));
}

function macaddr_join(addr)
{
	return join(":", map(addr, (val) => sprintf("%02x", val)));
}

// mac80211_hwsim repeats a permanent address in the wiphy address list
function macaddr_radio_base(addrs, radio_idx, base_addr)
{
	if (!radio_idx)
		return null;

	let addr = addrs[radio_idx];
	if (!addr || addr == base_addr)
		return null;
	if (length(filter(addrs, (val) => val == addr)) > 1)
		return null;

	return addr;
}

// Radios without an address of their own share the wiphy base address,
// and each takes its own slice of the addresses derived from it.
function macaddr_radio_slot(addrs, radio_idx, base_addr, idx)
{
	if (!radio_idx && macaddr_radio_base(addrs, 1, base_addr))
		return idx;
	if (idx >= MACADDR_RADIO_IDS)
		return null;

	return idx + radio_idx * MACADDR_RADIO_IDS;
}

function macaddr_generate(info, radio_idx, options, idx)
{
	let mbssid = options.mbssid > 0;
	let num_global = options.num_global;
	let use_global = !mbssid && idx < num_global;
	let base_addr = info.macaddress;
	let base_mask = info.mask;

	if (base_mask == "00:00:00:00:00:00")
		base_mask = "ff:ff:ff:ff:ff:ff";

	if (options.macaddr_base)
		base_addr = options.macaddr_base;
	else if (base_mask == "ff:ff:ff:ff:ff:ff" &&
	    (radio_idx > 0 || idx >= num_global)) {
		let addrs = info.addrs;

		let radio_addr = macaddr_radio_base(addrs, radio_idx, base_addr);

		if (radio_idx == null) {
			addrs = uniq(addrs);
			if (idx < length(addrs))
				return addrs[idx];
		} else if (radio_addr) {
			base_addr = radio_addr;
		} else {
			idx = macaddr_radio_slot(addrs, radio_idx, base_addr, idx);
			if (idx == null)
				return null;
			use_global = false;
		}
	}

	if (!idx && !mbssid)
		return base_addr;

	let addr = macaddr_split(base_addr);
	let mask = macaddr_split(base_mask);
	let type;

	if (mbssid)
		type = "b5";
	else if (use_global)
		type = "add";
	else if (mask[0] > 0)
		type = "b1";
	else if (mask[5] < 0xff)
		type = "b5";
	else
		type = "add";

	switch (type) {
	case "b1":
		if (!(addr[0] & 2))
			idx--;
		addr[0] |= 2;
		addr[0] ^= idx << 2;
		break;
	case "b5":
		if (mbssid)
			addr[0] |= 2;
		addr[5] ^= idx;
		break;
	default:
		for (let i = 5; i > 0; i--) {
			addr[i] += idx;
			if (addr[i] < 256)
				break;
			addr[i] %= 256;
		}
		break;
	}

	if (length(filter(addr, (val) => val > 0xff)))
		return null;

	return macaddr_join(addr);
}

function int_get(val, def)
{
	val = int(val ?? def);

	return val == val ? val : def;
}

function options_get(args)
{
	return {
		num_global: int_get(args.num_global, 1),
		macaddr_base: lc(args.macaddr_base ?? ""),
		mbssid: int_get(args.mbssid, 0) > 0 ? 1 : 0,
	};
}

function options_equal(a, b)
{
	return a.num_global == b.num_global &&
	       a.macaddr_base == b.macaddr_base &&
	       a.mbssid == b.mbssid;
}

function radio_get(radio)
{
	radio = int_get(radio, -1);

	return radio >= 0 ? radio : null;
}

function entries_prune()
{
	let present = {};

	for (let key, entry in entries) {
		present[entry.phy] ??= phy_index(entry.phy) != null;
		if (!present[entry.phy])
			delete entries[key];
	}
}

function wdev_list(phy)
{
	let idx = phy_index(phy);
	if (idx == null)
		return [];

	return nl80211.request(nl80211.const.NL80211_CMD_GET_INTERFACE,
			       nl80211.const.NLM_F_DUMP, { wiphy: idx }) ?? [];
}

function entry_key(owner, name)
{
	return owner + "/" + name;
}

function entry_shares(entry, ctx)
{
	return entry.share != null && entry.share == ctx.name;
}

// A stale entry keeps its address for its name. A new name takes it only
// when the range is full; a request that names the address always does.
function macaddr_holder(ctx, macaddr)
{
	let share = entries[ctx.share_key];
	let has_entry;

	for (let key, entry in entries) {
		if (entry.phy != ctx.phy || entry.macaddr != macaddr)
			continue;
		if (ctx.skip[key]) {
			has_entry = true;
			continue;
		}
		if (entry_shares(entry, ctx) || (entry.stale && ctx.yield))
			continue;

		return key;
	}

	if (reserved[macaddr] && !ctx.allow_reserved &&
	    share?.macaddr != macaddr)
		return "config";

	if (has_entry)
		return null;

	for (let wdev in ctx.wdevs) {
		if (index(shared_iftypes, wdev.iftype) >= 0)
			continue;
		if (wdev.mac != macaddr || wdev.ifname == ctx.ifname)
			continue;

		return "wdev:" + wdev.ifname;
	}

	return null;
}

function macaddr_next(ctx, info, radio, options)
{
	for (let pass = 0; pass < 2; pass++) {
		let pass_ctx = { ...ctx, yield: pass > 0 };

		for (let id = 0; id < MACADDR_IDS; id++) {
			let addr = macaddr_generate(info, radio, options, id);
			if (!addr)
				break;

			if (!macaddr_holder(pass_ctx, addr))
				return addr;
		}
	}

	return null;
}

function stale_drop(phy, macaddr, key)
{
	for (let cur_key, entry in entries)
		if (entry.stale && cur_key != key && entry.phy == phy &&
		    entry.macaddr == macaddr)
			delete entries[cur_key];
}

// a configured address comes back with the configuration
function entry_release(key)
{
	if (entries[key].static)
		delete entries[key];
	else
		entries[key].stale = true;
}

function entry_set(key, args, data)
{
	stale_drop(args.phy, data.macaddr, key);
	entries[key] = {
		owner: args.owner,
		name: args.name,
		group: args.group ?? "",
		phy: args.phy,
		radio: radio_get(args.radio),
		ifname: args.ifname,
		share: args.share,
		any_radio: !!args.any_radio,
		...data,
	};

	return { macaddr: data.macaddr };
}

function entry_reusable(entry, args, radio, options)
{
	if (!entry || entry.static || entry.phy != args.phy)
		return false;
	if (args.any_radio)
		return true;

	return entry.radio == radio && options_equal(entry.options, options);
}

export function macaddr_get(args)
{
	if (!args.phy || !args.owner || !args.name)
		return null;

	entries_prune();

	let info = phy_info(args.phy);
	if (!info)
		return { error: "phy" };

	let key = entry_key(args.owner, args.name);
	let entry = entries[key];
	let radio = radio_get(args.radio);
	let options = options_get(args);
	let macaddr = args.macaddr ? lc(args.macaddr) : null;
	let share_key = args.share ? entry_key(args.owner, args.share) : null;
	let ctx = {
		phy: args.phy,
		name: args.name,
		skip: { [key]: true },
		share_key,
		ifname: args.ifname,
		wdevs: wdev_list(args.phy),
		yield: true,
	};
	if (share_key)
		ctx.skip[share_key] = true;

	if (macaddr && args.static) {
		ctx.allow_reserved = true;
		let holder = macaddr_holder(ctx, macaddr);
		if (holder) {
			log_warn(`${key}: ${macaddr} is in use by ${holder}`);
			return { error: "conflict", holder };
		}

		return entry_set(key, args, { macaddr, static: true, options });
	}

	if (macaddr && !macaddr_holder(ctx, macaddr))
		return entry_set(key, args, { macaddr, static: false, options });

	if (entry_reusable(entry, args, radio, options) &&
	    !macaddr_holder(ctx, entry.macaddr))
		return entry_set(key, args, { macaddr: entry.macaddr, static: false, options });

	let addr = macaddr_next(ctx, info, radio, options);
	if (!addr)
		return { error: "exhausted" };

	return entry_set(key, args, { macaddr: addr, static: false, options });
};

function sync_value_apply(args, name, val, prev, options, taken)
{
	let macaddr = lc(val.macaddr ?? "");
	let base = val.from != null ? prev[val.from] : prev[name];
	let phy = val.phy ?? base?.phy;
	if (!macaddr || !phy || taken[macaddr])
		return null;

	let key = entry_key(args.owner, name);
	let share_key = val.share ? entry_key(args.owner, val.share) : null;
	// no wdev dump: hostapd renames the netdev of a renamed BSS later
	let ctx = {
		phy,
		name,
		skip: { [key]: true },
		share_key,
		allow_reserved: !!val.static,
		wdevs: [],
		yield: true,
	};

	for (let cur_key, entry in entries)
		if (entry.owner == args.owner && entry.group == args.group)
			ctx.skip[cur_key] = true;
	if (share_key)
		ctx.skip[share_key] = true;

	if (macaddr_holder(ctx, macaddr))
		return null;

	if (val.from == null && base?.macaddr != macaddr)
		base = null;

	let entry = base ? { ...base } : {
		owner: args.owner,
		group: args.group,
		options,
		static: !!val.static,
		share: val.share,
		any_radio: !!val.any_radio,
	};

	entry.name = name;
	entry.phy = phy;
	entry.macaddr = macaddr;
	if (val.radio != null)
		entry.radio = radio_get(val.radio);
	if (val.share != null)
		entry.share = val.share;
	if (val.static != null)
		entry.static = !!val.static;

	taken[macaddr] = { key, phy, share_key };

	return entry;
}

export function macaddr_sync(args)
{
	let list = args.macaddr ?? {};

	if (!args.owner || type(list) != "object")
		return null;

	args.group ??= "";
	entries_prune();

	let options = options_get(args);
	let prefix = entry_key(args.owner, "");
	let removed = [];
	let refused = [];
	let prev = {};

	for (let key, entry in entries)
		if (entry.owner == args.owner && entry.group == args.group)
			prev[entry.name] = entry;

	let from_names = {};
	for (let name, val in list)
		if (type(val) == "object" && val.from != null && prev[val.from])
			from_names[val.from] = true;
		else if (type(val) == "object")
			delete val.from;

	for (let name, entry in prev) {
		if (exists(list, name) || from_names[name] || entry.stale)
			continue;

		entry_release(prefix + name);
		push(removed, name);
	}

	let taken = {};
	let result = {};
	let moved = {};
	for (let name, val in list) {
		if (type(val) != "object")
			continue;

		let entry = sync_value_apply(args, name, val, prev, options, taken);
		if (!entry) {
			push(refused, name);
			continue;
		}

		result[name] = entry;
		if (val.from != null)
			moved[val.from] = true;
	}

	for (let name in from_names) {
		if (exists(list, name))
			continue;

		delete entries[prefix + name];
		if (!moved[name])
			push(removed, name);
	}

	for (let name, entry in result)
		entries[prefix + name] = entry;

	for (let key in keys(entries)) {
		let entry = entries[key];
		if (entry.owner != args.owner || result[entry.name] == entry)
			continue;
		if (entry.group != args.group && !exists(list, entry.name))
			continue;

		let holder = taken[entry.macaddr];
		if (holder && holder.key != key && holder.share_key != key &&
		    holder.phy == entry.phy) {
			delete entries[key];
			push(removed, entry.name);
			continue;
		}

		if (!exists(list, entry.name))
			continue;

		entry.group = args.group;
		delete entry.stale;
	}

	return { refused, removed };
};

export function macaddr_release(args)
{
	if (!args.owner || !args.name)
		return null;

	let key = entry_key(args.owner, args.name);
	let entry = entries[key];
	if (!entry)
		return {};

	entry_release(key);

	let to_key = args.to ? entry_key(args.owner, args.to) : null;
	if (!entries[key] || !to_key)
		return {};
	if (entries[to_key] && !(args.replace && entries[to_key].stale))
		return {};

	delete entries[key];
	entries[to_key] = {
		...entry,
		name: args.to,
		ifname: args.ifname ?? entry.ifname,
		share: entry.share != null ? (args.share ?? entry.share) : null,
	};

	return {};
};

export function macaddr_list(args)
{
	let ret = [];

	for (let key, entry in entries)
		if (!args.phy || entry.phy == args.phy)
			push(ret, entry);

	return {
		entries: ret,
		reserved: sort(keys(reserved)),
	};
};

export function owner_gone(owner)
{
	for (let key, entry in entries)
		if (entry.owner == owner)
			entry_release(key);
};

export function reserved_set(list)
{
	reserved = {};
	for (let addr in list)
		if (type(addr) == "string" && addr != "random")
			reserved[lc(addr)] = true;
};
