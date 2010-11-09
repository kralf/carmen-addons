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

#include "velodyne_ipc.h"

int carmen_velodyne_ipc_initialize(int argc, char *argv[]) {
  IPC_RETURN_TYPE err;

  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);

  err = IPC_defineMsg(CARMEN_VELODYNE_PACKAGE_MESSAGE_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_VELODYNE_PACKAGE_MESSAGE_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_VELODYNE_PACKAGE_MESSAGE_NAME);

  return 0;
}

void carmen_velodyne_publish_frame(int laser_id, char* filename, long filepos,
    double timestamp) {
  carmen_velodyne_package_message package;
  IPC_RETURN_TYPE err;

  package.laser_id = laser_id;
  package.filename = filename;
  package.filepos = filepos;

  package.timestamp = timestamp;
  package.host = carmen_get_host();

  err = IPC_publishData(CARMEN_VELODYNE_PACKAGE_MESSAGE_NAME, &package);
  carmen_test_ipc_exit(err, "Could not publish",
    CARMEN_VELODYNE_PACKAGE_MESSAGE_NAME);
}
