 /*********************************************************
 *
 * This source code is part of the Carnegie Mellon Robot
 * Navigation Toolkit (CARMEN)
 *
 * CARMEN Copyright (c) 2002 Michael Montemerlo, Nicholas
 * Roy, Sebastian Thrun, Dirk Haehnel, Cyrill Stachniss,
 * and Jared Glover
 *
 * CARMEN is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option)
 * any later version.
 *
 * CARMEN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General
 * Public License along with CARMEN; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307 USA
 *
 ********************************************************/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "commander.h"

#define CONFIGURATION_FILE "commander.conf"

int main(int argc, char **argv)
{
  int ret_val, ch = '\0', end = 0;
  char *config_file_name;

  if(argc == 1){
    config_file_name = strdup(
      CONFIGURATION_DESTINATION "/" CONFIGURATION_FILE);
  }
  else if(argc == 2){
    config_file_name = strdup(argv[1]);
  }
  else{
    fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
    fprintf(stderr, "Default value for <config_file> is "
      CONFIGURATION_DESTINATION "/" CONFIGURATION_FILE "\n");
    exit(1);
  }

  ret_val = carmen_commander_init(argc, argv);
  if(ret_val < 0){
    fprintf(stderr, "Error while  initializing commander.");
    carmen_commander_exit();
  }

  carmen_commander_make_menu(config_file_name);

  while(!end){

    ch = -1;
    ch = getch();

    switch(ch){

    case KEY_UP:
      carmen_commander_arrow_up();
      break;

    case KEY_DOWN:
      carmen_commander_arrow_down();
      break;

    case 'r': case 'R':
      carmen_commander_run_process();
      break;

    case 'k': case 'K':
      carmen_commander_stop_process();
      break;

    case 'd': case 'D':
      carmen_commander_show_process();
      break;

    case 'u': case 'U':
      carmen_commander_update();
      break;

    case 'q': case 'Q':
      if (carmen_commander_quit())
	end = 1;
      break;

    case -1:
      break;

    default:
      carmen_commander_print_message("Unknown command!");
      break;
    }

    carmen_commander_watch_processes();
  }

  endwin();
  return 0;
}
