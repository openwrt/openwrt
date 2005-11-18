# (S)NTP - (Simple) Network Time Protocol - RFCs 1305 and 2030
# Pattern quality: good veryfast overmatch 
#
# This pattern is tested and is believed to work. If this does not work
# for you, or you believe it could be improved, please post to
# l7-filter-developers@lists.sf.net .  Subscribe at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

# client|server
# Requires the server's timestamp to be in the present or future (of 2005).
# Tested with ntpdate on Linux.
# Assumes version 2, 3 or 4.

# Note that ntp packets are always 48 bytes, so you should match on that too.

ntp
^([\x13\x1b\x23\xd3\xdb\xe3]|[\x14\x1c$].......?.?.?.?.?.?.?.?.?[\xc6-\xff])
