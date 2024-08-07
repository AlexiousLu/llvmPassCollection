cmake_minimum_required(VERSION 3.26)

project(AliasAnalyzer)
if(${CMAKE_PROJECT_NAME} STREQUAL DefaultModule)
    message(FATAL_ERROR "Please change ProjectName")
endif()
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 20)

message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_ROOT = ${CMAKE_ROOT}")
message(STATUS "Current path: ${CMAKE_CURRENT_SOURCE_DIR}")


find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
set(LLVM_COLLECTION
  LLVMAsmParser LLVMSupport LLVMCore LLVMAnalysis LLVMIRReader
)

set(REQUIRED_PACKAGES ModuleLoader)
set(REQUIRED_PACKAGES_INCLUDE "")
list(TRANSFORM REQUIRED_PACKAGES PREPEND ../ OUTPUT_VARIABLE REQUIRED_PACKAGES_INCLUDE)
list(TRANSFORM REQUIRED_PACKAGES_INCLUDE APPEND /include)


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

# Build libraries.
link_directories(../Lib)
# add_compile_options(-fPIC)
add_library (${CMAKE_PROJECT_NAME} SHARED ${SourceCodes})
target_link_libraries(${CMAKE_PROJECT_NAME}
  PUBLIC
  "$<LINK_GROUP:RESCAN,${REQUIRED_PACKAGES}>"
  ${LLVM_COLLECTION}
)



# copy SHARED library to Lib
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} 
                   POST_BUILD 
                   COMMAND cp ./lib${CMAKE_PROJECT_NAME}.so ../../Lib/
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
)
