
#
# Game library
#

set  (GAME_SOURCES
	Game.cpp
	Game.h
	GameController.cpp
	GameController.h
	GameException.h
	GameParameters.h
	Log.cpp
	Log.h
	material.cpp
	material.h
	phys.cpp
	phys.h
	PointerContainer.cpp
	PointerContainer.h
	Renderer.cpp
	Renderer.h
	RenderParameters.h
	Scheduler.cpp
	Scheduler.h
	util.cpp
	util.h
	Vectors.cpp
	Vectors.h)

source_group(" " FILES ${GAME_SOURCES})

add_library (GameLib ${GAME_SOURCES})

# TODO: temporary, until we hide picojson into material
target_include_directories(GameLib PUBLIC
	${JSONCPP_INCLUDE_DIRS})
target_include_directories(GameLib PRIVATE
	${DEVIL_INCLUDE_DIRS})

target_include_directories(GameLib INTERFACE 
	..)

target_link_libraries (GameLib
	debug ${JSONCPP_LIBRARY_DEBUG}
	optimized ${JSONCPP_LIBRARY_RELEASE}
	${OPENGL_LIBRARIES}
	debug ${DEVIL_DEVIL_LIBRARY_DEBUG}
	debug ${DEVIL_ILU_LIBRARY_DEBUG}
	optimized ${DEVIL_DEVIL_LIBRARY_RELEASE}
	optimized ${DEVIL_ILU_LIBRARY_RELEASE}
	${ADDITIONAL_LIBRARIES})

