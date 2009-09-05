#include <carmen/global.h>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#include "transform.h"

void print_transform(transform_t t, char *str)
{
  int r, c;

  fprintf(stderr, "%s:\n", str);
  for(r = 0; r < 4; r++) {
    for(c = 0; c < 4; c++)
      fprintf(stderr, "%8.2f ", t[r][c]);
    fprintf(stderr, "\n");
  }
}

void create_transform_identity(transform_t t)
{
  int r, c;
  
  for(r = 0; r < 4; r++)
    for(c = 0; c < 4; c++)
      if(r == c)
        t[r][c] = 1;
      else
        t[r][c] = 0;
}

void transform_print(transform_t t, char *str)
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

void left_multiply_transform(transform_t t1, transform_t t2)
{
  transform_t result;
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

void rotate_transform_x(transform_t t, double theta)
{
  transform_t temp;
  double ctheta = cos(theta), stheta = sin(theta);

  create_transform_identity(temp);
  temp[1][1] = ctheta; 
  temp[1][2] = -stheta;
  temp[2][1] = stheta;
  temp[2][2] = ctheta;
  left_multiply_transform(t, temp);
}

void rotate_transform_y(transform_t t, double theta)
{
  transform_t temp;
  double ctheta = cos(theta), stheta = sin(theta);

  create_transform_identity(temp);
  temp[0][0] = ctheta; 
  temp[0][2] = stheta;
  temp[2][0] = -stheta;
  temp[2][2] = ctheta;
  left_multiply_transform(t, temp);
}

void rotate_transform_z(transform_t t, double theta)
{
  transform_t temp;
  double ctheta = cos(theta), stheta = sin(theta);

  create_transform_identity(temp);
  temp[0][0] = ctheta; 
  temp[0][1] = -stheta;
  temp[1][0] = stheta;
  temp[1][1] = ctheta;
  left_multiply_transform(t, temp);
}

void translate_transform(transform_t t, double x, double y, double z)
{
  t[0][3] += x;
  t[1][3] += y;
  t[2][3] += z;
}

void transform_copy(transform_t dest, transform_t src)
{
  int r, c;

  for(r = 0; r < 4; r++)
    for(c = 0; c < 4; c++)
      dest[r][c] = src[r][c];
}

void inverse_transform(transform_t in, transform_t out)
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

void transform_point(double *x, double *y, double *z, transform_t t)
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

