# HTTP - HyperText Transfer Protocol - RFC 2616
# Pattern attributes: great slow notsofast superset
# Protocol groups: document_retrieval ietf_draft_standard
# Wiki: http://protocolinfo.org/wiki/HTTP
# Copyright (C) 2008 Matthew Strait, Ethan Sommer; See ../LICENSE
#
# Usually runs on port 80
#
# This pattern has been tested and is believed to work well.
#
# this intentionally catches the response from the server rather than
# the request so that other protocols which use http (like kazaa) can be
# caught based on specific http requests regardless of the ordering of
# filters... also matches posts

# Sites that serve really long cookies may break this by pushing the
# server response too far away from the beginning of the connection. To
# fix this, increase the kernel's data buffer length.

http
# Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF (rfc 2616)
# As specified in rfc 2616 a status code is preceeded and followed by a
# space. 
http/(0\.9|1\.0|1\.1) [1-5][0-9][0-9] [\x09-\x0d -~]*(connection:|content-type:|content-length:|date:)|post [\x09-\x0d -~]* http/[01]\.[019]
# A slightly faster version that might be good enough:
#http/(0\.9|1\.0|1\.1) [1-5][0-9][0-9]|post [\x09-\x0d -~]* http/[01]\.[019]
# old pattern(s):
#(http[\x09-\x0d -~]*(200 ok|302 |304 )[\x09-\x0d -~]*(connection:|content-type:|content-length:))|^(post [\x09-\x0d -~]* http/)
