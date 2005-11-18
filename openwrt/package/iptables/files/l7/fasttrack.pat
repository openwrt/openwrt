# FastTrack - P2P filesharing (Kazaa, Morpheus, iMesh, Grokster, etc)
# Pattern quality: good notsofast
#
# Tested with Kazaa Lite Resurrection 0.0.7.6F
#
# This appears to match the download connections well, but not the search
# connections (I think they are encrypted :-( ).
#
# Please post to l7-filter-developers@lists.sf.net as to whether it works 
# for you or not.  If you believe it could be improved please post your 
# suggestions to that list as well. You may subscribe to this list at 
# http://lists.sourceforge.net/lists/listinfo/l7-filter-developers

fasttrack
# while this is a valid http request, this will be caught because
# the http pattern matches the response (and therefore the next packet)
# Even so, it's best to put this match earlier in the chain.
# http://cvs.berlios.de/cgi-bin/viewcvs.cgi/gift-fasttrack/giFT-FastTrack/PROTOCOL?rev=HEAD&content-type=text/vnd.viewcvs-markup

# This pattern is kinda slow, but not too bad.
^get (/.download/[ -~]*|/.supernode[ -~]|/.status[ -~]|/.network[ -~]*|/.files|/.hash=[0-9a-f]*/[ -~]*) http/1.1|user-agent: kazaa|x-kazaa(-username|-network|-ip|-supernodeip|-xferid|-xferuid|tag)|^give [0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]?[0-9]?[0-9]?

# This isn't much faster:
#^get (/.download/.*|/.supernode.|/.status.|/.network.*|/.files|/.hash=[0-9a-f]*/.*) http/1.1|user-agent: kazaa|x-kazaa(-username|-network|-ip|-supernodeip|-xferid|-xferuid|tag)|^give [0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]?[0-9]?[0-9]?

