remake_find_package(carmen CONFIG)
remake_find_package(libfirecam CONFIG)

remake_include(
  ../../lib/firecam/*
)

remake_add_executables(firecam_logger.c
  LINK firecam_interface firecam_core firecam_writelog
  ${CARMEN_LIBRARIES} ${LIBFIRECAM_LIBRARIES}
)
remake_add_executable(carmen-firecam firecam.cpp PREFIX OFF
  LINK firecam_interface firecam_core firecam_writelog
  ${CARMEN_LIBRARIES} ${LIBFIRECAM_LIBRARIES}
)
