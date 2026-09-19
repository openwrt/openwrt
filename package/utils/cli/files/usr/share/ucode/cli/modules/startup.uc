#!/usr/bin/env ucode

'use strict';

import { stat, popen } from 'fs';
import { connect } from 'ubus';
import * as uci from 'uci';

const module_path = '/usr/share/ucode/cli/modules';

function show_config(ctx, argv, named) {
	const handle = popen('/sbin/uci changes');
	let content = handle.read('all');
	return ctx.ok(sprintf('%s', content));
}

function save(ctx, argv, named) {
	const config = argv[0];
	const cursor = uci.cursor();

	cursor.load(config);
	cursor.commit(config);
	let msg = sprintf('Applied changes to configuration "%s".', config);

	return ctx.ok(msg);
}

function reload(ctx, argv, named) {
	system('/sbin/reload_config');
	return ctx.ok('Successfully reloaded configuration.');
}

function reboot(ctx, argv, named) {
	let conn = connect();

	conn.call('system', 'reboot');
	return ctx.ok('Rebooting...');
}

function revert(ctx, argv, named) {
	const config = argv[0];
	const cursor = uci.cursor();

	cursor.load(config);
	cursor.revert(config);
	let msg = sprintf('Reverted changes to configuration "%s".', config);

	return ctx.ok(msg);
}

const Config = {
	changes: {
		help: 'Show the config you want to apply',
		call: show_config,
	},
	reload: {
		help: 'Reload configuration after modification',
		call: reload,
	},
	revert: {
		help: 'Revert modification',
		args: [
		{
			required: true,
			name: "config",
			type: "string",
		}
		],
		call: revert,
	},
	save: {
		help: 'Apply changes to configuration',
		args: [
		{
			required: true,
			name: "config",
			type: "string",
		}
		],
		call: save,
	}
};

const System = {
	reboot: {
		help: 'Reboot the system',
		call: reboot,
	},
};

const Root = {
	configure: {
		help: 'Configure system',
		select_node: 'Config',
	},

	system: {
		help: 'Show system settings',
		select_node: 'System',
	},
};

model.add_nodes({ Root, Config, System });
