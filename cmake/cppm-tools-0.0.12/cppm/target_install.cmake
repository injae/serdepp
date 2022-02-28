function(cppm_write_target_dependency_file)
    cmake_parse_arguments(ARG "" "" "" ${ARGN})
    list(GET ARG_UNPARSED_ARGUMENTS 0 name)
    set(Deps "")
    get_target_property(deps ${name}_info CPPM_DEPENDENCIES)
    
    if(NOT deps MATCHES "deps-NOTFOUND")
        foreach(dep IN LISTS deps)
            set(Deps "${Deps}find_dependency(${dep})\n")
        endforeach()
    endif()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        "get_filename_component(CPPM_CURRENT_MODULE_DIR ../ ABSOLUTE)\n"
        "list(APPEND CMAKE_PREFIX_PATH \"$\{CPPM_CURRENT_MODULE_DIR}\")\n"
        "list(APPEND CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})\n"
        "include(CMakeFindDependencyMacro)\n"
        "${Deps}\n"
        "include(\$\{CMAKE_CURRENT_LIST_DIR\}/${PROJECT_NAME}-targets.cmake)\n"
    )
endfunction()

function(cppm_target_install)
    cmake_parse_arguments(ARG "" "" "" ${ARGN})
    list(GET ARG_UNPARSED_ARGUMENTS 0 name)
    if(TARGET ${name})
        get_target_property(_target_type ${name}_info CPPM_TYPE)
        if(_target_type MATCHES "BINARY")
            install(TARGETS ${name} RUNTIME DESTINATION bin) # $HOME/.cppm/local/share/${name}-${version}
            get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_INSTALL_PREFIX}/../../" ABSOLUTE)
            if("${PACKAGE_PREFIX_DIR}" STREQUAL "${CPPM_PREFIX}" AND (CMAKE_BUILD_TYPE MATCHES "Release"))
                if(NOT (EXISTS "{CPPM_PREFIX}/bin"))
                    file(MAKE_DIRECTORY ${CPPM_PREFIX}/bin)
                endif()
            endif()
        else()
            get_target_property(_namespace ${name}_info CPPM_NAMESPACE)
            include(CMakePackageConfigHelpers)
            write_basic_package_version_file(
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                VERSION ${${PROJECT_NAME}_VERSION}
                COMPATIBILITY SameMinorVersion # 0.2 != 0.3, 0.2 == 0.2.5
            ) 
            install(FILES
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                DESTINATION lib/cmake/${PROJECT_NAME}
            )
            cppm_write_target_dependency_file(${name})

            install(FILES
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
                DESTINATION lib/cmake/${PROJECT_NAME}
            )

            # project-targets.cmake install part
            install(TARGETS ${name} EXPORT ${PROJECT_NAME}-targets
                ARCHIVE  DESTINATION lib 
                LIBRARY  DESTINATION lib
                RUNTIME  DESTINATION bin
                INCLUDES DESTINATION include
            )
            install(DIRECTORY include/ DESTINATION include)

            install(EXPORT ${PROJECT_NAME}-targets
                FILE ${PROJECT_NAME}-targets.cmake
                NAMESPACE ${_namespace}::
                DESTINATION lib/cmake/${PROJECT_NAME}
            )

            message(STATUS "[cppm] Export CMake Module: ${_namespace}::${name}")
        endif()
    endif()
endfunction()

#add_custom_command(TARGET ${name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_INSTALL_PREFIX}/bin/$<TARGET_FILE_BASE_NAME:${name}>" "${CPPM_PREFIX}/bin/$<TARGET_FILE_BASE_NAME:${name}>" COMMENT "-- Linking ${CMAKE_INSTALL_PREFIX}/bin -> ${CPPM_PREFIX}/bin/")
