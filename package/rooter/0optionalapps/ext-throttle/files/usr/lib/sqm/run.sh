#!/bin/sh

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
#       Copyright (C) 2012-4 Michael D. Taht, Toke Høiland-Jørgensen, Sebastian Moeller


. /lib/functions.sh

. /etc/sqm/sqm.conf
. ${SQM_LIB_DIR}/functions.sh

ACTION="${1:-start}"
RUN_IFACE="$2"
LOCKDIR="${SQM_STATE_DIR}/sqm-run.lock"

check_state_dir
[ -d "${SQM_QDISC_STATE_DIR}" ] || ${SQM_LIB_DIR}/update-available-qdiscs

stop_statefile() {
    local f
    f="$1"
    # Source the state file prior to stopping; we need the variables saved in
    # there.
    [ -f "$f" ] && ( . "$f";
                     IFACE=$IFACE SCRIPT=$SCRIPT SQM_DEBUG=$SQM_DEBUG \
                          SQM_DEBUG_LOG=$SQM_DEBUG_LOG \
                          SQM_VERBOSITY_MAX=$SQM_VERBOSITY_MAX \
                          SQM_VERBOSITY_MIN=$SQM_VERBOSITY_MIN \
                          OUTPUT_TARGET=$OUTPUT_TARGET ${SQM_LIB_DIR}/stop-sqm )
}

start_sqm_section() {
    local section
    section="$1"
    export IFACE=$(config_get "$section" interface)

    [ -z "$RUN_IFACE" -o "$RUN_IFACE" = "$IFACE" ] || return
    [ "$(config_get "$section" enabled)" -eq 1 ] || return 0
    [ -f "${SQM_STATE_DIR}/${IFACE}.state" ] && return

    export UPLINK=$(config_get "$section" upload)
    export DOWNLINK=$(config_get "$section" download)
    export LLAM=$(config_get "$section" linklayer_adaptation_mechanism)
    export LINKLAYER=$(config_get "$section" linklayer)
    export OVERHEAD=$(config_get "$section" overhead)
    export STAB_MTU=$(config_get "$section" tcMTU)
    export STAB_TSIZE=$(config_get "$section" tcTSIZE)
    export STAB_MPU=$(config_get "$section" tcMPU)
    export ILIMIT=$(config_get "$section" ilimit)
    export ELIMIT=$(config_get "$section" elimit)
    export ITARGET=$(config_get "$section" itarget)
    export ETARGET=$(config_get "$section" etarget)
    export IECN=$(config_get "$section" ingress_ecn)
    export EECN=$(config_get "$section" egress_ecn)
    export IQDISC_OPTS=$(config_get "$section" iqdisc_opts)
    export EQDISC_OPTS=$(config_get "$section" eqdisc_opts)
    export TARGET=$(config_get "$section" target)
    export QDISC=$(config_get "$section" qdisc)
    export SCRIPT=$(config_get "$section" script)

    # The UCI names for these two variables are confusing and should have been
    # changed ages ago. For now, keep the bad UCI names but use meaningful
    # variable names in the scripts to not break user configs.
    export ZERO_DSCP_INGRESS=$(config_get "$section" squash_dscp)
    export IGNORE_DSCP_INGRESS=$(config_get "$section" squash_ingress)

    # If SQM_DEBUG or SQM_VERBOSITY_* were passed in via the command line make
    # them available to the other scripts this allows to override sqm's log
    # level as set in the GUI for quick debugging without GUI accesss.
    export SQM_DEBUG=${SQM_DEBUG:-$(config_get "$section" debug_logging)}
    export SQM_VERBOSITY_MAX=${SQM_VERBOSITY_MAX:-$(config_get "$section" verbosity)}
    export SQM_VERBOSITY_MIN

    "${SQM_LIB_DIR}/start-sqm"
}

release_lock() {
    PID=$(cat "$LOCKDIR/pid")
    if [ "$PID" -ne "$$" ]; then
        sqm_error "Trying to release lock with wrong PID $PID != $$"
        return 1
    fi

    rm -rf "$LOCKDIR"
    return 0
}

take_lock() {

    if mkdir "$LOCKDIR" 2>/dev/null; then
        sqm_trace "Acquired run lock"
        echo $$ > "$LOCKDIR/pid"

        trap release_lock 0
        return 0
    fi
    PID=$(cat "$LOCKDIR/pid")
    sqm_warn "Unable to get run lock - already held by $PID"
    return 1
}

MAX_TRIES=10
tries=$MAX_TRIES
while ! take_lock; do
    sleep 1
    tries=$((tries - 1))
    if [ "$tries" -eq 0 ]; then
        sqm_error "Giving up on getting lock after $MAX_TRIES attempts"
        sqm_error "This is a bug; please report it at https://github.com/tohojo/sqm-scripts/issues"
        sqm_error "Then, to re-enable sqm-scripts, manually remove $LOCKDIR"
        exit 1
    fi
done

if [ "$ACTION" = "stop" ]; then
    if [ -z "$RUN_IFACE" ]; then
        # Stopping all active interfaces
        for f in ${SQM_STATE_DIR}/*.state; do
            stop_statefile "$f"
        done
    else
        stop_statefile "${SQM_STATE_DIR}/${RUN_IFACE}.state"
    fi
else
    config_load sqm
    config_foreach start_sqm_section
fi
