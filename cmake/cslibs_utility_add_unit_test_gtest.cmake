# ${PROJECT_NAME}_add_unit_test_gtest(<NAME_OF_THE_TEST>
#    [<INCLUDE_DIRS> ...]
#    [<SOURCE_FILES> ...]
#    [<LINK_LIBRARIES> ...]
#    [<COMPILE_FLAGS> ...]
#)
function(${PROJECT_NAME}_add_unit_test_gtest)
    cmake_parse_arguments(unit_test
        # list of names of the boolean arguments (only defined ones will be true)
        ""
        # list of names of mono-valued arguments
        ""
        # list of names of multi-valued arguments (output variables are lists)
        "INCLUDE_DIRS;SOURCE_FILES;LINK_LIBRARIES;COMPILE_OPTIONS"
        # arguments of the function to parse, here we take the all original ones
        ${ARGN}
    )

    if(${catkin_FOUND})
        set(unit_test_NAME ${ARGV0})
        find_package(Boost REQUIRED COMPONENTS system)

        catkin_add_gtest(${unit_test_NAME}
            ${unit_test_SOURCE_FILES}
        )

        target_include_directories(${unit_test_NAME}
            PRIVATE
                ${unit_test_INCLUDE_DIRS}
                ${Boost_INCLUDE_DIRS}
        )

        target_link_libraries(${unit_test_NAME}
            #PRIVATE
                ${unit_test_LINK_LIBRARIES}
                ${GTEST_LIBRARIES}
                ${Boost_LIBRARIES}
                -pthread
        )
        target_compile_options(${unit_test_NAME}
            PRIVATE
                ${unit_test_COMPILE_OPTIONS}
        )
    else()
        set(unit_test_NAME ${PROJECT_NAME}_${ARGV0})
        enable_testing()

        add_executable(${unit_test_NAME}
            ${unit_test_SOURCE_FILES}
        )

        target_include_directories(${unit_test_NAME}
            PRIVATE
                ${unit_test_INCLUDE_DIRS}
        )

        target_link_libraries(${unit_test_NAME}
            PRIVATE
                ${unit_test_LINK_LIBRARIES}
                ${GTEST_LIBRARIES}
                -pthread
        )
        target_compile_options(${unit_test_NAME}
            PRIVATE
                ${unit_test_COMPILE_OPTIONS}
        )
        add_test(AllTestsIn${unit_test_NAME} ${unit_test_NAME})
    endif()
endfunction()
