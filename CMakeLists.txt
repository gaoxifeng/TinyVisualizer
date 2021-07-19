PROJECT(TinyVisualizer)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#COMPILE SETTINGS
SET(CMAKE_BUILD_TYPE Release CACHE STRING "Compiler optimization level")
SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
SET(CUDA_SEPARABLE_COMPILATION ON)
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  MESSAGE(STATUS "Build type: Debug")
  IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/openmp /bigobj -Wall -std=c++11 ${CMAKE_CXX_FLAGS}")
  ELSE(MSVC)
    SET(CMAKE_CXX_FLAGS "-include cstring -fopenmp -O0 -Wall -fPIC -std=c++11 -Wno-unused-function ${CMAKE_CXX_FLAGS}")
  ENDIF(MSVC)
ELSE()
  MESSAGE(STATUS "Build type: Release")
  IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/openmp /bigobj -Wall -std=c++11 ${CMAKE_CXX_FLAGS}")
  ELSE(MSVC)
    SET(CMAKE_CXX_FLAGS "-include cstring -fopenmp -O3 -Wall -fPIC -std=c++11 -Wno-unused-function ${CMAKE_CXX_FLAGS}")
  ENDIF(MSVC)
ENDIF()
SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

# Compile with compiler warnings
IF(MSVC)
  IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  ELSE()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  ENDIF()
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  IF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-anonymous-struct -Wno-c99-extensions -Wno-nested-anon-types")
  ENDIF()
ENDIF()

#LIBRARY
INCLUDE("${PROJECT_SOURCE_DIR}/cmake/LoadLibs.cmake")

#THIRD PARTY HEADERS
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/extern ${PROJECT_SOURCE_DIR}/extern/imgui ${PROJECT_SOURCE_DIR}/extern/glad/include)
MACRO(SEARCH_SOURCE NAME DIRPRJ)
  FILE(GLOB_RECURSE header${NAME} ${PROJECT_SOURCE_DIR}/${DIRPRJ}/*.h ${PROJECT_SOURCE_DIR}/${DIRPRJ}/*.hpp ${PROJECT_SOURCE_DIR}/${DIRPRJ}/*.hh)
  FILE(GLOB_RECURSE source${NAME} ${PROJECT_SOURCE_DIR}/${DIRPRJ}/*.cpp ${PROJECT_SOURCE_DIR}/${DIRPRJ}/*.cc ${PROJECT_SOURCE_DIR}/${DIRPRJ}/*.c)
ENDMACRO(SEARCH_SOURCE)
SEARCH_SOURCE(TinyVisualizer TinyVisualizer)
SEARCH_SOURCE(GLAD extern/glad)

SET(JOMPEG ${PROJECT_SOURCE_DIR}/extern/jo_mpeg.h)

FILE(GLOB IMGUI 
${PROJECT_SOURCE_DIR}/extern/imgui/*.h
${PROJECT_SOURCE_DIR}/extern/imgui/*.cpp
${PROJECT_SOURCE_DIR}/TinyVisualizer/imgui/*.h
${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.h
${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp
${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.h
${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp
)

#BUILD LIBRARY
IF(MSVC)
  ADD_LIBRARY(TinyVisualizer STATIC ${sourceTinyVisualizer} ${headerTinyVisualizer} ${sourceGLAD} ${headerGLAD} ${JOMPEG} ${IMGUI})
ELSE(MSVC)
  ADD_LIBRARY(TinyVisualizer SHARED ${sourceTinyVisualizer} ${headerTinyVisualizer} ${sourceGLAD} ${headerGLAD} ${JOMPEG} ${IMGUI})
ENDIF(MSVC)
SET_TARGET_PROPERTIES(TinyVisualizer PROPERTIES PUBLIC_HEADER "${headerTinyVisualizer}" PRIVATE_HEADER "${headerGLAD}")
TARGET_LINK_LIBRARIES(TinyVisualizer ${ALL_LIBRARIES})

IF(MSVC)
  SET(INSTALL_REL_DIRECTORY "C:/TinyVisualizer")
  INSTALL(TARGETS TinyVisualizer 
  ARCHIVE DESTINATION ${INSTALL_REL_DIRECTORY}/lib
  LIBRARY DESTINATION ${INSTALL_REL_DIRECTORY}/lib
  PUBLIC_HEADER DESTINATION ${INSTALL_REL_DIRECTORY}/include/TinyVisualizer
  PRIVATE_HEADER DESTINATION ${INSTALL_REL_DIRECTORY}/include/glad
  )
ELSE(MSVC)
  INSTALL(TARGETS TinyVisualizer LIBRARY DESTINATION "/usr/local/lib" PUBLIC_HEADER DESTINATION "/usr/local/include/TinyVisualizer" PRIVATE_HEADER DESTINATION "/usr/local/include/glad")
ENDIF(MSVC)

#EXE
MACRO(ADD_EXE NAME)
  ADD_EXECUTABLE(${NAME} Main/${NAME}.cpp)
  IF(MSVC)
    TARGET_LINK_LIBRARIES(${NAME} TinyVisualizer ${ALL_LIBRARIES})
  ELSE(MSVC)
    TARGET_LINK_LIBRARIES(${NAME} TinyVisualizer gomp ${ALL_LIBRARIES})
  ENDIF(MSVC)
ENDMACRO(ADD_EXE)
#DEBUG
ADD_EXE(mainBox2D)
ADD_EXE(mainBullet3D)
ADD_EXE(mainVisibility)
ADD_EXE(mainSphericalBox)
ADD_EXE(mainTerrain)
ADD_EXE(mainChecker)
ADD_EXE(mainOctree)
ADD_EXE(mainBezier)
ADD_EXE(mainLight)
ADD_EXE(mainArrow)
