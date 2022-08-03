#!/usr/bin/env python3
#
# certdata2pem.py - splits certdata.txt into multiple files
#
# Copyright (C) 2009 Philipp Kern <pkern@debian.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,
# USA.
#
# Openwrt changelog:
# - use env for shebang
# - allow skipping cert expiration check
# - use command line args for file inputs
#
# Usage: [python3] $(TOPDIR)/scripts/certdata2pem.py certdata.txt blacklist.txt

import base64
import datetime
import os.path
import re
import sys
import textwrap
import io

try:
    from cryptography import x509
except:
    print('Cryptography module failed to import, skipping expiration check')

objects = []

# Dirty file parser.
in_data, in_multiline, in_obj = False, False, False
field, type, value, obj = None, None, None, dict()

# Python 3 will not let us decode non-ascii characters if we
# have not specified an encoding, but Python 2's open does not
# have an option to set the encoding. Python 3's open is io.open
# and io.open has been backported to Python 2.6 and 2.7, so use io.open.
for line in io.open(sys.argv[1], 'rt', encoding='utf8'):
    # Ignore the file header.
    if not in_data:
        if line.startswith('BEGINDATA'):
            in_data = True
        continue
    # Ignore comment lines.
    if line.startswith('#'):
        continue
    # Empty lines are significant if we are inside an object.
    if in_obj and len(line.strip()) == 0:
        objects.append(obj)
        obj = dict()
        in_obj = False
        continue
    if len(line.strip()) == 0:
        continue
    if in_multiline:
        if not line.startswith('END'):
            if type == 'MULTILINE_OCTAL':
                line = line.strip()
                for i in re.finditer(r'\\([0-3][0-7][0-7])', line):
                    value.append(int(i.group(1), 8))
            else:
                value += line
            continue
        obj[field] = value
        in_multiline = False
        continue
    if line.startswith('CKA_CLASS'):
        in_obj = True
    line_parts = line.strip().split(' ', 2)
    if len(line_parts) > 2:
        field, type = line_parts[0:2]
        value = ' '.join(line_parts[2:])
    elif len(line_parts) == 2:
        field, type = line_parts
        value = None
    else:
        raise NotImplementedError('line_parts < 2 not supported.')
    if type == 'MULTILINE_OCTAL':
        in_multiline = True
        value = bytearray()
        continue
    obj[field] = value
if len(obj) > 0:
    objects.append(obj)

# Read blacklist.
blacklist = []
if os.path.exists(sys.argv[2]):
    for line in open(sys.argv[2], 'r'):
        line = line.strip()
        if line.startswith('#') or len(line) == 0:
            continue
        item = line.split('#', 1)[0].strip()
        blacklist.append(item)

# Build up trust database.
trust = dict()
for obj in objects:
    if obj['CKA_CLASS'] != 'CKO_NSS_TRUST':
        continue
    if obj['CKA_LABEL'] in blacklist:
        print("Certificate %s blacklisted, ignoring." % obj['CKA_LABEL'])
    elif obj['CKA_TRUST_SERVER_AUTH'] == 'CKT_NSS_TRUSTED_DELEGATOR':
        trust[obj['CKA_LABEL']] = True
    elif obj['CKA_TRUST_SERVER_AUTH'] == 'CKT_NSS_NOT_TRUSTED':
        print('!'*74)
        print("UNTRUSTED BUT NOT BLACKLISTED CERTIFICATE FOUND: %s" % obj['CKA_LABEL'])
        print('!'*74)
    else:
        print("Ignoring certificate %s.  SAUTH=%s, EPROT=%s" % \
              (obj['CKA_LABEL'], obj['CKA_TRUST_SERVER_AUTH'],
               obj['CKA_TRUST_EMAIL_PROTECTION']))

for obj in objects:
    if obj['CKA_CLASS'] == 'CKO_CERTIFICATE':
        if not obj['CKA_LABEL'] in trust or not trust[obj['CKA_LABEL']]:
            continue

        # skipping expiration check if error
        try:
            cert = x509.load_der_x509_certificate(obj['CKA_VALUE'])
            if cert.not_valid_after < datetime.datetime.utcnow():
                print('!'*74)
                print('Trusted but expired certificate found: %s' % obj['CKA_LABEL'])
                print('!'*74)
        except:
            pass

        bname = obj['CKA_LABEL'][1:-1].replace('/', '_')\
                                      .replace(' ', '_')\
                                      .replace('(', '=')\
                                      .replace(')', '=')\
                                      .replace(',', '_')

        # this is the only way to decode the way NSS stores multi-byte UTF-8
        # and we need an escaped string for checking existence of things
        # otherwise we're dependant on the user's current locale.
        if bytes != str:
            # We're in python 3, convert the utf-8 string to a
            # sequence of bytes that represents this utf-8 string
            # then encode the byte-sequence as an escaped string that
            # can be passed to open() and os.path.exists()
            bname = bname.encode('utf-8').decode('unicode_escape').encode('latin-1')
        else:
            # Python 2
            # Convert the unicode string back to its original byte form
            # (contents of files returned by io.open are returned as
            #  unicode strings)
            # then to an escaped string that can be passed to open()
            # and os.path.exists()
            bname = bname.encode('utf-8').decode('string_escape')

        fname = bname + b'.crt'
        if os.path.exists(fname):
            print("Found duplicate certificate name %s, renaming." % bname)
            fname = bname + b'_2.crt'
        f = open(fname, 'w')
        f.write("-----BEGIN CERTIFICATE-----\n")
        encoded = base64.b64encode(obj['CKA_VALUE']).decode('utf-8')
        f.write("\n".join(textwrap.wrap(encoded, 64)))
        f.write("\n-----END CERTIFICATE-----\n")
