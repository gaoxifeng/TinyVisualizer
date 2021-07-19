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

FIND_LIBRARY(TinyVisualizer_LIBRARY NAMES TinyVisualizer PATHS 
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}
  $ENV{TinyVisualizer_ROOT}/lib
  $ENV{TinyVisualizer_ROOT}
  /usr/lib
  /usr/local/lib
  C:/TinyVisualizer/lib
  )

#GLUT/GLFW
FIND_PACKAGE(OpenGL QUIET)
FIND_PACKAGE(GLFW QUIET)

IF(TinyVisualizer_INCLUDE_DIR AND TinyVisualizer_LIBRARY AND OPENGL_FOUND AND GLFW_FOUND)
  set(TinyVisualizer_FOUND TRUE)
  set(TinyVisualizer_INCLUDE_DIRS ${TinyVisualizer_INCLUDE_DIR})
  set(TinyVisualizer_LIBRARIES ${TinyVisualizer_LIBRARY} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
ENDIF(TinyVisualizer_INCLUDE_DIR AND TinyVisualizer_LIBRARY AND OPENGL_FOUND AND GLFW_FOUND)

MARK_AS_ADVANCED(TinyVisualizer_INCLUDE_DIR TinyVisualizer_INCLUDE_DIRS TinyVisualizer_LIBRARY TinyVisualizer_LIBRARIES)

ENDIF(NOT TinyVisualizer_FOUND)
