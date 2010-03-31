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

/** @addtogroup glv **/
// @{

/** \file vrmlout.h
  * \brief Definition of the functions of this module.
  *
  * This file specifies the functions of this module.
  **/

#ifndef CARMEN_GLV_VRMLOUT_H
#define CARMEN_GLV_VRMLOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#define        VRML_POINTSET        0
#define        VRML_LINESET         1
#define        VRML_FACESET         2
#define        VRML_3NORMFACESET    3

#include <carmen/global.h>
#include <carmen/carmen_stdio.h>

typedef struct {
  char filename[200], normal_filename[200];
  int closed, object_type, num_points;
  carmen_FILE *fp, *normal_fp;
} vrml_object_t, *vrml_object_p;

typedef struct {
  char filename[200];
  int num_objects;
  vrml_object_p object;
} vrml_file_t, *vrml_file_p;

int vrml_file_open(vrml_file_p vrml_file, char *filename);

int vrml_file_new_object(vrml_file_p vrml_file, int object_type,
  float r, float g, float b);

void vrml_file_close_object(vrml_file_p vrml_file, int num);

void vrml_file_close(vrml_file_p vrml_file);

void vrml_add_point(vrml_file_p vrml_file, int num, float x, float y, float z);

void vrml_add_line(vrml_file_p vrml_file, int num, float x1, float y1,
  float z1, float x2, float y2, float z2);

void vrml_add_face(vrml_file_p vrml_file, int num,
  float x1, float y1, float z1,
  float x2, float y2, float z2,
  float x3, float y3, float z3);

void vrml_add_face_with_normals(vrml_file_p vrml_file, int num,
  float x1, float y1, float z1,
  float n1x, float n1y, float n1z,
  float x2, float y2, float z2,
  float n2x, float n2y, float n2z,
  float x3, float y3, float z3,
  float n3x, float n3y, float n3z);

#ifdef __cplusplus
}
#endif

#endif

// @}
