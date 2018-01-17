cmake_minimum_required (VERSION 2.6)

project (ShipSandbox)
 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 
# External libraries 
#

set (JSONCPP_INCLUDE_DIRS "C:/Users/Neurodancer/source/repos/jsoncpp/include")
IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
	find_library(JSONCPP_LIBRARY NAMES lib_json
		HINTS "C:/Users/Neurodancer/source/repos/jsoncpp/makefiles/msvc2010/x64/Release")
ELSE()
	find_library(JSONCPP_LIBRARY NAMES lib_json
		HINTS "C:/Users/Neurodancer/source/repos/jsoncpp/makefiles/msvc2010/x64/Debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)


set (GLFW_INCLUDE_DIRS "C:/Users/Neurodancer/source/SDK/glfw-3.2.1.bin.WIN64/include")
IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
	find_library(GLFW_LIBRARY NAMES glfw3
		HINTS "C:/Users/Neurodancer/source/build/glfw/src/Release")
ELSE()
	find_library(GLFW_LIBRARY NAMES glfw3
		HINTS "C:/Users/Neurodancer/source/build/glfw/src/Debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)


set (DEVIL_INCLUDE_DIRS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/include")
find_library(DEVIL_DEVIL_LIBRARY NAMES DevIL
       HINTS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/lib/x64/Release")
find_library(DEVIL_ILU_LIBRARY NAMES ILU
       HINTS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/lib/x64/Release")
find_library(DEVIL_ILUT_LIBRARY NAMES ILUT
       HINTS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/lib/x64/Release")


find_package(OpenGL REQUIRED)

#
# ShipSandbox
#

set  (SHIP_SANDBOX_SOURCES
	game.cpp
	game.h
	main.cpp
	material.cpp
	material.h
	phys.cpp
	phys.h
	render.cpp
	render.h
	scheduler.cpp
	scheduler.h
	settingsDialog.cpp
	settingsDialog.h
	titanicApp.cpp
	titanicApp.h
	titanicMain.h
	util.cpp
	util.h
	vec.cpp
	vec.h)

source_group(" " FILES ${SHIP_SANDBOX_SOURCES})

add_executable (ShipSandbox ${SHIP_SANDBOX_SOURCES})

target_include_directories(
	ShipSandbox
	PRIVATE
	${JSONCPP_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}
	${DEVIL_INCLUDE_DIRS})

target_link_libraries (
	ShipSandbox
	${JSONCPP_LIBRARY}
	${OPENGL_LIBRARIES}
	${GLFW_LIBRARY}
	${DEVIL_DEVIL_LIBRARY}
	${DEVIL_ILU_LIBRARY}
	${DEVIL_ILUT_LIBRARY})

#
# Flags
#

if (MSVC)
	foreach(flag_var
		CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		endif(${flag_var} MATCHES "/MD")
		if(${flag_var} MATCHES "/W3")
			string(REGEX REPLACE "/W3" "" ${flag_var} "${${flag_var}}")
		endif(${flag_var} MATCHES "/W3")
		set(${flag_var} "${${flag_var}} /W4")
	endforeach(flag_var)

	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()
 
message ("cxx Flags:" ${CMAKE_CXX_FLAGS})
message ("cxx Flags Release:" ${CMAKE_CXX_FLAGS_RELEASE})
message ("cxx Flags Debug:" ${CMAKE_CXX_FLAGS_DEBUG})
