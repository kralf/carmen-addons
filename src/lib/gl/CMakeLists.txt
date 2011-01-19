remake_find_package(gl CONFIG)
remake_find_package(glu CONFIG)
remake_find_package(GLUT)

remake_add_library(gl LINK ${CARMEN_GLOBAL_LIBRARY}
  ${GL_LIBRARIES} ${GLU_LIBRARIES} ${GLUT_LIBRARIES})
remake_add_headers()
