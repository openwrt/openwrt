import * as uci from "uci";
import * as libubus from "ubus";
import { ex_wrap } from "./utils.uc";
import { access, dirname } from "fs";

let ubus = netifd.ubus = libubus.connect();
let wireless;

function uci_ctx()
{
	let savedir = netifd.dummy_mode ? "./tmp" : null;
	let ctx = uci.cursor(netifd.config_path, savedir, null, {
		strict: false
	});
	return ctx;
}

function config_init()
{
	let ctx = uci_ctx();

	if (wireless)
		wireless.config_init(ctx);
}

function config_start()
{
	if (wireless)
		wireless.config_start();
}

function check_interfaces()
{
	if (wireless)
		wireless.check_interfaces();
}

function hotplug(name, add)
{
	if (wireless)
		wireless.hotplug(name, add);
}

netifd.cb = {
	hotplug: ex_wrap(hotplug),
	config_init: ex_wrap(config_init),
	config_start: ex_wrap(config_start),
	check_interfaces: ex_wrap(check_interfaces),
};

const wireless_module = dirname(sourcepath()) + "/wireless.uc";
if (access(wireless_module, "r")) {
	try {
		wireless = loadfile(wireless_module)();
	} catch (e) {
		warn(`Error loading wireless module: ${e}\n${e.stacktrace[0].context}\n`);
	}
} else {
	warn(`Wireless module not found\n`);
}
