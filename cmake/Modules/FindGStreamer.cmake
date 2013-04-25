# - Try to find GStreamer
# Once done this will define
#
#  GSTREAMER_FOUND - system has GStreamer
#  GSTREAMER_INCLUDE_DIRS - the GStreamer include directory
#  GSTREAMER_LIBRARIES - the libraries needed to use GStreamer
#  GSTREAMER_DEFINITIONS - Compiler switches required for using GStreamer

#  (c)2006, Tim Beaulen <tbscope@gmail.com>

# TODO: Other versions --> GSTREAMER_X_Y_FOUND (Example: GSTREAMER_0_8_FOUND and GSTREAMER_1.0_FOUND etc)

IF (GSTREAMER_INCLUDE_DIRS AND GSTREAMER_LIBRARIES AND GSTREAMER_BASE_LIBRARY AND GSTREAMER_INTERFACE_LIBRARY)
   # in cache already
   SET(GStreamer_FIND_QUIETLY TRUE)
ELSE (GSTREAMER_INCLUDE_DIRS AND GSTREAMER_LIBRARIES AND GSTREAMER_BASE_LIBRARY AND GSTREAMER_INTERFACE_LIBRARY)
   SET(GStreamer_FIND_QUIETLY FALSE)
ENDIF (GSTREAMER_INCLUDE_DIRS AND GSTREAMER_LIBRARIES AND GSTREAMER_BASE_LIBRARY AND GSTREAMER_INTERFACE_LIBRARY)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   INCLUDE(UsePkgConfig)
   PKGCONFIG(gstreamer-1.0 _GStreamerIncDir _GStreamerLinkDir _GStreamerLinkFlags _GStreamerCflags)
   MESSAGE(STATUS "DEBUG: GStreamer include directory = ${_GStreamerIncDir}")
   MESSAGE(STATUS "DEBUG: GStreamer link directory = ${_GStreamerLinkDir}")
   MESSAGE(STATUS "DEBUG: GStreamer link flags = ${_GStreamerLinkFlags}")
   MESSAGE(STATUS "DEBUG: GStreamer CFlags = ${_GStreamerCflags}") 
   SET(GSTREAMER_DEFINITIONS ${_GStreamerCflags})
ENDIF (NOT WIN32)

FIND_PATH(GSTREAMER_INCLUDE_DIRS gst/gst.h
   PATHS
   ${_GStreamerIncDir}
   #PATH_SUFFIXES gst
   )

FIND_LIBRARY(GSTREAMER_LIBRARIES NAMES gstreamer-1.0
   PATHS
   ${_GStreamerLinkDir}
   )

FIND_LIBRARY(GSTREAMER_BASE_LIBRARY NAMES gstbase-1.0
   PATHS
   ${_GStreamerLinkDir}
   )
FIND_LIBRARY(GSTREAMER_BASE_LIBRARY NAMES gstpbutils-1.0
   PATHS
   ${_GStreamerLinkDir}
   )

IF (GSTREAMER_INCLUDE_DIRS)
   #MESSAGE(STATUS "DEBUG: Found GStreamer include dir: ${GSTREAMER_INCLUDE_DIRS}")
ELSE (GSTREAMER_INCLUDE_DIRS)
   MESSAGE(STATUS "GStreamer: WARNING: include dir not found")
ENDIF (GSTREAMER_INCLUDE_DIRS)

IF (GSTREAMER_LIBRARIES)
   #MESSAGE(STATUS "DEBUG: Found GStreamer library: ${GSTREAMER_LIBRARIES}")
ELSE (GSTREAMER_LIBRARIES)
   MESSAGE(STATUS "GStreamer: WARNING: library not found")
ENDIF (GSTREAMER_LIBRARIES)

MARK_AS_ADVANCED(GSTREAMER_INCLUDE_DIRS GSTREAMER_LIBRARIES GSTREAMER_BASE_LIBRARY GSTREAMER_INTERFACE_LIBRARY)
