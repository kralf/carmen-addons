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

/** @addtogroup nsick **/
// @{

/** \file nsick_ipc.h
  * \brief Definition of the communication of this module.
  *
  * This file specifies the interface to publish messages of that module
  * via ipc.
  **/

#ifndef CARMEN_EPOS_IPC_H
#define CARMEN_EPOS_IPC_H

#include "nsick_messages.h"

#ifdef __cplusplus
extern "C" {
#endif

int carmen_nsick_ipc_initialize(int argc, char *argv[]);

void carmen_nsick_publish_status(double pos, double timestamp);

void carmen_nsick_publish_laserpos(int laser_id, double x, double y, double z,
  double yaw, double pitch, double roll, double timestamp);

#ifdef __cplusplus
}
#endif

#endif

// @}
