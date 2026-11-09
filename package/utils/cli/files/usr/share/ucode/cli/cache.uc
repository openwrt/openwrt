// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 Felix Fietkau <nbd@nbd.name>
'use strict';

const CACHE_DEFAULT_TIMEOUT = 5;

function cache_get(key, fn, timeout)
{
	let now = time();
	let entry = this.entries[key];
	if (entry) {
		if (now < entry.timeout)
			return entry.data;

		if (!fn)
			delete this.entries[key];
	}

	if (!fn)
		return;

	let data = fn();
	if (!entry)
		this.entries[key] = entry = {};
	timeout ??= CACHE_DEFAULT_TIMEOUT;
	entry.timeout = now + timeout;
	entry.data = data;

	return data;
}

function cache_remove(key)
{
	delete this.entries[key];
}

function cache_gc() {
	let now = time();
	for (let key, entry in this.entries)
		if (now > entry.timeout)
			delete this.entries[key];
}

const cache_proto = {
	get: cache_get,
	remove: cache_remove,
	gc: cache_gc,
};

export function new(model) {
	model.cache_proto ??= { model, ...cache_proto };
	let cache = proto({
		entries: {},
	}, model.cache_proto);
	cache.gc_interval = model.uloop.interval(10000, () => {
		cache.gc();
	});

	return cache;
};
