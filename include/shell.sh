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

md5s() {
	cat "$@" | (
		md5sum 2>/dev/null ||
		md5
	) | awk '{print $1}'
}
