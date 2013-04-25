include_directories (${pf_SOURCE_DIR}/include)

include_directories(${Glib_INCLUDE_DIRS})
include_directories(${GIO_INCLUDE_DIRS})
include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${Glibmm_INCLUDE_DIRS})
include_directories(${GlibmmUtils_INCLUDE_DIRS})
#include_directories (${pf_SOURCE_DIR}/tests)

link_directories(${pf_BINARY_DIR}/src)

add_executable(factory-test factory-test)
set_target_properties(factory-test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(factory-test pf 
    ${Glibmm_LIBRARIES} ${GlibmmUtils_LIBRARIES} ${GStreamer_LIBRARIES} ${GSTREAMER_LIBRARIES} 
    ${Glib_LIBRARIES}  ${GIO_LIBRARIES})

add_executable(options-test options-test)
set_target_properties(options-test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(options-test pf 
    ${Glibmm_LIBRARIES} ${GlibmmUtils_LIBRARIES} ${GStreamer_LIBRARIES} ${GSTREAMER_LIBRARIES} 
    ${Glib_LIBRARIES}  ${GIO_LIBRARIES})
