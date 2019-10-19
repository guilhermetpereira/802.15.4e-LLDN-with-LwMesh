ASF_DRIVERS_DIR					= $(ASF_PLATFORM_DIR)/drivers

#list C source files
CSRC	+=

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
EXTRAINCDIRS +=

include																			\
		$(ASF_DRIVERS_DIR)/adc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/cpu/makefile.mk										\
		$(ASF_DRIVERS_DIR)/ext_int/makefile.mk									\
		$(ASF_DRIVERS_DIR)/ioport/makefile.mk									\
		$(ASF_DRIVERS_DIR)/macsc/makefile.mk									\
		$(ASF_DRIVERS_DIR)/sleep/makefile.mk									\
		$(ASF_DRIVERS_DIR)/spi/makefile.mk										\
		$(ASF_DRIVERS_DIR)/tc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/twi/makefile.mk										\
		$(ASF_DRIVERS_DIR)/usart/makefile.mk									\
		$(ASF_DRIVERS_DIR)/wdt/makefile.mk