# Add new build types
message("* Adding build type \"profile\"")
set(CMAKE_CXX_FLAGS_PROFILE
    "${GCC_DEBUG_FLAGS} -pg"
    CACHE STRING "Flags used by the C++ compiler during profile builds."
    FORCE )
set(CMAKE_C_FLAGS_PROFILE
    "${GCC_DEBUG_FLAGS} --pg"
    CACHE STRING "Flags used by the C compiler during profile builds."
    FORCE )
set(CMAKE_EXE_LINKER_FLAGS_PROFILE
    ""
    CACHE STRING "Flags used for linking binaries during profile builds."
    FORCE )
set(CMAKE_SHARED_LINKER_FLAGS_PROFILE
    ""
    CACHE STRING "Flags used by the shared libraries linker during profile builds."
    FORCE )
mark_as_advanced(
    CMAKE_CXX_FLAGS_PROFILE
    CMAKE_C_FLAGS_PROFILE
    CMAKE_EXE_LINKER_FLAGS_PROFILE
    CMAKE_SHARED_LINKER_FLAGS_PROFILE )

# Add new build types
message("* Adding build type \"optprofile\"")
set(CMAKE_CXX_FLAGS_OPTPROFILE
    "-O3 -g -pg"
    CACHE STRING "Flags used by the C++ compiler during optimized profile builds."
    FORCE )
set(CMAKE_C_FLAGS_OPTPROFILE
    "-O3 0g -pg"
    CACHE STRING "Flags used by the C compiler during optimized profile builds."
    FORCE )
set(CMAKE_EXE_LINKER_FLAGS_OPTPROFILE
    ""
    CACHE STRING "Flags used for linking binaries during optimized profile builds."
    FORCE )
set(CMAKE_SHARED_LINKER_FLAGS_OPTPROFILE
    ""
    CACHE STRING "Flags used by the shared libraries linker during optimized profile builds."
    FORCE )
mark_as_advanced(
    CMAKE_CXX_FLAGS_OPTPROFILE
    CMAKE_C_FLAGS_OPTPROFILE
    CMAKE_EXE_LINKER_FLAGS_OPTPROFILE
    CMAKE_SHARED_LINKER_FLAGS_OPTPROFILE )
