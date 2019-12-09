#!/bin/bash
PATH="/usr/sbin:/usr/bin:/sbin:/bin"
binpath=$(uci get AdGuardHome.AdGuardHome.binpath)
if [ -z "$binpath" ]; then
uci set AdGuardHome.AdGuardHome.binpath="/tmp/AdGuardHome/AdGuardHome"
binpath="/tmp/AdGuardHome/AdGuardHome"
fi
mkdir -p ${binpath%/*}
configpath=$(uci get AdGuardHome.AdGuardHome.configpath)
if [ -z "$configpath" ]; then
uci get AdGuardHome.AdGuardHome.configpath="/etc/AdGuardHome.yaml"
configpath="/etc/AdGuardHome.yaml"
fi
mkdir -p ${configpath%/*}
upxflag=$(uci get AdGuardHome.AdGuardHome.upxflag)

check_if_already_running(){
	running_tasks="$(ps |grep "AdGuardHome" |grep "update_core" |grep -v "grep" |awk '{print $1}' |wc -l)"
	[ "${running_tasks}" -gt "2" ] && echo -e "\nA task is already running."  && exit 2
}

clean_log(){
	echo "" > /tmp/AdGuardHome_update.log
}

check_latest_version(){
	latest_ver="$(wget -O- https://api.github.com/repos/AdguardTeam/AdGuardHome/releases/latest 2>/dev/null|grep -E 'tag_name' |grep -E 'v[0-9.]+' -o 2>/dev/null)"
	[ -z "${latest_ver}" ] && echo -e "\nFailed to check latest version, please try again later."  && exit 1
	
	if [ -f "$configpath" ]; then
		now_ver="$($binpath -c $configpath --check-config 2>&1| grep -m 1 -E 'v[0-9.]+' -o)"
	elif [ -f "$binpath" ]; then
		chmod a+x $binpath
		$binpath -l /tmp/AdGuardHometmp.log &
		if [ "$?" == "0" ]; then
			pid=$!
			sleep 2
			kill $pid
			now_ver="$(grep -m 1 -E 'v[0-9.]+' -o /tmp/AdGuardHometmp.log)"
			rm /tmp/AdGuardHometmp.log
		else
			echo "bin file may broken"
		fi
	fi
	if [ "${latest_ver}"x != "${now_ver}"x ]; then
		clean_log
		echo -e "Local version: ${now_ver}., cloud version: ${latest_ver}." 
		doupdate_core
	else
			echo -e "\nLocal version: ${now_ver}, cloud version: ${latest_ver}." 
			echo -e "You're already using the latest version." 
			uci set AdGuardHome.AdGuardHome.version="${latest_ver}"
			uci commit AdGuardHome
			if [ ! -z "$upxflag" ]; then
				filesize=$(ls -l $binpath | awk '{ print $5 }')
				if [ $filesize -gt 8000000 ]; then
					echo -e "start upx may take a long time"
					doupx
					mkdir -p "/tmp/AdGuardHomeupdate/AdGuardHome" >/dev/null 2>&1
					rm -fr /tmp/AdGuardHomeupdate/AdGuardHome/${binpath##*/}
					/tmp/upx-${upx_latest_ver}-${Arch}_linux/upx $upxflag $binpath -o /tmp/AdGuardHomeupdate/AdGuardHome/${binpath##*/}
					rm -rf /tmp/upx-${upx_latest_ver}-${Arch}_linux
					/etc/init.d/AdGuardHome stop
					rm $binpath
					mv -f /tmp/AdGuardHomeupdate/AdGuardHome/${binpath##*/} $binpath
					/etc/init.d/AdGuardHome start
					echo -e "finished"
				fi
			fi
			exit 3
	fi
}
doupx(){
	Archt="$(opkg info kernel | grep Architecture | awk -F "[ _]" '{print($2)}')"
	case $Archt in
	"i386")
	Arch="i386"
	;;
	"i686")
	Arch="i386"
	echo -e "i686 use $Arch may have bug" 
	;;
	"x86")
	Arch="amd64"
	;;
	"mipsel")
	Arch="mipsel"
	;;
	"mips64el")
	Arch="mips64el"
	Arch="mipsel"
	echo -e "mips64el use $Arch may have bug" 
	;;
	"mips")
	Arch="mips"
	;;
	"mips64")
	Arch="mips64"
	Arch="mips"
	echo -e "mips64 use $Arch may have bug" 
	;;
	"arm")
	Arch="arm"
	;;
	"armeb")
	Arch="armeb"
	;;
	"aarch64")
	Arch="arm64"
	;;
	"powerpc")
	Arch="powerpc"
	;;
	"powerpc64")
	Arch="powerpc64"
	;;
	*)
	echo -e "error not support $Archt" 
	exit 1
	;;
	esac
	upx_latest_ver="$(wget -O- https://api.github.com/repos/upx/upx/releases/latest 2>/dev/null|grep -E 'tag_name' |grep -E '[0-9.]+' -o 2>/dev/null)"
	wget-ssl --no-check-certificate -t 1 -T 10 -O  /tmp/upx-${upx_latest_ver}-${Arch}_linux.tar.xz "https://github.com/upx/upx/releases/download/v${upx_latest_ver}/upx-${upx_latest_ver}-${Arch}_linux.tar.xz"  >/dev/null 2>&1
	#tar xvJf
	which xz || (opkg update && opkg install xz) || exit 1
	mkdir -p /tmp/upx-${upx_latest_ver}-${Arch}_linux
	xz -d -c /tmp/upx-${upx_latest_ver}-${Arch}_linux.tar.xz| tar -x -C "/tmp" >/dev/null 2>&1
	rm /tmp/upx-${upx_latest_ver}-${Arch}_linux.tar.xz
}
doupdate_core(){
	echo -e "Updating core..." 
	mkdir -p "/tmp/AdGuardHomeupdate"
	rm -rf /tmp/AdGuardHomeupdate/* >/dev/null 2>&1
	Archt="$(opkg info kernel | grep Architecture | awk -F "[ _]" '{print($2)}')"
	case $Archt in
	"i386")
	Arch="386"
	;;
	"i686")
	Arch="386"
	;;
	"x86")
	Arch="amd64"
	;;
	"mipsel")
	Arch="mipsle"
	;;
	"mips64el")
	Arch="mips64le"
	Arch="mipsle"
	echo -e "mips64el use $Arch may have bug" 
	;;
	"mips")
	Arch="mips"
	;;
	"mips64")
	Arch="mips64"
	Arch="mips"
	echo -e "mips64 use $Arch may have bug" 
	;;
	"arm")
	Arch="arm"
	;;
	"aarch64")
	Arch="arm64"
	;;
	"powerpc")
	Arch="ppc"
	echo -e "error not support $Archt" 
	exit 1
	;;
	"powerpc64")
	Arch="ppc64"
	echo -e "error not support $Archt" 
	exit 1
	;;
	*)
	echo -e "error not support $Archt" 
	
	exit 1
	;;
	esac
	echo -e "start download ${latest_ver}/AdGuardHome_linux_${Arch}.tar.gz" 
	wget-ssl --no-check-certificate -t 1 -T 10 -O /tmp/AdGuardHomeupdate/AdGuardHome_linux_${Arch}.tar.gz "https://github.com/AdguardTeam/AdGuardHome/releases/download/${latest_ver}/AdGuardHome_linux_${Arch}.tar.gz"
	tar -zxf "/tmp/AdGuardHomeupdate/AdGuardHome_linux_${Arch}.tar.gz" -C "/tmp/AdGuardHomeupdate/"
	if [ ! -e "/tmp/AdGuardHomeupdate/AdGuardHome" ]; then
		echo -e "Failed to download core." 
		rm -rf "/tmp/AdGuardHomeupdate" >/dev/null 2>&1
		exit 1
	else 
		echo -e "download success start copy" 
		if [ ! -z "$upxflag" ]; then
		echo -e "start upx may take a long time" 
		doupx
        #maybe need chmod
		/tmp/upx-${upx_latest_ver}-${Arch}_linux/upx $upxflag /tmp/AdGuardHomeupdate/AdGuardHome/AdGuardHome
		rm -rf /tmp/upx-${upx_latest_ver}-${Arch}_linux
		fi
		echo -e "start copy" 
		/etc/init.d/AdGuardHome stop
		rm "$binpath"
		mv -f /tmp/AdGuardHomeupdate/AdGuardHome/AdGuardHome "$binpath"
		if [ "$?" == "1" ]; then
			echo "mv failed maybe not enough space please use upx or change bin to /tmp/AdGuardHome" 
			exit 1
		fi
		/etc/init.d/AdGuardHome start
	fi
	rm -rf "/tmp/AdGuardHomeupdate" >/dev/null 2>&1
	echo -e "Succeeded in updating core." 
	uci set AdGuardHome.AdGuardHome.version="${latest_ver}"
	uci commit AdGuardHome
	echo -e "Local version: ${now_ver}, cloud version: ${latest_ver}.\n" 
}

main(){
	check_if_already_running
	check_latest_version
}
	main
