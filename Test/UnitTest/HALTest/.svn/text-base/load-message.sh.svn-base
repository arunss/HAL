#!/bin/sh
# Modify These Variables according to your Driver
#Driver Name
module="MessageQ"

#Driver is Registerd As
device="MsgDriver"

#Where to Create a Device Node
devloc="var"
#==================================================

mode="664"

echo "Inserting Driver" $module
echo "==============================\n"
# invoke insmod with all arguments we got
# and use a pathname, as insmod doesn't look in . by default
/sbin/insmod ./$module.ko $* || exit 1

# retrieve major number
major=$(gawk "\$2==\"$device\" {print \$1}" /proc/devices)

# Remove stale nodes and replace them, then give gid and perms
rm -f /${devloc}/${device}

# Create a Device Entry Point as Node
mknod /${devloc}/${device} c $major 0
echo "==============================\n"
echo "Creating a Device Entry Node"
echo mknod /${devloc}/${device} c $major 0



