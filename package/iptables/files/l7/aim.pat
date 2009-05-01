# AIM - AOL instant messenger (OSCAR and TOC)
# Pattern attributes: good slow notsofast
# Protocol groups: chat proprietary
# Wiki: http://www.protocolinfo.org/wiki/AIM
# Copyright (C) 2008 Matthew Strait, Ethan Sommer; See ../LICENSE
#
# Usually runs on port 5190
#
# This may also match ICQ traffic.
# 
# This pattern has been tested and is believed to work well.

aim
# See http://gridley.res.carleton.edu/~straitm/final (and various other places)
# The first bit matches OSCAR signon and data commands, but not sure what
# \x03\x0b matches, but it works apparently.
# The next three bits match various parts of the TOC signon process.
# The third one is the magic number "*", then 0x01 for "signon", then up to four
# bytes ("up to" because l7-filter strips out nulls) which contain a sequence
# number (2 bytes) the data length (2 more) and 3 nulls (which don't count), 
# then 0x01 for the version number (not sure if there ever has been another 
# version)
# The fourth one is a command string, followed by some stuff, then the
# beginning of the "roasted" password

# This pattern is too slow!

^(\*[\x01\x02].*\x03\x0b|\*\x01.?.?.?.?\x01)|flapon|toc_signon.*0x
