[ -z "$N" ] && . /etc/functions.sh

# functions for parsing and generating json

json_init() {
	[ -n "$JSON_UNSET" ] && eval "unset $JSON_UNSET"
	export -- JSON_SEQ=0 JSON_STACK= JSON_CUR="JSON_VAR" JSON_UNSET=
}

json_add_generic() {
	local type="$1"
	local var="$2"
	local val="$3"
	local cur="${4:-$JSON_CUR}"

	export ${NO_EXPORT:+-n} -- "${cur}_$var=$val"
	export ${NO_EXPORT:+-n} -- "TYPE_${cur}_$var=$type"
	append JSON_UNSET "${cur}_$var TYPE_${cur}_$var"
	append "KEYS_${cur}" "$var"
}

json_add_table() {
	JSON_SEQ=$(($JSON_SEQ + 1))
	append JSON_STACK "$JSON_CUR"
	local table="JSON_TABLE$JSON_SEQ"
	export ${NO_EXPORT:+-n} -- "UP_$table=$JSON_CUR"
	JSON_CUR="$table"
}

json_add_object() {
	local cur="$JSON_CUR"
	json_add_table
	json_add_generic object "$1" "$JSON_CUR" "$cur"
}

json_close_object() {
	local oldstack="$JSON_STACK"
	export "KEYS_${JSON_CUR}"
	JSON_CUR="${JSON_STACK##* }"
	JSON_STACK="${JSON_STACK% *}"
	[[ "$oldstack" == "$JSON_STACK" ]] && JSON_STACK=
}

json_add_array() {
	local cur="$JSON_CUR"
	json_add_table
	json_add_generic array "$1" "$JSON_CUR" "$cur"
}

json_close_array() {
	json_close_object
}

json_add_string() {
	json_add_generic string "$1" "$2"
}

json_add_int() {
	json_add_generic int "$1" "$2"
}

json_add_boolean() {
	json_add_generic boolean "$1" "$2"
}

# functions read access to json variables

json_load() {
	eval `jshn -r "$1"`
}

json_dump() {
	jshn -w
}

json_get_type() {
	local dest="$1"
	local var="$2"
	eval "export ${NO_EXPORT:+-n} -- \"$dest=\${TYPE_${JSON_CUR}_$var}\""
}

json_get_var() {
	local dest="$1"
	local var="$2"
	eval "export ${NO_EXPORT:+-n} -- \"$dest=\${${JSON_CUR}_$var}\""
}

json_select() {
	local target="$1"
	local type

	[ -z "$1" ] && {
		JSON_CUR="JSON_VAR"
		return
	}
	[[ "$1" == ".." ]] && {
		eval "JSON_CUR=\"\${UP_$JSON_CUR}\""
		return;
	}
	json_get_type type "$target"
	case "$type" in
		object|array)
			json_get_var JSON_CUR "$target"
		;;
		*)
			echo "WARNING: Variable '$target' does not exist or is not an array/object"
		;;
	esac
}
