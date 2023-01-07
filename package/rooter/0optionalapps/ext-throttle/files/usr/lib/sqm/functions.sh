################################################################################
# (sqm) functions.sh
#
# These are all helper functions for various parts of SQM scripts. If you want
# to play around with your own shaper-qdisc-filter configuration look here for
# ready made tools, or examples start of on your own.
#
# Please note the SQM logger function is broken down into levels of logging.
# Use only levels appropriate to touch points in your script and realize the
# potential to overflow SYSLOG.
#
################################################################################
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
#   Copyright (C) 2012-2019
#       Michael D. Taht, Toke Høiland-Jørgensen, Sebastian Moeller
#       Eric Luehrsen
#
################################################################################

# Logging verbosity
VERBOSITY_SILENT=0
VERBOSITY_ERROR=1
VERBOSITY_WARNING=2
VERBOSITY_INFO=5
VERBOSITY_DEBUG=8
VERBOSITY_TRACE=10

sqm_logger() {
    local level_min
    local level_max
    local debug

    case $1 in
        ''|*[!0-9]*) LEVEL=$VERBOSITY_INFO ;; # empty or non-numbers
        *) LEVEL=$1; shift ;;
    esac

    level_min=${SQM_VERBOSITY_MIN:-$VERBOSITY_SILENT}
    level_max=${SQM_VERBOSITY_MAX:-$VERBOSITY_INFO}
    debug=${SQM_DEBUG:-0}

    if [ "$level_max" -ge "$LEVEL" ] && [ "$level_min" -le "$LEVEL" ] ; then
        if [ "$SQM_SYSLOG" -eq "1" ]; then
            logger -t SQM -s "$*"
        else
            echo "$@" >&2
        fi
    fi

    # this writes into SQM_START_LOG or SQM_STOP_LOG, log files are trucated in
    # start-sqm/stop-sqm respectively and should only take little space
    if [ "$debug" -eq "1" ]; then
        echo "$@" >> "${SQM_DEBUG_LOG}"
    fi
}

sqm_error() { sqm_logger $VERBOSITY_ERROR ERROR: "$@"; }
sqm_warn() { sqm_logger $VERBOSITY_WARNING WARNING: "$@"; }
sqm_log() { sqm_logger $VERBOSITY_INFO "$@"; }
sqm_debug() { sqm_logger $VERBOSITY_DEBUG "$@"; }
sqm_trace() { sqm_logger $VERBOSITY_TRACE "$@"; }


# Inspired from https://stackoverflow.com/questions/85880/determine-if-a-function-exists-in-bash
#fn_exists() { LC_ALL=C type $1 | grep -q 'is a function'; }
fn_exists() {
    local FN_CANDIDATE
    local CUR_LC_ALL
    local TYPE_OUTPUT
    local RET
    FN_CANDIDATE=$1
    # check that a candidate nme was given
    if [ -z "${FN_CANDIDATE}" ]; then
	sqm_error "fn_exists: no function name specified as first argument."
	return 1
    fi
    sqm_debug "fn_exists: function candidate name: ${FN_CANDIDATE}"

    # extract the textual type description
    TYPE_OUTPUT=$( LC_ALL=C type $1 2>&1 )
    sqm_debug "fn_exists: TYPE_OUTPUT: $TYPE_OUTPUT"

    # OpenWrt (2019) returns 'is a function'
    # Debian Buster/raspbian returns 'is a shell function'
    # let's just hope no Linux system reurn 'is a shell builtin function'
    echo ${TYPE_OUTPUT} | grep -q 'is .*function'
    RET=$?

    sqm_debug "fn_exists: return value: ${RET}"
    return ${RET}
}


# Transaction logging for ipt rules to allow for gracefull final teardown
ipt_log_restart() {
    [ -f "$IPT_TRANS_LOG" ] && rm -f "$IPT_TRANS_LOG"
}


# Function to negate iptables commands. Turns addition and insertion into
# deletion, and creation of new chains into deletion
# Its output has quotes around all parameters so we can preserve arguments
# containing whitespace across log file write & re-read
ipt_negate()
{
    for var in "$@"; do
        case "$var" in
            "-A"|"-I") echo -n '"-D" ' ;;
            "-N") echo -n '"-X" ' ;;
            *) echo -n "\"$var\" "  ;;
        esac
    done
    echo ""
}

ipt_log()
{
    echo "$@" >> $IPT_TRANS_LOG
}

# Split a string containing an iptables command line parameter invocation, then
# run it through ipt(). This is used to turn lines read from the log file, or
# output from ipt_negate() back into proper parameters contained in $@
ipt_run_split()
{
    eval "set -- $1"
    ipt "$@"
}

# Read the transaction log in reverse and execute using ipt to undo changes.
# Since we logged only ipt '-D' commands, ipt won't add them again to the
# transaction log, but will include them in the syslog/debug log.
ipt_log_rewind() {
    [ -f "$IPT_TRANS_LOG" ] || return 0
    sed -n '1!G;h;$p' "$IPT_TRANS_LOG" |
        while read line; do
            [ -n "$line" ] || continue
            ipt_run_split "$line"
        done
}

ipt() {
    local neg

    for var in "$@"; do
        case "$var" in
            "-A"|"-I"|"-N")
                # If the rule is an addition rule, we first run its negation,
                # then log that negation to be used by ipt_log_rewind() on
                # shutdown
                neg="$(ipt_negate "$@")"
                ipt_run_split "$neg"
                ipt_log "$neg"
                ;;
        esac
    done

    SILENT=1 ${IPTABLES} $IPTABLES_ARGS "$@"
    SILENT=1 ${IP6TABLES} $IPTABLES_ARGS "$@"
}


# wrapper to call iptables to allow debug logging
iptables_wrapper(){
    cmd_wrapper iptables ${IPTABLES_BINARY} "$@"
}

# wrapper to call ip6tables to allow debug logging
ip6tables_wrapper(){
    cmd_wrapper ip6tables ${IP6TABLES_BINARY} "$@"
}

# wrapper to call tc to allow debug logging
tc_wrapper(){
    cmd_wrapper tc ${TC_BINARY} "$@"
}

# wrapper to call ip to allow debug logging
ip_wrapper(){
    cmd_wrapper ip ${IP_BINARY} "$@"
}

# the actual command execution wrapper
cmd_wrapper(){
    # $1: the symbolic name of the command for informative output
    # $2: the name of the binary to call (potentially including the full path)
    # $3-$end: the actual arguments for $2
    local CALLERID
    local CMD_BINARY
    local LAST_ERROR
    local RET
    local ERRLOG

    CALLERID=$1 ; shift 1   # extract and remove the id string
    CMD_BINARY=$1 ; shift 1 # extract and remove the binary

    # Handle silencing of errors from callers
    ERRLOG="sqm_error"
    if [ "$SILENT" -eq "1" ]; then
        ERRLOG="sqm_debug"
        sqm_debug "cmd_wrapper: ${CALLERID}: invocation silenced by request, FAILURE either expected or acceptable."
        # The busybox shell doesn't understand the concept of an inline variable
        # only applying to a single command, so we need to reset SILENT
        # afterwards. Ugly, but it works...
        SILENT=0
    fi

    sqm_trace "cmd_wrapper: COMMAND: ${CMD_BINARY} $@"
    LAST_ERROR=$( ${CMD_BINARY} "$@" 2>&1 )
    RET=$?

    if [ "$RET" -eq "0" ] ; then
        sqm_debug "cmd_wrapper: ${CALLERID}: SUCCESS: ${CMD_BINARY} $@"
    else
        # this went south, try to capture & report more detail
        $ERRLOG "cmd_wrapper: ${CALLERID}: FAILURE (${RET}): ${CMD_BINARY} $@"
        $ERRLOG "cmd_wrapper: ${CALLERID}: LAST ERROR: ${LAST_ERROR}"
    fi

    return $RET
}


do_modules() {
    for m in $ALL_MODULES; do
        [ -d /sys/module/${m} ] || ${INSMOD} $m 2>>${OUTPUT_TARGET}
    done
}

# Write a state file to the filename given as $1. This version will extract all
# variable names defined in defaults.sh and since defaults.sh should contain all
# used variables this should be the complete set.
write_state_file() {
    local filename
    local awkscript
    awkscript='match($0, /[A-Z0-9_]+=/) {print substr($0, RSTART, RLENGTH-1)}'
    filename=$1
    shift
    awk "$awkscript" ${SQM_LIB_DIR}/defaults.sh | sort -u | while read var; do
        val=$(eval echo '$'$var)
        echo "$var=\"$val\""
    done > $filename
}

check_state_dir() {
    local PERM
    local OWNER

    if [ -z "${SQM_STATE_DIR}" ]; then
        SQM_DEBUG=0 sqm_error '$SQM_STATE_DIR is unset - check your config!'
        exit 1
    fi
    [ -d "${SQM_STATE_DIR}" ] || ( umask 077; mkdir -p "$SQM_STATE_DIR" ) || exit 1

    if [ ! -w "${SQM_STATE_DIR}" ] || [ ! -x "${SQM_STATE_DIR}" ]; then
        SQM_DEBUG=0 sqm_error "Cannot write to state dir '$SQM_STATE_DIR'"
        exit 1
    fi

    # OpenWrt doesn't have stat; for now just skip the remaining tests if it's
    # not available
    command -v stat >/dev/null 2>&1 || return 0

    PERM="0$(stat -L -c '%a' "${SQM_STATE_DIR}")"
    if [ "$((PERM & 0002))" -ne 0 ]; then
        SQM_DEBUG=0 sqm_error "State dir '$SQM_STATE_DIR' is world writable; this is unsafe, please fix"
        exit 1
    fi
    OWNER="$(stat -L -c '%u' "${SQM_STATE_DIR}")"
    if [ "$OWNER" -ne "$(id -u)" ]; then
        SQM_DEBUG=0 sqm_error "State dir '$SQM_STATE_DIR' is owned by a different user; this is unsafe, please fix"
        exit 1
    fi
}


# find the ifb device associated with a specific interface, return nothing of no
# ifb is associated with IF
get_ifb_associated_with_if() {
    local CUR_IF
    local CUR_IFB
    local TMP
    CUR_IF=$1
    # Stray ' in the comment is a fix for broken editor syntax highlighting
    CUR_IFB=$( $TC_BINARY -p filter show parent ffff: dev ${CUR_IF} | grep -o -E ifb'[^)\ ]+' )    # '
    sqm_debug "ifb associated with interface ${CUR_IF}: ${CUR_IFB}"

    # we could not detect an associated IFB for CUR_IF
    if [ -z "${CUR_IFB}" ]; then
        TMP=$( $TC_BINARY -p filter show parent ffff: dev ${CUR_IF} )
        if [ ! -z "${TMP}" ]; then
            # oops, there is output but we failed to properly parse it? Ask for a user report
            sqm_error "#---- CUT HERE ----#"
            sqm_error "get_ifb_associated_with_if failed to extrect the ifb name from:"
            sqm_error $( $TC_BINARY -p filter show parent ffff: dev ${CUR_IF} )
            sqm_error "Please report this as an issue at https://github.com/tohojo/sqm-scripts"
            sqm_error "Please copy and paste everything below the cut-here line into your issue report, thanks."
        else
            sqm_debug "Currently no ifb is associated with ${CUR_IF}, this is normal during starting of the sqm system."
        fi
    fi
    echo ${CUR_IFB}
}

ifb_name() {
    local CUR_IF
    local MAX_IF_NAME_LENGTH
    local IFB_PREFIX
    local NEW_IFB
    CUR_IF=$1
    MAX_IF_NAME_LENGTH=15
    IFB_PREFIX="ifb4"
    NEW_IFB=$( echo -n "${IFB_PREFIX}${CUR_IF}" | head -c $MAX_IF_NAME_LENGTH )

    echo ${NEW_IFB}
}

# if required
create_new_ifb_for_if() {
    local NEW_IFB
    NEW_IFB=$(ifb_name $1)
    create_ifb ${NEW_IFB}
    RET=$?
    echo $NEW_IFB
    return $RET
}


# TODO: report failures
create_ifb() {
    local CUR_IFB
    CUR_IFB=${1}
    $IP link add name ${CUR_IFB} type ifb
}

delete_ifb() {
    local CUR_IFB
    CUR_IFB=${1}
    $IP link set dev ${CUR_IFB} down
    $IP link delete ${CUR_IFB} type ifb
}


# the best match is either the IFB already associated with the current interface
# or a new named IFB
get_ifb_for_if() {
    local CUR_IF
    local CUR_IFB
    CUR_IF=$1
    # if an ifb is already associated return that
    CUR_IFB=$( get_ifb_associated_with_if ${CUR_IF} )
    [ -z "$CUR_IFB" ] && CUR_IFB=$( create_new_ifb_for_if ${CUR_IF} )
    [ -z "$CUR_IFB" ] && sqm_warn "Could not find existing IFB for ${CUR_IF}, nor create a new IFB instead..."
    echo ${CUR_IFB}
}


# Verify that a qdisc works, and optionally that it is part of a set of
# supported qdiscs. If passed a $2, this function will first check if $1 is in
# that (space-separated) list and return an error if it's not.
#
# note the ingress qdisc is different in that it requires tc qdisc replace dev
# tmp_ifb ingress instead of "root ingress"
verify_qdisc() {
    local qdisc
    local supported
    local ifb
    local root_string
    local args
    local IFB_MTU
    local found
    local randnum
    qdisc=$1
    supported="$2"
    randnum=$(tr -cd 0-9a-f < /dev/urandom 2>/dev/null | head -c 5)
    ifb=SQM_IFB_$randnum
    root_string="root" # this works for most qdiscs
    args=""
    IFB_MTU=1514

    if [ -n "$supported" ]; then
        found=0
        for q in $supported; do
            [ "$qdisc" = "$q" ] && found=1
        done
        [ "$found" -eq "1" ] || return 1
    fi
    create_ifb $ifb || return 1


    case $qdisc in
        #ingress is special
        ingress) root_string="" ;;
        #cannot instantiate tbf without args
        tbf)
    	    IFB_MTU=$( get_mtu $ifb )
	    IFB_MTU=$(( ${IFB_MTU} + 14 )) # TBF's warning is confused, it says MTU but it checks MTU + 14
	    args="limit 1 burst ${IFB_MTU} rate 1kbps"
	    ;;
    esac

    $TC qdisc replace dev $ifb $root_string $qdisc $args
    res=$?
    if [ "$res" = "0" ] ; then
        sqm_debug "QDISC $qdisc is useable."
    else
        sqm_error "QDISC $qdisc is NOT useable."
    fi
    delete_ifb $ifb
    return $res
}


get_htb_adsll_string() {
    ADSLL=""
    if [ "$LLAM" = "htb_private" -a "$LINKLAYER" != "none" ]; then
        # HTB defaults to MTU 1600 and an implicit fixed TSIZE of 256, but HTB
        # as of around 3.10.0 does not actually use a table in the kernel
        ADSLL="mpu ${STAB_MPU} linklayer ${LINKLAYER} overhead ${OVERHEAD} mtu ${STAB_MTU}"
        sqm_debug "ADSLL: ${ADSLL}"
    fi
    echo ${ADSLL}
}

get_stab_string() {
    local STABSTRING
    local TMP_LLAM
    STABSTRING=""
    TMP_LLAM=${LLAM}
    if [ "${LLAM}" = "default" -a "$QDISC" != "cake" ]; then
	sqm_debug "LLA: default link layer adjustment method for !cake is tc_stab"
	TMP_LLAM="tc_stab"
    fi

    if [ "${TMP_LLAM}" = "tc_stab" -a "$LINKLAYER" != "none" ]; then
        STABSTRING="stab mtu ${STAB_MTU} tsize ${STAB_TSIZE} mpu ${STAB_MPU} overhead ${OVERHEAD} linklayer ${LINKLAYER}"
        sqm_debug "STAB: ${STABSTRING}"
    fi
    echo ${STABSTRING}
}

# cake knows how to handle ATM and per packet overhead, so expose and use this...
get_cake_lla_string() {
    local STABSTRING
    local TMP_LLAM
    STABSTRING=""
    TMP_LLAM=${LLAM}
    if [ "${LLAM}" = "default" -a "$QDISC" = "cake" ]; then
	sqm_debug "LLA: default link layer adjustment method for cake is cake"
	TMP_LLAM="cake"
    fi

    if [ "${TMP_LLAM}" = "cake" -a "${LINKLAYER}" != "none" ]; then
        if [ "${LINKLAYER}" = "atm" ]; then
            STABSTRING="atm"
        fi

        STABSTRING="${STABSTRING} overhead ${OVERHEAD} mpu ${STAB_MPU}"

        sqm_debug "cake link layer adjustments: ${STABSTRING}"
    fi
    echo ${STABSTRING}
}


# centralize the implementation for the default sqm_start sqeuence
# the individual sqm_start function only need to do the individually
# necessary checking.
# This expects the calling script to supply both an egress() and ingress() function
# and will warn if they are missing
sqm_start_default() {
    #sqm_error "sqm_start_default"
    [ -n "$IFACE" ] || return 1

    # reset the iptables trace log
    ipt_log_restart

    if fn_exists sqm_prepare_script ; then
	sqm_debug "sqm_start_default: starting sqm_prepare_script"
        sqm_prepare_script
    else
	sqm_debug "sqm_start_default: no sqm_prepare_script function found, proceeding without."
    fi

    do_modules
    verify_qdisc $QDISC || return 1
    sqm_debug "sqm_start_default: Starting ${SCRIPT}"

    [ -z "$DEV" ] && DEV=$( get_ifb_for_if ${IFACE} )

    if [ "${UPLINK}" -ne 0 ];
    then
	CUR_DIRECTION="egress"
	fn_exists egress && egress || sqm_warn "sqm_start_default: ${SCRIPT} lacks an egress() function"
        #egress
        sqm_debug "sqm_start_default: egress shaping activated"
    else
        sqm_debug "sqm_start_default: egress shaping deactivated"
        SILENT=1 $TC qdisc del dev ${IFACE} root
    fi
    if [ "${DOWNLINK}" -ne 0 ];
    then
	CUR_DIRECTION="ingress"
	verify_qdisc ingress "ingress" || return 1
	fn_exists ingress && ingress || sqm_warn "sqm_start_default: ${SCRIPT} lacks an ingress() function"
        #ingress
        sqm_debug "sqm_start_default: ingress shaping activated"
    else
        sqm_debug "sqm_start_default: ingress shaping deactivated"
        SILENT=1 $TC qdisc del dev ${DEV} root
        SILENT=1 $TC qdisc del dev ${IFACE} ingress
    fi

    return 0
}


sqm_stop() {
    if [ "${DOWNLINK}" -ne 0 ]; then
       $TC qdisc del dev $IFACE ingress
       $TC qdisc del dev $IFACE root
       [ -n "$CUR_IFB" ] && $TC qdisc del dev $CUR_IFB root
       [ -n "$CUR_IFB" ] && sqm_debug "${0}: ${CUR_IFB} shaper deleted"
    fi

    # undo accumulated ipt commands during shutdown
    ipt_log_rewind
    # reset the iptables trace log
    ipt_log_restart

    [ -n "$CUR_IFB" ] && $IP link set dev ${CUR_IFB} down
    [ -n "$CUR_IFB" ] && $IP link delete ${CUR_IFB} type ifb
    [ -n "$CUR_IFB" ] && sqm_debug "${0}: ${CUR_IFB} interface deleted"
}

# Note this has side effects on the prio variable
# and depends on the interface global too
fc() {
    $TC filter add dev $interface protocol ip parent $1 prio $prio u32 match ip tos $2 0xfc classid $3
    prio=$(($prio + 1))
    $TC filter add dev $interface protocol ipv6 parent $1 prio $prio u32 match ip6 priority $2 0xfc classid $3
    prio=$(($prio + 1))
}


# allow better control over HTB's quantum variable
# this controlls how many bytes htb ties to deque from the current tier before
# switching to the next, if this is large mixing between pririty tiers will be
# lumpy, but at a lower CPU cost. In first approximation quantum should not be
# larger than burst.
get_htb_quantum() {
    local HTB_MTU
    local BANDWIDTH
    local DURATION_US
    local MIN_QUANTUM
    local QUANTUM
    HTB_MTU=$( get_mtu $1 )
    BANDWIDTH=$2
    DURATION_US=$3

    sqm_debug "get_htb_quantum: 1: ${1}, 2: ${2}, 3: ${3}"

    if [ -z "${DURATION_US}" ] ; then
	DURATION_US=${SHAPER_QUANTUM_DUR_US}	# the duration of the burst in microseconds
	sqm_warn "get_htb_quantum (by duration): Defaulting to ${DURATION_US} microseconds."
    fi

    if [ -n "${HTB_MTU}" -a "${DURATION_US}" -gt "0" ] ; then
    	QUANTUM=$( get_burst ${HTB_MTU} ${BANDWIDTH} ${DURATION_US} )
    fi

    if [ -z "$QUANTUM" ]; then
	MIN_QUANTUM=$(( ${MTU} + 48 ))	# add 48 bytes to MTU for the  ovehead
	MIN_QUANTUM=$(( ${MIN_QUANTUM} + 47 ))	# now do ceil(Min_BURST / 48) * 53 in shell integer arithmic
	MIN_QUANTUM=$(( ${MIN_QUANTUM} / 48 ))
	MIN_QUANTUM=$(( ${MIN_QUANTUM} * 53 ))	# for MTU 1489 to 1536 this will result in MIN_BURST = 1749 Bytes
	sqm_warn "get_htb_quantum: 0 bytes quantum will not work, defaulting to one ATM/AAL5 expanded MTU packet with overhead: ${MIN_QUANTUM}"
	echo ${MIN_QUANTUM}
    else
        echo ${QUANTUM}
    fi
}




# try to define the burst parameter in the duration required to transmit a burst
# at the configured bandwidth conceptuallly the matching quantum for this burst
# should be BURST/number_of_tiers to give each htb tier a chance to dequeue into
# each burst, but that most likely will end up with a somewhat too small quantum
# note: to get htb to report the configured burst/cburt one needs to issue the
# following command (for ifbpppoe-wan):
#	tc -d class show dev ifb4pppoe-wan
get_burst() {
    local MTU
    local BANDWIDTH
    local SHAPER_BURST_US
    local MIN_BURST
    local BURST
    MTU=$1
    BANDWIDTH=$2 # note bandwidth is always given in kbps
    SHAPER_BURST_US=$3

    sqm_debug "get_burst: 1: ${1}, 2: ${2}, 3: ${3}"

    if [ -z "${SHAPER_BURST_US}" ] ; then
	SHAPER_BURST_US=1000	# the duration of the burst in microseconds
	sqm_warn "get_burst (by duration): Defaulting to ${SHAPER_BURST_US} microseconds bursts."
    fi

    # let's assume ATM/AAL5 to be the worst case encapsulation
    #	and 48 Bytes a reasonable worst case per packet overhead
    MIN_BURST=$(( ${MTU} + 48 ))	# add 48 bytes to MTU for the  ovehead
    MIN_BURST=$(( ${MIN_BURST} + 47 ))	# now do ceil(Min_BURST / 48) * 53 in shell integer arithmic
    MIN_BURST=$(( ${MIN_BURST} / 48 ))
    MIN_BURST=$(( ${MIN_BURST} * 53 ))	# for MTU 1489 to 1536 this will result in MIN_BURST = 1749 Bytes

    # htb/tbf expect burst to be specified in bytes, while bandwidth is in kbps
    BURST=$(( ((${SHAPER_BURST_US} * ${BANDWIDTH}) / 8000) ))

    if [ ${BURST} -lt ${MIN_BURST} ] ; then
	sqm_log "get_burst (by duration): the calculated burst/quantum size of ${BURST} bytes was below the minimum of ${MIN_BURST} bytes."
	BURST=${MIN_BURST}
    fi

    sqm_debug "get_burst (by duration): BURST [Byte]: ${BURST}, BANDWIDTH [Kbps]: ${BANDWIDTH}, DURATION [us]: ${SHAPER_BURST_US}"

    echo ${BURST}
}


# Create optional burst parameters to leap over CPU interupts when the CPU is
# severly loaded. We need to be conservative though.
get_htb_burst() {
    local HTB_MTU
    local BANDWIDTH
    local DURATION_US
    local BURST
    HTB_MTU=$( get_mtu $1 )
    BANDWIDTH=$2
    DURATION_US=$3

    sqm_debug "get_htb_burst: 1: ${1}, 2: ${2}, 3: ${3}"

    if [ -z "${DURATION_US}" ] ; then
	DURATION_US=${SHAPER_BURST_DUR_US}	# the duration of the burst in microseconds
	sqm_warn "get_htb_burst (by duration): Defaulting to ${SHAPER_BURST_DUR_US} microseconds."
    fi

    if [ -n "${HTB_MTU}" -a "${DURATION_US}" -gt "0" ] ; then
    	BURST=$( get_burst ${HTB_MTU} ${BANDWIDTH} ${DURATION_US} )
    fi

    if [ -z "$BURST" ]; then
	sqm_debug "get_htb_burst: Default Burst, HTB will use MTU plus shipping and handling"
    else
        echo burst $BURST cburst $BURST
    fi
}

# For a default PPPoE link this returns 1492 just as expected but I fear we
# actually need the wire size of the whole thing not so much the MTU
get_mtu() {
    CUR_MTU=$(cat /sys/class/net/$1/mtu)
    sqm_debug "IFACE: ${1} MTU: ${CUR_MTU}"
    echo ${CUR_MTU}
}

# Set the autoflow variable to 1 if you want to limit the number of flows
# otherwise the default of 1024 will be used for all Xfq_codel qdiscs.

get_flows() {
    case $QDISC in
        codel|ns2_codel|pie|*fifo|pfifo_fast) ;;
        fq_codel|*fq_codel|sfq) echo flows $( get_flows_count ${1} ) ;;
    esac
}

get_flows_count() {
    if [ "${AUTOFLOW}" -eq "1" ]; then
        FLOWS=8
        [ $1 -gt 999 ] && FLOWS=16
        [ $1 -gt 2999 ] && FLOWS=32
        [ $1 -gt 7999 ] && FLOWS=48
        [ $1 -gt 9999 ] && FLOWS=64
        [ $1 -gt 19999 ] && FLOWS=128
        [ $1 -gt 39999 ] && FLOWS=256
        [ $1 -gt 69999 ] && FLOWS=512
        [ $1 -gt 99999 ] && FLOWS=1024
        case $QDISC in
          codel|ns2_codel|pie|*fifo|pfifo_fast) ;;
          fq_codel|*fq_codel|sfq) echo $FLOWS ;;
        esac
    else
        case $QDISC in
          codel|ns2_codel|pie|*fifo|pfifo_fast) ;;
          fq_codel|*fq_codel|sfq) echo 1024 ;;
        esac
    fi
}

# set the target parameter, also try to only take well formed inputs
# Note, the link bandwidth in the current direction (ingress or egress)
# is required to adjust the target for slow links
get_target() {
    local CUR_TARGET
    local CUR_LINK_KBPS
    CUR_TARGET=${1}
    CUR_LINK_KBPS=${2}
    [ ! -z "$CUR_TARGET" ] && sqm_debug "cur_target: ${CUR_TARGET} cur_bandwidth: ${CUR_LINK_KBPS}"
    CUR_TARGET_STRING=
    # either e.g. 100ms or auto
    CUR_TARGET_VALUE=$( echo ${CUR_TARGET} | grep -o -e \^'[[:digit:]]\+' )
    CUR_TARGET_UNIT=$( echo ${CUR_TARGET} | grep -o -e '[[:alpha:]]\+'\$ )

    AUTO_TARGET=
    UNIT_VALID=

    case $QDISC in
        *codel|*pie)
            if [ ! -z "${CUR_TARGET_VALUE}" -a ! -z "${CUR_TARGET_UNIT}" ];
            then
                case ${CUR_TARGET_UNIT} in
                    # permissible units taken from: tc_util.c get_time()
                    s|sec|secs|ms|msec|msecs|us|usec|usecs)
                        CUR_TARGET_STRING="target ${CUR_TARGET_VALUE}${CUR_TARGET_UNIT}"
                        UNIT_VALID="1"
                        ;;
                esac
            fi
            # empty field in GUI or undefined GUI variable now defaults to auto
            if [ -z "${CUR_TARGET_VALUE}" -a -z "${CUR_TARGET_UNIT}" ];
            then
                if [ ! -z "${CUR_LINK_KBPS}" ]; then
                    TMP_TARGET_US=$( adapt_target_to_slow_link $CUR_LINK_KBPS )
                    TMP_INTERVAL_STRING=$( adapt_interval_to_slow_link $TMP_TARGET_US )
                    CUR_TARGET_STRING="target ${TMP_TARGET_US}us ${TMP_INTERVAL_STRING}"
                    AUTO_TARGET="1"
                    sqm_debug "get_target defaulting to auto."
                else
                    sqm_warn "required link bandwidth in kbps not passed to get_target()."
                fi
            fi
            # but still allow explicit use of the keyword auto for backward compatibility
            case ${CUR_TARGET_UNIT} in
                auto|Auto|AUTO)
                    if [ ! -z "${CUR_LINK_KBPS}" ]; then
                        TMP_TARGET_US=$( adapt_target_to_slow_link $CUR_LINK_KBPS )
                        TMP_INTERVAL_STRING=$( adapt_interval_to_slow_link $TMP_TARGET_US )
                        CUR_TARGET_STRING="target ${TMP_TARGET_US}us ${TMP_INTERVAL_STRING}"
                        AUTO_TARGET="1"
                    else
                        sqm_warn "required link bandwidth in kbps not passed to get_target()."
                    fi
                    ;;
            esac

            case ${CUR_TARGET_UNIT} in
                default|Default|DEFAULT)
                    if [ ! -z "${CUR_LINK_KBPS}" ]; then
                        CUR_TARGET_STRING=""    # return nothing so the default target is not over-ridden...
                        AUTO_TARGET="1"
                        sqm_debug "get_target using qdisc default, no explicit target string passed."
                    else
                        sqm_warn "required link bandwidth in kbps not passed to get_target()."
                    fi
                    ;;
            esac
            if [ ! -z "${CUR_TARGET}" ]; then
                if [ -z "${CUR_TARGET_VALUE}" -o -z "${UNIT_VALID}" ]; then
                    [ -z "$AUTO_TARGET" ] && sqm_warn "${CUR_TARGET} is not a well formed tc target specifier; e.g.: 5ms (or s, us), or one of the strings auto or default."
                fi
            fi
            ;;
    esac
    echo $CUR_TARGET_STRING
}

# for low bandwidth links fq_codels default target of 5ms does not work too well
# so increase target for slow links (note below roughly 2500kbps a single packet
# will take more than 5 ms to be tansfered over the wire)
adapt_target_to_slow_link() {
    LINK_BW=$1
    # for ATM the worst case expansion including overhead seems to be 33 clls of
    # 53 bytes each
    MAX_DELAY=$(( 1000 * 1000 * 33 * 53 * 8 / 1000 )) # Max delay in us at 1kbps
    TARGET=$(( ${MAX_DELAY} / ${LINK_BW} ))  # note this truncates the decimals

    # do not change anything for fast links
    [ "$TARGET" -lt 5000 ] && TARGET=5000
    case ${QDISC} in
        *codel|pie)
            echo "${TARGET}"
            ;;
    esac
}

# codel looks at a whole interval to figure out wether observed latency stayed
# below target if target >= interval that will not work well, so increase
# interval by the same amonut that target got increased
adapt_interval_to_slow_link() {
    TARGET=$1
    case ${QDISC} in
        *codel)
            # Note this is not following codel theory to well as target should
            # be 5-10% of interval and the simple addition does not conserve
            # that relationship
            INTERVAL=$(( (100 - 5) * 1000 + ${TARGET} ))
            echo "interval ${INTERVAL}us"
            ;;
        pie)
            ## not sure if pie needs this, probably not
            #TUPDATE=$(( (30 - 20) * 1000 + ${TARGET} ))
            #echo "tupdate ${TUPDATE}us"
            ;;
    esac
}


# set quantum parameter if available for this qdisc
get_quantum() {
    case $QDISC in
        *fq_codel|fq_pie|drr) echo quantum $1 ;;
        *) ;;
    esac
}

# only show limits to qdiscs that can handle them...
# Note that $LIMIT contains the default limit
get_limit() {
    CURLIMIT=$1
    case $QDISC in
        *codel|*pie|pfifo_fast|sfq|pfifo) [ -z ${CURLIMIT} ] && CURLIMIT=${LIMIT}  # global default limit
                                          ;;
        bfifo) [ -z "$CURLIMIT" ] && [ ! -z "$LIMIT" ] && CURLIMIT=$(( ${LIMIT} * $( cat /sys/class/net/${IFACE}/mtu ) ))    # bfifo defaults to txquelength * MTU,
               ;;
        *) sqm_warn "qdisc ${QDISC} does not support a limit"
           ;;
    esac
    sqm_debug "get_limit: $1 CURLIMIT: ${CURLIMIT}"

    if [ ! -z "$CURLIMIT" ]; then
        echo "limit ${CURLIMIT}"
    fi
}

get_ecn() {
    CURECN=$1
    case ${CURECN} in
        ECN)
            case $QDISC in
                *codel|*pie|*red)
                    CURECN=ecn
                    ;;
                *)
                    CURECN=""
                    ;;
            esac
            ;;
        NOECN)
            case $QDISC in
                *codel|*pie|*red)
                    CURECN=noecn
                    ;;
                *)
                    CURECN=""
                    ;;
            esac
            ;;
        *)
            sqm_warn "ecn value $1 not handled"
            ;;
    esac
    sqm_debug "get_ECN: $1 CURECN: ${CURECN} IECN: ${IECN} EECN: ${EECN}"
    echo ${CURECN}

}

# This could be a complete diffserv implementation

diffserv() {

    interface=$1
    prio=1

    # Catchall

    $TC filter add dev $interface parent 1:0 protocol all prio 999 u32 \
        match ip protocol 0 0x00 flowid 1:12

    # Find the most common matches fast

    fc 1:0 0x00 1:12 # BE
    fc 1:0 0x20 1:13 # CS1
    fc 1:0 0x10 1:11 # IMM
    fc 1:0 0xb8 1:11 # EF
    fc 1:0 0xc0 1:11 # CS3
    fc 1:0 0xe0 1:11 # CS6
    fc 1:0 0x90 1:11 # AF42 (mosh)

    # Arp traffic
    $TC filter add dev $interface protocol arp parent 1:0 prio $prio handle 500 fw flowid 1:11

    prio=$(($prio + 1))
}

eth_setup() {
    ethtool -K $IFACE gso off
    ethtool -K $IFACE tso off
    ethtool -K $IFACE ufo off
    ethtool -K $IFACE gro off

    if [ -e /sys/class/net/$IFACE/queues/tx-0/byte_queue_limits ]; then
       for i in /sys/class/net/$IFACE/queues/tx-*/byte_queue_limits
       do
          echo $(( 4 * $( get_mtu ${IFACE} ) )) > $i/limit_max
       done
    fi
}
