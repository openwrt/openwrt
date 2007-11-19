# (S)NTP - (Simple) Network Time Protocol - RFCs 1305 and 2030
# Pattern attributes: good fast fast overmatch 
# Protocol groups: time_synchronization ietf_draft_standard
# Wiki: http://www.protocolinfo.org/wiki/NTP
#
# This pattern is tested and is believed to work.

# client|server
# Requires the server's timestamp to be in the present or future (of 2005).
# Tested with ntpdate on Linux.
# Assumes version 2, 3 or 4.

# Note that ntp packets are always 48 bytes, so you should match on that too.

ntp
^([\x13\x1b\x23\xd3\xdb\xe3]|[\x14\x1c$].......?.?.?.?.?.?.?.?.?[\xc6-\xff])
