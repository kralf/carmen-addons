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
  \file     serial.c
  \author   Stefan Gachter, Jan Weingarten, Ralf Kaestner <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
*/

#include <stdio.h>
#include <string.h>

#include "serial.h"
#include "util.h"

int nanotec_serial_open(nanotec_device_p dev, const char* device_name) {
  int result = NANOTEC_FALSE;

  fprintf(stderr, "INFO: serial connect %s ... ", device_name);
  dev->fd = nanotec_util_serial_open(device_name);
  if(dev->fd > 0) {
    fprintf(stderr, "ok\n");
    strcpy(dev->name, device_name);
    result = NANOTEC_TRUE;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_serial_close(nanotec_device_p dev) {
  int result = NANOTEC_FALSE;

  fprintf(stderr, "INFO: serial disconnect %s ... ", dev->name);
  if (!nanotec_util_serial_close(dev->fd)) {
    fprintf(stderr, "ok\n");
    dev->fd = NANOTEC_INVALID_HANDLE;
    dev->name[0] = 0;
    result = NANOTEC_TRUE;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_serial_setup(nanotec_device_p dev, int baudrate, int databits,
  int stopbits, int parity) {
  int result = NANOTEC_FALSE;

  fprintf(stderr, "INFO: serial setup %s ... ", dev->name);
  if(!nanotec_util_serial_cfg(dev->fd, baudrate, databits,
    stopbits == NANOTEC_TWOSTOPBITS ? 2 : 1,
    parity == NANOTEC_NOPARITY ? noparity :
      (parity == NANOTEC_EVENPARITY ? evenparity : oddparity))) {
    fprintf(stderr, "ok\n");
    fprintf(stderr, "INFO: set port param %6d:%d%c%d ... ",
      baudrate, databits, (parity == NANOTEC_NOPARITY ? 'N' :
      parity == NANOTEC_EVENPARITY ? 'E' : 'O'), stopbits);
    fprintf(stderr, "ok\n");
    dev->baudrate = baudrate;
    dev->databits = databits;
    dev->stopbits = stopbits;
    dev->parity = parity;
    result = NANOTEC_TRUE;
  }
  else
    fprintf(stderr, "failed\n");

  return result;
}

int nanotec_serial_read(nanotec_device_p dev, unsigned char *data, int m,
  ssize_t *n) {
  int result = NANOTEC_FALSE;

  if(!nanotec_util_buffer_read(dev->fd, data, m, n,
    NANOTEC_DATA_TIMEOUT*1e3, 0))
    result = NANOTEC_TRUE;
  else
    fprintf(stderr,
    "nanotec_serial_read: nanotec_util_buffer_read failed on %s\n", dev->name);

  return result;
}

int nanotec_serial_write(nanotec_device_p dev, unsigned char *data, int m,
  ssize_t *n) {
  int result = NANOTEC_FALSE;

  if(!nanotec_util_buffer_write(dev->fd, data, m, n, 0))
    result = NANOTEC_TRUE;
  else
    fprintf(stderr,
    "nanotec_serial_write: nanotec_util_buffer_write failed on %s\n",
    dev->name);

  return result;
}
