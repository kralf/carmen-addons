remake_find_library(carmen-param_interface param_interface.h PACKAGE CARMEN)
remake_find_library(carmen-camera_interface camera_interface.h PACKAGE CARMEN)
remake_find_library(firecam firecam.h PACKAGE libfirecam)

remake_include(
  ../../lib/firecam/*
)

remake_add_executables(
  LINK firecam_interface firecam_core
  ${CARMEN_PARAM_INTERFACE_LIBRARY} ${CARMEN_CAMERA_INTERFACE_LIBRARY}
  ${FIRECAM_LIBRARY}
)
