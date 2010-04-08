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

#include <libepos/epos.h>
#include <libepos/home.h>
#include <libepos/position_profile.h>

#include <tulibs/thread.h>

#include "nsick_messages.h"
#include "nsick_ipc.h"

int quit = 0;
pthread_mutex_t mutex;
double estimated_pos;

char* dev;

int node_id = 1;
char* enc_type;
int enc_pulses = 256;
double current = 2.0;
double gear_trans = 51.0;

char* home_method;
double home_curr = 1.0;
double home_vel = 10.0;
double home_acc = 10.0;
double home_pos = 92.0;

double nod_start = 45.0;
double nod_end = -45.0;
double nod_vel = 45.0;
double nod_acc = 45.0;

int laser_id = 1;
double laser_freq = 75.0;
double laser_x = 0.0;
double laser_z = 0.0;

void carmen_nsick_sigint_handler(int q __attribute__((unused))) {
  quit = 1;
}

int carmen_nsick_read_parameters(int argc, char **argv) {
  char module[] = "nsick";
  int num_params;

  carmen_param_t params[] = {
    {module, "epos_dev", CARMEN_PARAM_STRING, &dev, 0, NULL},

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

    {module, "nod_start", CARMEN_PARAM_DOUBLE, &nod_start, 0, NULL},
    {module, "nod_end", CARMEN_PARAM_DOUBLE, &nod_end, 0, NULL},
    {module, "nod_vel", CARMEN_PARAM_DOUBLE, &nod_vel, 0, NULL},
    {module, "nod_acc", CARMEN_PARAM_DOUBLE, &nod_acc, 0, NULL},

    {module, "laser_id", CARMEN_PARAM_INT, &laser_id, 0, NULL},
    {module, "laser_freq", CARMEN_PARAM_DOUBLE, &laser_freq, 0, NULL},
    {module, "laser_x", CARMEN_PARAM_DOUBLE, &laser_x, 0, NULL},
    {module, "laser_z", CARMEN_PARAM_DOUBLE, &laser_z, 0, NULL},
  };

  num_params = sizeof(params)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, params, num_params);

  return num_params;
}

void carmen_nsick_init(epos_node_p node) {
  config_t can_config, epos_config;
  config_init(&can_config);
  config_init(&epos_config);

  config_set_string(&can_config, "serial-dev", dev);

  config_set_int(&epos_config, EPOS_PARAMETER_ID, node_id);
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

  can_device_p can_dev = malloc(sizeof(can_device_t));
  can_init(can_dev, &can_config);
  epos_init(node, can_dev, &epos_config);

  config_destroy(&can_config);
  config_destroy(&epos_config);
}

int carmen_nsick_home(epos_node_p node) {
  int result;
  epos_home_t home;
  epos_position_profile_t profile;
  epos_home_method_t method = epos_home_pos_current;

  if (!strcmp(home_method, "pos_current"))
    method = epos_home_pos_current;
  else if (!strcmp(home_method, "neg_current"))
    method = epos_home_neg_current;
  else
    carmen_die("ERROR: unknown value of parameter epos_home_method\n");

  epos_home_init(&home, method, home_curr,
    carmen_degrees_to_radians(home_vel),
    carmen_degrees_to_radians(home_acc),
    carmen_degrees_to_radians(home_pos));
  if (!quit && !(result = epos_home_start(node, &home)))
    while (!quit && epos_home_wait(node, 0.1));
  if (quit) {
    epos_home_stop(node);
    return result;
  }

  epos_position_profile_init(&profile,
    carmen_degrees_to_radians(nod_start),
    carmen_degrees_to_radians(nod_vel),
    carmen_degrees_to_radians(nod_acc),
    carmen_degrees_to_radians(nod_acc),
    epos_profile_sinusoidal);
  if (!quit && !(result = epos_position_profile_start(node, &profile)))
    while (!quit && epos_profile_wait(node, 0.1));
  if (quit)
    epos_position_profile_stop(node);

  return result;
}

void* carmen_nsick_estimate(void* profile) {
  double x = 0.0, y = 0.0, z = 0.0;
  double yaw = 0.0, pitch = 0.0, roll = 0.0;
  double timestamp = carmen_get_time();

  if (pthread_mutex_trylock(&mutex) != EBUSY) {
    estimated_pos = epos_position_profile_estimate(profile, timestamp);
    pthread_mutex_unlock(&mutex);
  }

  pitch = estimated_pos;
  x = laser_x*cos(pitch)-laser_z*sin(pitch);
  z = laser_x*sin(pitch)+laser_z*cos(pitch);

  carmen_nsick_publish_laserpos(laser_id, x, y, z, yaw, pitch, roll, timestamp);

  return 0;
}

int carmen_nsick_nod(epos_node_p node, ssize_t num_sweeps) {
  int result = 0;
  float pos;
  double timestamp;
  epos_position_profile_t profile;
  thread_t thread;

  float start_pos = carmen_degrees_to_radians(nod_start);
  float end_pos = carmen_degrees_to_radians(nod_end);

  epos_position_profile_init(&profile, end_pos,
    carmen_degrees_to_radians(nod_vel),
    carmen_degrees_to_radians(nod_acc),
    carmen_degrees_to_radians(nod_acc),
    epos_profile_sinusoidal);
  estimated_pos = epos_get_position(node);

  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_lock(&mutex);
  thread_start(&thread, carmen_nsick_estimate, 0, &profile, laser_freq);

  ssize_t sweep = 0;
  while (!quit && (!num_sweeps || (sweep < num_sweeps)) &&
    !(result = epos_position_profile_start(node, &profile))) {
    pthread_mutex_unlock(&mutex);

    while (!quit && epos_profile_wait(node, 0.0)) {
      timestamp = carmen_get_time();
      pos = epos_get_position(node);
      timestamp = 0.5*(timestamp+carmen_get_time());

      carmen_nsick_publish_status(pos, timestamp);
    }

    pthread_mutex_lock(&mutex);
    profile.target_value = (profile.target_value == start_pos) ?
      end_pos : start_pos;

    ++sweep;
  }

  pthread_mutex_unlock(&mutex);
  thread_exit(&thread, 1);
  pthread_mutex_destroy(&mutex);
  epos_position_profile_stop(node);

  return result;
}

int carmen_nsick_close(epos_node_p node) {
  return epos_close(node);
}

int main(int argc, char *argv[]) {
  epos_node_t node;
  ssize_t num_sweeps = 0;

  if (argc > 1)
    num_sweeps = atoi(argv[1]);

  carmen_nsick_ipc_initialize(argc, argv);
  carmen_nsick_read_parameters(argc, argv);

  signal(SIGINT, carmen_nsick_sigint_handler);

  carmen_nsick_init(&node);
  if (epos_open(&node))
    carmen_die("ERROR: EPOS initialization failed\n");
  if (!quit && carmen_nsick_home(&node))
    carmen_die("ERROR: EPOS homing failed\n");

  if (!quit && carmen_nsick_nod(&node, num_sweeps))
    carmen_die("ERROR: EPOS profile travel failed\n");

  carmen_nsick_close(&node);

  epos_destroy(&node);
  return 0;
}
