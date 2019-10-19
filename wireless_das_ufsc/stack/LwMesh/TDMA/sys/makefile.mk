# SYS makefile
###############################################################################

#list C source files
CSRC +=																			\
	$(PATH_SYS)/src/sys.c														\
	$(PATH_SYS)/src/sysEncrypt.c												\
	$(PATH_SYS)/src/sysTimer.c

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
## Include directories for MAC
EXTRAINCDIRS +=																	\
	$(PATH_SYS)/inc