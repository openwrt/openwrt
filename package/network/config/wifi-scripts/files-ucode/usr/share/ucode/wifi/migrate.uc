// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2026 Mikhail Zhilkin <csharper2005@gmail.com>

'use strict';

import { cursor } from "uci";

const uci = cursor();

const iface_keys = {
	"priv_key":		"private_key",
	"priv_key_pwd":		"private_key_passwd"
};

let wireless_changed = false;

function migrate_section_keys(config, sid, mapping) {
	for (let old_key in mapping) {
		let new_key = mapping[old_key];
		let val = uci.get(config, sid, old_key);

		if (val != null) {
			uci.set(config, sid, new_key, val);
			uci.delete(config, sid, old_key);
			system([ "logger", `[${sid}] Migrated: ${old_key} -> ${new_key}` ]);
			wireless_changed = true;
		}
	}
}

uci.load("wireless");
uci.foreach("wireless", "wifi-iface", (s) => {
	migrate_section_keys("wireless", s['.name'], iface_keys);
});

if (wireless_changed)
	uci.commit("wireless");
