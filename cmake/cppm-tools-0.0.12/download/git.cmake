function(git_clone)
    cmake_parse_arguments(ARG "QUIET" "URL;BRANCH;PATH" "" ${ARGN})
    list(LENGTH ARG_UNPARSED_ARGUMENTS size)
    if(${size} LESS 1)
        message(FATAL_ERROR "You must provide a name")
    endif()
    list(GET ARG_UNPARSED_ARGUMENTS 0 name)

    find_package(Git REQUIRED)
    if(NOT GIT_FOUND)
        message(FATAL_ERROR "git not found!")
    endif()

    if(NOT ARG_URL)
        message(FATAL_ERROR "You must provide a git url")
    endif()

    if(NOT ARG_BRANCH)
        set(ARG_BRANCH main)
    endif()

    if(NOT ARG_PATH)
        set(ARG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(NOT EXISTS ${ARG_PATH})
        make_directory(${ARG_PATH})
    endif()

    if(EXISTS ${ARG_PATH}/${name})
        if(NOT ARG_QUIET)
            cppm_print("Updating ${name}")
        endif()
            execute_process(
                COMMAND ${GIT_EXECUTABLE} fetch --all
                COMMAND ${GIT_EXECUTABLE} reset --hard origin/${ARG_BRANCH}
                WORKING_DIRECTORY ${ARG_PATH}/${name}
                OUTPUT_VARIABLE output
                )
    else()
        if(NOT ARG_QUIET)
            cppm_print("Downloading ${name}")
        endif()
            execute_process(
                COMMAND ${GIT_EXECUTABLE} clone ${ARG_URL} ${name} --recursive
                WORKING_DIRECTORY ${ARG_PATH}
                OUTPUT_VARIABLE output
                )
            if(NOT ARG_QUIET)
                message(STATUS "[cppm] ${output}")
                cppm_print("[cppm] ${output}")
            endif()

            if(NOT ${ARG_BRANCH} MATCHES "main")
                excute_process(
                    COMMAND ${GIT_EXCUTEABLE} checkout ${ARG_BRANCH}
                    WORKING_DIRECTORY ${ARG_PATH}/${name}
                    OUTPUT_VARIABLE output
                    )
            endif()
    endif()

    if(NOT ARG_QUIET)
        message(STATUS "[cppm] ${output}")
    endif()
endfunction()

function(hash_check)
    set(src_path ${ARGV0})
    set(cache_path ${ARGV1})
    set(hash_file "${cache_path}/hash.cmake")
    set(hash_matched FALSE PARENT_SCOPE)
    if(EXISTS ${hash_file})
        include(${hash_file})
        execute_process(COMMAND git fetch WORKING_DIRECTORY ${src_path})
        execute_process(
            COMMAND git rev-parse --short origin/HEAD
            RESULT_VARIABLE result
            OUTPUT_VARIABLE short_hash
            WORKING_DIRECTORY ${src_path}
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        cppkg_print("cache: ${GIT_HASH}, origin: ${short_hash}")
        if("${short_hash}" STREQUAL "${GIT_HASH}")
            set(hash_matched TRUE PARENT_SCOPE)
        else()
            cppkg_print("update version ${short_hash} -> ${GIT_HASH}")
        endif()
    endif()
endfunction()

function(write_hash)
    set(src_path ${ARGV0})
    set(cache_path ${ARGV1})
    set(hash_matched FALSE PARENT_SCOPE)
    if(EXISTS ${src_path})
        execute_process(COMMAND git fetch WORKING_DIRECTORY ${src_path})
        execute_process(
            COMMAND git rev-parse --short origin/HEAD
            RESULT_VARIABLE result
            OUTPUT_VARIABLE short_hash
            WORKING_DIRECTORY ${src_path}
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(hash_file "${cache_path}/hash.cmake")
        set(file_data "set(GIT_HASH \"${short_hash}\")")
        if(EXISTS ${hash_file})
            include(${hash_file})
            if("${short_hash}" STREQUAL "${GIT_HASH}")
                cppkg_print("cache matched ${cache_path}")
                set(hash_matched TRUE PARENT_SCOPE)
            else()
                file(WRITE "${hash_file}" "${file_data}")
                cppkg_print("updated ${cache_path}")
            endif()
        else()
            file(WRITE "${hash_file}" "${file_data}")
            cppkg_print("updated ${cache_path}")
        endif()
    else()
        cppkg_print("updated ${cache_path}")
    endif()
endfunction()
