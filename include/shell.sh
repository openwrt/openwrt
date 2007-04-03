getvar() {
	eval "echo \"\${$1}\""
}

var2file() {
	local var
	eval "var=\"\${$1}\""
	if [ -n "$var" ]; then echo "$var" > "$2"; fi
}

isset() {
	local var
	eval "var=\"\${$1}\""
	[ -n "$var" ]
}

trapret() {(
	local retvals="$1"; shift
	local cmd="$1"; shift
	for retval in $(echo $retvals); do
		local trap_$retval=1
	done
	"$cmd" "$@" || {
		local retval="$?"
		eval "trapped=\${trap_$retval}"
		[ -n "$trapped" ] || {
			return $retval
		}
	}
)}

md5s() {
	which md5sum 2>&1 >/dev/null && md5sum "$@" | awk '{print $1}' || md5 "$@"
}
