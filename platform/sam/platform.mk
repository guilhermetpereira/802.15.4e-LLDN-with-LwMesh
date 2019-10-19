# ARM SAM Platform Makefile
###############################################################################

ASF_ROOT_DIR = $(ROOT_DIR)/platform
PLATFORM_DIR = $(ROOT_DIR)/platform/$(PLATFORM)

# Object files directory
#     To put object files in current directory, use a dot (.), do NOT make
#     this an empty or blank macro!
ifndef OBJDIR
OBJDIR = .
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

GCCLIB +=

# Optimization level, can be [0, 1, 2, 3, s].
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = 0

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS += $(ROOT_DIR)/board

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99
#CSTANDARD += -fgnu89-inline

# Place -D or -U options here for ASM sources
ARCH = cortex-m4
ADEFS = -DF_CPU=$(F_CPU)
ADEFS += -DARM_MATH_CM4=true
ADEFS += -Dprintf=iprintf

# Place -D or -U options here
CDEFS = $(SINGLESTEP)
CDEFS += -DCHB_DEBUG
CDEFS += -DARM_MATH_CM4=true
CPPDEFS += -Dprintf=iprintf

# Place -D or -U options here for C++ sources
CPPDEFS += -DF_CPU=$(F_CPU)UL
#CPPDEFS += -D__STDC_LIMIT_MACROS
#CPPDEFS += -D__STDC_CONSTANT_MACROS
CPPDEFS += -DARM_MATH_CM4=true
CPPDEFS += -Dprintf=iprintf

# Place -I options here
CINCS +=

ifdef STACK_TYPE
	include $(ROOT_DIR)/stack/stack.mk
endif

ifeq ($(strip $(USE_ASF)),1)
	include $(ASF_ROOT_DIR)/asf.mk
endif

include $(ROOT_DIR)/board/$(BOARD_TYPE)/board.mk
include $(ASF_ROOT_DIR)/resources/makefile.mk
include $(ASF_ROOT_DIR)/thirdparty/makefile.mk

ifndef MEM_MODEL
	MEM_MODEL = 1
endif

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing

CWARNINGS = -Wall
CWARNINGS += -Wextra
CWARNINGS += -Wshadow
CWARNINGS += -Wpointer-arith
CWARNINGS += -Wbad-function-cast
CWARNINGS += -Wcast-align
CWARNINGS += -Wsign-compare
CWARNINGS += -Waggregate-return
CWARNINGS += -Wstrict-prototypes
CWARNINGS += -Wmissing-prototypes
CWARNINGS += -Wmissing-declarations
CWARNINGS += -Wnested-externs
CWARNINGS += -Wunused
CWARNINGS += -Wundef

CPORTABILITY = -funsigned-char
CPORTABILITY += -funsigned-bitfields
#CPORTABILITY += -fpack-struct
CPORTABILITY += -fshort-enums
CPORTABILITY += -fdata-sections
#CPORTABILITY += -ffunction-sections
CPORTABILITY += -mlong-calls
CPORTABILITY += -pipe


CFLAGS += -DGCC_SAM4
CFLAGS += -DF_CPU=$(F_CPU)
CFLAGS += -g$(DEBUG)
CFLAGS += -DDEBUG=0
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += $(CPORTABILITY)
CFLAGS += $(CWARNINGS)
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)
CFLAGS += -DSIO_HUB
CFLAGS += -DTAL_TYPE=$(TAL_TYPE)
CFLAGS += -DPAL_GENERIC_TYPE=$(PAL_GENERIC_TYPE)
CFLAGS += -DPAL_TYPE=$(shell echo $(PAL_TYPE)| tr [:lower:] [:upper:])
CFLAGS += -DBOARD_TYPE=$(BOARD_TYPE)
CFLAGS += -DPLATFORM=$(PLATFORM)
CFLAGS += -DVENDOR_BOARDTYPES=1
CFLAGS += -DHIGHEST_STACK_LAYER=$(HIGHEST_STACK_LAYER)

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CPPWARNINGS += -Wall
CPPWARNINGS += -Wundef
#CPPWARNINGS += -mshort-calls
#CPPWARNINGS += -Wstrict-prototypes
#CPPWARNINGS += -Wunreachable-code
#CPPWARNINGS += -Wsign-compare

CPPORTABILITY += -funsigned-char
CPPORTABILITY += -funsigned-bitfields
#CPPORTABILITY += -fpack-struct
CPPORTABILITY += -fshort-enums
CPPORTABILITY += -fno-exceptions
#CPPORTABILITY += -fno-unit-at-a-time
CPPORTABILITY += -fdata-sections
#CPPORTABILITY += -ffunction-sections
CPPORTABILITY += -mlong-calls
CPPORTABILITY += -pipe
CPPORTABILITY += -std=c++0x


#CPPFLAGS = -g$(DEBUG) -g3 -pg
CPPFLAGS = -g3
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -O$(OPT)
CPPFLAGS += $(CPPORTABILITY)
CPPFLAGS += $(CWARNINGS)
CPPFLAGS += -Wa,-adhlns=$(<:%.cpp=$(OBJDIR)/%.lst)
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
#CPPFLAGS += $(CSTANDARD)

# Assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#ASFLAGS += $(ADEFS)
ASFLAGS += -Wa,-adhlns=$(<:%.asm=$(OBJDIR)/%.lst),-gstabs,--listing-cont-lines=100
ASFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
#ASFLAGS += $(CFLAGS)
ASFLAGS += -Wa,-g
ASFLAGS += -x assembler-with-cpp ### PUT on ALL Flags

#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
#PRINTF_LIB =
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)

MATH_LIB = -larm_cortexM4l_math -lm

# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS +=

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS += -Wl,-Map=$(TARGET).map
LDFLAGS += -Wl,--start-group
LDFLAGS += $(MATH_LIB)
#LDFLAGS += $(PRINTF_LIB)
#LDFLAGS += $(SCANF_LIB)
LDFLAGS += -Wl,--end-group
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += -Wl,--gc-sections
ARMTYPE_BASE = $(shell echo $(ARMTYPE)| tr [:upper:] [:lower:])
ifneq (, $(filter $(USE_BOOTLOADER),1))
SAMBA_PREFIX = _samba
else
SAMBA_PREFIX =
endif
ifneq (, $(filter $(ARMTYPE_BASE),sam4s))
MCU_BASE = $(shell echo $(MCU) | gawk "{print substr($$1,1,length($$1)-1)}")
LDFLAGS += -Wl,--script=$(PLATFORM_DIR)/utils/linker_scripts/$(ARMTYPE_BASE)/$(MCU_BASE)/gcc/flash$(SAMBA_PREFIX).ld
endif
ifneq (, $(filter $(ARMTYPE_BASE),sam4e))
MCU_BASE = $(shell echo $(MCU) | gawk "{print substr($$1,3,length($$1)-3)}")
LDFLAGS += -Wl,--script=$(PLATFORM_DIR)/utils/linker_scripts/$(ARMTYPE_BASE)/$(MCU_BASE)/gcc/flash$(SAMBA_PREFIX).ld
endif
ifneq (, $(filter $(ARMTYPE_BASE),sam4l))
MCU_BASE = $(ARMTYPE_BASE)$(shell echo $(MCU) | gawk "{print substr($$1,length($$1)-1,1)}")
LDFLAGS += -Wl,--script=$(PLATFORM_DIR)/utils/linker_scripts/$(ARMTYPE_BASE)/$(MCU_BASE)/gcc/flash$(SAMBA_PREFIX).ld
endif
LDFLAGS += -Wl,-Map=$(TARGET).map
LDFLAGS += -Wl,--cref
LDFLAGS += -Wl,--entry=Reset_Handler
LDFLAGS += -mcpu=$(ARCH) -mthumb
LDFLAGS += $(EXTMEMOPTS)



#============================================================================

# Define programs and commands.
PATH := $(ARM_GCC):$(PATH)
SHELL			= sh
#CROSS           ?= arm-freertos-eabi-
#CROSS           ?= arm-float-eabi-
CROSS           ?= arm-none-eabi-
AR              := $(CROSS)ar -rcsv
AS              := $(CROSS)as
CC              := $(CROSS)gcc
CPP             := $(CROSS)gcc -E
CXX             := $(CROSS)g++
LD              := $(CROSS)g++
NM              := $(CROSS)nm
OBJCOPY         := $(CROSS)objcopy
OBJDUMP         := $(CROSS)objdump
SIZE            := $(CROSS)size
GDB             := $(CROSS)gdb

REMOVE			:= rm -f
ifeq ($(os),Windows)
REMOVEDIR		:= rm  -rf
else
RMDIR           := rmdir -p --ignore-fail-on-non-empty
endif

COPY = cp
WINSHELL = cmd

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to ARM COFF:
MSG_EXTENDED_COFF = Converting to ARM Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:

# Don't use make's built-in rules and variables
MAKEFLAGS       += -rR

# Don't print 'Entering directory ...'
MAKEFLAGS       += --no-print-directory

# Function for reversing the order of a list
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

# Hide command output by default, but allow the user to override this
# by adding V=1 on the command line.
#
# This is inspired by the Kbuild system used by the Linux kernel.
ifdef V
  ifeq ("$(origin V)", "command line")
    VERBOSE = $(V)
  endif
endif
ifndef VERBOSE
  VERBOSE = 0
endif

ifeq ($(VERBOSE), 1)
  Q =
else
#  Q = @
  Q =
endif

# Define all object files.
OBJ = $(CSRC:%.c=$(OBJDIR)/%.o) $(CPPSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.asm=$(OBJDIR)/%.o)

# Define all listing files.
LST = $(CSRC:%.c=$(OBJDIR)/%.lst) $(CPPSRC:%.cpp=$(OBJDIR)/%.lst) $(ASRC:%.asm=$(OBJDIR)/%.lst)


# Compiler flags to generate dependency files.
CDEPS	= $(foreach file,$(CSRC:%.c=%.d),$(OBJDIR)/$(file))
CXXDEPS	= $(foreach file,$(CPPSRC:%.cpp=%.d),$(OBJDIR)/$(file))
DEPS	= $(CDEPS) $(CXXDEPS)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mcpu=$(ARCH) -mthumb -D=__$(shell echo $(PAL_TYPE)| tr [:lower:] [:upper:])__ -I. $(CFLAGS)
ALL_CPPFLAGS = -mcpu=$(ARCH) -mthumb -D=__$(shell echo $(PAL_TYPE)| tr [:lower:] [:upper:])__ -I. -x c++ $(CPPFLAGS)
ALL_ASFLAGS = -mcpu=$(ARCH) -mthumb -D=__$(shell echo $(PAL_TYPE)| tr [:lower:] [:upper:])__ -D__ASSEMBLY__ -I. $(ASFLAGS)








# Default target.
all: begin gccversion sizebefore build end

# Default lib target.
allib: begin gccversion sizebefore buildlib sizeafter end

# build target to build a HEX file.
build: elf hex srec bin eep lss sym

# build target to build a library.
buildlib: lib

elf: $(TARGET).elf
hex: $(TARGET).hex
srec: $(TARGET).srec
bin: $(TARGET).bin
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym
LIBNAME=lib$(TARGET).a
lib: $(LIBNAME)

$(TARGET).elf: $(OBJ)
	@echo $(MSG_LINKING) $@
	@$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE1 = $(SIZE) -Ax $(TARGET).elf
ELFSIZE2 = $(SIZE) -Bxt $(TARGET).elf

sizebefore:
#	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); \
#	$(ELFSIZE1); \
#	$(ELFSIZE2); \
#	2>/dev/null; echo; fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); \
	$(ELFSIZE1); \
	$(ELFSIZE2); \
	2>/dev/null; echo; fi

# Display compiler version information.
gccversion :
	@$(CC) --version



coff: $(TARGET).elf
	@echo $(MSG_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof



# Create final output files (.hex, .srec, .eep) from ELF output file.
%.hex: %.elf
	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature $< $@

%.srec: %.elf
	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -O srec --srec-len 128 $< $@

%.bin: %.elf
	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -O binary $< $@

%.eep: %.elf
	@echo $(MSG_EEPROM) $@
	@-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo $(MSG_EXTENDED_LISTING) $@
	@$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo $(MSG_SYMBOL_TABLE) $@
	@$(NM) -n $< > $@



# Create library from object files.
.SECONDARY : $(TARGET).a
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
	@echo $(MSG_CREATING_LIBRARY) $@
	@$(AR) $@ $(OBJ)


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo $(MSG_LINKING) $@
	@$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c
	@echo $(MSG_COMPILING) $<
	@$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp
	@echo $(MSG_COMPILING_CPP) $<
	@$(CC) -c $(ALL_CPPFLAGS) $< -o $@


# Compile: create assembler files from C source files.
%.s : %.c
	@$(CC) -S $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C++ source files.
%.s : %.cpp
	@$(CC) -S $(ALL_CPPFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.asm
	@echo $(MSG_ASSEMBLING) $<
	@$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Create preprocessed source for use in sending a bug report.
%.i : %.c
	@$(CC) -E -mcpu=$(ARCH) -I. $(CFLAGS) $< -o $@


# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).bin
	$(REMOVE) $(TARGET).srec
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	@$(REMOVE) $(CSRC:%.c=$(OBJDIR)/%.o)
	@$(REMOVE) $(CSRC:%.c=$(OBJDIR)/%.lst)
	@$(REMOVE) $(CSRC:.c=.s)
	@$(REMOVE) $(CSRC:.c=.i)
	@$(REMOVE) $(ASRC:%.asm=$(OBJDIR)/%.o)
	@$(REMOVE) $(ASRC:%.asm=$(OBJDIR)/%.lst)

	@$(REMOVE) $(CSRC:.c=$(OBJDIR)/%.d)
	@$(REMOVE) $(ASRC:.asm=$(OBJDIR)/%.d)
	$(REMOVE) .dep/*
#	$(REMOVEDIR) $(OBJDIR)

rebuild:
	make clean
	make all

rebuildlib:
	make clean
	make allib

# Include the dependency files.
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
-include $(DEPS)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex srec eep lss sym coff extcoff \
clean clean_list program debug gdb-config
