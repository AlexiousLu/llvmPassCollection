cmake_minimum_required(VERSION 3.26)

project(ModuleLoader)
if(${CMAKE_PROJECT_NAME} STREQUAL DefaultModule)
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

set(IncludeDir
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
  ${LLVM_COLLECTION}
  OpenMP::OpenMP_CXX
)

# copy SHARED library to Lib
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} 
                   POST_BUILD 
                   COMMAND cp ./lib${CMAKE_PROJECT_NAME}.so ../../Lib/
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
)
