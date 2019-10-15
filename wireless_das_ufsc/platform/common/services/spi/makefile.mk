ifneq (, $(filter $(ASF_SPI_SERVICE),1))

ASF_COMMON_SERVICES_SPI_DIR				= $(ASF_COMMON_SERVICES_DIR)/spi

#list C source files
ifneq (, $(filter $(PLATFORM),mega))
CSRC	+=																		\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)rf_spi/spi_master.c				\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)rf_usart_spi/usart_spi.c
else
CSRC	+=																		\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)_spi/spi_master.c					\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)_usart_spi/usart_spi.c
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
ifneq (, $(filter $(PLATFORM),mega))
EXTRAINCDIRS += 																\
	$(ASF_COMMON_SERVICES_SPI_DIR)												\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)rf_spi							\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)rf_usart_spi
else
EXTRAINCDIRS += 																\
	$(ASF_COMMON_SERVICES_SPI_DIR)												\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)_spi								\
	$(ASF_COMMON_SERVICES_SPI_DIR)/$(PLATFORM)_usart_spi
endif

endif