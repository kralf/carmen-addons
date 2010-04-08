remake_find_library(carmen-param_interface param_interface.h PACKAGE CARMEN)
remake_find_library(carmen-laser_interface laser_interface.h PACKAGE CARMEN)
remake_find_library(carmen-writelog writelog.h PACKAGE CARMEN)
remake_find_library(epos epos.h PACKAGE libepos)
remake_find_library(tulibs-thread thread.h PACKAGE tulibs)

remake_include(
  ../../lib/glv
  ../../lib/nsick/*
)

remake_add_executables(
  LINK nsick_interface nsick_core nsick_readlog nsick_writelog glv
  ${CARMEN_PARAM_INTERFACE_LIBRARY} ${CARMEN_LASER_INTERFACE_LIBRARY}
  ${CARMEN_WRITELOG_LIBRARY} ${EPOS_LIBRARY} ${TULIBS_THREAD_LIBRARY}
)
remake_add_scripts(nsick_init)
