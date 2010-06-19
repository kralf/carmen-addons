#include "commander.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define CONFIG_FILE_NAME "elrob_commander.conf"

int main(int argc, char **argv)
{
  int ret_val, ch = '\0', end = 0;
  char *config_file_name;

  if(argc == 1){
    config_file_name = strdup(CONFIG_FILE_NAME);
  }
  else if(argc == 2){
    config_file_name = strdup(argv[1]);
  }
  else{
    fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
    fprintf(stderr, "Default value for <config_file> is " CONFIG_FILE_NAME "\n");
    exit(1);
  }

  ret_val = commander_init(argc, argv);
  if(ret_val < 0){
    fprintf(stderr, "Error while  initializing commander.");
    commander_exit();
  }

  commander_make_menu(config_file_name);

  while(!end){

    ch = -1;
    ch = getch();

    switch(ch){

    case KEY_UP:
      commander_arrow_up();
      break;

    case KEY_DOWN:
      commander_arrow_down();
      break;

    case 'r': case 'R':
      commander_run_process();
      break;

    case 'k': case 'K':
      commander_stop_process();
      break;

    case 'd': case 'D':
      commander_show_process();
      break;

    case 'u': case 'U':
      commander_update();
      break;

    case 'q': case 'Q':
      if (commander_quit())
	end = 1;
      break;

    case -1:
      break;

    default:
      commander_print_message("Unknown command!");
      break;
    }

    commander_watch_processes();
  }

  endwin();
  return 0;
}
