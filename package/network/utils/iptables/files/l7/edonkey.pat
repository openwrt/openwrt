# eDonkey2000 - P2P filesharing - http://edonkey2000.com and others
# Pattern attributes: good veryfast fast overmatch
# Protocol groups: p2p
# Wiki: http://www.protocolinfo.org/wiki/EDonkey
# Copyright (C) 2008 Matthew Strait, Ethan Sommer; See ../LICENSE
#
# Tested recently (April/May 2006) with eMule 0.47a and eDonkey2000 1.4
# and a long time ago with something else. 
# 
# In addition to matching what you might expect, this matches much of
# what eMule does when you tell it to only connect to the KAD network. 
# I don't quite know what to make of this.

# Thanks to Matt Skidmore <fox AT woozle.org>

edonkey

# http://gd.tuwien.ac.at/opsys/linux/sf/p/pdonkey/eDonkey-protocol-0.6
#
# In addition to \xe3, \xc5 and \xd4, I see a lot of \xe5.
# As of April 2006, I also see some \xe4.
#
# God this is a mess.  What an irritating protocol.  
# This will match about 2% of streams with random data in them!
# (But fortunately much fewer than 2% of streams that are other protocols.
# You can test this with the data in ../testing/)

^[\xc5\xd4\xe3-\xe5].?.?.?.?([\x01\x02\x05\x14\x15\x16\x18\x19\x1a\x1b\x1c\x20\x21\x32\x33\x34\x35\x36\x38\x40\x41\x42\x43\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58[\x60\x81\x82\x90\x91\x93\x96\x97\x98\x99\x9a\x9b\x9c\x9e\xa0\xa1\xa2\xa3\xa4]|\x59................?[ -~]|\x96....$)

# matches everything and too much 
# ^(\xe3|\xc5|\xd4)

# ipp2p essentially uses "\xe3....\x47", which doesn't seem at all right to me.

# bandwidtharbitrator uses 
# e0.*@.*6[a-z].*p$|e0.*@.*[a-z]6[a-z].*p0$|e.*@.*[0-9]6.*p$|emule|edonkey
# no comments to explain what all the mush is, of course...
