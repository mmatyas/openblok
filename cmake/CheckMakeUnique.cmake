# Check whether the C++14 `std::make_unique` is available

include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

cmake_push_check_state()

set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX14_STANDARD_COMPILE_OPTION}")
set(TESTCODE_MAKEUNIQUE "
	#include <memory>
    int main() {
        std::unique_ptr<int> ptr = std::make_unique<int>(0);
    }
")
check_cxx_source_compiles("${TESTCODE_MAKEUNIQUE}" CXX_HAS_MAKEUNIQUE)

cmake_pop_check_state()
