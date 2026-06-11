import { readfile, access } from 'fs';
import { md5 } from 'digest';
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
	if (s == null)
		return null;
	s = lc(s);
	if (!match(s, /^\s*[0-9a-f]{2}((\s+|[-:]?)[0-9a-f]{2})*\s*$/))
		return null;
	return replace(s, /[[:space:]:-]/g, '');
}

function resolve_sigusr1() {
	let arch = trim(readfile('/proc/sys/kernel/arch') ?? '');
	return (substr(arch, 0, 4) == 'mips') ? 16 : 10;
}

if (access('/sbin/udhcpc', 'x'))
netifd.add_proto({
	name: "dhcp",
	"renew-handler": true,

	config: function(ctx) {
		let d = ctx.data;
		return {
			ipaddr: d.ipaddr,
			hostname: d.hostname,
			clientid: d.clientid,
			vendorid: d.vendorid,
			broadcast: d.broadcast,
			norelease: d.norelease,
			reqopts: d.reqopts,
			defaultreqopts: d.defaultreqopts,
			iface6rd: d.iface6rd,
			sendopts: d.sendopts,
			delegate: d.delegate,
			zone6rd: d.zone6rd,
			zone: d.zone,
			mtu6rd: d.mtu6rd,
			customroutes: d.customroutes,
			classlessroute: d.classlessroute,
			_duid: ctx.uci.get('network', '@globals[0]', 'dhcp_default_duid'),
		};
	},

	setup: function(proto) {
		let cfg = proto.config;
		let iface = proto.iface;
		let dev = proto.device;

		let argv = [
			'/sbin/udhcpc',
			'-p', `/var/run/udhcpc-${dev}.pid`,
			'-s', '/lib/netifd/dhcp.script',
			'-f', '-t', '0',
			'-i', dev,
		];

		if (cfg.ipaddr)
			push(argv, '-r', split(cfg.ipaddr, '/')[0]);

		let hostname = cfg.hostname;
		if (!hostname)
			hostname = trim(readfile('/proc/sys/kernel/hostname') ?? '');
		if (hostname == '*')
			hostname = null;
		if (hostname)
			push(argv, '-x', `hostname:${hostname}`);

		let reqopts = cfg.reqopts;
		if (type(reqopts) == 'string')
			reqopts = split(reqopts, /\s+/);
		for (let opt in reqopts ?? [])
			if (opt) push(argv, '-O', opt);

		let sendopts = cfg.sendopts;
		if (type(sendopts) == 'string')
			sendopts = [sendopts];

		let dhcpopts = [];
		let has_opt60 = false;

		for (let opt in sendopts ?? []) {
			if (!opt) continue;
			push(dhcpopts, '-x', opt);

			if (match(opt, /^(0[xX]3[cC]|60|vendor):/))
				has_opt60 = true;
		}

		if (bool(cfg.defaultreqopts) == '0')
			push(argv, '-o');

		if (bool(cfg.broadcast) == '1')
			push(argv, '-B');

		if (bool(cfg.norelease) != '1')
			push(argv, '-R');

		let clientid = cfg.clientid;
		if (clientid) {
			clientid = validate_hex(clientid);
			if (!clientid)
				warn(`dhcp: ${iface}: ignoring invalid clientid value\n`);
		}
		if (!clientid) {
			let duid = cfg._duid;
			// dhcp_default_duid is written by another netifd module after the
			// config callback runs, so cfg._duid may still be null even though
			// a fresh cursor sees it.
			if (!duid) {
				let cursor = uci.cursor();
				duid = cursor.get('network', '@globals[0]', 'dhcp_default_duid');
			}
			if (duid) {
				duid = validate_hex(duid);
				if (!duid)
					warn(`dhcp: ${iface}: ignoring invalid dhcp_default_duid value\n`);
			}
			if (duid) {
				let iaid = substr(md5(dev), 0, 8);
				clientid = 'ff' + iaid + duid;
			}
		}
		if (clientid)
			push(argv, '-x', `0x3d:${clientid}`);

		if (cfg.vendorid && !has_opt60) {
			push(argv, '-x', `0x3c:${hexenc(cfg.vendorid)}`);
			has_opt60 = true;
		}
		if (has_opt60)
			push(argv, '-V', '');

		if (cfg.iface6rd != '0' &&
		    (access('/lib/netifd/proto/6rd.sh', 'f') || access('/lib/netifd/proto/6rd.uc', 'f')))
			push(argv, '-O', '212');

		if (bool(cfg.classlessroute) != '0')
			push(argv, '-O', '121');

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

		proto.run_command(argv, env);
	},

	renew: function(proto) {
		proto.kill_command(resolve_sigusr1());
	},

	teardown: function(proto) {
		proto.kill_command();
	},
});
