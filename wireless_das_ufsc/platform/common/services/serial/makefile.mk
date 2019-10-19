ASF_COMMON_SERVICES_SERIAL_DIR			= $(ASF_COMMON_SERVICES_DIR)/serial

ifneq (, $(filter $(ASF_USART_SERVICE),1))

# List C++ source files here. (C dependencies are automatically generated.)
CSRC += $(ASF_COMMON_SERVICES_SERIAL_DIR)/usart_serial.c

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
EXTRAINCDIRS += $(ASF_COMMON_SERVICES_SERIAL_DIR)

ifneq (, $(filter $(PLATFORM),mega megarf))
	EXTRAINCDIRS += $(ASF_COMMON_SERVICES_SERIAL_DIR)/megarf_usart
else
	ifneq (, $(filter $(PLATFORM),xmega sam0 uc3))
		EXTRAINCDIRS += $(ASF_COMMON_SERVICES_SERIAL_DIR)/$(PLATFORM)_usart
	else
		EXTRAINCDIRS += $(ASF_COMMON_SERVICES_SERIAL_DIR)/$(PLATFORM)_uart
	endif
endif

endif