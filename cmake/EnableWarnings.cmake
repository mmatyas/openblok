include(TryAddingCompilerFlag)

function(enable_warnings target)
	if(MSVC)
	    try_adding_compiler_flag(${target} /W3)
	else()
	    try_adding_compiler_flag(${target} -Wall -Wextra -pedantic)
	endif()
endfunction()
