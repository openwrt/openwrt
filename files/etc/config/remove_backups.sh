# Deletes the backup* files in the /tmp directory
find /tmp -type f -name "backup*.tar.gz" -exec rm -f {} \;
