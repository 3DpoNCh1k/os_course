begin_task()
set_task_sources(elf_entry.cpp elf_entry.h)
add_task_test(unit_tests tests/unit.cpp)
add_task_test(test_example tests/test_example.cpp)
get_task_target(TEST_NAME unit_tests)
add_custom_command(
        TARGET "${TEST_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
end_task()
