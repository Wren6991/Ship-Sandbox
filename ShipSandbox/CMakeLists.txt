
#
# ShipSandbox application
#

set  (SHIP_SANDBOX_SOURCES
	AboutDialog.cpp
	AboutDialog.h
	EventTickerPanel.cpp
	EventTickerPanel.h
	LoggingDialog.cpp
	LoggingDialog.h
	MainApp.cpp
	MainFrame.cpp
	MainFrame.h
	SettingsDialog.cpp
	SettingsDialog.h
	SoundController.cpp
	SoundController.h
	SplashScreenDialog.cpp
	SplashScreenDialog.h
	Version.h
	)

source_group(" " FILES ${SHIP_SANDBOX_SOURCES})

set  (RESOURCE_FILES
	Resources/ShipSandbox.rc
	Resources/Ship.ico)

source_group("Resources" FILES ${RESOURCE_FILES})

add_executable (ShipSandbox ${SHIP_SANDBOX_SOURCES} ${RESOURCE_FILES})

target_include_directories(ShipSandbox PRIVATE ${SFML_INCLUDE_DIR})
target_include_directories(ShipSandbox PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_compile_definitions(ShipSandbox PRIVATE "${wxWidgets_DEFINITIONS}")
target_link_libraries (ShipSandbox
	GameLib
	GladLib
	${OPENGL_LIBRARIES}
	${SFML_LIBRARIES}
	${wxWidgets_LIBRARIES}
	${ADDITIONAL_LIBRARIES})


if (MSVC)
	set_target_properties(ShipSandbox PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /NODEFAULTLIB:MSVCRTD")
else (MSVC)
endif (MSVC)


#
# Set VS properties
#

if (MSVC)

	set (VS_STARTUP_PROJECT ShipSandbox)
	
	set_target_properties(
		ShipSandbox
		PROPERTIES
			# Set debugger working directory to binary output directory
			VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)"

			# Set output directory to binary output directory - VS will add the configuration type
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)

endif (MSVC)



#
# Copy files
#

message (STATUS "Copying data files and runtime files...")

file(COPY "${CMAKE_SOURCE_DIR}/Data" "${CMAKE_SOURCE_DIR}/Ships"
	DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
file(COPY "${CMAKE_SOURCE_DIR}/Data" "${CMAKE_SOURCE_DIR}/Ships"
	DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")
file(COPY "${CMAKE_SOURCE_DIR}/Data" "${CMAKE_SOURCE_DIR}/Ships"
	DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo")

if (WIN32)
	file(COPY ${DEVIL_RUNTIME_LIBRARIES}
		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
	file(COPY ${DEVIL_RUNTIME_LIBRARIES}
		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")
	file(COPY ${DEVIL_RUNTIME_LIBRARIES}
		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo")

	file(COPY ${SFML_RUNTIME_DEBUG_LIBRARIES}
		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
	file(COPY ${SFML_RUNTIME_RELEASE_LIBRARIES}
		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")
	file(COPY ${SFML_RUNTIME_RELEASE_LIBRARIES}
		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo")
endif (WIN32)
