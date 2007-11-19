# MSN Messenger - Microsoft Network chat client
# Pattern attributes: good slow notsofast
# Protocol groups: chat proprietary
# Wiki: http://www.protocolinfo.org/wiki/MSN_Messenger
#
# Usually uses TCP port 1863
# http://www.hypothetic.org/docs/msn/index.php
# http://msnpiki.msnfanatic.com/
#
# This pattern has been tested and is believed to work well.

msnmessenger

# First branch: login
#   ver: allow versions up to 99.
#   I've never seen a cvr other than cvr0.  Maybe this will be trouble later?
#   Can't anchor at the beginning because sometimes this is encapsulated in
#   HTTP.  But either way, the first packet ends like this.
# Second/Third branches: accepting/sending a message
#   I will assume that these can also be encapsulated in HTTP, although I have
#   not checked.  Example of each direction:
#   ANS 1 quadong@hotmail.com 1139803431.29427 17522047
#   USR 1 quadong@hotmail.com 530423708.968145.366138

# Branches are written entirely separately for better performance.
ver [0-9]+ msnp[1-9][0-9]? [\x09-\x0d -~]*cvr0\x0d\x0a$|usr 1 [!-~]+ [0-9. ]+\x0d\x0a$|ans 1 [!-~]+ [0-9. ]+\x0d\x0a$

