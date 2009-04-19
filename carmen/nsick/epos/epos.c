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

#include <carmen/carmen.h>

#include <epos.h>
#include <home.h>
#include <position_profile.h>

#include "epos_messages.h"
#include "epos_ipc.h"

int quit = 0;

char* dev;

int node_id = 1;
char* enc_type;
int enc_pulses = 256;
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

void carmen_epos_sigint_handler(int q __attribute__((unused))) {
  quit = 1;
}

int carmen_epos_read_parameters(int argc, char **argv) {
  int num_params;
  carmen_param_t params[] = {
    {"epos", "dev", CARMEN_PARAM_STRING, &dev, 0, NULL},

    {"epos", "node_id", CARMEN_PARAM_INT, &node_id, 0, NULL},
    {"epos", "enc_type", CARMEN_PARAM_STRING, &enc_type, 0, NULL},
    {"epos", "enc_pulses", CARMEN_PARAM_INT, &enc_pulses, 0, NULL},
    {"epos", "gear_trans", CARMEN_PARAM_DOUBLE, &gear_trans, 0, NULL},

    {"epos", "home_method", CARMEN_PARAM_STRING, &home_method, 0, NULL},
    {"epos", "home_curr", CARMEN_PARAM_DOUBLE, &home_curr, 0, NULL},
    {"epos", "home_vel", CARMEN_PARAM_DOUBLE, &home_vel, 0, NULL},
    {"epos", "home_acc", CARMEN_PARAM_DOUBLE, &home_acc, 0, NULL},
    {"epos", "home_pos", CARMEN_PARAM_DOUBLE, &home_pos, 0, NULL},

    {"epos", "nod_start", CARMEN_PARAM_DOUBLE, &nod_start, 0, NULL},
    {"epos", "nod_end", CARMEN_PARAM_DOUBLE, &nod_end, 0, NULL},
    {"epos", "nod_vel", CARMEN_PARAM_DOUBLE, &nod_vel, 0, NULL},
    {"epos", "nod_acc", CARMEN_PARAM_DOUBLE, &nod_acc, 0, NULL},

    {"epos", "laser_id", CARMEN_PARAM_INT, &laser_id, 0, NULL},
  };

  num_params = sizeof(params)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, params, num_params);

  return num_params;
}

int carmen_epos_init(epos_node_p node) {
  int num_params;
  epos_parameter_t params[] = {
    {"dev-name", ""},
    {"node-id", ""},
    {"enc-type", ""},
    {"enc-pulses", ""},
    {"gear-trans", ""},
  };

  strcpy(params[0].value, dev);
  sprintf(params[1].value, "%d", node_id);
  if (!strcmp(enc_type, "3chan"))
    sprintf(params[2].value, "%d", epos_enc_3chan);
  else if (!strcmp(enc_type, "2chan"))
    sprintf(params[2].value, "%d", epos_enc_2chan);
  else if (!strcmp(enc_type, "hall"))
    sprintf(params[2].value, "%d", epos_hall);
  else
    carmen_die("ERROR: unknown value of parameter epos_enc_type\n");
  sprintf(params[3].value, "%d", enc_pulses);
  sprintf(params[4].value, "%f", gear_trans);

  num_params = sizeof(params)/sizeof(epos_parameter_t);
  return epos_init(node, params, num_params);
}

int carmen_epos_home(epos_node_p node) {
  int result;
  epos_home_t home;
  epos_home_method_t method = epos_pos_current;

  if (!strcmp(home_method, "pos_current"))
    method = epos_pos_current;
  else if (!strcmp(home_method, "neg_current"))
    method = epos_neg_current;
  else
    carmen_die("ERROR: unknown value of parameter epos_home_method\n");

  epos_home_init(&home, method, home_curr,
    carmen_degrees_to_radians(home_vel),
    carmen_degrees_to_radians(home_acc),
    carmen_degrees_to_radians(home_pos));

  if (!(result = epos_home_start(node, &home))) {
    while (!quit && epos_home_wait(node, 0.1));
    epos_home_stop(node);
  }

  return result;
}

int carmen_epos_nod(epos_node_p node) {
  int result = 0;
  epos_position_profile_t profile;

  float start_pos = carmen_degrees_to_radians(nod_start);
  float end_pos = carmen_degrees_to_radians(nod_end);

  epos_position_profile_init(&profile, start_pos,
    carmen_degrees_to_radians(nod_vel),
    carmen_degrees_to_radians(nod_acc),
    carmen_degrees_to_radians(nod_acc),
    epos_sinusoidal);

  while (!quit && !(result = epos_position_profile_start(node, &profile))) {
    while (!quit && epos_profile_wait(node, 0.1)) {
      fprintf(stdout, "\rEPOS angular position: %8.2f deg",
        epos_get_position(node)*180.0/M_PI);
      fflush(stdout);
    }
    profile.target_value = (profile.target_value == start_pos) ?
      end_pos : start_pos;
  }
  fprintf(stdout, "\n");
  epos_position_profile_stop(node);

  return result;
}

int carmen_epos_close(epos_node_p node) {
  return epos_close(node);
}

int main(int argc, char *argv[]) {
  epos_node_t node;

  carmen_epos_ipc_initialize(argc, argv);
  carmen_epos_read_parameters(argc, argv);

  signal(SIGINT, carmen_epos_sigint_handler);

  if (carmen_epos_init(&node))
    carmen_die("ERROR: EPOS initialization failed\n");
  if (carmen_epos_home(&node))
    carmen_die("ERROR: EPOS homing failed\n");

  if (carmen_epos_nod(&node))
    carmen_die("ERROR: EPOS profile travel failed\n");

  carmen_epos_close(&node);

  return 0;
}
