# FTP - File Transfer Protocol - RFC 959
# Pattern attributes: great notsofast fast
# Protocol groups: document_retrieval ietf_internet_standard
# Wiki: http://protocolinfo.org/wiki/FTP
#
# Usually runs on port 21.  Note that the data stream is on a dynamically
# assigned port, which means that you will need the FTP connection 
# tracking module in your kernel to usefully match FTP data transfers.
# 
# This pattern is well tested.
#
# Handles the first two things a server should say:
#
# First, the server says it's ready by sending "220".  Most servers say 
# something after 220, even though they don't have to, and it usually 
# includes the string "ftp" (l7-filter is case insensitive). This 
# includes proftpd, vsftpd, wuftpd, warftpd, pureftpd, Bulletproof FTP 
# Server, and whatever ftp.microsoft.com uses.  Almost all servers use only 
# ASCII printable characters between the "220" and the "FTP", but non-English
# ones might use others.
# 
# The next thing the server sends is a 331.  All the above servers also 
# send something including "password" after this code.  By default, we 
# do not match on this because it takes another packet and is more work 
# for regexec.

ftp
# by default, we allow only ASCII
^220[\x09-\x0d -~]*ftp

# This covers UTF-8 as well 
#^220[\x09-\x0d -~\x80-\xfd]*ftp

# This allows any characters and is about 4x faster than either of the above 
# (which are about the same as each other)
#^220.*ftp

# This is much slower
#^220[\x09-\x0d -~]*ftp|331[\x09-\x0d -~]*password

# This pattern is more precise, but takes longer to match. (3 packets vs. 1)
#^220[\x09-\x0d -~]*\x0d\x0aUSER[\x09-\x0d -~]*\x0d\x0a331

# same as above, but slightly less precise and only takes 2 packets.
#^220[\x09-\x0d -~]*\x0d\x0aUSER[\x09-\x0d -~]*\x0d\x0a
