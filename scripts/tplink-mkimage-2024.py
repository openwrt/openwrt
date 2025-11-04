#!/usr/bin/env python3
# SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

import argparse
import struct
import os
import hashlib
import zlib
import json
import sys

TAG_SIZE = 512
STATIC_IMAGE_TOKEN = bytes([94, 47, 174, 27, 103, 1, 218, 192, 213, 153, 91, 200, 191, 214, 58, 110])
STATIC_MAGIC_NUM = bytes([85, 170, 85, 170, 241, 226, 211, 196, 229, 166, 106, 94, 76, 61, 46, 31, 170, 85, 170, 85])

def auto_int(x):
	return int(x, 0)

def hex_to_bytes(h, expected_len):
	h = h.strip()
	if h.startswith('0x'):
		h = h[2:]
	b = bytes.fromhex(h)
	if len(b) > expected_len:
		b = b[:expected_len]
	return b.ljust(expected_len, b'\0')

def main():
	parser = argparse.ArgumentParser(description="Tool to make new tp-link firmware version (v3) for raw nand")

	parser.add_argument("-k", "--kernel", default="external/kernel.bin", help="Kernel file to append in firmware")
	parser.add_argument("-r", "--rootfs", default="external/root.squashfs", help="Squash filesystem to append in firmware")
	parser.add_argument("-b", "--bootloader", default="external/tcboot.bin", help="Bootloader file to append in firmware")
	parser.add_argument("-o", "--output", default="firmware.bin", help="Output firmware image file")

	parser.add_argument("-I", "--hardware_id", default="25626d307af0fc3f8a37af7dab0f5e9b", help="Hardware ID Hex string")
	parser.add_argument("-F", "--firmware_id", default="22011719891011778899aabcbcddeeff", help="Firmware ID Hex string")
	parser.add_argument("-O", "--oem_id", default="edfd645ce4f0dee7b7b9c4280c6d063e", help="OEM ID Hex string")

	parser.add_argument("-t", "--tag_magic", type=auto_int, default=0x3000003, help="Tag Magic Version")

	parser.add_argument("-P", "--product_id", type=auto_int, default=0x6054b801, help="Product ID")
	parser.add_argument("-V", "--product_ver", type=auto_int, default=0x60066, help="Product Version")
	parser.add_argument("-H", "--extra_hardware_ver", type=auto_int, default=0, help="Extra Hardware Version")
	parser.add_argument("-S", "--sw_revision", type=auto_int, default=0x55aa0010, help="Software Revision")
	parser.add_argument("-L", "--platform_version", type=auto_int, default=0xa5030000, help="Platform Version")
	parser.add_argument("-X", "--special_version", type=auto_int, default=0x1, help="Special Version")
	parser.add_argument("-K", "--kernel_text_offset", type=auto_int, default=0xc0088000, help="Kernel Text Offset")
	parser.add_argument("-E", "--kernel_entrypoint_offset", type=auto_int, default=0xc0088000, help="Kernel Entry Point Offset")

	parser.add_argument("-a", "--kernel_append", type=auto_int, default=0, help="Total kernel size to append")

	args = parser.parse_args()

	img_tag_data = {
		"BootloaderSize": 0,
		"BootloaderAddress": 0,
		"KernelSize": 0,
		"KernelAddress": 0,
		"RootfsSize": 0,
		"RootfsAddress": 0,
		"TotalSize": 0,
		"BinCrc32": 0
	}

	try:
		with open(args.output, "wb+") as f_out:
			f_out.write(b'\0' * TAG_SIZE)
			current_offset = 0

			if args.bootloader and os.path.exists(args.bootloader):
				size = os.path.getsize(args.bootloader)
				img_tag_data["BootloaderSize"] = size
				img_tag_data["BootloaderAddress"] = current_offset
				current_offset += size

				with open(args.bootloader, "rb") as f_in:
					while chunk := f_in.read(65536):
						f_out.write(chunk)

			if not os.path.exists(args.kernel):
				raise FileNotFoundError(f"Kernel file {args.kernel} not found")

			kernel_size = os.path.getsize(args.kernel)
			img_tag_data["KernelSize"] = kernel_size
			img_tag_data["KernelAddress"] = current_offset
			current_offset += kernel_size

			with open(args.kernel, "rb") as f_in:
				while chunk := f_in.read(65536):
					f_out.write(chunk)

			if args.kernel_append > 0:
				append_size = args.kernel_append - kernel_size
				if append_size > 0:
					img_tag_data["KernelSize"] += append_size
					current_offset += append_size
					zeros_written = 0
					while zeros_written < append_size:
						write_size = min(65536, append_size - zeros_written)
						f_out.write(b'\0' * write_size)
						zeros_written += write_size

			if not os.path.exists(args.rootfs):
				raise FileNotFoundError(f"Rootfs file {args.rootfs} not found")

			rootfs_size = os.path.getsize(args.rootfs)
			img_tag_data["RootfsSize"] = rootfs_size
			img_tag_data["RootfsAddress"] = current_offset
			current_offset += rootfs_size

			with open(args.rootfs, "rb") as f_in:
				while chunk := f_in.read(65536):
					f_out.write(chunk)

			img_tag_data["TotalSize"] = (img_tag_data["KernelSize"] +
																	 img_tag_data["RootfsSize"] +
																	 img_tag_data["BootloaderSize"] + TAG_SIZE)

			f_out.seek(TAG_SIZE)
			crc32_val = 0
			md5_hash = hashlib.md5()

			while chunk := f_out.read(65536):
				crc32_val = zlib.crc32(chunk, crc32_val)
				md5_hash.update(chunk)

			crc32_val = crc32_val & 0xFFFFFFFF
			img_tag_data["BinCrc32"] = crc32_val
			image_valid_token = md5_hash.digest().ljust(20, b'\0')

			imagetag_fmt = "< I 16s 16s 16s I I I 20s 20s I I I I I I I I I I I I I 52s 128s 128s 48s"
			imagetag_data = struct.pack(
				imagetag_fmt,
				args.tag_magic,
				hex_to_bytes(args.hardware_id, 16),
				hex_to_bytes(args.firmware_id, 16),
				hex_to_bytes(args.oem_id, 16),
				args.product_id,
				args.product_ver,
				args.extra_hardware_ver,
				image_valid_token,
				STATIC_MAGIC_NUM,
				args.kernel_text_offset,
				args.kernel_entrypoint_offset,
				img_tag_data["TotalSize"],
				img_tag_data["KernelAddress"],
				img_tag_data["KernelSize"],
				img_tag_data["RootfsAddress"],
				img_tag_data["RootfsSize"],
				img_tag_data["BootloaderAddress"],
				img_tag_data["BootloaderSize"],
				args.sw_revision,
				args.platform_version,
				args.special_version,
				img_tag_data["BinCrc32"],
				b'\0' * 52,  # Reserved1
				b'\0' * 128, # Sig
				b'\0' * 128, # ResSig
				b'\0' * 48   # Reserved2
			)

			f_out.seek(0)
			f_out.write(imagetag_data)

			print(json.dumps(img_tag_data, indent=2))
			print("\nFirmware successfully saved to:", args.output)

	except Exception as e:
		print(f"Error: {e}", file=sys.stderr)
		sys.exit(1)

if __name__ == "__main__":
	main()
