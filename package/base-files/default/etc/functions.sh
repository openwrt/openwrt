#!/bin/sh
alias debug=${DEBUG:-:}

# valid interface?
if_valid () (
  ifconfig "$1" >&- 2>&- ||
  [ "${1%%[0-9]}" = "br" ] ||
  { debug "# missing interface '$1' ignored"; false; }
)

hotplug_dev() {
	env -i ACTION=$1 INTERFACE=$2 /sbin/hotplug net
}
