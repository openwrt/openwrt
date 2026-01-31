#!/usr/bin/ucode

import { stdin, open, error } from 'fs';

if (length(ARGV) == 0 && stdin.isatty()) {
	warn("usage: b64decode [stdin|path]\n");
	exit(1);
}

let fp = stdin;
let source = ARGV[0];

if (source) {
	fp = open(source);
	if (!fp) {
		warn('b64decode: unable to open ${source}: ${error()}\n');
		exit(1);
	}
}

print(b64dec(fp.read("all")));
fp.close();
exit(0);
