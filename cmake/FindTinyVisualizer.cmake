IF(NOT TinyVisualizer_FOUND)

FIND_PATH(TinyVisualizer_INCLUDE_DIR TinyVisualizer/Drawer.h
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  /usr/include
  /usr/local/include
  ${TinyVisualizer_ROOT}/include
  ${TinyVisualizer_ROOT}
  )

FIND_PATH(ImGui_INCLUDE_DIR imgui/imgui.h
  ${PROJECT_SOURCE_DIR}/extern
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  /usr/include
  /usr/local/include
  ${TinyVisualizer_ROOT}/extern
  ${TinyVisualizer_ROOT}/include
  ${TinyVisualizer_ROOT}
  )

FIND_PATH(glad_INCLUDE_DIR glad/gl.h
  ${PROJECT_SOURCE_DIR}/extern/glad/include
  ${PROJECT_SOURCE_DIR}/extern/glad
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  /usr/include
  /usr/local/include
  ${TinyVisualizer_ROOT}/extern/glad/include
  ${TinyVisualizer_ROOT}/extern/glad
  ${TinyVisualizer_ROOT}/include
  ${TinyVisualizer_ROOT}
  )

FIND_LIBRARY(TinyVisualizer_LIBRARIES NAME TinyVisualizer PATHS 
  ${PROJECT_SOURCE_DIR}/Release
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}
  /usr/lib
  /usr/local/lib
  ${TinyVisualizer_ROOT}/Release
  ${TinyVisualizer_ROOT}/lib
  ${TinyVisualizer_ROOT}
  ${TinyVisualizer_ROOT}-build/Release
  ${TinyVisualizer_ROOT}-build/lib
  ${TinyVisualizer_ROOT}-build
  )

IF(TinyVisualizer_INCLUDE_DIR AND TinyVisualizer_LIBRARIES)
  set(TinyVisualizer_FOUND TRUE)
  set(TinyVisualizer_INCLUDE_DIRS ${TinyVisualizer_INCLUDE_DIR} ${ImGui_INCLUDE_DIR} ${glad_INCLUDE_DIR})
ENDIF(TinyVisualizer_INCLUDE_DIR AND TinyVisualizer_LIBRARIES)

MARK_AS_ADVANCED(TinyVisualizer_INCLUDE_DIR TinyVisualizer_INCLUDE_DIRS TinyVisualizer_LIBRARIES)

ENDIF(NOT TinyVisualizer_FOUND)
