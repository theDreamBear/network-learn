# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/homework/network-learn/epoll

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/homework/network-learn/epoll/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/epoll.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/epoll.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/epoll.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/epoll.dir/flags.make

CMakeFiles/epoll.dir/main.cpp.o: CMakeFiles/epoll.dir/flags.make
CMakeFiles/epoll.dir/main.cpp.o: /home/homework/network-learn/epoll/main.cpp
CMakeFiles/epoll.dir/main.cpp.o: CMakeFiles/epoll.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/homework/network-learn/epoll/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/epoll.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/epoll.dir/main.cpp.o -MF CMakeFiles/epoll.dir/main.cpp.o.d -o CMakeFiles/epoll.dir/main.cpp.o -c /home/homework/network-learn/epoll/main.cpp

CMakeFiles/epoll.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/epoll.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/homework/network-learn/epoll/main.cpp > CMakeFiles/epoll.dir/main.cpp.i

CMakeFiles/epoll.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/epoll.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/homework/network-learn/epoll/main.cpp -o CMakeFiles/epoll.dir/main.cpp.s

# Object files for target epoll
epoll_OBJECTS = \
"CMakeFiles/epoll.dir/main.cpp.o"

# External object files for target epoll
epoll_EXTERNAL_OBJECTS =

epoll: CMakeFiles/epoll.dir/main.cpp.o
epoll: CMakeFiles/epoll.dir/build.make
epoll: CMakeFiles/epoll.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/homework/network-learn/epoll/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable epoll"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/epoll.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/epoll.dir/build: epoll
.PHONY : CMakeFiles/epoll.dir/build

CMakeFiles/epoll.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/epoll.dir/cmake_clean.cmake
.PHONY : CMakeFiles/epoll.dir/clean

CMakeFiles/epoll.dir/depend:
	cd /home/homework/network-learn/epoll/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/homework/network-learn/epoll /home/homework/network-learn/epoll /home/homework/network-learn/epoll/cmake-build-debug /home/homework/network-learn/epoll/cmake-build-debug /home/homework/network-learn/epoll/cmake-build-debug/CMakeFiles/epoll.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/epoll.dir/depend
