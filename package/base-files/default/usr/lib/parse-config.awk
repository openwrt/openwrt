{
	line=$0
	gsub(/^[ \t]*/, "")
	gsub(/[ \t]*$/, "")
}

$1 == "@define" {
	v[$2] = $3
}

$1 == "@ifdef" {
	if_open = 1
	if (v[$2] != "") noprint = 0
	else noprint = 1
}

$1 == "@ifndef" {
	if_open = 1
	if (v[$2] != "") noprint = 1
	else noprint = 0
}

$1 == "@else" {
	if (noprint == 1) noprint = 0
	else noprint = 1
}

($1 !~ /^@/) && (noprint != 1) {
	n=split(line "@@", a, /@@/)
	for (i=1; i<=n; i++) {
		if ((i % 2) == 1) printf a[i]
		else printf v[a[i]]
	}
	print ""
}

$1 == "@endif" {
	if_open = 0
	noprint = 0
}
