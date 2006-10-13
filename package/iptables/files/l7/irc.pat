# IRC - Internet Relay Chat - RFC 1459
# Pattern quality: good veryfast
#
# Usually runs on port 6666 or 6667
# Note that chat traffic runs on these ports, but IRC-DCC traffic (which
# can use much more bandwidth) uses a dynamically assigned port, so you 
# must have the IRC connection tracking module in your kernel to classify
# this.
#
# This pattern has been tested and is believed to work well.  If it does not
# work for you, or you believe it could be improved, please post to 
# l7-filter-developers@lists.sf.net .  This list may be subscribed to at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

irc
# First thing that happens is that the client sends NICK and USER, in 
# either order.  This allows MIRC color codes (\x02-\x0d instead of
# \x09-\x0d).
^(nick[\x09-\x0d -~]*user[\x09-\x0d -~]*:|user[\x09-\x0d -~]*:[\x02-\x0d -~]*nick[\x09-\x0d -~]*\x0d\x0a)

