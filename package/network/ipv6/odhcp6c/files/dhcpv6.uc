import { access, popen } from 'fs';
import { md5 } from 'digest';
import * as libubus from 'ubus';
import * as uci from 'uci';

function bool(v) {
	if (v == null) return null;
	let s = lc(trim('' + v));
	if (s == '1' || s == 'on' || s == 'true' || s == 'yes' || s == 'enabled')
		return '1';
	if (s == '0' || s == 'off' || s == 'false' || s == 'no' || s == 'disabled')
		return '0';
	return null;
}

function validate_hex(s) {
	return hexenc(hexdec(s, ' \t\r\n:-'));
}

function resolve_sigusr1() {
	let p = popen('kill -l SIGUSR1', 'r');
	return p ? +p.read('all') : null;
}

function default_duid(cfg, iface) {
	let duid = cfg._duid;
	if (!duid)
		duid = uci.cursor()?.get('network', '@globals[0]', 'dhcp_default_duid');
	if (duid) {
		duid = validate_hex(duid);
		if (!duid)
			warn(`dhcpv6: ${iface}: ignoring invalid dhcp_default_duid value\n`);
	}
	return duid;
}

const config_opts = [
	'reqaddress', 'reqprefix', 'clientid', 'sendclientid', 'reqopts',
	'defaultreqopts', 'noslaaconly', 'forceprefix', 'extendprefix',
	'norelease', 'strict_rfc7550', 'noserverunicast', 'noclientfqdn',
	'noacceptreconfig', 'ip6prefix', 'iface_dslite', 'zone_dslite',
	'encaplimit_dslite', 'mtu_dslite', 'iface_map', 'zone_map',
	'encaplimit_map', 'iface_464xlat', 'zone_464xlat', 'zone',
	'userclass', 'vendorclass', 'sendopts', 'delegate', 'skpriority',
	'soltimeout', 'fakeroutes', 'sourcefilter', 'keep_ra_dnslifetime',
	'ra_holdoff', 'verbose', 'dynamic',
];

if (access('/usr/sbin/odhcp6c', 'x'))
netifd.add_proto({
	name: "dhcpv6",
	"renew-handler": true,

	config: function(ctx) {
		let d = ctx.data;
		let cfg = {};
		for (let opt in config_opts)
			cfg[opt] = d[opt];
		cfg.ip6ifaceid = d.ip6ifaceid || d.ifaceid;
		cfg._duid = ctx.uci.get('network', '@globals[0]', 'dhcp_default_duid');
		return cfg;
	},

	setup: function(proto) {
		let cfg = proto.config;
		let iface = proto.iface;
		let dev = proto.device;

		let argv = [
			'/usr/sbin/odhcp6c',
			'-s', '/lib/netifd/dhcpv6.script',
		];

		if (cfg.reqaddress)
			push(argv, `-N${cfg.reqaddress}`);

		let reqprefix = cfg.reqprefix;
		if (!reqprefix || reqprefix == 'auto')
			reqprefix = '0';
		if (reqprefix != 'no') {
			let m = match(reqprefix, /:([0-9a-fA-F]{1,8})$/);
			let iaid, hint;
			if (m) {
				iaid = m[1];
				hint = substr(reqprefix, 0, length(reqprefix) - length(iaid) - 1);
			} else {
				iaid = substr(md5(dev), 0, 8);
				hint = reqprefix;
				reqprefix = `${reqprefix}:${iaid}`;
			}

			let hnum = (substr(hint, 0, 1) == '/') ? substr(hint, 1) : hint;
			if (match(hnum, /^[0-9]+$/) && int(hnum) <= 128) {
				if (substr(reqprefix, 0, 1) == '/')
					reqprefix = substr(reqprefix, 1);
			} else {
				let parts = split(hint, '/');
				let ok = (length(iptoarr(parts[0])) == 16);
				if (ok && length(parts) > 1)
					ok = match(parts[1], /^[0-9]+$/) && int(parts[1]) <= 128;
				if (!ok) {
					reqprefix = `0:${iaid}`;
					warn(`dhcpv6: ${iface}: ignoring invalid prefix hint\n`);
				}
			}

			push(argv, `-P${reqprefix}`);
		}

		let clientid;
		if (cfg.sendclientid == 'global') {
			clientid = default_duid(cfg, iface);
		} else if (cfg.sendclientid == 'hardware') {
			clientid = null;
		} else {
			clientid = cfg.clientid;
			if (clientid) {
				clientid = validate_hex(clientid);
				if (!clientid)
					warn(`dhcpv6: ${iface}: ignoring invalid clientid value\n`);
			}
			if (!clientid)
				clientid = default_duid(cfg, iface);
		}
		if (clientid)
			push(argv, `-c${clientid}`);

		if (bool(cfg.defaultreqopts) == '0')
			push(argv, '-R');

		if (bool(cfg.noslaaconly) == '1')        push(argv, '-S');
		if (bool(cfg.forceprefix) == '1')        push(argv, '-F');
		if (bool(cfg.norelease) == '1')          push(argv, '-k');
		if (bool(cfg.strict_rfc7550) == '1')     push(argv, '--strict-rfc7550');
		if (bool(cfg.noserverunicast) == '1')    push(argv, '-U');
		if (bool(cfg.noclientfqdn) == '1')       push(argv, '-f');
		if (bool(cfg.noacceptreconfig) == '1')   push(argv, '-a');
		if (bool(cfg.keep_ra_dnslifetime) == '1') push(argv, '-L');

		if (cfg.ip6ifaceid)
			push(argv, `-i${cfg.ip6ifaceid}`);

		if (cfg.vendorclass)
			push(argv, `-V${cfg.vendorclass}`);
		if (cfg.userclass)
			push(argv, `-u${cfg.userclass}`);

		if (cfg.skpriority)
			push(argv, `-K${cfg.skpriority}`);

		if (cfg.ra_holdoff)
			push(argv, `-m${cfg.ra_holdoff}`);

		if (cfg.verbose)
			push(argv, `-l${cfg.verbose}`);

		let sendopts = cfg.sendopts;
		if (type(sendopts) == 'string')
			sendopts = split(sendopts, /[ \t]+/);
		for (let opt in sendopts ?? [])
			if (opt) push(argv, `-x${opt}`);

		let reqopts = [];

		let user_reqopts = cfg.reqopts;
		if (type(user_reqopts) == 'string')
			user_reqopts = split(user_reqopts, /\s+/);
		for (let opt in user_reqopts ?? [])
			if (opt) push(reqopts, opt);

		let ubus = libubus.connect();
		if (ubus) {
			let handlers = ubus.call('network', 'get_proto_handlers');
			if (handlers?.dslite)
				push(reqopts, '64');
			if (handlers?.map)
				push(reqopts, '94', '95', '96');
			ubus.disconnect();
		}

		for (let opt in reqopts)
			push(argv, `-r${opt}`);

		push(argv, `-t${cfg.soltimeout || '120'}`);

		push(argv, dev);

		let env = [`INTERFACE=${iface}`];

		let ip6prefix = cfg.ip6prefix;
		if (type(ip6prefix) == 'string')
			ip6prefix = split(ip6prefix, /[ \t]+/);
		let ip6prefixes = [];
		for (let prefix in ip6prefix ?? [])
			if (prefix)
				push(ip6prefixes, prefix);
		if (length(ip6prefixes))
			push(env, `USERPREFIX=${join(' ', ip6prefixes)}`);

		if (cfg.iface_dslite)
			push(env, `IFACE_DSLITE=${cfg.iface_dslite}`);
		if (cfg.mtu_dslite)
			push(env, `MTU_DSLITE=${cfg.mtu_dslite}`);
		if (cfg.zone_dslite)
			push(env, `ZONE_DSLITE=${cfg.zone_dslite}`);
		if (cfg.encaplimit_dslite)
			push(env, `ENCAPLIMIT_DSLITE=${cfg.encaplimit_dslite}`);

		if (cfg.iface_map)
			push(env, `IFACE_MAP=${cfg.iface_map}`);
		if (cfg.zone_map)
			push(env, `ZONE_MAP=${cfg.zone_map}`);
		if (cfg.encaplimit_map)
			push(env, `ENCAPLIMIT_MAP=${cfg.encaplimit_map}`);

		if (cfg.iface_464xlat)
			push(env, `IFACE_464XLAT=${cfg.iface_464xlat}`);
		if (cfg.zone_464xlat)
			push(env, `ZONE_464XLAT=${cfg.zone_464xlat}`);

		if (bool(cfg.delegate) == '0') {
			push(env, 'IFACE_DSLITE_DELEGATE=0');
			push(env, 'IFACE_MAP_DELEGATE=0');
			push(env, 'IFACE_464XLAT_DELEGATE=0');
		}

		if (cfg.zone)
			push(env, `ZONE=${cfg.zone}`);

		if (bool(cfg.fakeroutes) != '0')
			push(env, 'FAKE_ROUTES=1');
		if (bool(cfg.sourcefilter) == '0')
			push(env, 'NOSOURCEFILTER=1');
		if (bool(cfg.extendprefix) == '1')
			push(env, 'EXTENDPREFIX=1');

		if (bool(cfg.dynamic) == '0')
			push(env, 'DYNAMIC=0');
		else
			push(env, 'DYNAMIC=1');

		proto.run_command(argv, env);
	},

	renew: function(proto) {
		let sig = resolve_sigusr1();
		if (sig > 0)
			proto.kill_command(sig);
	},

	teardown: function(proto) {
		proto.kill_command();
	},
});
