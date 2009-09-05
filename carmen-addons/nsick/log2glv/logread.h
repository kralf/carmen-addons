#ifndef LOGREAD_H
#define LOGREAD_H

#include "transform.h"

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

#endif
