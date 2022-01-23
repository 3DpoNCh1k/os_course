begin_task()
set_task_sources(fahrenheit.S fahrenheit.h ../swap/swap.S ../swap/swap.h)
add_task_test(unit_tests tests/unit.cpp)
end_task()