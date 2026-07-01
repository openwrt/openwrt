import { access, glob as fsglob } from 'fs';
import * as libubus from 'ubus';
import { query } from 'resolv';

const have_pppd = access('/usr/sbin/pppd', 'x');

const ppp_errors = [
	"OK",                 //  0
	"FATAL_ERROR",        //  1
	"OPTION_ERROR",       //  2
	"NOT_ROOT",           //  3
	"NO_KERNEL_SUPPORT",  //  4
	"USER_REQUEST",       //  5
	"LOCK_FAILED",        //  6
	"OPEN_FAILED",        //  7
	"CONNECT_FAILED",     //  8
	"PTYCMD_FAILED",      //  9
	"NEGOTIATION_FAILED", // 10
	"PEER_AUTH_FAILED",   // 11
	"IDLE_TIMEOUT",       // 12
	"CONNECT_TIME",       // 13
	"CALLBACK",           // 14
	"PEER_DEAD",          // 15
	"HANGUP",             // 16
	"LOOPBACK",           // 17
	"INIT_FAILED",        // 18
	"AUTH_TOPEER_FAILED", // 19
	"TRAFFIC_LIMIT",      // 20
	"CNID_AUTH_FAILED",   // 21
];

function ppp_exitcode_tostring(code) {
	return ((code >= 0) ? ppp_errors[code] : null) ?? "UNKNOWN_ERROR";
}

function bool(v) {
	if (v == null) return null;
	let s = lc(trim('' + v));
	if (s == '1' || s == 'on' || s == 'true' || s == 'yes' || s == 'enabled')
		return '1';
	if (s == '0' || s == 'off' || s == 'false' || s == 'no' || s == 'disabled')
		return '0';
	return null;
}

function ppp_select_ipaddr(subnets) {
	let res = null;
	let res_mask = null;

	for (let subnet in subnets) {
		let parts = split(subnet, '/');
		let addr = parts[0];
		let mask = int(parts[1]);

		if (res_mask != null && mask != 32) {
			if (mask > res_mask || res_mask == 32) {
				res = addr;
				res_mask = mask;
			}
		} else if (res_mask == null) {
			res = addr;
			res_mask = mask;
		}
	}

	return res;
}

function ppp_generic_config(d) {
	return {
		username: d.username,
		password: d.password,
		keepalive: d.keepalive,
		keepalive_adaptive: d.keepalive_adaptive,
		demand: d.demand,
		pppd_options: d.pppd_options,
		connect: d.connect,
		disconnect: d.disconnect,
		ipv6: d.ipv6,
		authfail: d.authfail,
		mtu: d.mtu,
		pppname: d.pppname,
		unnumbered: d.unnumbered,
		reqprefix: d.reqprefix,
		persist: d.persist,
		maxfail: d.maxfail,
		holdoff: d.holdoff,
		sourcefilter: d.sourcefilter,
		delegate: d.delegate,
		norelease: d.norelease,
	};
}

function ppp_generic_setup(proto, cfg, extra_args) {
	let iface = proto.iface;

	let ipv6 = null;
	let autoipv6 = null;

	if (access('/proc/sys/net/ipv6') && cfg.ipv6 != '0') {
		if (!cfg.ipv6 || cfg.ipv6 == 'auto') {
			ipv6 = '1';
			autoipv6 = '1';
		} else {
			ipv6 = cfg.ipv6;
		}
	}

	let reqprefix = cfg.reqprefix;
	let norelease = cfg.norelease;
	if (autoipv6 != '1') {
		reqprefix = null;
		norelease = null;
	}

	let demand_args = [];
	let demand_val = int(cfg.demand || '0');
	if (demand_val > 0)
		demand_args = ['precompiled-active-filter', '/etc/ppp/filter', 'demand', 'idle', '' + demand_val];

	let persist = bool(cfg.persist);
	if (persist != null)
		persist = (persist == '1') ? 'persist' : 'nopersist';

	let maxfail = cfg.maxfail;
	if (!maxfail)
		maxfail = (persist == 'persist') ? '0' : '1';

	let mtu = cfg.mtu;

	let pppname = cfg.pppname;
	if (!pppname)
		pppname = `${proto.proto}-${iface}`;

	let localip = null;
	if (cfg.unnumbered) {
		proto.add_host_dependency('', cfg.unnumbered);
		let ubus = libubus.connect();
		if (ubus) {
			let status = ubus.call(`network.interface.${cfg.unnumbered}`, 'status');
			let subnets = [];
			for (let addr in status?.['ipv4-address'] ?? [])
				push(subnets, `${addr.address}/${addr.mask}`);
			localip = ppp_select_ipaddr(subnets);
			ubus.disconnect();
		}
		if (!localip) {
			proto.block_restart();
			return;
		}
	}

	let keepalive = cfg.keepalive || '5 1';
	let ka_parts = split(keepalive, /[, ]+/);
	let lcp_failure = ka_parts[0];
	let lcp_interval = (length(ka_parts) > 1) ? ka_parts[-1] : '5';
	let lcp_adaptive = (bool(cfg.keepalive_adaptive) ?? '1') != '0';

	if (int(lcp_failure || '0') < 1)
		lcp_failure = null;

	let connect = cfg.connect;
	let disconnect = cfg.disconnect;

	let sourcefilter = (bool(cfg.sourcefilter) == '0');
	let delegate = (bool(cfg.delegate) != '0') ? null : '0';
	norelease = (bool(norelease) == '1') ? '1' : null;

	let argv = [
		'/usr/sbin/pppd',
		'nodetach', 'ipparam', iface,
		'ifname', pppname,
	];

	if (localip)
		push(argv, `${localip}:`);

	if (lcp_failure) {
		push(argv, 'lcp-echo-interval', lcp_interval,
		     'lcp-echo-failure', lcp_failure);
		if (lcp_adaptive)
			push(argv, 'lcp-echo-adaptive');
	}

	if (ipv6)
		push(argv, '+ipv6');
	if (autoipv6)
		push(argv, 'set', `AUTOIPV6=${autoipv6}`);
	if (reqprefix)
		push(argv, 'set', `REQPREFIX=${reqprefix}`);
	if (norelease)
		push(argv, 'set', 'NORELEASE=1');
	if (sourcefilter)
		push(argv, 'set', 'NOSOURCEFILTER=1');
	if (delegate)
		push(argv, 'set', `DELEGATE=${delegate}`);

	push(argv, 'nodefaultroute', 'usepeerdns');

	for (let arg in demand_args)
		push(argv, arg);
	if (persist)
		push(argv, persist);
	push(argv, 'maxfail', maxfail);

	if (cfg.holdoff)
		push(argv, 'holdoff', cfg.holdoff);
	if (cfg.username)
		push(argv, 'user', cfg.username, 'password', cfg.password ?? '');
	if (connect)
		push(argv, 'connect', connect);
	if (disconnect)
		push(argv, 'disconnect', disconnect);

	push(argv, 'ip-up-script', '/lib/netifd/ppp-up');
	if (ipv6)
		push(argv, 'ipv6-up-script', '/lib/netifd/ppp6-up');
	push(argv, 'ip-down-script', '/lib/netifd/ppp-down');
	if (ipv6)
		push(argv, 'ipv6-down-script', '/lib/netifd/ppp-down');

	if (mtu)
		push(argv, 'mtu', mtu, 'mru', mtu);

	for (let arg in extra_args ?? [])
		push(argv, arg);

	if (cfg.pppd_options) {
		let opts = split(cfg.pppd_options, /\s+/);
		for (let opt in opts)
			if (opt) push(argv, opt);
	}

	proto.run_command(argv);
}

function ppp_generic_teardown(proto) {
	let error = int(getenv('ERROR') ?? '5');
	let errorstring = ppp_exitcode_tostring(error);

	if (error == 2) {
		proto.error([errorstring]);
		proto.block_restart();
	} else if (error == 11 || error == 19) {
		proto.error([errorstring]);
		if (bool(proto.config.authfail) == '1')
			proto.block_restart();
	} else if (error != 0) {
		proto.error([errorstring]);
	}

	proto.kill_command();
}

if (have_pppd)
netifd.add_proto({
	name: "ppp",
	"no-device": true,
	available: true,
	lasterror: true,

	config: function(ctx) {
		let d = ctx.data;
		return {
			...ppp_generic_config(d),
			device: d.device,
		};
	},

	setup: function(proto) {
		let cfg = proto.config;
		ppp_generic_setup(proto, cfg, cfg.device ? [cfg.device] : []);
	},

	teardown: function(proto) {
		ppp_generic_teardown(proto);
	},
});

if (have_pppd && length(fsglob('/usr/lib/pppd/*/pppoe.so'))) {
	netifd.add_proto({
		name: "pppoe",
		lasterror: true,

		config: function(ctx) {
			let d = ctx.data;
			return {
				...ppp_generic_config(d),
				ac: d.ac,
				service: d.service,
				ac_mac: d.ac_mac,
				host_uniq: d.host_uniq,
				padi_attempts: d.padi_attempts,
				padi_timeout: d.padi_timeout,
			};
		},

		setup: function(proto) {
			let cfg = proto.config;

			system(['/sbin/modprobe', '-qa', 'slhc', 'ppp_generic', 'pppox', 'pppoe']);

			if (!cfg.mtu)
				cfg.mtu = '1492';

			let extra = ['plugin', 'pppoe.so'];

			if (cfg.ac)
				push(extra, 'rp_pppoe_ac', cfg.ac);
			if (cfg.service)
				push(extra, 'rp_pppoe_service', cfg.service);
			if (cfg.ac_mac)
				push(extra, 'pppoe-mac', cfg.ac_mac);
			if (cfg.host_uniq)
				push(extra, 'host-uniq', cfg.host_uniq);
			if (cfg.padi_attempts)
				push(extra, 'pppoe-padi-attempts', cfg.padi_attempts);
			if (cfg.padi_timeout)
				push(extra, 'pppoe-padi-timeout', cfg.padi_timeout);

			push(extra, `nic-${proto.device}`);

			ppp_generic_setup(proto, cfg, extra);
		},

		teardown: function(proto) {
			ppp_generic_teardown(proto);
		},
	});
}

if (have_pppd && length(fsglob('/usr/lib/pppd/*/pppoatm.so'))) {
	netifd.add_proto({
		name: "pppoa",
		"no-device": true,
		available: true,
		lasterror: true,

		config: function(ctx) {
			let d = ctx.data;
			return {
				...ppp_generic_config(d),
				atmdev: d.atmdev,
				vci: d.vci,
				vpi: d.vpi,
				encaps: d.encaps,
			};
		},

		setup: function(proto) {
			let cfg = proto.config;

			system(['/sbin/modprobe', '-qa', 'slhc', 'ppp_generic', 'pppox', 'pppoatm']);

			let encaps;
			if (cfg.encaps == '1' || cfg.encaps == 'vc')
				encaps = 'vc-encaps';
			else
				encaps = 'llc-encaps';

			let atmdev = cfg.atmdev;
			let vpi = cfg.vpi || '8';
			let vci = cfg.vci || '35';

			let atm_path;
			if (atmdev)
				atm_path = `${atmdev}.${vpi}.${vci}`;
			else
				atm_path = `${vpi}.${vci}`;

			ppp_generic_setup(proto, cfg, [
				'plugin', 'pppoatm.so',
				atm_path,
				encaps,
			]);
		},

		teardown: function(proto) {
			ppp_generic_teardown(proto);
		},
	});
}

if (have_pppd && length(fsglob('/usr/lib/pppd/*/pptp.so'))) {
	netifd.add_proto({
		name: "pptp",
		"no-device": true,
		available: true,
		lasterror: true,

		config: function(ctx) {
			let d = ctx.data;
			return {
				...ppp_generic_config(d),
				server: d.server,
				interface: d.interface,
			};
		},

		setup: function(proto) {
			let cfg = proto.config;
			let iface = proto.iface;

			let server = cfg.server;
			let addrs;

			if (server) {
				let literal = iptoarr(server);
				if (literal) {
					if (length(literal) == 4)
						addrs = [server];
				} else {
					let result = query(server, { type: ['A'], timeout: 5000 });
					addrs = [];
					for (let name, recs in result)
						for (let a in recs?.A ?? [])
							push(addrs, a);
				}
			}

			if (!addrs || !length(addrs)) {
				warn(`pptp: ${iface}: could not resolve server address\n`);
				sleep(5000);
				proto.setup_failed();
				return;
			}

			for (let ip in addrs)
				proto.add_host_dependency(ip, cfg.interface);

			system(['/sbin/modprobe', '-qa', 'slhc', 'ppp_generic', 'ppp_async', 'ppp_mppe', 'ip_gre', 'gre', 'pptp']);
			sleep(1000);

			ppp_generic_setup(proto, cfg, [
				'plugin', 'pptp.so',
				'pptp_server', server,
				'file', '/etc/ppp/options.pptp',
			]);
		},

		teardown: function(proto) {
			ppp_generic_teardown(proto);
		},
	});
}
