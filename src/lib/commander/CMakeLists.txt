remake_find_package(ncurses CONFIG)

remake_add_library(
  commander_core
  LINK ${NCURSES_LIBRARIES}
)
remake_add_headers()
remake_pkg_config_generate(REQUIRES ncurses)
