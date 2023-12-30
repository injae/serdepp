mark_as_advanced(CMAKE_TOOLCHAIN_FILE)
#message("--- toolchain ${CMAKE_PROJECT_NAME}:${CMAKE_PROJECT_VERSION}")
# IF CMAKE_COMPILER_CHECK_TIME LOAD TOOLCHAIN CMAKE_PROJECT_NAME IS CMAKE_TRY_COMPILE
# TOOLCHAIN FILE CALLED FROM ALL COMPILER CHECHK SEQUENCE
# Need to toolchain variable caching

get_property(_CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE)

if(CPPM_LOAD)
    return()
endif()

if(CPPM_EXTERNAL_TOOLCHAIN_FILE)
    include("${CPPM_EXTERNAL_TOOLCHAIN_FILE}")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR (NOT CMAKE_SYSTEM_NAME AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows"))
    set(env_home "$ENV{USERPROFILE}")
else()
    set(env_home "$ENV{HOME}")
endif()
string(REPLACE "\\" "/" HOME "${env_home}")
get_filename_component(CPPM_CORE "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(CPPM_TOOL "${CPPM_CORE}" NAME)
list(APPEND CMAKE_MODULE_PATH "${HOME}/.cppm/cmake")
include(${CPPM_TOOL}/utility/set_cmake_cache)
cppm_set(CPPM_ROOT    "${HOME}/.cppm") #
cppm_set(CPPM_TOOL    "${CPPM_TOOL}") #
cppm_set(CPPM_PREFIX  "${CPPM_ROOT}")
cppm_set(CPPM_MODULE  "${CPPM_ROOT}/cmake")
cppm_set(CPPM_PKGS    "${CPPM_PREFIX}/cppkg")
cppm_set(CPPM_CACHE   "${CPPM_ROOT}/cache")
cppm_set(CPPM_CORE    "${CPPM_CORE}")
cppm_set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")

include(${CPPM_TOOL}/cppm/setting)
include(${CPPM_TOOL}/utility/cppm_print)

default_cache(USE_CPPM_PATH OFF BOOL)
default_cache(CPPKG_GIT_VERSION OFF BOOL)
mark_as_advanced(USE_CPPM_PATH)
mark_as_advanced(CPPKG_GIT_VERSION)
#mark_as_advanced(CPPM_PROJECT_VERSION_HINT) cmake project version not settinged project set version

_cppm_arch_flag()
_cppm_os_flag()
_cppm_generator()
_cppm_set_prefix()
_cppm_rpath()
_cppkg_define_property()
_cppm_ccache()
cppm_set(CMAKE_PROJECT_INCLUDE ${CPPM_CORE}/add_cppm_target.cmake)

#cppm_create_symlink(${CMAKE_INSTALL_PREFIX})
cppm_set(CPPM_LOAD ON)

list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
            CPPM_ROOT
            CPPM_CORE
            cppm_target_arch
            cppm_target_base_platform
            cppm_target_platform
            CPPM_EXTERNAL_TOOLCHAIN_FILE
            CMAKE_PREFIX_PATH
            CMAKE_FIND_ROOT_PATH
        )

## with vcpkg
default_cache(WITH_VCPKG OFF BOOL)
if(WITH_VCPKG)
    if(DEFINED ENV{VCPKG_ROOT})
        set(vcpkg_toolchains "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        include("${vcpkg_toolchains}")
        set(cppm_detect_vcpkg True)
    else()
        if(NOT "${vcpkg_exe}" STREQUAL "vcpkg_exe-NOTFOUND")
            get_filename_component(vcpkg_path ${vcpkg_exe} DIRECTORY CACHE)
            set(vcpkg_toolchains "${vcpkg_path}/scripts/buildsystems/vcpkg.cmake")
            if(EXISTS ${vcpkg_toolchains})
                cppm_set(cppm_detect_vcpkg TRUE)
                include("${vcpkg_toolchains}")
            else()
                cppm_set(cppm_detect_vcpkg FALSE)
            endif()

            cppm_set(cppm_detect_vcpkg FALSE)
        endif()
    endif()
endif()
