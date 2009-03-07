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

#include "nanotec_ipc.h"

void carmen_nanotec_publish_status(void) {
  carmen_nanotec_status_message status;
  IPC_RETURN_TYPE err;

  status.timestamp = carmen_get_time();
  status.host = carmen_get_host();

  err = IPC_publishData(CARMEN_NANOTEC_STATUS_NAME, &status);
  carmen_test_ipc_exit(err, "Could not publish", CARMEN_NANOTEC_STATUS_NAME);
}

void carmen_nanotec_publish_laserpos(void) {
  carmen_nanotec_laserpos_message laserpos;
  IPC_RETURN_TYPE err;

  laserpos.timestamp = carmen_get_time();
  laserpos.host = carmen_get_host();

  err = IPC_publishData(CARMEN_NANOTEC_LASERPOS_NAME, &laserpos);
  carmen_test_ipc_exit(err, "Could not publish", CARMEN_NANOTEC_LASERPOS_NAME);
}

int carmen_nanotec_ipc_initialize(int argc, char *argv[]) {
  IPC_RETURN_TYPE err;

  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);

  err = IPC_defineMsg(CARMEN_NANOTEC_STATUS_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_NANOTEC_STATUS_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_NANOTEC_STATUS_NAME);

  err = IPC_defineMsg(CARMEN_NANOTEC_LASERPOS_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_NANOTEC_LASERPOS_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_NANOTEC_LASERPOS_NAME);

  return 0;
}
