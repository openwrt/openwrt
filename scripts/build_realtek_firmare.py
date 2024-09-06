#!/usr/bin/python3
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2024 Markus Stockhausen <markus.stockhausen@gmx.de>
#
# The PHYs of Realtek switches need well defined startup patching. The GPL
# source drops contain several magic sequences that cannot be explained in
# detail. Writing that magic data into some structure into a header and
# reference that inside the code would require to include all sequences
# for all 4 platforms (RTL838x, RTL839x, RTL930x and RTL931x) into the
# kernel and a lot of references and individual coding.
#
# Define a generic firmware data format that can be consumed by the driver.
# See rtph_patch_phy() and rtph_load_fw_sequence() in rtl83xx-phy.c with
# the next patch. This script takes a standalone C header file (not used
# in the normal kernel sources) and generates a binary firmware that can
# be loaded inside the drivers.
#
# The input file is a classic C header file with multiple structures of
# the type 'rtph_fw_seq'. See rtl83xx-phy.h structures rtph_fw_seq and
# rtph_fw_data for more details of the contained elements. E.g.
#
# struct rtph_fw_seq rtph_fw_8218b_6276A_8390_perchip = {
#   0 ,  0,
#   {
#   { 0, 0x1e, 0x0008, 0xffff },
#   { 0, 0x1f, 0x0405, 0xffff },
#   ...
#   {-1, 0, 0, 0 },
#   }
# };
#
# A firmware output file contains a head a directory and at the end the
# raw patch data. See structure rtph_fw_head and rtph_fw_dir in the
# header rtl83xx-phy.h.
#
# head
#   - magic (4 bytes)
#   - CRC checksum of the following data (4 bytes)
# directory
#   - size of directory (4 bytes)
#   - n directory entries consisting of
#     - hash value of the structure name (4 bytes)
#     - offset of patch data for this directory entry (4 bytes)
# patch data
# - patch data (2 bytes each)

import argparse
import binascii
import re

HEADER = 0x83009300
DATASIZE = 2

# remove_comments() removes /* */ or // c style comments and is quotation
# mark aware.
def remove_comments(string):
    pattern = r"(\".*?\"|\'.*?\')|(/\*.*?\*/|//[^\r\n]*$)"
    regex = re.compile(pattern, re.MULTILINE|re.DOTALL)
    def _replacer(match):
        if match.group(2) is not None:
            return ""
        else:
            return match.group(1)
    return regex.sub(_replacer, string)

# read_header_file() reads the C header input. Sanitizing might need
# some improvement but it works quite well. The function returns a list
# of string structures that resemble the input file
def read_header_file(indata):
    indata = remove_comments(indata)
    # Remove C declarations (brackets, semicolon, ...)
    indata = re.sub('[\\{\\}\\,\\;]', ' ', indata)
    # Cleanup multiple whitespaces
    indata = ' '.join(indata.split())
    # Split into list
    indata = indata.split('struct rtph_fw_seq')
    # Filter structures
    return [d for d in indata if "=" in d]

# convert_intermediate_data() takes the sanitized input and converts
# it into the target structure. Strings are transformed into integers
# and the sequence ids (hash of sequence name) are generated.
def convert_intermediate_data(indata):
    blocks = []
    for d in indata:
        kvp = d.split("=");
        name = kvp[0].strip().upper()
        vals = kvp[1].split()
        # first element of block is the sequence name
        block = [name]
        # second element of block is the sequence id (hash of name)
        block.append(binascii.crc32(str.encode(name)) % (1<<32))
        # third element of block is the patch data
        block.append(list(map(lambda x:int(x,0) % (1<<16),vals)))
        blocks.append(block)
    return blocks

# create_raw_data() takes the intermediate data and produces the binary
# output data. As the Realtek chips are MIPS big endian the output data
# is generated directly in the target endian order.
def create_raw_data(blocks):
    # Create directory size
    dirdata = bytearray(len(blocks).to_bytes(4, byteorder = 'big'))
    pos = 12 + 8 * len(blocks)
    # Create directory entries with sequence id and offset
    for b in blocks:
        dirdata += bytearray(b[1].to_bytes(4, byteorder = 'big'))
        dirdata += bytearray(pos.to_bytes(4, byteorder = 'big'))
        pos += len(b[2]) * DATASIZE
    # Create patch data
    patchdata = bytearray()
    for b in blocks:
        for v in b[2]:
            patchdata += bytearray(v.to_bytes(DATASIZE, byteorder = 'big'))
    # Create header consisting of identifier and CRC sum
    crc = binascii.crc32(dirdata + patchdata) % (1<<32)
    hdrdata = bytearray(HEADER.to_bytes(4, byteorder = 'big'))
    hdrdata += bytearray(crc.to_bytes(4, byteorder = 'big'))
    return hdrdata + dirdata + patchdata

# show_result() gives helpful output of what to do with the file.
# Especially the automatic generated sequence ids must be known so
# that the driver can identify the required patch sequence.
def show_result(blocks):
    print("Created Realtek firmware file. Instructions for driver:\n")
    for b in blocks:
        print("#define " + b[0] + " " + ("0x%0.8X" % b[1]).lower())
    print("")
    for b in blocks:
        print("rtph_patch_phy(phydev, ..., " + b[0] + ");")

parser = argparse.ArgumentParser(description='Create Realtek firmware.')
parser.add_argument('headerfile', type=argparse.FileType('r'))
parser.add_argument('firmwarefile', type=argparse.FileType('wb'))
args = parser.parse_args()

header = read_header_file(args.headerfile.read())
intermediate = convert_intermediate_data(header)
firmware = create_raw_data(intermediate)
args.firmwarefile.write(firmware)
show_result(intermediate)
