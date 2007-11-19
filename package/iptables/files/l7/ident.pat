# Ident - Identification Protocol - RFC 1413
# Pattern attributes: good fast fast
# Protocol groups: networking ietf_proposed_standard
# Wiki: http://www.protocolinfo.org/wiki/Ident
#
# Usually runs on port 113
#
# This pattern is believed to work.

ident
# "number , numberCRLF" possibly without the CR and/or LF.
# ^$ is appropriate because the first packet should never have anything
# else in it.
^[1-9][0-9]?[0-9]?[0-9]?[0-9]?[\x09-\x0d]*,[\x09-\x0d]*[1-9][0-9]?[0-9]?[0-9]?[0-9]?(\x0d\x0a|[\x0d\x0a])?$
