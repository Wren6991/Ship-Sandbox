#
# Resources library
#

set  (RESOURCES_SOURCES
	)

if (MSVC)
	list(APPEND RESOURCES_SOURCES
		ShipSandbox.rc
		Ship.ico)
endif(MSVC)

source_group(" " FILES ${RESOURCES_SOURCES})

add_library (Resources ${RESOURCES_SOURCES})

target_include_directories (Resources PUBLIC
	${WXWIDGETS_INCLUDE_DIRS})

target_include_directories(Resources INTERFACE 
	..)