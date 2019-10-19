# AVR 8 XMega Platform Makefile
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
ifndef OPT
OPT = s
endif

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

# Place -D or -U options here
CDEFS = $(SINGLESTEP)
CDEFS += -DF_CPU=$(F_CPU)UL
CDEFS += -DCHB_DEBUG

# Place -D or -U options here for C++ sources
CPPDEFS += -DF_CPU=$(F_CPU)UL
#CPPDEFS += -D__STDC_LIMIT_MACROS
#CPPDEFS += -D__STDC_CONSTANT_MACROS

# Place -D or -U options here for ASM sources
ADEFS = -DF_CPU=$(F_CPU)

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
CPORTABILITY += -fpack-struct
CPORTABILITY += -fshort-enums

CFLAGS += -DGCC_XMEGA_AVR
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
CFLAGS += -DBOARD_TYPE=$(BOARD_TYPE)
CFLAGS += -DPLATFORM=$(PLATFORM)
CFLAGS += -DVENDOR_BOARDTYPES=1

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
CPPORTABILITY += -fpack-struct
CPPORTABILITY += -fshort-enums
CPPORTABILITY += -fno-exceptions
#CPPORTABILITY += -fno-unit-at-a-time
CPPORTABILITY = -std=c++0x

CPPFLAGS = -g$(DEBUG)
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
PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)

MATH_LIB = -lm

# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS +=

#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file


# Processor definitions
ifeq ($(MCU), atxmega16a4)
  BOOT_SECTION_START		=0x004000
endif
ifeq ($(MCU), atxmega32a4)
  BOOT_SECTION_START		=0x008000
endif
ifneq (, $(findstring $(MCU), atxmega64a1 atxmega64a3 atxmega64a4))
  BOOT_SECTION_START		=0x010000
endif
ifneq (, $(findstring $(MCU), atxmega128a1 atxmega128a3 atxmega128a4))
  BOOT_SECTION_START		=0x020000
endif
ifneq (, $(findstring $(MCU), atxmega192a1 atxmega192a3))
  BOOT_SECTION_START		=0x030000
endif
ifneq (, $(findstring $(MCU), atxmega256a1 atxmega256a3 atxmega256a3u atxmega256a3b atxmega256a3bu))
  BOOT_SECTION_START		=0x040000
endif

#LDFLAGS += -Wl,--relax //////// (DOT NOT EXECUTE SOME METHODS)
LDFLAGS += -Wl,--section-start=.boot=$(BOOT_SECTION_START)
#LDFLAGS += -Wl,--gc-sections
#LDFLAGS += -Wl,--script=$(PLATFORM_DIR)/linkerScr/$(MCU).ld
LDFLAGS += -Wl,-Map=$(TARGET).map

LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)

#---------------- Programming Options (avrdude) ----------------

# Programming hardware
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = avr911

# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = com2    # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

AVRDUDE_BAUDRATE = 115200

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(AVRDUDE_MCU) -b $(AVRDUDE_BAUDRATE) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)

#---------------- Debugging Options ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)

# Set the DEBUG_UI to either gdb or insight.
DEBUG_UI = gdb
#DEBUG_UI = insight

# Set the debugging back-end to either avarice, simulavr.
DEBUG_BACKEND = avarice
#DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when
#     avarice is running on a different computer.
DEBUG_HOST = localhost


#============================================================================

# Define directories, if needed.
#DIRAVR = c:/winavr
#DIRAVRBIN = $(DIRAVR)/bin
#DIRAVRUTILS = $(DIRAVR)/utils/bin
#DIRINC = .
#DIRLIB = $(DIRAVR)/avr/lib

# Define programs and commands.
PATH := $(AVR_GCC):$(PATH)
#SHELL = $(GIT)\usr\bin\bash
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AR = avr-ar -rcsv
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp
WINSHELL = cmd
MKDIR_P = $(GIT)\usr\bin\mkdir -p

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
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
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -I. -x c++ $(CPPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. $(ASFLAGS)





# Default target.
#all: begin gccversion sizebefore build sizeafter end
all: begin gccversion build end

# Default lib target.
allib: begin gccversion sizebefore buildlib sizeafter end

# build target to build a HEX file.
#build: extcoff elf hex srec eep lss sym
build: elf hex srec eep lss sym

# build target to build a library.
buildlib: lib


elf: $(TARGET).elf
hex: $(TARGET).hex
srec: $(TARGET).srec
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym
LIBNAME=lib$(TARGET).a
lib: $(LIBNAME)

$(TARGET).elf: $(OBJ)
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) --mcu=$(MCU) --format=avr $(TARGET).elf

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
	2>/dev/null; echo; fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
	2>/dev/null; echo; fi



# Display compiler version information.
gccversion :
	@$(CC) --version



# Program the device.
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set
#     a breakpoint at main().
gdb-config:
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif
	@echo break main >> $(GDBINIT_FILE)

debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
	$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause

else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT = $(OBJCOPY) --debugging
COFFCONVERT += --change-section-address .data-0x800000
COFFCONVERT += --change-section-address .bss-0x800000
COFFCONVERT += --change-section-address .noinit-0x800000
COFFCONVERT += --change-section-address .eeprom-0x810000



coff: $(TARGET).elf
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof



# Create final output files (.hex, .srec, .eep) from ELF output file.
%.hex: %.elf
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature $< $@

%.srec: %.elf
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O srec --srec-len 128 $< $@

%.eep: %.elf
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@



# Create library from object files.
.SECONDARY : $(TARGET).a
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
	@echo $(MSG_CREATING_LIBRARY) $@
	$(AR) $@ $(OBJ)


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp
	@echo $(MSG_COMPILING_CPP) $<
	$(CC) -c $(ALL_CPPFLAGS) $< -o $@


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C++ source files.
%.s : %.cpp
	$(CC) -S $(ALL_CPPFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.asm
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@


# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
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

#	$(REMOVE) $(CSRC:.c=.d)
#	$(REMOVEDIR) .dep

rebuild:
	make clean
	make all

rebuildlib:
	make clean
	make allib

# Include the dependency files.
-include $(DEPS)

# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex srec eep lss sym coff extcoff \
clean clean_list program debug gdb-config
