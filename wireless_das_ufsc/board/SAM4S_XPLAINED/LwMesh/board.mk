Board_LwMesh_DIR	= $(Board_DIR)/LwMesh

#list C source files
CSRC +=																			\
	$(Board_LwMesh_DIR)/src/hal.c												\
	$(Board_LwMesh_DIR)/src/halSleep.c											\
	$(Board_LwMesh_DIR)/src/halTimer.c											\
	$(Board_LwMesh_DIR)/src/halPhy.c											\
	$(Board_LwMesh_DIR)/src/halUart.c

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

EXTRAINCDIRS +=																	\
	$(Board_LwMesh_DIR)/inc
