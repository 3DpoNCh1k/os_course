begin_task()
set_task_sources(stack_swap.S stack_swap.h ../swap/swap.S ../swap/swap.h)
add_task_test(unit_tests tests/unit.cpp)
end_task()