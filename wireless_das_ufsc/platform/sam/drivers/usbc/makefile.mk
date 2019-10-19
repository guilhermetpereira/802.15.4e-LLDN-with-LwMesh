ASF_DRIVERS_USBC_DIR			= $(ASF_DRIVERS_DIR)/usbc

ifneq (, $(filter $(ASF_USB_DRIVER),1))
ifneq (, $(filter $(ARMTYPE),SAM4L))

CDEFS += -DUDD_ENABLE
CPPDEFS += -DUDD_ENABLE

#list C source files
CSRC	+=																		\
	$(ASF_DRIVERS_USBC_DIR)/usbc_device.c

ifeq ($(strip $(ASF_USB_HOST_DRIVER)),1)
CSRC	+=																		\
	$(ASF_DRIVERS_USBC_DIR)/usbc_host.c
endif

# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC += 

# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC +=

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS +=																	\
	$(ASF_DRIVERS_USBC_DIR)

endif
endif