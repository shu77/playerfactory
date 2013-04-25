include_directories (${pf_SOURCE_DIR}/include)
include_directories(${Glib_INCLUDE_DIRS})
include_directories(${GIO_INCLUDE_DIRS})
include_directories(${Glibmm_INCLUDE_DIRS})
include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${GlibmmUtils_INCLUDE_DIRS})

set(OBJS playerfactory abstractplayer pipeline genericplayer genericpipeline
         options)

add_library(pf SHARED ${OBJS})
target_link_libraries(pf ${Glibmm_LIBRARIES} ${GlibmmUtils_LIBRARIES} ${Glib_LIBRARIES} ${GSTREAMER_LIBRARIES} 
${GIO_LIBRARIES})
