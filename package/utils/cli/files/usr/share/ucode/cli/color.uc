// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

const color_codes = {
	black: 30,
	red: 31,
	green: 32,
	yellow: 33,
	blue: 34,
	magenta: 35,
	cyan: 36,
	white: 37,
	default: 39
};

function color_str(n)
{
	return "\e["+n+"m";
}

function color_code(str)
{
	let n = 0;
	if (substr(str, 0, 7) == "bright_") {
		str = substr(str, 7);
		n += 60;
	}
	if (!color_codes[str])
		return;

	n += color_codes[str];
	return n;
}

export function color_fg(name, str)
{
	let n = color_code(name);
	if (!n)
		return str;

	let ret = color_str(n);
	if (str != null)
		ret += str + color_str(39);

	return ret;
};

export function color_bg(name, str)
{
	let n = color_code(name);
	if (!n)
		return str;

	let ret = color_str(n + 10);
	if (str != null)
		ret += str + color_str(49);

	return ret;
};

export function bold(str)
{
	return color_str(1) + str + color_str(0);
};
