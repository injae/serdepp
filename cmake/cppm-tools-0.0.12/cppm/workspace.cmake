macro(cppm_workspace)
    cmake_parse_arguments(ARG "" "PATH" "" ${ARGN})
    add_subdirectory(${ARG_PATH})

endmacro()


