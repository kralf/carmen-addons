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

#include <carmen/global.h>
#include <carmen/param_interface.h>
#include <carmen/laser_interface.h>
#include <carmen/writelog.h>

#include "nsick_interface.h"

#include "nsick_writelog.h"

carmen_FILE *outfile = NULL;
double logger_starttime;

static int log_params = 1;
static int log_status = 1;
static int log_laserpos = 1;
static int log_laser = 1;

void get_logger_params(int argc, char** argv) {
  int num_params;

  carmen_param_t param_list[] = {
    {"nsick_logger", "params", CARMEN_PARAM_ONOFF, &log_params, 0, NULL},
    {"nsick_logger", "status", CARMEN_PARAM_ONOFF, &log_status, 0, NULL},
    {"nsick_logger", "laserpos", CARMEN_PARAM_ONOFF, &log_laserpos, 0, NULL},
    {"nsick_logger", "laser", CARMEN_PARAM_ONOFF, &log_laser, 0, NULL},
  };

  num_params = sizeof(param_list)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, param_list, num_params);
}

void nsick_status_handler(carmen_nsick_status_message* status) {
  fprintf(stderr, "S");
  nsick_writelog_write_nsick_status(status, outfile,
    carmen_get_time()-logger_starttime);
}

void nsick_laserpos_handler(carmen_nsick_laserpos_message* laserpos) {
  fprintf(stderr, "P");
  nsick_writelog_write_nsick_laserpos(laserpos, outfile,
    carmen_get_time()-logger_starttime);
}

void laser_laser1_handler(carmen_laser_laser_message* laser) {
  fprintf(stderr, "L");
  nsick_writelog_write_laser_laser(laser, 1, outfile,
    carmen_get_time()-logger_starttime);
}

void nsick_log_params() {
  char **variables, **values, **modules;
  int list_length, index, num_modules, module_index;
  char *robot_name, *hostname;

  robot_name = carmen_param_get_robot();
  carmen_param_get_modules(&modules, &num_modules);
  carmen_logwrite_write_robot_name(robot_name, outfile);
  free(robot_name);

  carmen_param_get_paramserver_host(&hostname);

  for (module_index = 0; module_index < num_modules; module_index++) {
    if(carmen_param_get_all(modules[module_index], &variables, &values, NULL,
        &list_length) < 0) {
      IPC_perror("Error retrieving all variables of module");
      exit(-1);
    }
    for (index = 0; index < list_length; index++) {
      carmen_logwrite_write_param(modules[module_index], variables[index],
        values[index], carmen_get_time(), hostname, outfile, carmen_get_time());
      free(variables[index]);
      free(values[index]);
    }
    free(variables);
    free(values);
    free(modules[module_index]);
  }
  free(hostname);
  free(modules);
}

void shutdown_module(int sig) {
  if(sig == SIGINT) {
    carmen_fclose(outfile);
    carmen_ipc_disconnect();
    fprintf(stderr, "\nDisconnecting.\n");
    exit(0);
  }
}

int main(int argc, char **argv) {
  char filename[1024];
  char key;
  int result;

  /* initialize connection to IPC network */
  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);

  /* open logfile, check if file overwrites something */
  if(argc < 2)
    carmen_die("usage: %s <logfile>\n", argv[0]);
  sprintf(filename, "%s", argv[1]);

  outfile = carmen_fopen(filename, "r");
  if (outfile != NULL) {
    fprintf(stderr, "Overwrite %s? ", filename);
    result = scanf("%c", &key);
    if (toupper(key) != 'Y')
      exit(-1);
    carmen_fclose(outfile);
  }
  outfile = carmen_fopen(filename, "w");
  if(outfile == NULL)
    carmen_die("Error: Could not open file %s for writing.\n", filename);
  nsick_writelog_write_header(outfile);

  get_logger_params(argc, argv);

  if (log_params)
    nsick_log_params();

  if (log_status)
    carmen_nsick_subscribe_status_message(NULL,
    (carmen_handler_t)nsick_status_handler, CARMEN_SUBSCRIBE_ALL);

  if (log_laserpos)
    carmen_nsick_subscribe_laserpos_message(NULL,
    (carmen_handler_t)nsick_laserpos_handler, CARMEN_SUBSCRIBE_ALL);

  if (log_laser)
    carmen_laser_subscribe_laser1_message(NULL, (carmen_handler_t)
    laser_laser1_handler, CARMEN_SUBSCRIBE_ALL);

  signal(SIGINT, shutdown_module);

  logger_starttime = carmen_get_time();
  carmen_ipc_dispatch();

  return 0;
}
