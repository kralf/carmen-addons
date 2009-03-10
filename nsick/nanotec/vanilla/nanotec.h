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
  \file     nanotec.h
  \author   Stefan Gachter, Jan Weingarten <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
  \log      7 Oct 2006: Initial implementation.
*/

#ifndef NANOTEC_H
#define NANOTEC_H

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

#include "global.h"


#define NANOTEC_NOPARITY                          0
#define NANOTEC_EVENPARITY                        1
#define NANOTEC_ONESTOPBIT                        1
#define NANOTEC_TWOSTOPBITS                       2
#define NANOTEC_FALSE                             0
#define NANOTEC_TRUE                              1
#define NANOTEC_INVALID_HANDLE                    -1

#define NANOTEC_BUFFER_SIZE                       1024
#define NANOTEC_MAX_COMMAND_SIZE                  1024
#define NANOTEC_MAX_NAME_LENGTH                   256

#define NANOTEC_POLL_WAIT_TIME                    1e-3
#define NANOTEC_MAX_TIME_FOR_DATA                 0.25
#define NANOTEC_MAX_TIME_FOR_TESTING_BAUDRATE     1.0

#define NANOTEC_WORK_MODE_POS                     1
#define NANOTEC_WORK_MODE_VEL                     2
#define NANOTEC_WORK_MODE_FLAG                    3
#define NANOTEC_WORK_MODE_CYCLE                   4

#define NANOTEC_STEP_MODE_REL                     1
#define NANOTEC_STEP_MODE_ABS                     2
#define NANOTEC_STEP_MODE_INT                     3
#define NANOTEC_STEP_MODE_EXT                     4

#define NANOTEC_STEP_SIZE_1_1                     1
#define NANOTEC_STEP_SIZE_1_2                     2
#define NANOTEC_STEP_SIZE_1_4                     4
#define NANOTEC_STEP_SIZE_1_5                     5
#define NANOTEC_STEP_SIZE_1_8                     8
#define NANOTEC_STEP_SIZE_1_10                    10

#define NANOTEC_MAX_NB_STEPS                      1000
#define NANOTEC_MAX_INIT_POS                      500

#define NANOTEC_STATUS_READY                      0x01
#define NANOTEC_STATUS_REF_REACHED                0x02
#define NANOTEC_STATUS_POS_ERROR                  0x04
#define NANOTEC_STATUS_TRAVEL_END                 0x08
#define NANOTEC_STATUS_MODE_POS                   0x10
#define NANOTEC_STATUS_MODE_VEL                   0x20
#define NANOTEC_STATUS_MODE_FLAG                  0x40

#define NANOTEC_SWITCH_DISABLE                    0
#define NANOTEC_SWITCH_RETURN                     1
#define NANOTEC_SWITCH_FORWARD                    2
#define NANOTEC_SWITCH_STOP                       3


typedef enum { NANOTEC_STEPMOTOR } motor_model_t;
typedef enum { NANOTEC_LEFT, NANOTEC_RIGHT } rot_dir_t;

typedef int NANOTEC_HANDLE;
typedef uint16_t NANOTEC_DWORD;
typedef uint8_t NANOTEC_BYTE;

/** defined the type of motor that is used **/
typedef enum {
  NANOTEC_MOTOR = 0,
  UNKNOWN_MOTOR = 99
} motor_type_t;


typedef struct {
  NANOTEC_HANDLE fd;
  char *name;
  motor_model_t type;
  int baudrate;
  NANOTEC_DWORD parity;
  NANOTEC_BYTE databits;
  NANOTEC_BYTE stopbits;
  int motor_num;
} motor_device_t, *motor_device_p;


typedef struct {
  char device_name[NANOTEC_MAX_NAME_LENGTH];
  motor_model_t type;
  int motor_num;
  int baudrate;
  NANOTEC_BYTE databits, stopbits;
  NANOTEC_DWORD parity;
  int work_mode, step_mode, step_size;
  rot_dir_t rot_dir;
  int start_freq, max_freq_1, max_freq_2, ramp;
  int nb_steps, init_pos;
  int change;
} motor_settings_t;


typedef struct {
  motor_settings_t settings;
  motor_device_t dev;
} nanotec_motor_t, *nanotec_motor_p;


/** API */
void nanotec_set_default_parameters(nanotec_motor_p motor, int motor_num);

/** API */
void nanotec_set_parameters(nanotec_motor_p motor, char *device_name,
  int baudrate, int nb_steps, int init_pos);

/** API */
int nanotec_set_step_mode(nanotec_motor_p motor, int mode);

/** API */
int nanotec_set_rot_direction(nanotec_motor_p motor, rot_dir_t dir);

/** API */
int nanotec_set_repetitions(nanotec_motor_p motor, int reps);

/** API */
int nanotec_set_steps(nanotec_motor_p motor, int steps);

/** API */
int nanotec_set_switch_behavior(nanotec_motor_p motor, int behavior);

/** API */
int nanotec_move(nanotec_motor_p motor);

/** API */
void nanotec_move_nsteps(nanotec_motor_p motor, rot_dir_t dir, int steps);

/** API */
void nanotec_home(nanotec_motor_p motor, int offset);

/** API */
void nanotec_start_motor(nanotec_motor_p motor);

/** API */
void nanotec_stop_motor(nanotec_motor_p motor);

/** API */
void nanotec_set_configuration(nanotec_motor_p motor);

/** API */
int nanotec_get_status(nanotec_motor_p motor, int mask);

/** API */
int nanotec_wait_status(nanotec_motor_p motor, int mask);

/** API */
int nanotec_get_position(nanotec_motor_p motor);


/** OS-specific implementation */
void nanotec_set_serial_params(nanotec_motor_p motor);

/** OS-specific implementation */
void nanotec_read_serial_port(nanotec_motor_p motor, unsigned char *data,
  int m, ssize_t *n);

/** OS-specific implementation */
void nanotec_write_serial_port(nanotec_motor_p motor, unsigned char *data,
  int m, ssize_t *n);

/** OS-specific implementation */
int nanotec_serial_connect(nanotec_motor_p motor);

/** OS-specific implementation */
void nanotec_set_baudrate(nanotec_motor_p motor, int brate);


/** generic implementation */
void nanotec_install_settings(nanotec_motor_p motor);

/** generic implementation */
void nanotec_connect_device(nanotec_motor_p motor);

/** generic implementation */
int nanotec_write_command(nanotec_motor_p motor, unsigned char *command,
  int cmd_length);

/** generic implementation */
int nanotec_read_data(nanotec_motor_p motor, unsigned char *data,
  unsigned char *command, int cmd_length, int timeout);

/** generic implementation */
void nanotec_check_settings(nanotec_motor_p motor);

/** generic implementation */
int nanotec_set_pause(nanotec_motor_p motor, int pause);

/** generic implementation */
int nanotec_set_working_mode(nanotec_motor_p motor, int mode);

/** generic implementation */
int nanotec_set_step_size(nanotec_motor_p motor, int size);

/** generic implementation */
int nanotec_set_phase_current(nanotec_motor_p motor);

/** generic implementation */
int nanotec_set_current_lowering(nanotec_motor_p motor);

/** generic implementation */
int nanotec_set_start_freq(nanotec_motor_p motor, int freq);

/** generic implementation */
int nanotec_set_max_freq(nanotec_motor_p motor, int freq, int nb);

/** generic implementation */
int nanotec_set_backlash(nanotec_motor_p motor);

/** generic implementation */
int nanotec_set_ramp(nanotec_motor_p motor, int steps);

/** generic implementation */
int nanotec_save_settings(nanotec_motor_p motor);

/** generic implementation */
int nanotec_test_baudrate(nanotec_motor_p motor, int baudrate);

/** generic implementation */
int nanotec_check_baudrate(nanotec_motor_p motor, int baudrate);

/** generic implementation */
void nanotec_set_config(nanotec_motor_p motor);

#endif
