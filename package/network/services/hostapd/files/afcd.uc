#!/usr/bin/env ucode
'use strict';
import { basename } from "fs";
let uclient = require("uclient");
let uloop = require("uloop");
let libubus = require("ubus");
let opts = {};
let reqs = [];

const usage_message = `Usage: ${basename(sourcepath())} <options>
Options:
	-u <url>:		AFC server URL (required)
	-c <path>:		AFC server CA certificate

`;

function usage() {
    warn(usage_message);
    exit(1);
}

while (substr(ARGV[0], 0, 1) == "-") {
	let opt = substr(shift(ARGV), 1);
	switch (opt) {
	case 'u':
		opts.url = shift(ARGV);
		break;
	case 'c':
		opts.cert = shift(ARGV);
		if (!opts.cert)
			usage();
		break;
	default:
		usage();
	}
}

if (!opts.url)
	usage();

function request_done(cb, error)
{
	if (!cb.req)
		return;

	if (error)
		delete cb.data;

	cb.req.reply({ data: cb.data }, error);

	delete cb.req;
	delete cb.client;
}

const cb_proto = {
	data_read: function(cb) {
		let cur;
		while (length(cur = this.read()) > 0)
			cb.data += cur;
	},
	data_eof: function(cb) {
		request_done(cb, 0);
	},
	error: function(cb, code) {
		request_done(cb, libubus.STATUS_UNKNOWN_ERROR);
	},
};

function handle_request(req)
{
	let cb = proto({ data: "" }, cb_proto);

	let cl = uclient.new(opts.url, null, cb);

	if (!cl.ssl_init({ verify: true, ca_files: [ opts.cert ] })) {
		warn(`Failed to initialize SSL\n`);
		return false;
	}

	if (!cl.connect()) {
		warn(`Failed to connect\n`);
		return false;
	}

	let meta = {
		headers: {
			"Content-Type": "application/json",
		},
		post_data: req.args.data
	};

	if (!cl.request("POST", meta)) {
		warn(`Failed to send request\n`);
		return false;
	}

	cb.client = cl;
	cb.req = req;

	return true;
}

function add_ubus(ubus) {
	return ubus.publish("afc", {
		request: {
			call: function(req) {
				if (!req.args.data)
					return libubus.STATUS_INVALID_ARGUMENT;

				let ret = handle_request(req);
				if (!ret)
					return libubus.STATUS_UNKNOWN_ERROR;

				req.defer();
			},
			args: {
				data: "",
			},
		},
	});
}

uloop.init();

let ubus = libubus.connect();
if (!add_ubus(ubus)) {
	warn("Failed to publish ubus object\n");
	exit(1);
}

uloop.run();
uloop.done();
