#!/usr/bin/python3
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2024 OpenWrt.org
#
# ./linksys-header.py <ImageFileIn> <ImageFileOut> <ModelID>
#
# This script adds an image header for Linksys switches of the LGS3xxC/LGS3xxMPC
# series. It resembles a U-Boot legacy format image header, all data in network
# byte order (aka natural aka bigendian). Possible values for ModelID are
# 
# "BKS-RTL83xx" : smaller models with RTL83xx SOC
# "BKS-RTL93xx" : larger models with RTL93xx SOC

import argparse
import os
import zlib
import array

MAGIC83 = 0x07800001
MAGIC93 = 0x07600001
VERSION1 = 1
VERSION2 = 1
VERSION3 = 2
VERSION4 = 2
COMPANY = "belkin"
MODULE = "IMG"

def xcrc32(buf):
    return (0xffffffff - zlib.crc32(buf, 0xffffffff)).to_bytes(4, 'big')

def encode_model(model):
    map = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-"
    code = bytearray()

    code.append(map.index(model[:1]))
    model = model[1:]
    model = model + " " * (3 - len(model) % 4)

    while model != "":
        b1 = map.index(model[0:1])
        b2 = map.index(model[1:2])
        b3 = map.index(model[2:3])
        b4 = map.index(model[3:4])
        model = model[4:]

        code.append(b1 << 2 | b2 >> 4)
        code.append((b2 & 0xf) << 4 | b3 >> 2)
        code.append((b3 & 0x3) << 6 | b4)

    return code

def create_header(buf, model):
    head = bytearray(32)

    if model == "BKS-RTL83xx":
        head[0:4] = MAGIC83.to_bytes(4, 'big')
    else:
        head[0:4] = MAGIC93.to_bytes(4, 'big')

    head[8:12] = (0x627e134b).to_bytes(4, 'big')
    head[12:16] = len(buf).to_bytes(4, 'big')
    head[24:28] = xcrc32(buf)
    head[28:29] = VERSION1.to_bytes(1)
    head[29:30] = VERSION2.to_bytes(1)
    head[30:31] = VERSION3.to_bytes(1)
    head[31:32] = VERSION4.to_bytes(1)
    head[16:16 + len(COMPANY)] = bytes(COMPANY,'ascii')

    mod = MODULE + "-{:1d}.{:02d}.{:02d}.{:02d}".format(VERSION1, VERSION2, VERSION3, VERSION4)
    head.extend(bytes(mod,'ascii'))
    head.append(0x00)
    head.extend(encode_model(model))
    head.extend(bytes([0x00] * (64 - len(head))))

    head[4:8] = xcrc32(head)

    return head

parser = argparse.ArgumentParser(description='Generate Linksys LGS3xxC/LGS3xxMPC header.')
parser.add_argument('source', type=argparse.FileType('r+b'))
parser.add_argument('dest', type=argparse.FileType('wb'))
parser.add_argument('model')
args = parser.parse_args()

buf = bytearray(args.source.read())
head = create_header(buf, args.model)
args.dest.write(head)
args.dest.write(buf)
