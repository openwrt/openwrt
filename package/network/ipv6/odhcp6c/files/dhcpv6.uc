import { readfile, access } from 'fs';
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

if (access('/usr/sbin/odhcp6c', 'x'))
netifd.add_proto({
	name: "dhcpv6",
	"renew-handler": true,

	config: function(ctx) {
		let d = ctx.data;
		return {
			reqaddress: d.reqaddress,
			reqprefix: d.reqprefix,
			clientid: d.clientid,
			reqopts: d.reqopts,
			defaultreqopts: d.defaultreqopts,
			noslaaconly: d.noslaaconly,
			forceprefix: d.forceprefix,
			extendprefix: d.extendprefix,
			norelease: d.norelease,
			strict_rfc7550: d.strict_rfc7550,
			noserverunicast: d.noserverunicast,
			noclientfqdn: d.noclientfqdn,
			noacceptreconfig: d.noacceptreconfig,
			ip6prefix: d.ip6prefix,
			iface_dslite: d.iface_dslite,
			zone_dslite: d.zone_dslite,
			encaplimit_dslite: d.encaplimit_dslite,
			mtu_dslite: d.mtu_dslite,
			iface_map: d.iface_map,
			zone_map: d.zone_map,
			encaplimit_map: d.encaplimit_map,
			iface_464xlat: d.iface_464xlat,
			zone_464xlat: d.zone_464xlat,
			zone: d.zone,
			ip6ifaceid: d.ip6ifaceid || d.ifaceid,
			userclass: d.userclass,
			vendorclass: d.vendorclass,
			sendopts: d.sendopts,
			delegate: d.delegate,
			skpriority: d.skpriority,
			soltimeout: d.soltimeout,
			fakeroutes: d.fakeroutes,
			sourcefilter: d.sourcefilter,
			keep_ra_dnslifetime: d.keep_ra_dnslifetime,
			ra_holdoff: d.ra_holdoff,
			verbose: d.verbose,
			dynamic: d.dynamic,
			_duid: ctx.uci.get('network', '@globals[0]', 'dhcp_default_duid'),
		};
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
		if (reqprefix != 'no')
			push(argv, `-P${reqprefix}`);

		let clientid = cfg.clientid;
		if (clientid == '*') {
			clientid = null;
		} else if (clientid) {
			clientid = validate_hex(clientid);
			if (!clientid)
				warn(`dhcpv6: ${iface}: ignoring invalid clientid value\n`);
		}
		if (!clientid) {
			let duid = cfg._duid;
			if (!duid) {
				let cursor = uci.cursor();
				duid = cursor.get('network', '@globals[0]', 'dhcp_default_duid');
			}
			if (duid) {
				duid = validate_hex(duid);
				if (!duid)
					warn(`dhcpv6: ${iface}: ignoring invalid dhcp_default_duid value\n`);
			}
			clientid = duid;
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
			sendopts = [sendopts];
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
			ip6prefix = split(ip6prefix, /\s+/);
		if (ip6prefix && length(ip6prefix))
			push(env, `USERPREFIX=${join(' ', ip6prefix)}`);

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
		proto.kill_command(resolve_sigusr1());
	},

	teardown: function(proto) {
		proto.kill_command();
	},
});
