#!/bin/sh
# Remove deleted files using bcwipe from the online backup utility.

/usr/bin/find /scsi/www/backup_system/*/files/deleted -name "delete.*" -exec /bin/rm -f {} \;

# Remove empty directories.

/usr/bin/find /scsi/www/backup_system -depth -type d -name "*.d" -empty -exec /bin/rmdir {} \;
