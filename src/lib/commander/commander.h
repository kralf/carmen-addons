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

/** @addtogroup commander **/
// @{

/**
 * \file commander.h
 * \brief Commander functions.
 *
 * This library provides the commander functions.
 **/

#ifndef CARMEN_COMMANDER_H
#define CARMEN_COMMANDER_H

#include <ncurses.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char *name;
  char *call;
  int state;
  int pid;
  int x_forw;
  int is_genom;
  char *log_file_name;
  char *infile_name;
  char *host_name;
} carmen_commander_process;

int carmen_commander_init(int argc, char **argv);
void carmen_commander_exit();
int carmen_commander_quit();
void carmen_commander_die(char *msg, int exit_code);
void carmen_commander_print_message(char const *msg);

void carmen_commander_make_menu(char *config_file_name);

void carmen_commander_arrow_down();
void carmen_commander_arrow_up();
void carmen_commander_arrow_right();
void carmen_commander_arrow_left();

void carmen_commander_run_process();
void carmen_commander_stop_process();
void carmen_commander_show_process();
void carmen_commander_update();
void carmen_commander_watch_processes();

#ifdef __cplusplus
}
#endif

#endif
// @}
