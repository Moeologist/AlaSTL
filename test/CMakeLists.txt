cmake_minimum_required(VERSION 3.10)
project(alaSTL_TEST VERSION 0.0.1 LANGUAGES CXX)

option(ALASTL_USE_CXX14 "alaSTL use c++14" OFF)

if(ALASTL_USE_CXX14)
    set(CMAKE_CXX_STANDARD 14)
else()
    set(CMAKE_CXX_STANDARD 17)
endif()

execute_process(COMMAND git log --pretty=format:"%H" -n 1
                OUTPUT_VARIABLE GIT_COMMIT
                ERROR_QUIET)
string(REPLACE "\'" "\"" GIT_COMMIT ${GIT_COMMIT})
configure_file(TestConfig.h.in TestConfig.h)

include_directories("${PROJECT_SOURCE_DIR}/../include")
include_directories("${PROJECT_SOURCE_DIR}/support")
add_compile_options("-stdlib=libc++")

enable_testing()

function(add_test_file)
    cmake_parse_arguments(_ "" "" "FILE;ARGS" "" ${ARGN})
    if(NOT DEFINED __FILE)
        message(WARNING "add_test_file FILE not defined")
        return()
    endif()
    get_filename_component(BASENAME ${__FILE} NAME_WLE)
    get_filename_component(EXTNAME ${__FILE} EXT)
    string(SHA1 HASH ${__FILE})
    string(SUBSTRING ${HASH} 0 8 H)
    set(ID "${H}")

    add_executable(${ID} ${__FILE})
    add_test(NAME ${ID} COMMAND ${ID} ${__ARGS})
    install(TARGETS ${ID} DESTINATION bin)
endfunction()

file(GLOB_RECURSE LIBCXX_TEST_SRC_FILES RELATIVE "${PROJECT_SOURCE_DIR}/" "std/*.cpp")

foreach(f ${LIBCXX_TEST_SRC_FILES})
    add_test_file(FILE ${f})
endforeach()

install(FILES TestCongig.h DESTINATION include)
