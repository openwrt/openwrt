#!/usr/bin/env ucode

'use strict';

import * as uci from 'uci';
import * as ubus from 'ubus';

let tz = {};

function get_timezones() {
	const conn = ubus.connect();
	let res = conn.call('luci', 'getTimezones');

	for (let key in res) {
		let value = res[key].tzstring;
		if (index(values(tz), value) > -1)
			continue;

		tz[key] = value ? value : null;
	}
}

function feedback(ctx, success) {
	if (success == true || !success)
		return ctx.ok('Changes applied');
	return ctx.command_failed('Changes not applied');
}

function validate_timezone(zone) {
	if (index(values(tz), zone) < 0)
		return false;

	return true;
}

function validate_zonename(zone) {
	if (index(keys(tz), zone) < 0)
		return false;

	return true;
}

function add_system_option(ctx, argv, named) {
	const cursor = uci.cursor();
	cursor.load('system');

	let section = cursor.get_first('system', 'system');

	if (!section)
		section = cursor.add('system', 'system');

	for (let key in keys(named)) {
		let zone_exists;
		let value = named[key];

		if (key == 'timezone') {
			zone_exists = validate_timezone(value);

			if (zone_exists != true)
				return ctx.not_found('Timezone "%s" not found.', value);

		}

		if (key == 'zonename') {
			zone_exists = validate_zonename(value);

			if (zone_exists != true)
				return ctx.not_found('Zonename "%s" not found.', value);
		}

		let success = cursor.set('system', section, key, value);
		cursor.save('system');
		feedback(ctx, success);
	}
}

function add_ntp_option(ctx, argv, named) {
	const cursor = uci.cursor();
	cursor.load('system');

	let section = cursor.get_first('system', 'ntp');

	if (!section)
		section = cursor.add('system', 'ntp');

	for (let key in keys(named)) {
		let value = named[key];
		let list;
		let success;

		if (key == 'server') {
			list = cursor.get('system', section, key);
			
			if (!key)
				success = cursor.set('system', section, key, [value]);
			else
				success = cursor.list_append('system', section, key, value);
		}
		else
			success = cursor.set('system', section, key, value);

		cursor.save('system');
		feedback(ctx, success);
	}
}

get_timezones();

const Add_Sys = {
	system: {
		help: 'Add system options',
		named_args: {
			hostname: {
				required: false,
				default: 'OpenWrt',
				args: {
					type: 'string',
					required: true,
				},
				validate: function(ctx, argv) {
					let name = argv[0];

					if (!name)
						return false;

					if (match(name, /\./))
						return false;

					return true;
				}
			},
			description: {
				required: false,
				args: {
					type: 'string',
					required: true,
				},
			},
			notes: {
				required: false,
				args: {
					type: 'string',
					required: true,
				},
			},
			buffersize: {
				required: false,
				args: {
					type: 'int',
					min: 1,
					required: true,
				}
			},
			conloglevel: {
				required: false,
				args: {
					type: 'int',
					min: 1,
					max: 8,
					required: true,
				}
			},
			cronloglevel: {
				required: false,
				args: {
					type: 'int',
					min: 1,
					max: 7,
					required: true,
				}
			},
			log_buffer_size: {
				required: false,
				args: {
					type: 'int',
					min: 1,
					required: true,
				}
			},
			log_file: {
				required: false,
				args: {
					type: 'path',
					required: true,
				}
			},
			log_hostname: {
				required: false,
				args: {
					type: 'string',
					required: true,
				}
			},
			log_ip: {
				required: false,
				args: {
					type: 'host',
					required: true,
				}
			},
			log_port: {
				required: false,
				default: 514,
				args: {
					type: 'int',
					min: 1,
					max: 65535,
					required: true,
				}
			},
			log_prefix: {
				required: false,
				args: {
					type: 'string',
					required: true,
				},
			},
			log_proto: {
				required: false,
				args: {
					type: 'enum',
					ignore_case: true,
					value: [ 'udp', 'tcp' ],
					required: true,
				},
			},
			log_remote: {
				required: false,
				default: true,
				args: {
					type: 'bool',
					required: true,
				},
			},
			log_size: {
				required: false,
				default: 128,
				args: {
					type: 'int',
					min: 1,
					required: true,
				},
			},
			log_trailer_null: {
				required: false,
				default: false,
				args: {
					type: 'bool',
					required: true,
				},
			},
			log_type: {
				required: false,
				default: 'circular',
				args: {
					type: 'enum',
					ignore_case: true,
					value: [ 'circular', 'file' ],
					required: true,
				},
			},
			ttylogin: {
				required: false,
				default: false,
				args: {
					type: 'bool',
					required: true,
				},
			},
			urandom_seed: {
				required: false,
				default: false,
				args: {
					type: 'bool',
					required: true,
				},
			},
			timezone: {
				required: false,
				default: 'UTC',
				args: {
					type: 'string',
					ignore_case: true,
					value: values(tz),
					required: true,
				},
			},
			zonename: {
				required: false,
				default: 'UTC',
				args: {
					type: 'string',
					ignore_case: true,
					value: keys(tz),
					required: true,
				},
			},
			zram_comp_algo: {
				required: false,
				args: {
					type: 'enum',
					ignore_case: true,
					value: [ 'lzo', 'lzo-rle', 'lz4', 'zstd' ],
				},
			},
			zram_size_mb: {
				required: false,
				args: {
					type: 'int',
					min: 1,
				},
				validate: function(ctx, argv, named) {
					let value = argv[0];

					if (value % 2058 != 0)
						return false;
					return true;
				}
			}
		},
		call: add_system_option,
	},

	ntp: {
		help: 'add ntp option',
		named_args: {
			server: {
				required: false,
				args: {
					type: 'host',
					required: true,
				}
			},
			enabled: {
				required: true,
				default: false,
				args: {
					type: 'bool',
					require: true,
				}
			},
			enable_server: {
				required: false,
				args: {
					type: 'bool',
					required: true,
				}
			},
			interface: {
				required: false,
				args: {
					type: 'string',
					min: 1,
					required: true,
				}
			},
			use_dhcp: {
				required: false,
				default: true,
				args: {
					type: 'bool',
					required: true,
				}
			}
		},
		call: add_ntp_option,
	},
};

const Root = {
	configure: {
		help: 'Configure system',
		select_node: 'Config',
	},
};

const Config = {
	system: {
		help: 'Add system options',
		select_node: 'System',
	},
};

const System = {
	add: {
		help: 'Add system options',
		select_node: 'Add_Sys',
	},
};

model.add_nodes({ Root, Config, System, Add_Sys });
