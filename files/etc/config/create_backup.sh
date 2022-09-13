# Creates a backup using sysupgrade in the /tmp directory
/etc/config/opkgscript.sh write
umask go=
sysupgrade -b /tmp/backup-${HOSTNAME}-$(date +%F).tar.gz

