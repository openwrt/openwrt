# eDonkey2000 - P2P filesharing - http://edonkey2000.com
# Pattern quality: good veryfast overmatch
#
# Please post to l7-filter-developers@lists.sf.net as to whether this pattern 
# works for you or not.  If you believe it could be improved please post your 
# suggestions to that list as well. You may subscribe to this list at 
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

# Thanks to Matt Skidmore <fox AT woozle.org>

edonkey

# http://gd.tuwien.ac.at/opsys/linux/sf/p/pdonkey/eDonkey-protocol-0.6
#
# In addition to \xe3, \xc5 and \xd4, I see a lot of \xe5
#
# God this is a mess.  What an irritating protocol.  
# This will match about 1% of streams with random data in them!

^[\xe3\xc5\xe5\xd4](....)?([\x01\x02\x05\x14\x15\x16\x18\x19\x1a\x1b\x1c\x20\x21\x32\x33\x34\x35\x36\x38\x40\x41\x42\x43\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x5b\x5c\x60\x81\x82\x90\x91\x93\x96\x97\x98\x99\x9a\x9b\x9c\x9e\xa0\xa1\xa2\xa3\xa4]|\x59................?[ -~]|\x96....$)

# matches everything and too much 
# ^(\xe3|\xc5|\xd4)

# ipp2p essentially uses "\xe3....\x47", which doesn't seem at all right to me.

# bandwidtharbitrator uses 
# e0.*@.*6[a-z].*p$|e0.*@.*[a-z]6[a-z].*p0$|e.*@.*[0-9]6.*p$|emule|edonkey
# no comments to explain what all the mush is, of course...
