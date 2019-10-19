ASF_COMMON_UTILS_STDIO_DIR			= $(ASF_COMMON_UTILS_DIR)/stdio

ifneq (, $(filter $(USE_SIO_MANAGER),1))
#list C source files
CSRC	+=																		\
	$(ASF_COMMON_UTILS_STDIO_DIR)/read.c										\
	$(ASF_COMMON_UTILS_STDIO_DIR)/write.c

ifneq (, $(filter $(SIO2HOST_CHANNEL),SIO_USB))
#list C source files
CSRC	+=																		\
	$(ASF_COMMON_UTILS_STDIO_DIR)/stdio_usb/stdio_usb.c
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
	$(ASF_COMMON_UTILS_STDIO_DIR)												\
	$(ASF_COMMON_UTILS_STDIO_DIR)/stdio_usb										\
	$(ASF_COMMON_UTILS_STDIO_DIR)/stdio_serial

endif