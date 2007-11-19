# POP3 - Post Office Protocol version 3 (popular e-mail protocol) - RFC 1939
# Pattern attributes: great veryfast fast
# Protocol groups: mail ietf_internet_standard
# Wiki: http://www.protocolinfo.org/wiki/POP
#
# This pattern has been tested somewhat.

# this is a difficult protocol to match because of the relative lack of 
# distinguishing information.  Read on.
pop3

# this the most conservative pattern.  It should definitely work.
#^(\+ok|-err)

# this pattern assumes that the server says _something_ after +ok or -err
# I think this is probably the way to go.
^(\+ok |-err )

# more that 90% of servers seem to say "pop" after "+ok", but not all.
#^(\+ok .*pop)

# Here's another tack. I think this is my second favorite.
#^(\+ok [\x09-\x0d -~]*(ready|hello|pop|starting)|-err [\x09-\x0d -~]*(invalid|unknown|unimplemented|unrecognized|command))

# this matches the server saying "you have N messages that are M bytes",
# which the client probably asks for early in the session (not tested)
#\+ok [0-9]+ [0-9]+

# some sample servers:
# RFC example:        +OK POP3 server ready <1896.697170952@dbc.mtview.ca.us>
# mail.dreamhost.com: +OK Hello there.
# pop.carleton.edu:   +OK POP3D(*) Server PMDFV6.2.2 at Fri, 12 Sep 2003 19:28:10 -0500 (CDT) (APOP disabled)
# mail.earthlink.net: +OK NGPopper vEL_4_38 at earthlink.net ready <25509.1063412951@falcon>
# *.email.umn.edu:    +OK Cubic Circle's v1.22 1998/04/11 POP3 ready <7d1e0000da67623f@aquamarine.tc.umn.edu>
# mail.yale.edu:      +OK POP3 pantheon-po01 v2002.81 server ready
# mail.gustavus.edu:  +OK POP3 solen v2001.78 server ready
# mail.reed.edu:      +OK POP3 letra.reed.edu v2002.81 server ready
# mail.bowdoin.edu:   +OK mail.bowdoin.edu POP3 service (iPlanet Messaging Server 5.2 HotFix 1.15 (built Apr 28 2003))
# pop.colby.edu:      +OK Qpopper (version 4.0.5) at basalt starting.
# mail.mac.com:       +OK Netscape Messaging Multiplexor ready

# various error strings:
#-ERR Invalid command.
#-ERR invalid command
#-ERR unimplemented
#-ERR Invalid command, try one of: USER name, PASS string, QUIT
#-ERR Unknown AUTHORIZATION state command
#-ERR Unrecognized command
#-ERR Unknown command: "sadf'".
