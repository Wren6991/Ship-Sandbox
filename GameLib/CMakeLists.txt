
#
# Game library
#

set  (GAME_SOURCES
	AABB.cpp
	AABB.h
	FixedSizeVector.h
	Game.cpp
	Game.h
	GameController.cpp
	GameController.h
	GameEventDispatcher.h
	GameException.h
	GameMath.h
	GameOpenGL.h
	GameParameters.h
	IGameEventHandler.h
	Log.cpp
	Log.h
	Material.cpp
	Material.h
	PointerContainer.h
	ProgressCallback.h
	RenderContext.cpp
	RenderContext.h
	ResourceLoader.cpp
	ResourceLoader.h
	Scheduler.cpp
	Scheduler.h
	TupleKeys.h
	Utils.cpp
	Utils.h	
	Vectors.cpp
	Vectors.h)

set  (PHYSICS_SOURCES
	Cable.cpp
	Cable.h
	Cloud.h
	ElectricalElement.cpp
	ElectricalElement.h
	Generator.cpp
	Generator.h
	Lamp.cpp
	Lamp.h
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

target_include_directories(GameLib PUBLIC ${PICOJSON_INCLUDE_DIRS})
target_include_directories(GameLib PRIVATE ${IL_INCLUDE_DIR})
target_include_directories(GameLib INTERFACE ..)

target_link_libraries (GameLib
	GladLib
	${OPENGL_LIBRARIES}
	${IL_LIBRARIES}
	${ILU_LIBRARIES}
	${ILUT_LIBRARIES}
	${ADDITIONAL_LIBRARIES})
