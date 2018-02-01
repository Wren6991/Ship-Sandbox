#
# GTest
#

set (GTEST_SDK_ROOT "C:/Users/Neurodancer/source/repos/googletest-release-1.8.0/googletest")

set (GTEST_INCLUDE_DIRS 
	"${GTEST_SDK_ROOT}/include")

set (GTEST_LIBRARY_DEBUG
	"${GTEST_SDK_ROOT}/msvc/x64/Debug/gtestd.lib")
set (GTEST_LIBRARY_RELEASE
	"${GTEST_SDK_ROOT}/msvc/x64/Release/gtest.lib")



#
# Unit tests
#

set  (UNIT_TEST_SOURCES
	Main.cpp
	PointerContainerTests.cpp
	ShipTests.cpp
	VectorsTests.cpp)

add_executable (UnitTests ${UNIT_TEST_SOURCES})

target_include_directories (UnitTests PRIVATE
	${GTEST_INCLUDE_DIRS})

target_link_libraries (UnitTests
	GameLib
	${OPENGL_LIBRARIES}
	debug ${GTEST_LIBRARY_DEBUG}
	optimized ${GTEST_LIBRARY_RELEASE}
	${ADDITIONAL_LIBRARIES})

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING /DGTEST_HAS_TR1_TUPLE=0")
endif(MSVC)
