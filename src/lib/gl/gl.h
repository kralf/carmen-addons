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
 * either version 2 of the License, or (at your option)f
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


/** @addtogroup gl **/
// @{

/** \file gl.h
  * \brief OpenGL visualization.
  *
  * This file specifies the OpenGL visualization interface.
  **/

#ifndef CARMEN_GL_H
#define CARMEN_GL_H

#include <GL/glut.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Rendering styles
  */
typedef enum {
  gl_style_points,
  gl_style_wireframe,
  gl_style_solid
} gl_style_t;

/** Initialize the OpenGL display.
  * This function initializes the display and creates a viewport window.
  */
void gl_initialize(int argc, char **argv);

/** Set the clear color.
  */
void gl_clear_color(float red, float green, float blue);

/** Set the clipping planes.
  */
void gl_clip(float near, float far);

/** Set the cursor parameters.
  */
void gl_cursor(float size);

/** Set the camera parameters.
  */
void gl_camera(float x, float y, float z, float pan, float tilt,
  float distance);

/** Enter the OpenGL main loop.
  * This function does not return.
  */
void gl_loop();

/** Draw the scene.
  * This function is abstract and must be implemented by the caller.
  */
void gl_draw_scene();

#ifdef __cplusplus
}
#endif

#endif

// @}
