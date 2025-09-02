import * as uci from "uci";
import * as uloop from "uloop";
import * as libubus from "ubus";
import { access, dirname } from "fs";

function ex_handler(e)
{
	netifd.log(netifd.L_WARNING, `Exception: ${e}\n${e.stacktrace[0].context}\n`);
}

uloop.guard(ex_handler);
libubus.guard(ex_handler);

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

function ex_wrap(cb)
{
	let fn = cb;
	return (...args) => {
		try {
			return fn(...args);
		} catch (e) {
			netifd.log(netifd.L_WARNING, `${e}\n${e.stacktrace[0].context}`);
		}
	};
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
		netifd.log(netifd.L_WARNING, `Error loading wireless module: ${e}\n${e.stacktrace[0].context}\n`);
	}
} else {
	netifd.log(netifd.L_WARNING, `Wireless module not found\n`);
}
