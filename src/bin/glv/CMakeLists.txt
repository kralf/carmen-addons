remake_find_package(GLUT)

remake_include(../../lib/glv)

remake_add_executables(LINK glv ${CARMEN_GLOBAL_LIBRARY} ${GLUT_LIBRARIES})
