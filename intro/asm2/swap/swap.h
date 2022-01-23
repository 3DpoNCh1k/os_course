#pragma once

#include <cstdint>

// int64_t used for escaping some stack smashing error (stack canary protection
// from compiler)
extern "C" void swap(int64_t* a, int64_t* b);
