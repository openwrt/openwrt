# Gnutella - P2P filesharing
# Pattern quality: good fast
#
# This should match both Gnutella and "Gnutella2" ("Mike's protocol")
# 
# Various clients use this protocol including Mactella, Shareaza,
# GTK-gnutella, Gnucleus, Gnotella, LimeWire, BearShare, and iMesh.
# 
# This is tested with gtk-gnutella and Shareaza.
#
# Please report on how this pattern works for you at
# l7-filter-developers@lists.sf.net .  If you can improve on this
# pattern, please also post to that list. You may subscribe at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

# http://www.gnutella2.com/tiki-index.php?page=UDP%20Transceiver
# http://rfc-gnutella.sf.net/
# http://www.gnutella2.com/tiki-index.php?page=Gnutella2%20Specification
# http://en.wikipedia.org/wiki/Shareaza

gnutella

# The first part matches UDP messages - All start with "GND", then have
# a flag byte which is either \x00, \x01 or \x02, then two sequence bytes
# that can be anything, then a fragment number, which must start at 1.
# The rest matches TCP first client message or first server message (in case 
# we can't see client messages).  Some parts of this are empirical rather than 
# document based.  Assumes version is between 0.0 and 2.9. (usually is
# 0.4 or 0.6).  I'm guessing at many of the user-agents.
# The last bit is emprical and probably only matches Limewire.
^(gnd[\x01\x02]?.?.?\x01|gnutella connect/[012]\.[0-9]\x0d\x0a|get /uri-res/n2r\?urn:sha1:|get /.*user-agent: (gtk-gnutella|bearshare|mactella|gnucleus|gnotella|limewire|imesh)|get /.*content-type: application/x-gnutella-packets|giv [0-9]*:[0-9a-f]*/|queue [0-9a-f]* [1-9][0-9]?[0-9]?\.[1-9][0-9]?[0-9]?\.[1-9][0-9]?[0-9]?\.[1-9][0-9]?[0-9]?:[1-9][0-9]?[0-9]?[0-9]?|gnutella.*content-type: application/x-gnutella|..................lime)

# Needlessly precise, at the expense of time
#^(gnd[\x01\x02]?.?.?\x01|gnutella connect/[012]\.[0-9]\x0d\x0a|get /uri-res/n2r\?urn:sha1:|get /[\x09-\x0d -~]*user-agent: (gtk-gnutella|bearshare|mactella|gnucleus|gnotella|limewire|imesh)|get /[\x09-\x0d -~]*content-type: application/x-gnutella-packets|giv [0-9]*:[0-9a-f]*/|queue [0-9a-f]* [1-9][0-9]?[0-9]?\.[1-9][0-9]?[0-9]?\.[1-9][0-9]?[0-9]?\.[1-9][0-9]?[0-9]?:[1-9][0-9]?[0-9]?[0-9]?|gnutella[\x09-\x0d -~]*content-type: application/x-gnutella|..................lime)


