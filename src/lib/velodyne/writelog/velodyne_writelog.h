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

/**
 * \file velodyne_writelog.h
 * \brief Library for writing Velodyne HDL log files.
 *
 * This library should be used to write logfiles for the Velodyne HDL.
 **/

#ifndef CARMEN_VELODYNE_WRITELOG_H
#define CARMEN_VELODYNE_WRITELOG_H

#include <carmen/carmen_stdio.h>

#include "velodyne_messages.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CARMEN_LOGFILE_HEADER "# CARMEN Logfile"

void velodyne_writelog_write_header(carmen_FILE *outfile);

void velodyne_writelog_write_velodyne_packet(carmen_velodyne_packet_message
  *packet, carmen_FILE *outfile, double timestamp);

#ifdef __cplusplus
}
#endif

#endif
// @}
