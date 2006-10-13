# VNC - Virtual Network Computing.  Also known as RFB - Remote Frame Buffer
# Pattern quality: good fast
# http://www.realvnc.com/documentation.html
# 
# This pattern has been verified with vnc v3.3.7 on WinXP and Linux
# Please report on how this pattern works for you at
# l7-filter-developers@lists.sf.net .  If you can improve on this pattern,
# please also post to that list. You may subscribe at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers
#
# Thanks to Trevor Paskett <tpaskett AT cymphonix.com> for this pattern.

vnc
# Assumes single digit major and minor version numbers 
# This message should be all alone in the first packet, so ^$ is appropriate
^rfb 00[1-9]\.00[0-9]\x0a$

# This is a more restrictive version which assumes the version numbers
# are ones actually in existance at the time of this writing, i.e. 3.3,
# 3.7 and 3.8 (with some clients wrongly reporting 3.5).  It should be
# slightly faster, but probably not worth the extra maintenance. 
# ^rfb 003\.00[3578]\x0a$

