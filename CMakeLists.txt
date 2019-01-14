cmake_minimum_required(VERSION 3.9)
project(cqhttp-ext-long-polling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "/utf-8 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "/MP ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CQHTTP_PROJECT_DIR}/cmake/Modules/")

include(cotire)
include(FindVcpkgIncludeDir)
include(FixDebugLibraryLookup)

include_directories(${VCPKG_INCLUDE_DIR})
include_directories(${CQHTTP_PROJECT_DIR}/src)
include_directories(src)

add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
                -D_WIN32_WINNT=0x0501
                -DWIN32_LEAN_AND_MEAN
                -DNOMINMAX
                -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
                -D_SCL_SECURE_NO_WARNINGS)

include(FixLinkConflict)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
set(LIB_NAME "LongPolling")
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

cotire(${LIB_NAME})

add_custom_command(TARGET ${LIB_NAME}
                   POST_BUILD
                   COMMAND
                   powershell -ExecutionPolicy Bypass -NoProfile -File "${PROJECT_SOURCE_DIR}/scripts/post_build.ps1" ${LIB_NAME} "$<TARGET_FILE_DIR:${LIB_NAME}>")
