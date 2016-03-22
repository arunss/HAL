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

BUILDDIR	:= $(shell pwd)

HEADERS   	= $(BUILDDIR)
SOURCE   	= $(BUILDDIR)
LIBDIR		= $(BUILDDIR)
BINDIR		= $(BUILDDIR)

#Directory containing Makefile
BUILD		= .

#
# Local Variables
#
SRCDIR = .
OBJDIR = .
INCDIR = $(HEADERS)
#TOOLPATH = .
KERNELDIR = /home/docsis/linux-2.6.24
KERNELINC = $(KERNELDIR)/include/docsis
FINDHPATH	= 


#==============================================================================
# Include configuring file
#==============================================================================


#==============================================================================
# CFLAG AFLAGS Common for entire project workspace
#==============================================================================
#CFLAGS := $(shell cat $(BUILDDIR)/compiler.opt)
#AFLAGS := $(shell cat $(BUILDDIR)/assembler.opt)

# list of generated object files.
#OBJS = 

# list of source file.
#SRCS = 

# list of source file.
#include $(BUILDDIR)/Makefile.def

#
# Make "config" the default target if there is no configuration file or
# "depend" the target if there is no top-level dependency information.
#

#CONFIGFILES	= $(BUILDDIR)/config.mk



#==============================================================================
# Obtain all cygwin path from dos path (Not Used Any Where, Having this for 
#                                                          future extension
#==============================================================================
#TOOLBASE = $(shell cygpath -u -p $(TOOLBASE))
#TOOLPATH = $(shell cygpath -u -p $(TOOLPATH))
#SYSLIBDIR = $(shell cygpath -u -p $(SYSLIBPATH))


#==============================================================================
# Get the system libraries need to be included for build
# Include ALL Sys Libs (Linking will take care which one to include)
#==============================================================================

#SYSLIBS		=$(wildcard $(SYSLIBPATH)/*.a)
#SYSLIBS		+=$(wildcard $(SYSLIBPATH)/*.obj)
#SYSLIBS		+=$(wildcard $(SYSLIBPATH)/*.o)

#SYSLIBS 	+= $(shell cat $(BUILDDIR)/extlib.inc)

#==============================================================================
# Get all Directory Components to be included for Build
#==============================================================================

BUILD_COMPONENT := $(shell cat $(BUILDDIR)/Makefile.inc)

SUBDIRS = $(BUILD_COMPONENT)




#==============================================================================
# Export all Build Variables for other make
#==============================================================================

export	     PLATFORM \
	BUILDDIR HEADERS SOURCE BINDIR LIBDIR SYSLIBDIR\
	MAKE CONFIGFILES APP_NAME LIBS SYSLIBS BUILD



#PATH += :$(shell cygpath -u -p $(TOOLBASE))
#PATH += :$(shell cygpath -u -p $(TOOLPATH))
#PATH += :$(shell cygpath -u -p $(SYSLIBPATH))


#==============================================================================
# Make Target <First Level>
#==============================================================================

# We shouldn't have this in the top Make,
# It is only requierd in the (sub)directory Make
first_rule: all


#==============================================================================
# top-level rule, to compile everything.
#==============================================================================

all:	do-it-all display_logo


#==============================================================================
# Rule to BUILD
#==============================================================================
do-it-all:    subdirs display_logo
clean:  cleanall display_logo
drivers:    bsp_drivers display_logo
apps:   bsp_application display_logo
mac_test: bsp_mac_test display_logo    

#==============================================================================
# target specific compilation.
#==============================================================================
bsp_prepare:
	echo "Configuring For Building BSP Drivers"
	-mkdir -p $(KERNELINC)
	-cp ./../Code/Source/Common/* $(KERNELINC)



#==============================================================================
# Displays the Build Logo
#==============================================================================
display_logo: dummy
	@echo " "
	@echo " "
	@echo "####  #####  ##### 		 ####  #   # ##### #     #### "
	@echo "#   # #      #   #		 #   # #   #   #   #     #   #"
	@echo "####  #####  #####	###	 ####  #   #   #   #     #   #"
	@echo "#   #     #  #			 #   # #   #   #   #     #   #"
	@echo "####  #####  #   		 ####  ##### ##### ##### #### "
	@echo " "
	@echo "BUILD SUCCESSFUL"
	@echo " "
	@echo " "
	@echo " "

#==============================================================================
# Rule to Expand Directory/Module and DO Make
#==============================================================================

#
# Expand SUBDIRS for Make
#
subdirs: $(patsubst %, _dir_%, $(SUBDIRS))

#
# for each directoy listed in the SUBDIRS do make 
#

$(patsubst %, _dir_%, $(SUBDIRS)) : dummy
	@echo "==================================================================="
	@echo "Building $(patsubst _dir_%, %, $@) "
	@echo "==================================================================="
	$(MAKE) CFLAGS="$(CFLAGS)"  AFLAGS="$(AFLAGS)" -C   \
	$(patsubst _dir_%, %, $@)/$(BUILD) copy
	@echo " "
#
# Include Modules Need to be compiled for this project
# I don't know why it is required
#

Module: dummy
	$(MAKE) CFLAGS="$(CFLAGS)" $(subst $@, _dir_$@, $@)





#==============================================================================
# Rule to Clean the Project 
#==============================================================================


cleanall: dummy
	@echo " "
	@echo " "
	@echo "Cleaning all compiler and build generated temp files from "
	@echo "following directories....."
	#@for dir in $(SUBDIRS); do echo $$dir; done
	@echo " "
	@for dir in $(SUBDIRS); \
		do echo "==========================================================="; \
		   echo "Cleaning $$dir"; \
		   echo "==========================================================="; \
		   $(MAKE) -C $$dir/$(BUILD) clean; \
		   echo " "; echo " "; done



# Declare the contents of the .PHONY variable as phony.
.PHONY: dummy