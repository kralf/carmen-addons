  /*!
 *
 * Copyright (C) 2006
 * Swiss Federal Institute of Technology, Zurich. All rights reserved.
 *
 * Developed at the Autonomous Systems Lab.
 * Visit our homepage at http://www.asl.ethz.ch/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/*!
  \file     profile.c
  \author   Ralf Kaestner <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
*/

#include <stdio.h>

#include "profile.h"

void nanotec_profile_default(nanotec_profile_p profile) {
  profile->step_mode = NANOTEC_STEP_MODE_REL;
  profile->step_size = NANOTEC_STEP_SIZE_1_10;

  profile->direction = NANOTEC_RIGHT;
  profile->num_steps = 1000;
  profile->start_freq = 100;
  profile->max_freq = 400;
  profile->ramp = 14;
  profile->num_reps = 1;
  profile->change_dir = 0;
  profile->break_time = 0.1;
}

void nanotec_profile_init(nanotec_profile_p profile, int num_steps,
  int max_freq, int ramp) {
  nanotec_profile_default(profile);

  profile->num_steps = num_steps;
  profile->max_freq = max_freq;
  profile->ramp = ramp;
}

int nanotec_profile_start(nanotec_motor_p motor, nanotec_profile_p profile) {
  nanotec_motor_set_work_mode(motor, NANOTEC_WORK_MODE_POS);
  nanotec_motor_set_switch_behavior(motor, NANOTEC_SWITCH_DISABLE);
  nanotec_motor_set_step_mode(motor, profile->step_mode);
  nanotec_motor_set_step_size(motor, profile->step_size);

  nanotec_motor_set_direction(motor, profile->direction);
  nanotec_motor_set_steps(motor, profile->num_steps);
  nanotec_motor_set_start_freq(motor, profile->start_freq);
  nanotec_motor_set_max_freq(motor, profile->max_freq);
  nanotec_motor_set_ramp(motor, profile->ramp);
  nanotec_motor_set_repetitions(motor, profile->num_reps);
  nanotec_motor_set_change_dir(motor, profile->change_dir);
  nanotec_motor_set_break(motor, profile->break_time);

  nanotec_motor_start(motor);
}
