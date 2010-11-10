remake_find_package(gl CONFIG)
remake_find_package(glu CONFIG)
remake_find_package(GLUT)
remake_find_library(carmen-param_interface param_interface.h PACKAGE CARMEN)
remake_find_library(carmen-writelog writelog.h PACKAGE CARMEN)
remake_find_library(velodyne-acquisition AcquisitionThread.h
  PACKAGE libvelodyne)

remake_include(
  ../../lib/velodyne/*
)

remake_add_executables(
  LINK velodyne_interface velodyne_core velodyne_writelog
  ${GL_LIBRARIES} ${GLU_LIBRARIES} ${GLUT_LIBRARIES}
  ${CARMEN_PARAM_INTERFACE_LIBRARY} ${CARMEN_WRITELOG_LIBRARY}
  ${VELODYNE_ACQUISITION_LIBRARY}
)
