begin_task()
set_task_sources(swap.S swap.h)
add_task_test(unit_tests tests/unit.cpp)
end_task()