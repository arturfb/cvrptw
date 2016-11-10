# cvrptw
A solver for the capacitated vehicle routing problem with time windows

Requires:
  cmake
  cplex
  boost (program_options)
  
1) If cplex isn't at default location, start by editing src/cmake/Findcplex.cmake.
Modify line 2 according to where cplex is installed in your system:
SET(CPLEX_ROOT_DIR "/your/path/to/cplex/folder")

2) To compile:
  cd <project_directory>/src/build
  cmake ..
  make
  
3) To run:
  ./cvrptw <instance_file> [options]
  
For example, assuming your are in your build directory:
  ./cvrptw ../../instances/cordeau c101 --threads 10 --timelimit 3600
  
  
Questions?
  Contact me: afbrum@inf.ufrgs.br
