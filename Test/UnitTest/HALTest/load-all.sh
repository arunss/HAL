#!/bin/sh
# Modify These Variables according to your Driver
./load-message.sh
./load-reg.sh
/sbin/insmod ./I2CDriver.ko
/sbin/insmod ./SPIDriver.ko
/sbin/insmod ./Buffermodule.ko
/sbin/insmod ./DMAModule.ko
./load-ds.sh
./load-us.sh



