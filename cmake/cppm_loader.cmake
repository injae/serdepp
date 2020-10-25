if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR (NOT CMAKE_SYSTEM_NAME AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows"))
    set(env_home "$ENV{USERPROFILE}")
else()
    set(env_home "$ENV{HOME}")
endif()
string(REPLACE "\\" "/" HOME "${env_home}")

set(CPPM_ROOT          ${HOME}/.cppm)
set(CPPM_LOADER_CACHE  ${CPPM_ROOT}/cache/cppm-loader/git)
set(CPPM_LOADER_PATH   ${CPPM_ROOT}/cmake/cppm-loader)

if(NOT DEFINED IS_CPPM_LOADER_LOADED)
set(_install_script "${CPPM_LOADER_CACHE}/install-script")
file(WRITE ${_install_script}/CMakeLists.txt
"cmake_minimum_required(VERSION 3.6)
project(CPPM_LOADER_DOWNLOAD NONE)
include(ExternalProject)
ExternalProject_Add(cppm-loader
    GIT_REPOSITORY https://github.com/injae/cppm-loader.git
    SOURCE_DIR ${CPPM_LOADER_PATH}
    BINARY_DIR ${CPPM_LOADER_CACHE}/build
    CONFIGURE_COMMAND \"\"
    BUILD_COMMAND  \"\"
    INSTALL_COMMAND \"\"
)"
)
execute_process(COMMAND cmake . WORKING_DIRECTORY ${_install_script} OUTPUT_QUIET)
execute_process(COMMAND cmake  --build . WORKING_DIRECTORY ${_install_script} OUTPUT_QUIET)
endif()
include(${CPPM_LOADER_PATH}/load.cmake)
