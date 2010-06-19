remake_find_library(ncurses ncurses.h)

remake_add_library(
  commander_core
  LINK ${NCURSES_LIBRARY}
)
remake_add_headers()
