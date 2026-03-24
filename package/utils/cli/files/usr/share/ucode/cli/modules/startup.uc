#!/usr/bin/env ucode

'use strict';

import { popen } from 'fs';
import { connect } from 'ubus';
import * as uci from 'uci';

function show_config(ctx, argv, named) {
	const handle = popen('/sbin/uci changes');

	if (!handle)
		return ctx.command_failed('Failed to show configuration changes.');

	let content = handle.read('all');
	let status = handle.close();

	if (status != 0)
		return ctx.command_failed('Failed to show configuration changes.');

	return ctx.ok(sprintf('%s', content));
}

function save(ctx, argv, named) {
	const config = argv[0];
	const cursor = uci.cursor();

	if (!cursor.load(config))
		return ctx.command_failed('Failed to load configuration "%s".', config);

	if (!cursor.commit(config))
		return ctx.command_failed('Failed to commit configuration "%s".', config);

	let msg = sprintf('Applied changes to configuration "%s".', config);

	return ctx.ok(msg);
}

function reload(ctx, argv, named) {
	if (system('/sbin/reload_config') != 0)
		return ctx.command_failed('Failed to reload configuration.');

	return ctx.ok('Successfully reloaded configuration.');
}

function reboot(ctx, argv, named) {
	let conn = connect();

	if (!conn || conn.call('system', 'reboot') == null)
		return ctx.command_failed('Failed to reboot system.');

	return ctx.ok('Rebooting...');
}

function revert(ctx, argv, named) {
	const config = argv[0];
	const cursor = uci.cursor();

	if (!cursor.load(config))
		return ctx.command_failed('Failed to load configuration "%s".', config);

	if (!cursor.revert(config))
		return ctx.command_failed('Failed to revert configuration "%s".', config);

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
