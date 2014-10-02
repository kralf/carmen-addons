remake_find_package(carmen CONFIG)
remake_find_package(libnsick CONFIG)
remake_find_package(tulibs CONFIG)

remake_include(
  ../../lib/nsick/*
  ../../lib/glv
  ../../lib/gl
  ${LIBNSICK_INCLUDE_DIRS}
  ${TULIBS_INCLUDE_DIRS}
)

remake_add_executables(nsick_logger.c
  LINK nsick_interface nsick_core nsick_writelog
  ${CARMEN_LIBRARIES} ${LIBNSICK_LIBRARIES}
)
remake_add_executable(carmen-nsick nsick.c PREFIX OFF
  LINK nsick_interface nsick_core nsick_writelog
  ${CARMEN_LIBRARIES} ${LIBNSICK_LIBRARIES} ${M_LIBRARY}
)
remake_add_executables(nsick_log2glv.c
  LINK nsick_readlog glv ${TULIBS_LIBRARIES} ${M_LIBRARY}
)
remake_add_executables(nsick_view.cpp
  LINK nsick_interface gl ${CARMEN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
)
remake_add_scripts(nsick_init)
