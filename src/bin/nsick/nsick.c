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
#include <carmen/laser_interface.h>

#include <libcan/can_serial.h>
#include <libnsick/nsick.h>

#include "nsick_messages.h"
#include "nsick_ipc.h"

int quit = 0;

nsick_device_t dev;
transform_pose_t pose;

char* serial_dev;

int node_id = 1;
char* enc_type;
int enc_pulses = 256;
double current = 2.0;
double gear_trans = -51.0;

char* home_method;
double home_curr = 1.0;
double home_vel = 10.0;
double home_acc = 10.0;
double home_pos = -92.0;

double start_pos = -45.0;
double end_pos = 45.0;
double max_vel = 45.0;
double max_acc = 45.0;
double control_freq = 10.0;

int laser_id = 1;
double laser_freq = 75.0;
double laser_x = 0.037;
double laser_y = 0.0;
double laser_z = -0.032;
double laser_yaw = 0.0;
double laser_pitch = 0.0;
double laser_roll = 0.0;

int points_publish = 0;

void carmen_nsick_sigint_handler(int q __attribute__((unused))) {
  quit = 1;
}

void carmen_nsick_laser_handler(carmen_laser_laser_message* message) {
  int num_points = message->num_readings;
  transform_point_t points[num_points];
  float x[num_points], y[num_points], z[num_points];
  int i;

  fprintf(stderr, "L");
  for (i = 0; i < num_points; ++i) {
    float angle = message->config.start_angle+
      i/(float)num_points*message->config.fov;
    transform_point_init(&points[i], message->range[i]*cos(angle),
      message->range[i]*sin(angle), 0.0);
  }
  nsick_sensor_transform_points(&dev.sensor, pose.pitch, points, num_points);
  for (i = 0; i < num_points; ++i) {
    x[i] = points[i].x;
    y[i] = points[i].y;
    z[i] = points[i].z;
  }
  
  carmen_nsick_publish_pointcloud(message->id, num_points, x, y, z,
    message->timestamp);
}

void carmen_nsick_points_handler(char* module, char* variable, char* value) {
  if (points_publish)
    carmen_laser_subscribe_laser_message(laser_id, NULL,
      (carmen_handler_t)carmen_nsick_laser_handler, CARMEN_SUBSCRIBE_LATEST);
  else
    carmen_laser_unsubscribe_laser_message(laser_id,
      (carmen_handler_t)carmen_nsick_laser_handler);
}

int carmen_nsick_read_parameters(int argc, char **argv) {
  char module[] = "nsick";
  int num_params;

  carmen_param_t params[] = {
    {module, "can_serial_dev", CARMEN_PARAM_STRING, &serial_dev, 0, NULL},

    {module, "epos_node_id", CARMEN_PARAM_INT, &node_id, 0, NULL},
    {module, "epos_enc_type", CARMEN_PARAM_STRING, &enc_type, 0, NULL},
    {module, "epos_enc_pulses", CARMEN_PARAM_INT, &enc_pulses, 0, NULL},
    {module, "epos_current", CARMEN_PARAM_DOUBLE, &current, 0, NULL},
    {module, "epos_gear_trans", CARMEN_PARAM_DOUBLE, &gear_trans, 0, NULL},

    {module, "epos_home_method", CARMEN_PARAM_STRING, &home_method, 0, NULL},
    {module, "epos_home_curr", CARMEN_PARAM_DOUBLE, &home_curr, 0, NULL},
    {module, "epos_home_vel", CARMEN_PARAM_DOUBLE, &home_vel, 0, NULL},
    {module, "epos_home_acc", CARMEN_PARAM_DOUBLE, &home_acc, 0, NULL},
    {module, "epos_home_pos", CARMEN_PARAM_DOUBLE, &home_pos, 0, NULL},

    {module, "start_pos", CARMEN_PARAM_DOUBLE, &start_pos, 0, NULL},
    {module, "end_pos", CARMEN_PARAM_DOUBLE, &end_pos, 0, NULL},
    {module, "max_vel", CARMEN_PARAM_DOUBLE, &max_vel, 0, NULL},
    {module, "max_acc", CARMEN_PARAM_DOUBLE, &max_acc, 0, NULL},
    {module, "control_freq", CARMEN_PARAM_DOUBLE, &control_freq, 0, NULL},

    {module, "points_publish", CARMEN_PARAM_ONOFF, &points_publish,  1,
      carmen_nsick_points_handler},

    {module, "laser_id", CARMEN_PARAM_INT, &laser_id, 0, NULL},
    {module, "laser_freq", CARMEN_PARAM_DOUBLE, &laser_freq, 0, NULL},
    {module, "laser_x", CARMEN_PARAM_DOUBLE, &laser_x, 0, NULL},
    {module, "laser_y", CARMEN_PARAM_DOUBLE, &laser_y, 0, NULL},
    {module, "laser_z", CARMEN_PARAM_DOUBLE, &laser_z, 0, NULL},
    {module, "laser_yaw", CARMEN_PARAM_DOUBLE, &laser_yaw, 0, NULL},
    {module, "laser_pitch", CARMEN_PARAM_DOUBLE, &laser_pitch, 0, NULL},
    {module, "laser_roll", CARMEN_PARAM_DOUBLE, &laser_roll, 0, NULL},
  };

  num_params = sizeof(params)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, params, num_params);

  return num_params;
}

void carmen_nsick_init(nsick_device_t* dev) {
  config_t can_config, epos_config, nsick_config;
  config_init(&can_config);
  config_init(&epos_config);
  config_init(&nsick_config);

  config_set_string(&can_config, CAN_SERIAL_PARAMETER_DEVICE, serial_dev);

  config_set_int(&epos_config, EPOS_PARAMETER_DEVICE_NODE_ID, node_id);
  if (!strcmp(enc_type, "3chan"))
    config_set_int(&epos_config, EPOS_PARAMETER_SENSOR_TYPE, epos_sensor_3chan);
  else if (!strcmp(enc_type, "2chan"))
    config_set_int(&epos_config, EPOS_PARAMETER_SENSOR_TYPE, epos_sensor_2chan);
  else if (!strcmp(enc_type, "hall"))
    config_set_int(&epos_config, EPOS_PARAMETER_SENSOR_TYPE, epos_sensor_hall);
  else
    carmen_die("ERROR: unknown value of parameter epos_enc_type\n");
  config_set_int(&epos_config, EPOS_PARAMETER_SENSOR_PULSES, enc_pulses);
  config_set_float(&epos_config, EPOS_PARAMETER_MOTOR_CURRENT, current);
  config_set_float(&epos_config, EPOS_PARAMETER_GEAR_TRANSMISSION, gear_trans);
  if (!strcmp(home_method, "pos_current"))
    config_set_int(&epos_config, EPOS_PARAMETER_HOME_METHOD,
      epos_home_pos_current);
  else if (!strcmp(home_method, "neg_current"))
    config_set_int(&epos_config, EPOS_PARAMETER_HOME_METHOD,
      epos_home_neg_current);
  else
    carmen_die("ERROR: unknown value of parameter epos_home_method\n");
  config_set_float(&epos_config, EPOS_PARAMETER_HOME_CURRENT, home_curr);
  config_set_float(&epos_config, EPOS_PARAMETER_HOME_VELOCITY, home_vel);
  config_set_float(&epos_config, EPOS_PARAMETER_HOME_ACCELERATION, home_acc);
  config_set_float(&epos_config, EPOS_PARAMETER_HOME_POSITION, home_pos);

  config_set_float(&nsick_config, NSICK_PARAMETER_START_POSITION, start_pos);
  config_set_float(&nsick_config, NSICK_PARAMETER_END_POSITION, end_pos);
  config_set_float(&nsick_config, NSICK_PARAMETER_MAX_VELOCITY, max_vel);
  config_set_float(&nsick_config, NSICK_PARAMETER_MAX_ACCELERATION, max_acc);
  config_set_float(&nsick_config, NSICK_PARAMETER_CONTROL_FREQUENCY,
    control_freq);
  config_set_float(&nsick_config, NSICK_PARAMETER_SENSOR_X, laser_x);
  config_set_float(&nsick_config, NSICK_PARAMETER_SENSOR_Y, laser_y);
  config_set_float(&nsick_config, NSICK_PARAMETER_SENSOR_Z, laser_z);
  config_set_float(&nsick_config, NSICK_PARAMETER_SENSOR_YAW, laser_yaw);
  config_set_float(&nsick_config, NSICK_PARAMETER_SENSOR_PITCH, laser_pitch);
  config_set_float(&nsick_config, NSICK_PARAMETER_SENSOR_ROLL, laser_roll);
  
  can_device_t* can_dev = malloc(sizeof(can_device_t));
  can_device_init_config(can_dev, &can_config);
  epos_node_t* epos_node = malloc(sizeof(epos_node_t));
  epos_node_init_config(epos_node, can_dev, &epos_config);
  nsick_init_config(dev, epos_node, can_dev, &nsick_config);

  config_destroy(&can_config);
  config_destroy(&epos_config);
  config_destroy(&nsick_config);
}

int carmen_nsick_home(nsick_device_t* dev) {
  int result;

  if (!quit && !(result = nsick_home(dev, 0.0)))
    while (!quit && nsick_home_wait(dev, 0.1));
  if (quit)
    nsick_home_stop(dev);

  return result;
}

int carmen_nsick_nod(nsick_device_t* dev, ssize_t num_sweeps) {
  int result;
  double timestamp;

  if (!quit && !(result = nsick_start(dev, num_sweeps))) {
    while (!quit && nsick_wait(dev, 0.0)) {
      timestamp = nsick_get_pose_estimate(dev, &pose);
      carmen_nsick_publish_laserpos(laser_id, pose.x, pose.y, pose.z,
        pose.yaw, pose.pitch, pose.roll, timestamp);
      carmen_ipc_sleep(1.0/laser_freq);
    }
    if (quit)
      nsick_stop(dev);
  }

  return result;
}

int carmen_nsick_disconnect(nsick_device_t* dev) {
  return nsick_disconnect(dev);
}

int main(int argc, char *argv[]) {
  ssize_t num_sweeps = 0;

  if (argc > 1)
    num_sweeps = atoi(argv[1]);

  carmen_nsick_ipc_initialize(argc, argv);
  carmen_nsick_read_parameters(argc, argv);

  if (points_publish)
    carmen_laser_subscribe_laser_message(laser_id, NULL,
      (carmen_handler_t)carmen_nsick_laser_handler, CARMEN_SUBSCRIBE_LATEST);
  
  signal(SIGINT, carmen_nsick_sigint_handler);

  carmen_nsick_init(&dev);
  if (nsick_connect(&dev))
    carmen_die("ERROR: initialization failed\n");
  if (!quit && carmen_nsick_home(&dev))
    carmen_die("ERROR: homing failed\n");

  if (!quit && carmen_nsick_nod(&dev, num_sweeps))
    carmen_die("ERROR: profile travel failed\n");
  carmen_nsick_disconnect(&dev);

  nsick_destroy(&dev);
  return 0;
}
