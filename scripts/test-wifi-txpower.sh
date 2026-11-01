#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
SCRIPT="$SCRIPT_DIR/../package/network/config/wifi-scripts"
SCRIPT="$SCRIPT/files-ucode/lib/netifd/wireless/mac80211.sh"

node - "$SCRIPT" <<'NODE'
const fs = require('fs');

const source = fs.readFileSync(process.argv[2], 'utf8');
const start = source.indexOf('function setup_phy(');
if (start < 0)
	throw new Error('setup_phy() not found');

const body = source.indexOf('{', start);
let depth = 0;
let quote = null;
let end = -1;

for (let i = body; i < source.length; i++) {
	const ch = source[i];

	if (quote) {
		if (ch === '\\')
			i++;
		else if (ch === quote)
			quote = null;
		continue;
	}

	if (ch === "'" || ch === '"' || ch === '`') {
		quote = ch;
		continue;
	}

	if (ch === '{')
		depth++;
	else if (ch === '}' && --depth === 0) {
		end = i + 1;
		break;
	}
}

if (end < 0)
	throw new Error('unterminated setup_phy()');

const netifd = { set_data() {} };
const get_channel_frequency = () => null;
const set_default = (object, key, value) => {
	if (object[key] == null)
		object[key] = value;
};
const log = () => {};
const commands = [];
const system = command => commands.push(command);
const setup_phy = eval(`(${source.slice(start, end)})`);

function check(txpower, expected) {
	commands.length = 0;
	const config = { txantenna: 1, rxantenna: 1, distance: 0 };
	if (txpower !== undefined)
		config.txpower = txpower;

	setup_phy('phy-test', config, { txantenna: 1, rxantenna: 1 });
	const command = commands.find(value => value.startsWith('iw phy phy-test set txpower '));
	if (command !== `iw phy phy-test set txpower ${expected}`)
		throw new Error(`txpower ${txpower}: got ${command}, expected ${expected}`);
}

check(0, 'fixed 000');
check(1, 'fixed 100');
check(undefined, 'auto');
NODE
