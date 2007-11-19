# FastTrack - P2P filesharing (Kazaa, Morpheus, iMesh, Grokster, etc)
# Pattern attributes: good slow notsofast
# Protocol groups: p2p
# Wiki: http://www.protocolinfo.org/wiki/Fasttrack
#
# Tested with Kazaa Lite Resurrection 0.0.7.6F
#
# This appears to match the download connections well, but not the search
# connections (I think they are encrypted :-( ).

fasttrack
# while this is a valid http request, this will be caught because
# the http pattern matches the response (and therefore the next packet)
# Even so, it's best to put this match earlier in the chain.
# http://cvs.berlios.de/cgi-bin/viewcvs.cgi/gift-fasttrack/giFT-FastTrack/PROTOCOL?rev=HEAD&content-type=text/vnd.viewcvs-markup

# This pattern is kinda slow, but not too bad.
^get (/.download/[ -~]*|/.supernode[ -~]|/.status[ -~]|/.network[ -~]*|/.files|/.hash=[0-9a-f]*/[ -~]*) http/1.1|user-agent: kazaa|x-kazaa(-username|-network|-ip|-supernodeip|-xferid|-xferuid|tag)|^give [0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]?[0-9]?[0-9]?

# This isn't much faster:
#^get (/.download/.*|/.supernode.|/.status.|/.network.*|/.files|/.hash=[0-9a-f]*/.*) http/1.1|user-agent: kazaa|x-kazaa(-username|-network|-ip|-supernodeip|-xferid|-xferuid|tag)|^give [0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]?[0-9]?[0-9]?

