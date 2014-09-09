#!/bin/sh
# Remove temporary files from the online backup utility.

# If a file is older than 24 hours, remove it.

/usr/bin/find /var/www/maddog-backup.d/data -mtime +1 -name "bcksys.*" -exec /bin/rm {} \;
exit 0
