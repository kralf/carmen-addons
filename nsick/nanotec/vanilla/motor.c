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
#include <string.h>

#include "nanotec.h"
#include "serial.h"

int nanotec_motor_write(nanotec_motor_p motor, unsigned char *command,
  ssize_t size) {
  int result = NANOTEC_FALSE;
  unsigned char write_buffer[NANOTEC_MAX_COMMAND_SIZE];
  unsigned char read_buffer[NANOTEC_MAX_COMMAND_SIZE];
  int pos = 0, i;
  ssize_t written = 0, read = 0;
  int loop = 1;

  write_buffer[pos++] = 0x23;
  write_buffer[pos++] = 0x01;
  for(i = 0; i < size; i++)
    write_buffer[pos++] = command[i];
  write_buffer[pos++] = 0x0d;

  // DEBUG
  //fprintf(stderr, "\n");
  //for(i = 0;i < pos;i ++)
  //  fprintf(stderr, "%x ", write_buffer[i]);
  //fprintf(stderr, "\n");
  // DEBUG

  nanotec_serial_write(&motor->dev, write_buffer, pos, &written);
  if ((nanotec_serial_read(&motor->dev, read_buffer, written-1, &read)) &&
    (read == written-1) && !strncmp(&write_buffer[1], read_buffer, read))
    result = NANOTEC_TRUE;

  return result;
}

int nanotec_motor_read(nanotec_motor_p motor, unsigned char *data,
  unsigned char *command, ssize_t size, int timeout) {
  unsigned char write_buffer[NANOTEC_MAX_COMMAND_SIZE];
  unsigned char read_buffer[NANOTEC_MAX_COMMAND_SIZE];
  int pos = 0, i;
  ssize_t written = 0, read = 0;

  write_buffer[pos++] = 0x23;
  write_buffer[pos++] = 0x01;
  for(i = 0; i < size; i++)
    write_buffer[pos++] = command[i];
  write_buffer[pos++] = 0x0d;

  nanotec_serial_write(&motor->dev, write_buffer, pos, &written);

  pos = 0;
  nanotec_serial_write(&motor->dev, write_buffer, pos, &written);
  if (nanotec_serial_read(&motor->dev, read_buffer, size+1, &read) &&
    !strncmp(&write_buffer[1], read_buffer, size)) {
    while (nanotec_serial_read(&motor->dev, read_buffer, 1, &read)) {
      if (read_buffer[0] != 0x0d)
        data[pos++] = read_buffer[0];
      else
        break;
    }
  }

  // DEBUG
  //fprintf(stderr, "\n");
  //for(i = 0;i < pos;i ++)
  //  fprintf(stderr, "%x ", data[i]);
  //fprintf(stderr, "\n");
  // DEBUG

  return pos;
}

void nanotec_motor_convert(int number, int pos, int digits, unsigned char*
  buffer) {
  char ascii[10];
  int i;

  sprintf(ascii, "%i", number);
  for (i = 0; i < digits; i++)
    buffer[pos + i] = ascii[i];
}

int nanotec_motor_test(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;
  int data_length;
  unsigned char data[NANOTEC_BUFFER_SIZE];
  unsigned char cmd[1] = {0x24};

  fprintf(stderr, "INFO: motor test ... ");
  data_length = nanotec_motor_read(motor, data, cmd, 1, NANOTEC_TEST_TIMEOUT);

  if ((data_length == 1) && ((data[0] & 0x01) == 0x01))
    result = NANOTEC_TRUE;

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_work_mode(nanotec_motor_p motor, int mode) {
  int result = NANOTEC_FALSE;
  unsigned char cmd_work_mode_pos[2] = {0x21, 0x31};
  unsigned char cmd_work_mode_rev[2] = {0x21, 0x32};
  unsigned char cmd_work_mode_flag[2] = {0x21, 0x33};
  unsigned char cmd_work_mode_cycle[2] = {0x21, 0x34};

  fprintf(stderr, "INFO: set work mode %d ... ", mode);
  switch(mode) {
  case NANOTEC_WORK_MODE_POS:
    if(nanotec_motor_write(motor, cmd_work_mode_pos, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_WORK_MODE_VEL:
    if(nanotec_motor_write(motor, cmd_work_mode_rev, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_WORK_MODE_FLAG:
    if(nanotec_motor_write(motor, cmd_work_mode_flag, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_WORK_MODE_CYCLE:
    if(nanotec_motor_write(motor, cmd_work_mode_cycle, 2))
      result = NANOTEC_TRUE;
    break;
  }

  if (result == NANOTEC_TRUE) {
    fprintf(stderr, "ok\n");
    motor->settings.work_mode = mode;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_step_mode(nanotec_motor_p motor, int mode) {
  int result = NANOTEC_FALSE;
  unsigned char cmd_step_mode_rel[2] = {0x70, 0x31};
  unsigned char cmd_step_mode_abs[2] = {0x70, 0x32};
  unsigned char cmd_step_mode_int[2] = {0x70, 0x33};
  unsigned char cmd_step_mode_ext[2] = {0x70, 0x34};

  fprintf(stderr, "INFO: set step mode %d ... ", mode);
  switch(mode) {
  case NANOTEC_STEP_MODE_REL:
    if(nanotec_motor_write(motor, cmd_step_mode_rel, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_MODE_ABS:
    if(nanotec_motor_write(motor, cmd_step_mode_abs, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_MODE_INT:
    if(nanotec_motor_write(motor, cmd_step_mode_int, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_MODE_EXT:
    if(nanotec_motor_write(motor, cmd_step_mode_ext, 2))
      result = NANOTEC_TRUE;
    break;
  }

  if (result == NANOTEC_TRUE) {
    fprintf(stderr, "ok\n");
    motor->settings.step_mode = mode;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_direction(nanotec_motor_p motor, nanotec_dir_t
  direction) {
  int result = NANOTEC_FALSE;
  unsigned char cmd_rigth[2] = {0x64, 0x31};
  unsigned char cmd_left[2] = {0x64, 0x30};

  fprintf(stderr, "INFO: set rotational direction %d ... ", direction);
  switch(direction) {
  case NANOTEC_RIGHT:
    if(nanotec_motor_write(motor, cmd_rigth, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_LEFT:
    if(nanotec_motor_write(motor, cmd_left, 2))
      result = NANOTEC_TRUE;
    break;
  }

  if (result == NANOTEC_TRUE) {
    fprintf(stderr, "ok\n");
    motor->settings.direction = direction;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_repetitions(nanotec_motor_p motor, int num_reps) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

  fprintf(stderr, "INFO: set repetitions %d ... ", num_reps);
  if ((num_reps >= 0) && (num_reps <= 255)) {
    cmd[0] = 0x57;
    digits = nanotec_get_digits(num_reps);
    nanotec_motor_convert(num_reps, 1, digits, cmd);

    if(nanotec_motor_write(motor, cmd, digits+1))
      result = NANOTEC_TRUE;
  }

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_change_dir(nanotec_motor_p motor, int change_dir) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[NANOTEC_MAX_COMMAND_SIZE];

  fprintf(stderr, "INFO: set change of direction %d ... ", change_dir);
  cmd[0] = 0x74;
  cmd[1] = 0x30+change_dir;
  if(nanotec_motor_write(motor, cmd, 2))
    result = NANOTEC_TRUE;

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_steps(nanotec_motor_p motor, int num_steps) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

  fprintf(stderr, "INFO: set steps %d ... ", num_steps);
  if ((num_steps >= 0) && (num_steps <= 8388607)) {
    cmd[0] = 0x73;
    digits = nanotec_get_digits(num_steps);
    nanotec_motor_convert(num_steps, 1, digits, cmd);

    if(nanotec_motor_write(motor, cmd, digits+1))
      result = NANOTEC_TRUE;
  }

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_break(nanotec_motor_p motor, float time) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[NANOTEC_MAX_COMMAND_SIZE];
  int time_val = time*10;
  int digits;

  fprintf(stderr, "INFO: set break %.2fs ... ", time);
  if ((time_val >= 1) && (time_val <= 255)) {
    cmd[0] = 0x50;
    digits = nanotec_get_digits(time_val);
    nanotec_motor_convert(time_val, 1, digits, cmd);

    if(nanotec_motor_write(motor, cmd, digits+1))
      result = NANOTEC_TRUE;
  }

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_step_size(nanotec_motor_p motor, int size) {
  int result = NANOTEC_FALSE;
  unsigned char cmd_step_size_1_1[2] = {0x67, 0x31}; // 1/1
  unsigned char cmd_step_size_1_2[2] = {0x67, 0x32}; // 1/2
  unsigned char cmd_step_size_1_4[2] = {0x67, 0x34}; // 1/4
  unsigned char cmd_step_size_1_5[2] = {0x67, 0x35}; // 1/5
  unsigned char cmd_step_size_1_8[2] = {0x67, 0x38}; // 1/8
  unsigned char cmd_step_size_1_10[3] = {0x67, 0x31, 0x30}; // 1/10

  fprintf(stderr, "INFO: set step size %d ... ", size);
  switch(size) {
  case NANOTEC_STEP_SIZE_1_1:
    if(nanotec_motor_write(motor, cmd_step_size_1_1, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_SIZE_1_2:
    if(nanotec_motor_write(motor, cmd_step_size_1_2, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_SIZE_1_4:
    if(nanotec_motor_write(motor, cmd_step_size_1_4, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_SIZE_1_5:
    if(nanotec_motor_write(motor, cmd_step_size_1_5, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_SIZE_1_8:
    if(nanotec_motor_write(motor, cmd_step_size_1_8, 2))
      result = NANOTEC_TRUE;
    break;
  case NANOTEC_STEP_SIZE_1_10:
    if(nanotec_motor_write(motor, cmd_step_size_1_10, 3))
      result = NANOTEC_TRUE;
    break;
  }

  if (result == NANOTEC_TRUE) {
    fprintf(stderr, "ok\n");
    motor->settings.step_size = size;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_phase_current(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[4] = {0x69, 0x31, 0x30, 0x30};

  fprintf(stderr, "INFO: set phase current 100%% ... ");
  if(nanotec_motor_write(motor, cmd, 4))
    result = NANOTEC_TRUE;

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_current_reduction(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[4] = {0x72, 0x31, 0x30, 0x30};

  fprintf(stderr, "INFO: set current reduction none ... ");
  if(nanotec_motor_write(motor, cmd, 4))
    result = NANOTEC_TRUE;

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_clearance(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[3] = {0x7a, 0x30, 0x30};

  fprintf(stderr, "INFO: set clearance none ... ");
  if(nanotec_motor_write(motor, cmd, 3)) {
    usleep(NANOTEC_CMD_TIMEOUT*1e6);
    result = NANOTEC_TRUE;
  }

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_start_freq(nanotec_motor_p motor, int freq) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

  fprintf(stderr, "INFO: set start frequency %dHz ... ", freq);
  if ((freq >= 100) || (freq <= 20000)) {
    cmd[0] = 0x75;
    digits = nanotec_get_digits(freq);
    nanotec_motor_convert(freq, 1, digits, cmd);

    if(nanotec_motor_write(motor, cmd, digits+1))
      result = NANOTEC_TRUE;
  }
  
  if (result == NANOTEC_TRUE) {
    fprintf(stderr, "ok\n");
    motor->settings.start_freq = freq;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_max_freq(nanotec_motor_p motor, int freq) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

  fprintf(stderr, "INFO: set maximum frequency %dHz ... ", freq);
  if ((freq >= 100) || (freq <= 20000)) {
    cmd[0] = 0x6f;
    digits = nanotec_get_digits(freq);
    nanotec_motor_convert(freq, 1, digits, cmd);

    if(nanotec_motor_write(motor, cmd, digits+1))
      result = NANOTEC_TRUE;
  }

  if (result == NANOTEC_TRUE) {
    fprintf(stderr, "ok\n");
    motor->settings.max_freq = freq;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_ramp(nanotec_motor_p motor, int ramp) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[NANOTEC_MAX_COMMAND_SIZE];
  int digits;

  fprintf(stderr, "INFO: set ramp %d ... ", ramp);
  if ((ramp >= 1) || (ramp <= 255)) {
    cmd[0] = 0x62;
    digits = nanotec_get_digits(ramp);
    nanotec_motor_convert(ramp, 1, digits, cmd);

    if(nanotec_motor_write(motor, cmd, digits+1))
      result = NANOTEC_TRUE;
  }

  if (result == NANOTEC_TRUE) {
    fprintf(stderr, "ok\n");
    motor->settings.ramp = ramp;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_set_switch_behavior(nanotec_motor_p motor, int behavior) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[4] = {0x6c, 0x69, 0x62, 0x30+behavior};

  fprintf(stderr, "INFO: set switch behavior %d ... ", behavior);
  if(nanotec_motor_write(motor, cmd, 4))
    result = NANOTEC_TRUE;

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_save(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[2] = {0x3e, 0x31};

  fprintf(stderr, "INFO: save settings ... ");
  if(nanotec_motor_write(motor, cmd, 2))
    result = NANOTEC_TRUE;

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_start(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[1] = {0x41};

  fprintf(stderr, "INFO: start motor ... ");
  if(nanotec_motor_write(motor, cmd, 1)) {
    usleep(NANOTEC_CMD_TIMEOUT*1e6);
    result = NANOTEC_TRUE;
  }

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_stop(nanotec_motor_p motor) {
  int result = NANOTEC_FALSE;
  unsigned char cmd[1] = {0x53};

  fprintf(stderr, "INFO: stop motor ... ");
  if(nanotec_motor_write(motor, cmd, 1))
    result = NANOTEC_TRUE;

  if (result == NANOTEC_TRUE)
    fprintf(stderr, "ok\n");
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_motor_get_status(nanotec_motor_p motor, int mask) {
  int data_length;
  unsigned char data[NANOTEC_BUFFER_SIZE];
  unsigned char cmd[1] = {0x24};

  data_length = nanotec_motor_read(motor, data, cmd, 1, NANOTEC_DATA_TIMEOUT);
  if (data_length != 1) {
    fprintf(stderr, "nanotec_motor_get_status: failed\n");
    return 0;
  }

  return data[0] & mask;
}

int nanotec_motor_get_position(nanotec_motor_p motor) {
  int data_length, pos, i;
  unsigned char data[NANOTEC_BUFFER_SIZE], b[3];
  unsigned char cmd[1] = {0x43};

  data_length = nanotec_motor_read(motor, data, cmd, 1, NANOTEC_DATA_TIMEOUT);
  if (data_length != 9) {
    fprintf(stderr, "nanotec_motor_get_position: failed\n");
    return 0;
  }

  for (i = 0; i < 3; ++i) {
    char str[4] = {0, 0, 0, 0};
    strncpy(str, &data[i*3], 3);
    b[i] = atoi(str);
  }

  pos = b[0]*65536+b[1]*256+b[2];

  return pos;
}

int nanotec_motor_wait_status(nanotec_motor_p motor, int mask) {
  int status = 0;

  while (1) {
    status = nanotec_motor_get_status(motor, mask);
    if (status == mask)
      break;
    else
      usleep(NANOTEC_POLL_TIMEOUT*1e6);
  }

  return status;
}
