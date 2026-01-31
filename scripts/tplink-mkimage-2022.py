#!/usr/bin/env python3

'''A program for manipulating tplink2022 images.

A tplink2022 is an image format encountered on TP-Link devices around the year
2022. This was seen at least on the EAP610-Outdoor. The format is a container
for a rootfs, and has optional fields for the "software" version. It also
 requires a "support" string that describes the list of compatible devices.

This module is intended for creating such images with an OpenWRT UBI image, but
also supports analysis and extraction of vendor images. Altough tplink2022
images can be signed, this program does not support signing image.

To get an explanation of the commandline arguments, run this program with the
"--help" argument.
'''

import argparse
import hashlib
import os
import pprint
import re
import struct


def decode_header(datafile):
    '''Read the tplink2022 image header anbd decode it into a dictionary'''
    header = {}
    fmt = '>2I'

    datafile.seek(0x1014)
    raw_header = datafile.read(8)
    fields = struct.unpack(fmt, raw_header)

    header['rootfs_size'] = fields[0]
    header['num_items'] = fields[1]
    header['items'] = []

    rootfs = {}
    rootfs['name'] = 'rootfs.ubi'
    rootfs['offset'] = 0
    rootfs['size'] = header['rootfs_size']
    header['items'].append(rootfs)

    for _ in range(header['num_items']):
        entry = datafile.read(0x2c)
        fmt = '>I32s2I'
        fields = struct.unpack(fmt, entry)

        section = {}
        section['name'] = fields[1].decode("utf-8").rstrip('\0')
        section['type'] = fields[0]
        section['offset'] = fields[2]
        section['size'] = fields[3]
        header['items'].append(section)
    return header

def extract(datafile):
    '''Extract the sections of the tplink2022 image to separate files'''
    header = decode_header(datafile)

    pretty = pprint.PrettyPrinter(indent=4, sort_dicts=False)
    pretty.pprint(header)

    for section in header['items']:
        datafile.seek(0x1814 + section['offset'])
        section_contents = datafile.read(section['size'])

        with open(f"{section['name']}.bin", 'wb') as section_file:
            section_file.write(section_contents)

    with open('leftover.bin', 'wb') as extras_file:
        extras_file.write(datafile.read())

def get_section_contents(section):
    '''I don't remember what this does. It's been a year since I wrote this'''
    if section.get('data'):
        data = section['data']
    elif section.get('file'):
        with open(section['file'], 'rb') as section_file:
            data = section_file.read()
    else:
        data = bytes()

    if section['size'] != len(data):
        raise ValueError("Wrong section size", len(data))

    return data

def write_image(output_image, header):
    '''Write a tplink2022 image with the contents in the "header" dictionary'''
    with open(output_image, 'w+b') as out_file:
        # header MD5
        salt = [ 0x7a, 0x2b, 0x15, 0xed,
             0x9b, 0x98, 0x59, 0x6d,
             0xe5, 0x04, 0xab, 0x44,
             0xac, 0x2a, 0x9f, 0x4e
        ]

        out_file.seek(4)
        out_file.write(bytes(salt))

        # unknown section
        out_file.write(bytes([0xff] * 0x1000))

        # Table of contents
        raw_header = struct.pack('>2I', header['rootfs_size'],
                        header['num_items'])
        out_file.write(raw_header)

        for section in header['items']:
            if section['name'] == 'rootfs.ubi':
                continue

            hdr = struct.pack('>I32s2I',
                section.get('type', 0),
                section['name'].encode('utf-8'),
                section['offset'],
                section['size']
            )

            out_file.write(hdr)

        for section in header['items']:
            out_file.seek(0x1814 + section['offset'])
            out_file.write(get_section_contents(section))

        size = out_file.tell()

        out_file.seek(4)
        md5_sum = hashlib.md5(out_file.read())

        out_file.seek(0)
        out_file.write(struct.pack('>I16s', size, md5_sum.digest()))

def encode_soft_verson():
    '''Not sure of the meaning of version. Also doesn't appear to be needed.'''
    return struct.pack('>4B1I2I', 0xff, 1, 0 ,0, 0x2020202, 30000, 1)

def create_image(output_image, root, support):
    '''Create an image with a ubi "root" and a "support" string.'''
    header = {}

    header['rootfs_size'] = os.path.getsize(root)
    header['items'] = []

    rootfs = {}
    rootfs['name'] = 'rootfs.ubi'
    rootfs['file'] = root
    rootfs['offset'] = 0
    rootfs['size'] = header['rootfs_size']
    header['items'].append(rootfs)

    support_list = {}
    support_list['name'] = 'support-list'
    support_list['data'] = re.sub("\\\\r\\\\n ?", "\r\n", support).encode("utf-8")
    support_list['offset'] = header['rootfs_size']
    support_list['size'] = len(support_list['data'])
    header['items'].append(support_list)

    sw_version = {}
    sw_version['name'] = 'soft-version'
    sw_version['type'] = 1
    sw_version['data'] = encode_soft_verson()
    sw_version['offset'] = support_list['offset'] + support_list['size']
    sw_version['size'] = len(sw_version['data'])
    header['items'].append(sw_version)

    header['num_items'] = len(header['items']) - 1
    write_image(output_image, header)

def main(args):
    '''We support image analysis,extraction, and creation'''
    if args.extract:
        with open(args.image, 'rb') as image:
            extract(image)
    elif args.create:
        if not args.rootfs or not args.support:
            raise ValueError('To create an image, specify rootfs and support list')
        create_image(args.image, args.rootfs, args.support)
    else:
        with open(args.image, 'rb') as image:
            header = decode_header(image)

            pretty = pprint.PrettyPrinter(indent=4, sort_dicts=False)
            pretty.pprint(header)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='EAP extractor')
    parser.add_argument('--info', action='store_true')
    parser.add_argument('--extract', action='store_true')
    parser.add_argument('--create', action='store_true')
    parser.add_argument('image', type=str,
                    help='Name of image to create or decode')
    parser.add_argument('--rootfs', type=str,
                    help='When creating an EAP image, UBI image with rootfs and kernel')
    parser.add_argument('--support', type=str,
                    help='String for the "support-list" section')

    main(parser.parse_args())
