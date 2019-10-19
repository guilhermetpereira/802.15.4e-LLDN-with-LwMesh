ASF_UTILS_SYSCALLS_DIR					= $(ASF_UTILS_DIR)/syscalls

#list C source files
CSRC	+=																		\
	$(ASF_UTILS_SYSCALLS_DIR)/gcc/syscalls.c									\
	$(ASF_UTILS_SYSCALLS_DIR)/gcc/hardfault_handler_c.c							\

# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC += 

# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC	+=																		\
	$(ASF_UTILS_SYSCALLS_DIR)/gcc/hardfault_handler.asm

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS += 																\
	$(ASF_UTILS_SYSCALLS_DIR)/gcc