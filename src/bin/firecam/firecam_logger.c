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

#include "firecam_interface.h"
#include "firecam_writelog.h"

carmen_FILE *outfile = NULL;
double logger_starttime;

static int log_params = 1;
static int log_frame = 1;

void get_logger_params(int argc, char** argv) {
  int num_params;

  carmen_param_t param_list[] = {
    {"firecam_logger", "params", CARMEN_PARAM_ONOFF, &log_params, 0, NULL},
    {"firecam_logger", "frame", CARMEN_PARAM_ONOFF, &log_frame, 0, NULL},
  };

  num_params = sizeof(param_list)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, param_list, num_params);
}

void firecam_frame_handler(carmen_firecam_frame_message* frame) {
  fprintf(stderr, "F");
  firecam_writelog_write_firecam_frame(frame, outfile,
    carmen_get_time()-logger_starttime);
}

void firecam_log_params() {
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
  firecam_writelog_write_header(outfile);

  get_logger_params(argc, argv);

  if (log_params)
    firecam_log_params();

  if (log_frame)
    carmen_firecam_subscribe_frame_message(NULL,
    (carmen_handler_t)firecam_frame_handler, CARMEN_SUBSCRIBE_ALL);

  signal(SIGINT, shutdown_module);

  logger_starttime = carmen_get_time();
  carmen_ipc_dispatch();

  return 0;
}
