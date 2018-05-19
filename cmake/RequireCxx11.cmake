# If C++14 support is available, we should use that,
# otherwise fall back to C++11
function(require_cxx11_or_higher target)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 14)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
endfunction()
