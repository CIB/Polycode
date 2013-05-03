CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
 
PROJECT(UserProject)

SET(POLYCODE_ROOT_DIR ${UserProject_SOURCE_DIR}/..)
SET(POLYCODE_RELEASE_DIR ${POLYCODE_ROOT_DIR}/Release/${CMAKE_SYSTEM_NAME})
SET(CMAKE_INSTALL_PREFIX ${POLYCODE_RELEASE_DIR}/Standalone/)

SET(CMAKE_PREFIX_PATH 
    ${POLYCODE_RELEASE_DIR}/Framework/Core/Dependencies
    ${POLYCODE_RELEASE_DIR}/Framework/Modules/Dependencies
    ${POLYCODE_RELEASE_DIR}/Framework/Tools/Dependencies)

SET(CMAKE_DEBUG_POSTFIX "_d")
SET(CMAKE_MODULE_PATH ${POLYCODE_ROOT_DIR}/CMake)
INCLUDE(${POLYCODE_ROOT_DIR}/CMake/PolycodeIncludes.cmake)
INCLUDE(${POLYCODE_ROOT_DIR}/CMake/FindBullet.cmake)

SET(userproject_SRCS
		src/Main.cpp
)
 
SET(userproject_HDRS
	include/Main.h
)

INSTALL(FILES ${POLYCODE_ROOT_DIR}/LICENSE.txt
		DESTINATION ./)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
		${POLYCODE_RELEASE_DIR}/Framework/Core/include
        ${POLYCODE_RELEASE_DIR}/Framework/Modules/include
        ${POLYCODE_RELEASE_DIR}/Framework/Core/Dependencies/include
        ${POLYCODE_RELEASE_DIR}/Framework/Core/Dependencies/include/AL
        ${UserProject_SOURCE_DIR}/include)

LINK_DIRECTORIES(${POLYCODE_RELEASE_DIR}/Framework/Core/lib
        ${POLYCODE_RELEASE_DIR}/Framework/Modules/lib
        ${POLYCODE_RELEASE_DIR}/Framework/Bindings/Lua/Core/lib)
		
# WIN32 makes it build a GUI application on windows(no effect on other platforms)
ADD_EXECUTABLE(UserProject WIN32 ${userproject_SRCS} ${userproject_HDRS})

 
IF(MSVC OR MINGW)
 
	INSTALL(FILES ${POLYCODE_RELEASE_DIR}/Framework/Core/Dependencies/bin/OpenAL32.dll DESTINATION bin)
	INSTALL(FILES ${POLYCODE_RELEASE_DIR}/Framework/Player/default.pak DESTINATION bin)
	INSTALL(FILES ${POLYCODE_RELEASE_DIR}/Framework/Player/hdr.pak DESTINATION bin)
	INSTALL(FILES ${POLYCODE_RELEASE_DIR}/Framework/Player/api.pak DESTINATION bin)
	INSTALL(FILES ${POLYCODE_RELEASE_DIR}/Framework/Player/UI.pak DESTINATION bin)
	INSTALL(FILES ${POLYCODE_RELEASE_DIR}/Framework/Player/Physics2D.pak DESTINATION bin)
	INSTALL(FILES ${POLYCODE_RELEASE_DIR}/Framework/Player/Physics3D.pak DESTINATION bin)

	TARGET_LINK_LIBRARIES(UserProject 
        PolycodeLua_d
        Polycore_d
        ${ZLIB_LIBRARIES}
        ${LUA_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${PNG_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        ${PHYSFS_LIBRARY}
        ${OGG_LIBRARY}
        ${VORBIS_LIBRARY}
        ${VORBISFILE_LIBRARY}
        opengl32
        glu32
        winmm
        ws2_32
		Polycode2DPhysics
		Polycode3DPhysics
		PolycodeUI
		optimized ${BOX2D_RELEASE_LIBRARY}
		debug ${BOX2D_DEBUG_LIBRARY}
        optimized ${LIBBULLETMULTITHREADED}
        optimized ${LIBBULLETSOFTBODY} 
        optimized ${LIBBULLETDYNAMICS} 
        optimized ${LIBBULLETCOLLISION}
        optimized ${LIBBULLETMATH}            
        debug ${LIBBULLETMULTITHREADED_DEBUG}
        debug ${LIBBULLETSOFTBODY_DEBUG} 
        debug ${LIBBULLETDYNAMICS_DEBUG} 
        debug ${LIBBULLETCOLLISION_DEBUG}
        debug ${LIBBULLETMATH_DEBUG}  
     )
 
ELSEIF(APPLE)
		MESSAGE("Target system not currently supported.")
ELSE(MSVC OR MINGW)
	TARGET_LINK_LIBRARIES(UserProject 
		rt
		pthread
		PolycodeLua_d
		PolycodeUI_d
		Polycore_d
		${LUA_LIBRARY}
		${FREETYPE_LIBRARIES}
		${VORBISFILE_LIBRARY}
		${VORBIS_LIBRARY}
		${OGG_LIBRARY}
		${OPENAL_LIBRARY}
		${PHYSFS_LIBRARY}
		${PNG_LIBRARIES}
		${ZLIB_LIBRARY}
		${OPENGL_LIBRARIES}
		${SDL_LIBRARY}
		dl
		Polycode2DPhysics_d
		Polycode3DPhysics_d
		${BOX2D_RELEASE_LIBRARY}
		
		optimized ${LIBBULLETMULTITHREADED}
		optimized ${LIBBULLETSOFTBODY} 
		optimized ${LIBBULLETDYNAMICS} 
		optimized ${LIBBULLETCOLLISION}
		optimized ${LIBBULLETMATH}
		       
		debug ${LIBBULLETMULTITHREADED_DEBUG}
		debug ${LIBBULLETSOFTBODY_DEBUG} 
		debug ${LIBBULLETDYNAMICS_DEBUG} 
		debug ${LIBBULLETCOLLISION_DEBUG}
		debug ${LIBBULLETMATH_DEBUG}  
	)

ENDIF(MSVC OR MINGW)
	   
LINK_DIRECTORIES(${POLYCODE_RELEASE_DIR}/Framework/Core/lib
								${POLYCODE_RELEASE_DIR}/Framework/Core/Dependencies/lib)
