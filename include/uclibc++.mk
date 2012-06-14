ifndef DUMP
  ifdef __package_mk
    $(error uclibc++.mk must be included before package.mk)
  endif
endif

PKG_PREPARED_DEPENDS += CONFIG_USE_UCLIBCXX
CXX_DEPENDS = +USE_UCLIBCXX:uclibcxx +USE_LIBSTDCXX:libstdcpp

ifneq ($(CONFIG_USE_UCLIBCXX),)
  TARGET_CXX="g++-uc"
endif
