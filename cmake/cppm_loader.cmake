if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR (NOT CMAKE_SYSTEM_NAME AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows"))
    set(env_home "$ENV{USERPROFILE}")
else()
    set(env_home "$ENV{HOME}")
endif()
string(REPLACE "\\" "/" HOME "${env_home}")

set(CPPM_ROOT          ${HOME}/.cppm)
set(CPPM_LOADER_CACHE  ${CPPM_ROOT}/cache/cppm-loader/git)
set(CPPM_LOADER_PATH   ${CPPM_ROOT}/cmake/cppm-loader)

option(ENABLE_INLINE_CPPM_TOOLS OFF)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cppm-version.cmake AND (ENABLE_INLINE_CPPM_TOOLS))
    include(cmake/cppm-version.cmake)
    set(CPPM_TOOL "cppm-tools-${CPPM_TOOLS_VERSION}")
    set(CPPM_CORE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CPPM_TOOL})
    list(APPEND CMAKE_MODULE_PATH "${CPPM_CORE}")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CPPM_TOOL}/core_load.cmake)
else()
    if(NOT DEFINED IS_CPPM_LOADER_LOADED)
        set(_install_script "${CPPM_LOADER_CACHE}/install-script")
        file(WRITE ${_install_script}/CMakeLists.txt
        "cmake_minimum_required(VERSION 3.12)
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

if(ENABLE_INLINE_CPPM_TOOLS)
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cppm-version.cmake"
        "set(CPPM_TOOLS_VERSION ${CPPM_TOOLS_VERSION})"
    )
    file(COPY ${CPPM_CORE} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    message("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CPPM_TOOL}/.git")
    file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CPPM_TOOL}/.git")
endif()
    #include(FetchContent)
    #FetchContent_Declare(
    #    cppm_loader_toolchains
    #    GIT_REPOSITORY https://github.com/injae/cppm-loader.git
    #)
    #
    #FetchContent_MakeAvailable(cppm_loader_toolchains)
    #include(${CMAKE_CURRENT_BINARY_DIR}/_deps/cppm_loader_toolchains-src/load.cmake)

endif()

