cmake_minimum_required(VERSION 3.19)
project(mysqlorm)

set(CMAKE_CXX_STANDARD 11)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(./)

# 包含 mysql 头文件所在目录
#include_directories("D:/Apps/MySQL/mysql-8.0.30-winx64/include/")
include_directories("D:/MySQL/include/")

file(GLOB_RECURSE SOURCES "utils/*.cpp" "myorm/*.cpp" "models/*.cpp")

# 链接 mysql 库文件
# link_directories("D:/Apps/MySQL/mysql-8.0.30-winx64/lib/")
link_directories("D:/MySQL/lib/")
link_libraries(libmysql)

#add_executable(main ${SOURCES} main.cpp main.cpp myorm/batch.h myorm/connection.h myorm/connection.cpp myorm/connection_pool.h myorm/connection_pool.cpp myorm/database.h myorm/database.cpp myorm/model.h myorm/query.h myorm/transaction.h myorm/transaction.cpp)
add_executable(main ${SOURCES} main.cpp)
