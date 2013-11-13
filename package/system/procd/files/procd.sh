# procd API:
#
# procd_open_service(name, [script]):
#   Initialize a new procd command message containing a service with one or more instances
#
# procd_close_service()
#   Send the command message for the service
#
# procd_open_instance([name]):
#   Add an instance to the service described by the previous procd_open_service call
#
# procd_set_param(type, [value...])
#   Available types:
#     command: command line (array).
#     respawn info: array with 3 values $restart_timeout $fail_hreshold $max_fail
#     env: environment variable (passed to the process)
#     data: arbitrary name/value pairs for detecting config changes (table)
#     file: configuration files (array)
#     netdev: bound network device (detects ifindex changes)
#
#   No space separation is done for arrays/tables - use one function argument per command line argument
#
# procd_close_instance():
#   Complete the instance being prepared
#
# procd_kill(service, [instance]):
#   Kill a service instance (or all instances)
#

. $IPKG_INSTROOT/usr/share/libubox/jshn.sh

_PROCD_SERVICE=

_procd_call() {
	local old_cb

	json_set_namespace procd old_cb
	"$@"
	json_set_namespace $old_cb
}

_procd_wrapper() {
	while [ -n "$1" ]; do
		eval "$1() { _procd_call _$1 \"\$@\"; }"
		shift
	done
}

_procd_ubus_call() {
	local cmd="$1"

	ubus call service "$cmd" "$(json_dump)"
	json_cleanup
}

_procd_open_service() {
	local name="$1"
	local script="$2"

	_PROCD_SERVICE="$name"
	_PROCD_INSTANCE_SEQ=0

	json_init
	json_add_string name "$name"
	[ -n "$script" ] && json_add_string script "$script"
	json_add_object instances
}

_procd_close_service() {
	json_close_object
	service_triggers
	_procd_ubus_call set
}

_procd_add_array_data() {
	while [ -n "$1" ]; do
		json_add_string "" "$1"
		shift
	done
}

_procd_add_array() {
	json_add_array "$1"
	shift
	_procd_add_array_data "$@"
	json_close_array
}

_procd_add_table_data() {
	while [ -n "$1" ]; do
		local var="${1%%=*}"
		local val="${1#*=}"
		[[ "$1" == "$val" ]] && val=
		json_add_string "$var" "$val"
		shift
	done
}

_procd_add_table() {
	json_add_object "$1"
	shift
	_procd_add_table_data "$@"
	json_close_object
}

_procd_open_instance() {
	local name="$1"; shift

	_PROCD_INSTANCE_SEQ="$(($_PROCD_INSTANCE_SEQ + 1))"
	name="${name:-instance$_PROCD_INSTANCE_SEQ}"
	json_add_object "$name"
}

_procd_open_trigger() {
	json_add_array "triggers"
}

_procd_open_validate() {
	json_add_array "validate"
}

_procd_set_param() {
	local type="$1"; shift

	case "$type" in
		env|data)
			_procd_add_table "$type" "$@"
		;;
		command|netdev|file|respawn)
			_procd_add_array "$type" "$@"
		;;
		nice)
			json_add_int "$type" "$1"
		;;
	esac
}

_procd_add_config_trigger() {
	json_add_array
	_procd_add_array_data "$1"
	shift

	json_add_array
	_procd_add_array_data "if"

	json_add_array
	_procd_add_array_data "eq" "package" "$1"
	shift
	json_close_array

	json_add_array
	_procd_add_array_data "run_script" "$@"
	json_close_array

	json_close_array

	json_close_array
}

_procd_add_reload_trigger() {
	local script=$(readlink "$initscript")
	local name=$(basename ${script:-$initscript})

	_procd_open_trigger
	_procd_add_config_trigger "config.change" $1 /etc/init.d/$name reload
	_procd_close_trigger
}

_procd_add_validation() {
	_procd_open_validate
	$@
	_procd_close_validate
}

_procd_append_param() {
	local type="$1"; shift

	json_select "$type"
	case "$type" in
		env|data)
			_procd_add_table_data "$@"
		;;
		command|netdev|file|respawn)
			_procd_add_array_data "$@"
		;;
	esac
	json_select ..
}

_procd_close_instance() {
	json_close_object
}

_procd_close_trigger() {
	json_close_array
}

_procd_close_validate() {
	json_close_array
}

_procd_add_instance() {
	_procd_open_instance
	_procd_set_param command "$@"
	_procd_close_instance
}

_procd_kill() {
	local service="$1"
	local instance="$2"

	json_init
	[ -n "$service" ] && json_add_string name "$service"
	[ -n "$instance" ] && json_add_string instance "$instance"
	_procd_ubus_call delete
}

uci_validate_section()
{
	local error=0

	[ "$4" = "" ] && return 1
	[ "$3" = "" ] && {
		json_add_object
		json_add_string "package" "$1"
		json_add_string "type" "$2"
		json_add_object "data"

		shift; shift; shift

		while [ -n "$1" ]; do
			local tmp=${1#*:}
			json_add_string "${1%%:*}" "${tmp%%:*}"
			shift
		done

		json_close_object
		json_close_object
		return 0
	}

	local section="${3}"
	config_load "${1}"
	shift; shift; shift

	while [ -n "$1" ]; do
		local name=${1%%:*}
		local tmp=${1#*:}
		local type=${tmp%%:*}
		local default=""

		[ "$tmp" = "$type" ] || default=${tmp#*:}

		shift
		config_get "${name}" "${section}" "${name}"
		eval val=\$$name

		[ "$type" = "bool" ] && {
			case "$val" in
			1|on|true|enabled) val=1;;
			0|off|false|disabled) val=0;;
			*) val="";;
			esac
		}
		[ -z "$val" ] && val=${default}
		eval $name=\"$val\"
		[ -z "$val" ] || {
			/sbin/validate_data "${type}" "${val}"
			[ $? -eq 0 ] || error="$((error + 1))"
		}
	done

	return $error
}

_procd_wrapper \
	procd_open_service \
	procd_close_service \
	procd_add_instance \
	procd_add_config_trigger \
	procd_add_reload_trigger \
	procd_open_trigger \
	procd_close_trigger \
	procd_open_instance \
	procd_close_instance \
	procd_set_param \
	procd_append_param \
	procd_add_validation \
	procd_kill
