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
  \file     motor.h
  \author   Stefan Gachter, Jan Weingarten, Ralf Kaestner <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
*/

#ifndef NANOTEC_MOTOR_H
#define NANOTEC_MOTOR_H

#include "settings.h"
#include "serial.h"

typedef struct {
  int id;
  nanotec_motor_type_t type;
  nanotec_settings_t settings;
  nanotec_device_t dev;
} nanotec_motor_t, *nanotec_motor_p;

int nanotec_motor_test(nanotec_motor_p motor);

int nanotec_motor_set_work_mode(nanotec_motor_p motor, int mode);

int nanotec_motor_set_step_mode(nanotec_motor_p motor, int mode);

int nanotec_motor_set_direction(nanotec_motor_p motor, nanotec_dir_t direction);

int nanotec_motor_set_repetitions(nanotec_motor_p motor, int num_reps);

int nanotec_motor_set_change_dir(nanotec_motor_p motor, int change_dir);

int nanotec_motor_set_steps(nanotec_motor_p motor, int num_steps);

int nanotec_motor_set_break(nanotec_motor_p motor, float time);

int nanotec_motor_set_step_size(nanotec_motor_p motor, int size);

int nanotec_motor_set_phase_current(nanotec_motor_p motor);

int nanotec_motor_set_current_reduction(nanotec_motor_p motor);

int nanotec_motor_set_clearance(nanotec_motor_p motor);

int nanotec_motor_set_start_freq(nanotec_motor_p motor, int freq);

int nanotec_motor_set_max_freq(nanotec_motor_p motor, int freq);

int nanotec_motor_set_ramp(nanotec_motor_p motor, int ramp);

int nanotec_motor_set_switch_behavior(nanotec_motor_p motor, int behavior);

int nanotec_motor_save(nanotec_motor_p motor);

int nanotec_motor_start(nanotec_motor_p motor);

int nanotec_motor_stop(nanotec_motor_p motor);

int nanotec_motor_get_status(nanotec_motor_p motor, int mask);

int nanotec_motor_get_position(nanotec_motor_p motor);

int nanotec_motor_wait_status(nanotec_motor_p motor, int mask);

#endif
