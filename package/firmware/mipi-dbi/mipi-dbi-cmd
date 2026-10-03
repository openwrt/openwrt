#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# SPDX-License-Identifier: CC0-1.0
#
# Written in 2022 by Noralf Trønnes <noralf@tronnes.org>
#
# To the extent possible under law, the author(s) have dedicated all copyright and related and
# neighboring rights to this software to the public domain worldwide. This software is
# distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

from __future__ import print_function
import argparse
import sys


def hexstr(buf):
    return ' '.join('{:02x}'.format(x) for x in buf)


def parse_binary(buf):
    if len(buf) < 18:
        raise ValueError('file too short, len=%d' % len(buf))
    if buf[:15] != b'MIPI DBI\x00\x00\x00\x00\x00\x00\x00':
        raise ValueError('wrong magic: %s' % hexstr(buf[:15]))
    if buf[15] != 1:
        raise ValueError('wrong version: %d' % (buf[15]))

    result = ''
    cmds = buf[16:]
    i = 0
    while i < len(cmds):
        try:
            pos = i
            cmd = cmds[i]
            i += 1
            num_params = cmds[i]
            i += 1
            params = cmds[i:i+num_params]
            if len(params) != num_params:
                raise IndexError()

            if cmd == 0x00 and num_params == 1:
                s = 'delay %d\n' % params[0]
            else:
                s = 'command 0x%02x' % cmd
                if params:
                    s += ' '
                    s += ' '.join('0x{:02x}'.format(x) for x in params)
                s += '\n'
        except IndexError:
            raise ValueError('malformed file at offset %d: %s' % (pos + 16, hexstr(cmds[pos:])))
        i += num_params
        result += s

    return result


def print_file(fn):
    with open(args.fw_file, mode='rb') as f:
        fw = f.read()
    s = parse_binary(bytearray(fw))
    print(s)


def parse_values(parts):
    vals = []
    for x in parts:
        try:
            val = int(x, 0)
        except ValueError:
            raise ValueError('not a number: %s' % x)
        if val < 0 or val > 255:
            raise ValueError('value out of bounds: %s (%d)' % (hex(val), val))
        vals.append(val)
    return vals


def make_file(fw_file, input_file):
    with open(input_file, mode='r') as f:
        lines = f.readlines()

    buf = bytearray()
    buf.extend(b'MIPI DBI\x00\x00\x00\x00\x00\x00\x00') # magic
    buf.append(1) # version

    for idx, line in enumerate(lines):
        # strip off comments and skip empty lines
        comment_idx = line.find('#')
        if comment_idx >= 0:
            line = line[:comment_idx]
        line = line.strip()
        if not line:
            continue

        try:
            parts = line.split()
            if parts[0] == 'command':
                vals = parse_values(parts[1:])
                buf.append(vals[0])
                num_params = len(vals) - 1
                buf.append(num_params)
                if num_params:
                    buf.extend(vals[1:])
            elif parts[0] == 'delay':
                vals = parse_values(parts[1:])
                if len(vals) != 1:
                    raise ValueError('delay takes exactly one argument')
                buf.append(0x00)
                buf.append(1)
                buf.append(vals[0])
            else:
                raise ValueError('unknown keyword: %s' % parts[0])
        except ValueError as e:
            raise ValueError('error: %s\nline %d: %s' % (e, idx + 1, line))

    with open(fw_file, mode='wb') as f:
        f.write(buf)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='MIPI DBI Linux driver firmware tool')
    parser.add_argument('fw_file', help='firmware binary file')
    parser.add_argument('input', nargs='?', help='Input commands file')
    parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')
    parser.add_argument('-d', '--debug', action='store_true', help='Print exception callstack')
    args = parser.parse_args()

    try:
        if args.input:
                make_file(args.fw_file, args.input)
                if args.verbose:
                    print_file(args.fw_file)
        else:
            print_file(args.fw_file)
    except Exception as e:
        if args.debug:
            raise
        print(e, file=sys.stderr)
        sys.exit(1)
