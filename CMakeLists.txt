cmake_minimum_required (VERSION 2.6)
 
project (ShipSandbox)
 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 
# External libraries 
#

set (GLFW_INCLUDE_DIRS "C:/Users/Neurodancer/source/SDK/glfw-3.2.1.bin.WIN64/include")
find_library(GLFW_LIBRARY NAMES glfw3
	HINTS "C:/Users/Neurodancer/source/SDK/glfw-3.2.1.bin.WIN64/lib-vc2015")

set (JSONCPP_INCLUDE_DIRS "C:/Users/Neurodancer/source/repos/jsoncpp/include")
IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
	find_library(JSONCPP_LIBRARY NAMES lib_json
		HINTS "C:/Users/Neurodancer/source/repos/jsoncpp/makefiles/msvc2010/x64/Release")
ELSE()
	find_library(JSONCPP_LIBRARY NAMES lib_json
		HINTS "C:/Users/Neurodancer/source/repos/jsoncpp/makefiles/msvc2010/x64/Debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)

set (DEVIL_INCLUDE_DIRS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/include")
find_library(DEVIL_LIBRARY NAMES DevIL
	HINTS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/lib/x64/Release")


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
	${GLFW_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIRS}
	${DEVIL_INCLUDE_DIRS})

target_link_libraries (
	ShipSandbox
	${GLFW_LIBRARY}
	${JSONCPP_LIBRARY}
	${DEVIL_LIBRARY})

#
# Flags
#

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4") 
message ("cxx Flags:" ${CMAKE_CXX_FLAGS})
 