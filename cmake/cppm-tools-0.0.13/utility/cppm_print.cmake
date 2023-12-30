function(cppm_print)
    foreach(message ${ARGV})
        if(NOT ${PROJECT_NAME}_NO_MESSAGE)
            message(STATUS "[cppm] ${message}")
        endif()
    endforeach()
endfunction()

function(cppm_test_print)
    foreach(message ${ARGV})
        message(STATUS "[cppm-test] ${message}")
    endforeach()
endfunction()

function(cppkg_print)
    foreach(message ${ARGV})
        message(STATUS "[cppkg] ${message}")
    endforeach()
endfunction()


function(cppm_error_print)
    foreach(message ${ARGV})
            message(STATUS "[cppm-error] ${message}")
    endforeach()
    message(FATAL_ERROR "")
endfunction()

function(cppm_warning_print)
    foreach(message ${ARGV})
            message(WARNING "[cppm-warning] ${message}")
    endforeach()
endfunction()