#! /bin/sh

usage() {
   echo "Usage: ${0} <realm>"
   exit 1
}

test -n "${1}" || usage

REALM="${1}"
DIGCMD=$(command -v dig)
PRINTCMD=$(command -v printf)

validate_host() {
         echo ${@} | tr -d '\n\t\r' | grep -E '^[_0-9a-zA-Z][-._0-9a-zA-Z]*$'
}

validate_port() {
         echo ${@} | tr -d '\n\t\r' | grep -E '^[0-9]+$'
}

srv_lookup() {
   ${DIGCMD} +short srv $SRV_HOST | sort -n -k1 |
   while read line ; do
      set $line ; PORT=$(validate_port $3) ; HOST=$(validate_host $4)
      if [ -n "${HOST}" ] && [ -n "${PORT}" ]; then 
         $PRINTCMD "\thost ${HOST%.}:${PORT}\n"
      fi
   done
}

naptr_lookup() {
    ${DIGCMD} +short naptr ${REALM} | grep aaa+auth:radius.tls.tcp | sort -n -k1 |
    while read line; do
    set $line ; TYPE=$3 ; HOST=$6
    if [ "$TYPE" = "\"s\"" -o "$TYPE" = "\"S\"" ]; then
        SRV_HOST=${HOST%.}
        srv_lookup
    fi
    done
}

if test -x "${DIGCMD}" ; then
   SERVERS=$(naptr_lookup)
else
   echo "${0} requires \"dig\" command."
   exit 1
fi

if test -n "${SERVERS}" ; then
        $PRINTCMD "server dynamic_radsec.${REALM} {\n${SERVERS}\n\ttype TLS\n}\n"
        exit 0
fi

exit 10				# No server found.
