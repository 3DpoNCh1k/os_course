begin_task()
set_task_sources(atomic.cpp atomic.h helpers.cpp)
add_task_test(unit_tests tests/unit.cpp)
end_task()
