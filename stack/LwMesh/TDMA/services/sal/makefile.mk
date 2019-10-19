ifneq (, $(filter $(PLATFORM),mega mega_rf))
	ASF_THIRDPARTY_WIRELESS_SERVICES_TALSAL_DIR = $(ASF_THIRDPARTY_WIRELESS_SERVICES_SAL_DIR)/atmegarf_sal
	CFLAGS += -DSAL_TYPE=ATMEGARF_SAL
else
	ifneq (, $(filter $(PLATFORM),xmega))
		ASF_THIRDPARTY_WIRELESS_SERVICES_TALSAL_DIR = $(ASF_THIRDPARTY_WIRELESS_SERVICES_SAL_DIR)/atxmega_sal
		CFLAGS += -DSAL_TYPE=ATXMEGA_SAL
	else
		ifneq (, $(filter $(TAL_TYPE),AT86RF212 AT86RF212B AT86RF231 AT86RF232 AT86RF233))
			ASF_THIRDPARTY_WIRELESS_SERVICES_TALSAL_DIR = $(ASF_THIRDPARTY_WIRELESS_SERVICES_SAL_DIR)/at86rf2xx
			CFLAGS += -DSAL_TYPE=AT86RF2xx
		endif
	endif
endif

ifdef ASF_THIRDPARTY_WIRELESS_SERVICES_TALSAL_DIR

#list C source files
CSRC	+=																		\
	$(ASF_THIRDPARTY_WIRELESS_SERVICES_TALSAL_DIR)/src/sal.c

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
	$(ASF_THIRDPARTY_WIRELESS_SERVICES_SAL_DIR)/inc

endif
