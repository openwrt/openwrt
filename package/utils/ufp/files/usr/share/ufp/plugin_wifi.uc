import * as struct from "struct";
let ubus, uloop, global, timer;
let ap_cache = {};

const ie_tags = {
	PWR_CAPABILITY: 33,
	HT_CAP: 45,
	EXT_CAPAB: 127,
	VHT_CAP: 191,
	__EXT_START: 0x100,
	HE_CAP: 0x100 | 35,
	VENDOR_WPS: 0x0050f204,
};

const ie_parser_proto = {
	reset: function() {
		this.offset = 0;
	},

	parseAt: function(offset) {
		let hdr = substr(this.buffer, offset, 2);
		if (length(hdr) != 2)
			return null;

		let data = this.hdr.unpack(hdr);
		if (length(data != 2))
			return null;

		let len = data[1];
		offset += 2;
		data[1] += 2;

		if (data[0] == 221 && len >= 4) {
			hdr = substr(this.buffer, offset, 4);
			if (length(hdr) != 4)
				return null;

			let val = this.vendor_hdr.unpack(hdr);
			if (length(val) != 1 || val[0] < 0x200)
				return null;

			data[0] = val[0];
			len -= 4;
			offset += 4;
		} else if (data[0] == 255 && len >= 1) {
			hdr = substr(this.buffer, offset, 2);
			if (length(hdr) != 2)
				return null;
			data[0] = 0x100 + this.hdr.unpack(hdr)[0];
			len -= 1;
			offset += 1;
		}

		data[2] = data[1];
		data[1] = substr(this.buffer, offset, len);
		if (length(data[1]) != len)
			return null;

		return data;
	},

	next: function() {
		let data = this.parseAt(this.offset);
		if (!data)
			return null;

		this.offset += data[2];
		return data;
	},

	foreach: function(cb) {
		let offset = 0;
		let data;

		while ((data = this.parseAt(offset)) != null) {
			offset += data[2];
			let ret = cb(data);
			if (type(ret) == "boolean" && !ret)
				break;
		}
	},
};

function ie_parser(data) {
	let parser = {
		offset: 0,
		buffer: data,
		hdr: struct.new("BB"),
		vendor_hdr: struct.new(">I"),
	};

	proto(parser, ie_parser_proto);

	return parser;
}

function format_fn(unpack_str, format)
{
	return (data) => {
		data = struct.unpack(unpack_str, data);
		if (data && data[0])
			data = data[0];
		else
			data = 0;
		return sprintf(format, data)
	};
}

let unpack;
unpack = {
	u8: format_fn("B", "%02x"),
	le16: format_fn("<H", "%04x"),
	le32: format_fn("<I", "%08x"),
	bytes: (data) => join("", map(split(data, ""), unpack.u8)),
};
let fingerprint_order = [
	"htcap", "htagg", "htmcs", "vhtcap", "vhtrxmcs", "vhttxmcs",
	"txpow", "extcap", "wps", "hemac", "hephy"
];

function format_wps_ie(data) {
	let offset = 0;
	let len = length(data);
	let s = struct.new("<HH");

	while (offset + 4 <= len) {
		let hdr = s.unpack(substr(data, offset, 4));
		let val = substr(data, offset + 4, hdr[1]);

		offset += 4 + hdr[1];
		if (hdr[0] != 0x1023)
			continue;

		if (length(val) != hdr[1])
			break;

		return replace(val, /[^A-Za-z0-9]/, "_");
	}

	return null;
}

function ie_fingerprint_str(id) {
	if (id >= 0x200)
		return sprintf("221(%08x)", id);
	if (id >= 0x100)
		return sprintf("255(%d)", id - 0x100);
	return sprintf("%d", id);
}

let vendor_ie_filter = [
	0x0050f2, // Microsoft WNN
	0x506f9a, // WBA
	0x8cfdf0, // Qualcom
	0x001018, // Broadcom
	0x000c43, // Ralink
];

function ie_fingerprint(data, mode) {
	let caps = {
		tags: [],
		vendor_list: {}
	};
	let parser = ie_parser(data);

	parser.foreach(function(ie) {
		let skip = false;
		let val = ie[1];
		switch (ie[0]) {
		case ie_tags.HT_CAP:
			caps.htcap = unpack.le16(substr(val, 0, 2));
			caps.htagg = unpack.u8(substr(val, 2, 1));
			caps.htmcs = unpack.le32(substr(val, 3, 4));
			break;
		case ie_tags.VHT_CAP:
			caps.vhtcap = unpack.le32(substr(val, 0, 4));
			caps.vhtrxmcs = unpack.le32(substr(val, 4, 4));
			caps.vhttxmcs = unpack.le32(substr(val, 8, 4));
			break;
		case ie_tags.EXT_CAPAB:
			caps.extcap = unpack.bytes(val);
			break;
		case ie_tags.PWR_CAPABILITY:
			caps.txpow = unpack.le16(val);
			break;
		case ie_tags.VENDOR_WPS:
			caps.wps = format_wps_ie(val);
			break;
		case ie_tags.HE_CAP:
			if (mode != "wifi6") {
				skip = true;
				break;
			}
			caps.hemac =
				unpack.le16(substr(val, 4, 2)) +
				unpack.le32(substr(val, 0, 4));
			caps.hephy =
				unpack.le16(substr(val, 15, 2)) +
				unpack.le32(substr(val, 11, 4)) +
				unpack.le32(substr(val, 7, 4));
			break;
		}
		if (ie[0] > 0x200) {
			let vendor = ie[0] >> 8;
			if (!(vendor in vendor_ie_filter))
				caps.vendor_list[sprintf("%06x", vendor)] = 1;
		}
		if (!skip)
			push(caps.tags, ie[0]);
		return null;
	});

	switch (mode) {
	case "wifi6":
		if (mode == "wifi6" && !caps.hemac)
			return null;
		break;
	case "wifi-vendor-oui":
		return caps.vendor_list;
	default:
		break;
	}

	let tags = map(caps.tags, ie_fingerprint_str);
	return
		join(",", tags) + "," +
		join(",", map(
			filter(fingerprint_order, (key) => !!caps[key]),
			(key) => `${key}:${caps[key]}`
		));
}

function fingerprint(mac, mode, ies) {
	switch (mode) {
	case "wifi4":
		if (!ies.assoc_ie)
			break;

		let assoc = ie_fingerprint(ies.assoc_ie, mode);
		if (!assoc)
			break;

		global.device_add_data(mac, `${mode}|${assoc}`);
		break;
	case "wifi-vendor-oui":
		let list = ie_fingerprint(ies.assoc_ie, mode);
		for (let oui in list) {
			global.device_add_data(mac, `${mode}-${oui}|1`);
		}
		break;
	case "wifi6":
	default:
		let val = ie_fingerprint(ies.assoc_ie, mode);
		if (!val)
			break;

		global.device_add_data(mac, `${mode}|${val}`);
		break;
	}
}

const fingerprint_modes = [ "wifi4", "wifi6", "wifi-vendor-oui" ];

function client_refresh(ap, mac, prev_cache)
{
	let ies = ubus.call(ap, "get_sta_ies", { address: mac });
	if (type(ies) != "object" || !ies.assoc_ie)
		return null;

	ies.assoc_ie = b64dec(ies.assoc_ie);
	if (ies.probe_ie)
		ies.probe_ie = b64dec(ies.probe_ie);

	for (let mode in fingerprint_modes)
		fingerprint(mac, mode, ies);

	return ies;
}

function refresh()
{
	let ap_objs = filter(ubus.list(), (name) => match(name, /^hostapd\./));
	let prev_cache = ap_cache;
	ap_cache = {};

	timer.set(30 * 1000);
	for (let ap in ap_objs) {
		try {
			let cur_cache = {};
			let prev_ap_cache = prev_cache[ap] ?? {};

			ap_cache[ap] = cur_cache;

			let clients = ubus.call(ap, "get_clients").clients;
			for (let client in clients) {
				let client_cache = prev_ap_cache[client];
				if (!client_cache || !client_cache.assoc_ie || !client_cache.probe_ie)
					client_cache = client_refresh(ap, client);
				global.device_refresh(client);
			}
		} catch (e) {
		}
	}
}

function init(gl) {
	global = gl;
	ubus = gl.ubus;
	uloop = gl.uloop;

	global.add_weight({
		wifi4: 2.0,
		wifi6: 3.0,
		"wifi-vendor-oui": 2.0
	});

	timer = uloop.timer(1000, refresh);
}

return { init, refresh };
