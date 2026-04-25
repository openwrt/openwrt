REQUIRE_IMAGE_METADATA=1
RAMFS_COPY_BIN='fitblk fit_check_sign'

platform_do_upgrade() {
	case "$(board_name)" in
	qcom,ipq5332-ap-mi01.2-qcn9160-c1-u7-pro-xgs)
		CI_KERNPART="kernel0"
		fit_do_upgrade "$1"
		;;
	*)
		echo "Sysupgrade is not supported on your board yet."
		return 1
		;;
	esac
}

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	case "$(board_name)" in
	qcom,ipq5332-ap-mi01.2-qcn9160-c1-u7-pro-xgs)
		fit_check_image "$1"
		;;
	*)
		echo "Sysupgrade is not supported on your board yet."
		return 1
		;;
	esac
}

platform_copy_config() {
	case "$(board_name)" in
	qcom,ipq5332-ap-mi01.2-qcn9160-c1-u7-pro-xgs)
		emmc_copy_config
		;;
	esac
}
