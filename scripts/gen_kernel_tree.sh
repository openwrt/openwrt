#!/bin/sh

set -eu

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
	echo 'A wrapper script to instantiate the bootstrap container.'
	echo '    -b  OpenWRT root directory [BUILD_ROOT]'
	echo '    -f  Force overwrite of the git dir during checkout'
	echo '    -h  Print this usage'
	echo '    -k  Linux kernel path [LINUX_KERNEL_PATH]'
	echo "    -l  Linux kernel release version (e.g. 'v5.10') [LINUX_KERNEL_RELEASE]"
	echo "    -o  OpenWrt version (e.g. '22.03') [OPENWRT_VER]"
	echo "    -p  Optional Platform name (e.g. 'ath79' [PLATFORM_NAME]"
	echo "    -r  Remote to be used (e.g. 'origin' [KERNEL_GIT_REMOTE])"
	echo '    -t  Tag branch name with openwrt git hash [OPENWRT_TAG]'
	echo
	echo 'Example: scripts/gen_kernel_tree.sh -f -k linux -l v5.15 -o 22.03 -p realtek -r stable -t'
}

generate_tree()
{
	kernel_version="${linux_kernel_release}$(sed -n "s|^LINUX_VERSION-${linux_kernel_release}[[:space:]]*=[[:space:]]\(.*\)$|\1|p" "include/kernel-${linux_kernel_release}")"
	openwrt_hash="$(git --git-dir="${build_root}/.git" describe --always --dirty)"
	(
		cd "${linux_kernel_path}"
		if [ "$(head -n1 "${linux_kernel_path}/MAINTAINERS")" != 'List of maintainers and how to submit kernel changes' ]; then
			echo 'Not in a kernel dir, please supply or cd into a kernel dir'
		fi

		git --git-dir "${linux_kernel_path}/.git" fetch ${kernel_git_remote:+${kernel_git_remote}}
		git --git-dir "${linux_kernel_path}/.git" checkout \
		    ${force_overwrite:--b} \
		    "openwrt-${openwrt_ver:?}/${linux_kernel_release}${platform_name:+-${platform_name}}${openwrt_tag:+${openwrt_hash#'reboot'}}" \
		    "v${kernel_version:?}"

		git --git-dir "${linux_kernel_path}/.git" am "${build_root}/target/linux/generic/backport-${linux_kernel_release}/"*'.patch'
		git --git-dir "${linux_kernel_path}/.git" am "${build_root}/target/linux/generic/pending-${linux_kernel_release}/"*'.patch'
		git --git-dir "${linux_kernel_path}/.git" am "${build_root}/target/linux/generic/hack-${linux_kernel_release}/"*'.patch'

		cp -a "${build_root}/target/linux/generic/files/"* "${linux_kernel_path}"
		git --git-dir "${linux_kernel_path}/.git" add "${linux_kernel_path}"
		git --git-dir "${linux_kernel_path}/.git" commit --message 'All OpenWRT generic files'

		if [ -n "${platform_name:-}" ]; then
			if [ -d "${build_root}/target/linux/${platform_name}/patches-${linux_kernel_release}" ]; then
				git --git-dir "${linux_kernel_path}/.git" am "${build_root}/target/linux/${platform_name}/patches-${linux_kernel_release}/"*'.patch'
			fi

			if [ -d "${build_root}/target/linux/${platform_name}/files" ]; then
				cp -a "${build_root}/target/linux/${platform_name}/files/"* "${linux_kernel_path}"
			fi
			if [ -d "${build_root}/target/linux/${platform_name}/files-${linux_kernel_release}" ]; then
				cp -a "${build_root}/target/linux/${platform_name}/files-${linux_kernel_release}/"* "${linux_kernel_path}"
			fi
			git --git-dir "${linux_kernel_path}/.git" add "${linux_kernel_path}"
			git --git-dir "${linux_kernel_path}/.git" commit --message "All OpenWRT '${platform_name}' files"
		fi
	)
}

main()
{
	while getopts ':b:hfk:l:o:p:r:t' _options; do
		case "${_options}" in
		'b')
			build_root="${OPTARG}"
			;;
		'f')
			force_overwrite='-B'
			;;
		'h')
			usage
			exit 0
			;;
		'k')
			linux_kernel_path="${OPTARG}"
			;;
		'l')
			linux_kernel_release="${OPTARG}"
			;;
		'o')
			openwrt_ver="${OPTARG}"
			;;
		'p')
			platform_name="${OPTARG}"
			;;
		'r')
			kernel_git_remote="${OPTARG}"
			;;
		't')
			openwrt_tag='true'
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

	build_root="${build_root:-${BUILD_ROOT:-$(dirname "$(readlink -f "${0}")")/..}}"
	kernel_git_remote="${kernel_git_remote:-${KERNEL_git_REMOTE:-}}"
	linux_kernel_path="$(readlink -f "${linux_kernel_path:-${LINUX_KERNEL_PATH:-$(pwd)}}")"
	linux_kernel_release="${linux_kernel_release:-${LINUX_KERNEL_RELEASE:-}}"
	linux_kernel_release="${linux_kernel_release#'v'}"
	openwrt_tag="${openwrt_tag:-${OPENWRT_TAG:-}}"
	openwrt_ver="${openwrt_ver:-${OPENWRT_VER:-}}"
	platform_name="${platform_name:-${PLATFORM_NAME:-}}"

	if [ ! -d "${linux_kernel_path:-}" ]; then
		e_err "Missing existing kernel tree in '${linux_kernel_path:-}'"
		exit 1
	fi

	generate_tree
}

main "${@}"

exit 0
