
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
	_Material.cpp
	_Material.h
	phys.cpp
	phys.h
	PointerContainer.cpp
	PointerContainer.h
	RenderParameters.h
	RenderUtils.cpp
	RenderUtils.h
	Scheduler.cpp
	Scheduler.h
	Utils.cpp
	Utils.h
	Vectors.cpp
	Vectors.h)

source_group(" " FILES ${GAME_SOURCES})

add_library (GameLib ${GAME_SOURCES})

target_include_directories(GameLib PUBLIC
	${PICOJSON_INCLUDE_DIRS})

target_include_directories(GameLib PRIVATE
	${DEVIL_INCLUDE_DIRS})

target_include_directories(GameLib INTERFACE 
	..)

target_link_libraries (GameLib
	${OPENGL_LIBRARIES}
	debug ${DEVIL_DEVIL_LIBRARY_DEBUG}
	debug ${DEVIL_ILU_LIBRARY_DEBUG}
	optimized ${DEVIL_DEVIL_LIBRARY_RELEASE}
	optimized ${DEVIL_ILU_LIBRARY_RELEASE}
	${ADDITIONAL_LIBRARIES})

