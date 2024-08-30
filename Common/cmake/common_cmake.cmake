include("$ENV{MYENVS}/vcpkg/scripts/buildsystems/vcpkg.cmake")

include_directories("$ENV{MYENVS}/vcpkg/installed/x64-linux/include")

if(${CMAKE_PROJECT_NAME} STREQUAL DefaultModule)
    message(FATAL_ERROR "Please change ProjectName")
endif()
# set(CMAKE_C_COMPILER "gcc-8")
# set(CMAKE_CXX_COMPILER "g++-8")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 20)

message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_ROOT = ${CMAKE_ROOT}")
message(STATUS "Current path: ${CMAKE_CURRENT_SOURCE_DIR}")


find_package(LLVM 15.0.0 REQUIRED CONFIG) #  PATHS /home/zhipeng/my_envs/llvm-15 NO_DEFAULT_PATH
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Found LLVM include dir: ${LLVM_INCLUDE_DIRS}")
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
set(LLVM_COLLECTION
  LLVMAsmParser LLVMSupport LLVMCore LLVMAnalysis LLVMIRReader
)
