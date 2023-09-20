cmake_minimum_required(VERSION 3.26)

project(ModuleLoader)
if(${CMAKE_PROJECT_NAME} STREQUAL DefaultModule)
    message(FATAL_ERROR "Please change ProjectName")
endif()
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 20)

message(STATUS "Current path: ${CMAKE_CURRENT_SOURCE_DIR}")


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
add_library (${CMAKE_PROJECT_NAME} STATIC ${SourceCodes})


# copy static library to Lib
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} 
                   POST_BUILD 
                   COMMAND cp ./lib${CMAKE_PROJECT_NAME}.a ../../Lib/
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
)
