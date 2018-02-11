enable_testing()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING /DGTEST_HAS_TR1_TUPLE=0")
endif(MSVC)

# google advises to compile gtest from source with the project
add_subdirectory("C:/Users/Neurodancer/source/repos/googletest-release-1.8.0" gtest)

set (UNIT_TEST_SOURCES
	FixedSizeVectorTests.cpp
	PointerContainerTests.cpp
	ShipTests.cpp
	VectorsTests.cpp)

add_executable (UnitTests ${UNIT_TEST_SOURCES})
add_test (UnitTests UnitTests)

target_link_libraries (UnitTests
	GameLib
	${OPENGL_LIBRARIES}
	gtest_main
	${ADDITIONAL_LIBRARIES})
