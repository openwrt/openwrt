// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

function mdns_data(model)
{
	return model.ubus.call("umdns", "browse", { array: true, address: false });
}

function refresh_timer(model)
{
	model.ubus.call("umdns", "update");

	model.mdns.refresh_count++;
	if (model.mdns.refresh_count < 3)
		model.mdns.timer.set(500);

	if (model.mdns.refresh_count < 2)
		return;

	let data = mdns_data(model);
	for (let service_name, service_data in data) {
		for (let host_name, host_data in service_data) {
			let interface = host_data.iface;
			if (!interface)
				continue;
			if (host_data.host)
				continue;

			let question = host_name + "." + service_name + ".local";
			model.ubus.call("umdns", "query", { question, interface });
		}
	}
}

function refresh_start(model)
{
	model.mdns.refresh_count = 0;
	model.mdns.timer ??= model.uloop.timer(500, () => refresh_timer(model));
}

function strip_local(name)
{
	if (substr(name, -6) == ".local")
		name = substr(name, 0, -6);

	return name;
}

function get_hosts(model)
{
	let data = model.ubus.call("umdns", "hosts", { array: true });
	let ret = {};

	for (let name, val in data) {
		name = strip_local(name);
		ret[lc(name)] = val;
	}

	return ret;
}

function get_host_names(model)
{
	let data = model.ubus.call("umdns", "hosts", { array: true });
	let ret = {};

	for (let name, val in data) {
		name = strip_local(name);
		ret[lc(name)] = name;
	}

	return ret;
}

function get_host_servicenames(model)
{
	let hosts = {};
	let data = mdns_data(model);
	for (let service_name, service_data in data) {
		for (let host_name, host_data in service_data) {
			let name = strip_local(host_data.host);

			hosts[lc(name)] ??= [];
			if (index(hosts[lc(name)], host_name) < 0)
				push(hosts[lc(name)], host_name);
		}
	}
	return hosts;
}

function get_service_hosts(model, name)
{
	name = lc(name);

	let data = mdns_data(model);
	for (let cur_name, hosts in data)
		if (lc(cur_name) == name)
			return hosts;
}

function get_host_services(model)
{
	let hosts = {};
	let data = mdns_data(model);
	for (let service_name, service_data in data) {
		for (let host_name, host_data in service_data) {
			host_data.name = host_name;
			host_name = lc(strip_local(host_data.host));
			hosts[host_name] ??= {};
			hosts[host_name][service_name] = host_data;
		}
	}
	return hosts;
}

function host_info(host)
{
	let ret = {};
	if (host.ipv4)
		ret.IPv4 = host.ipv4;
	if (host.ipv6)
		ret.IPv6 = host.ipv6;
	return ret;
}

const host_arg = {
	name: "host",
	help: "host name",
	type: "enum",
	ignore_case: true,
	value: () => values(get_host_names(model)),
};

const service_arg = {
	name: "service",
	help: "service name",
	type: "enum",
	ignore_case: true,
	value: () => keys(mdns_data(model)),
};

function add_field(ret, name, val)
{
	if (val == null)
		return;
	if (type(ret) == "array")
		push(ret, [ name, val ]);
	else
		ret[name] = val;
}

function service_info(data)
{
	let info = [];

	add_field(info, "Name", data.name);
	add_field(info, "Interface", data.iface);
	add_field(info, "Port", data.port);
	add_field(info, "Text", data.txt);

	return info;
}

const MDNS = {
	refresh: {
		help: "Refresh service list by sending queries",
		call: function(ctx, argv, named) {
			refresh_start(model);
			return ctx.ok("Querying hosts");
		}
	},
	service: {
		help: "Show service info",
		args: [ service_arg ],
		call: function (ctx, argv, named) {
			let name = argv[0];
			if (name != null) {
				let data = get_service_hosts(model, name);
				if (!data)
					return ctx.not_found("Service not found: %s", name);

				let ret = {};
				for (let name, host in data) {
					if (!host.host)
						continue;
					let host_name = strip_local(host.host);
					host.name = name;
					ret["Host " + host_name] = service_info(host);
				}

				return ctx.multi_table("Service " + name, ret);
			}

			let data = mdns_data(model);
			let services = {};
			for (let service_name, service_data in data) {
				let hosts = [];
				for (let name, host in service_data)
					if (host.host)
						push(hosts, `${name}(${strip_local(host.host)})`);
				if (length(hosts))
					services[service_name] = sort(hosts);
			}

			return ctx.table("Services", services);
		}
	},
	host: {
		help: "Host information",
		args: [ host_arg ],
		call: function (ctx, argv, named) {
			let hosts_svc = get_host_services(model);
			let hosts = get_hosts(model);
			let host = argv[0];

			if (host == null) {
				let host_names = get_host_names(model);
				let ret = {};

				for (let lc_name, name in host_names) {
					let data = hosts[lc_name];
					if (!data)
						continue;

					let title = "Host " + name;
					ret[title] = host_info(data);
					let svc = hosts_svc[lc_name];
					if (svc)
						ret[title].services = keys(svc);
				}
				return ctx.multi_table("Hosts", ret);
			}

			let lc_host = lc(host);
			let data = hosts[lc_host];
			if (!data)
				return ctx.not_found("Host not found: " + host);

			let ret = {};
			ret.Info = host_info(data);

			for (let service_name, sdata in hosts_svc[lc_host])
				ret["Service " + service_name] = service_info(sdata);

			return ctx.multi_table("Host " + host, ret);
		}
	},
};

const Root = {
	mdns: {
		help: "Browse mdns hosts",
		select_node: "MDNS",
		select: function(ctx, argv) {
			try {
				refresh_start(model);
			} catch (e) {
				ctx.model.exception(e);
			}
			return true;
		},
	}
};

model.add_nodes({ Root, MDNS });
model.mdns = {};
