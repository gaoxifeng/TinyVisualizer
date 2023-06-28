IF(NOT TinyVisualizer_FOUND)

FIND_PATH(TinyVisualizer_INCLUDE_DIR TinyVisualizer/Drawer.h
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  $ENV{TinyVisualizer_ROOT}/include
  $ENV{TinyVisualizer_ROOT}
  /usr/include
  /usr/local/include
  C:/TinyVisualizer/include
  )

FIND_PATH(ImGui_INCLUDE_DIR imgui/imgui.h
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  $ENV{TinyVisualizer_ROOT}/include
  $ENV{TinyVisualizer_ROOT}
  /usr/include
  /usr/local/include
  C:/TinyVisualizer/include
  )

FIND_LIBRARY(TinyVisualizer_LIBRARY NAMES TinyVisualizer PATHS 
  ${PROJECT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}
  ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}
  $ENV{TinyVisualizer_ROOT}/lib/${CMAKE_BUILD_TYPE}
  $ENV{TinyVisualizer_ROOT}/${CMAKE_BUILD_TYPE}
  /usr/lib/${CMAKE_BUILD_TYPE}
  /usr/local/lib/${CMAKE_BUILD_TYPE}
  C:/TinyVisualizer/lib/${CMAKE_BUILD_TYPE}
  )
  

FIND_LIBRARY(glfw_LIBRARY NAMES glfw3 PATHS 
  ${PROJECT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}
  ${PROJECT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}
  $ENV{TinyVisualizer_ROOT}/lib/${CMAKE_BUILD_TYPE}
  $ENV{TinyVisualizer_ROOT}/${CMAKE_BUILD_TYPE}
  /usr/lib/${CMAKE_BUILD_TYPE}
  /usr/local/lib/${CMAKE_BUILD_TYPE}
  C:/TinyVisualizer/lib/${CMAKE_BUILD_TYPE}
  )

IF(TinyVisualizer_INCLUDE_DIR AND TinyVisualizer_LIBRARY AND glfw_LIBRARY)
  set(TinyVisualizer_FOUND TRUE)
  set(TinyVisualizer_LIBRARIES ${TinyVisualizer_LIBRARY} ${glfw_LIBRARY})
  set(TinyVisualizer_INCLUDE_DIRS ${TinyVisualizer_INCLUDE_DIR} ${ImGui_INCLUDE_DIR})
ENDIF(TinyVisualizer_INCLUDE_DIR AND TinyVisualizer_LIBRARIES AND glfw_LIBRARY)

MARK_AS_ADVANCED(TinyVisualizer_INCLUDE_DIR TinyVisualizer_INCLUDE_DIRS TinyVisualizer_LIBRARIES)

ENDIF(NOT TinyVisualizer_FOUND)
