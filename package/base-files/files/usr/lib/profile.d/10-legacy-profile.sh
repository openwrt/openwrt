[ -f /etc/profile.d/profile.legacy ] && cat << EOF

 === NOTICE ========================================
 /etc/profile is no longer kept across upgrades.
 Your modified copy is not in use and was saved as
 /etc/profile.d/profile.legacy
 Move your changes to /etc/profile.d/*.sh and remove
 that file to dismiss this message.
 ---------------------------------------------------

EOF
