function(add_cppkg_info name)
    cmake_parse_arguments(ARG "" "VERSION;DEPEND;" "MODULE" ${ARGN})
    if(TARGET ${name}_info)
    else()
        add_custom_target(${name}_info COMMENT "Cppkg Info Target")
    endif()

    set_target_properties(${name}_info PROPERTIES
        CPPM_MODULE "${ARG_MODULE}"
        CPPM_VERSION "${ARG_VERSION}"
        CPPM_DEPEND  "${ARG_DEPEND}"
        CPPM_DESCRIPTION "${name}/${ARG_VERSION}"
    )
endfunction()


function(find_cppkg)
    cmake_parse_arguments(ARG "HUNTER;SYSTEM" "TYPE;OPTIONAL" "MODULE;COMPONENTS;LOADPATH" ${ARGN})
    list(GET ARG_UNPARSED_ARGUMENTS 0 name)
    list(GET ARG_UNPARSED_ARGUMENTS 1 version)
    set(version_ ${version})

    message("==>[[${name}]]")
    if(version STREQUAL "latest" OR (version STREQUAL "git"))
        if(version STREQUAL "git")
            set(GIT_MODULE PATHS "${CPPKG_PKGS}/lib-cppkg-git/${name}" NO_DEFAULT_PATH)
        endif()
        set(version "")
        set(_recompile TRUE)
    else()
        set(_recompile FALSE)
    endif()

    if(DEFINED ARG_COMPONENTS)
        set(component_script COMPONENTS ${ARG_COMPONENTS})
    else()
        set(component_script)
    endif()


    set(small_opt_var "${PROJECT_NAME}_use_${name}")
    string(TOUPPER ${small_opt_var} optional_variable) 
    if(DEFINED ARG_OPTIONAL)
        option(${optional_variable} "Optional Dependency Flag: ${name}" ${ARG_OPTIONAL})
    else()
        option(${optional_variable} "Optional Dependency Flag: ${name}" ON)
    endif()

    if(${optional_variable})
        set(_is_can_use TRUE)
    else()
        set(_is_can_use FALSE)
        cppkg_print("Optional Package OFF ${name}, Flag: ${optional_variable}=ON|OFF")
        message("<==[[${name}]]")
        return()
    endif()

    if(ARG_HUNTER) 
        hunter_add_package(${name} ${component_script})
        cppm_print("Load Package ${name} from Hunter")
    endif()

    include(${CPPM_TOOL}/cppkg/search_cppkg)
    search_cppkg(${name} ${component_script} ${ARG_TYPE} VERSION ${version} ${GIT_MODULE})
    if(NOT ${name}_found)
        set(_recompile TRUE)
        if(ARG_SYSTEM)
            cppm_error_print("Can't find Package ${name}/${${name}_VERSION}")
            return()
        endif(ARG_SYSTEM)
    endif()

    cppm_set_if(_recompile "cppm_build_type_change" "TRUE")
    if(NOT cppm_generator_type STREQUAL "visual")
        set(CPPKG_INSTALL_BUILD_TYPE "Release")
    else()
        set(CPPKG_INSTALL_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    endif()

    set(_cppkg "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/${name}/${version_}/${name}.cmake.in") 
    if(EXISTS ${_cppkg})
        if(_recompile)
            configure_file(thirdparty/${name}/${version_}/${name}.cmake.in
                           ${CMAKE_BINARY_DIR}/thirdparty/${name}/${version_}/CMakeLists.txt)
            execute_process(COMMAND
                            ${CMAKE_COMMAND}
                            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
                            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
                            "-DCMAKE_BUILD_TYPE=${CPPKG_INSTALL_BUILD_TYPE}" .
                            RESULT_VARIABLE result
                            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/${name}/${version_})
            execute_process(COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE}
                            RESULT_VARIABLE result
                            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/${name}/${version_})
        endif()
    endif()

    if(DEFINED ARG_LOADPATH)
        set(ROOT_CPPM_VERSION ${CPPM_VERSION})
        add_cppkg_info(${name}
             MODULE  "${ARG_MODULE}"
             VERSION "${version_}"
             DEPEND  "${PROJECT_NAME}"
        )
        set_target_properties(${name}_info PROPERTIES CPPM_LOADPATH "${ARG_LOADPATH}")
        if(NOT ARG_LOADPATH MATCHES "^\.\./.*$") # out of tree dependency(workspace) use this option
            add_subdirectory(${ARG_LOADPATH})
        else()
            cppkg_print("Load Workspace ${name}/${version_} from ${ARG_LOADPATH}")
        endif()
        set(CPPM_VERSION ${ROOT_CPPM_VERSION})
    elseif("${ARG_TYPE}" STREQUAL "bin")
        search_cppkg(${name} ${component_script} ${ARG_TYPE} VERSION ${version} ${GIT_MODULE})
        if(${name}_found)
            cppkg_print("Find Binary: ${name} from ${${name}_path}")
        else()
            cppm_error_print("Can't find binary ${name}")
        endif()
    else()
        search_cppkg(${name} ${component_script} ${ARG_TYPE} VERSION ${version} ${GIT_MODULE})
        if(NOT ${name}_found)
            cppkg_print("Can't find Package: ${name}/${version} from Cppkg")
            search_cppkg(${name} ${component_script} ${ARG_TYPE} ${GIT_MODULE})
            if(${name}_found)
                cppkg_print("Find Alternative Package: ${name}/${${name}_VERSION} from Cppkg")
            endif()
        endif()
        if(${name}_found)
            cppkg_print("Load Package: ${name}/${${name}_VERSION} from Cppkg")
            add_cppkg_info(${name}
                MODULE  "${ARG_MODULE}"
                VERSION "${${name}_VERSION}"
                DEPEND  "${PROJECT_NAME}"
            )
        else()
            cppm_error_print("Can't find Package ${name}/${${name}_VERSION}")
        endif()
    endif()
    message("<==[[${name}]]")
endfunction()


        #file(SHA256 ${_cppkg} _cppkg_hash)
        #set_cache_check(${PROJECT_NAME}_${name}_${version_}_hash ${_cppkg_hash} STRING)
        #if(NOT _is_same OR (_recompile))
