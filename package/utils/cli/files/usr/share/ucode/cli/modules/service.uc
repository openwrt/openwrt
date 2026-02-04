// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

import { glob, access, basename } from "fs";

function get_services()
{
	return model.cache.get("init_service_list", () => {
		let services = glob("/etc/init.d/*");
		services = filter(services, (v) => !system([ "grep", "-q", "start_service()", v ]));
		services = map(services, basename);
		return sort(services);
	});
}

function get_service_status( name)
{
	return model.ubus.call("service", "list", (name ? { name } : null));
}

function service_running(name)
{
	let status = get_service_status(name);
	return !!(status && status[name]);
}

function __service_cmd(name, cmd)
{
	return system([ "/etc/init.d/" + name, cmd ]) == 0;
}

function service_cmd(ctx, name, cmd, msg)
{
	if (__service_cmd(name, cmd))
		return ctx.ok(msg);
	else
		return ctx.command_failed("Command failed");
}

function service_validate(ctx, argv)
{
	let name = argv[0];
	if (!name)
		return ctx.missing_argument("Missing argument: %s", "name");

	if (index(get_services(), name) < 0)
		return ctx.not_found("Service not found: %s", name);

	return true;
}

const service_args = [
	{
		name: "name",
		help: "Service name",
		type: "enum",
		value: (ctx) => get_services()
	}
];

const service_settings = {
	enabled: {
		help: "Service enabled at system boot",
	},
	disabled: {
		help: "Service disabled at system boot",
	}
};

const SystemService = {
	list: {
		help: "List services",
		call: function(ctx, argv) {
			return ctx.list("Services", get_services());
		}
	},
	reload: {
		help: "Reload service",
		validate: service_validate,
		args: service_args,
		call: function(ctx, argv) {
			return service_cmd(ctx, shift(argv), "reload", "Service reloaded");
		}
	},
	restart: {
		help: "Restart service",
		validate: service_validate,
		args: service_args,
		call: function(ctx, argv) {
			return service_cmd(ctx, shift(argv), "restart", "Service restarted");
		}
	},
	set: {
		help: "Change service settings",
		validate: service_validate,
		args: service_args,
		named_args: service_settings,
		call: function(ctx, argv, param) {
			if (!length(param))
				return ctx.invalid_argument("No settings provided");

			if (param.enabled && param.disabled)
				return ctx.invalid_argument("enabled and disabled cannot be set at the same time");

			if (param.enabled && !__service_cmd(name, "enable"))
				ctx.command_failed("Command failed: %s", "enable");

			if (param.disabled && !__service_cmd(name, "disable"))
				ctx.command_failed("Command failed: %s", "disable");

			return ctx.ok("Settings changed");
		}
	},
	start: {
		help: "Start service",
		validate: service_validate,
		args: service_args,
		call: function(ctx, argv) {
			let name = shift(argv);

			if (service_running(name))
				return ctx.invalid_argument("Service already running", name);

			return service_cmd(ctx, name, "start", "Service started");
		}
	},
	stop: {
		help: "Stop service",
		validate: service_validate,
		args: service_args,
		call: function(ctx, argv) {
			let name = shift(argv);

			if (!service_running(name))
				return ctx.invalid_argument("Service not running", name);

			return service_cmd(ctx, name, "stop", "Service stopped");
		}
	},
	status: {
		help: "Service status",
		args: service_args,
		call: function(ctx, argv) {
			let name = shift(argv);
			if (!name) {
				let data = {};
				for (let service in get_services()) {
					let running = service_running(service);
					data[service] = running ? "running" : "not running";
				}
				return ctx.table("Status", data);
			}

			if (index(get_services(), name) < 0)
				return ctx.not_found("Service not found: %s", name);

			let data = {
				"Running": service_running(name),
				"Enabled": __service_cmd(name, "enabled"),
			};
			return ctx.table("Status", data);
		}
	}
};

const Root = {
	service: {
		help: "System service configuration",
		select_node: "SystemService",
	}
};

model.add_nodes({ Root, SystemService });
