ASF_DRIVERS_NVM_DIR				= $(ASF_DRIVERS_DIR)/nvm

#list C source files
CSRC	+=																		\
	$(ASF_DRIVERS_NVM_DIR)/nvm.c

# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC += 

# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC +=																			\
	$(ASF_DRIVERS_NVM_DIR)/nvm_asm.asm

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS +=																	\
	$(ASF_DRIVERS_NVM_DIR)