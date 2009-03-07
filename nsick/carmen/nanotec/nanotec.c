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

#include "nanotec_messages.h"
#include "nanotec_ipc.h"

#include "../../nanotec/vanilla/nanotec.h"

int stop = 0;

int laser_id = 1;
char* motor_dev;

void sigint_handler(int q __attribute__((unused))) {
  stop = 1;
}

void read_parameters(int argc, char **argv) {
  int num_params;
  carmen_param_t param_list[] = {
    {"nanotec", "motor_dev", CARMEN_PARAM_STRING, &motor_dev, 0, NULL},
    {"nanotec", "laser_id", CARMEN_PARAM_INT, &laser_id, 0, NULL}
  };

  num_params = sizeof(param_list)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, param_list, num_params);
}

int main(int argc, char *argv[]) {
  carmen_nanotec_ipc_initialize(argc, argv);
  read_parameters(argc, argv);

  signal(SIGINT, sigint_handler);

  while (!stop) {
    carmen_nanotec_publish_status();
    carmen_nanotec_publish_laserpos();

    usleep(250000);
  }

  return 0;
}
