set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(LIBS_LIST "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

macro(get_task_target VAR NAME)
    set(${VAR} task_${HW_NAME}_${TASK_NAME}_${NAME})
endmacro()

function(add_task_executable BINARY_NAME)
    set(BINARY_SOURCES ${ARGN})

    add_executable(${BINARY_NAME} ${BINARY_SOURCES} ${TASK_SOURCES})
    target_link_libraries(${BINARY_NAME} "pthread")
    if (LIBS_LIST)
        target_link_libraries(${BINARY_NAME} "${LIBS_LIST}")
        add_dependencies(${BINARY_NAME} "${LIBS_LIST}")
    endif()
endfunction()


macro(begin_task)
    set(TASK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    message(DEBUG "TASK_DIR = '${TASK_DIR}'")

    get_filename_component(TASK_NAME ${TASK_DIR} NAME)
    get_filename_component(HW_DIR ${TASK_DIR} DIRECTORY)
    get_filename_component(HW_NAME ${HW_DIR} NAME)

    message(STATUS "Homework = '${HW_NAME}', task = '${TASK_NAME}'")

    include_directories(${TASK_DIR})

    set(TEST_LIST "")
endmacro()


macro(task_link_libraries)
    list(APPEND LIBS_LIST ${ARGV})
endmacro()


macro(set_task_sources)
    prepend(TASK_SOURCES "${TASK_DIR}/" ${ARGV})
endmacro()



# Tests

function(add_task_test BINARY_NAME)
    get_task_target(TEST_NAME ${BINARY_NAME})

    prepend(TEST_SOURCES "${TASK_DIR}/" ${ARGN})
    add_task_executable(${TEST_NAME} ${TEST_SOURCES})

    # Append test to TEST_LIST
    list(APPEND TEST_LIST ${TEST_NAME})
    set(TEST_LIST "${TEST_LIST}" PARENT_SCOPE)
endfunction()

function(add_task_all_tests_target)
    get_task_target(ALL_TESTS_TARGET "run_all_tests")
    run_chain(${ALL_TESTS_TARGET} ${TEST_LIST})
endfunction()

function(end_task)
    if(${TOOL_BUILD})
        add_task_all_tests_target()
    endif()
endfunction()
