remake_find_library(carmen-param_interface param_interface.h PACKAGE CARMEN)
remake_find_library(carmen-writelog writelog.h PACKAGE CARMEN)
remake_find_library(velodyne-acquisition acquisition/AcquisitionThread.h
  PACKAGE libvelodyne)

remake_include(
  ../../lib/velodyne/*
  ../../lib/gl
  ${VELODYNE_ACQUISITION_HEADERS}
)

remake_add_executables(velodyne.cpp velodyne_logger.c
  LINK velodyne_interface velodyne_core velodyne_writelog
    ${CARMEN_PARAM_INTERFACE_LIBRARY} ${CARMEN_WRITELOG_LIBRARY}
    ${VELODYNE_ACQUISITION_LIBRARY}
)
remake_add_executables(velodyne_view.cpp
  LINK velodyne_interface gl
    ${CARMEN_PARAM_INTERFACE_LIBRARY}
)
