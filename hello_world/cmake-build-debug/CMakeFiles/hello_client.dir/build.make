# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.28.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.28.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/nichao/Code/c++/networks/hello_world

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/nichao/Code/c++/networks/hello_world/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/hello_client.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/hello_client.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/hello_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hello_client.dir/flags.make

CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o: CMakeFiles/hello_client.dir/flags.make
CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o: /Users/nichao/Code/c++/networks/hello_world/tcp_hello_client.cpp
CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o: CMakeFiles/hello_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nichao/Code/c++/networks/hello_world/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o -MF CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o.d -o CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o -c /Users/nichao/Code/c++/networks/hello_world/tcp_hello_client.cpp

CMakeFiles/hello_client.dir/tcp_hello_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/hello_client.dir/tcp_hello_client.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/nichao/Code/c++/networks/hello_world/tcp_hello_client.cpp > CMakeFiles/hello_client.dir/tcp_hello_client.cpp.i

CMakeFiles/hello_client.dir/tcp_hello_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/hello_client.dir/tcp_hello_client.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/nichao/Code/c++/networks/hello_world/tcp_hello_client.cpp -o CMakeFiles/hello_client.dir/tcp_hello_client.cpp.s

# Object files for target hello_client
hello_client_OBJECTS = \
"CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o"

# External object files for target hello_client
hello_client_EXTERNAL_OBJECTS =

hello_client: CMakeFiles/hello_client.dir/tcp_hello_client.cpp.o
hello_client: CMakeFiles/hello_client.dir/build.make
hello_client: CMakeFiles/hello_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/nichao/Code/c++/networks/hello_world/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable hello_client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hello_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hello_client.dir/build: hello_client
.PHONY : CMakeFiles/hello_client.dir/build

CMakeFiles/hello_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hello_client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hello_client.dir/clean

CMakeFiles/hello_client.dir/depend:
	cd /Users/nichao/Code/c++/networks/hello_world/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/nichao/Code/c++/networks/hello_world /Users/nichao/Code/c++/networks/hello_world /Users/nichao/Code/c++/networks/hello_world/cmake-build-debug /Users/nichao/Code/c++/networks/hello_world/cmake-build-debug /Users/nichao/Code/c++/networks/hello_world/cmake-build-debug/CMakeFiles/hello_client.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/hello_client.dir/depend

