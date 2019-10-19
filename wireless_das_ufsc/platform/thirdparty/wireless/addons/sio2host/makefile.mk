ASF_THIRDPARTY_WIRELESS_ADDONS_SIO2HOST_DIR	= $(ASF_THIRDPARTY_WIRELESS_ADDONS_DIR)/sio2host

#list C source files
ifneq (, $(filter $(SIO2HOST_CHANNEL),SIO_SERIAL))
CSRC	+=	$(ASF_THIRDPARTY_WIRELESS_ADDONS_SIO2HOST_DIR)/uart/sio2host.c
endif
ifneq (, $(filter $(SIO2HOST_CHANNEL),SIO_USB))
CSRC	+=	$(ASF_THIRDPARTY_WIRELESS_ADDONS_SIO2HOST_DIR)/usb/sio2host.c
endif
ifneq (, $(filter $(SIO2HOST_CHANNEL),SIO_FTDI))
CSRC	+=	$(ASF_THIRDPARTY_WIRELESS_ADDONS_SIO2HOST_DIR)/usb_ftdi/sio2host.c
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
	$(ASF_THIRDPARTY_WIRELESS_ADDONS_SIO2HOST_DIR)/uart							\
	$(ASF_THIRDPARTY_WIRELESS_ADDONS_SIO2HOST_DIR)/usb
