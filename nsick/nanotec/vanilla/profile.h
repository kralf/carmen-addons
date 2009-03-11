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
  \file     profile.h
  \author   Ralf Kaestner <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
*/

#ifndef NANOTEC_PROFILE_H
#define NANOTEC_PROFILE_H

#include "motor.h"

typedef struct {
  int step_mode;
  int step_size;

  nanotec_dir_t direction;
  int num_steps;
  int start_freq;
  int max_freq;
  int ramp;
  int num_reps;
  int change_dir;
  float break_time;
} nanotec_profile_t, *nanotec_profile_p;

void nanotec_profile_default(nanotec_profile_p profile);

void nanotec_profile_init(nanotec_profile_p profile, int num_steps,
  int max_freq, int ramp);

int nanotec_profile_start(nanotec_motor_p motor, nanotec_profile_p profile);

#endif
