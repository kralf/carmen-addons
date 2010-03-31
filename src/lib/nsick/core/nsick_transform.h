/*********************************************************
 *
 * This source code is part of the Carnegie Mellon Robot
 * Navigation Toolkit (CARMEN)
 *
 * CARMEN Copyright (c) 2002 Michael Montemerlo, Nicholas
 * Roy, Sebastian Thrun, Dirk Haehnel, Cyrill Stachniss,
 * and Jared Glover
 *
 * CARMEN is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option)
 * any later version.
 *
 * CARMEN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General
 * Public License along with CARMEN; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307 USA
 *
 ********************************************************/

/** @addtogroup nsick **/
// @{

/** \file nsick_transform.h
  * \brief Transformation methods for this module.
  *
  * This file specifies transformation methods for the nodding SICK.
  **/

#ifndef CARMEN_NSICK_TRANSFORM_H
#define CARMEN_NSICK_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef double nsick_transform_t[4][4];

void nsick_print_transform(nsick_transform_t t, char *str);

void nsick_create_transform_identity(nsick_transform_t t);

void nsick_transform_print(nsick_transform_t t, char *str);

void nsick_left_multiply_transform(nsick_transform_t t1, nsick_transform_t t2);

void nsick_rotate_transform_x(nsick_transform_t t, double theta);

void nsick_rotate_transform_y(nsick_transform_t t, double theta);

void nsick_rotate_transform_z(nsick_transform_t t, double theta);

void nsick_translate_transform(nsick_transform_t t, double x, double y,
  double z);

void nsick_inverse_transform(nsick_transform_t in, nsick_transform_t out);

void nsick_transform_point(double *x, double *y, double *z,
  nsick_transform_t t);

void nsick_transform_copy(nsick_transform_t dest, nsick_transform_t src);

#ifdef __cplusplus
}
#endif

#endif
