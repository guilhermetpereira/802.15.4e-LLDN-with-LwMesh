# Resources makefile
###############################################################################
PATH_RESOURCE = $(ASF_ROOT_DIR)/resources

#list C source files

CSRC +=																			\
	$(PATH_RESOURCE)/buffer/src/bmm.c											\
	$(PATH_RESOURCE)/queue/src/qmm.c

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
## Include directories for TAL
EXTRAINCDIRS +=																	\
	$(PATH_RESOURCE)															\
	$(PATH_RESOURCE)/buffer/inc													\
	$(PATH_RESOURCE)/queue/inc