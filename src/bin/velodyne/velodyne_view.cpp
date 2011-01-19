#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>

#include <deque>

#include <carmen/global.h>

#include "velodyne_interface.h"
#include "gl.h"

#define MAX_POINTCLOUDS       1000

struct velodyne_pointcloud {
  int num_points;
  float* x;
  float* y;
  float* z;
};

int quit = 0;
int id = 1;

std::deque<velodyne_pointcloud> pointclouds;
std::deque<GLuint> lists;

pthread_t thread;

void velodyne_sigint_handler(int q) {
  if (q == SIGINT) {
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

void velodyne_pointcloud_handler(carmen_velodyne_pointcloud_message*
    message) {
  velodyne_pointcloud pointcloud;
  velodyne_pointcloud_init(&pointcloud, message->num_points);
  
  memcpy(pointcloud.x, message->x, pointcloud.num_points*sizeof(float));
  memcpy(pointcloud.y, message->y, pointcloud.num_points*sizeof(float));
  memcpy(pointcloud.z, message->z, pointcloud.num_points*sizeof(float));

  pointclouds.push_back(pointcloud);
}

void velodyne_finalize() {
  quit = 1;

  pthread_cancel(thread);
  pthread_join(thread, 0);
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

void gl_draw_scene() {
  int i;

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
}

int main(int argc, char **argv) {
  int i;
  
  for(i = 1; i < argc; ++i)
    if (argv[i][0] != '-') {
    id = atoi(argv[i]);
    break;
  }

  carmen_ipc_initialize(argc, argv);
  carmen_velodyne_subscribe_pointcloud_message(NULL,
    (carmen_handler_t)velodyne_pointcloud_handler, CARMEN_SUBSCRIBE_ALL);

  signal(SIGINT, velodyne_sigint_handler);
  atexit(velodyne_finalize);

  pthread_create(&thread, 0, velodyne_listen, 0);

  gl_initialize(argc, argv);

  gl_clear_color(1.0, 1.0, 1.0);
  gl_clip(0.1, 60.0);
  gl_camera(0.0, 0.0, 0.0, 0.0, 89.0, 20.0);

  gl_loop();
  
  while (!pointclouds.empty()) {
    velodyne_pointcloud_free(&pointclouds.front());
    pointclouds.pop_front();
  }
  
  return 0;
}
