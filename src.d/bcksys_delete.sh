#!/bin/sh
# Remove deleted files from the online backup utility.

/usr/bin/find /var/www/maddog-backup.d/*/files/deleted -name "delete.*" -exec /bin/rm -f {} \;

# Remove empty directories.

/usr/bin/find /var/www/maddog-backup.d -depth -type d -name "*.d" -empty -exec /bin/rmdir {} \;
