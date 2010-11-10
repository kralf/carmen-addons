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
 * either version 2 of the License, or (at your option)f
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

/** \file velodyne_messages.h
  * \brief Definition of the messages for this module.
  *
  * This file specifies the messages for this modules used to transmit
  * data via ipc to other modules.
  **/

#ifndef CARMEN_VELODYNE_MESSAGES_H
#define CARMEN_VELODYNE_MESSAGES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int laser_id;
  char* filename;
  long filepos;
  double timestamp;
  char* host;
} carmen_velodyne_packet_message;

#define CARMEN_VELODYNE_PACKET_MESSAGE_NAME "velodyne_packet_message"
#define CARMEN_VELODYNE_PACKET_MESSAGE_FMT "{int,string,long,double,string}"

typedef struct {
  int laser_id;
  int num_points;
  float* x;
  float* y;
  float* z;
  double timestamp;
  char* host;
} carmen_velodyne_pointcloud_message;

#define CARMEN_VELODYNE_POINTCLOUD_MESSAGE_NAME "velodyne_pointcloud_message"
#define CARMEN_VELODYNE_POINTCLOUD_MESSAGE_FMT \
  "{int,int,<float:2>,<float:2>,<float:2>,double,string}"

#ifdef __cplusplus
}
#endif

#endif

// @}
