include(CheckCXXCompilerFlag)

macro(try_adding_compiler_flag flag)
    check_cxx_compiler_flag(${flag} CXX_[${flag}]_SUPPORTED)
    if(CXX_[${flag}]_SUPPORTED)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endif()
endmacro()
