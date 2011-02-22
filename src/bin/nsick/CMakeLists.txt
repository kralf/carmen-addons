remake_find_library(carmen-param_interface param_interface.h PACKAGE CARMEN)
remake_find_library(carmen-laser_interface laser_interface.h PACKAGE CARMEN)
remake_find_library(carmen-writelog writelog.h PACKAGE CARMEN)
remake_find_library(nsick nsick.h PACKAGE libnsick)
remake_find_library(tulibs-transform transform.h PACKAGE tulibs)

remake_include(
  ../../lib/nsick/*
  ../../lib/glv
  ../../lib/gl
)

remake_add_executables(nsick.c nsick_logger.c
  LINK nsick_interface nsick_core nsick_writelog
  ${CARMEN_PARAM_INTERFACE_LIBRARY} ${CARMEN_LASER_INTERFACE_LIBRARY}
  ${CARMEN_WRITELOG_LIBRARY} ${NSICK_LIBRARY}
)
remake_add_executables(nsick_log2glv.c
  LINK nsick_readlog glv ${TULIBS_TRANSFORM_LIBRARY}
)
remake_add_executables(nsick_view.cpp
  LINK nsick_interface gl ${CARMEN_PARAM_INTERFACE_LIBRARY}
)
remake_add_scripts(nsick_init)
