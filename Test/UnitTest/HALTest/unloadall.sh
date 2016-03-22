#!/bin/sh
# Modify These Variables according to your Driver
#Driver Name
/sbin/rmmod ./UsDriver.ko
/sbin/rmmod ./DsDriver.ko
/sbin/rmmod ./DMAModule.ko
/sbin/rmmod ./Buffermodule.ko
/sbin/rmmod ./SPIDriver.ko
/sbin/rmmod ./I2CDriver.ko
/sbin/rmmod ./MessageQ.ko
/sbin/rmmod ./HALReg.ko




