# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build"

# Include any dependencies generated for this target.
include CMakeFiles/lib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lib.dir/flags.make

CMakeFiles/lib.dir/core/camera/camera.cpp.o: CMakeFiles/lib.dir/flags.make
CMakeFiles/lib.dir/core/camera/camera.cpp.o: ../core/camera/camera.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/lib.dir/core/camera/camera.cpp.o"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lib.dir/core/camera/camera.cpp.o -c "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/camera/camera.cpp"

CMakeFiles/lib.dir/core/camera/camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lib.dir/core/camera/camera.cpp.i"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/camera/camera.cpp" > CMakeFiles/lib.dir/core/camera/camera.cpp.i

CMakeFiles/lib.dir/core/camera/camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lib.dir/core/camera/camera.cpp.s"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/camera/camera.cpp" -o CMakeFiles/lib.dir/core/camera/camera.cpp.s

CMakeFiles/lib.dir/core/camera/camera.cpp.o.requires:

.PHONY : CMakeFiles/lib.dir/core/camera/camera.cpp.o.requires

CMakeFiles/lib.dir/core/camera/camera.cpp.o.provides: CMakeFiles/lib.dir/core/camera/camera.cpp.o.requires
	$(MAKE) -f CMakeFiles/lib.dir/build.make CMakeFiles/lib.dir/core/camera/camera.cpp.o.provides.build
.PHONY : CMakeFiles/lib.dir/core/camera/camera.cpp.o.provides

CMakeFiles/lib.dir/core/camera/camera.cpp.o.provides.build: CMakeFiles/lib.dir/core/camera/camera.cpp.o


CMakeFiles/lib.dir/core/log/log.cpp.o: CMakeFiles/lib.dir/flags.make
CMakeFiles/lib.dir/core/log/log.cpp.o: ../core/log/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/lib.dir/core/log/log.cpp.o"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lib.dir/core/log/log.cpp.o -c "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/log/log.cpp"

CMakeFiles/lib.dir/core/log/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lib.dir/core/log/log.cpp.i"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/log/log.cpp" > CMakeFiles/lib.dir/core/log/log.cpp.i

CMakeFiles/lib.dir/core/log/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lib.dir/core/log/log.cpp.s"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/log/log.cpp" -o CMakeFiles/lib.dir/core/log/log.cpp.s

CMakeFiles/lib.dir/core/log/log.cpp.o.requires:

.PHONY : CMakeFiles/lib.dir/core/log/log.cpp.o.requires

CMakeFiles/lib.dir/core/log/log.cpp.o.provides: CMakeFiles/lib.dir/core/log/log.cpp.o.requires
	$(MAKE) -f CMakeFiles/lib.dir/build.make CMakeFiles/lib.dir/core/log/log.cpp.o.provides.build
.PHONY : CMakeFiles/lib.dir/core/log/log.cpp.o.provides

CMakeFiles/lib.dir/core/log/log.cpp.o.provides.build: CMakeFiles/lib.dir/core/log/log.cpp.o


CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o: CMakeFiles/lib.dir/flags.make
CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o: ../core/machine_learning/machine_learning.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o -c "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/machine_learning/machine_learning.cpp"

CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.i"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/machine_learning/machine_learning.cpp" > CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.i

CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.s"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/core/machine_learning/machine_learning.cpp" -o CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.s

CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.requires:

.PHONY : CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.requires

CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.provides: CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.requires
	$(MAKE) -f CMakeFiles/lib.dir/build.make CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.provides.build
.PHONY : CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.provides

CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.provides.build: CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o


# Object files for target lib
lib_OBJECTS = \
"CMakeFiles/lib.dir/core/camera/camera.cpp.o" \
"CMakeFiles/lib.dir/core/log/log.cpp.o" \
"CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o"

# External object files for target lib
lib_EXTERNAL_OBJECTS =

liblib.a: CMakeFiles/lib.dir/core/camera/camera.cpp.o
liblib.a: CMakeFiles/lib.dir/core/log/log.cpp.o
liblib.a: CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o
liblib.a: CMakeFiles/lib.dir/build.make
liblib.a: CMakeFiles/lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library liblib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/lib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lib.dir/build: liblib.a

.PHONY : CMakeFiles/lib.dir/build

CMakeFiles/lib.dir/requires: CMakeFiles/lib.dir/core/camera/camera.cpp.o.requires
CMakeFiles/lib.dir/requires: CMakeFiles/lib.dir/core/log/log.cpp.o.requires
CMakeFiles/lib.dir/requires: CMakeFiles/lib.dir/core/machine_learning/machine_learning.cpp.o.requires

.PHONY : CMakeFiles/lib.dir/requires

CMakeFiles/lib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lib.dir/clean

CMakeFiles/lib.dir/depend:
	cd "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main" "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main" "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build" "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build" "/home/josep/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/build/CMakeFiles/lib.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/lib.dir/depend

