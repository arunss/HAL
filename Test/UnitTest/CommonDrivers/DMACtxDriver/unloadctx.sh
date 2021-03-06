#!/bin/sh
# Modify These Variables according to your Driver
#Driver Name
module="DmaCtxDriver"

#Driver is Registerd As
device="DmaCtxDriver"

#Where to Create a Device Node
devloc="var"
#==================================================

echo "==============================\n"

# retrieve major number
major=$(gawk "\$2==\"$device\" {print \$1}" /proc/devices)

# Remove stale nodes and replace them, then give gid and perms
rm -f /${devloc}/${device}

echo "Removing a Device Entry Node"
echo rm -f /${devloc}/${device}

echo "Removing Driver\n" $module

# invoke insmod with all arguments we got
# and use a pathname, as insmod doesn't look in . by default
/sbin/rmmod ./$module.ko $* || exit 1
echo "==============================\n"





