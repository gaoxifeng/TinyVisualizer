CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(TinyVisualizer)

#COMPILE SETTINGS
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
SET(CMAKE_BUILD_TYPE Release CACHE STRING "Compiler optimization level")
SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  MESSAGE(STATUS "Build type: Debug")
  IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/openmp /bigobj /Wall /std:c++17 ${CMAKE_CXX_FLAGS}")
  ELSE(MSVC)
    ADD_COMPILE_DEFINITIONS(_GLIBCXX_DEBUG=1)
    ADD_COMPILE_DEFINITIONS(_GLIBCXX_USE_CXX11_ABI=1)
    SET(CMAKE_CXX_FLAGS "-fopenmp -O0 -Wall -fPIC -std=c++17 -Wno-unused-function -Wno-unused-local-typedefs ${CMAKE_CXX_FLAGS}")
  ENDIF(MSVC)
ELSE()
  MESSAGE(STATUS "Build type: Release")
  IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/openmp /bigobj /Wall /std:c++17 ${CMAKE_CXX_FLAGS}")
  ELSE(MSVC)
    ADD_COMPILE_DEFINITIONS(_GLIBCXX_USE_CXX11_ABI=1)
    SET(CMAKE_CXX_FLAGS "-fopenmp -O3 -Wall -fPIC -std=c++17 -Wno-unused-function -Wno-unused-local-typedefs ${CMAKE_CXX_FLAGS}")
  ENDIF(MSVC)
ENDIF()
SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

#RTTI Enabled
OPTION(USE_RTTI "Whether to enable native RTTI" ON)
IF(USE_RTTI)
  REMOVE_DEFINITIONS(-DCUSTOM_RTTI_SUPPORT)
ELSE(USE_RTTI)
  ADD_DEFINITIONS(-DCUSTOM_RTTI_SUPPORT)
  IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/GR- ${CMAKE_CXX_FLAGS}")
  ELSE(MSVC)
    SET(CMAKE_CXX_FLAGS "-fno-rtti ${CMAKE_CXX_FLAGS}")
  ENDIF(MSVC)
ENDIF(USE_RTTI)

#C++11 ABI
OPTION(USE_CXX11_ABI "Whether to use C++11's ABI" ON)
IF(NOT ${USE_CXX11_ABI})
  MESSAGE(STATUS "Disabling C++11's ABI")
  SET(CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 ${CMAKE_CXX_FLAGS}")
ELSE()
  MESSAGE(STATUS "Enabling C++11's ABI")
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
SEARCH_SOURCE(Python Python)

#BUILD LIBRARY
SET(CMAKE_INSTALL_INCLUDEDIR "include")
SET(CMAKE_INSTALL_LIBDIR "lib")
ADD_LIBRARY(TinyVisualizer SHARED ${sourceTinyVisualizer} ${headerTinyVisualizer} ${sourceGLAD} ${headerGLAD} ${IMGUI})
SET_TARGET_PROPERTIES(TinyVisualizer PROPERTIES PUBLIC_HEADER "${headerTinyVisualizer}")
TARGET_LINK_LIBRARIES(TinyVisualizer ${ALL_LIBRARIES})

#PYTHON LIBRARY
IF(Python2_FOUND OR Python3_FOUND)
  ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/extern/pybind11)
  PYBIND11_ADD_MODULE(pyTinyVisualizer SHARED ${sourcePython} ${headerPython} ${sourceTinyVisualizer} ${headerTinyVisualizer} ${sourceGLAD} ${headerGLAD} ${IMGUI})
  TARGET_LINK_LIBRARIES(pyTinyVisualizer PRIVATE ${ALL_LIBRARIES})
  INSTALL(TARGETS pyTinyVisualizer
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
ENDIF()

#INSTALL LIBRARY
INSTALL(TARGETS TinyVisualizer
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_BUILD_TYPE}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_BUILD_TYPE}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/TinyVisualizer)
#INSTALL CMAKE
#INSTALL(DIRECTORY cmake
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/TinyVisualizer/cmake
#  FILES_MATCHING PATTERN FindTinyVisualizer.cmake)
#INSTALL HEADER
INSTALL(DIRECTORY extern/glad/include 
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN gl.h PATTERN khrplatform.h)
INSTALL(DIRECTORY extern/imgui
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/TinyVisualizer
  FILES_MATCHING PATTERN imgui.h PATTERN imconfig.h)

#EXE
MACRO(ADD_EXE NAME)
  ADD_EXECUTABLE(${NAME} Main/${NAME}.cpp)
  TARGET_LINK_LIBRARIES(${NAME} TinyVisualizer ${ALL_LIBRARIES})
ENDMACRO(ADD_EXE)
#DEBUG
ADD_EXE(mainBox2D)
ADD_EXE(mainBullet3D)
ADD_EXE(mainVisibility)
ADD_EXE(mainSphericalBoxAndPoints)
ADD_EXE(mainSelection2D)
ADD_EXE(mainSelection3D)
ADD_EXE(mainMultiview)
ADD_EXE(mainTerrain)
ADD_EXE(mainChecker)
ADD_EXE(mainOctree)
ADD_EXE(mainBezier)
ADD_EXE(mainLight)
ADD_EXE(mainArrow)
ADD_EXE(mainCell)
ADD_EXE(mainCellCmd)
ADD_EXE(mainImGui)
ADD_EXE(mainMesh)
