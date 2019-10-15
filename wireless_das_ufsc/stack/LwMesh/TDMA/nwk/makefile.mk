# LwMesh Network makefile
###############################################################################

#list C source files
CSRC +=																			\
	$(PATH_NWK)/src/nwk.c														\
	$(PATH_NWK)/src/nwkDataReq.c												\
	$(PATH_NWK)/src/nwkFrame.c													\
	$(PATH_NWK)/src/nwkGroup.c													\
	$(PATH_NWK)/src/nwkRoute.c													\
	$(PATH_NWK)/src/nwkRouteDiscovery.c											\
	$(PATH_NWK)/src/nwkRx.c														\
	$(PATH_NWK)/src/nwkSecurity.c												\
	$(PATH_NWK)/src/nwkTx.c

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
	$(PATH_NWK)/inc