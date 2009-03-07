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

/** @addtogroup nanotec **/
// @{

/** \file nanotec_ipc.h
 * \brief Definition of the communication of the module nanotec.
 *
 * This file specifies the interface to publish messages of that module
 * via ipc.
 **/

#ifndef CARMEN_NANOTEC_IPC_H
#define CARMEN_NANOTEC_IPC_H

#include "nanotec_messages.h"

#ifdef __cplusplus
extern "C" {
#endif

int carmen_nanotec_ipc_initialize(int argc, char *argv[]);

void carmen_nanotec_publish_status(void);

void carmen_nanotec_publish_laserpos(void);

#ifdef __cplusplus
}
#endif

#endif

// @}
