#!/usr/bin/env bash
TOPDIR=`dirname "$0"`/..
eval env -S \
	`make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.TARGET_CONFIGURE_OPTS` \
	PATH=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.TARGET_PATH)"` \
	CFLAGS=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.TARGET_CFLAGS)"` \
	CPPFLAGS=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.TARGET_CPPFLAGS)"` \
	CXXFLAGS=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.TARGET_CXXFLAGS)"` \
	LDFLAGS=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.TARGET_LDFLAGS)"` \
	STAGING_DIR=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.STAGING_DIR)"` \
	DESTDIR=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.STAGING_DIR)"` \
	CMAKE_PREFIX_PATH=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.STAGING_DIR)/usr"` \
	host_alias=`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.GNU_TARGET_NAME)"` \
	PROMPT_COMMAND=\"echo -en \\\\\\e[1\\\;34m`printf '%q ' "$(make -s TOPDIR="${TOPDIR}" -f "${TOPDIR}/rules.mk" val.GNU_TARGET_NAME)"` \\\\\\e[0\\\;39m\" \
	`printf '%q ' "${@-$SHELL}"`

#
