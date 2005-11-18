# Ident - Identification Protocol - RFC 1413
# Pattern quality: good veryfast
# Usually runs on port 113
#
# This pattern is believed to work.  If it does not
# work for you, or you believe it could be improved, please post to 
# l7-filter-developers@lists.sf.net .  This list may be subscribed to at
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

ident
# "number , numberCRLF" possibly without the CR and/or LF.
# ^$ is appropriate because the first packet should never have anything
# else in it.
^[1-9][0-9]?[0-9]?[0-9]?[0-9]?[\x09-\x0d]*,[\x09-\x0d]*[1-9][0-9]?[0-9]?[0-9]?[0-9]?(\x0d\x0a|[\x0d\x0a])?$
