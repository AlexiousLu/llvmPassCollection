# set(CMAKE_TOOLCHAIN_FILE "/home/zhipeng/my_envs/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(ProjectName)
cmake_minimum_required(VERSION 3.22)

include("/home/zhipeng/my_envs/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(unofficial-libmysql REQUIRED)

# target_link_libraries(main PRIVATE unofficial::libmysql::libmysql)
include_directories(${MYSQL_INCLUDE_DIR})
link_directories(${MYSQL_LIB_DIR})