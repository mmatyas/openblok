include(TryAddingCompilerFlag)

function(enable_warnings target)
	if(MSVC)
	    try_adding_compiler_flag(${target} /W3) # warnings
	else()
	    try_adding_compiler_flag(${target} -Wall)
	    try_adding_compiler_flag(${target} -Wextra)
	    try_adding_compiler_flag(${target} -pedantic)
	endif()
endfunction()
