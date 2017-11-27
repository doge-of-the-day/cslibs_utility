cmake_minimum_required(VERSION 2.8.3)
project(cslibs_utility)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(cmake/cslibs_utility_enable_c++11.cmake)
include(cmake/cslibs_utility_show_headers.cmake)

find_package(catkin QUIET)
if(${catkin_FOUND})
catkin_package(
    INCLUDE_DIRS    include
    CFG_EXTRAS      cslibs_utility_enable_c++11.cmake
                    cslibs_utility_show_headers.cmake
)
install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
endif()

include_directories(
    include
)

cslibs_utility_show_headers()
