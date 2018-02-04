
#
# Game library
#

set  (GAME_SOURCES
	AABB.cpp
	AABB.h
	Game.cpp
	Game.h
	GameController.cpp
	GameController.h
	GameException.cpp
	GameException.h
	GameOpenGL.h
	GameParameters.h
	Log.cpp
	Log.h
	Material.cpp
	Material.h
	PointerContainer.cpp
	PointerContainer.h
	RenderParameters.h
	RenderUtils.h
	Scheduler.cpp
	Scheduler.h
	Utils.cpp
	Utils.h	
	Vectors.cpp
	Vectors.h)

set  (PHYSICS_SOURCES
	Physics.h
	Point.cpp
	Point.h
	Ship.cpp
	Ship.h
	ShipElement.h
	Spring.cpp
	Spring.h
	Triangle.cpp
	Triangle.h
	World.cpp
	World.h)

source_group(" " FILES ${GAME_SOURCES})

source_group("Physics" FILES ${PHYSICS_SOURCES})

add_library (GameLib ${GAME_SOURCES} ${PHYSICS_SOURCES})

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

