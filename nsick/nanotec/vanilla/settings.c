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
  \file     settings.c
  \author   Stefan Gachter, Jan Weingarten, Ralf Kaestner <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
*/

#include <stdio.h>
#include <string.h>

#include "settings.h"

void nanotec_settings_default(nanotec_settings_p settings, int motor_id) {
  settings->motor_id = motor_id;
  settings->type = NANOTEC_MOTOR;

  strcpy(settings->device_name, "COM1");
  settings->baudrate = 19200;
  settings->databits = 8;
  settings->stopbits = NANOTEC_TWOSTOPBITS;
  settings->parity = NANOTEC_NOPARITY;

  settings->work_mode = NANOTEC_WORK_MODE_POS;
  settings->step_mode = NANOTEC_STEP_MODE_REL;
  settings->step_size = NANOTEC_STEP_SIZE_1_10;
  settings->direction = NANOTEC_LEFT;
  settings->start_freq = 100;
  settings->max_freq = 400;
  settings->ramp = 14;
  settings->init_pos = 0;
}

void nanotec_settings_init(nanotec_settings_p settings, char *device_name,
  int baudrate, int init_pos) {
  nanotec_settings_default(settings, 0);

  if (device_name != NULL)
    strcpy(settings->device_name, device_name);
  if (baudrate != 0)
    settings->baudrate = baudrate;
  if (init_pos != 0)
    settings->init_pos = init_pos;
}

void nanotec_settings_profile(nanotec_settings_p settings, int start_freq,
  int max_freq, int ramp) {
  if (start_freq != 0)
    settings->start_freq = start_freq;
  if (max_freq != 0)
    settings->max_freq = max_freq;
  if (ramp != 0)
    settings->ramp = ramp;
}

int nanotec_settings_check(nanotec_settings_p settings) {
  int result = NANOTEC_TRUE;

  if(settings->baudrate != 19200) {
    fprintf(stderr, "ERROR: baudrate of %d is not valid!\n",
      settings->baudrate);
    result = NANOTEC_FALSE;
  }

  if((settings->init_pos < 0) || (settings->init_pos > NANOTEC_MAX_INIT_POS)) {
    fprintf(stderr, "ERROR: number of steps of %d is out of bounds!\n",
	    settings->init_pos);
    result = NANOTEC_FALSE;
  }

  return result;
}
