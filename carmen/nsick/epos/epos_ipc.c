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

#include "epos_ipc.h"

int carmen_epos_ipc_initialize(int argc, char *argv[]) {
  IPC_RETURN_TYPE err;

  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);

  err = IPC_defineMsg(CARMEN_EPOS_STATUS_MESSAGE_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_EPOS_STATUS_MESSAGE_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_EPOS_STATUS_MESSAGE_NAME);

  err = IPC_defineMsg(CARMEN_EPOS_LASERPOS_MESSAGE_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_EPOS_LASERPOS_MESSAGE_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_EPOS_LASERPOS_MESSAGE_NAME);

  return 0;
}

void carmen_epos_publish_status(double pos, double timestamp) {
  carmen_epos_status_message status;
  IPC_RETURN_TYPE err;

  status.pos = pos;

  status.timestamp = timestamp;
  status.host = carmen_get_host();

  err = IPC_publishData(CARMEN_EPOS_STATUS_MESSAGE_NAME, &status);
  carmen_test_ipc_exit(err, "Could not publish",
    CARMEN_EPOS_STATUS_MESSAGE_NAME);
}

void carmen_epos_publish_laserpos(int laser_id, double timestamp) {
  carmen_epos_laserpos_message laserpos;
  IPC_RETURN_TYPE err;

  laserpos.id = laser_id;

  laserpos.timestamp = timestamp;
  laserpos.host = carmen_get_host();

  err = IPC_publishData(CARMEN_EPOS_LASERPOS_MESSAGE_NAME, &laserpos);
  carmen_test_ipc_exit(err, "Could not publish",
    CARMEN_EPOS_LASERPOS_MESSAGE_NAME);
}
