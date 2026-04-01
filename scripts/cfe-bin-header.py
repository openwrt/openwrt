#!/usr/bin/env python3

import argparse
import os
import struct
import shutil

def auto_int(x):
	return int(x, 0)

def create_header(args, size):
	header = struct.pack('>III', args.entry_addr, args.load_addr, size)
	return header

def create_output(args):
	in_st = os.stat(args.input_file)
	in_size = in_st.st_size

	header = create_header(args, in_size)
	print(header)

	# Optimization: stream file contents with shutil.copyfileobj to avoid loading entire file in memory
	with open(args.output_file, 'wb') as out_f:
		out_f.write(header)
		with open(args.input_file, 'rb') as in_f:
			shutil.copyfileobj(in_f, out_f)

def main():
	global args

	parser = argparse.ArgumentParser(description='')

	parser.add_argument('--entry-addr',
		dest='entry_addr',
		action='store',
		type=auto_int,
		help='Entry Address')

	parser.add_argument('--input-file',
		dest='input_file',
		action='store',
		type=str,
		help='Input file')

	parser.add_argument('--load-addr',
		dest='load_addr',
		action='store',
		type=auto_int,
		help='Load Address')

	parser.add_argument('--output-file',
		dest='output_file',
		action='store',
		type=str,
		help='Output file')

	args = parser.parse_args()

	if (not args.input_file) or (not args.output_file):
		parser.print_help()

	if not args.entry_addr:
		args.entry_addr = 0x80010000

	if not args.load_addr:
		args.load_addr = 0x80010000

	create_output(args)

main()
