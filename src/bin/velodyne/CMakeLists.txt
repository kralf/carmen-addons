remake_find_package(carmen CONFIG)
remake_find_package(libvelodyne CONFIG)

remake_include(
  ../../lib/velodyne/*
  ../../lib/gl
  ${LIBVELODYNE_INCLUDE_DIRS}
)

remake_add_executables(velodyne.cpp
  LINK velodyne_interface velodyne_core ${CARMEN_LIBRARIES}
    ${LIBVELODYNE_LIBRARIES}
)
remake_add_executables(velodyne_logger.c
  LINK velodyne_interface velodyne_writelog ${CARMEN_LIBRARIES}
)
remake_add_executables(velodyne_view.cpp
  LINK velodyne_interface gl ${CARMEN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
)
set_target_properties(velodyne PROPERTIES COMPILE_FLAGS -std=c++0x)
