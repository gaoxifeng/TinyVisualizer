# Install script for directory: C:/TinyVisualizer

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/TinyVisualizer")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/TinyVisualizer/build/temp.win-amd64-cpython-310/Release/extern/glfw/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/TinyVisualizer/lib/Release/glfw3.lib")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "C:/TinyVisualizer/lib/Release" TYPE STATIC_LIBRARY FILES "C:/TinyVisualizer/build/temp.win-amd64-cpython-310/Release/extern/glfw/src/Release/glfw3.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/TinyVisualizer/lib/Release/TinyVisualizer.lib")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
      message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    file(INSTALL DESTINATION "C:/TinyVisualizer/lib/Release" TYPE STATIC_LIBRARY FILES "C:/TinyVisualizer/build/temp.win-amd64-cpython-310/Release/Release/TinyVisualizer.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/TinyVisualizer/include/TinyVisualizer/ArrowShape.h;C:/TinyVisualizer/include/TinyVisualizer/BezierCurveShape.h;C:/TinyVisualizer/include/TinyVisualizer/Box2DShape.h;C:/TinyVisualizer/include/TinyVisualizer/Bullet3DShape.h;C:/TinyVisualizer/include/TinyVisualizer/Camera2D.h;C:/TinyVisualizer/include/TinyVisualizer/Camera3D.h;C:/TinyVisualizer/include/TinyVisualizer/CameraExportPlugin.h;C:/TinyVisualizer/include/TinyVisualizer/CameraManipulator.h;C:/TinyVisualizer/include/TinyVisualizer/CaptureGIFPlugin.h;C:/TinyVisualizer/include/TinyVisualizer/CaptureMPEG2Plugin.h;C:/TinyVisualizer/include/TinyVisualizer/CellShape.h;C:/TinyVisualizer/include/TinyVisualizer/CompositeShape.h;C:/TinyVisualizer/include/TinyVisualizer/DefaultLight.h;C:/TinyVisualizer/include/TinyVisualizer/Drawer.h;C:/TinyVisualizer/include/TinyVisualizer/DrawerUtility.h;C:/TinyVisualizer/include/TinyVisualizer/FBO.h;C:/TinyVisualizer/include/TinyVisualizer/FileDialog.h;C:/TinyVisualizer/include/TinyVisualizer/FirstPersonCameraManipulator.h;C:/TinyVisualizer/include/TinyVisualizer/ImGuiPlugin.h;C:/TinyVisualizer/include/TinyVisualizer/LowDimensionalMeshShape.h;C:/TinyVisualizer/include/TinyVisualizer/MakeMesh.h;C:/TinyVisualizer/include/TinyVisualizer/MakeTexture.h;C:/TinyVisualizer/include/TinyVisualizer/Matrix.h;C:/TinyVisualizer/include/TinyVisualizer/MeshShape.h;C:/TinyVisualizer/include/TinyVisualizer/SceneStructure.h;C:/TinyVisualizer/include/TinyVisualizer/Shader.h;C:/TinyVisualizer/include/TinyVisualizer/BoneTransformVert.h;C:/TinyVisualizer/include/TinyVisualizer/DebugDrawTexCoordFrag.h;C:/TinyVisualizer/include/TinyVisualizer/DefaultFrag.h;C:/TinyVisualizer/include/TinyVisualizer/DefaultLightFrag.h;C:/TinyVisualizer/include/TinyVisualizer/DefaultLightVert.h;C:/TinyVisualizer/include/TinyVisualizer/DefaultVert.h;C:/TinyVisualizer/include/TinyVisualizer/MatVecMultVert.h;C:/TinyVisualizer/include/TinyVisualizer/NormalGeom.h;C:/TinyVisualizer/include/TinyVisualizer/RoundPointFrag.h;C:/TinyVisualizer/include/TinyVisualizer/RoundPointVert.h;C:/TinyVisualizer/include/TinyVisualizer/ShadowFrag.h;C:/TinyVisualizer/include/TinyVisualizer/ShadowLightFrag.h;C:/TinyVisualizer/include/TinyVisualizer/ShadowLightVert.h;C:/TinyVisualizer/include/TinyVisualizer/ShadowLightVertNoNormal.h;C:/TinyVisualizer/include/TinyVisualizer/ShadowVert.h;C:/TinyVisualizer/include/TinyVisualizer/TexCopyFrag.h;C:/TinyVisualizer/include/TinyVisualizer/ThickLineFrag.h;C:/TinyVisualizer/include/TinyVisualizer/ThickLineGeom.h;C:/TinyVisualizer/include/TinyVisualizer/ThickLineVert.h;C:/TinyVisualizer/include/TinyVisualizer/XORFrag.h;C:/TinyVisualizer/include/TinyVisualizer/ShadowAndLight.h;C:/TinyVisualizer/include/TinyVisualizer/SkinnedMeshShape.h;C:/TinyVisualizer/include/TinyVisualizer/TerrainShape.h;C:/TinyVisualizer/include/TinyVisualizer/Texture.h;C:/TinyVisualizer/include/TinyVisualizer/TrackballCameraManipulator.h;C:/TinyVisualizer/include/TinyVisualizer/VBO.h;C:/TinyVisualizer/include/TinyVisualizer/VisibilityScore.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/TinyVisualizer/include/TinyVisualizer" TYPE FILE FILES
    "C:/TinyVisualizer/TinyVisualizer/ArrowShape.h"
    "C:/TinyVisualizer/TinyVisualizer/BezierCurveShape.h"
    "C:/TinyVisualizer/TinyVisualizer/Box2DShape.h"
    "C:/TinyVisualizer/TinyVisualizer/Bullet3DShape.h"
    "C:/TinyVisualizer/TinyVisualizer/Camera2D.h"
    "C:/TinyVisualizer/TinyVisualizer/Camera3D.h"
    "C:/TinyVisualizer/TinyVisualizer/CameraExportPlugin.h"
    "C:/TinyVisualizer/TinyVisualizer/CameraManipulator.h"
    "C:/TinyVisualizer/TinyVisualizer/CaptureGIFPlugin.h"
    "C:/TinyVisualizer/TinyVisualizer/CaptureMPEG2Plugin.h"
    "C:/TinyVisualizer/TinyVisualizer/CellShape.h"
    "C:/TinyVisualizer/TinyVisualizer/CompositeShape.h"
    "C:/TinyVisualizer/TinyVisualizer/DefaultLight.h"
    "C:/TinyVisualizer/TinyVisualizer/Drawer.h"
    "C:/TinyVisualizer/TinyVisualizer/DrawerUtility.h"
    "C:/TinyVisualizer/TinyVisualizer/FBO.h"
    "C:/TinyVisualizer/TinyVisualizer/FileDialog.h"
    "C:/TinyVisualizer/TinyVisualizer/FirstPersonCameraManipulator.h"
    "C:/TinyVisualizer/TinyVisualizer/ImGuiPlugin.h"
    "C:/TinyVisualizer/TinyVisualizer/LowDimensionalMeshShape.h"
    "C:/TinyVisualizer/TinyVisualizer/MakeMesh.h"
    "C:/TinyVisualizer/TinyVisualizer/MakeTexture.h"
    "C:/TinyVisualizer/TinyVisualizer/Matrix.h"
    "C:/TinyVisualizer/TinyVisualizer/MeshShape.h"
    "C:/TinyVisualizer/TinyVisualizer/SceneStructure.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/BoneTransformVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/DebugDrawTexCoordFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/DefaultFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/DefaultLightFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/DefaultLightVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/DefaultVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/MatVecMultVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/NormalGeom.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/RoundPointFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/RoundPointVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ShadowFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ShadowLightFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ShadowLightVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ShadowLightVertNoNormal.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ShadowVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/TexCopyFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ThickLineFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ThickLineGeom.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/ThickLineVert.h"
    "C:/TinyVisualizer/TinyVisualizer/Shader/XORFrag.h"
    "C:/TinyVisualizer/TinyVisualizer/ShadowAndLight.h"
    "C:/TinyVisualizer/TinyVisualizer/SkinnedMeshShape.h"
    "C:/TinyVisualizer/TinyVisualizer/TerrainShape.h"
    "C:/TinyVisualizer/TinyVisualizer/Texture.h"
    "C:/TinyVisualizer/TinyVisualizer/TrackballCameraManipulator.h"
    "C:/TinyVisualizer/TinyVisualizer/VBO.h"
    "C:/TinyVisualizer/TinyVisualizer/VisibilityScore.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/TinyVisualizer/include")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/TinyVisualizer" TYPE DIRECTORY FILES "C:/TinyVisualizer/extern/glad/include" FILES_MATCHING REGEX "/gl\\.h$" REGEX "/khrplatform\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/TinyVisualizer/include/imgui")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/TinyVisualizer/include" TYPE DIRECTORY FILES "C:/TinyVisualizer/extern/imgui" FILES_MATCHING REGEX "/imgui\\.h$" REGEX "/imconfig\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/TinyVisualizer/cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/TinyVisualizer" TYPE DIRECTORY FILES "C:/TinyVisualizer/cmake" FILES_MATCHING REGEX "/findtinyvisualizer\\.cmake$")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/TinyVisualizer/build/temp.win-amd64-cpython-310/Release/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
