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
#     limits: resource limits (passed to the process)
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

	[ -n "$PROCD_DEBUG" ] && json_dump >&2
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
		env|data|limits)
			_procd_add_table "$type" "$@"
		;;
		command|netdev|file|respawn|watch)
			_procd_add_array "$type" "$@"
		;;
		error)
			json_add_array "$type"
			json_add_string "" "$@"
			json_close_array
		;;
		nice)
			json_add_int "$type" "$1"
		;;
	esac
}

_procd_add_interface_trigger() {
	json_add_array
	_procd_add_array_data "$1"
	shift

	json_add_array
	_procd_add_array_data "if"

	json_add_array
	_procd_add_array_data "eq" "interface" "$1"
	shift
	json_close_array

	json_add_array
	_procd_add_array_data "run_script" "$@"
	json_close_array

	json_close_array

	json_close_array
}

_procd_add_network_trigger() {
	local script=$(readlink "$initscript")
	local name=$(basename ${script:-$initscript})

	_procd_open_trigger
	_procd_add_interface_trigger "interface.*" $1 /etc/init.d/$name reload
	_procd_close_trigger
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
	local file

	_procd_open_trigger
	for file in "$@"; do
		_procd_add_config_trigger "config.change" "$file" /etc/init.d/$name reload
	done
	_procd_close_trigger
}

_procd_add_validation() {
	_procd_open_validate
	$@
	_procd_close_validate
}

_procd_append_param() {
	local type="$1"; shift
	local _json_no_warning=1

	json_select "$type"
	[ $? = 0 ] || {
		_procd_set_param "$type" "$@"
		return
	}
	case "$type" in
		env|data|limits)
			_procd_add_table_data "$@"
		;;
		command|netdev|file|respawn|watch)
			_procd_add_array_data "$@"
		;;
		error)
			json_add_string "" "$@"
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
	local _package="$1"
	local _type="$2"
	local _name="$3"
	local _error
	shift; shift; shift
	local _result=`/sbin/validate_data "$_package" "$_type" "$_name" "$@" 2> /dev/null`
	_error=$?
	eval "$_result"
	[ "$_error" = "0" ] || `/sbin/validate_data "$_package" "$_type" "$_name" "$@" 1> /dev/null`
	return $_error
}

_procd_wrapper \
	procd_open_service \
	procd_close_service \
	procd_add_instance \
	procd_add_config_trigger \
	procd_add_reload_trigger \
	procd_add_interface_trigger \
	procd_add_network_trigger \
	procd_open_trigger \
	procd_close_trigger \
	procd_open_instance \
	procd_close_instance \
	procd_open_validate \
	procd_close_validate \
	procd_set_param \
	procd_append_param \
	procd_add_validation \
	procd_kill
