# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /data/data/code_person

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/data/code_person

# Include any dependencies generated for this target.
include tuple/CMakeFiles/tuple.dir/depend.make

# Include the progress variables for this target.
include tuple/CMakeFiles/tuple.dir/progress.make

# Include the compile flags for this target's objects.
include tuple/CMakeFiles/tuple.dir/flags.make

tuple/CMakeFiles/tuple.dir/tuple.cc.o: tuple/CMakeFiles/tuple.dir/flags.make
tuple/CMakeFiles/tuple.dir/tuple.cc.o: tuple/tuple.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/data/code_person/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tuple/CMakeFiles/tuple.dir/tuple.cc.o"
	cd /data/data/code_person/tuple && /data/impala/toolchain/gcc-4.9.2/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tuple.dir/tuple.cc.o -c /data/data/code_person/tuple/tuple.cc

tuple/CMakeFiles/tuple.dir/tuple.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tuple.dir/tuple.cc.i"
	cd /data/data/code_person/tuple && /data/impala/toolchain/gcc-4.9.2/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/data/code_person/tuple/tuple.cc > CMakeFiles/tuple.dir/tuple.cc.i

tuple/CMakeFiles/tuple.dir/tuple.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tuple.dir/tuple.cc.s"
	cd /data/data/code_person/tuple && /data/impala/toolchain/gcc-4.9.2/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/data/code_person/tuple/tuple.cc -o CMakeFiles/tuple.dir/tuple.cc.s

tuple/CMakeFiles/tuple.dir/tuple.cc.o.requires:

.PHONY : tuple/CMakeFiles/tuple.dir/tuple.cc.o.requires

tuple/CMakeFiles/tuple.dir/tuple.cc.o.provides: tuple/CMakeFiles/tuple.dir/tuple.cc.o.requires
	$(MAKE) -f tuple/CMakeFiles/tuple.dir/build.make tuple/CMakeFiles/tuple.dir/tuple.cc.o.provides.build
.PHONY : tuple/CMakeFiles/tuple.dir/tuple.cc.o.provides

tuple/CMakeFiles/tuple.dir/tuple.cc.o.provides.build: tuple/CMakeFiles/tuple.dir/tuple.cc.o


tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o: tuple/CMakeFiles/tuple.dir/flags.make
tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o: tuple/scope_exit_trigger.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/data/code_person/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o"
	cd /data/data/code_person/tuple && /data/impala/toolchain/gcc-4.9.2/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tuple.dir/scope_exit_trigger.cc.o -c /data/data/code_person/tuple/scope_exit_trigger.cc

tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tuple.dir/scope_exit_trigger.cc.i"
	cd /data/data/code_person/tuple && /data/impala/toolchain/gcc-4.9.2/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/data/code_person/tuple/scope_exit_trigger.cc > CMakeFiles/tuple.dir/scope_exit_trigger.cc.i

tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tuple.dir/scope_exit_trigger.cc.s"
	cd /data/data/code_person/tuple && /data/impala/toolchain/gcc-4.9.2/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/data/code_person/tuple/scope_exit_trigger.cc -o CMakeFiles/tuple.dir/scope_exit_trigger.cc.s

tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.requires:

.PHONY : tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.requires

tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.provides: tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.requires
	$(MAKE) -f tuple/CMakeFiles/tuple.dir/build.make tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.provides.build
.PHONY : tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.provides

tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.provides.build: tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o


# Object files for target tuple
tuple_OBJECTS = \
"CMakeFiles/tuple.dir/tuple.cc.o" \
"CMakeFiles/tuple.dir/scope_exit_trigger.cc.o"

# External object files for target tuple
tuple_EXTERNAL_OBJECTS =

tuple/libtuple.a: tuple/CMakeFiles/tuple.dir/tuple.cc.o
tuple/libtuple.a: tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o
tuple/libtuple.a: tuple/CMakeFiles/tuple.dir/build.make
tuple/libtuple.a: tuple/CMakeFiles/tuple.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/data/code_person/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libtuple.a"
	cd /data/data/code_person/tuple && $(CMAKE_COMMAND) -P CMakeFiles/tuple.dir/cmake_clean_target.cmake
	cd /data/data/code_person/tuple && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tuple.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tuple/CMakeFiles/tuple.dir/build: tuple/libtuple.a

.PHONY : tuple/CMakeFiles/tuple.dir/build

tuple/CMakeFiles/tuple.dir/requires: tuple/CMakeFiles/tuple.dir/tuple.cc.o.requires
tuple/CMakeFiles/tuple.dir/requires: tuple/CMakeFiles/tuple.dir/scope_exit_trigger.cc.o.requires

.PHONY : tuple/CMakeFiles/tuple.dir/requires

tuple/CMakeFiles/tuple.dir/clean:
	cd /data/data/code_person/tuple && $(CMAKE_COMMAND) -P CMakeFiles/tuple.dir/cmake_clean.cmake
.PHONY : tuple/CMakeFiles/tuple.dir/clean

tuple/CMakeFiles/tuple.dir/depend:
	cd /data/data/code_person && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/data/code_person /data/data/code_person/tuple /data/data/code_person /data/data/code_person/tuple /data/data/code_person/tuple/CMakeFiles/tuple.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tuple/CMakeFiles/tuple.dir/depend

