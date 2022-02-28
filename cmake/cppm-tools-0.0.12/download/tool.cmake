function(cppm_download_package)
    cmake_parse_arguments(ARG "" "GIT;GIT_TAG;VERSION;URL;PATH;INSTALL_SCRIPT" "" ${ARGN})
    list(LENGTH ARG_UNPARSED_ARGUMENTS size)
    if(${size} LESS 1)
        message(FATAL_ERROR "You must provide a name")
    endif()
    list(GET ARG_UNPARSED_ARGUMENTS 0 name)

    if(DEFINED ARG_VERSION)
        set(VERSION ${ARG_VERSION})
    else()
        if(DEFINED ARG_GIT)
            if(DEFINED ARG_GIT_TAG)
                set(VERSION ${ARG_GIT_TAG})
            else()
                set(VERSION git)
            endif()
        else()
            set(VERSION unknown)
        endif()
    endif()

    if(NOT DEFINED ARG_PATH)
        set(ARG_PATH ${CPPM_CACHE}/${name}/${VERSION}/src)
    endif()

    set(_install_script "${CPPM_CACHE}/${name}/${VERSION}/install-script")
    file(WRITE "${_install_script}/CMakeLists.txt"
        "cmake_minimum_required(VERSION 3.2)\n"
        "project(CPPM_TOOLS_DOWNLOAD NONE)\n"
        "include(ExternalProject)\n"
        "ExternalProject_Add(\n"
        "    ${name}\n"
        "    GIT_REPOSITORY ${ARG_GIT}\n"
        "    GIT_TAG        ${ARG_GIT_TAG}"
        "    URL            ${ARG_URL}"
        "    SOURCE_DIR     ${ARG_PATH}"
        "    BINARY_DIR     ${CPPM_CACHE}/${name}/${VERSION}/build"
        "    CONFIGURE_COMMAND \"${INSTALL_SCRIPT}\""
        "    BUILD_COMMAND \"\"\n"
        "    INSTALL_COMMAND \"\"\n"
        ")\n"
    )
    execute_process(COMMAND cmake . WORKING_DIRECTORY ${_install_script} OUTPUT_QUIET)
    execute_process(COMMAND cmake  --build . WORKING_DIRECTORY ${_install_script} OUTPUT_QUIET)
    if(ARG_INSTALL_SCRIPT)
        execute_process(COMMAND ${ARG_INSTALL_SCRIPT} WORKING_DIRECTORY ${ARG_PATH})
    endif()
    message(STATUS "[cppm] ${name} download to ${ARG_PATH}")
endfunction()
