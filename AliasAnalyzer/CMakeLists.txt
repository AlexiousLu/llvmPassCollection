cmake_minimum_required(VERSION 3.22)

project(AliasAnalyzer)

include("../Common/cmake/common_cmake.cmake")

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
