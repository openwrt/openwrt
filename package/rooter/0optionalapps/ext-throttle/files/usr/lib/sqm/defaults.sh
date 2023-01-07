# You need to jiggle these parameters. Note limits are tuned towards a <10Mbit uplink <60Mbup down

[ -z "$SCRIPT" ] && SCRIPT=
[ -z "$UPLINK" ] && UPLINK=2302
[ -z "$DOWNLINK" ] && DOWNLINK=14698
[ -z "$IFACE" ] && IFACE=eth0
[ -z "$QDISC" ] && QDISC=fq_codel
[ -z "$LLAM" ] && LLAM="default"
[ -z "$LINKLAYER" ] && LINKLAYER="none"
[ -z "$OVERHEAD" ] && OVERHEAD=0
[ -z "$STAB_MTU" ] && STAB_MTU=2047
[ -z "$STAB_MPU" ] && STAB_MPU=0
[ -z "$STAB_TSIZE" ] && STAB_TSIZE=512
[ -z "$AUTOFLOW" ] && AUTOFLOW=0
[ -z "$LIMIT" ] && LIMIT=1001	# sane global default for *LIMIT for fq_codel on a small memory device
[ -z "$ILIMIT" ] && ILIMIT=
[ -z "$ELIMIT" ] && ELIMIT=
[ -z "$ITARGET" ] && ITARGET=
[ -z "$ETARGET" ] && ETARGET=
[ -z "$IECN" ] && IECN="ECN"
[ -z "$EECN" ] && EECN="ECN"
# These two used to be called something else; preserve backwards compatibility
[ -z "$ZERO_DSCP_INGRESS" ] && ZERO_DSCP_INGRESS="${ZERO_DSCP:-${SQUASH_DSCP:-1}}"
[ -z "$IGNORE_DSCP_INGRESS" ] && IGNORE_DSCP_INGRESS="${IGNORE_DSCP:-${SQUASH_INGRESS:-1}}"

[ -z "$IQDISC_OPTS" ] && IQDISC_OPTS=""
[ -z "$EQDISC_OPTS" ] && EQDISC_OPTS=""

# handling of specific important binaries
[ -z "$TC" ] && TC=tc_wrapper
[ -z "$TC_BINARY" ] && TC_BINARY=$(command -v tc)
[ -z "$IP" ] && IP=ip_wrapper
[ -z "$IP_BINARY" ] && IP_BINARY=$(command -v ip)
[ -z "$IPTABLES" ] && IPTABLES=iptables_wrapper
[ -z "$IPTABLES_BINARY" ] && IPTABLES_BINARY=$(command -v iptables)
[ -z "$IP6TABLES" ] && IP6TABLES=ip6tables_wrapper
[ -z "$IP6TABLES_BINARY" ] && IP6TABLES_BINARY=$(command -v ip6tables)
[ -z "$IPTABLES_ARGS" ] && IPTABLES_ARGS="-w 1"


# Try modprobe first, fall back to insmod
if [ -z "$INSMOD" ]; then
	INSMOD=$(command -v modprobe)
	if [ -n "$INSMOD" ]; then
		INSMOD="${INSMOD} -q"
	else
		INSMOD=$(command -v insmod)
	fi
fi

[ -z "$TARGET" ] && TARGET="5ms"
[ -z "$IPT_MASK" ] && IPT_MASK="0xff" # to disable: set mask to 0xffffffff
#sm: we need the functions above before trying to set the ingress IFB device
#sm: *_CAKE_OPTS should contain the diffserv keyword for cake
[ -z "$INGRESS_CAKE_OPTS" ] && INGRESS_CAKE_OPTS="diffserv3"
[ -z "$EGRESS_CAKE_OPTS" ] && EGRESS_CAKE_OPTS="diffserv3"

[ -z "$CUR_DIRECTION" ] && CUR_DIRECTION="NONE"


# HTB without a sufficiently large burst/cburst value is a bit CPU hungry
# so allow to specify the permitted burst in the time domain (microseconds)
# so the user has a feeling for the associated worst case latency cost
# set to zero to use htb default butst of one MTU
[ -z "$SHAPER_BURST_DUR_US" ] && SHAPER_BURST_DUR_US=1000
[ -z "$ISHAPER_BURST_DUR_US" ] && ISHAPER_BURST_DUR_US=$SHAPER_BURST_DUR_US
[ -z "$ESHAPER_BURST_DUR_US" ] && ESHAPER_BURST_DUR_US=$SHAPER_BURST_DUR_US

# use the same logic for the calculation of htb's quantum
# quantum controlls how many bytes htb tries to deque from the current tier
# before switching tiers.
[ -z "$SHAPER_QUANTUM_DUR_US" ] && SHAPER_QUANTUM_DUR_US=$SHAPER_BURST_DUR_US
[ -z "$ISHAPER_QUANTUM_DUR_US" ] && ISHAPER_QUANTUM_DUR_US=$SHAPER_QUANTUM_DUR_US
[ -z "$ESHAPER_QUANTUM_DUR_US" ] && ESHAPER_QUANTUM_DUR_US=$SHAPER_QUANTUM_DUR_US


# Logging verbosity
VERBOSITY_SILENT=0
VERBOSITY_ERROR=1
VERBOSITY_WARNING=2
VERBOSITY_INFO=5
VERBOSITY_DEBUG=8
VERBOSITY_TRACE=10
[ -z "$SQM_VERBOSITY_MAX" ] && SQM_VERBOSITY_MAX=$VERBOSITY_INFO
# For silencing only errors
[ -z "$SQM_VERBOSITY_MIN" ] && SQM_VERBOSITY_MIN=$VERBOSITY_SILENT

[ -z "$SQM_DEBUG" ] && SQM_DEBUG=0
if [ "$SQM_DEBUG" -eq "1" ]
then
    SQM_DEBUG_STEM="${SQM_STATE_DIR}/${IFACE}"
    SQM_START_LOG="${SQM_DEBUG_STEM}.start-sqm.log"
    SQM_STOP_LOG="${SQM_DEBUG_STEM}.stop-sqm.log"
    [ -z "SQM_DEBUG_LOG" ] &&  SQM_DEBUG_LOG="${SQM_DEBUG_STEM}.debug.log"
    OUTPUT_TARGET="${SQM_DEBUG_LOG}"
else
    OUTPUT_TARGET="/dev/null"
fi


# Can be overridden by callers that want to silence error output for a
# particular command
SILENT=0

# Transaction log for unwinding ipt rules
IPT_TRANS_LOG="${SQM_STATE_DIR}/${IFACE}.iptables.log"

# These are the modules that do_modules() will attempt to load
ALL_MODULES="act_ipt sch_$QDISC sch_ingress act_mirred cls_fw cls_flow cls_u32 sch_htb sch_hfsc"
