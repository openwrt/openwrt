AM_CFLAGS = @WARN_CFLAGS@

CLEANFILES = libJudy.la

include_HEADERS = ../Judy.h

lib_LTLIBRARIES = libJudy.la

libJudy_la_SOURCES =

libJudy_la_LIBADD = ../JudyCommon/*.lo ../JudyL/*.lo ../Judy1/*.lo ../JudyHS/*.lo ../JudySL/*.lo 

libJudy_la_LDFLAGS = @VERSION_INFO@

DISTCLEANFILES = .deps Makefile
