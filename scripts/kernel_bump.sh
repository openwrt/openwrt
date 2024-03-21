#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2024 Olliver Schinagl <oliver@schinagl.nl>

set -eu
if [ -n "${DEBUG_TRACE_SH:-}" ] && \
   [ "${DEBUG_TRACE_SH:-}" != "${DEBUG_TRACE_SH#*"$(basename "${0}")"*}" ] || \
   [ "${DEBUG_TRACE_SH:-}" = 'all' ]; then
	set -x
fi

REQUIRED_COMMANDS='
	[
	basename
	command
	echo
	exit
	git
	printf
	set
	shift
'

_msg()
{
	_level="${1:?Missing argument to function}"
	shift

	if [ "${#}" -le 0 ]; then
		echo "${_level}: No content for this message ..."
		return
	fi

	echo "${_level}: ${*}"
}

e_err()
{
	_msg 'err' "${*}" >&2
}

e_warn()
{
	_msg 'warning' "${*}"
}

e_notice()
{
	_msg 'notice' "${*}"
}

usage()
{
	echo "Usage: ${0}"
	echo 'Helper script to bump the target kernel version, whilst keeping history.'
	echo "    -p  Optional Platform name (e.g. 'ath79' [PLATFORM_NAME]"
	echo "    -s  Source version of kernel (e.g. 'v6.1' [SOURCE_VERSION])"
	echo "    -t  Target version of kernel (e.g. 'v6.6' [TARGET_VERSION]')"
	echo
	echo 'All options can also be passed in environment variables (listed between [BRACKETS]).'
	echo 'Note that this script must be run from within the OpenWrt git repository.'
	echo 'Example: scripts/kernel_bump.sh -p realtek -s v6.1 -t v6.6'
}

cleanup()
{
	trap - EXIT HUP INT QUIT ABRT ALRM TERM

	if [ -n "${initial_branch:-}" ] && \
	   [ "$(git rev-parse --abbrev-ref HEAD)" != "${initial_branch:-}" ]; then
		git switch "${initial_branch}"
	fi
}

init()
{
	src_file="$(readlink -f "${0}")"
	src_dir="${src_file%%"${src_file##*'/'}"}"
	initial_branch="$(git rev-parse --abbrev-ref HEAD)"
	initial_commitish="$(git rev-parse HEAD)"

	if [ -n "$(git status --porcelain | grep -v '^?? .*')" ]; then
		echo 'Git respository not in a clean state, will not continue.'
		exit 1
	fi

	if [ -n "${src_dir##*'/scripts/'}" ]; then
		echo "This script '${src_file}' is not in the scripts subdirectory, this is unexpected, cannot continue."
		exit 1
	fi

	source_version="${source_version#v}"
	target_version="${target_version#v}"

	trap cleanup EXIT HUP INT QUIT ABRT ALRM TERM
}

bump_kernel()
{
	if [ -z "${platform_name}" ] || \
	   [ -d "${PWD}/image" ]; then
		platform_name="${PWD}"
	fi
	platform_name="${platform_name##*'/'}"

	_target_dir="${src_dir}/../target/linux/${platform_name}"

	if [ ! -d "${_target_dir}/image" ]; then
		e_err "Cannot find target linux directory '${_target_dir:-not defined}'. Not in a platform directory, or -p not set."
		exit 1
	fi

	git switch --force-create '__openwrt_kernel_files_mover'

	for _path in "${_target_dir}/"*; do
		if [ ! -s "${_path}" ] || \
		   [ "${_path}" = "${_path%%"-${source_version}"}" ]; then
			continue
		fi

		_target_path="${_path%%"-${source_version}"}-${target_version}"
		if [ -s "${_target_path}" ]; then
			e_err "Target '${_target_path}' already exists!"
			exit 1
		fi

		git mv \
			"${_path}" \
			"${_target_path}"
	done

	find "${_target_dir}" -iname "config-${source_version}" | while read -r _config; do
		_path="${_config%%"/config-${source_version}"}"
		git mv "${_config}" "${_path}/config-${target_version}"
	done

	git commit \
		--signoff \
		--message "kernel/${platform_name}: Create kernel files for v${target_version} (from v${source_version})" \
		--message 'This is an automatically generated commit.' \
		--message 'When doing `git bisect`, consider `git bisect --skip`.'

	git checkout 'HEAD~' "${_target_dir}"
	git commit \
		--signoff \
		--message "kernel/${platform_name}: Restore kernel files for v${source_version}" \
		--message "$(printf "This is an automatically generated commit which aids following Kernel patch history,\nas git will see the move and copy as a rename thus defeating the purpose.\n\nSee: https://lists.openwrt.org/pipermail/openwrt-devel/2023-October/041673.html\nfor the original discussion.")"
	git switch "${initial_branch:?Unable to switch back to original branch. Quitting.}"
	GIT_EDITOR=true git merge --no-ff '__openwrt_kernel_files_mover'
	git branch --delete '__openwrt_kernel_files_mover'

	echo "Original commitish was '${initial_commitish}'."
	echo 'Kernel bump complete. Remember to use `git log --follow`.'
}

check_requirements()
{
	for _cmd in ${REQUIRED_COMMANDS}; do
		if ! _test_result="$(command -V "${_cmd}")"; then
			_test_result_fail="${_test_result_fail:-}${_test_result}\n"
		else
			_test_result_pass="${_test_result_pass:-}${_test_result}\n"
		fi
	done

	echo 'Available commands:'
	# As the results contain \n, we expect these to be interpreted.
	# shellcheck disable=SC2059
	printf "${_test_result_pass:-none\n}"
	echo
	echo 'Missing commands:'
	# shellcheck disable=SC2059
	printf "${_test_result_fail:-none\n}"
	echo

	if [ -n "${_test_result_fail:-}" ]; then
		echo 'Command test failed, missing programs.'
		test_failed=1
	fi
}

main()
{
	while getopts 'hp:s:t:' _options; do
		case "${_options}" in
		'h')
			usage
			exit 0
			;;
		'p')
			platform_name="${OPTARG}"
			;;
		's')
			source_version="${OPTARG}"
			;;
		't')
			target_version="${OPTARG}"
			;;
		':')
			e_err "Option -${OPTARG} requires an argument."
			exit 1
			;;
		*)
			e_err "Invalid option: -${OPTARG}"
			exit 1
			;;
		esac
	done
	shift "$((OPTIND - 1))"

	platform_name="${platform_name:-${PLATFORM_NAME:-}}"
	source_version="${source_version:-${SOURCE_VERSION:-}}"
	target_version="${target_version:-${TARGET_VERSION:-}}"

	if [ -z "${source_version:-}" ] || [ -z "${target_version:-}" ]; then
		e_err "Source (${source_version:-missing source version}) and target (${target_version:-missing target version}) versions need to be defined."
		echo
		usage
		exit 1
	fi

	check_requirements

	init
	bump_kernel
	cleanup
}

main "${@}"

exit 0
