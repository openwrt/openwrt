# Tell automake we don't want to comply with every last clause in the
# GNU Maintainer's Manual.
AUTOMAKE_OPTIONS = foreign

# We need to build the following subdirectories in this order.  Note that
# we put a Makefile.am in every subdirectory, even if there's nothing to
# compile, so that we can support 'make dist' gracefully.
#
# Dependencies: src <- tool (for libJudy), tool <- doc (for jhton), src <-
# test (for libJudy).
#SUBDIRS = src tool doc test make_includes
#SUBDIRS = src/JudyCommon src/JudyL src/Judy1 src/JudySL src/JudyHS src/obj
SUBDIRS = src

# These files will be included in our tarballs, even though automake knows
# nothing else about them.
#EXTRA_DIST = Makefile.multi original_configure .cvsignore

DISTCLEANFILES = config.log config.status Makefile libtool make.out
