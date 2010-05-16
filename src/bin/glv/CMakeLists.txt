remake_find_package(gl CONFIG)
remake_find_package(glu CONFIG)
remake_find_package(GLUT)

remake_include(../../lib/glv)

remake_add_executables(LINK glv ${CARMEN_GLOBAL_LIBRARY}
  ${GL_LIBRARIES} ${GLU_LIBRARIES} ${GLUT_LIBRARIES})
