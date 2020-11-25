#!/usr/bin/env bash

[ -d build ] || {
	echo "Execute as ./build/check-controllers.sh" >&2
	exit 1
}

find . -type f -name '*.lua' -path '*/controller/*' | while read controller; do
	controller="${controller#./}"
	base="${controller%%/controller/*}"

	sed -rne 's#^.*\b(cbi|form)\([[:space:]]*("([^"]*)"|\047([^\047]*)\047)[[:space:]]*[,)].*$#\1 \3\4#gp' "$controller" | while read type map; do
		model="$base/model/cbi/$map.lua"
		package="${controller##*/controller/}"; package="${package%.lua}"; package="luci.controller.${package//\//.}"

		if ! grep -sqE '\bmodule[[:space:]]*\(?[[:space:]]*("|\047|\[=*\[)'"$package" "$controller"; then
			echo "'$controller' does not contain the expected\n\t'module(\"$package\", ...)' line.\n"
		fi

		grep -sqE '\b(Form|SimpleForm)[[:space:]]*\(' "$model" && ! grep -sqE '\bMap[[:space:]]*\(' "$model" && is_form=1 || is_form=0

		if [ ! -f "$model" ]; then
			echo -e "'$controller' references $type('$map')\n\tbut expected file '$model' does not exist.\n"
		elif [ $type = "cbi" -a $is_form = 1 ]; then
			echo -e "'$controller' references $type('$map')\n\tbut '$model' looks like a Form or SimpleForm.\n"
		elif [ $type = "form" -a $is_form = 0 ]; then
			echo -e "'$controller' references $type('$map')\n\tbut '$model' does not look like a Form or SimpleForm.\n"
		fi
	done
done
