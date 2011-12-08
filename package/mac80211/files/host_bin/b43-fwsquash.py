#!/usr/bin/env python
#
# b43 firmware file squasher
# Removes unnecessary firmware files
#
# Copyright (c) 2009 Michael Buesch <mb@bu3sch.de>
#
# Licensed under the GNU/GPL version 2 or (at your option) any later version.
#

import sys
import os

def usage():
	print("Usage: %s PHYTYPES COREREVS /path/to/extracted/firmware" % sys.argv[0])
	print("")
	print("PHYTYPES is a comma separated list of:")
	print("A         => A-PHY")
	print("AG        => Dual A-PHY G-PHY")
	print("G         => G-PHY")
	print("LP        => LP-PHY")
	print("N         => N-PHY")
	print("")
	print("COREREVS is a comma separated list of core revision numbers.")

if len(sys.argv) != 4:
	usage()
	sys.exit(1)

phytypes = sys.argv[1]
corerevs = sys.argv[2]
fwpath = sys.argv[3]

phytypes = phytypes.split(',')
try:
	corerevs = map(lambda r: int(r), corerevs.split(','))
except ValueError:
	print("ERROR: \"%s\" is not a valid COREREVS string\n" % corerevs)
	usage()
	sys.exit(1)


fwfiles = os.listdir(fwpath)
fwfiles = filter(lambda str: str.endswith(".fw"), fwfiles)
if not fwfiles:
	print("ERROR: No firmware files found in %s" % fwpath)
	sys.exit(1)

required_fwfiles = []

def revs_match(revs_a, revs_b):
	for rev in revs_a:
		if rev in revs_b:
			return True
	return False

def phytypes_match(types_a, types_b):
	for type in types_a:
		type = type.strip().upper()
		if type in types_b:
			return True
	return False

revmapping = {
	"ucode2.fw"		: (2,3,),
	"ucode4.fw"		: (4,),
	"ucode5.fw"		: (5,6,7,8,9,10,),
	"ucode11.fw"		: (11,12,),
	"ucode13.fw"		: (13,),
	"ucode14.fw"		: (14,),
	"ucode15.fw"		: (15,),
	"ucode16_mimo.fw"	: (16,),
	"ucode24_mimo.fw"	: (24,),
	"ucode29_mimo.fw"	: (29,),
	"pcm4.fw"		: (1,2,3,4,),
	"pcm5.fw"		: (5,6,7,8,9,10,),
}

initvalmapping = {
	"a0g1initvals5.fw"	: ( (5,6,7,8,9,10,),	("AG",), ),
	"a0g0initvals5.fw"	: ( (5,6,7,8,9,10,),	("A", "AG",), ),
	"b0g0initvals2.fw"	: ( (2,4,),		("G",), ),
	"b0g0initvals5.fw"	: ( (5,6,7,8,9,10,),	("G",), ),
	"b0g0initvals13.fw"	: ( (13,),		("G",), ),
	"n0initvals11.fw"	: ( (11,12,),		("N",), ),
	"n0initvals16.fw"	: ( (16,),		("N",), ),
	"lp0initvals13.fw"	: ( (13,),		("LP",), ),
	"lp0initvals14.fw"	: ( (14,),		("LP",), ),
	"lp0initvals15.fw"	: ( (15,),		("LP",), ),
	"a0g1bsinitvals5.fw"	: ( (5,6,7,8,9,10,),	("AG",), ),
	"a0g0bsinitvals5.fw"	: ( (5,6,7,8,9,10,),	("A", "AG"), ),
	"b0g0bsinitvals5.fw"	: ( (5,6,7,8,9,10,),	("G",), ),
	"n0bsinitvals11.fw"	: ( (11,12,),		("N",), ),
	"n0bsinitvals16.fw"	: ( (16,),		("N",), ),
	"lp0bsinitvals13.fw"	: ( (13,),		("LP",), ),
	"lp0bsinitvals14.fw"	: ( (14,),		("LP",), ),
	"lp0bsinitvals15.fw"	: ( (15,),		("LP",), ),
	"lcn0initvals24.fw"	: ( (24,),		("LNC",), ),
	"ht0initvals29.fw"	: ( (29,),		("HT",), ),
}

for f in fwfiles:
	if f in revmapping:
		if revs_match(corerevs, revmapping[f]):
			required_fwfiles += [f]
		continue
	if f in initvalmapping:
		if revs_match(corerevs, initvalmapping[f][0]) and\
		   phytypes_match(phytypes, initvalmapping[f][1]):
			required_fwfiles += [f]
		continue
	print("WARNING: Firmware file %s not found in the mapping lists" % f)

for f in fwfiles:
	if f not in required_fwfiles:
		print("Deleting %s" % f)
		os.unlink(fwpath + '/' + f)

