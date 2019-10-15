ifneq (, $(filter $(PLATFORM),mega mega_rf))
	ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_MGR_DIR = $(ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_DIR)/megarf
else
	ifneq (, $(filter $(PLATFORM),xmega))
		ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_MGR_DIR = $(ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_DIR)/xmega_a3u
	else
		ifneq (, $(filter $(ARMTYPE),SAM4L))
			ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_MGR_DIR = $(ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_DIR)/sam4l
		else
			ifneq (, $(filter $(PLATFORM),sam sam0))
				ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_MGR_DIR = $(ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_DIR)/$(PLATFORM)
			endif
		endif
	endif
endif

ifdef ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_MGR_DIR

#list C source files
CSRC	+=																		\
	$(ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_MGR_DIR)/sleep_mgr.c

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
	$(ASF_THIRDPARTY_WIRELESS_SERVICES_SLEEP_DIR)

endif
