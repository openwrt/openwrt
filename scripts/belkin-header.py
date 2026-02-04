#!/usr/bin/python3
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2024 OpenWrt.org
#
# ./belkin-header.py <ImageFileIn> <ImageFileOut> <BelkinHeader> <BelkinModel>
#
# This script adds an image header for Belkin devices. As of now only Realtek
# based switches of the Linksys LGS3xxC/LGS3xxMPC series are known to use this
# format. It resembles a U-Boot legacy format image header, all data in network
# byte order (aka natural aka big endian).
#
# Known values for BelkinHeader are
#
# 0x07800001 : RTL838x based switch
# 0x07600001 : RTL93xx based switch
#
# Known values for BelkinModel are
#
# BKS-RTL83xx : RTL838x based switch
# BKS-RTL93xx : RTL93xx based switch

import argparse
import os
import zlib
import array
import sys
import time

VERSION1 = 1
VERSION2 = 1
VERSION3 = 2
VERSION4 = 2
COMPANY = "belkin"
MODULE = "IMG"

def xcrc32(buf):
    return (0xffffffff - zlib.crc32(buf, 0xffffffff)).to_bytes(4, byteorder='big')

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

def create_header(buf, belkin_header, belkin_model):
    head = bytearray(32)

    head[0:4] = int(belkin_header, 0).to_bytes(4, 'big')
    head[8:12] = int(time.time()).to_bytes(4, 'big')
    head[12:16] = len(buf).to_bytes(4, byteorder='big')
    head[24:28] = xcrc32(buf)
    head[28:29] = VERSION1.to_bytes(1, byteorder='big')
    head[29:30] = VERSION2.to_bytes(1, byteorder='big')
    head[30:31] = VERSION3.to_bytes(1, byteorder='big')
    head[31:32] = VERSION4.to_bytes(1, byteorder='big')
    head[16:16 + len(COMPANY)] = bytes(COMPANY,'ascii')

    mod = MODULE + "-{:1d}.{:02d}.{:02d}.{:02d}".format(VERSION1, VERSION2, VERSION3, VERSION4)
    head.extend(bytes(mod,'ascii'))
    head.append(0x00)
    head.extend(encode_model(belkin_model))
    head.extend(bytes([0x00] * (64 - len(head))))

    head[4:8] = xcrc32(head)

    return head

parser = argparse.ArgumentParser(description='Generate Belkin header.')
parser.add_argument('source', type=argparse.FileType('r+b'))
parser.add_argument('dest', type=argparse.FileType('wb'))
parser.add_argument('belkin_header')
parser.add_argument('belkin_model')
args = parser.parse_args()

buf = bytearray(args.source.read())
head = create_header(buf, args.belkin_header, args.belkin_model)
args.dest.write(head)
args.dest.write(buf)
