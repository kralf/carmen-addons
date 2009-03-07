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

#include <carmen/carmen.h>
#include <carmen/nanotec_interface.h>

#include "writelog.h"

carmen_FILE *outfile = NULL;
double logger_starttime;

static int log_status = 1;
static int log_laserpos = 1;
static int log_laser = 1;

void get_logger_params(int argc, char** argv) {
  int num_params;

  carmen_param_t param_list[] = {
    {"logger", "status", CARMEN_PARAM_ONOFF, &log_status, 0, NULL},
    {"logger", "laserpos", CARMEN_PARAM_ONOFF, &log_laserpos, 0, NULL},
    {"logger", "laser", CARMEN_PARAM_ONOFF, &log_laser, 0, NULL},
  };

  num_params = sizeof(param_list)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, param_list, num_params);
}

void nanotec_status_handler(carmen_nanotec_status_message* status) {
  fprintf(stderr, "S");
  carmen_logwrite_write_nanotec_status(status, outfile,
    carmen_get_time()-logger_starttime);
}

void nanotec_laserpos_handler(carmen_nanotec_laserpos_message* laserpos) {
  fprintf(stderr, "P");
  carmen_logwrite_write_nanotec_laserpos(laserpos, outfile,
    carmen_get_time()-logger_starttime);
}

void laser_laser1_handler(carmen_laser_laser_message* laser) {
  fprintf(stderr, "L");
  carmen_logwrite_write_laser_laser(laser, 1, outfile,
    carmen_get_time()-logger_starttime);
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

  /* initialize connection to IPC network */
  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);	

  /* open logfile, check if file overwrites something */
  if(argc < 2) 
    carmen_die("usage: %s <logfile>\n", argv[0]);
  sprintf(filename, argv[1]);

  outfile = carmen_fopen(filename, "r");
  if (outfile != NULL) {
    fprintf(stderr, "Overwrite %s? ", filename);
    scanf("%c", &key);
    if (toupper(key) != 'Y')
      exit(-1);
    carmen_fclose(outfile);
  }
  outfile = carmen_fopen(filename, "w");
  if(outfile == NULL)
    carmen_die("Error: Could not open file %s for writing.\n", filename);
  carmen_logwrite_write_header(outfile);

  get_logger_params(argc, argv);

  if (log_status) 
    carmen_nanotec_subscribe_status_message(NULL,
    (carmen_handler_t)nanotec_status_handler, CARMEN_SUBSCRIBE_ALL);

  if (log_laserpos) 
    carmen_nanotec_subscribe_laserpos_message(NULL,
    (carmen_handler_t)nanotec_laserpos_handler, CARMEN_SUBSCRIBE_ALL);

  if (log_laser)
    carmen_laser_subscribe_laser1_message(NULL, (carmen_handler_t)
    laser_laser1_handler, CARMEN_SUBSCRIBE_ALL);

  signal(SIGINT, shutdown_module);

  logger_starttime = carmen_get_time();
  carmen_ipc_dispatch();

  return 0;
}
