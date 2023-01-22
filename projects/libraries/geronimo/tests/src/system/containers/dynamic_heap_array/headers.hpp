
#pragma once

#include "geronimo/system/containers/dynamic_heap_array.hpp"
#include "geronimo/system/containers/generic_array_container.hpp"
#include "geronimo/system/containers/static_array.hpp"

#include "geronimo/system/TraceLogger.hpp"

#include "system/containers/generic_array_container_commons/common.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

template <std::size_t N>
using shorthand_dynamic_heap_array =
  gero::dynamic_heap_array<common::TestStructure,
                            std::allocator<common::TestStructure>, N>;
