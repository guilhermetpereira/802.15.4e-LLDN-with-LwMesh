ASF_COMMON_SERVICES_CLOCK_DIR	= $(ASF_COMMON_SERVICES_DIR)/clock

#list C source files
ifneq (, $(filter $(PLATFORM),mega_rf xmega))
CSRC += $(ASF_COMMON_SERVICES_CLOCK_DIR)/$(PLATFORM)/sysclk.c
else
#CSRC += $(ASF_COMMON_SERVICES_CLOCK_DIR)/$(shell echo $(ARMTYPE) | tr [:upper:] [:lower:])/sysclk.c
CSRC += $(ASF_COMMON_SERVICES_CLOCK_DIR)/sam4l/sysclk.c
endif

ifneq (, $(filter $(shell echo $(ARMTYPE) | tr [:upper:] [:lower:]),sam4l uc3l))
	CSRC +=																							\
		$(ASF_COMMON_SERVICES_CLOCK_DIR)/sam4l/dfll.c
endif

ifneq (, $(filter $(shell echo $(ARMTYPE) | tr [:upper:] [:lower:]),sam4l uc3l uc3c uc3d))
	CSRC +=																							\
		$(ASF_COMMON_SERVICES_CLOCK_DIR)/sam4l/osc.c	\
		$(ASF_COMMON_SERVICES_CLOCK_DIR)/sam4l/pll.c
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
ifneq (, $(filter $(PLATFORM),mega_rf xmega))
EXTRAINCDIRS += 																\
	$(ASF_COMMON_SERVICES_CLOCK_DIR)											\
	$(ASF_COMMON_SERVICES_CLOCK_DIR)/$(PLATFORM)
else
EXTRAINCDIRS += 																\
	$(ASF_COMMON_SERVICES_CLOCK_DIR)											\
	$(ASF_COMMON_SERVICES_CLOCK_DIR)/sam4l
endif