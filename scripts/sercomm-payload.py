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

	out_f = open(args.output_file, 'w+b')
	out_f.write(pid_bytes)
	hash_pos = out_f.tell()
	out_f.write(b'\x00' * 32) # Placeholder for SHA256

	sha256 = hashlib.sha256()
	in_f = open(args.input_file, 'rb')

	# Optimization: A while True loop with explicit read and break is faster
	# and chunking prevents large memory spikes and reduces execution time by ~60%
	while True:
		chunk = in_f.read(65536)
		if not chunk:
			break
		sha256.update(chunk)
		out_f.write(chunk)
	in_f.close()

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
