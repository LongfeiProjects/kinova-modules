# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/smaricha/qt_workspace/kinova_root/subcmake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/smaricha/qt_workspace/kinova_root/build-subcmake-Desktop_Qt_5_5_0_GCC_64bit-Default

# Include any dependencies generated for this target.
include CMakeFiles/subcmake.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/subcmake.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/subcmake.dir/flags.make

CMakeFiles/subcmake.dir/main.cpp.o: CMakeFiles/subcmake.dir/flags.make
CMakeFiles/subcmake.dir/main.cpp.o: /home/smaricha/qt_workspace/kinova_root/subcmake/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/smaricha/qt_workspace/kinova_root/build-subcmake-Desktop_Qt_5_5_0_GCC_64bit-Default/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/subcmake.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/subcmake.dir/main.cpp.o -c /home/smaricha/qt_workspace/kinova_root/subcmake/main.cpp

CMakeFiles/subcmake.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/subcmake.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/smaricha/qt_workspace/kinova_root/subcmake/main.cpp > CMakeFiles/subcmake.dir/main.cpp.i

CMakeFiles/subcmake.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/subcmake.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/smaricha/qt_workspace/kinova_root/subcmake/main.cpp -o CMakeFiles/subcmake.dir/main.cpp.s

CMakeFiles/subcmake.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/subcmake.dir/main.cpp.o.requires

CMakeFiles/subcmake.dir/main.cpp.o.provides: CMakeFiles/subcmake.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/subcmake.dir/build.make CMakeFiles/subcmake.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/subcmake.dir/main.cpp.o.provides

CMakeFiles/subcmake.dir/main.cpp.o.provides.build: CMakeFiles/subcmake.dir/main.cpp.o

# Object files for target subcmake
subcmake_OBJECTS = \
"CMakeFiles/subcmake.dir/main.cpp.o"

# External object files for target subcmake
subcmake_EXTERNAL_OBJECTS =

subcmake: CMakeFiles/subcmake.dir/main.cpp.o
subcmake: CMakeFiles/subcmake.dir/build.make
subcmake: CMakeFiles/subcmake.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable subcmake"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/subcmake.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/subcmake.dir/build: subcmake
.PHONY : CMakeFiles/subcmake.dir/build

CMakeFiles/subcmake.dir/requires: CMakeFiles/subcmake.dir/main.cpp.o.requires
.PHONY : CMakeFiles/subcmake.dir/requires

CMakeFiles/subcmake.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/subcmake.dir/cmake_clean.cmake
.PHONY : CMakeFiles/subcmake.dir/clean

CMakeFiles/subcmake.dir/depend:
	cd /home/smaricha/qt_workspace/kinova_root/build-subcmake-Desktop_Qt_5_5_0_GCC_64bit-Default && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smaricha/qt_workspace/kinova_root/subcmake /home/smaricha/qt_workspace/kinova_root/subcmake /home/smaricha/qt_workspace/kinova_root/build-subcmake-Desktop_Qt_5_5_0_GCC_64bit-Default /home/smaricha/qt_workspace/kinova_root/build-subcmake-Desktop_Qt_5_5_0_GCC_64bit-Default /home/smaricha/qt_workspace/kinova_root/build-subcmake-Desktop_Qt_5_5_0_GCC_64bit-Default/CMakeFiles/subcmake.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/subcmake.dir/depend

