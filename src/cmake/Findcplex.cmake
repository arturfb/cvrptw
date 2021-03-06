#SET(CPLEX_ROOT_DIR "" CACHE PATH "CPLEX root directory")
SET(CPLEX_ROOT_DIR "/opt/ibm/ILOG/CPLEX_Studio125")

FIND_PATH(CPLEX_INCLUDE_DIR
  ilcplex/cplex.h
  PATHS "C:/ILOG/CPLEX/include"
  PATHS "/opt/ilog/cplex/include"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cplex/include"
  HINTS ${CPLEX_ROOT_DIR}/include
  HINTS ${CPLEX_ROOT_DIR}/cplex/include
)

FIND_PATH(CONCERT_INCLUDE_DIR
  ilconcert/ilomodel.h
  PATHS "C:/ILOG/CONCERT/include"
  PATHS "/opt/ilog/concert/include"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/concert/include"
  HINTS ${CPLEX_ROOT_DIR}/include
  HINTS ${CPLEX_ROOT_DIR}/concert/include
)

FIND_PATH(CPOPTIMIZER_INCLUDE_DIR
  ilcp/cp.h
  PATHS "C:/ILOG/CPOPTIMIZER/include"
  PATHS "/opt/ilog/cpoptimizer/include"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cpoptimizer/include"
  HINTS ${CPLEX_ROOT_DIR}/include
  HINTS ${CPLEX_ROOT_DIR}/cpoptimizer/include
)

FIND_LIBRARY(CPLEX_LIBRARY
  cplex
  PATHS "C:/ILOG/CPLEX/lib/msvc7/stat_mda"
  PATHS "/opt/ilog/cplex/bin"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cplex/lib/x86-64_sles10_4.1/static_pic/"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cplex/bin"
  HINTS ${CPLEX_ROOT_DIR}/bin
  HINTS ${CPLEX_ROOT_DIR}/cplex/bin
  HINTS ${CPLEX_ROOT_DIR}/lib
  HINTS ${CPLEX_ROOT_DIR}/cplex/lib
  HINTS ${CPLEX_ROOT_DIR}/cplex/lib/x86-64_sles10_4.1
  HINTS ${CPLEX_ROOT_DIR}/cplex/lib/x86-64_sles10_4.1/static_pic
  HINTS ${CPLEX_ROOT_DIR}/cplex/bin/x86-64_sles10_4.1
)

FIND_LIBRARY(ILOCPLEX_LIBRARY
  ilocplex
  PATHS "C:/ILOG/CPLEX/lib/msvc7/stat_mda"
  PATHS "/opt/ilog/cplex/bin"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cplex/lib/x86-64_sles10_4.1/static_pic/"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cplex/bin"
  HINTS ${CPLEX_ROOT_DIR}/bin
  HINTS ${CPLEX_ROOT_DIR}/cplex/bin
  HINTS ${CPLEX_ROOT_DIR}/lib
  HINTS ${CPLEX_ROOT_DIR}/cplex/lib
  HINTS ${CPLEX_ROOT_DIR}/cplex/lib/x86-64_sles10_4.1/static_pic
)

FIND_LIBRARY(CONCERT_LIBRARY
  concert
  PATHS "C:/ILOG/CONCERT/lib/msvc7/stat_mda"
  PATHS "/opt/ilog/concert/bin"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/concert/lib/x86-64_sles10_4.1/static_pic/"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/concert/bin"
  HINTS ${CPLEX_ROOT_DIR}/bin
  HINTS ${CPLEX_ROOT_DIR}/concert/bin
  HINTS ${CPLEX_ROOT_DIR}/lib
  HINTS ${CPLEX_ROOT_DIR}/concert/lib
  HINTS ${CPLEX_ROOT_DIR}/concert/lib/x86-64_sles10_4.1/static_pic
)

FIND_LIBRARY(CPOPTIMIZER_LIBRARY
  cp
  PATHS "C:/ILOG/CPOPTIMIZER/lib/msvc7/stat_mda"
  PATHS "/opt/ilog/cpoptimizer/bin"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cpoptimizer/lib/x86-64_sles10_4.1/static_pic/"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cpoptimizer/bin"
  HINTS ${CPLEX_ROOT_DIR}/bin
  HINTS ${CPLEX_ROOT_DIR}/cpoptimizer/bin
  HINTS ${CPLEX_ROOT_DIR}/lib
  HINTS ${CPLEX_ROOT_DIR}/cpoptimizer/lib
  HINTS ${CPLEX_ROOT_DIR}/cpoptimizer/lib/x86-64_sles10_4.1/static_pic
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CPLEX DEFAULT_MSG CPLEX_LIBRARY CPLEX_INCLUDE_DIR)

FIND_PATH(CPLEX_BIN_DIR
  cplex.dll
  PATHS "C:/ILOG/CPLEX/bin/x86_win32"
  PATHS "~/ILOG/CPLEX_Studio_AcademicResearch122/cplex/bin"
  HINTS ${CPLEX_ROOT_DIR}/bin
  HINTS ${CPLEX_ROOT_DIR}/cplex/bin
)

IF(CPLEX_FOUND)
  SET(CPLEX_INCLUDE_DIRS "${CPLEX_INCLUDE_DIR};${CONCERT_INCLUDE_DIR};${CPOPTIMIZER_INCLUDE_DIR}")
  SET(CPLEX_LIBRARIES "${ILOCPLEX_LIBRARY};${CPLEX_LIBRARY};${CONCERT_LIBRARY};pthread;z;boost_program_options")
  IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(CPLEX_LIBRARIES "${CPLEX_LIBRARIES};m;pthread")
  ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
ENDIF(CPLEX_FOUND)

MARK_AS_ADVANCED(CPLEX_LIBRARY CPLEX_INCLUDE_DIR CPLEX_BIN_DIR)

IF(CPLEX_FOUND)
  SET(LEMON_HAVE_LP TRUE)
  SET(LEMON_HAVE_MIP TRUE)
  SET(LEMON_HAVE_CPLEX TRUE)
ENDIF(CPLEX_FOUND)
