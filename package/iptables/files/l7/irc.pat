# IRC - Internet Relay Chat - RFC 1459
# Pattern attributes: great veryfast fast
# Protocol groups: chat ietf_proposed_standard
# Wiki: http://www.protocolinfo.org/wiki/IRC
#
# Usually runs on port 6666 or 6667
# Note that chat traffic runs on these ports, but IRC-DCC traffic (which
# can use much more bandwidth) uses a dynamically assigned port, so you 
# must have the IRC connection tracking module in your kernel to classify
# this.
#
# This pattern has been tested and is believed to work well.

irc
# First thing that happens is that the client sends NICK and USER, in 
# either order.  This allows MIRC color codes (\x02-\x0d instead of
# \x09-\x0d).
^(nick[\x09-\x0d -~]*user[\x09-\x0d -~]*:|user[\x09-\x0d -~]*:[\x02-\x0d -~]*nick[\x09-\x0d -~]*\x0d\x0a)

