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
  \file     serial.h
  \author   Stefan Gachter, Jan Weingarten, Ralf Kaestner <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
*/

#ifndef NANOTEC_SERIAL_H
#define NANOTEC_SERIAL_H

#include <unistd.h>

#include "global.h"

typedef struct {
  NANOTEC_HANDLE fd;
  char name[NANOTEC_MAX_NAME_LENGTH];
  int baudrate;
  NANOTEC_DWORD parity;
  NANOTEC_BYTE databits;
  NANOTEC_BYTE stopbits;
} nanotec_device_t, *nanotec_device_p;

int nanotec_serial_open(nanotec_device_p dev, const char* device_name);

int nanotec_serial_close(nanotec_device_p dev);

int nanotec_serial_setup(nanotec_device_p dev, int baudrate, int databits,
  int stopbits, int parity);

int nanotec_serial_read(nanotec_device_p dev, unsigned char *data, int m,
  ssize_t *n);

int nanotec_serial_write(nanotec_device_p dev, unsigned char *data, int m,
  ssize_t *n);

#endif
