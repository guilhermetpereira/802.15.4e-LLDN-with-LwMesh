ASF_DRIVERS_PIO_DIR				= $(ASF_DRIVERS_DIR)/pio

ifneq (, $(filter $(ASF_PIO_DRIVER),1))
ifneq (, $(filter $(ARMTYPE),SAM4S SAM4E))

#list C source files
CSRC	+=																		\
	$(ASF_DRIVERS_PIO_DIR)/pio.c

ifdef ASF_PIO_HANDLER_DRIVER
CSRC	+=																		\
	$(ASF_DRIVERS_PIO_DIR)/pio_handler.c
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
	$(ASF_DRIVERS_PIO_DIR)

endif
endif