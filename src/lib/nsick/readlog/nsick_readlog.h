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

/**
 * \file nsick_readlog.h
 * \brief Library for reading nodding SICK log files.
 *
 * This library should be used to read logfiles for the nodding SICK.
 **/

#ifndef NSICK_READLOG_H
#define NSICK_READLOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int laser_num;
  double x, y, z, yaw, pitch, roll;
  double timestamp;
} laser_pos_t, *laser_pos_p;

typedef struct {
  int ignore, laser_num;
  double x, y, z, roll, pitch, yaw;
  int num_readings;
  float *range;
  float *endpoint_x, *endpoint_y, *endpoint_z;
  double timestamp;
} laser_scan_t, *laser_scan_p;

typedef struct {
  int num_laser, max_laser;
  laser_scan_p laser;
  int num_pos, max_pos;
  laser_pos_p pos;
} logdata_t, *logdata_p;

void read_nsick_logfile(char *filename, logdata_p logdata);

#ifdef __cplusplus
}
#endif

#endif
// @}
