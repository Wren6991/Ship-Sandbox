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


set (WXWIDGET_INCLUDE_DIRS 
	"C:/Users/Neurodancer/source/repos/wxWidgets/include" 
	"C:/Users/Neurodancer/source/repos/wxWidgets/include/msvc")

link_directories("C:/Users/Neurodancer/source/repos/wxWidgets/lib/vc_x64_lib")

IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
	set (WXWIDGET_INCLUDE_DIRS 
		${WXWIDGET_INCLUDE_DIRS}
		"C:/Users/Neurodancer/source/repos/wxWidgets/lib/vc_x64_lib/mswu")
	set (WXWIDGET_WXMSW_LIBRARY
		"C:/Users/Neurodancer/source/repos/wxWidgets/lib/vc_x64_lib/wxmsw31u_core.lib")
	set (WXWIDGET_WXBASE_LIBRARY
		"C:/Users/Neurodancer/source/repos/wxWidgets/lib/vc_x64_lib/wxbase31u.lib")
ELSE()
	set (WXWIDGET_INCLUDE_DIRS 
		${WXWIDGET_INCLUDE_DIRS}
		"C:/Users/Neurodancer/source/repos/wxWidgets/lib/vc_x64_lib/mswud")
	set (WXWIDGET_WXMSW_LIBRARY
		"C:/Users/Neurodancer/source/repos/wxWidgets/lib/vc_x64_lib/wxmsw31ud_core.lib")
	set (WXWIDGET_WXBASE_LIBRARY
		"C:/Users/Neurodancer/source/repos/wxWidgets/lib/vc_x64_lib/wxbase31ud.lib")
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)


set (DEVIL_INCLUDE_DIRS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/include")

find_library(DEVIL_DEVIL_LIBRARY NAMES DevIL
       HINTS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/lib/x64/Release")
find_library(DEVIL_ILU_LIBRARY NAMES ILU
       HINTS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/lib/x64/Release")
find_library(DEVIL_ILUT_LIBRARY NAMES ILUT
       HINTS "C:/Users/Neurodancer/source/SDK/DevIL Windows SDK/lib/x64/Release")


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
	#game.cpp
	#game.h
	#main.cpp
	main_test_wx.cpp
	#material.cpp
	#material.h
	#phys.cpp
	#phys.h
	#render.cpp
	#render.h
	#scheduler.cpp
	#scheduler.h
	#settingsDialog.cpp
	#settingsDialog.h
	#titanicApp.cpp
	#titanicApp.h
	#titanicMain.h
	#util.cpp
	#util.h
	#vec.cpp
	#vec.h
	)

source_group(" " FILES ${SHIP_SANDBOX_SOURCES})

add_executable (ShipSandbox ${SHIP_SANDBOX_SOURCES})

target_include_directories(
	ShipSandbox
	PRIVATE
	${JSONCPP_INCLUDE_DIRS}
	${WXWIDGET_INCLUDE_DIRS}
	${DEVIL_INCLUDE_DIRS})

target_link_libraries (
	ShipSandbox
	${JSONCPP_LIBRARY}
	${OPENGL_LIBRARIES}
	${WXWIDGET_WXMSW_LIBRARY}
	${WXWIDGET_WXBASE_LIBRARY}
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

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE")

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
