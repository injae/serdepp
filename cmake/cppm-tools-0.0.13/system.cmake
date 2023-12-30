macro(home_directory) # return HOME
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        set(env_home "$ENV{USERPROFILE}")
    else()
        set(env_home "$ENV{HOME}")
    endif()
    string(REPLACE "\\" "/" HOME "${env_home}")
endmacro()

