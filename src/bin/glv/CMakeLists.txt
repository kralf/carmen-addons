remake_find_package(GLUT)
remake_find_package(OpenGL CONFIG gl)

remake_include(../../lib/glv)

remake_add_executables(LINK glv ${CARMEN_GLOBAL_LIBRARY}
  ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
