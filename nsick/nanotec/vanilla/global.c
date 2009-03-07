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
 * This source code is based on the Carnegie Mellon Robot
 * Navigation Toolkit (CARMEN)
 *
 * CARMEN Copyright (c) 2002 Michael Montemerlo, Nicholas
 * Roy, Sebastian Thrun, Dirk Haehnel, Cyrill Stachniss,
 * and Jared Glover
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
  \file     global.c
  \author   Stefan Gachter <br>
            Autonomous Systems Laboratory <br>
            Swiss Federal Institute of Technology (ETHZ) <br>
            Zurich, Switzerland.
  \version  1.0
  \brief
  \log      7 Oct 2006: Initial implementation.
*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


int get_digits(int X)
{
	return (int)(log10((double)X) + 1);
}


int round_angle(double X)
{
  if (X >= 0)
    return (int)(X + 0.5);
  else
    return (int)(X - 0.5);
}

double degrees_to_radians(double theta)
{
  return (theta * M_PI / 180.0);
}


double get_time(void)
{
  struct timeval tv;
  double t;

  if (gettimeofday(&tv, NULL) < 0)
    fprintf(stderr, "Warning: get_time() encountered error in gettimeofday()!\n");
  t = tv.tv_sec + tv.tv_usec/1000000.0;
  return t;
}

