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

#Include Common Definations
include ../../CommonMakeDefn/Makefile.def

#==============================================================================

#Include Module Shipping Name
ModName := DMACtxDriver
obj-m := $(ModName).o

#Driver Source File Directory
SRCDIR := ../../../../Code/Source/DMACtxDriver

#Include Module Files
$(ModName)-y := $(SRCDIR)/DOCSHALDMACtxDrv.o
$(ModName)-y += $(SRCDIR)/DOCSHALDMACtxCore.o

#Make Targets Are Defined From Here

all: dummy
	$(MAKE) -C $(KDIR) -I $(PWD) -I $(INC) M=$(PWD) modules

clean: dummy
	$(MAKE) -C $(KDIR) -I $(PWD) M=$(PWD) clean
	$(RM) -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers
	$(RM) -rf $(SRCDIR)/*.o $(SRCDIR)/.*.cmd $(SRCDIR)/*~

help: 	dummy
	@echo "Available Targets"
	@echo "=============================================================="
	@echo "make           :Compile $(ModName) module"	
	@echo "make all       :Compile $(ModName) module"	
	@echo "make clean     :Clean $(ModName) module"	
	@echo "make clean all :Clean & then Compile $(ModName) module"
	@echo "make test      :Compile test application for $(ModName) module"	
	@echo "=============================================================="	
	@echo

depend .depend dep: dummy
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

# Declare the contents of the .PHONY variable as phony.
.PHONY: dummy