#ifndef DGC_TRANSFORM_H
#define DGC_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef double transform_t[4][4];

void print_transform(transform_t t, char *str);

void create_transform_identity(transform_t t);

void transform_print(transform_t t, char *str);
   
void left_multiply_transform(transform_t t1, transform_t t2);

void rotate_transform_x(transform_t t, double theta);

void rotate_transform_y(transform_t t, double theta);

void rotate_transform_z(transform_t t, double theta);

void translate_transform(transform_t t, double x, double y, double z);

void inverse_transform(transform_t in, transform_t out);

void transform_point(double *x, double *y, double *z, transform_t t);

void transform_copy(transform_t dest, transform_t src);

#ifdef __cplusplus
}
#endif

#endif
