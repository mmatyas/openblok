include(CheckCXXCompilerFlag)

function(try_adding_compiler_flag target flag)
    check_cxx_compiler_flag(${flag} CXX_[${flag}]_SUPPORTED)
    if(CXX_[${flag}]_SUPPORTED)
        target_compile_options(${target} PRIVATE ${flag})
    endif()
endfunction()
