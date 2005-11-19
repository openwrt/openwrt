#!/bin/ash

# allow env to override nvram
nvram () {
  if [ -x /usr/sbin/nvram ]; then
    case $1 in
      get) eval "echo \${$2:-\$(/usr/sbin/nvram get $2)}";;
      *) /usr/sbin/nvram $*;;
    esac
  else
    case $1 in
      get) eval "echo \${$2:-\${DEFAULT_$2}}";;
      *);;
    esac
  fi  
}

