// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

import { time_format } from "cli.utils";

function get_interfaces()
{
	let data = model.ubus.call("network.interface", "dump");
	if (!data)
		return {};

	let ret = {};
	for (let iface in data.interface)
		ret[iface.interface] = iface;

	return ret;
}

function interface_validate(ctx, argv)
{
	let name = argv[0];
	if (!name)
		return ctx.missing_argument("Missing argument: %s", "name");

	if (index(get_interfaces(), name) < 0)
		return ctx.not_found("Interface not found: %s", name);

	return true;
}

const interface_args = [
	{
		name: "interface",
		help: "Interface name",
		type: "enum",
		value: (ctx) => keys(get_interfaces())
	}
];

function interface_status(data)
{
	if (data.up)
		return "up";
	if (!data.autostart)
		return "down";
	if (!data.available)
		return "unavailable";
	return "pending";
}

const Network = {
	list: {
		help: "List interfaces",
		call: function(ctx, argv) {
			return ctx.list("Interfaces", keys(get_interfaces()));
		}
	},
	reload: {
		help: "Reload network config",
		call: function(ctx, argv) {
			model.ubus.call("network", "reload");
			return ctx.ok("Configuration reloaded");
		}
	},
	restart: {
		help: "Restart interface",
		validate: interface_validate,
		args: interface_args,
		call: function(ctx, argv) {
			let name = shift(argv);
			model.ubus.call("network.interface."+name, "down");
			model.ubus.call("network.interface."+name, "up");
			return ctx.ok("Interface restarted");
		}
	},
	start: {
		help: "Start interface",
		validate: interface_validate,
		args: interface_args,
		call: function(ctx, argv) {
			let name = shift(argv);
			model.ubus.call("network.interface."+name, "up");
			return ctx.ok("Interface started");
		}
	},
	stop: {
		help: "Stop interface",
		validate: interface_validate,
		args: interface_args,
		call: function(ctx, argv) {
			let name = shift(argv);
			model.ubus.call("network.interface."+name, "down");
			return ctx.ok("Interface stopped");
		}
	},
	status: {
		help: "Interface status",
		args: interface_args,
		call: function(ctx, argv) {
			let name = shift(argv);
			let status = get_interfaces();
			if (!name) {
				let data = {};
				for (let iface, ifdata in status)
					data[iface] = interface_status(ifdata);

				return ctx.table("Status", data);
			}

			let ifdata = status[name];
			let data = {
				Status: interface_status(ifdata),
			};
			if (ifdata.up)
				data.Uptime = time_format(ifdata.uptime);

			if (length(ifdata["ipv4-address"]) > 0)
				data.IPv4 = join(", ", map(ifdata["ipv4-address"], (v) => v.address + "/" + v.mask));
			if (length(ifdata["ipv6-address"]) > 0)
				data.IPv6 = join(", ", map(ifdata["ipv6-address"], (v) => v.address + "/" + v.mask));
			if (length(ifdata["dns-server"]) > 0)
				data.DNS = join(", ", ifdata["dns-server"]);
			if (length(ifdata["route"]) > 0)
				data.Routes = join(", ", map(ifdata["route"], (v) => (v.mask == 0 ? "Default" : `${v.target}/${v.mask}`) + ": " + v.nexthop));
			return ctx.table("Status", data);
		}
	}
};

const Root = {
	network: {
		help: "Network interface configuration",
		select_node: "Network",
	}
};

model.add_nodes({ Root, Network });
