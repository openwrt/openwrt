#!/usr/bin/env bash

DIR="$1"
FOUND=0

if [ -d "$DIR" ]; then
	DIR="$(cd "$DIR"; pwd)"
else
	echo "Usage: $0 toolchain-dir"
	exit 1
fi

echo -n "Locating cpp ... "
for bin in bin usr/bin usr/local/bin; do
	for cmd in "$DIR/$bin/"*-cpp; do
		if [ -x "$cmd" ]; then
			echo "$cmd"
			CPP="$cmd"
			break
		fi
	done
done

if [ ! -x "$CPP" ]; then
	echo "Can't locate a cpp executable in '$DIR' !"
	exit 1
fi

for lib in $(STAGING_DIR="$dir" "$CPP" -x c -v /dev/null 2>&1 | sed -ne 's#:# #g; s#^LIBRARY_PATH=##p'); do
	if [ -d "$lib" ]; then
		grep -qs "STAGING_DIR" "$lib/specs" && rm -f "$lib/specs"
		if [ $FOUND -lt 1 ]; then
			echo -n "Patching specs ... "
			STAGING_DIR="$dir" "$CPP" -dumpspecs | awk '
				mode ~ "link" {
					sub("%{L.}", "%{L*} -L %:getenv(STAGING_DIR /usr/lib) -rpath-link %:getenv(STAGING_DIR /usr/lib)")
				}
				mode ~ "cpp" {
					$0 = $0 " -idirafter %:getenv(STAGING_DIR /usr/include)"
				}
				{
					print $0
					mode = ""
				}
				/^\*cpp:/ {
					mode = "cpp"
				}
				/^\*link.*:/ {
					mode = "link"
				}
			' > "$lib/specs"
			echo "ok"
			FOUND=1
		fi
	fi
done

if [ $FOUND -lt 1 ]; then
	echo "Failed to locate library directory!"
	exit 1
else
	echo "Toolchain successfully patched."
	exit 0
fi
