remake_find_library(carmen-param_interface param_interface.h PACKAGE CARMEN)
remake_find_library(carmen-writelog writelog.h PACKAGE CARMEN)
# remake_find_library(velodyne velodyne.h PACKAGE libvelodyne)

remake_include(
  ../../lib/velodyne/*
)

remake_add_executables(
  LINK velodyne_interface velodyne_core velodyne_writelog
  ${CARMEN_PARAM_INTERFACE_LIBRARY} ${CARMEN_WRITELOG_LIBRARY}
  ${VELODYNE_LIBRARY}
)
