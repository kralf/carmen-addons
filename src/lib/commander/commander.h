#include <ncurses.h>

#ifdef __cplusplus
extern "C" {
#endif

  int commander_init(int argc, char **argv);
  void commander_exit();
  bool commander_quit();
  void commander_die(char *msg, int exit_code);
  void commander_print_message(char const *msg);

  void commander_make_menu(char *config_file_name);

  void commander_arrow_down();
  void commander_arrow_up();
  void commander_arrow_right();
  void commander_arrow_left();
  
  void commander_run_process();
  void commander_stop_process();
  void commander_show_process();
  void commander_update();
  void commander_watch_processes();

#ifdef __cplusplus
}
#endif
