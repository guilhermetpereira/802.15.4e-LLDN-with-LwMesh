# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\dlfcn-win32-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32

# Include any dependencies generated for this target.
include src/CMakeFiles/dl.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/dl.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/dl.dir/flags.make

src/CMakeFiles/dl.dir/dlfcn.c.obj: src/CMakeFiles/dl.dir/flags.make
src/CMakeFiles/dl.dir/dlfcn.c.obj: dlfcn-win32-master/src/dlfcn.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/dl.dir/dlfcn.c.obj"
	cd /d C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\src && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\dl.dir\dlfcn.c.obj   -c C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\dlfcn-win32-master\src\dlfcn.c

src/CMakeFiles/dl.dir/dlfcn.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dl.dir/dlfcn.c.i"
	cd /d C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\src && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\dlfcn-win32-master\src\dlfcn.c > CMakeFiles\dl.dir\dlfcn.c.i

src/CMakeFiles/dl.dir/dlfcn.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dl.dir/dlfcn.c.s"
	cd /d C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\src && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\dlfcn-win32-master\src\dlfcn.c -o CMakeFiles\dl.dir\dlfcn.c.s

# Object files for target dl
dl_OBJECTS = \
"CMakeFiles/dl.dir/dlfcn.c.obj"

# External object files for target dl
dl_EXTERNAL_OBJECTS =

bin/libdl.dll: src/CMakeFiles/dl.dir/dlfcn.c.obj
bin/libdl.dll: src/CMakeFiles/dl.dir/build.make
bin/libdl.dll: src/CMakeFiles/dl.dir/linklibs.rsp
bin/libdl.dll: src/CMakeFiles/dl.dir/objects1.rsp
bin/libdl.dll: src/CMakeFiles/dl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library ..\bin\libdl.dll"
	cd /d C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\dl.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/dl.dir/build: bin/libdl.dll

.PHONY : src/CMakeFiles/dl.dir/build

src/CMakeFiles/dl.dir/clean:
	cd /d C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\src && $(CMAKE_COMMAND) -P CMakeFiles\dl.dir\cmake_clean.cmake
.PHONY : src/CMakeFiles/dl.dir/clean

src/CMakeFiles/dl.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\dlfcn-win32-master C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\dlfcn-win32-master\src C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32 C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\src C:\Users\Guilherme\Documents\GitHub\LLDN-App-on-AtmelStudio7.0\application\TDMA\ServerTDMA\dlfc-win32\src\CMakeFiles\dl.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/dl.dir/depend

