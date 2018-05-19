# Check if we can use the C++14 std::make_unique
include(CheckMakeUnique)

# If C++14 support is available, we should use that,
# otherwise fall back to C++11
function(require_cxx11_or_higher target)
	if(CXX_HAS_MAKEUNIQUE)
	    set_target_properties(${target} PROPERTIES CXX_STANDARD 14)
	else()
	    set_target_properties(${target} PROPERTIES CXX_STANDARD 11)
	    set_target_properties(${target} PROPERTIES CXX_STANDARD_REQUIRED ON)
	endif()
	set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
endfunction()
