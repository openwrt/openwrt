# Note #! intentionally left out.  This should be called with the version of 
# python you care about.

import sys
print ("/usr/lib/python%s.%s/site-packages" % (sys.version_info[0], sys.version_info[1],))
