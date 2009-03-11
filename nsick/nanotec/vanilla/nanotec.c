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
  \file     nanotec.c
  \author   Stefan Gachter, Jan Weingarten, Ralf Kaestner <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
*/

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "nanotec.h"

int nanotec_init(nanotec_motor_p motor, int motor_id, const char* device_name) {
  int result = NANOTEC_FALSE;
  nanotec_settings_default(&motor->settings, motor_id);

  fprintf(stderr, "INFO: motor type ... ");
  fprintf(stderr, "%s\n", (motor->settings.type == NANOTEC_MOTOR) ? "nanotec" :
    "unknown");
  if (nanotec_settings_check(&motor->settings) &&
    nanotec_serial_open(&motor->dev, device_name) &&
    nanotec_serial_setup(&motor->dev, motor->settings.baudrate,
      motor->settings.databits, motor->settings.stopbits,
      motor->settings.parity) &&
    nanotec_motor_stop(motor) &&
    nanotec_motor_test(motor)) {
    motor->type = motor->settings.type;
    return nanotec_setup(motor);
  }
  else
    fprintf(stderr, "ERROR: communication failed\n");

  return result;
}

int nanotec_close(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;

  if (nanotec_serial_close(&motor->dev))
    result = NANOTEC_TRUE;    

  return result;
}

int nanotec_setup(nanotec_motor_p motor) {
  if(!nanotec_motor_set_phase_current(motor))
    return NANOTEC_FALSE;    
  if(!nanotec_motor_set_current_reduction(motor))
    return NANOTEC_FALSE;    
  if(!nanotec_motor_set_clearance(motor))
    return NANOTEC_FALSE;

  if(!nanotec_motor_set_work_mode(motor, motor->settings.work_mode))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_step_mode(motor, motor->settings.step_mode))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_step_size(motor, motor->settings.step_size))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_direction(motor, motor->settings.direction))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_start_freq(motor, motor->settings.start_freq))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_max_freq(motor, motor->settings.max_freq))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_ramp(motor, motor->settings.ramp))
    return NANOTEC_FALSE;

  if(!nanotec_motor_set_repetitions(motor, 1))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_change_dir(motor, 0))
    return NANOTEC_FALSE;
  if(!nanotec_motor_set_break(motor, 0.1))
    return NANOTEC_FALSE;

  return NANOTEC_TRUE;
}

int nanotec_home(nanotec_motor_p motor, int offset) {
  nanotec_settings_t settings = motor->settings;

  nanotec_motor_set_start_freq(motor, 100);
  nanotec_motor_set_max_freq(motor, 100);

  nanotec_motor_set_step_mode(motor, NANOTEC_STEP_MODE_REL);
  nanotec_motor_set_direction(motor, NANOTEC_RIGHT);
  nanotec_motor_set_steps(motor, 100);
  nanotec_motor_start(motor);
  nanotec_motor_wait_status(motor, NANOTEC_STATUS_READY);

  nanotec_motor_set_step_mode(motor, NANOTEC_STEP_MODE_INT);
  nanotec_motor_set_direction(motor, NANOTEC_LEFT);
  nanotec_motor_start(motor);
  nanotec_motor_wait_status(motor, NANOTEC_STATUS_REF_REACHED);
  usleep(4000000);

  nanotec_motor_set_step_mode(motor, NANOTEC_STEP_MODE_REL);
  nanotec_motor_set_direction(motor, NANOTEC_RIGHT);
  nanotec_motor_set_steps(motor, offset);
  nanotec_motor_start(motor);
  nanotec_motor_wait_status(motor, NANOTEC_STATUS_READY);

  nanotec_motor_set_work_mode(motor, settings.work_mode);
  nanotec_motor_set_step_mode(motor, settings.step_mode);
  nanotec_motor_set_start_freq(motor, settings.start_freq);
  nanotec_motor_set_max_freq(motor, settings.max_freq);
}
