include_directories (${pf_SOURCE_DIR}/include)

include_directories(${Glibmm_INCLUDE_DIRS})
include_directories(${GlibmmUtils_INCLUDE_DIRS})

link_directories(${pf_BINARY_DIR}/src)


add_executable(factory-test factory-test)
set_target_properties(factory-test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(factory-test pf 
    ${Glibmm_LIBRARIES} ${GlibmmUtils_LIBRARIES})
