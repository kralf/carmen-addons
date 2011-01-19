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

#include <math.h>
#include <signal.h>
#include <stdio.h>

#include <carmen/global.h>

#include "gl.h"

#define GL_WINDOW_WIDTH         600
#define GL_WINDOW_HEIGHT        400

#define GL_MAX_FPS              20.0

#define GL_WHEEL_UP             3
#define GL_WHEEL_DOWN           4

#define GL_CAMERA_IDLE          0
#define GL_CAMERA_ROTATING      1
#define GL_CAMERA_MOVING        2
#define GL_CAMERA_ZOOMING       3

#define GL_MIN_RANGE            1.0
#define GL_MAX_RANGE            100.0
#define GL_MIN_Z                -5.0

#define GL_ZOOM_SENSITIVITY     1.1
#define GL_ROTATE_SENSITIVITY   0.50
#define GL_MOVE_SENSITIVITY     0.001

int gl_stereo = 0;

float gl_clear_red = 0.0;
float gl_clear_green = 0.0;
float gl_clear_blue = 0.0;

float gl_cursor_size = 1.0;

int gl_window_width = GL_WINDOW_WIDTH;
int gl_window_height = GL_WINDOW_HEIGHT;

float gl_near = 0.5*GL_MIN_RANGE;
float gl_far = 2.0*GL_MAX_RANGE;

float gl_camera_z_offset = 0.0;
float gl_camera_x_offset = 0.0;
float gl_camera_y_offset = 0.0;
float gl_camera_pan = 0.0;
float gl_camera_tilt = 89.0;
float gl_camera_distance = GL_MIN_RANGE;
float gl_camera_aperture = 50.0;
float gl_camera_eye_ratio = 30.0;

int gl_camera_state = GL_CAMERA_IDLE;
int gl_last_mouse_x;
int gl_last_mouse_y;

void gl_keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27: case 'q': case 'Q':
      raise(SIGINT);
      break;
    case 'a':
      gl_camera_z_offset += 0.1;
      break;
    case 'z':
      gl_camera_z_offset -= 0.1;
      break;
    default:
      break;
  }
}

void gl_mouse(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    gl_last_mouse_x = x;
    gl_last_mouse_y = y;

    if (button == GLUT_LEFT_BUTTON)
      gl_camera_state = GL_CAMERA_ROTATING;
    else if (button == GLUT_RIGHT_BUTTON)
      gl_camera_state = GL_CAMERA_MOVING;
    else
      gl_camera_state = GL_CAMERA_IDLE;
  }
  else if (state == GLUT_UP) {
    if (button == GL_WHEEL_UP)
      gl_camera_distance /= GL_ZOOM_SENSITIVITY;
    else if (button == GL_WHEEL_DOWN)
      gl_camera_distance *= GL_ZOOM_SENSITIVITY;
    else
      gl_camera_state = GL_CAMERA_IDLE;
  }
}

void gl_mouse_motion(int x, int y) {
  int dx, dy;

  if (gl_camera_state == GL_CAMERA_ROTATING) {
    gl_camera_pan -= (x-gl_last_mouse_x)*GL_ROTATE_SENSITIVITY;
    gl_camera_tilt += (y-gl_last_mouse_y)*GL_ROTATE_SENSITIVITY;
  }
  else if (gl_camera_state == GL_CAMERA_MOVING) {
    dx = x-gl_last_mouse_x;
    dy = y-gl_last_mouse_y;
    
    gl_camera_x_offset += -dy*cos(gl_camera_pan*M_PI/180.0)*
      GL_MOVE_SENSITIVITY*gl_camera_distance;
    gl_camera_y_offset += -dy*sin(gl_camera_pan*M_PI/180.0)*
      GL_MOVE_SENSITIVITY*gl_camera_distance;
    gl_camera_x_offset += dx*cos((gl_camera_pan-90.0)*M_PI/180.0)*
      GL_MOVE_SENSITIVITY*gl_camera_distance;
    gl_camera_y_offset += dx*sin((gl_camera_pan-90.0)*M_PI/180.0)*
      GL_MOVE_SENSITIVITY*gl_camera_distance;
  }
  else if (gl_camera_state == GL_CAMERA_ZOOMING)
    gl_camera_distance -= (y-gl_last_mouse_y)*GL_ZOOM_SENSITIVITY;

  if (gl_camera_tilt < 0)
    gl_camera_tilt = 0;
  else if (gl_camera_tilt > 89.0)
    gl_camera_tilt = 89.0;
  if (gl_camera_distance < GL_MIN_RANGE)
    gl_camera_distance = GL_MIN_RANGE;
  if (gl_camera_z_offset < GL_MIN_Z)
    gl_camera_z_offset = GL_MIN_Z;
  
  gl_last_mouse_x = x;
  gl_last_mouse_y = y;
}

void gl_draw_cursor() {
  if (gl_cursor_size > 0.0) {
    glPushMatrix();
    glTranslatef(gl_camera_x_offset, gl_camera_y_offset, gl_camera_z_offset);

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-gl_cursor_size, 0.0, 0.0);
    glVertex3f(gl_cursor_size, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, gl_cursor_size, 0.0);
    glVertex3f(0.0, -gl_cursor_size, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, gl_cursor_size);
    glVertex3f(0.0, 0.0, -gl_cursor_size);
    glEnd();

    glPopMatrix();
  }
}

void gl_display() {
  float cpan, ctilt;
  float camera_x, camera_y, camera_z;
  float camera_x_dir, camera_y_dir, camera_z_dir;
  float camera_x_up, camera_y_up, camera_z_up;
  float r_x, r_y, r_z, r_len;
  float left, right, top, bottom, near, far;
  float max_diff, eye_distance;
  float ratio, radians, wd2, ndfl;
  
  near = gl_near;
  far = gl_far;
  ratio = gl_window_width/(float)gl_window_height;
  radians = 0.5*gl_camera_aperture*M_PI/180.0;
  wd2 = near*tan(radians);
  ndfl = near/gl_camera_distance;
    
  camera_x_up = 0.0;
  camera_y_up = 0.0;
  camera_z_up = 1.0;

  cpan = gl_camera_pan*M_PI/180.0;
  ctilt = gl_camera_tilt*M_PI/180.0;

  camera_x = gl_camera_distance*cos(cpan)*cos(ctilt)+gl_camera_x_offset;
  camera_y = gl_camera_distance*sin(cpan)*cos(ctilt)+gl_camera_y_offset;
  camera_z = gl_camera_distance*sin(ctilt)+gl_camera_z_offset;
  camera_x_dir = gl_camera_x_offset-camera_x;
  camera_y_dir = gl_camera_y_offset-camera_y;
  camera_z_dir = gl_camera_z_offset-camera_z;
  eye_distance = gl_camera_distance/gl_camera_eye_ratio;

  fprintf(stderr, "\rx = %8.2f  y = %8.2f  z = %8.2f",
    camera_x, camera_y, camera_z);

  glClearColor(gl_clear_red, gl_clear_green, gl_clear_blue, 0.0);
    
  if (gl_stereo) {
    r_x = camera_y_dir*camera_z_up-camera_z_dir*camera_y_up;
    r_y = camera_z_dir*camera_x_up-camera_x_dir*camera_z_up;
    r_z = camera_x_dir*camera_y_up-camera_y_dir*camera_x_up;
    r_len = sqrt(r_x*r_x+r_y*r_y+r_z*r_z);
    r_x = r_x/r_len*0.5*eye_distance;
    r_y = r_y/r_len*0.5*eye_distance;
    r_z = r_z/r_len*0.5*eye_distance;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left = -ratio*wd2-0.5*eye_distance*ndfl;
    right = ratio*wd2-0.5*eye_distance*ndfl;
    top = wd2;
    bottom = -wd2;
    glFrustum(left, right, bottom, top, near, far);

    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK_RIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
      camera_x+r_x,
      camera_y+r_y,
      camera_z+r_z,
      gl_camera_x_offset+r_x,
      gl_camera_y_offset+r_y,
      gl_camera_z_offset+r_z,
      camera_x_up,
      camera_y_up,
      camera_z_up);

    gl_draw_scene();
    gl_draw_cursor();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left = -ratio*wd2+0.5*eye_distance*ndfl;
    right = ratio*wd2+0.5*eye_distance*ndfl;
    top = wd2;
    bottom = -wd2;
    glFrustum(left, right, bottom, top, near, far);

    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
      camera_x-r_x,
      camera_y-r_y,
      camera_z-r_z,
      gl_camera_x_offset-r_x,
      gl_camera_y_offset-r_y,
      gl_camera_z_offset-r_z,
      camera_x_up,
      camera_y_up,
      camera_z_up);

    gl_draw_scene();
    gl_draw_cursor();
  }
  else {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left = -ratio*wd2;
    right = ratio*wd2;
    top = wd2;
    bottom = -wd2;
    glFrustum(left, right, bottom, top, near, far);

    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
      camera_x,
      camera_y,
      camera_z,
      gl_camera_x_offset,
      gl_camera_y_offset,
      gl_camera_z_offset,
      camera_x_up,
      camera_y_up,
      camera_z_up);

    gl_draw_scene();
    gl_draw_cursor();
  }

  glutSwapBuffers();
}

void gl_idle() {
  static double last_update = 0;
  double current_time;

  current_time = carmen_get_time();
  
  if (current_time-last_update > 1.0/GL_MAX_FPS) {
    glutPostRedisplay();
    last_update = current_time;
  }
}

void gl_reshape(int width, int height) {
  gl_window_width = width;
  gl_window_height = height;

  glViewport(0, 0, width, height);
}

void gl_finalize() {
  fprintf(stderr, "\n");
}

void gl_initialize(int argc, char **argv) {
  int i;
  
  glutInit(&argc, argv);
  atexit(gl_finalize);

  for (i = 1; i < argc; ++i)
    if (!strcmp(argv[1], "-stereo"))
      gl_stereo = 1;
  
  if (gl_stereo)
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STEREO);
  else
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  
  glutInitWindowSize(gl_window_width, gl_window_height);
  glutInitWindowPosition(10, 10);
  
  glutCreateWindow(argv[0]);
  glutReshapeFunc(gl_reshape);
  glutKeyboardFunc(gl_keyboard);
  glutDisplayFunc(gl_display);
  glutMouseFunc(gl_mouse);
  glutMotionFunc(gl_mouse_motion);
  glutIdleFunc(gl_idle);

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
}

void gl_clear_color(float red, float green, float blue) {
  gl_clear_red = red;
  gl_clear_green = green;
  gl_clear_blue = blue;

  glutPostRedisplay();
}

void gl_clip(float near, float far) {
  gl_near = near;
  gl_far = far;

  glutPostRedisplay();
}

void gl_cursor(float size) {
  gl_cursor_size = size;
  
  glutPostRedisplay();
}

void gl_camera(float x, float y, float z, float pan, float tilt,
    float distance) {
  gl_camera_z_offset = x;
  gl_camera_x_offset = y;
  gl_camera_y_offset = z;
  gl_camera_pan = pan;
  gl_camera_tilt = tilt;
  gl_camera_distance = distance;

  glutPostRedisplay();
}

void gl_loop() {
  glutMainLoop();
}
