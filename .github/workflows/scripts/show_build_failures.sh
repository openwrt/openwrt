#!/bin/bash

original_exit_code="${ret:-1}"
log_dir_path="${1:-logs}"
context="${2:-10}"

show_make_build_errors() {
	grep -slr 'make\[[[:digit:]]\+\].*Error [[:digit:]]\+$' "$log_dir_path" | while IFS= read -r log_file; do
		printf "====== Make errors from %s ======\n" "$log_file";
		grep -r -C"$context" 'make\[[[:digit:]]\+\].*Error [[:digit:]]\+$' "$log_file" ;
	done
}

show_make_build_errors
exit "$original_exit_code"
