#!/bin/ash

# allow env to override nvram
nvram () {
  if [ -x /usr/sbin/nvram ]; then
    case $1 in
      get) eval "echo \${$2:-\$(command nvram get $2)}";;
      *) command nvram $*;;
    esac
  else
    case $1 in
      get) eval "echo \${$2:-\${DEFAULT_$2}}";;
      *);;
    esac
  fi  
}

