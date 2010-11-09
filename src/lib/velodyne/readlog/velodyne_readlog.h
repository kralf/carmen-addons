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
 * \file velodyne_readlog.h
 * \brief Library for reading Velodyne HDL log files.
 *
 * This library should be used to read logfiles for the Velodyne HDL.
 **/

#ifndef CARMEN_VELODYNE_READLOG_H
#define CARMEN_VELODYNE_READLOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int laser_id;
  char* filename;
  long filepos;
  double timestamp;
} velodyne_package_t, *velodyne_package_p;

typedef struct {
  int num_package, max_package;
  velodyne_package_p package;
} logdata_t, *logdata_p;

void read_velodyne_logfile(char *filename, logdata_p logdata);

#ifdef __cplusplus
}
#endif

#endif
// @}
