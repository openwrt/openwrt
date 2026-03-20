#!/usr/bin/env ucode

'use strict';

import * as uci from 'uci';

const cursor = uci.cursor();
cursor.load('firewall');

function create_section(type) {
	let section = cursor.get_first('firewall', type);

	if (type == 'defaults' && section)
		return section;

	section = cursor.add('firewall', type);

	return section;
}

function apply_changes(ctx, named, type) {
	let section = create_section(type);

	if (length(named) == 0) {
		cursor.save('firewall');
		return ctx.ok('Unnamed section added.');
	}

	for (let key in keys(named)) {
		let value = named[key];
		let success = cursor.set('firewall', section, key, value);
		cursor.save('firewall');
		if (!success)
			return ctx.command_failed('Change %s.%s.%s=%s not applied',
				'firewall', section, key, value);
	}

	return ctx.ok('Changes applied');
}

function parseCidr (cidr, type) {
	let m = split(cidr, '/', 2);

	switch (type) {
		case 'cidr4':
			if (m && +m[1] <= 32 && length(iptoarr(m[0])) == 4)
				return true;
			else
				return false;
		default: 
			if (m && +m[1] <= 128 && length(iptoarr(m[0])) == 16)
				return true;
			else
				return false;
	}
}

function validateEnabled (enabled) {
	return match(enabled, /^(yes|no|1|0)$/i);
}

function parseMac (mac) {
	mac = lc(mac);
	let arr = split(mac, ':');

	if (length(arr) != 6 || length(filter(arr, (v) => !match(v, /^[0-9a-f][0-9a-f]$/))))
		return false;
	return true;
}

function validateTime (time) {
	let t, msg;

	/*
	 * if no parameter given it resolves to always.
	 */
	if (!time)
		return true;

	t = match(time, /^([0-9]{1,2})(:([0-9]{1,2})(:([0-9]{1,2}))?)?$/);

	if (t == null || t[1] > 23 || t[3] > 59 || t[5] > 59)
		return false;

	return true;
}

function validateDate (date) {
	let d, t;
	/*
	 * if no parameter given it resolves to always.
	 */
	 if (!date)
	 	return true;

	d = match(date, /^([0-9]{4})(-([0-9]{1,2})(-([0-9]{1,2})(T([0-9:]+))?)?)?$/);

	/*
	 * applied parsing logic from fw4:
	 * https://github.com/openwrt/firewall4/blob/b6e5157527d361f99ad52eaa6da273cb0f2dfd59/root/usr/share/ucode/fw4.uc#L1341
	 * Somehow it seems to be fixed to evaluate to fals year after 2038.
	 */
	if (d == null || d[1] < 1970 || d[1] > 2038 || d[3] > 12 || d[5] > 31)
		return false;

	t = validateTime(d[7] ?? null);

	return t;
}

function validateMark (mark) {
	let mask = 0xFFFFFFFF;

	if (!mark)
		return false;

	let m = match(mark, /^!?(0?x?[0-9a-f]+)(\/(0?x?[0-9a-f]+))?$/i);

	if (!m)
		return false;

	if (+m[1] > mask)
		return false;

	if (m[3] && m[3] > mask)
		return false;
	return true;
}

function validateLimit (limit) {
	let m = match(limit, /^([0-9]+)(\/([a-z]+)?)$/);
	let choices = [ "second", "minute", "hour", "day" ];
	let contains;

	if (!m)
		return false;

	if (m[3]) {
		contains = false;
		for (let i = 0; i < length(choices); i++) {
			if (lc(substr(choices[i], 0, length(m[3]))) == m[3]) {
				contains = true;
				break;
			}
		}

		return contains;
	}

	return true;
}

function validateIP (ip) {
	let mv4, mv6, ipv4, ipv6, mask, valid_mask;

	mv4 = match(ip, /^([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3})([/-]?([0-9]{1,2}))?$/);
	mv6 = match(ip, /^([0-9a-fA-F:.]+)([/-]?([0-9]{1,3}))?$/);

	if (!mv4 && !mv6)
		return false;

	if(mv4) {
		ipv4 = iptoarr(mv4[1]);

		if (!ipv4)
			return false;

		mask = mv4[3];
		if (mask) {
			if (int(mask) < 0 || int(mask) > 32)
				return false;
		}
	}
	else {
		ipv6 = iptoarr(mv6[1]);

		if (!ipv6)
			return false;

		mask = mv6[3];
		if (mask) {

			if (int(mask < 0 || int(mask) > 128))
				return false;
		}
	}
	return true;
}

function validatePort(port) {
	let m;

	if (!port)
		return false;

	m = match(port, /^([0-9]{1,5})([-:]([0-9]{1,5}))?$/i);

	if (!m)
		return false;

	if (m[3]) {
		let min_port = +m[1];
		let max_port = +m[3];

		if (min_port > max_port ||
		    min_port < 0 || max_port < 0 ||
		    min_port > 65535 || max_port > 65535)
			return false;
	}
	else {
	let pn = +m[1];

	if (pn < 0 || pn > 65535)
		return false;
	}

	return true;
}

function validateMonthdays (md) {
	let m;

	if (!md)
		return false;

	m = match(md, /^!?(.*)$/);

	if (!m)
		return false;

	for (let entry in split(m[1], ' ')) {
		let mday = match(entry, /^[0-9]{1,2}$/);

		if (!mday)
			return false;

		mday = int(mday[0]);
		if (mday < 1 || mday > 31)
			return false;
	}

	return true;
}

function validateWeekdays (wd) {
	let m;
	const weekdays = [ 'mon', 'tue', 'wed', 'thu', 'fri', 'sat', 'sun' ];

	if (!wd)
		return false;

	m = match(wd, /^!?(.*)$/);

	if (!m)
		return false;

	for (let day in split(m[1], ' ')) {
		let valid = index(weekdays, day) >= 0 ? true : false;
		
		if (!valid)
			return false;
	}

	return true;
}

function add_defaults_option(ctx, argv, named) {
	let synflood_rate = true;

	if (named.synflood_rate) {
		synflood_rate = validateLimit(named.synflood_rate);

		if (!synflood_rate)
			return ctx.invalid_argument('Invalid entry for synflood_rate');
	}

	apply_changes(ctx, named, 'defaults');
}

function add_zone_option(ctx, argv, named) {
	let masq_src, masq_dest;
	let log_limit, subnet;

	masq_src = masq_dest = true;
	log_limit = subnet = true;

	if (named.masq_src) {
		masq_src = validateIP(named.masq_src);

		if (!masq_src)
			return ctx.invalid_argument('Invalid entry for masq_src');
	}

	if (named.masq_dest) {
		masq_dest = validateIP(named.masq_dest);

		if (!masq_dest)
			return ctx.invalid_argument('Invalid entry for masq_dest');
	}

	if (named.log_limit) {
		log_limit = validateLimit(named.log_limit);

		if (!log_limit)
			return ctx.invalid_argument('Invalid entry for log_limit');
	}

	if (named.subnet) {
		subnet = validateIP(named.subnet);

		if (!subnet)
			return ctx.invalid_argument('Invalid entry for subnet');
	}

	apply_changes(ctx, named, 'zone');
}

function add_forwarding_option(ctx, argv, named) {
	apply_changes(ctx, named, 'forwarding');
}

function add_rule_option(ctx, argv, named) {
	let limit, smark, sxmark;
	let monthdays, weekdays;
	let start_time, stop_time;
	let start_date, stop_date;
	let src_port, dest_port;
	let src_ip, dest_ip;
	let mark;

	monthdays = weekdays = true;
	start_time = stop_time = true;
	start_date = stop_date = true;
	limit = smark = sxmark = true;
	src_port = dest_port = true;
	src_ip = dest_ip = true;
	mark = true;

	if (named.limit) {
		limit = validateLimit(named.limit);

		if (!limit)
			return ctx.invalid_argument('Invalid entry for limit');
	}

	if (named.set_mark) {
		smark = validateMark(named.set_mark);

		if (!smark)
			return ctx.invalid_argument('Invalid entry for set_mark');
	}

	if (named.set_xmark) {
		sxmark = validateMark(named.set_xmark);

		if (!sxmark)
			return ctx.invalid_argument('Invalid entry for set_mark');
	}

	if (named.monthdays) {
		monthdays = validateMonthdays(named.monthdays);

		if (!monthdays)
			return ctx.invalid_argument('Invalid entry for monthdays');
	}

	if (named.weekdays) {
		weekdays = validateWeekdays(named.weekdays);

		if (!weekdays)
			return ctx.invalid_argument('Invalid entry for weekdays');
	}

	if (named.start_time) {
		start_time = validateTime(named.start_time);

		if (!start_time)
			return ctx.invalid_argument('Invalid entry for start_time');
	}

	if (named.stop_time) {
		stop_time = validateTime(named.stop_time);

		if (!stop_time)
			return ctx.invalid_argument('Invalid entry for stop_time');
	}

	if (named.start_date) {
		start_date = validateDate(named.start_date);

		if (!start_date)
			return ctx.invalid_argument('Invalid entry for start_date');
	}

	if (named.stop_date) {
		stop_date = validateDate(named.stop_time);

		if (!stop_date)
			return ctx.invalid_argument('Invalid entry for stop_date');
	}

	if (named.mark) {
		mark = validateMark(named.mark);

		if (!mark)
			return ctx.invalid_argument('Invalid entry for mark');
	}

	if (named.target == 'MARK') {
		if (!named.set_mark || !named.set_xmark)
			return ctx.missing_argument('Arguments set_mark and set_xmark are missing');
	}

	if (named.src_port) {
		src_port = validatePort(named.src_port);

		if (!src_port)
			return ctx.invalid_argument('Invalid entry for src_port');
	}

	if (named.dest_port) {
		dest_port = validatePort(named.dest_port);

		if (!dest_port)
			return ctx.invalid_argument('Invalid entry for dest_port');
	}

	if (named.src_ip) {
		src_ip = validateIP(named.src_ip);

		if (!src_ip)
			return ctx.invalid_argument('Invalid entry for src_ip');
	}

	if (named.dest_ip) {
		dest_ip = validateIP(named.dest_ip);

		if (!dest_ip)
			return ctx.invalid_argument('Invalid entry for dest_ip');
	}

	apply_changes(ctx, named, 'rule');
}

function add_ipset_option(ctx, argv, named) {
	let arr = split(named.entry, ' ');

	for (let entry in arr) {
		let ipv4 = length(iptoarr(entry)) == 4;
		let ipv6 = length(iptoarr(entry)) == 16;
		let cidr4 = parseCidr(entry, 'cidr4');
		let cidr6 = parseCidr(entry, 'cidr6');
		let mac = parseMac(entry);

		if (!ipv4 && !ipv6 && !cidr4 && !cidr6 && !mac) {
			let msg = sprintf('Entry "%s" is neither ip, cidr or mac',
				entry);
			return ctx.invalid_argument(msg);
		}
	}

	apply_changes(ctx, named, 'ipset');
}

function add_redirect_option(ctx, argv, named) {
	let enabled, monthdays, weekdays;
	let start_time, stop_time;
	let start_date, stop_date;
	let src_port, src_dport, dest_port;
	let src_ip, src_dip, dest_ip;
	let mark;

	enabled = monthdays = weekdays = true;
	start_time = stop_time = true;
	start_date = stop_date = true;
	src_port = src_dport = dest_port = true;
	src_ip = src_dip = dest_ip = true;
	mark = true;

	if (named.enabled) {
		enabled = validateEnabled(named.enabled);

		if (!enabled)
			return ctx.invalid_argument(
				'enabled option has to be "yes", "no", "1", "0"'
			);
	}

	if (named.monthdays) {
		monthdays = validateMonthdays(named.monthdays);

		if (!monthdays)
			return ctx.invalid_argument('Invalid entry for monthdays');
	}

	if (named.weekdays) {
		weekdays = validateWeekdays(named.weekdays);

		if (!weekdays)
			return ctx.invalid_argument('Invalid entry for weekdays');
	}

	if (named.start_time) {
		start_time = validateTime(named.start_time);

		if (!start_time)
			return ctx.invalid_argument('Invalid entry for start_time');
	}

	if (named.stop_time) {
		stop_time = validateTime(named.stop_time);

		if (!stop_time)
			return ctx.invalid_argument('Invalid entry for stop_time');
	}

	if (named.start_date) {
		start_date = validateDate(named.start_date);

		if (!start_date)
			return ctx.invalid_argument('Invalid entry for start_date');
	}

	if (named.stop_date) {
		stop_date = validateDate(named.stop_date);

		if (!stop_date)
			return ctx.invalid_argument('Invalid entry for stop_date');
	}

	if (named.mark) {
		mark = validateMark(named.mark);

		if (!mark)
			return ctx.invalid_argument('Invalid entry for mark');
	}

	if (named.src_port) {
		src_port = validatePort(named.src_port);

		if (!src_port)
			return ctx.invalid_argument('Invalid entry for src_port');
	}

	if (named.src_dport) {
		src_dport = validatePort(named.src_dport);

		if (!src_dport)
			return ctx.invalid_argument('Invalid entry for src_dport');
	}

	if (named.dest_port) {
		dest_port = validatePort(named.dest_port);

		if (!dest_port)
			return ctx.invalid_argument('Invalid entry for dest_port');
	}

	if (named.src_ip) {
		src_ip = validateIP(named.src_ip);

		if (!src_ip)
			return ctx.invalid_argument('Invalid entry for src_ip');
	}

	if (named.src_dip) {
		src_dip = validateIP(named.src_dip);

		if (!src_dip)
			return ctx.invalid_argument('Invalid entry for src_dip');
	}

	if (named.dest_ip) {
		dest_ip = validateIP(named.dest_ip);

		if (!dest_ip)
			return ctx.invalid_argument('Invalid entry for dest_ip');
	}

	apply_changes(ctx, named, 'redirect');
}

function add_include_option(ctx, argv, named) {
	if (match(named.type, /nftables/) && !named.position)
		return ctx.missing_argument('Missing argument: %s', 'position');

	if (match(named.position, /^chain-/) && !named.chain)
			return ctx.missing_argument('Missing argument: %s', 'chain');

	apply_changes(ctx, named, 'include');
}

const Add_Fw = {
	defaults: {
		help: 'Add defaults to firewall',
		named_args: {
			input: {
				required: false,
				default: 'ACCEPT',
				args: {
					type: 'enum',
					ignore_case: true,
					value: [ 'ACCEPT', 'REJECT', 'DROP' ],
				}
			},
			output: {
				required: false,
				default: 'ACCEPT',
				args: {
					type: 'enum',
					ignore_case: true,
					value: [ 'ACCEPT', 'REJECT', 'DROP' ],
				}
			},
			forward: {
				required: false,
				default: 'REJECT',
				args: {
					type: 'enum',
					ignore_case: true,
					value: [ 'ACCEPT', 'REJECT', 'DROP' ],
				}
			},
			drop_invalid: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			syn_flood: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			syn_flood_protect: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			synflood_rate: {
				required: false,
				default: '25/s',
				args: {
					help: 'Set to packets/second (e.g. 25/s)',
					type: 'string',
				}
			},
			synflood_burst: {
				required: false,
				default: 50,
				args: {
					help: 'Set burst limit for SYN packets',
					type: 'int',
					min: 0,
				}
			},
			tcp_syncookies: {
				required: false,
				default: true,
				args: {
					type: 'bool',
				}
			},
			tcp_ecn: {
				required: false,
				default: 0,
				args: {
					help: '0: Disable, 1: Enable, 2: Enable when requested for ingress',
					type: 'int',
					min: 0,
					max: 2,
				}
			},
			tcp_window_scaling: {
				required: false,
				default: true,
				args: {
					type: 'bool',
				}
			},
			accept_redirects:  {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			accept_source_route: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			custom_chains: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			disable_ipv6: {
				required: false,
				default: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			flow_offloading: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			flow_offloading_hw: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			tcp_reject_code: {
				required: false,
				default: 0,
				args: {
					required: true,
					type: 'int',
					min: 0,
					max: 2,
				}
			},
			any_reject_code: {
				required: false,
				default: 1,
				args: {
					required: true,
					type: 'int',
					min: 0,
					max: 2,
				}
			},
			auto_helper: {
				required: false,
				default: true,
				args: {
					type: 'bool',
				}
			},
			auto_includes: {
				required: false,
				default: true,
				args: {
					type: 'bool',
				}
			},
		},
		call: add_defaults_option,
	},

	zone: {
		help: 'Add zone to firewall',
		named_args: {
			name: {
				required: true,
				args: {
					required: true,
					type: 'string',
					max: 11,
				}
			},
			network: {
				help: 'Specify list of interfaces (e.g. "wan wan6")',
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			masq: {
				required: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			masq6: {
				required: false,
				default: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			masq_src: {
				help: 'Specify list of subnets (e.g. 0.0.0.0/0). Negation is also possible.',
				required: false,
				default: '0.0.0.0/0',
				args: {
					required: true,
					type: 'string',
				}
			},
			masq_dest: {
				help: 'Specify list of subnets (e.g. 0.0.0.0/0). Negation is also possible.',
				required: false,
				default: '0.0.0.0/0',
				args: {
					required: true,
					type: 'string',
				}
			},
			masq_allow_invalid: {
				required: false,
				default: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			mtu_fix: {
				required: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			input: {
				required: false,
				args: {
					required: true,
					type: 'enum',
					ignore_case: true,
					value: [ 'ACCEPT', 'REJECT', 'DROP' ],
				}
			},
			output: {
				required: false,
				args: {
					required: true,
					type: 'enum',
					ignore_case: true,
					value: [ 'ACCEPT', 'REJECT', 'DROP' ],
				}
			},
			forward: {
				required: false,
				args: {
					required: true,
					type: 'enum',
					ignore_case: true,
					value: [ 'ACCEPT', 'REJECT', 'DROP' ],
				}
			},
			family: {
				help: 'Specify address family',
				required: false,
				args: {
					required: true,
					type: 'enum',
					value: [ 'any', 'ipv4', 'ipv6' ],
				}
			},
			log: {
				required: false,
				default: 0,
				args: {
					required: true,
					type: 'int',
					min: 0,
					max: 1,
				}
			},
			log_limit: {
				help: 'Limit amount of log messages per interval (e.g. 10/minute)',
				required: false,
				default: '10/minute',
				args: {
					required: true,
					type: 'string',
				}
			},
			device: {
				help: 'List L3 network interface names',
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			subnet: {
				help: 'List of IP subnets attached to this zone',
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			custom_chains: {
				required: false,
				default: true,
				args: {
					required: true,
					type: 'bool',
				}
			},
			enabled: {
				required: false,
				default: true,
				args: {
					type: 'bool',
				}
			},
			auto_helper: {
				required: false,
				args: {
					type: 'bool',
				}
			},
			helper: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
		},
		call: add_zone_option,
	},

	forwarding: {
		help: 'Add forwarding to firewall',
		named_args: {
			name: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			src: {
				required: true,
				args: {
					required: true,
					type: 'string',
				}
			},
			dest: {
				required: true,
				args: {
					required: true,
					type: 'string',
				}
			},
			family: {
				required: false,
				default: 'any',
				args: {
					type: 'enum',
					value: [ 'any', 'ipv4', 'ipv6' ],
				}
			},
			enabled: {
				required: false,
				default: true,
				args: {
					required: true,
					type: 'bool',
				}
			},
		},
		call: add_forwarding_option,
	},

	rule: {
		help: 'Add rule to firewall',
		named_args: {
			name: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			src: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			src_ip: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			src_mac: {
				required: false,
				args: {
					required: true,
					type: 'macaddr',
				}
			},
			src_port: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			proto: {
				required: false,
				default: 'tcp udp',
				args: {
					required: true,
					type: 'string',
				}
			},
			icmp_type: {
				required: false,
				default: 'any',
				args: {
					required: true,
					type: 'string',
				}
			},
			dest: {
				required: false,
				args: {
					type: 'string',
				}
			},
			dest_ip: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			dest_port: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			mark: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			start_date: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			stop_date: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			start_time: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			stop_time: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			weekdays: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			monthdays: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			utc_time: {
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			target: {
				required: true,
				default: 'DROP',
				args: {
					type: 'enum',
					ignore_case: true,
					value: [ 'ACCEPT', 'REJECT', 'DROP', 'MARK', 'NOTRACK' ],
				}
			},
			set_mark: {
				required: false,
				available: function (ctx, argv, named) {
					if (named.target == 'MARK')
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'string',
				}
			},
			set_xmark: {
				required: false,
				available: function (ctx, argv, named) {
					if (named.target == 'MARK')
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'string',
				}
			},
			family: {
				required: false,
				args: {
					required: true,
					type: 'enum',
					value: [ 'any', 'ipv4', 'ipv6' ],
				}
			},
			limit: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			limit_burst: {
				required: false,
				default: 5,
				args: {
					required: true,
					type: 'int',
					min: 0,
				}
			},
			enabled: {
				required: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			device: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			direction: {
				required: false,
				args: {
					required: true,
					type: 'enum',
					value: [ 'in', 'out' ],
				}
			},
			set_helper: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			helper: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
		},
		call: add_rule_option,
	},

	redirect: {
		help: 'Add redirect to firewall',
		named_args: {
			name: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			src: {
				required: true,
				available: function (ctx, argv, named) {
					if (named.target == 'DNAT')
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'string',
				}
			},
			src_ip: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			src_dip: {
				required: true,
				available: function (ctx, argv, named) {
					if (named.target == 'SNAT')
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'string',
				}
			},
			src_mac: {
				required: false,
				args: {
					required: true,
					type: 'macaddr',
				}
			},
			src_port: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			src_dport: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			proto: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			dest: {
				required: true,
				available: function (ctx, argv, named) {
					if (named.target == 'SNAT')
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'string',
				}
			},
			dest_ip: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			dest_port: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			ipset: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			mark: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			start_date: {
				required: false,
				args: {
					required: false,
					type: 'string',
				}
			},
			stop_date: {
				required: false,
				args: {
					required: false,
					type: 'string',
				}
			},
			start_time: {
				required: false,
				args: {
				required: false,
					type: 'string',
				}
			},
			stop_time: {
				required: false,
				args: {
					required: false,
					type: 'string',
				}
			},
			weekdays: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			monthdays: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			utc_time: {
				required: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			target: {
				required: false,
				args: {
					required: true,
					type: 'enum',
					default: 'DNAT',
					ignore_case: true,
					value: [ 'DNAT', 'SNAT' ],
				}
			},
			family: {
				required: false,
				args: {
					required: true,
					type: 'enum',
					value: [ 'any', 'ipv4', 'ipv6' ],
				}
			},
			reflection: {
				required: false,
				args: {
					required: true,
					type: 'bool',
				}
			},
			reflection_src: {
				required: false,
				default: 'internal',
				available: function (ctx, argv, named) {
					if (named.reflection)
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'enum',
					value: [ 'internal', 'external' ],
				}
			},
			reflection_zone: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			limit: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
			limit_burst: {
				required: false,
				default: '5',
				args: {
					required: true,
					type: 'int',
					min: 0,
				}
			},
			enabled: {
				required: false,
				args: {
					required: true,
					type: 'string',
				},
			},
			helper: {
				required: false,
				args: {
					required: true,
					type: 'string',
				}
			},
		},
		call: add_redirect_option,
	},

	ipset: {
		help: 'Add ipset to firewall',
		named_args: {
			enable: {
				help: 'Allows to disable the declaration of the ipset without the need to delete the section',
				required: false,
				default: true,
				args: {
					type: 'bool',
				}
			},
			comment: {
				required: false,
				args: {
					type: 'bool',
				}
			},
			name: {
				help: 'Specify the firewall internal name of the ipset',
				required: true,
				args: {
					required: true,
					type: 'string',
				}
			},
			family: {
				help: 'Specify the address family for the ipset',
				required: false,
				default: 'ipv4',
				args: {
					required: true,
					type: 'enum',
					value: [ 'any', 'ipv4', 'ipv6' ],
				}
			},
			match: {
				help: 'Specify the matched datatypes',
				required: true,
				args: {
					required: true,
					type: 'enum',
					value: [
					'src_ip',
					'src_port',
					'src_mac',
					'src_net',
					'src_set',
					'dest_ip',
					'dest_port',
					'dest_mac',
					'dest_net',
					'dest_set',
					],
				}
			},
			maxelem: {
				required: false,
				default: 65536,
				args: {
					required: true,
					type: 'int',
					min: 0,
					max: 65536,
				}
			},
			timeout: {
				required: false,
				default: 0,
				args: {
					required: true,
					type: 'int',
					min: 0,
				}
			},
			entry: {
				help: 'Specify the value of the datatype (IP, CIDR, MAC)',
				required: false,
				args: {
					required: true,
					type: 'string',
				},
			},
			loadfile: {
				help: 'A path URL that contains a list of CIDR',
				required: false,
				args: {
					required: true,
					type: 'path',
				}
			},
		},
		call: add_ipset_option,
	},

	include: {
		help: 'Add include to firewall',
		named_args: {
			enabled: {
				help: 'Allows to disable the corresponding include without having to delete the section',
				required: false,
				default: false,
				args: {
					type: 'bool',
				}
			},
			type: {
				help: 'Specify the type of include (script) for fw3 and (nftables) for fw4',
				required: false,
				default: 'script',
				args: {
					required: true,
					type: 'enum',
					value: [ 'script', 'nftables' ],
				}
			},
			path: {
				help: 'Specify the filename to include',
				required: true,
				args: {
					required: true,
					type: 'path',
				}
			},
			position: {
				help: 'Specify the position at which the rule will be inserted',
				required: false,
				available: function (ctx, argv, named) {
					if (match(named.type, /nftables/))
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'enum',
					value: [
					'ruleset-pre',
					'ruleset-post',
					'table-pre',
					'table-post',
					'chain-pre',
					'chain-post'
					]
				}
			},
			chain: {
				help: 'Specify the chain in which the rules will be inserted',
				required: false,
				available: function (ctx, argv, named) {
					if (match(named.position, /^chain-/))
						return true;
					return false;
				},
				args: {
					required: true,
					type: 'string',
				}
			},
		},
		call: add_include_option,
	},
};

const Root = {
	configure: {
		help: 'Configure system',
		select_node: 'Config',
	},
};

const Config = {
	firewall: {
		help: 'Add firewall options',
		select_node: 'Firewall',
	},
};

const Firewall = {
	add: {
		help: 'Add firewall options',
		select_node: 'Add_Fw',
	},
};

model.add_nodes({ Root, Config, Firewall, Add_Fw });
