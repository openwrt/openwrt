#!/usr/bin/python
from sys import stdin, stdout
while True:
	c = stdin.read(2)
	if len(c) < 2:
		break
	n1, n2 = ord(c[0]), ord(c[1])
	stdout.write(chr(((n2 & 15) << 4) + ((n2 & 240) >> 4)))
	stdout.write(chr(((n1 & 15) << 4) + ((n1 & 240) >> 4)))
