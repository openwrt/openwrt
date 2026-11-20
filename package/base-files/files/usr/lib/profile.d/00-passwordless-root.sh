if grep -Esq '^root::' /etc/shadow ; then
cat << EOF

 === WARNING! =====================================
 There is no root password defined on this device!
 Use the "passwd" command to set up a new password
 in order to prevent unauthorized SSH logins.
 --------------------------------------------------

EOF
fi
