cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXXCOMPILE clang++)
set(CMAKE_CXX_STANDARD 23)

project(StuPidSQL)

add_compile_options("-g")
# add_compile_options("-O2")
add_link_options("-rdynamic")

aux_source_directory(src SRC)

add_executable(StuPidSQL ${SRC})

target_include_directories(StuPidSQL
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)