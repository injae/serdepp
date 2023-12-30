if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR (NOT CMAKE_SYSTEM_NAME AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows"))
    set(env_home "$ENV{USERPROFILE}")
else()
    set(env_home "$ENV{HOME}")
endif()
string(REPLACE "\\" "/" HOME "${env_home}")
set(CPPM_ROOT ${HOME}/.cppm)

cppm_download_package(cppkg
    GIT https://github.com/injae/cppkg.git
    PATH ${CPPM_ROOT}/repo/cppkg
)
