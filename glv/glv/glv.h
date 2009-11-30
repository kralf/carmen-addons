#ifndef DGC_GLV_H
#define DGC_GLV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <carmen/global.h>

#include "my_stdio.h"

#define       MESSAGE_ID_POINT         0
#define       MESSAGE_ID_LINE          1
#define       MESSAGE_ID_FACE          2
#define       MESSAGE_ID_COLOR         3

typedef struct {
  unsigned char r, g, b;
} glv_color_t, *glv_color_p;

typedef struct {
  glv_color_t c;
  float x, y, z;
} glv_point_t, *glv_point_p;

typedef struct {
  glv_color_t c;
  glv_point_t p1, p2;
} glv_line_t, *glv_line_p;

typedef struct {
  glv_color_t c;
  glv_point_t p1, p2, p3;
  glv_point_t normal;
} glv_face_t, *glv_face_p;

typedef struct {
  int num_points, max_points;
  glv_point_p point;
  int num_lines, max_lines;
  glv_line_p line;
  int num_faces, max_faces;
  glv_face_p face;
  glv_point_t centroid, min, max;
} glv_object_t, *glv_object_p;

void write_color_glv(my_FILE *fp, unsigned char r, unsigned char g, 
		     unsigned char b);

void write_point_glv(my_FILE *fp, float x, float y, float z);

void write_line_glv(my_FILE *fp, float x1, float y1, float z1,
		    float x2, float y2, float z2);

void write_face_glv(my_FILE *fp, float x1, float y1, float z1,
		    float x2, float y2, float z2,
		    float x3, float y3, float z3);

glv_object_p glv_object_read(char *filename);

#ifdef __cplusplus
}
#endif

#endif
