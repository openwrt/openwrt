#!/bin/sh
logfile="/tmp/frpc.log"
dir="/usr/share/frpc/"
client_file=`uci get frpc.@frpc[0].client_file 2>/dev/null`
frpc_new_version=`uci get frpc.@frpc[0].edition 2>/dev/null`
	
	if ( ! cmp -s ${dir}frpc_version ${dir}frpc_new_version );then
		echo "$(date "+%Y-%m-%d %H:%M:%S") 检测到frpc版本为$frpc_new_version..." >> ${logfile}

		UpdateApp() {
			for a in $(opkg print-architecture | awk '{print $2}'); do
				case "$a" in
					all|noarch)
						;;
					aarch64_armv8-a|arm_arm1176jzf-s_vfp|arm_arm926ej-s|arm_cortex-a15_neon-vfpv4|arm_cortex-a5|arm_cortex-a53_neon-vfpv4|arm_cortex-a7_neon-vfpv4|arm_cortex-a8_vfpv3|arm_cortex-a9|arm_cortex-a9_neon|arm_cortex-a9_vfpv3|arm_fa526|arm_mpcore|arm_mpcore_vfp|arm_xscale|armeb_xscale)
						ARCH="linux_arm"
						;;
					i386_pentium|i386_pentium4)
						ARCH="linux_386"
						;;
					ar71xx|mips_24kc|mips_mips32|mips64_octeon)
						ARCH="linux_mips"
						;;
					mipsel_24kc|mipsel_24kec_dsp|mipsel_74kc|mipsel_mips32|mipsel_1004kc_dsp)
						ARCH="linux_mipsle"
						;;
					x86_64)
						ARCH="linux_amd64"
						;;
					*)
						exit 0
						;;
				esac
			done
		}

		download_binary(){
			echo "$(date "+%Y-%m-%d %H:%M:%S") 开始下载frpc二进制文件..." >> ${logfile}
			bin_dir="/tmp"
			UpdateApp
			cd $bin_dir
			down_url=https://github.com/fatedier/frp/releases/download/v"$frpc_new_version"/frp_"$frpc_new_version"_"$ARCH".tar.gz

			local a=0
			while [ ! -f $bin_dir/frp_"$frpc_new_version"_"$ARCH".tar.gz ]; do
				[ $a = 6 ] && exit
				/usr/bin/wget -T10 $down_url
				sleep 2
				let "a = a + 1"
			done
	
			if [ -f $bin_dir/frp_"$frpc_new_version"_"$ARCH".tar.gz ]; then
				echo "$(date "+%Y-%m-%d %H:%M:%S") 成功下载frpc二进制文件" >> ${logfile}
				killall -q -9 frpc
	
				tar -xzvf frp_"$frpc_new_version"_"$ARCH".tar.gz -C $bin_dir/
				mv $bin_dir/frp_"$frpc_new_version"_"$ARCH"/frpc $client_file

				rm -rf $bin_dir/frp_"$frpc_new_version"_"$ARCH".tar.gz
				rm -rf $bin_dir/frp_"$frpc_new_version"_"$ARCH"
				if [ -f "/usr/bin/frpc" ]; then
					chmod +x /usr/bin/frpc
					/etc/init.d/frpc restart
				fi
			else
				echo "$(date "+%Y-%m-%d %H:%M:%S") 下载frpc二进制文件失败，请重试！" >> ${logfile}
			fi


		}

		download_binary
		echo "" > ${dir}frpc_version
		echo "$frpc_new_version" > ${dir}frpc_version
		rm -rf ${dir}frpc_new_version
	else
		echo "$(date "+%Y-%m-%d %H:%M:%S") frpc已经是最新的了..." >> ${logfile}
		rm -rf ${dir}frpc_new_version
	fi

