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
		$(ASF_DRIVERS_DIR)/acc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/adc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/ast/makefile.mk										\
		$(ASF_DRIVERS_DIR)/bpm/makefile.mk										\
		$(ASF_DRIVERS_DIR)/chipid/makefile.mk									\
		$(ASF_DRIVERS_DIR)/flashcalw/makefile.mk								\
		$(ASF_DRIVERS_DIR)/gmac/makefile.mk										\
		$(ASF_DRIVERS_DIR)/gpio/makefile.mk										\
		$(ASF_DRIVERS_DIR)/efc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/eic/makefile.mk										\
		$(ASF_DRIVERS_DIR)/pdca/makefile.mk										\
		$(ASF_DRIVERS_DIR)/pio/makefile.mk										\
		$(ASF_DRIVERS_DIR)/pmc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/spi/makefile.mk										\
		$(ASF_DRIVERS_DIR)/tc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/twi/makefile.mk										\
		$(ASF_DRIVERS_DIR)/twim/makefile.mk										\
		$(ASF_DRIVERS_DIR)/udp/makefile.mk										\
		$(ASF_DRIVERS_DIR)/usbc/makefile.mk										\
		$(ASF_DRIVERS_DIR)/uart/makefile.mk										\
		$(ASF_DRIVERS_DIR)/usart/makefile.mk									\
		$(ASF_DRIVERS_DIR)/wdt/makefile.mk