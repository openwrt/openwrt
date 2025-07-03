let ubus, global, last_refresh;
let rtnl = require("rtnl");

const homekit_types = {
	"2": "Bridge",
	"3": "Fan",
	"4": "Garage Door Opener",
	"5": "Lightbulb",
	"6": "Door Lock",
	"7": "Outlet",
	"8": "Switch",
	"9": "Thermostat",
	"10": "Sensor",
	"11": "Security System",
	"12": "Door",
	"13": "Window",
	"14": "Window Covering",
	"15": "Programmable Switch",
	"17": "IP Camera",
	"18": "Video Doorbell",
	"19": "Air Purifier",
	"20": "Heater",
	"21": "Air Conditioner",
	"22": "Humidifier",
	"23": "Dehumidifier",
	"28": "Sprinkler",
	"29": "Faucet",
	"30": "Shower System",
	"31": "Television",
	"32": "Remote Control",
	"33": "WiFi Router",
	"34": "Audio Receiver",
	"35": "TV Set Top Box",
	"36": "TV Stick",
};

function get_arp_cache() {
	let ret = {};

	let cache = rtnl.request(rtnl.const.RTM_GETNEIGH, rtnl.const.NLM_F_DUMP, {});
	for (let data in cache) {
		if (!data.lladdr || !data.dst)
			continue;

		ret[data.dst] = data.lladdr;
	}

	return ret;
}

function find_arp_entry(arp, addrs)
{
	for (let addr in addrs) {
		let val = arp[addr];
		if (val)
			return val;
	}
	return null;
}

function get_host_addr_cache()
{
	let arp = get_arp_cache();
	let host_cache = ubus.call("umdns", "hosts", { array: true });
	let host_addr = {};
	for (let host in host_cache) {
		let host_data = host_cache[host];
		host_addr[host] = find_arp_entry(arp, host_data.ipv4) ??
				  find_arp_entry(arp, host_data.ipv6);
	}

	return host_addr;
}

function convert_txt_array(data) {
	let txt = {};

	for (let rec in data) {
		rec = split(rec, "=", 2);
		if (rec[1])
			txt[rec[0]] = rec[1];
	}

	return txt;
}

function handle_apple(txt, name)
{
	let ret = [];
	let model = txt.model ?? txt.rpMd ?? txt.am;
	if (model)
		push(ret, `apple_model|${model}`);
	if (name)
		push(ret, `%device_name|mdns_implicit_device_name|${name}`);

	return ret;
}

function handle_homekit(txt)
{
	let ret = [];
	if (txt.ci) {
		let type = homekit_types[txt.ci];
		if (type)
			push(ret, `%class|homekit_class|${type}`);
	}

	if (txt.md)
		push(ret, `%device|mdns_model_string|${txt.md}`);

	return ret;
}

function handle_googlecast_model(txt)
{
	let ret = [];
	let model = txt.model ?? txt.rpMd ?? txt.md;
	if (model)
		push(ret, `%device|mdns_model_string|${model}`);
	if (txt.fn)
		push(ret, `%device_name|mdns_device_name|${txt.fn}`);
	if (txt.rs == 'TV')
		push(ret, "%class|mdns_tv|TV");
	return ret;
}

function handle_printer(txt)
{
	let ret = [];
	let vendor = txt.usb_MFG;
	let model = txt.usb_MDL ?? txt.ty ?? replace(txt.product, /^\((.*)\)$/, "$1");
	let weight = (txt.usb_MFG && txt.usb_MDL) ? "mdns_vendor_model_string" : "mdns_model_string";
	if (vendor)
		push(ret, `%vendor|${weight}|${vendor}`);
	if (model)
		push(ret, `%device|${weight}|${model}`);
	push(ret, "%class|mdns_printer|Printer");

	return ret;
}

function handle_scanner(txt)
{
	let ret = [];
	let vendor = txt.mfg;
	let model = txt.mdl ?? txt.ty;
	let weight = (txt.mfg && txt.mdl) ? "mdns_vendor_model_string" : "mdns_model_string";
	if (vendor)
		push(ret, `%vendor|${weight}|${vendor}`);
	if (model)
		push(ret, `%device|${weight}|${model}`);
	push(ret, "%class|mdns_scanner|Scanner");

	return ret;
}

function handle_hue(txt, name)
{
	let ret = [];
	push(ret, `%vendor|mdns_service|Philips`);
	push(ret, `%device|mdns_service|Hue`);
	if (name)
		push(ret, `%device_name|mdns_implicit_device_name|${name}`);

	return ret;
}

function handle_fritzbox(txt)
{
	let ret = [];
	push(ret, `%vendor|mdns_service|AVM`);
	push(ret, `%device|mdns_service|FRITZ!Box`);

	return ret;
}

const service_handler = {
	"_airplay._tcp": handle_apple,
	"_companion-link._tcp": handle_apple,
	"_raop._tcp": (txt) => handle_apple(txt), // skip name
	"_googlecast._tcp": handle_googlecast_model,
	"_ipp._tcp": handle_printer,
	"_scanner._tcp": handle_scanner,
	"_hap._tcp": handle_homekit,
	"_hap._udp": handle_homekit,
	"_hue._tcp": handle_hue,
	"_fbox._tcp": handle_fritzbox,
	"_avmnexus._tcp": handle_fritzbox,
};

function arp_resolve(list)
{
	let host_cache = ubus.call("umdns", "hosts", { array: true });
	for (let entry in list) {
		let iface = entry[0];
		let host = entry[1];
		if (!host_cache[host])
			continue;
		for (let addr in host_cache[host].ipv4)
			rtnl.request(rtnl.const.RTM_NEWNEIGH,
				     rtnl.const.NLM_F_CREATE | rtnl.const.NLM_F_REPLACE,
				     { dev: iface, state: rtnl.const.NUD_INCOMPLETE,
				       flags: rtnl.const.NTF_USE, family: rtnl.const.AF_INET,
				       dst: addr });
	}
}

function refresh() {
	if (last_refresh == time())
		return;

	let host_cache = get_host_addr_cache();
	let query_list = [];
	let arp_pending = [];
	let mdns_data;

	for (let i = 0; i < 2; i++) {
		mdns_data = ubus.call("umdns", "browse", { array: true, address: false });
		for (let service in mdns_data) {
			if (!service_handler[service])
				continue;
			let service_data = mdns_data[service];
			for (let host in service_data) {
				let host_entry = service_data[host].host;
				let iface = service_data[host].iface;
				if (!iface)
					continue;
				if (!host_entry)
					push(query_list, [ `${host}.${service}.local`, iface ]);
				else if (!host_cache[host_entry])
					push(arp_pending, [ iface, host_entry ]);
			}
		}

		if (!length(arp_pending) && !length(query_list))
			break;

		if (length(arp_pending) > 0)
			arp_resolve(arp_pending);

		if (length(query_list) > 0) {
			for (let query in query_list)
				ubus.call("umdns", "query", { question: query[0], interface: query[1] });
		}

		sleep(1000);

		host_cache = get_host_addr_cache();
		mdns_data = ubus.call("umdns", "browse", { array: true, address: false });
	}

	for (let service in mdns_data) {
		if (!service_handler[service])
			continue;

		let service_data = mdns_data[service];
		for (let host in service_data) {
			let txt = service_data[host].txt;
			if (!txt)
				continue;

			let host_entry = service_data[host].host;
			if (!host_entry)
				continue;

			let mac = host_cache[host_entry];
			if (!mac)
				continue;

			txt = convert_txt_array(txt);
			let match = service_handler[service](txt, host);
			for (let info in match)
				global.device_add_data(mac, info);

			global.device_refresh(mac);
		}
	}
}

function init(gl) {
	global = gl;
	ubus = gl.ubus;

	global.add_weight({
		apple_model: 10.0,
		mdns_device_name: 10.0,
		mdns_implicit_device_name: 5.0,
		mdns_vendor_model_string: 10.0,
		mdns_service: 10.0,
		mdns_tv: 5.0,
		mdns_model_string: 5.0,
		mdns_printer: 5.0,
		mdns_scanner: 1.0,
		homekit_class: 2.0,
	});
}

return { init, refresh };
