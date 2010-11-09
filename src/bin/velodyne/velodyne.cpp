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

#include <sstream>
#include <algorithm>

#include <carmen/global.h>
#include <carmen/param_interface.h>
#include <carmen/camera_interface.h>

// #include <libvelodyne/velodyne.h>

#include "velodyne_messages.h"
#include "velodyne_ipc.h"

int quit = 0;
int id = 1;

int dump_enabled = 0;
char* dump_dir = 0;
char dump_path[4096];

void carmen_velodyne_sigint_handler(int q __attribute__((unused))) {
  quit = 1;
}

void carmen_velodyne_dump_handler(char* module, char* variable, char* value) {
  if (!realpath(dump_dir, dump_path)) {
    dump_path[0] = '\0';
    carmen_warn("\nWarning: Dump directory %s is invalid\n", dump_dir);
  }
}

int carmen_velodyne_read_parameters(int argc, char **argv) {
  char module[] = "velodyne";
  int num_params;

  char dump_enabled_var[256], dump_dir_var[256];

  if (argc == 2)
    id = atoi(argv[1]);

  sprintf(dump_enabled_var, "velodyne%d_dump_enabled", id);
  sprintf(dump_dir_var, "velodyne%d_dump_dir", id);

  carmen_param_t params[] = {
    {module, dump_enabled_var, CARMEN_PARAM_ONOFF, &dump_enabled, 1,
      carmen_velodyne_dump_handler},
    {module, dump_dir_var, CARMEN_PARAM_DIR, &dump_dir, 1,
      carmen_velodyne_dump_handler},
  };

  num_params = sizeof(params)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, params, num_params);

  if (!realpath(dump_dir, dump_path)) {
    dump_path[0] = '\0';
    carmen_warn("\nWarning: Dump directory %s is invalid\n", dump_dir);
  }

  return num_params;
}

int main(int argc, char *argv[]) {
  IPC_RETURN_TYPE err;

  carmen_velodyne_ipc_initialize(argc, argv);

  carmen_velodyne_read_parameters(argc, argv);

  signal(SIGINT, carmen_velodyne_sigint_handler);
  while (!quit) {
    if (dump_enabled) {
      // carmen_velodyne_publish_package(id, (char*)filename.c_str(),
      //   filepos, frame.getTimestamp());
    }

    carmen_ipc_sleep(0.0);
  }

  return 0;
}
