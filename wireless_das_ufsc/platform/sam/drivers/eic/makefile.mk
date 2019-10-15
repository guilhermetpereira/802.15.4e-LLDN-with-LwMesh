ASF_DRIVERS_EIC_DIR				= $(ASF_DRIVERS_DIR)/eic

ifneq (, $(filter $(ASF_EIC_DRIVER),1))
ifneq (, $(filter $(ARMTYPE),SAM4L))

#list C source files
CSRC	+=																		\
	$(ASF_DRIVERS_EIC_DIR)/eic.c

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
	$(ASF_DRIVERS_EIC_DIR)

endif
endif