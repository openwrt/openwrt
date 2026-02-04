// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

export function time_format(val)
{
	let ret = `${val % 60}s`;

	val /= 60;
	if (!val)
		return ret;

	ret = `${val % 60}m ${ret}`;

	val /= 60;
	if (!val)
		return ret;

	ret = `${val % 24 }h ${ret}`;

	val /= 24;
	if (!val)
		return ret;

	return `${val}d ${ret}`;
};
