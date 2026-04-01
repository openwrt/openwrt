#!/usr/bin/env python3

import argparse
import hashlib
import os
import shutil

def create_output(args):
	if (args.pid_file):
		pid_st = os.stat(args.pid_file)
		pid_size = pid_st.st_size

		pid_f = open(args.pid_file, 'rb')
		pid_bytes = pid_f.read(pid_size)
		pid_f.close()
	else:
		pid_bytes = bytes.fromhex(args.pid)

	# Optimization: compute SHA256 in 64K chunks to avoid O(N) memory overhead for large files
	sha256 = hashlib.sha256()
	with open(args.input_file, 'rb') as in_f:
		while True:
			chunk = in_f.read(65536)
			if not chunk:
				break
			sha256.update(chunk)

	# Optimization: stream the file contents with shutil.copyfileobj
	with open(args.output_file, 'wb') as out_f:
		out_f.write(pid_bytes)
		out_f.write(sha256.digest())
		with open(args.input_file, 'rb') as in_f:
			shutil.copyfileobj(in_f, out_f)

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
