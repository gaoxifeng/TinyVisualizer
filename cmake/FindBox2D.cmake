# - Find Box2D
# Find the native Box2D includes and libraries
#
#  BOX2D_INCLUDE_DIR - where to find TmxParser/Config.hpp, etc.
#  BOX2D_LIBRARIES   - List of libraries when using libTmxParser.
#  BOX2D_FOUND       - True if libTmxParser found.

# First, try config-mode lookup (works with vcpkg which ships box2dConfig.cmake).
find_package(box2d CONFIG QUIET)
if(box2d_FOUND)
    if(TARGET box2d::box2d)
        get_target_property(_box2d_inc box2d::box2d INTERFACE_INCLUDE_DIRECTORIES)
        if(_box2d_inc)
            set(BOX2D_INCLUDE_DIR "${_box2d_inc}" CACHE PATH "Box2D include directory" FORCE)
        endif()
        set(BOX2D_LIBRARY box2d::box2d)
        set(BOX2D_LIBRARY_RELEASE box2d::box2d)
        set(BOX2D_LIBRARY_DEBUG box2d::box2d)
        set(BOX2D_FOUND TRUE)
        if(NOT BOX2D_FIND_QUIETLY)
            message(STATUS "Box2D found (config mode): ${BOX2D_INCLUDE_DIR}")
        endif()
        return()
    endif()
endif()

# Fallback: manual search for header/library paths.

if(BOX2D_INCLUDE_DIR)
  # Already in cache, be silent
  set(BOX2D_FIND_QUIETLY TRUE)
endif(BOX2D_INCLUDE_DIR)

find_path(BOX2D_INCLUDE_DIR box2d/box2d.h
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw          # Fink
  /opt/local   # DarwinPorts
  /opt/csw     # Blastwave
  /opt
  ${BOX2DDIR}
  $ENV{BOX2DDIR})

find_library(BOX2D_LIBRARY_DEBUG
  NAMES Box2D-d box2d
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/lib/x86_64-linux-gnu
  /usr/local
  /usr
  /sw          # Fink
  /opt/local   # DarwinPorts
  /opt/csw     # Blastwave
  /opt
  ${BOX2DDIR}
  ${BOX2DDIR}/build/src/Debug
  $ENV{BOX2DDIR})

find_library(BOX2D_LIBRARY_RELEASE
  NAMES Box2D box2d
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/lib/x86_64-linux-gnu
  /usr/local
  /usr
  /sw          # Fink
  /opt/local   # DarwinPorts
  /opt/csw     # Blastwave
  /opt
  ${BOX2DDIR}
  ${BOX2DDIR}/build/src/Release
  ${BOX2DDIR}/build/src/RelWithDebInfo
  $ENV{BOX2DDIR})

if(BOX2D_LIBRARY_RELEASE AND BOX2D_LIBRARY_DEBUG)
  set(BOX2D_LIBRARY debug ${BOX2D_LIBRARY_DEBUG} optimized ${BOX2D_LIBRARY_RELEASE})
endif()

if(BOX2D_LIBRARY_RELEASE AND NOT BOX2D_LIBRARY_DEBUG)
  set(BOX2D_LIBRARY_DEBUG ${BOX2D_LIBRARY_RELEASE})
  set(BOX2D_LIBRARY ${BOX2D_LIBRARY_RELEASE})
endif()

if(BOX2D_LIBRARY_DEBUG AND NOT BOX2D_LIBRARY_RELEASE)
  set(BOX2D_LIBRARY_RELEASE ${BOX2D_LIBRARY_DEBUG})
  set(BOX2D_LIBRARY ${BOX2D_LIBRARY_DEBUG})
endif()

if(NOT BOX2D_INCLUDE_DIR OR NOT BOX2D_LIBRARY)
  if(BOX2D_FIND_REQUIRED)
    message(FATAL_ERROR "Box2D not found.")
  elseif(NOT BOX2D_FIND_QUIETLY)
    message("Box2D not found.")
  endif()
else()
  set(BOX2D_FOUND true)
  if (NOT BOX2D_FIND_QUIETLY)
    message(STATUS "Box2D found: ${BOX2D_INCLUDE_DIR}")
  endif()
endif()
