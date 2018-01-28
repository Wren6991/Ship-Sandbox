
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
	render.cpp
	render.h
	RenderParameters.h
	Scheduler.cpp
	Scheduler.h
	util.cpp
	util.h
	Vectors.cpp
	Vectors.h)

source_group(" " FILES ${GAME_SOURCES})

add_library (GameLib ${GAME_SOURCES})

target_include_directories(GameLib PRIVATE
	${JSONCPP_INCLUDE_DIRS})

target_include_directories(GameLib PUBLIC
	${DEVIL_INCLUDE_DIRS})

target_include_directories(GameLib INTERFACE 
	..)

target_link_libraries (GameLib
	debug ${JSONCPP_LIBRARY_DEBUG}
	optimized ${JSONCPP_LIBRARY_RELEASE}
	${OPENGL_LIBRARIES}
	${DEVIL_DEVIL_LIBRARY}
	${DEVIL_ILU_LIBRARY}
	${DEVIL_ILUT_LIBRARY}
	${ADDITIONAL_LIBRARIES})

