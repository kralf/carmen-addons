remake_find_library(carmen-param_interface param_interface.h PACKAGE CARMEN)
remake_find_library(carmen-map_interface map_interface.h PACKAGE CARMEN)
remake_find_library(carmen-map_io map_io.h PACKAGE CARMEN)

# param_interface map_interface map_io

remake_add_executables(
  LINK ${CARMEN_PARAM_INTERFACE_LIBRARY} ${CARMEN_MAP_INTERFACE_LIBRARY}
    ${CARMEN_MAP_IO_LIBRARY}
)
