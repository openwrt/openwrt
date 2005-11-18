# SSH - Secure SHell
# Pattern quality: great veryfast
# usually runs on port 22
#
# http://www.ietf.org/internet-drafts/draft-ietf-secsh-transport-22.txt
#
# This pattern has been tested and is believed to work well.  If it does not
# work for you, or you believe it could be improved, please post to 
# l7-filter-developers@lists.sf.net .  This list may be subscribed to at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

ssh
^ssh-[12]\.[0-9]

# old pattern:
# (diffie-hellman-group-exchange-sha1|diffie-hellman-group1-sha1.ssh-rsa|ssh-dssfaes128-cbc|3des-cbc|blowfish-cbc|cast128-cbc|arcfour|aes192-cbc|aes256-cbc|rijndael-cbc@lysator.liu.sefaes128-cbc|3des-cbc|blowfish-cbc|cast128-cbc|arcfour|aes192-cbc|aes256-cbc|rijndael-cbc@lysator.liu.seuhmac-md5|hmac-sha1|hmac-ripemd160)+
