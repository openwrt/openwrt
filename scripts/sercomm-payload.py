#!/usr/bin/env python3

import argparse
import hashlib
import os

def create_output(args):
	if (args.pid_file):
		pid_st = os.stat(args.pid_file)
		pid_size = pid_st.st_size

		pid_f = open(args.pid_file, 'r+b')
		pid_bytes = pid_f.read(pid_size)
		pid_f.close()
	else:
		pid_bytes = bytes.fromhex(args.pid)

	sha256 = hashlib.sha256()

	out_f = open(args.output_file, 'w+b')
	# Write PID bytes first
	out_f.write(pid_bytes)

	# Save position and write placeholder for the 32-byte SHA256 digest
	hash_pos = out_f.tell()
	out_f.write(b'\0' * 32)

	# Read input file in chunks to calculate hash and stream directly to output
	# This avoids loading large firmware files into memory at once (O(1) memory usage)
	in_f = open(args.input_file, 'r+b')
	while True:
		chunk = in_f.read(65536)
		if not chunk:
			break
		sha256.update(chunk)
		out_f.write(chunk)
	in_f.close()

	# Seek back and write the actual hash digest
	out_f.seek(hash_pos)
	out_f.write(sha256.digest())

	out_f.close()

def main():
	global args

	parser = argparse.ArgumentParser(description='')

	parser.add_argument('--input-file',
		dest='input_file',
		action='store',
		type=str,
		help='Input file')

	parser.add_argument('--output-file',
		dest='output_file',
		action='store',
		type=str,
		help='Output file')

	parser.add_argument('--pid-file',
		dest='pid_file',
		action='store',
		type=str,
		help='Sercomm PID file')

	parser.add_argument('--pid',
		dest='pid',
		action='store',
		type=str,
		help='Sercomm PID')

	args = parser.parse_args()

	if ((not args.input_file) or
	    (not args.output_file) or
	    (not args.pid_file and not args.pid)):
		parser.print_help()

	create_output(args)

main()
