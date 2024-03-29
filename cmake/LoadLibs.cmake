#LIBRARIES
ADD_DEFINITIONS(-DSTB_SUPPORT)

#EIGEN3
FIND_PACKAGE(Eigen3 QUIET REQUIRED)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})

#GLFW
FIND_PACKAGE(GLFW QUIET REQUIRED)
INCLUDE_DIRECTORIES(${GLFW_INCLUDE_DIR})
LIST(APPEND ALL_LIBRARIES ${GLFW_LIBRARY})

#IMGUI
ADD_DEFINITIONS(-DIMGUI_SUPPORT)
ADD_DEFINITIONS(-DIMGUI_IMPL_OPENGL_LOADER_GLAD2)
FILE(GLOB IMGUI
  ${PROJECT_SOURCE_DIR}/extern/imgui/*.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/*.cpp
  ${PROJECT_SOURCE_DIR}/TinyVisualizer/imgui/*.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp
  ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp)

#Box2D
OPTION(USE_BOX2D "Add internal support for Box2D" OFF)
MARK_AS_ADVANCED(USE_BOX2D)
IF(USE_BOX2D)
  FIND_PACKAGE(Box2D QUIET)
  IF(BOX2D_FOUND)
    INCLUDE_DIRECTORIES(${BOX2D_INCLUDE_DIR})
    MESSAGE(STATUS "Found Box2D @ ${BOX2D_INCLUDE_DIR}")
    LIST(APPEND ALL_LIBRARIES ${BOX2D_LIBRARY})
    ADD_DEFINITIONS(-DBOX2D_SUPPORT)
  ELSE(BOX2D_FOUND)
    MESSAGE(WARNING "Cannot find Box2D!")
  ENDIF(BOX2D_FOUND)
ELSE(USE_BOX2D)
  MESSAGE(STATUS "Compile without Box2D!")
ENDIF(USE_BOX2D)

#Bullet
OPTION(USE_BULLET "Add internal support for bullet" OFF)
MARK_AS_ADVANCED(USE_BULLET)
IF(USE_BULLET)
  FIND_PACKAGE(Bullet QUIET)
  IF(Bullet_FOUND)
    INCLUDE_DIRECTORIES(${BULLET_INCLUDE_DIRS})
    MESSAGE(STATUS "Found Bullet @ ${BULLET_INCLUDE_DIRS}")
    LIST(APPEND ALL_LIBRARIES ${BULLET_LIBRARIES})
    ADD_DEFINITIONS(-DBULLET_SUPPORT)
  ELSE(Bullet_FOUND)
    MESSAGE(WARNING "Cannot find Bullet!")
  ENDIF(Bullet_FOUND)
ELSE(USE_BULLET)
  MESSAGE(STATUS "Compile without Bullet!")
ENDIF(USE_BULLET)

#Assimp
FIND_PACKAGE(Assimp QUIET)
IF(Assimp_FOUND)
  ADD_DEFINITIONS(-DASSIMP_SUPPORT)
  INCLUDE_DIRECTORIES(${ASSIMP_INCLUDE_DIR})
  MESSAGE(STATUS "Found Assimp @ ${ASSIMP_INCLUDE_DIR}")
  IF(MSVC)
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      LIST(APPEND ALL_LIBRARIES ${ASSIMP_LIBRARY_DEBUG})
    ELSE()
      LIST(APPEND ALL_LIBRARIES ${ASSIMP_LIBRARY_RELEASE})
    ENDIF()
  ELSE(MSVC)
    LIST(APPEND ALL_LIBRARIES ${ASSIMP_LIBRARY_RELEASE})
  ENDIF(MSVC)
ELSE(Assimp_FOUND)
  MESSAGE(STATUS "Compile without Assimp!")
ENDIF(Assimp_FOUND)

#OpenGL
FIND_PACKAGE(OpenGL QUIET REQUIRED COMPONENTS OpenGL)
LIST(APPEND ALL_LIBRARIES ${OPENGL_gl_LIBRARY})
