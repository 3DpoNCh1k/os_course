begin_task()
set_task_sources(wait_die.c wait_die.h helpers.cpp wdlock.h)
add_task_test(unit_tests tests/unit.cpp)
end_task()
