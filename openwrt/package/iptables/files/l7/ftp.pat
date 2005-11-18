# FTP - File Transfer Protocol - RFC 959
# Pattern quality: great fast
#
# Usually runs on port 21.  Note that the data stream is on a dynamically
# assigned port, which means that you will need the FTP connection 
# tracking module in your kernel to usefully match FTP data transfers.
# 
# This pattern is well tested.  If it does not
# work for you, or you believe it could be improved, please post to 
# l7-filter-developers@lists.sf.net .  This list may be subscribed to at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers
#
# Matches the first two things a server should say.  Most servers say 
# something after 220, even though they don't have to, and it usually
# includes the string "ftp" (l7-filter is case insensitive).
# This includes proftpd, vsftpd, wuftpd, warftpd, pureftpd, Bulletproof 
# FTP Server, and whatever ftp.microsoft.com uses.  Just in case, the next 
# thing the server sends is a 331.  All the above servers also send 
# something including "password" after this code.
ftp
# actually, let's just do the first for now, it's faster
^220[\x09-\x0d -~]*ftp

# This is ~10x faster if the stream starts with "220"
#^220.*ftp

# This will match more, but much slower
#^220[\x09-\x0d -~]*ftp|331[\x09-\x0d -~]*password

# This pattern is more precise, but takes longer to match. (3 packets vs. 1)
#^220[\x09-\x0d -~]*\x0d\x0aUSER[\x09-\x0d -~]*\x0d\x0a331

# same as above, but slightly less precise and only takes 2 packets.
#^220[\x09-\x0d -~]*\x0d\x0aUSER[\x09-\x0d -~]*\x0d\x0a
