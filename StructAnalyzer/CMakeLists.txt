cmake_minimum_required(VERSION 3.26)

project(StructAnalyzer)
if(${CMAKE_PROJECT_NAME} STREQUAL DefaultExec)
    message(FATAL_ERROR "Please change ProjectName")
endif()
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 20)

message(STATUS "Current path: ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
set(LLVM_COLLECTION
  LLVMAsmParser LLVMSupport LLVMCore LLVMAnalysis LLVMIRReader
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  message(STATUS "OpenMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# other package in this dictionary
set(REQUIRED_PACKAGES ModuleLoader ModuleAnalyzer)
set(REQUIRED_PACKAGES_INCLUDE "")
list(TRANSFORM REQUIRED_PACKAGES PREPEND ../ OUTPUT_VARIABLE REQUIRED_PACKAGES_INCLUDE)
list(TRANSFORM REQUIRED_PACKAGES_INCLUDE APPEND /include)


# third-party project
include_directories( AFTER "/root/my_envs/sqlite3/include")
link_directories( AFTER "/root/my_envs/sqlite3/lib")

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
  sqlite3
)
