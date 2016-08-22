set(TEST_SRC
	test_GraphicsContext.cpp

	main.cpp
)

add_executable(openblok_test ${TEST_SRC})

target_link_libraries(openblok_test UnitTest++)
target_link_libraries(openblok_test module_system)
target_link_libraries(openblok_test module_game)
