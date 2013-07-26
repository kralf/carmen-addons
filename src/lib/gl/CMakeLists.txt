remake_add_library(gl LINK ${CARMEN_CORE_LIBRARIES}
  ${GL_LIBRARIES} ${GLU_LIBRARIES} ${GLUT_glut_LIBRARY})
remake_add_headers()
remake_pkg_config_generate(REQUIRES carmen-core gl glu)
