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

#include <carmen/global.h>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#include "nsick_transform.h"

void nsick_print_transform(nsick_transform_t t, char *str)
{
  int r, c;

  fprintf(stderr, "%s:\n", str);
  for(r = 0; r < 4; r++) {
    for(c = 0; c < 4; c++)
      fprintf(stderr, "%8.2f ", t[r][c]);
    fprintf(stderr, "\n");
  }
}

void nsick_create_transform_identity(nsick_transform_t t)
{
  int r, c;

  for(r = 0; r < 4; r++)
    for(c = 0; c < 4; c++)
      if(r == c)
        t[r][c] = 1;
      else
        t[r][c] = 0;
}

void nsick_transform_print(nsick_transform_t t, char *str)
{
  int r, c;

  fprintf(stderr, "%s:\n", str);
  for(r = 0; r < 4; r++) {
    for(c = 0; c < 4; c++)
      fprintf(stderr, "%8.3f ", t[r][c]);
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
}

void nsick_left_multiply_transform(nsick_transform_t t1, nsick_transform_t t2)
{
  nsick_transform_t result;
  int i, j, k;

  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++) {
      result[i][j] = 0;
      for(k = 0; k < 4; k++)
        result[i][j] += t2[i][k] * t1[k][j];
    }
  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      t1[i][j] = result[i][j];
}

void nsick_rotate_transform_x(nsick_transform_t t, double theta)
{
  nsick_transform_t temp;
  double ctheta = cos(theta), stheta = sin(theta);

  nsick_create_transform_identity(temp);
  temp[1][1] = ctheta;
  temp[1][2] = -stheta;
  temp[2][1] = stheta;
  temp[2][2] = ctheta;
  nsick_left_multiply_transform(t, temp);
}

void nsick_rotate_transform_y(nsick_transform_t t, double theta)
{
  nsick_transform_t temp;
  double ctheta = cos(theta), stheta = sin(theta);

  nsick_create_transform_identity(temp);
  temp[0][0] = ctheta;
  temp[0][2] = stheta;
  temp[2][0] = -stheta;
  temp[2][2] = ctheta;
  nsick_left_multiply_transform(t, temp);
}

void nsick_rotate_transform_z(nsick_transform_t t, double theta)
{
  nsick_transform_t temp;
  double ctheta = cos(theta), stheta = sin(theta);

  nsick_create_transform_identity(temp);
  temp[0][0] = ctheta;
  temp[0][1] = -stheta;
  temp[1][0] = stheta;
  temp[1][1] = ctheta;
  nsick_left_multiply_transform(t, temp);
}

void nsick_translate_transform(nsick_transform_t t, double x, double y,
  double z)
{
  t[0][3] += x;
  t[1][3] += y;
  t[2][3] += z;
}

void nsick_transform_copy(nsick_transform_t dest, nsick_transform_t src)
{
  int r, c;

  for(r = 0; r < 4; r++)
    for(c = 0; c < 4; c++)
      dest[r][c] = src[r][c];
}

void nsick_inverse_transform(nsick_transform_t in, nsick_transform_t out)
{
  gsl_matrix *A = gsl_matrix_calloc(4, 4), *inverse = gsl_matrix_calloc(4, 4);
  gsl_permutation *perm = gsl_permutation_alloc(4);
  int r, c, signum;

  for(r = 0; r < 4; r++)
    for(c = 0; c < 4; c++)
      gsl_matrix_set(A, r, c, in[r][c]);
  gsl_linalg_LU_decomp(A, perm, &signum);
  gsl_linalg_LU_invert(A, perm, inverse);
  for(r = 0; r < 4; r++)
    for(c = 0; c < 4; c++)
      out[r][c] = gsl_matrix_get(inverse, r, c);
  gsl_matrix_free(A);
  gsl_matrix_free(inverse);
  gsl_permutation_free(perm);
}

void nsick_transform_point(double *x, double *y, double *z, nsick_transform_t t)
{
  double v1[3], v2[3];
  int r, c;

  v1[0] = *x;
  v1[1] = *y;
  v1[2] = *z;
  for(r = 0; r < 3; r++) {
    v2[r] = 0;
    for(c = 0; c < 3; c++)
      v2[r] += t[r][c] * v1[c];
    v2[r] += t[r][3];
  }
  *x = v2[0];
  *y = v2[1];
  *z = v2[2];
}

