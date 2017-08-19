# Check whether the C++14 `std::make_unique` is available

include(CheckCXXSourceCompiles)


set(TESTCODE_MAKEUNIQUE "#include <memory>
    int main() {
        std::unique_ptr<int> ptr = std::make_unique<int>(0);
    }
")

check_cxx_source_compiles("${TESTCODE_MAKEUNIQUE}" HAS_MAKEUNIQUE)
