#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>

#include <deque>

#include <GL/glut.h>

#include <carmen/global.h>

#include "velodyne_interface.h"

#define GLUT_WHEEL_UP         3
#define GLUT_WHEEL_DOWN       4

#define CAMERA_IDLE           0
#define CAMERA_ROTATING       1
#define CAMERA_MOVING         2

#define ZOOM_SENSITIVITY      1.1
#define ROTATE_SENSITIVITY    0.50
#define MOVE_SENSITIVITY      0.001
#define MIN_RANGE             10.0
#define MAX_RANGE             120.0
#define MIN_Z                 -5.0

#define MAX_POINTCLOUDS       1000

struct velodyne_pointcloud {
  int num_points;
  float* x;
  float* y;
  float* z;
};

int quit = 0;
int id = 1;

int window_width = 400;
int window_height = 300;

int camera_state = CAMERA_IDLE;
float camera_pan = 0.0;
float camera_tilt = 89.0;
float camera_distance = MIN_RANGE;
float camera_z_offset = 0.0;
float camera_x_offset = 0.0;
float camera_y_offset = 0.0;

int last_mouse_x;
int last_mouse_y;

std::deque<velodyne_pointcloud> pointclouds;
std::deque<GLuint> lists;

pthread_t thread;

#define MAX(a,b) (((a)<(b))?(b):(a))

void velodyne_sigint_handler(int q) {
  if(q == SIGINT) {
    quit = 1;
    exit(0);
  }
}

void velodyne_pointcloud_init(velodyne_pointcloud* pointcloud,
    int num_points) {
  pointcloud->num_points = num_points;
  pointcloud->x = (float*)malloc(num_points*sizeof(float));
  pointcloud->y = (float*)malloc(num_points*sizeof(float));
  pointcloud->z = (float*)malloc(num_points*sizeof(float));
}

void velodyne_pointcloud_free(velodyne_pointcloud* pointcloud) {
  free(pointcloud->x);
  free(pointcloud->y);
  free(pointcloud->z);
}

void velodyne_set_display_mode(int w, int h) {
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, w / (float)h, MIN_RANGE / 2.0, MAX_RANGE * 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void velodyne_reshape(int w, int h) {
  window_width = w;
  window_height = h;
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  velodyne_set_display_mode(w, h);
}

void velodyne_keyboard(unsigned char key, int x, int y) {
  switch(key) {
    case 27: case 'q': case 'Q':
      velodyne_sigint_handler(SIGINT);
      break;
    case 'a':
      camera_z_offset += 0.1;
      break;
    case 'z':
      camera_z_offset -= 0.1;
    default:
      break;
  }
}

void velodyne_display() {
  float cpan, ctilt, eye_x, eye_y, eye_z, camera_x, camera_y, camera_z;
  float line_len = MAX_RANGE * 0.05;
  int i, n;

  cpan = camera_pan * M_PI / 180.0;
  ctilt = camera_tilt * M_PI / 180.0;

  camera_x = camera_distance * cos(cpan) * cos(ctilt);
  camera_y = camera_distance * sin(cpan) * cos(ctilt);
  camera_z = camera_distance * sin(ctilt);
  eye_x = camera_x + camera_x_offset;
  eye_y = camera_y + camera_y_offset;
  eye_z = camera_z + camera_z_offset;

  glClearColor(1.0, 1.0, 1.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  velodyne_set_display_mode(window_width, window_height);

  gluLookAt(eye_x, eye_y, eye_z,
    camera_x_offset,
    camera_y_offset,
    camera_z_offset, 0, 0, 1);

  fprintf(stderr, "\rx = %8.2f  y = %8.2f  z = %8.2f",
    eye_x, eye_y, eye_z);

  while (!pointclouds.empty()) {
    velodyne_pointcloud pointcloud = pointclouds.front();
    pointclouds.pop_front();

    GLuint list_index = glGenLists(1);
    glNewList(list_index, GL_COMPILE);
    glBegin(GL_POINTS);
    for (int i = 0; i < pointcloud.num_points; ++i)
      glVertex3f(pointcloud.x[i], pointcloud.y[i], pointcloud.z[i]);
    glEnd();
    glEndList();
    
    lists.push_back(list_index);
    velodyne_pointcloud_free(&pointcloud);

    if (lists.size() > MAX_POINTCLOUDS) {
      glDeleteLists(lists.front(), 1);
      lists.pop_front();
    }
  }

  glColor3f(0, 0, 0);
  for (std::deque<GLuint>::const_iterator it = lists.begin();
      it != lists.end(); ++it)
    glCallList(*it);
    
  glPushMatrix();
  glTranslatef(camera_x_offset, camera_y_offset, 0);
  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(-line_len, 0, 0);
  glVertex3f(line_len, 0, 0);
  glColor3f(0, 1, 0);
  glVertex3f(0, line_len, 0);
  glVertex3f(0, -line_len, 0);
  glColor3f(0, 0, 1);
  glVertex3f(0, 0, line_len);
  glVertex3f(0, 0, -line_len);
  glEnd();
  glPopMatrix();

  glutSwapBuffers();
}

void velodyne_mouse(int button, int state, int x, int y) {
  if(state == GLUT_DOWN) {
    last_mouse_x = x;
    last_mouse_y = y;
    
    if(button == GLUT_LEFT_BUTTON)
      camera_state = CAMERA_ROTATING;
    else if(button == GLUT_RIGHT_BUTTON)
      camera_state = CAMERA_MOVING;
    else
      camera_state = CAMERA_IDLE;
  }
  else if(state == GLUT_UP) {
    if(button == GLUT_WHEEL_UP)
      camera_distance /= ZOOM_SENSITIVITY;
    else if(button == GLUT_WHEEL_DOWN)
      camera_distance *= ZOOM_SENSITIVITY;
    else
      camera_state = CAMERA_IDLE;
  }
}

void velodyne_motion(int x, int y) {
  int dx, dy;

  if(camera_state == CAMERA_ROTATING) {
    camera_pan -= (x - last_mouse_x) * ROTATE_SENSITIVITY;
    camera_tilt += (y - last_mouse_y) * ROTATE_SENSITIVITY;
  }
  else if(camera_state == CAMERA_MOVING) {
    dx = x - last_mouse_x;
    dy = y - last_mouse_y;
    camera_x_offset += -dy * cos(camera_pan * M_PI / 180.0) *
      MOVE_SENSITIVITY * camera_distance;
    camera_y_offset += -dy * sin(camera_pan * M_PI / 180.0) *
      MOVE_SENSITIVITY * camera_distance;
    camera_x_offset += dx * cos((camera_pan - 90.0) * M_PI / 180.0) *
      MOVE_SENSITIVITY * camera_distance;
    camera_y_offset += dx * sin((camera_pan - 90.0) * M_PI / 180.0) *
      MOVE_SENSITIVITY * camera_distance;
  }

  if(camera_tilt < 0)
    camera_tilt = 0;
  else if(camera_tilt > 89.0)
    camera_tilt = 89.0;
  if(camera_distance < MIN_RANGE)
    camera_distance = MIN_RANGE;
  if(camera_z_offset < MIN_Z)
    camera_z_offset = MIN_Z;
  last_mouse_x = x;
  last_mouse_y = y;
}

void velodyne_idle() {
  glutPostRedisplay();
}

void velodyne_pointcloud_handler(carmen_velodyne_pointcloud_message*
    message) {
  velodyne_pointcloud pointcloud;
  velodyne_pointcloud_init(&pointcloud, message->num_points);
  
  memcpy(pointcloud.x, message->x, pointcloud.num_points*sizeof(float));
  memcpy(pointcloud.y, message->y, pointcloud.num_points*sizeof(float));
  memcpy(pointcloud.z, message->z, pointcloud.num_points*sizeof(float));

  pointclouds.push_back(pointcloud);
}

void velodyne_initialize(int argc, char **argv) {
  float light_ambient[] = { 0, 0, 0, 0 };
  float light_diffuse[] = { 1, 1, 1, 1 };
  float light_specular[] = { 0, 0, 0, 0 };
  float light_position[] = { 0, 0, 100, 0 };

  /* initialize glut */
  glutInit(&argc, argv);

  /* setup the window */
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(window_width, window_height);
  glutInitWindowPosition(10, 10);
  glutCreateWindow(argv[0]);
  glutReshapeFunc(velodyne_reshape);
  glutKeyboardFunc(velodyne_keyboard);
  glutDisplayFunc(velodyne_display);
  glutMouseFunc(velodyne_mouse);
  glutMotionFunc(velodyne_motion);
  glutIdleFunc(velodyne_idle);

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  glDisable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  glEnable(GL_COLOR_MATERIAL);
}

void velodyne_finalize() {
  quit = 1;

  pthread_cancel(thread);
  pthread_join(thread, 0);
  
  fprintf(stderr, "\n");
}

void* velodyne_listen(void* p) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);

  while (!quit) {
    carmen_ipc_sleep(0.0);
    pthread_testcancel();
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc == 2)
    id = atoi(argv[1]);

  carmen_ipc_initialize(argc, argv);
  carmen_velodyne_subscribe_pointcloud_message(NULL,
    (carmen_handler_t)velodyne_pointcloud_handler, CARMEN_SUBSCRIBE_ALL);

  camera_distance = MAX_RANGE * 0.5;

  velodyne_initialize(argc, argv);
  signal(SIGINT, velodyne_sigint_handler);

  pthread_create(&thread, 0, velodyne_listen, 0);
  
  atexit(velodyne_finalize);
  glutMainLoop();

  while (!pointclouds.empty()) {
    velodyne_pointcloud_free(&pointclouds.front());
    pointclouds.pop_front();
  }
  
  return 0;
}
