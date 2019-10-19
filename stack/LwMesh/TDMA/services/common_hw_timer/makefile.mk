LDFLAGS += -Wl,--start-group -L$(ASF_THIRDPARTY_WIRELESS_SERVICES_COMMON_HW_TIMMER_DIR)/$(PLATFORM)/lib

ifneq (, $(filter $(PLATFORM),mega mega_rf))
ifneq (, $(filter $(TAL_TYPE),atmegarfr2 ATMEGARFR2))
LDFLAGS += -lmegarfr2_lib_hw_timer
else
LDFLAGS += -lmegarfa1_lib_hw_timer
endif
endif
ifneq (, $(filter $(PLATFORM),xmega))
LDFLAGS += -lxmegaa3_hw_timer_lib
endif
ifneq (, $(filter $(PLATFORM),sam))
LDFLAGS += -lsam4l_lib_hw_timer
endif
ifneq (, $(filter $(PLATFORM),uc3))
LDFLAGS += -luc3_lib_hw_timer
endif

LDFLAGS += -Wl,--end-group

#list C source files
CSRC	+=																		\
	$(ASF_THIRDPARTY_WIRELESS_SERVICES_COMMON_HW_TIMMER_DIR)/$(PLATFORM)/hw_timer.c

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
	$(ASF_THIRDPARTY_WIRELESS_SERVICES_COMMON_HW_TIMMER_DIR)					\
	$(ASF_THIRDPARTY_WIRELESS_SERVICES_COMMON_HW_TIMMER_DIR)/$(PLATFORM)