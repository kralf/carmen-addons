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
  \log      7 Oct 2006: Initial implementation.
*/


#include <stdlib.h>
#include <stdio.h>
#include "nanotec.h"
#include "util.h"

void nanotec_set_default_parameters(nanotec_motor_p motor, int motor_num) {
  motor->settings.type = NANOTEC_MOTOR;
  motor->settings.motor_num = motor_num;
  strcpy(motor->settings.device_name, "COM1");
  motor->settings.baudrate = 19200;
  motor->settings.databits = 8;
  motor->settings.parity = NANOTEC_NOPARITY;
  motor->settings.stopbits = NANOTEC_TWOSTOPBITS;
  motor->settings.work_mode = NANOTEC_WORK_MODE_POS;
  motor->settings.step_mode = NANOTEC_STEP_MODE_REL;
  motor->settings.step_size = NANOTEC_STEP_SIZE_1_10;
  motor->settings.rot_dir = NANOTEC_LEFT;
  motor->settings.start_freq = 100;
  motor->settings.max_freq_1 = 400;
  motor->settings.max_freq_2 = 400;
  motor->settings.ramp = 14;
  motor->settings.nb_steps = 0;
  motor->settings.init_pos = 0;
}

void nanotec_set_parameters(nanotec_motor_p motor, char *device_name,
  int baudrate, int nb_steps, int init_pos) {
  nanotec_set_default_parameters(motor, 0);

  /** Device **/
  if(device_name != NULL)
    strcpy(motor->settings.device_name, device_name);

  /** Baudrate **/
  if(baudrate != 0)
    motor->settings.baudrate = baudrate;

  /** Number of Steps **/
  if(nb_steps != 0)
    motor->settings.nb_steps = nb_steps;

  /** Number of Steps **/
  if(init_pos != 0)
    motor->settings.init_pos = init_pos;
}

void nanotec_install_settings(nanotec_motor_p motor) {
  motor->dev.type = motor->settings.type;
  motor->dev.baudrate = motor->settings.baudrate;
  motor->dev.parity = motor->settings.parity;
  motor->dev.fd = NANOTEC_INVALID_HANDLE;
  motor->dev.databits = motor->settings.databits;
  motor->dev.stopbits = motor->settings.stopbits;
  motor->dev.name =		// memory leak?
    (char *)malloc((strlen(motor->settings.device_name) + 1) * sizeof(char));
  test_alloc(motor->dev.name);
  strcpy(motor->dev.name, motor->settings.device_name);
}


void nanotec_connect_device(nanotec_motor_p motor) {
  nanotec_install_settings(motor);
  fprintf(stderr, "INFO: connect %s .................... ", motor->dev.name);
  if(!nanotec_serial_connect(motor)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");
  fprintf(stderr, "INFO: set port param %6d:%d%c%d ....... ",
	  motor->dev.baudrate, motor->dev.databits,
	  (motor->dev.parity == NANOTEC_NOPARITY ? 'N' :
    motor->dev.parity == NANOTEC_EVENPARITY ? 'E' : 'O'),
	  motor->dev.stopbits);
  nanotec_set_serial_params(motor);
  fprintf(stderr, "ok\n");
}


int nanotec_write_command(nanotec_motor_p motor, unsigned char *command,
  int cmd_length) {
  unsigned char write_buffer[NANOTEC_MAX_COMMAND_SIZE];
  unsigned char read_buffer[NANOTEC_MAX_COMMAND_SIZE];
  int pos = 0, i;
  ssize_t write_val = 0, read_val = 0;
  int loop, answer;
  double start_time;

  /* SYNC CHARS */
  write_buffer[pos++] = 0x23;
  /* ADDRESS */
  write_buffer[pos++] = 0x01;
  /* COMMAND */
  if(cmd_length > 0)
    for(i = 0; i < cmd_length; i++)
      write_buffer[pos++] = command[i];
  /* SYNC CHARS */
  write_buffer[pos++] = 0x0d;
  // DEBUG
  //fprintf(stderr, "\n");
  //for(i = 0;i < pos;i ++) {
  //  fprintf(stderr, "%x ", write_buffer[i]);
  //}
  //fprintf(stderr, "\n");
  // DEBUG
  nanotec_write_serial_port(motor, write_buffer, pos, &write_val);

  start_time = get_time();
  loop = 1;
  pos = 0;
  answer = NANOTEC_FALSE;
  while(loop) {
    nanotec_read_serial_port(motor, read_buffer, 1, &read_val);
    if(read_val == 1) {
      if(write_buffer[pos + 1] != read_buffer[0]) {
        pos = 0;
      }
      else {
        pos++;
        if(pos == (write_val - 2)) {
          answer = NANOTEC_TRUE;
          loop = 0;
        }
      }
    }
    if(get_time() - start_time > NANOTEC_MAX_TIME_FOR_DATA)
      loop = 0;
  }
  return(answer);
}


int nanotec_read_data(nanotec_motor_p motor, unsigned char *data,
  unsigned char *command, int cmd_length, int timeout) {
  unsigned char write_buffer[NANOTEC_MAX_COMMAND_SIZE];
  unsigned char read_buffer[NANOTEC_MAX_COMMAND_SIZE];
  int pos = 0, i;
  ssize_t write_val = 0, read_val = 0;

  /* SYNC CHARS */
  write_buffer[pos++] = 0x23;
  /* ADDRESS */
  write_buffer[pos++] = 0x01;
  /* COMMAND */
  if(cmd_length > 0)
    for(i = 0; i < cmd_length; i++)
      write_buffer[pos++] = command[i];
  /* SYNC CHARS */
  write_buffer[pos++] = 0x0d;
  nanotec_write_serial_port(motor, write_buffer, pos, &write_val);

  pos = 0;
  do {
    nanotec_read_serial_port(motor, read_buffer, 1, &read_val);
    if(read_val == 1)
      data[pos] = read_buffer[0];
    else
      return(0);
  }
  while(read_buffer[0] != 0x01);
  pos++;

  do {
    nanotec_read_serial_port(motor, read_buffer, 1, &read_val);
    if(read_val == 1)
      data[pos++] = read_buffer[0];
    else
      return(0);
  }
  while(read_buffer[0] != 0x0d);

  // DEBUG
  //fprintf(stderr, "\n");
  //for(i = 0;i < pos;i ++) {
  //  fprintf(stderr, "%x ", data[i]);
  //}
  //fprintf(stderr, "\n");
  // DEBUG

  return(pos);
}


void nanotec_check_settings(nanotec_motor_p motor) {
  if(motor->settings.baudrate != 19200) {
    fprintf(stderr, "ERROR: baudrate of %d is not valid!\n",
	    motor->settings.baudrate);
    exit(EXIT_FAILURE);
  }

  if((motor->settings.nb_steps < 0) ||
    (motor->settings.nb_steps > NANOTEC_MAX_NB_STEPS)) {
    fprintf(stderr, "ERROR: number of steps of %d is out of bounds!\n",
	    motor->settings.nb_steps);
    exit(EXIT_FAILURE);
  }

  if((motor->settings.init_pos < 0) ||
    (motor->settings.init_pos > NANOTEC_MAX_INIT_POS)) {
    fprintf(stderr, "ERROR: number of steps of %d is out of bounds!\n",
	    motor->settings.init_pos);
    exit(EXIT_FAILURE);
  }
}

//void CNanotecMotor::writeNumberToTelegram(int number, int idxstart,
//  int ndigits, unsigned char *telegram)
void convert_number(int number, int pos, int digits, unsigned char *buffer) {
  char ascii[10];
  int i;

	sprintf(ascii, "%i", number);
	for (i = 0; i < digits; i++)
		buffer[pos + i] = ascii[i];
}


//int CNanotecMotor::setpause(int npause)
int nanotec_set_pause(nanotec_motor_p motor, int pause) {
  unsigned char	cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

	if ((pause < 1) || (pause > 255)) // Pause is pause*0.1s.
		return(NANOTEC_FALSE);

	cmd[0] = 0x50;
  digits = get_digits(pause);
  convert_number(pause, 1, digits, cmd);

  if(!nanotec_write_command(motor, cmd, digits+1))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


//int CNanotecMotor::betriebsmodussetzen(int welchen)
int nanotec_set_working_mode(nanotec_motor_p motor, int mode) {
	unsigned char cmd_work_mode_pos[2] = {0x21, 0x31};
	unsigned char cmd_work_mode_rev[2] = {0x21, 0x32};
	unsigned char cmd_work_mode_flag[2] = {0x21, 0x33};
	unsigned char cmd_work_mode_cycle[2] = {0x21, 0x34};

  switch(mode) {
  case NANOTEC_WORK_MODE_POS:
    if(!nanotec_write_command(motor, cmd_work_mode_pos, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_WORK_MODE_VEL:
    if(!nanotec_write_command(motor, cmd_work_mode_rev, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_WORK_MODE_FLAG:
    if(!nanotec_write_command(motor, cmd_work_mode_flag, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_WORK_MODE_CYCLE:
    if(!nanotec_write_command(motor, cmd_work_mode_cycle, 2))
      return(NANOTEC_FALSE);
    break;
  }
  motor->settings.work_mode = mode;
  return(NANOTEC_TRUE);
}


//int CNanotecMotor::schrittmodussetzen(int welchen)
int nanotec_set_step_size(nanotec_motor_p motor, int size) {
	unsigned char cmd_step_size_1_1[2] = {0x67, 0x31}; // 1/1
	unsigned char cmd_step_size_1_2[2] = {0x67, 0x32}; // 1/2
	unsigned char cmd_step_size_1_4[2] = {0x67, 0x34}; // 1/4
	unsigned char cmd_step_size_1_5[2] = {0x67, 0x35}; // 1/5
	unsigned char cmd_step_size_1_8[2] = {0x67, 0x38}; // 1/8
	unsigned char cmd_step_size_1_10[3] = {0x67, 0x31, 0x30}; // 1/10

  switch(size) {
  case NANOTEC_STEP_SIZE_1_1:
    if(!nanotec_write_command(motor, cmd_step_size_1_1, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_SIZE_1_2:
    if(!nanotec_write_command(motor, cmd_step_size_1_2, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_SIZE_1_4:
    if(!nanotec_write_command(motor, cmd_step_size_1_4, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_SIZE_1_5:
    if(!nanotec_write_command(motor, cmd_step_size_1_5, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_SIZE_1_8:
    if(!nanotec_write_command(motor, cmd_step_size_1_8, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_SIZE_1_10:
    if(!nanotec_write_command(motor, cmd_step_size_1_10, 3))
      return(NANOTEC_FALSE);
    break;
  }
  motor->settings.step_size = size;
  return(NANOTEC_TRUE);
}


//void CNanotecMotor::phasenstrom()
int nanotec_set_phase_current(nanotec_motor_p motor) {
	unsigned char cmd[4] = {0x69, 0x31, 0x30, 0x30};

  if(!nanotec_write_command(motor, cmd, 4))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


//void CNanotecMotor::stromabsenkung()
int nanotec_set_current_lowering(nanotec_motor_p motor) {
	unsigned char cmd[4] = {0x72, 0x31, 0x30, 0x30};

  if(!nanotec_write_command(motor, cmd, 4))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


//int CNanotecMotor::startfrequenzsetzen(int startF)
int nanotec_set_start_freq(nanotec_motor_p motor, int freq) {
  unsigned char	cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

	if ((freq < 100) || (freq > 20000))
		return(NANOTEC_FALSE);

	cmd[0] = 0x75;
  digits = get_digits(freq);
  convert_number(freq, 1, digits, cmd);

  if(!nanotec_write_command(motor, cmd, digits+1))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
  motor->settings.start_freq = freq;
}


//int CNanotecMotor::maxfrequenzsetzen(int maxF)
int nanotec_set_max_freq(nanotec_motor_p motor, int freq, int nb) {
  unsigned char	cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

	if ((freq < 100) || (freq > 20000))
		return(NANOTEC_FALSE);

  switch(nb) {
  case 1:
    cmd[0] = 0x6f;
    break;
  case 2:
    cmd[0] = 0x6e;
    break;
  }

  digits = get_digits(freq);
  convert_number(freq, 1, digits, cmd);

  if(!nanotec_write_command(motor, cmd, digits+1))
    return(NANOTEC_FALSE);
  switch(nb) {
  case 1:
    motor->settings.max_freq_1 = freq;
    break;
  case 2:
    motor->settings.max_freq_2 = freq;
    break;
  }
  return(NANOTEC_TRUE);
}


//void CNanotecMotor::umkehrspiel()
int nanotec_set_backlash(nanotec_motor_p motor) {
	unsigned char cmd[3] = {0x7a, 0x30, 0x30};

  if(!nanotec_write_command(motor, cmd, 3))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


//int CNanotecMotor::rampesetzen(int nsteps)
int nanotec_set_ramp(nanotec_motor_p motor, int steps) {
  unsigned char	cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

	if ((steps < 1) || (steps > 255))
		return(NANOTEC_FALSE);

	cmd[0] = 0x62;
  digits = get_digits(steps);
  convert_number(steps, 1, digits, cmd);

  if(!nanotec_write_command(motor, cmd, digits+1))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
  motor->settings.ramp = steps;
}


//void CNanotecMotor::satzspeichern()
int nanotec_save_settings(nanotec_motor_p motor) {
	unsigned char cmd[2] = {0x3e, 0x31};

  if(!nanotec_write_command(motor, cmd, 2))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


int nanotec_test_baudrate(nanotec_motor_p motor, int baudrate) {
  int data_length;
  unsigned char data[NANOTEC_BUFFER_SIZE];
  unsigned char cmd[1] = {0x24};

  nanotec_set_baudrate(motor, baudrate);
  data_length = nanotec_read_data(motor, data, cmd, 1,
    NANOTEC_MAX_TIME_FOR_TESTING_BAUDRATE);

  if(data_length != 4)
    return(NANOTEC_FALSE);

  if((data[2] && 0x01) != 0x01)
    return(NANOTEC_FALSE);

  return(NANOTEC_TRUE);
}


int nanotec_check_baudrate(nanotec_motor_p motor, int baudrate) {
  fprintf(stderr, "INFO: check baudrate .................. %d ... ", baudrate);
  if(nanotec_test_baudrate(motor, baudrate)) {
    fprintf(stderr, "ok\n");
    return(NANOTEC_TRUE);
  }
  else {
    fprintf(stderr, "failed\n");
    return(NANOTEC_FALSE);
  }
}


//void CNanotecMotor::initializePositioning()
void nanotec_set_config(nanotec_motor_p motor) {
  fprintf(stderr, "INFO: set working mode ................ %d ... ",
    motor->settings.work_mode);
  if(!nanotec_set_working_mode(motor, motor->settings.work_mode)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set rotational direction ........ %d ... ",
    motor->settings.rot_dir);
  if(!nanotec_set_rot_direction(motor, motor->settings.rot_dir)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set step mode ................... %d ... ",
    motor->settings.step_mode);
  if(!nanotec_set_step_mode(motor, motor->settings.step_mode)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set step size ................... %d ... ",
    motor->settings.step_size);
  if(!nanotec_set_step_size(motor, motor->settings.step_size)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set steps ....................... %d ... ", 1);
  if(!nanotec_set_steps(motor, 1)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set repetitions ................. %d ... ", 1);
  if(!nanotec_set_repetitions(motor, 1)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set pause ....................... %.2fs ... ", 0.1*1);
  if(!nanotec_set_pause(motor, 1)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set phase current ............... 100%% ... ");
  if(!nanotec_set_phase_current(motor)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set current lowering ............ none ... ");
  if(!nanotec_set_current_lowering(motor)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set start frequency ............. %dHz ... ",
    motor->settings.start_freq);
  if(!nanotec_set_start_freq(motor, motor->settings.start_freq)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set first maximum frequency ..... %dHz ... ",
    motor->settings.max_freq_1);
  if(!nanotec_set_max_freq(motor, motor->settings.max_freq_1, 1)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set second maximum frequency .... %dHz ... ",
    motor->settings.max_freq_2);
  if(!nanotec_set_max_freq(motor, motor->settings.max_freq_2, 2)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set ramp ........................ %d ... ",
    motor->settings.ramp);
  if(!nanotec_set_ramp(motor, motor->settings.ramp)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  fprintf(stderr, "INFO: set backlash .................... none ... ");
  if(!nanotec_set_backlash(motor)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");

  motor->settings.change = NANOTEC_TRUE;
}


int nanotec_set_step_mode(nanotec_motor_p motor, int mode) {
	unsigned char cmd_step_mode_rel[2] = {0x70, 0x31};
	unsigned char cmd_step_mode_abs[2] = {0x70, 0x32};
	unsigned char cmd_step_mode_int[2] = {0x70, 0x33};
	unsigned char cmd_step_mode_ext[2] = {0x70, 0x34};

  switch(mode) {
  case NANOTEC_STEP_MODE_REL:
    if(!nanotec_write_command(motor, cmd_step_mode_rel, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_MODE_ABS:
    if(!nanotec_write_command(motor, cmd_step_mode_abs, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_MODE_INT:
    if(!nanotec_write_command(motor, cmd_step_mode_int, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_STEP_MODE_EXT:
    if(!nanotec_write_command(motor, cmd_step_mode_ext, 2))
      return(NANOTEC_FALSE);
    break;
  }
  motor->settings.step_mode = mode;
  return(NANOTEC_TRUE);
}


int nanotec_set_rot_direction(nanotec_motor_p motor, rot_dir_t dir) {
  unsigned char cmd_rigth[2] = {0x64, 0x31};
  unsigned char cmd_left[2] = {0x64, 0x30};

  switch(dir) {
  case NANOTEC_RIGHT:
    if(!nanotec_write_command(motor, cmd_rigth, 2))
      return(NANOTEC_FALSE);
    break;
  case NANOTEC_LEFT:
    if(!nanotec_write_command(motor, cmd_left, 2))
      return(NANOTEC_FALSE);
    break;
  }
  motor->settings.rot_dir = dir;
  return(NANOTEC_TRUE);
}


int nanotec_set_repetitions(nanotec_motor_p motor, int reps) {
  unsigned char	cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

	if ((reps < 1) || (reps > 255))
		return(NANOTEC_FALSE);

	cmd[0] = 0x57;
  digits = get_digits(reps);
  convert_number(reps, 1, digits, cmd);

  if(!nanotec_write_command(motor, cmd, digits+1))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


int nanotec_set_steps(nanotec_motor_p motor, int steps) {
  unsigned char	cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

	if ((steps < 1) || (steps > 10000))
		return(NANOTEC_FALSE);

	cmd[0] = 0x73;
  digits = get_digits(steps);
  convert_number(steps, 1, digits, cmd);

  if(!nanotec_write_command(motor, cmd, digits+1))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


int nanotec_move(nanotec_motor_p motor) {
  unsigned char cmd[1] = {0x41};

  if(!nanotec_write_command(motor, cmd, 1))
    return(NANOTEC_FALSE);
  return(NANOTEC_TRUE);
}


void nanotec_move_nsteps(nanotec_motor_p motor, rot_dir_t dir, int steps) {
  int step_mode;

  step_mode = motor->settings.step_mode;
  nanotec_set_step_mode(motor, NANOTEC_STEP_MODE_REL);

  nanotec_set_rot_direction(motor, dir);
  nanotec_set_repetitions(motor, 1);
  nanotec_set_steps(motor, steps);
  nanotec_move(motor);

  nanotec_set_step_mode(motor, step_mode);
}


void nanotec_set_reference(nanotec_motor_p motor, int start_pos) {
  int step_mode, start_freq, max_freq;

  step_mode = motor->settings.step_mode;
  start_freq = motor->settings.start_freq;
  max_freq = motor->settings.max_freq_1;

  nanotec_set_start_freq(motor, 100);
  nanotec_set_max_freq(motor, 100, 1);

  nanotec_move_nsteps(motor, NANOTEC_RIGHT, 300);
  nanotec_wait_status(motor, NANOTEC_STATUS_READY);

  nanotec_set_step_mode(motor, NANOTEC_STEP_MODE_INT);
  nanotec_set_rot_direction(motor, NANOTEC_LEFT);
  nanotec_move(motor);
  nanotec_wait_status(motor, NANOTEC_STATUS_READY);

  if(start_pos > 0) {
    nanotec_move_nsteps(motor, NANOTEC_RIGHT, start_pos);
    nanotec_wait_status(motor, NANOTEC_STATUS_READY);
  }

  nanotec_set_step_mode(motor, step_mode);
  nanotec_set_start_freq(motor, start_freq);
  nanotec_set_max_freq(motor, max_freq, 1);
}


void nanotec_start_motor(nanotec_motor_p motor) {
  nanotec_check_settings(motor);

  fprintf(stderr, "INFO: MOTOR type ...................... ");
  fprintf(stderr, "%s\n", (motor->settings.type == NANOTEC_MOTOR) ? "NANOTEC" :
    "UNKNOWN_MOTOR");

  /* open the serial port */
  nanotec_connect_device(motor);

  /* make sure the baudrate is set correctly */
  if(!nanotec_check_baudrate(motor, motor->settings.baudrate)) {
    fprintf(stderr, "ERROR: communication does not work!\n");
    exit(EXIT_FAILURE);
  }

  /* set start position */
  nanotec_set_reference(motor, motor->settings.init_pos);
}


void nanotec_stop_motor(nanotec_motor_p motor) {
  fprintf(stderr, "INFO: disconnect %s ................. ", motor->dev.name);
  if(!close(motor->dev.fd)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");
}


void nanotec_set_configuration(nanotec_motor_p motor) {
  nanotec_check_settings(motor);

  fprintf(stderr, "INFO: MOTOR type ...................... ");
  fprintf(stderr, "%s\n", (motor->settings.type == NANOTEC_MOTOR) ? "NANOTEC" :
    "UNKNOWN_MOTOR");

  /* open the serial port */
  nanotec_connect_device(motor);

  /* make sure the baudrate is set correctly */
  if(!nanotec_check_baudrate(motor, motor->settings.baudrate)) {
    fprintf(stderr, "ERROR: communication does not work!\n");
    exit(EXIT_FAILURE);
  }

  nanotec_set_config(motor);

  if(motor->settings.change) {
    while(nanotec_save_settings(motor));
  }

  fprintf(stderr, "INFO: disconnect %s ................. ", motor->dev.name);
  if(!close(motor->dev.fd)) {
    fprintf(stderr, "failed\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "ok\n");
}


int nanotec_get_status(nanotec_motor_p motor, int mask) {
  int data_length;
  unsigned char data[NANOTEC_BUFFER_SIZE];
  unsigned char cmd[1] = {0x24};

  data_length = nanotec_read_data(motor, data, cmd, 1,
    NANOTEC_MAX_TIME_FOR_DATA);
  if (data_length != 4) {
    fprintf(stderr, "nanotec_get_status: failed\n");
    return 0;
  }

  return data[2] & mask;
}

int nanotec_wait_status(nanotec_motor_p motor, int mask) {
  int status = 0;

  while (1) {
    status = nanotec_get_status(motor, mask);
    if (status == mask) {
      printf("reached: 0x%X\n", status);
      break;
    }
    else
      usleep(NANOTEC_POLL_TIME*1e6);
  }

  return status;
}

int nanotec_get_position(nanotec_motor_p motor) {
  int data_length, pos, i;
  unsigned char data[NANOTEC_BUFFER_SIZE], b[3];
  unsigned char cmd[1] = {0x43};

  data_length = nanotec_read_data(motor, data, cmd, 1,
    NANOTEC_MAX_TIME_FOR_DATA);
  if (data_length != 12) {
    fprintf(stderr, "nanotec_get_position: failed\n");
    return 0;
  }

  for (i = 0; i < 3; ++i) {
    char str[4] = {0, 0, 0, 0};
    strncpy(str, &data[i*3+2], 3);
    b[i] = atoi(str);
  }

  pos = b[0]*65536+b[1]*256+b[2];

  return pos;
}


void nanotec_set_serial_params(nanotec_motor_p motor) {
  if(0 != posix_util_serial_cfg(motor->dev.fd, motor->dev.baudrate,
    motor->settings.databits,
    motor->settings.stopbits == NANOTEC_TWOSTOPBITS ? 2 : 1,
    motor->settings.parity == NANOTEC_NOPARITY ?
    POSIX_UTIL_NOPARITY : (motor->settings.parity == NANOTEC_EVENPARITY ?
      POSIX_UTIL_EVENPARITY : POSIX_UTIL_ODDPARITY))){
    fprintf(stderr,
      "nanotec_set_serial_params: posix_util_serial_cfg failed\n");
    exit(EXIT_FAILURE);
  }
}


void nanotec_read_serial_port(nanotec_motor_p motor, unsigned char *data,
  int m, ssize_t *n) {
  if(0 != posix_util_buffer_read(motor->dev.fd, data, m, n,
    NANOTEC_MAX_TIME_FOR_DATA*1e3, 0)){
    fprintf(stderr,
      "nanotec_read_serial_port: posix_util_buffer_read failed on %s\n",
      motor->dev.name);
    exit(EXIT_FAILURE);
  }
}


void nanotec_write_serial_port(nanotec_motor_p motor, unsigned char *data,
  int m, ssize_t *n) {
  if(0 != posix_util_buffer_write(motor->dev.fd, data, m, n, 0)){
    fprintf(stderr,
      "nanotec_write_serial_port: posix_util_buffer_write failed on %s\n",
      motor->dev.name);
    exit(EXIT_FAILURE);
  }
}


int nanotec_serial_connect(nanotec_motor_p motor) {
  motor->dev.fd = posix_util_serial_open(motor->dev.name);
  if(motor->dev.fd < 0)
    return NANOTEC_FALSE;
  return NANOTEC_TRUE;
}


void nanotec_set_baudrate(nanotec_motor_p motor, int brate) {
  if(0 != posix_util_serial_cfg(motor->dev.fd, brate, motor->settings.databits,
        motor->settings.stopbits == NANOTEC_TWOSTOPBITS ? 2 : 1,
        motor->settings.parity == NANOTEC_NOPARITY ?
          POSIX_UTIL_NOPARITY : (motor->settings.parity == NANOTEC_EVENPARITY ?
                 POSIX_UTIL_EVENPARITY : POSIX_UTIL_ODDPARITY))){
    fprintf(stderr,
      "nanotec_set_baudrate: posix_util_serial_cfg failed on %s",
      motor->dev.name);
    exit(EXIT_FAILURE);
  }
}
