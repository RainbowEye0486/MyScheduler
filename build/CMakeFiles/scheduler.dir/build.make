# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/airobots/MyScheduler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/airobots/MyScheduler/build

# Include any dependencies generated for this target.
include CMakeFiles/scheduler.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/scheduler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/scheduler.dir/flags.make

CMakeFiles/scheduler.dir/src/main.cpp.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/airobots/MyScheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/scheduler.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/scheduler.dir/src/main.cpp.o -c /home/airobots/MyScheduler/src/main.cpp

CMakeFiles/scheduler.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/scheduler.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/airobots/MyScheduler/src/main.cpp > CMakeFiles/scheduler.dir/src/main.cpp.i

CMakeFiles/scheduler.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/scheduler.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/airobots/MyScheduler/src/main.cpp -o CMakeFiles/scheduler.dir/src/main.cpp.s

CMakeFiles/scheduler.dir/src/Scheduler.cpp.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/src/Scheduler.cpp.o: ../src/Scheduler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/airobots/MyScheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/scheduler.dir/src/Scheduler.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/scheduler.dir/src/Scheduler.cpp.o -c /home/airobots/MyScheduler/src/Scheduler.cpp

CMakeFiles/scheduler.dir/src/Scheduler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/scheduler.dir/src/Scheduler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/airobots/MyScheduler/src/Scheduler.cpp > CMakeFiles/scheduler.dir/src/Scheduler.cpp.i

CMakeFiles/scheduler.dir/src/Scheduler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/scheduler.dir/src/Scheduler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/airobots/MyScheduler/src/Scheduler.cpp -o CMakeFiles/scheduler.dir/src/Scheduler.cpp.s

CMakeFiles/scheduler.dir/src/SA.cpp.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/src/SA.cpp.o: ../src/SA.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/airobots/MyScheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/scheduler.dir/src/SA.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/scheduler.dir/src/SA.cpp.o -c /home/airobots/MyScheduler/src/SA.cpp

CMakeFiles/scheduler.dir/src/SA.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/scheduler.dir/src/SA.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/airobots/MyScheduler/src/SA.cpp > CMakeFiles/scheduler.dir/src/SA.cpp.i

CMakeFiles/scheduler.dir/src/SA.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/scheduler.dir/src/SA.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/airobots/MyScheduler/src/SA.cpp -o CMakeFiles/scheduler.dir/src/SA.cpp.s

CMakeFiles/scheduler.dir/src/GA.cpp.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/src/GA.cpp.o: ../src/GA.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/airobots/MyScheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/scheduler.dir/src/GA.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/scheduler.dir/src/GA.cpp.o -c /home/airobots/MyScheduler/src/GA.cpp

CMakeFiles/scheduler.dir/src/GA.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/scheduler.dir/src/GA.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/airobots/MyScheduler/src/GA.cpp > CMakeFiles/scheduler.dir/src/GA.cpp.i

CMakeFiles/scheduler.dir/src/GA.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/scheduler.dir/src/GA.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/airobots/MyScheduler/src/GA.cpp -o CMakeFiles/scheduler.dir/src/GA.cpp.s

CMakeFiles/scheduler.dir/src/GP.cpp.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/src/GP.cpp.o: ../src/GP.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/airobots/MyScheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/scheduler.dir/src/GP.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/scheduler.dir/src/GP.cpp.o -c /home/airobots/MyScheduler/src/GP.cpp

CMakeFiles/scheduler.dir/src/GP.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/scheduler.dir/src/GP.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/airobots/MyScheduler/src/GP.cpp > CMakeFiles/scheduler.dir/src/GP.cpp.i

CMakeFiles/scheduler.dir/src/GP.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/scheduler.dir/src/GP.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/airobots/MyScheduler/src/GP.cpp -o CMakeFiles/scheduler.dir/src/GP.cpp.s

# Object files for target scheduler
scheduler_OBJECTS = \
"CMakeFiles/scheduler.dir/src/main.cpp.o" \
"CMakeFiles/scheduler.dir/src/Scheduler.cpp.o" \
"CMakeFiles/scheduler.dir/src/SA.cpp.o" \
"CMakeFiles/scheduler.dir/src/GA.cpp.o" \
"CMakeFiles/scheduler.dir/src/GP.cpp.o"

# External object files for target scheduler
scheduler_EXTERNAL_OBJECTS =

bin/scheduler: CMakeFiles/scheduler.dir/src/main.cpp.o
bin/scheduler: CMakeFiles/scheduler.dir/src/Scheduler.cpp.o
bin/scheduler: CMakeFiles/scheduler.dir/src/SA.cpp.o
bin/scheduler: CMakeFiles/scheduler.dir/src/GA.cpp.o
bin/scheduler: CMakeFiles/scheduler.dir/src/GP.cpp.o
bin/scheduler: CMakeFiles/scheduler.dir/build.make
bin/scheduler: CMakeFiles/scheduler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/airobots/MyScheduler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable bin/scheduler"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/scheduler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/scheduler.dir/build: bin/scheduler

.PHONY : CMakeFiles/scheduler.dir/build

CMakeFiles/scheduler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/scheduler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/scheduler.dir/clean

CMakeFiles/scheduler.dir/depend:
	cd /home/airobots/MyScheduler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/airobots/MyScheduler /home/airobots/MyScheduler /home/airobots/MyScheduler/build /home/airobots/MyScheduler/build /home/airobots/MyScheduler/build/CMakeFiles/scheduler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/scheduler.dir/depend

