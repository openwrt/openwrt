# SSL and TLS - Secure Socket Layer / Transport Layer Security - RFC 2246
# Pattern quality: good fast
# Usually runs on port 443
#
# This is a superset validcertssl.  For it to match, it must be first.
# 
# This pattern has been tested and is believed to work well.  If it does not
# work for you, or you believe it could be improved, please post to
# l7-filter-developers@lists.sf.net .  This list may be subscribed to at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

ssl
# Client Hello | Server Hello with certificate
# This allows SSL 3.X, which includes TLS 1.0, known internally as SSL 3.1
^(.?.?\x16\x03.*\x16\x03|.?.?\x01\x03\x01?.*\x0b)
