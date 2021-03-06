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

/** \file nsick_interface.h
  * \brief Definition of the interface of this module.
  *
  * This file specifies the interface to subscribe the messages of
  * that module and to receive its data via ipc.
  **/

#ifndef CARMEN_NSICK_INTERFACE_H
#define CARMEN_NSICK_INTERFACE_H

#include "nsick_messages.h"

#ifdef __cplusplus
extern "C" {
#endif

void carmen_nsick_subscribe_status_message(carmen_nsick_status_message*
  status, carmen_handler_t handler, carmen_subscribe_t subscribe_how);
void carmen_nsick_unsubscribe_status_message(carmen_handler_t handler);

void carmen_nsick_subscribe_laserpos_message(carmen_nsick_laserpos_message*
  laserpos, carmen_handler_t handler, carmen_subscribe_t subscribe_how);
void carmen_nsick_unsubscribe_laserpos_message(carmen_handler_t handler);

void carmen_nsick_subscribe_pointcloud_message(
  carmen_nsick_pointcloud_message* pointcloud, carmen_handler_t handler,
  carmen_subscribe_t subscribe_how);
void carmen_nsick_unsubscribe_pointcloud_message(carmen_handler_t handler);

#ifdef __cplusplus
}
#endif

#endif
// @}
