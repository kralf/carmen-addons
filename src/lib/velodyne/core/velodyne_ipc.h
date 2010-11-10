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

/** @addtogroup velodyne **/
// @{

/** \file velodyne_ipc.h
  * \brief Definition of the communication of this module.
  *
  * This file specifies the interface to publish messages of that module
  * via ipc.
  **/

#ifndef CARMEN_VELODYNE_IPC_H
#define CARMEN_VELODYNE_IPC_H

#include "velodyne_messages.h"

#ifdef __cplusplus
extern "C" {
#endif

int carmen_velodyne_ipc_initialize(int argc, char *argv[]);

void carmen_velodyne_publish_packet(int laser_id, char* filename, long filepos,
  double timestamp);
void carmen_velodyne_publish_pointcloud(int laser_id, int num_points, float* x,
  float* y, float* z, double timestamp);

#ifdef __cplusplus
}
#endif

#endif

// @}
