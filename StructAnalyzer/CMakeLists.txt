cmake_minimum_required(VERSION 3.22)

project(StructAnalyzer)

include("../Common/cmake/common_cmake.cmake")

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  message(STATUS "OpenMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# other package in this dictionary
set(REQUIRED_PACKAGES ModuleLoader ModuleAnalyzer DatabaseManager)
set(REQUIRED_PACKAGES_INCLUDE "")
list(TRANSFORM REQUIRED_PACKAGES PREPEND ../ OUTPUT_VARIABLE REQUIRED_PACKAGES_INCLUDE)
list(TRANSFORM REQUIRED_PACKAGES_INCLUDE APPEND /include)


# third-party project - must be explicit include in cmake, 
#     since compile_command(and clangd) will not include such projects indexed in PATH
find_package(unofficial-sqlite3 CONFIG REQUIRED)

set(IncludeDir
  ${REQUIRED_PACKAGES_INCLUDE}
  ../Common/include
  ./include
)
include_directories(${IncludeDir})
message(STATUS "Found Include Path: ${IncludeDir}")

# build directory have some cpp file, so only find src file in src folder.
file(GLOB_RECURSE SourceCodes
  "src/*.cc" "src/*.cpp" "src/*.cxx"
)
message(STATUS "Found SourceCodes: ${SourceCodes}")

# Build executable. link library needed in target_link_libraries

link_directories(../Lib)
add_executable(${CMAKE_PROJECT_NAME} ${SourceCodes})
target_link_libraries(${CMAKE_PROJECT_NAME} 
  PUBLIC
  "$<LINK_GROUP:RESCAN,${REQUIRED_PACKAGES}>" # no need to handle the dependency of requried_packages
  ${LLVM_COLLECTION}
  OpenMP::OpenMP_CXX
  unofficial::sqlite3::sqlite3
)
