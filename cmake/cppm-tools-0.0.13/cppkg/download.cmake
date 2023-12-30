macro(download_package)
    set(options LOCAL GLOBAL)
    set(oneValueArgs URL GIT GIT_TAG SHA256 TYPE)
    set(multiValueArgs CMAKE_ARGS CONFIGURE_CMD BUILD_CMD INSTALL_CMD)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    list(GET ARG_UNPARSED_ARGUMENTS 0 name)
    list(GET ARG_UNPARSED_ARGUMENTS 1 version)
    list(REMOVE_AT ARG_UNPARSED_ARGUMENTS 0 1)

    cppm_setting(NO_MESSAGE)

    set(_version ${version})
    if(version STREQUAL "git")
      set(version "")
      set(_is_git TRUE)
    endif()

    include(${CPPM_TOOL}/cppkg/search_cppkg)
    search_cppkg(${name} ${ARG_TYPE} VERSION ${version})
    if(NOT ${name}_found)
        set(_recompile TRUE)
        cppkg_print("Can not find ${name} package")
    endif()

    if(ARG_SHA256)
        set(HASH URL_HASH SHA256=${ARG_SHA256})
    else()
        if(NOT _is_git)
            cppkg_print("If use SHA256 option, can use download cache")
        endif()
        set(HASH)
    endif()
    
    set(_cache_path ${CPPM_CACHE}/${name}/${_version})

    if(ARG_GLOBAL)
        set(OPTIONAL_ARGS "")
    else()
        set(OPTIONAL_ARGS "-DUSE_CPPM_PATH=ON")
        if(_is_git)
            set(OPTIONAL_ARGS "${OPTIONAL_ARGS};-DCPPKG_GIT_VERSION=ON")
            set(_source_path ${_cache_path}/src)
        else()
            set(OPTIONAL_ARGS "${OPTIONAL_ARGS};-DCPPM_PROJECT_VERSION_HINT=${version}")
            set(_source_path ${_cache_path}/src)
        endif()
    endif()
    _cppm_rpath()

    if(ARG_CONFIGURE_CMD)
        set(_configure_cmd ${ARG_CONFIGURE_CMD})
    else()
        set(_configure_cmd "")
    endif()

    if(ARG_BUILD_CMD)
        set(_build_cmd ${ARG_BUILD_CMD})
    else()
        set(_build_cmd "")
    endif()

    if(ARG_INSTALL_CMD)
        set(_install_cmd ${ARG_INSTALL_CMD})
    else()
        set(_install_cmd ${CMAKE_COMMAND} --build . --target install --target cppm_link --config ${CMAKE_BUILD_TYPE})
    endif()

    include(ExternalProject)
    if(_recompile OR _is_git)
        if(NOT EXISTS ${_cache_path})
            file(MAKE_DIRECTORY ${_cache_path})
        endif()
        if(_is_git)
            include(${CPPM_TOOL}/download/git)
            hash_check(${_source_path} ${_cache_path})
        endif()
        set(_binary_directory ${_cache_path}/build/${cppm_build_type}-${cppm_generator_type})
        if(NOT hash_matched OR (NOT EXISTS ${_binary_directory} OR (_recompile)))
            cppkg_print("Download ${name} package")
            cppkg_print("Cache Direcroty ${_cache_path}")
            set(_cmake_file_api "${_binary_directory}/.cmake/api/v1/query")
            file(MAKE_DIRECTORY "${_cmake_file_api}")
            file(TOUCH "${_cmake_file_api}/cache-v2")
            file(TOUCH "${_cmake_file_api}/codemodel-v2")
            file(TOUCH "${_cmake_file_api}/toolchains-v1")
            file(TOUCH "${_cmake_file_api}/cmakeFiles-v1")
            ExternalProject_Add(
                _${name}
                URL ${ARG_URL}
                ${HASH}
                ${CH_URL_HASH}
                GIT_REPOSITORY ${ARG_GIT}
                GIT_TAG ${ARG_GIT_TAG}
                DOWNLOAD_DIR ${_cache_path}
                DOWNLOAD_NO_PROGRESS TRUE
                SOURCE_DIR ${_source_path}
                BINARY_DIR ${_binary_directory}
                CMAKE_ARGS
                    ${CMAKE_ARGS}
                    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
                    "${OPTIONAL_ARGS}"
                    ${ARG_CMAKE_ARGS}
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -G ${CMAKE_GENERATOR} -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                CONFIGURE_COMMAND ${_configure_cmd}
                BUILD_COMMAND ${_build_cmd}
                INSTALL_COMMAND ${_install_cmd}
                STEP_TARGETS download
                ${ARG_UNPARSED_ARGUMENTS}
            )

            if(_is_git)
                write_hash(${_source_path} ${_cache_path})
            endif()
        else()
            cppkg_print("load cache: ${_binary_directory}")
        endif()
    endif()
endmacro()


#if(EXISTS ${_cache_path}/hash.cmake)
#    include(${_cache_path}/hash.cmake)
#    set(CH_URL_HASH URL_HASH SHA256=${URL_HASH})
#else()
#    set(CH_URL_HASH)
#endif()

#    ExternalProject_Get_Property(_${name} DOWNLOADED_FILE)
#    set(hash_file ${_cache_path}/hash.cmake)
#    file(WRITE ${_cache_path}/gen_hash.cmake
#    "file(MD5 ${DOWNLOADED_FILE} _file_hash)
#    \nset(file_data \"set(URL_HASH \$\{_file_hash\})\")
#    \nfile(WRITE \"${hash_file}\" \"\$\{file_data\}\")\n")
#    add_custom_command(TARGET _${name}
#    COMMAND cmake -P gen_hash.cmake
#    WORKING_DIRECTORY ${_cache_path}
#    DEPEND _${name}-download

#string(REPLACE ";" "|" CMAKE_PREFIX_PATH_ALT_SEP "${CMAKE_PREFIX_PATH}")
#        LIST_SEPARATOR |
#-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_ALT_SEP}

#            install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin/" DESTINATION ${CPPM_PREFIX}/bin USE_SOURCE_PERMISSIONS) 
