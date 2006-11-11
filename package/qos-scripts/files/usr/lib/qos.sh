#!/bin/sh
. /etc/functions.sh

insmod="insmod"
[ -f /sbin/modprobe ] && insmod="modprobe"

add_insmod() {
	eval "export isset=\${insmod_$1}"
	case "$isset" in
		1) ;;
		*) append INSMOD "$insmod $* >&- 2>&-" "$N"; export insmod_$1=1;;
	esac
}

find_ifname() {(
	include /lib/network
	scan_interfaces
	config_get "$1" ifname
)}

parse_matching_rule() {
	local var="$1"
	local section="$2"
	local options="$3"
	local prefix="$4"
	local suffix="$5"
	local proto="$6"
	local mport=""
	local ports=""

	append "$var" "$prefix" "$N"
	for option in $options; do
		case "$option" in
			proto) config_get value "$section" proto; proto="${proto:-$value}";;
		esac
	done
	config_get type "$section" TYPE
	case "$type" in
		classify) unset pkt; append "$var" "-m mark --mark 0";;
		default) pkt=1; append "$var" "-m mark --mark 0";;
		reclassify) pkt=1;;
	esac
	append "$var" "${proto:+-p $proto}"
	for option in $options; do
		config_get value "$section" "$option"
		
		case "$pkt:$option" in
			*:srchost)
				append "$var" "-s $value"
			;;
			*:dsthost)
				append "$var" "-d $value"
			;;
			*:ipp2p)
				add_insmod ipt_ipp2p
				append "$var" "-m ipp2p"
				case "$value" in
					all) append "$var" "--edk --dc --kazaa --gnu --bit";;
					*) append "$var" "--$value";;
				esac
			;;
			*:layer7)
				add_insmod ipt_layer7
				append "$var" "-m layer7 --l7proto $value${pkt:+ --l7pkt}"
			;;
			*:ports|*:srcports|*:dstports)
				value="$(echo "$value" | sed -e 's,-,:,g')"
				lproto=${lproto:-tcp}
				case "$proto" in
					""|tcp|udp) append "$var" "-m ${proto:-tcp -p tcp} -m multiport";;
					*) unset "$var"; return 0;;
				esac
				case "$option" in
					ports)
						config_set "$section" srcports ""
						config_set "$section" dstports ""
						config_set "$section" portrange ""
						append "$var" "--ports $value"
					;;
					srcports)
						config_set "$section" ports ""
						config_set "$section" dstports ""
						config_set "$section" portrange ""
						append "$var" "--sports $value"
					;;
					dstports)
						config_set "$section" ports ""
						config_set "$section" srcports ""
						config_set "$section" portrange ""
						append "$var" "--dports $value"
					;;
				esac
				ports=1
			;;
			*:portrange)
				config_set "$section" ports ""
				config_set "$section" srcports ""
				config_set "$section" dstports ""
				value="$(echo "$value" | sed -e 's,-,:,g')"
				case "$proto" in
					""|tcp|udp) append "$var" "-m ${proto:-tcp -p tcp} --sport $value --dport $value";;
					*) unset "$var"; return 0;;
				esac
				ports=1
			;;
			1:pktsize)
				value="$(echo "$value" | sed -e 's,-,:,g')"
				add_insmod ipt_length
				append "$var" "-m length --length $value"
			;;
			1:limit)
				add_insmod ipt_limit
				append "$var" "-m limit --limit $value"
			;;
			1:tcpflags)
				case "$proto" in
					tcp) append "$var" "-m tcp --tcp-flags ALL $value";;
					*) unset $var; return 0;;
				esac
			;;
			1:mark)
				config_get class "${value##!}" classnr
				[ -z "$class" ] && continue;
				case "$value" in
					!*) append "$var" "-m mark ! --mark $class";;
					*) append "$var" "-m mark --mark $class";;
				esac
		esac
	done
	append "$var" "$suffix"
	case "$ports:$proto" in
		1:)	parse_matching_rule "$var" "$section" "$options" "$prefix" "$suffix" "udp";;
	esac
}

config_cb() {
	option_cb() {
		return 0
	}

	# Section start
	case "$1" in
		interface)
			config_set "$1" "classgroup" "Default"
			config_set "$1" "upload" "128"
		;;
		classify|default|reclassify)
			option_cb() {
				append options "$1"
			}
		;;
	esac

    # Section end
	config_get TYPE "$CONFIG_SECTION" TYPE
	case "$TYPE" in
		interface)
			config_get enabled "$CONFIG_SECTION" enabled
			config_get download "$CONFIG_SECTION" download
			config_get classgroup "$CONFIG_SECTION" classgroup
			config_set "$CONFIG_SECTION" imqdev "$C"
			[ -z "$enabled" -o "$(($enabled))" -eq 0 ] || {
				C=$(($C+1))
				INTERFACES="$INTERFACES $CONFIG_SECTION"
				config_set "$classgroup" enabled 1
			}
			config_get device "$CONFIG_SECTION" device
			[ -z "$device" ] && device="$(find_ifname ${CONFIG_SECTION})"
			config_set "$CONFIG_SECTION" device "${device:-eth0}"
		;;
		classgroup) append CG "$CONFIG_SECTION";;
		classify|default|reclassify)
			case "$TYPE" in
				classify) var="ctrules";;
				*) var="rules";;
			esac
			config_get target "$CONFIG_SECTION" target
			config_set "$CONFIG_SECTION" options "$options"
			append "$var" "$CONFIG_SECTION"
			unset options
		;;
	esac
}

class_main_qdisc() {
	local device="$1"
	awk -f - <<EOF
BEGIN {
	limit = int("$maxrate")
	m2 = int("$m2")
	dmax = int("$dmax")
	umax = int("$umax")
	share = int("$share")
	
	if (!(m2 > 0)) {
		dmax = 500
		umax = 1500
		m2 = 10
		rt = 0
	} else {
		rt = 1
	}

	cdata = ""
	pdmax = int (dmax + (umax * 8 / limit))
	if (rt == 1) {
		if (share > 0) cdata = " rt"
		else cdata = " ls"
		if ((umax > 0) && (dmax > 0)) {
			cdata = cdata " umax " umax "b dmax " pdmax "ms"
		}
		cdata = cdata " rate " m2 "kbit"
	}
	if (share > 0) {
		if ((m2 > 0) && (umax > 0) && (dmax > 0)) {
			cdata = cdata " ls umax " umax "b dmax " pdmax "ms rate " share "kbit"
		} else {
			cdata = cdata " ls m1 " share "kbit d 500ms m2 " share "kbit"
		}
	}

	print "tc class add dev $device parent 1:1 classid 1:${classnr}0 hfsc" cdata " ul rate " limit "kbit"
}
EOF
}

class_leaf_qdisc() {
	local device="$1"
	awk -f - <<EOF

function qlen(rate, m2, umax, dmax,    qb, qr, qt, ql) {
	qlen_min = 5 # minimum queue length
	qlen_base = 1.7	# base value - queueing time in seconds
	qlen_avgr = 0.7 # avgrate modifier
	qlen_dmax = 0.0 # dmax modifier

	# bits in a packet
	qb = 1500
	if ((m2 > 0) && (umax > 0)) qb -= int((1500 - umax) * qlen_pkt)
	qb *= 8	
	
	# rate in bits/s
	qr = rate
	qr -= int((rate - m2) * qlen_avgr)
	qr *= 1024
	
	# queue time
	qt = qlen_base + qlen_dmax * (dmax / 1000)

	# queue length
	ql = int(qr * qt / qb)
	if (ql < qlen_min) ql = qlen_min

	return ql
}

BEGIN {
	sfq_dthresh	= 25 # use sfq for download if pktdelay set to this or lower

	limit = int("$maxrate")
	m2 = int("$m2")
	dmax = int("$dmax")
	umax = int("$umax")
	
	if (!(m2 > 0)) {
		dmax = 500
		umax = 1500
		m2 = 10
	}
	
	cqlen = ${dl_mode:+2 * }qlen(limit, m2, umax, dmax)

	printf "tc qdisc add dev $device parent 1:${classnr}0 handle ${classnr}00: "
	if (("$dir" != "down") || ((dmax > 0) && (dmax <= sfq_dthresh))) {
		print "sfq perturb 10 limit " cqlen 
	} else {
		avpkt = 1200
		min = int(limit * 1024 / 8 * 0.1)
		dqb = cqlen * 1500
		max = int(min + (dqb - min) * 0.25)
		burst = int((2 * min + max) / (3 * avpkt))
		
		print "red min " min " max " max " burst " burst " avpkt " avpkt " limit " dqb " probability 0.04 ecn"
	}
}
EOF
}

enum_classes() {
	local c="0"
	config_get classes "$1" classes
	config_get default "$1" default
	for class in $classes; do
		c="$(($c + 1))"
		config_set "${class}" classnr $c
		case "$class" in
			$default) class_default=$c;;
		esac
	done
	class_default="${class_default:-$c}"
}

cls_var() {
	local varname="$1"
	local class="$2"
	local name="$3"
	local type="$4"
	local default="$5"
	local tmp tmp1 tmp2
	config_get tmp1 "$class" "$name"
	config_get tmp2 "${class}_${type}" "$name"
	tmp="${tmp2:-$tmp1}"
	tmp="${tmp:-$tmp2}"
	export ${varname}="${tmp:-$default}"
}

start_interface() {
	local iface="$1"
	local num_imq="$2"
	config_get device "$iface" device
	config_get enabled "$iface" enabled
	[ -z "$device" -o -z "$enabled" ] && exit
	config_get upload "$iface" upload
	config_get halfduplex "$iface" halfduplex
	config_get download "$iface" download
	config_get classgroup "$iface" classgroup
	
	download="${download:-${halfduplex:+$upload}}"
	enum_classes "$classgroup"
	for dir in up${halfduplex} ${download:+down}; do
		case "$dir" in
			up)
				upload=$(($upload * 98 / 100 - 10))
				dev="$device"
				rate="$upload"
				dl_mode=""
				prefix="cls"
			;;
			down)
				add_insmod imq numdevs="$num_imq"
				config_get imqdev "$iface" imqdev
				download=$(($download * 96 / 100 - 64))
				dev="imq$imqdev"
				rate="$download"
				dl_mode=1
				prefix="d_cls"
			;;
			*) continue;;
		esac
		for class in $classes; do
			cls_var umax "$class" packetsize $dir 1500
			cls_var dmax "$class" packetdelay $dir 500
			
			cls_var maxrate "$class" limitrate $dir 100
			cls_var share "$class" linksharing $dir 0
			cls_var m2 "$class" avgrate $dir 0
			maxrate=$(($maxrate * $rate / 100))
			share=$(($share * $rate / 100))
			m2=$(($m2 * $rate / 100))
		
			config_get classnr "$class" classnr
			append ${prefix}q "$(class_main_qdisc "$dev" "$iface")" "$N"
			append ${prefix}l "$(class_leaf_qdisc "$dev" "$iface")" "$N"
			append ${prefix}f "tc filter add dev $dev parent 1: prio $classnr protocol ip handle $classnr fw flowid 1:${classnr}0" "$N"
		done
		export dev_${dir}="ifconfig $dev up txqueuelen 5 >&- 2>&-
tc qdisc del dev $dev root >&- 2>&-
tc qdisc add dev $dev root handle 1: hfsc default ${class_default}0
tc class add dev $dev parent 1: classid 1:1 hfsc sc rate ${rate}kbit ul rate ${rate}kbit"
	done
	add_insmod cls_fw
	add_insmod sch_hfsc
	add_insmod sch_sfq
	add_insmod sch_red
	cat <<EOF
${INSMOD:+$INSMOD$N}${dev_up:+$dev_up
$clsq
$clsl
$clsf
}${imqdev:+$dev_down
$d_clsq
$d_clsl
$d_clsf
}
EOF
	unset INSMOD clsq clsf clsl d_clsq d_clsl d_clsf dev_up dev_down
}

start_interfaces() {
	local C="$1"
	for iface in $INTERFACES; do
		start_interface "$iface" "$C"
	done
}

add_rules() {
	local var="$1"
	local rules="$2"
	local prefix="$3"
	
	for rule in $rules; do
		unset iptrule
		config_get target "$rule" target
		config_get target "$target" classnr
		config_get options "$rule" options
		parse_matching_rule iptrule "$rule" "$options" "$prefix" "-j MARK --set-mark $target"
		append "$var" "$iptrule" "$N"
	done
}

start_cg() {
	local cg="$1"
	local iptrules
	local pktrules
	local sizerules
	local download
	enum_classes "$cg"
	add_rules iptrules "$ctrules" "iptables -t mangle -A ${cg}_ct"
	config_get classes "$cg" classes
	for class in $classes; do
		config_get mark "$class" classnr
		config_get maxsize "$class" maxsize
		[ -z "$maxsize" -o -z "$mark" ] || {
			add_insmod ipt_length
			append pktrules "iptables -t mangle -A ${cg} -m mark --mark $mark -m length --length $maxsize: -j MARK --set-mark 0" "$N"
		}
	done
	add_rules pktrules "$rules" "iptables -t mangle -A ${cg}"
	for iface in $INTERFACES; do
		config_get classgroup "$iface" classgroup
		config_get device "$iface" device
		config_get imqdev "$iface" imqdev
		config_get dl "$iface" download
		config_get halfduplex "$iface" halfduplex
		add_insmod ipt_IMQ
		append up "iptables -t mangle -A OUTPUT -o $device -j ${cg}" "$N"
		append up "iptables -t mangle -A FORWARD -o $device -j ${cg}" "$N"
		[ -z "$dl" ] || {
			[ -z "$halfduplex" ] || {
				append down "iptables -t mangle -A POSTROUTING -o $device -j IMQ --todev $imqdev" "$N"
			}
			append down "iptables -t mangle -A PREROUTING -i $device -j ${cg}" "$N"
			append down "iptables -t mangle -A PREROUTING -i $device -j IMQ --todev $imqdev" "$N"
		}
	done
	cat <<EOF
$INSMOD
iptables -t mangle -N ${cg} >&- 2>&-
iptables -t mangle -N ${cg}_ct >&- 2>&-
${iptrules:+${iptrules}${N}iptables -t mangle -A ${cg}_ct -j CONNMARK --save-mark}
iptables -t mangle -A ${cg} -j CONNMARK --restore-mark
iptables -t mangle -A ${cg} -m mark --mark 0 -j ${cg}_ct
$pktrules
$up$N${down:+${down}$N}
EOF
	unset INSMOD
}

start_firewall() {
	add_insmod ipt_multiport
	add_insmod ipt_CONNMARK
	cat <<EOF
iptables -t mangle -F
iptables -t mangle -X
EOF
	for group in $CG; do
		start_cg $group
	done
}

C="0"
INTERFACES=""
config_load qos

C="0"
for iface in $INTERFACES; do
	export C="$(($C + 1))"
done

case "$1" in
	all)
		start_interfaces "$C"
		start_firewall
	;;
	interface)
		start_interface "$2" "$C"
	;;
	interfaces)
		start_interfaces
	;;
	firewall)
		start_firewall
	;;
esac
