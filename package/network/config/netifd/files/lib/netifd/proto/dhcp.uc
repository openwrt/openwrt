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

function has(v) {
	return v != null && v !== '';
}

function shell_str(v) {
	if (type(v) == 'bool')
		return v ? '1' : '0';
	if (type(v) == 'double')
		return sprintf('%f', v);
	return (type(v) == 'int') ? '' + v : v;
}

function validate_hex(s) {
	return hexenc(hexdec(s, ' \t\r\n:-'));
}

function dhcp_optcode(opt) {
	let code = match(opt, /^[ \t=:]*([^ \t=:]+)[ \t=:]/s)?.[1];
	if (code == 'vendor')
		return 60;
	if (match(code, /^(0[xX][0-9a-fA-F]+|0[0-7]*|[1-9][0-9]*)$/s))
		return int(code, 0);
	return null;
}

function resolve_sigusr1() {
	let p = popen('kill -l SIGUSR1', 'r');
	return p ? +p.read('all') : null;
}

function default_clientid(dev) {
	let duid = uci.cursor()?.get('network', '@globals[0]', 'dhcp_default_duid');
	if (duid) {
		duid = validate_hex(duid);
		if (!duid)
			warn(`dhcp: ${dev}: ignoring invalid dhcp_default_duid value\n`);
	}
	if (!duid)
		return null;
	let iaid = substr(md5(dev), 0, 8);
	return 'ff' + iaid + duid;
}

const config_opts = {
	ipaddr: 'string',
	hostname: 'string',
	clientid: 'string',
	sendclientid: 'string',
	vendorid: 'string',
	broadcast: 'bool',
	norelease: 'bool',
	reqopts: 'string',
	defaultreqopts: 'bool',
	iface6rd: 'string',
	sendopts: 'array',
	delegate: 'bool',
	zone6rd: 'string',
	zone: 'string',
	mtu6rd: 'string',
	customroutes: 'string',
	classlessroute: 'bool',
	timeout: 'int',
	retry: 'int',
	tryagain: 'int',
};

function config_val(kind, val) {
	if (kind == 'array') {
		if (type(val) == 'string')
			return filter(split(val, /[ \t]/), v => v != '');
		return (type(val) == 'array') ? val : null;
	}
	if (type(val) == 'array')
		return null;
	if (kind == 'bool')
		return bool(val);
	if (kind == 'int') {
		if (type(val) != 'string' ||
		    !match(val, /^[[:space:]]*[+-]?(0[xX][0-9a-fA-F]+|0[0-7]*|[1-9][0-9]*)$/s))
			return null;
		let n = int(val, 0);
		return (n >= -2147483648 && n <= 2147483647) ? '' + n : null;
	}
	return val;
}

if (access('/sbin/udhcpc', 'x'))
netifd.add_proto({
	name: "dhcp",
	"renew-handler": true,

	config: function(ctx) {
		let cfg = {};
		for (let opt, kind in config_opts)
			cfg[opt] = config_val(kind, ctx.data[opt]);
		return cfg;
	},

	setup: function(proto) {
		let cfg = {};
		for (let opt, val in proto.config)
			cfg[opt] = shell_str(val);
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

		if (has(cfg.timeout))
			push(argv, '-T', cfg.timeout);
		if (has(cfg.tryagain))
			push(argv, '-A', cfg.tryagain);

		let ipaddr = trim(split('' + (cfg.ipaddr ?? ''), '/')[0]);
		if (ipaddr)
			push(argv, '-r', ipaddr);

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

			if (dhcp_optcode(opt) == 60)
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
			clientid = default_clientid(dev);
		} else if (cfg.sendclientid == 'hardware') {
			clientid = null;
		} else if (cfg.sendclientid == 'none') {
			push(argv, '-C');
		} else {
			clientid = cfg.clientid;
			if (clientid) {
				clientid = validate_hex(clientid);
				if (!clientid)
					warn(`dhcp: ${dev}: ignoring invalid clientid value\n`);
			}
			if (!clientid)
				clientid = default_clientid(dev);
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
