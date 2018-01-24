cmake_minimum_required (VERSION 2.6)

project (ShipSandbox)
 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 
# External libraries 
#


#
# JsonCPP
#

set (JSONCPP_SDK_ROOT "C:/Users/Neurodancer/source/repos/jsoncpp")

set (JSONCPP_INCLUDE_DIRS "${JSONCPP_SDK_ROOT}/include")

IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
	find_library(JSONCPP_LIBRARY NAMES lib_json
		HINTS "${JSONCPP_SDK_ROOT}/makefiles/msvc2010/x64/Release")
ELSE()
	find_library(JSONCPP_LIBRARY NAMES lib_json
		HINTS "${JSONCPP_SDK_ROOT}/makefiles/msvc2010/x64/Debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)


#
# WxWidgets
#

set (WXWIDGETS_SDK_ROOT "C:/Users/Neurodancer/source/repos/wxWidgets")

set (WXWIDGETS_INCLUDE_DIRS 
	"${WXWIDGETS_SDK_ROOT}/include" 
	"${WXWIDGETS_SDK_ROOT}/include/msvc")

link_directories("${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib")

IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
	set (WXWIDGETS_INCLUDE_DIRS 
		${WXWIDGETS_INCLUDE_DIRS}
		"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/mswu")
	set (WXWIDGETS_WXMSW_LIBRARY
		"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxmsw31u_core.lib")
	set (WXWIDGETS_WXBASE_LIBRARY
		"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxbase31u.lib")
ELSE()
	set (WXWIDGETS_INCLUDE_DIRS 
		${WXWIDGETS_INCLUDE_DIRS}
		"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/mswud")
	set (WXWIDGETS_WXMSW_LIBRARY
		"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxmsw31ud_core.lib")
	set (WXWIDGETS_WXBASE_LIBRARY
		"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxbase31ud.lib")
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)


#
# DevIL
#

set (DEVIL_SDK_ROOT "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK")

set (DEVIL_INCLUDE_DIRS "${DEVIL_SDK_ROOT}/include")

find_library(DEVIL_DEVIL_LIBRARY NAMES DevIL
       HINTS "${DEVIL_SDK_ROOT}/lib/x64/unicode/Release")
find_library(DEVIL_ILU_LIBRARY NAMES ILU
       HINTS "${DEVIL_SDK_ROOT}/lib/x64/unicode/Release")
find_library(DEVIL_ILUT_LIBRARY NAMES ILUT
       HINTS "${DEVIL_SDK_ROOT}/lib/x64/unicode/Release")


#
# Misc
#

if (MSVC)
	set(ADDITIONAL_LIBRARIES "comctl32;rpcrt4;advapi32") # winmm.lib wsock32.lib
else(MSVC)
	set(ADDITIONAL_LIBRARIES "")
endif(MSVC)


find_package(OpenGL REQUIRED)


#
# ShipSandbox
#

set  (SHIP_SANDBOX_SOURCES
	Game.cpp
	Game.h
	GameController.cpp
	GameController.h
	GameException.h
	GameParameters.h
	Log.cpp
	Log.h
	LoggingDialog.cpp
	LoggingDialog.h
	MainApp.cpp
	MainFrame.cpp
	MainFrame.h
	material.cpp
	material.h
	phys.cpp
	phys.h
	render.cpp
	render.h
	RenderParameters.h
	Scheduler.cpp
	Scheduler.h
	SettingsDialog.cpp
	SettingsDialog.h
	util.cpp
	util.h
	vec.cpp
	vec.h
	Version.h
	)

source_group(" " FILES ${SHIP_SANDBOX_SOURCES})

add_executable (ShipSandbox ${SHIP_SANDBOX_SOURCES})

target_include_directories(
	ShipSandbox
	PRIVATE
	${JSONCPP_INCLUDE_DIRS}
	${WXWIDGETS_INCLUDE_DIRS}
	${DEVIL_INCLUDE_DIRS})

target_link_libraries (
	ShipSandbox
	${JSONCPP_LIBRARY}
	${OPENGL_LIBRARIES}
	${WXWIDGETS_WXMSW_LIBRARY}
	${WXWIDGETS_WXBASE_LIBRARY}
	${DEVIL_DEVIL_LIBRARY}
	${DEVIL_ILU_LIBRARY}
	${DEVIL_ILUT_LIBRARY}
	${ADDITIONAL_LIBRARIES})

if (MSVC)
	set_target_properties(ShipSandbox PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
else (MSVC)
endif(MSVC)


#
# Flags
#

if (MSVC)
	set_target_properties(ShipSandbox PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE /D_CRT_SECURE_NO_WARNINGS")

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
else(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif(MSVC)
 
message ("cxx Flags:" ${CMAKE_CXX_FLAGS})
message ("cxx Flags Release:" ${CMAKE_CXX_FLAGS_RELEASE})
message ("cxx Flags Debug:" ${CMAKE_CXX_FLAGS_DEBUG})
