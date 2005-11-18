# Bittorrent - P2P filesharing / publishing tool - http://www.bittorrent.com
# Pattern quality: great veryfast
#
# This pattern has been tested and is believed to work well.  If it does not
# work for you, or you believe it could be improved, please post to 
# l7-filter-developers@lists.sf.net .  This list may be subscribed to at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers
bittorrent

# Does not attempt to match the HTTP download of the tracker
# 0x13 is the length of "bittorrent protocol"
# Second two bits match UDP wierdness, commented out until it's tested
#^(\x13bittorrent protocol|d1:ad2:id20:|\x08'7P\)[RP])
^\x13bittorrent protocol
