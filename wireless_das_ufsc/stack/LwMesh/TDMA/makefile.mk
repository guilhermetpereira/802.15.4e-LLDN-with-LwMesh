# Lightweight Mesh Makefile
###############################################################################

# Path variables
## Path to main project directory
LWMESH_DIR = $(STACK_DIR)
PATH_NWK = $(LWMESH_DIR)/nwk
PATH_PHY = $(LWMESH_DIR)/phy
PATH_SYS = $(LWMESH_DIR)/sys
PATH_SVR = $(LWMESH_DIR)/services

HAL_TYPE = $(strip $(MCU))
PHY_TYPE = $(strip $(TAL_TYPE))
CFLAGS += -DPHY_$(shell echo $(TAL_TYPE) | tr [:lower:] [:upper:])

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
## Include directories for general includes
#EXTRAINCDIRS +=

# Include Makefiles
include																			\
	$(PATH_NWK)/makefile.mk														\
	$(PATH_PHY)/$(PHY_TYPE)/makefile.mk											\
	$(PATH_SYS)/makefile.mk														\
	$(PATH_SVR)/makefile.mk