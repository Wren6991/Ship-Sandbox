
#
# ShipSandbox application
#

set  (SHIP_SANDBOX_SOURCES
	LoggingDialog.cpp
	LoggingDialog.h
	MainApp.cpp
	MainFrame.cpp
	MainFrame.h
	SettingsDialog.cpp
	SettingsDialog.h
	Version.h
	)

source_group(" " FILES ${SHIP_SANDBOX_SOURCES})

add_executable (ShipSandbox ${SHIP_SANDBOX_SOURCES})

target_include_directories (ShipSandbox	PRIVATE
	${JSONCPP_INCLUDE_DIRS}
	${WXWIDGETS_INCLUDE_DIRS})

target_link_libraries (ShipSandbox
	GameLib
	Resources
	${OPENGL_LIBRARIES}
	debug ${WXWIDGETS_WXMSW_LIBRARY_DEBUG}
	optimized ${WXWIDGETS_WXMSW_LIBRARY_RELEASE}
	debug ${WXWIDGETS_WXBASE_LIBRARY_DEBUG}
	optimized ${WXWIDGETS_WXBASE_LIBRARY_RELEASE}
	${ADDITIONAL_LIBRARIES})

if (MSVC)
	set_target_properties(ShipSandbox PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
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
			VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$(ConfigurationName)

			# Set output directory to binary output directory - VS will add the configuration type
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

endif (MSVC)



#
# Copy files
#

message (STATUS "Copying data files and DevIL runtime files...")

foreach(build_type ${CMAKE_CONFIGURATION_TYPES})

	file(COPY "${CMAKE_SOURCE_DIR}/Data"
		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${build_type}")

	if (WIN32)
		file(COPY ${DEVIL_RUNTIME_LIBRARIES}
			DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${build_type}")
	endif (WIN32)

endforeach(build_type)
