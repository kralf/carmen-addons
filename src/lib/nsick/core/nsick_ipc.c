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

#include "nsick_ipc.h"

int carmen_nsick_ipc_initialize(int argc, char *argv[]) {
  IPC_RETURN_TYPE err;

  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);

  err = IPC_defineMsg(CARMEN_NSICK_STATUS_MESSAGE_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_NSICK_STATUS_MESSAGE_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_NSICK_STATUS_MESSAGE_NAME);

  err = IPC_defineMsg(CARMEN_NSICK_LASERPOS_MESSAGE_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_NSICK_LASERPOS_MESSAGE_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_NSICK_LASERPOS_MESSAGE_NAME);

  err = IPC_defineMsg(CARMEN_NSICK_POINTCLOUD_MESSAGE_NAME,
    IPC_VARIABLE_LENGTH, CARMEN_NSICK_POINTCLOUD_MESSAGE_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_NSICK_POINTCLOUD_MESSAGE_NAME);

  return 0;
}

void carmen_nsick_publish_status(double pos, double timestamp) {
  carmen_nsick_status_message status;
  IPC_RETURN_TYPE err;

  status.pos = pos;

  status.timestamp = timestamp;
  status.host = carmen_get_host();

  err = IPC_publishData(CARMEN_NSICK_STATUS_MESSAGE_NAME, &status);
  carmen_test_ipc_exit(err, "Could not publish",
    CARMEN_NSICK_STATUS_MESSAGE_NAME);
}

void carmen_nsick_publish_laserpos(int laser_id, double x, double y, double z,
    double yaw, double pitch, double roll, double timestamp) {
  carmen_nsick_laserpos_message laserpos;
  IPC_RETURN_TYPE err;

  laserpos.laser_id = laser_id;
  laserpos.x = x;
  laserpos.y = y;
  laserpos.z = z;
  laserpos.yaw = yaw;
  laserpos.pitch = pitch;
  laserpos.roll = roll;

  laserpos.timestamp = timestamp;
  laserpos.host = carmen_get_host();

  err = IPC_publishData(CARMEN_NSICK_LASERPOS_MESSAGE_NAME, &laserpos);
  carmen_test_ipc_exit(err, "Could not publish",
    CARMEN_NSICK_LASERPOS_MESSAGE_NAME);
}

void carmen_nsick_publish_pointcloud(int laser_id, int num_points, float* x,
    float* y, float* z, double timestamp) {
  carmen_nsick_pointcloud_message pointcloud;
  IPC_RETURN_TYPE err;

  pointcloud.laser_id = laser_id;
  pointcloud.num_points = num_points;
  pointcloud.x = x;
  pointcloud.y = y;
  pointcloud.z = z;

  pointcloud.timestamp = timestamp;
  pointcloud.host = carmen_get_host();

  err = IPC_publishData(CARMEN_NSICK_POINTCLOUD_MESSAGE_NAME, &pointcloud);
  carmen_test_ipc_exit(err, "Could not publish",
    CARMEN_NSICK_POINTCLOUD_MESSAGE_NAME);
}
