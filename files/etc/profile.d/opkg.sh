opkg() {
local OPKG_CMD="${1}"
local OPKG_UCI="$(uci -q get opkg.defaults."${OPKG_CMD}")"
case "${OPKG_CMD}" in
(init|uci|import|save|restore|rollback\
|upgr|export|newconf|proc|reinstall) opkg_"${@}" ;;
(*) command opkg "${@}" ;;
esac
}

opkg_init() {
uci import opkg < /dev/null
uci -q batch << EOI
set opkg.defaults='opkg'
set opkg.defaults.import='/etc/backup/installed_packages.txt'
set opkg.defaults.save='auto'
set opkg.defaults.restore='auto'
set opkg.defaults.rollback='auto'
set opkg.defaults.upgr='ai'
set opkg.defaults.export='ai'
set opkg.defaults.proc='--force-depends'
set opkg.defaults.reinstall='--force-reinstall'
set opkg.defaults.newconf='/etc'
EOI
}

opkg_uci() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
local OPKG_OPT="${OPKG_OPT:-auto}"
if ! uci -q get opkg > /dev/null
then opkg init
fi
uci -q batch << EOI
delete opkg.'${OPKG_OPT}'
set opkg.'${OPKG_OPT}'='opkg'
$(sed -r -e "s/^(.*)\s(.*)$/\
del_list opkg.'${OPKG_OPT}'.'\2'='\1'\n\
add_list opkg.'${OPKG_OPT}'.'\2'='\1'/")
commit opkg
EOI
}

opkg_import() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
if [ -e "${OPKG_OPT}" ]
then sed -n -r -e "s/\s(overlay|unknown)$/\
\tipkg/p" "${OPKG_OPT}" \
| opkg uci auto
fi
}

opkg_save() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
local OPKG_WR="$(opkg export wr)"
local OPKG_WI="$(opkg export wi)"
local OPKG_UR="$(opkg export ur)"
local OPKG_UI="$(opkg export ui)"
if uci -q get fstab.rwm > /dev/null \
&& grep -q -e "\s/rwm\s" /etc/mtab
then {
sed -e "s/$/\trpkg/" "${OPKG_WR}"
sed -e "s/$/\tipkg/" "${OPKG_WI}"
} | opkg uci init
fi
{
sed -e "s/$/\trpkg/" "${OPKG_UR}"
sed -e "s/$/\tipkg/" "${OPKG_UI}"
} | opkg uci "${OPKG_OPT}"
rm -f "${OPKG_WR}" "${OPKG_WI}" "${OPKG_UR}" "${OPKG_UI}"
}

opkg_restore() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
local OPKG_CONF="${OPKG_OPT}"
local OPKG_AI="$(opkg export ai)"
local OPKG_PR="$(opkg export pr)"
local OPKG_PI="$(opkg export pi)"
grep -x -f "${OPKG_AI}" "${OPKG_PR}" \
| opkg proc remove
grep -v -x -f "${OPKG_AI}" "${OPKG_PI}" \
| opkg proc install
rm -f "${OPKG_AI}" "${OPKG_PR}" "${OPKG_PI}"
}

opkg_rollback() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
local OPKG_CONF="${OPKG_OPT}"
local OPKG_UR="$(opkg export ur)"
local OPKG_UI="$(opkg export ui)"
local OPKG_PR="$(opkg export pr)"
local OPKG_PI="$(opkg export pi)"
if uci -q get opkg."${OPKG_CONF}" > /dev/null
then opkg restore "${OPKG_CONF}"
grep -v -x -f "${OPKG_PI}" "${OPKG_UI}" \
| opkg proc remove
grep -v -x -f "${OPKG_PR}" "${OPKG_UR}" \
| opkg proc install
fi
rm -f "${OPKG_UR}" "${OPKG_UI}" "${OPKG_PR}" "${OPKG_PI}"
}

opkg_upgr() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
case "${OPKG_OPT}" in
(ai|oi) opkg_"${OPKG_CMD}"_type ;;
esac | opkg proc upgrade
}

opkg_upgr_type() {
local OPKG_AI="$(opkg export ai)"
local OPKG_OI="$(opkg export oi)"
local OPKG_AU="$(opkg export au)"
case "${OPKG_OPT::1}" in
(a) grep -x -f "${OPKG_AI}" "${OPKG_AU}" ;;
(o) grep -x -f "${OPKG_OI}" "${OPKG_AU}" ;;
esac
rm -f "${OPKG_AI}" "${OPKG_OI}" "${OPKG_AU}"
}

opkg_export() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
local OPKG_TEMP="$(mktemp -t opkg.XXXXXX)"
case "${OPKG_OPT}" in
(ai|au) opkg_"${OPKG_CMD}"_cmd ;;
(ri|wr|wi|or|oi) opkg_"${OPKG_CMD}"_type ;;
(ur|ui) opkg_"${OPKG_CMD}"_run ;;
(pr|pi) opkg_"${OPKG_CMD}"_uci ;;
esac > "${OPKG_TEMP}"
echo "${OPKG_TEMP}"
}

opkg_export_cmd() {
local OPKG_TYPE
case "${OPKG_OPT:1}" in
(i) OPKG_TYPE="installed" ;;
(u) OPKG_TYPE="upgradable" ;;
esac
opkg list-"${OPKG_TYPE}" \
| sed -e "s/\s.*$//"
}

opkg_export_type() {
local OPKG_INFO="/usr/lib/opkg/info"
local OPKG_TYPE
case "${OPKG_OPT::1}" in
(r) OPKG_INFO="/rom${OPKG_INFO}" ;;
(w) OPKG_INFO="/rwm/upper${OPKG_INFO}" ;;
(o) OPKG_INFO="/overlay/upper${OPKG_INFO}" ;;
esac
case "${OPKG_OPT:1}" in
(r) OPKG_TYPE="c" ;;
(i) OPKG_TYPE="f" ;;
esac
find "${OPKG_INFO}" -name "*.control" \
-type "${OPKG_TYPE}" 2> /dev/null \
| sed -e "s/^.*\///;s/\.control$//"
}

opkg_export_run() {
local OPKG_AI="$(opkg export ai)"
local OPKG_RI="$(opkg export ri)"
case "${OPKG_OPT:1}" in
(r) grep -v -x -f "${OPKG_AI}" "${OPKG_RI}" ;;
(i) grep -v -x -f "${OPKG_RI}" "${OPKG_AI}" ;;
esac
rm -f "${OPKG_AI}" "${OPKG_RI}"
}

opkg_export_uci() {
local OPKG_TYPE
case "${OPKG_OPT:1}" in
(r) OPKG_TYPE="rpkg" ;;
(i) OPKG_TYPE="ipkg" ;;
esac
uci -q get opkg."${OPKG_CONF}"."${OPKG_TYPE}" \
| sed -e "s/\s/\n/g"
}

opkg_proc() {
local OPKG_OPT="${OPKG_UCI}"
local OPKG_CMD="${1:?}"
local OPKG_PKG
while read -r OPKG_PKG
do opkg "${OPKG_CMD}" "${OPKG_PKG}" ${OPKG_OPT}
done
}

opkg_reinstall() {
local OPKG_OPT="${OPKG_UCI}"
opkg install "${@}" ${OPKG_OPT}
}

opkg_newconf() {
local OPKG_OPT="${1:-${OPKG_UCI}}"
find "${OPKG_OPT}" -name "*-opkg"
}
