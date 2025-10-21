#!/usr/bin/env python3

'''A program for manipulating tplink2023 images.

A tplink2023 is an image format encountered on TP-Link devices around the year
2023. This was seen at least on the BE800V1. The format is a container
for a rootfs, and has optional fields for the "software" version. It also
 requires a "support" string that describes the list of compatible devices.

This module is intended for creating such images with an OpenWRT UBI image, but
also supports analysis and extraction of vendor images. Altough tplink2023
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
    '''Read the tplink2023 image header and decode it into a dictionary'''
    header = {}

    datafile.seek(0x1014)

    header['items'] = []

    while True:
        entry = datafile.read(0x2c)
        fmt = '>32s3I'
        fields = struct.unpack(fmt, entry)

        section = {}
        section['name'] = fields[0].decode("utf-8").rstrip('\0')
        section['offset'] = 0x1014 + fields[1]
        section['size'] = fields[3]
        header['items'].append(section)

        if fields[2] == 0:
            # last section, rootfs follows
            datafile.seek(section['offset'] + section['size'])
            break

        # move to next section
        datafile.seek(0x1014 + fields[2])


    current_offset = datafile.tell()
    datafile.seek(0, os.SEEK_END)
    rootfs_size = datafile.tell() - current_offset
    rootfs = {}
    rootfs['name'] = 'rootfs.ubi'
    rootfs['offset'] = current_offset
    rootfs['size'] = rootfs_size
    header['rootfs'] = rootfs

    return header

def extract(datafile):
    '''Extract the sections of the tplink2023 image to separate files'''
    header = decode_header(datafile)

    pretty = pprint.PrettyPrinter(indent=4, sort_dicts=False)
    pretty.pprint(header)

    for section in header['items']:
        datafile.seek(section['offset'])
        section_contents = datafile.read(section['size'])

        with open(f"{section['name']}.bin", 'wb') as section_file:
            section_file.write(section_contents)

    rootfs = header['rootfs']
    datafile.seek(rootfs['offset'])
    rootfs_contents = datafile.read(rootfs['size'])

    with open(f"{rootfs['name']}.bin", 'wb') as section_file:
        section_file.write(rootfs_contents)

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
    '''Write a tplink2023 image with the contents in the "header" dictionary'''
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

        # partition table + inline data
        start_addr = 0x2c # start of data relative to start of partition table
        for section in header['items']:
            # offset to next partition table entry relative to start of partition table
            # in last partition entry this value is 0 and rootfs data follows immediately
            # there is no entry for rootfs partition
            next_offset = start_addr + section['size']
            if section['last']:
                next_offset = 0

            hdr = struct.pack('>32s3I',
                section['name'].encode('utf-8'),
                start_addr,
                next_offset,
                section['size']
            )

            # increment start_addr to point to next section
            start_addr += section['size'] + 0x2c

            out_file.write(hdr)
            out_file.write(get_section_contents(section))

        # rootfs data at the end
        out_file.write(get_section_contents(header['rootfs']))

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

    header['items'] = []

    rootfs = {}
    rootfs['name'] = 'rootfs.ubi'
    rootfs['file'] = root
    rootfs['size'] = os.path.getsize(root)
    header["rootfs"] = rootfs

    support_list = {}
    support_list['name'] = 'support-list'
    support_list['data'] = re.sub("\\\\r\\\\n ?", "\r\n", support).encode("utf-8")
    support_list['size'] = len(support_list['data'])
    support_list['last'] = 0
    header['items'].append(support_list)

    sw_version = {}
    sw_version['name'] = 'soft-version'
    sw_version['data'] = encode_soft_verson()
    sw_version['size'] = len(sw_version['data'])
    sw_version['last'] = 1
    header['items'].append(sw_version)

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
