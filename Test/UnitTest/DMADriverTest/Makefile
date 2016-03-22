##******************************************************//
##INTEL CONFIDENTIAL
##Copyright 2005-2008 Intel Corporation All Rights Reserved. 
##The source code contained or described herein and all documents 
##related to the source code ("Material") are owned by Intel 
##Corporation or its// suppliers or licensors. Title to the Material 
##remains with Intel Corporation or its suppliers and licensors. The 
##Material contains trade secrets and proprietary  and confidential
##information of Intel or its suppliers and licensors. The Material 
##is protected by worldwide copyright and trade secret laws and 
##treaty provisions. No part of the Material may be used, copied, 
##reproduced, modified, published, uploaded, posted, transmitted, 
##distributed, or disclosed in any way without Intel�s prior express
##written permission.
##
##No license under any patent, copyright, trade secret or other 
##intellectual property right is granted to or conferred upon you by 
##disclosure or delivery of the Materials, either expressly, by 
##implication, inducement, estoppel or otherwise. Any license under
## such intellectual property rights must be express and approved by
## Intel in writing
##******************************************************//

#Include Module Shipping Name
ModName := DMATest
obj-m := $(ModName).o

#Driver Source File Directory
SRCDIR = .
OBJDIR = .
INCDIR = ../../../Code/Source/Common/
COPYDIR = /home/temp/nfs_mount/nfs

#Include Source Files
C_FILES :=  $(SRCDIR)/testDMA.c

#Get Source Files
SRCS := $(A_FILES) $(C_FILES)

#Make Targets Are Defined From Here

all: dummy
	arm-none-linux-gnueabi-gcc -I $(INCDIR)  $(SRCS) -o $(obj-m)

copy: all
	-rm $(COPYDIR)/$(ModName).o
	-cp *.o $(COPYDIR)
	
clean: dummy
	$(RM) -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers

	#$(MAKE) -C $(KDIR) M=$(PWD) clean

depend .depend dep:
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif

# Declare the contents of the .PHONY variable as phony.
.PHONY: dummy