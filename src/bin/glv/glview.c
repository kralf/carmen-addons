#include <signal.h>

#include "gl.h"
#include "glv.h"

#define MAX(a,b) (((a)<(b))?(b):(a))

int glv_num_objects = 0;
glv_object_p *glv_objects = 0;

int glv_list = -1;

void glv_shutdown_module(int x) {
  if (x == SIGINT) {
    exit(0);
  }
}

int glv_same_color(glv_color_t c1, glv_color_t c2) {
  if (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b)
    return 1;
  else
    return 0;
}

void glv_lighting() {
  float light_ambient[] = { 0, 0, 0, 0 };
  float light_diffuse[] = { 1, 1, 1, 1 };
  float light_specular[] = { 0, 0, 0, 0 };
  float light_position[] = { 0, 0, 100, 0 };

  glDisable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
}

void gl_draw_scene() {
  int i, n;
  glv_color_t current_color;

  glv_lighting();
  
  current_color.r = 255;
  current_color.g = 255;
  current_color.b = 255;
  glColor3f(current_color.r/255.0, current_color.g/255.0,
    current_color.b/255.0);

  glPushMatrix();
  glTranslatef(-glv_objects[0]->centroid.x, -glv_objects[0]->centroid.y, 0);
  
  for (n = 0; n < glv_num_objects; n++) {
    glBegin(GL_POINTS);
    
    for (i = 0; i < glv_objects[n]->num_points; i++) {
      if (!glv_same_color(glv_objects[n]->point[i].c, current_color)) {
        current_color = glv_objects[n]->point[i].c;
        glColor3f(current_color.r/255.0, current_color.g/255.0,
          current_color.b/255.0);
      }
      glVertex3f(glv_objects[n]->point[i].x, glv_objects[n]->point[i].y,
        glv_objects[n]->point[i].z);
    }
    glEnd();

    glBegin(GL_LINES);
    for (i = 0; i < glv_objects[n]->num_lines; i++) {
      if (!glv_same_color(glv_objects[n]->line[i].c, current_color)) {
        current_color = glv_objects[n]->line[i].c;
        glColor3f(current_color.r/255.0, current_color.g/255.0,
          current_color.b/255.0);
      }
      glVertex3f(glv_objects[n]->line[i].p1.x, glv_objects[n]->line[i].p1.y,
        glv_objects[n]->line[i].p1.z);
      glVertex3f(glv_objects[n]->line[i].p2.x, glv_objects[n]->line[i].p2.y,
        glv_objects[n]->line[i].p2.z);
    }
    glEnd();

    if (glv_objects[n]->num_faces > 0) {
      glEnable(GL_LIGHTING);
      glBegin(GL_TRIANGLES);
      for(i = 0; i < glv_objects[n]->num_faces; i++) {
        if(!glv_same_color(glv_objects[n]->face[i].c, current_color)) {
          current_color = glv_objects[n]->face[i].c;
          glColor3f(current_color.r / 255.0, current_color.g / 255.0,
              current_color.b / 255.0);
        }
        glNormal3f(glv_objects[n]->face[i].normal.x,
          glv_objects[n]->face[i].normal.y, glv_objects[n]->face[i].normal.z);
        glVertex3f(glv_objects[n]->face[i].p1.x, glv_objects[n]->face[i].p1.y,
          glv_objects[n]->face[i].p1.z);
        glVertex3f(glv_objects[n]->face[i].p2.x, glv_objects[n]->face[i].p2.y,
          glv_objects[n]->face[i].p2.z);
        glVertex3f(glv_objects[n]->face[i].p3.x, glv_objects[n]->face[i].p3.y,
          glv_objects[n]->face[i].p3.z);
      }
      glEnd();
      glDisable(GL_LIGHTING);
    }
  }

  glPopMatrix();
}

int main(int argc, char **argv) {
  int i;

  if(argc < 2)
    carmen_die("Error: missing arguments\n"
	    "Usage: %s [OPTIONS] FILE1 [FILE2 [...]]\n", argv[0]);
  
  for(i = 1; i < argc; ++i)
    if (argv[i][0] != '-') {
    glv_objects = (glv_object_p*)realloc(glv_objects,
      (glv_num_objects+1)*sizeof(glv_object_t));
    glv_objects[glv_num_objects] = glv_object_read(argv[i]);
    ++glv_num_objects;
  }
  carmen_test_alloc(glv_objects);

  signal(SIGINT, glv_shutdown_module);

  gl_initialize(argc, argv);
  
  gl_clear_color(0.2, 0.2, 0.2);
  gl_clip(0.1, MAX(glv_objects[0]->max.x-glv_objects[0]->min.x,
    -glv_objects[0]->max.y-glv_objects[0]->min.y)*10.0);
  gl_camera(0.0, 0.0, 0.0, 0.0, 89.0,
    (glv_objects[0]->max.x-glv_objects[0]->min.x)*5.0);

  gl_loop();

  return 0;
}
