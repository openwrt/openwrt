import { readfile, access, popen } from 'fs';
import { md5 } from 'digest';
import * as uci from 'uci';

function bool(v) {
	if (v == null) return null;
	let s = '' + v;
	if (s == '1' || s == 'true')
		return '1';
	if (s == '0' || s == 'false')
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

function default_clientid(iface, dev) {
	let duid = uci.cursor()?.get('network', '@globals[0]', 'dhcp_default_duid');
	if (duid) {
		duid = validate_hex(duid);
		if (!duid)
			warn(`dhcp: ${iface}: ignoring invalid dhcp_default_duid value\n`);
	}
	if (!duid)
		return null;
	let iaid = substr(md5(dev), 0, 8);
	return 'ff' + iaid + duid;
}

const config_opts = [
	'ipaddr', 'hostname', 'clientid', 'sendclientid', 'vendorid',
	'broadcast', 'norelease', 'reqopts', 'defaultreqopts', 'iface6rd',
	'sendopts', 'delegate', 'zone6rd', 'zone', 'mtu6rd', 'customroutes',
	'classlessroute', 'timeout', 'retry', 'tryagain',
];
const array_opts = [ 'sendopts' ];
const int_opts = [ 'timeout', 'retry', 'tryagain' ];

function config_val(opt, val) {
	if (type(val) == 'array')
		return (opt in array_opts) ? val : null;
	if (opt in int_opts) {
		if (type(val) != 'string' ||
		    !match(val, /^[[:space:]]*[+-]?(0[xX][0-9a-fA-F]+|0[0-7]*|[1-9][0-9]*)$/))
			return null;
		return '' + int(val, 0);
	}
	return val;
}

if (access('/sbin/udhcpc', 'x'))
netifd.add_proto({
	name: "dhcp",
	"renew-handler": true,

	config: function(ctx) {
		let cfg = {};
		for (let opt in config_opts)
			cfg[opt] = config_val(opt, ctx.data[opt]);
		return cfg;
	},

	setup: function(proto) {
		let cfg = proto.config;
		let iface = proto.iface;
		let dev = proto.device;

		let retry = cfg.retry || '0';

		let argv = [
			'/sbin/udhcpc',
			'-p', `/var/run/udhcpc-${dev}.pid`,
			'-s', '/lib/netifd/dhcp.script',
			'-f', '-t', retry,
			'-i', dev,
		];

		if (cfg.timeout)
			push(argv, '-T', cfg.timeout);
		if (cfg.tryagain)
			push(argv, '-A', cfg.tryagain);

		if (cfg.ipaddr)
			push(argv, '-r', split(cfg.ipaddr, '/')[0]);

		let hostname = cfg.hostname;
		if (!hostname)
			hostname = trim(readfile('/proc/sys/kernel/hostname') ?? '');
		if (hostname == '*')
			hostname = null;
		if (hostname)
			push(argv, '-x', `hostname:${hostname}`);

		let dhcpopts = [];
		let has_opt60 = false;

		let reqopts = cfg.reqopts;
		if (type(reqopts) == 'string')
			reqopts = split(reqopts, /\s+/);
		for (let opt in reqopts ?? [])
			if (opt) push(dhcpopts, '-O', opt);

		let sendopts = cfg.sendopts;
		if (type(sendopts) == 'string')
			sendopts = split(sendopts, /[ \t]+/);
		for (let opt in sendopts ?? []) {
			if (!opt) continue;
			push(dhcpopts, '-x', opt);

			if (match(opt, /^(0[xX]3[cC]|60|vendor):/))
				has_opt60 = true;
		}

		if (cfg.vendorid && !has_opt60) {
			push(dhcpopts, '-x', `0x3c:${hexenc(cfg.vendorid)}`);
			has_opt60 = true;
		}

		if (cfg.iface6rd != '0' &&
		    (access('/lib/netifd/proto/6rd.sh', 'f') || access('/lib/netifd/proto/6rd.uc', 'f')))
			push(dhcpopts, '-O', '212');

		if (bool(cfg.classlessroute) != '0')
			push(dhcpopts, '-O', '121');

		if (has_opt60)
			push(argv, '-V', '');

		let clientid;
		if (cfg.sendclientid == 'global') {
			clientid = default_clientid(iface, dev);
		} else if (cfg.sendclientid == 'hardware') {
			clientid = null;
		} else if (cfg.sendclientid == 'none') {
			push(argv, '-C');
		} else {
			clientid = cfg.clientid;
			if (clientid) {
				clientid = validate_hex(clientid);
				if (!clientid)
					warn(`dhcp: ${iface}: ignoring invalid clientid value\n`);
			}
			if (!clientid)
				clientid = default_clientid(iface, dev);
		}
		if (clientid)
			push(argv, '-x', `0x3d:${clientid}`);

		if (bool(cfg.defaultreqopts) == '0')
			push(argv, '-o');

		if (bool(cfg.broadcast) == '1')
			push(argv, '-B');

		if (bool(cfg.norelease) != '1')
			push(argv, '-R');

		for (let opt in dhcpopts)
			push(argv, opt);

		let env = [`INTERFACE=${iface}`];
		if (cfg.iface6rd)
			push(env, `IFACE6RD=${cfg.iface6rd}`);
		if (cfg.zone6rd)
			push(env, `ZONE6RD=${cfg.zone6rd}`);
		if (cfg.zone)
			push(env, `ZONE=${cfg.zone}`);
		if (cfg.mtu6rd)
			push(env, `MTU6RD=${cfg.mtu6rd}`);
		if (cfg.customroutes)
			push(env, `CUSTOMROUTES=${cfg.customroutes}`);
		if (bool(cfg.delegate) == '0')
			push(env, 'IFACE6RD_DELEGATE=0');

		proto.run_command(map(argv, v => '' + (v ?? '')), env);
	},

	renew: function(proto) {
		let sig = resolve_sigusr1();
		if (sig > 0)
			proto.kill_command(sig);
	},

	restart: function(proto) {
		proto.kill_command(1);
	},

	teardown: function(proto) {
		proto.kill_command();
	},
});
