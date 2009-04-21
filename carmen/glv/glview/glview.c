#include <signal.h>
#include <GL/glut.h>

#include <carmen/glv.h>

#define WINDOW_WIDTH     400
#define WINDOW_HEIGHT    400

/* graphics stuff */

#define CAMERA_IDLE        0
#define CAMERA_ROTATING    1
#define CAMERA_MOVING      2
#define CAMERA_ZOOMING     3

#define ZOOM_SENSITIVITY      2
#define ROTATE_SENSITIVITY    0.50
#define MOVE_SENSITIVITY      0.001
#define MIN_RANGE             10.0
#define MIN_Z                 -5.0

int window_width = WINDOW_WIDTH, window_height = WINDOW_HEIGHT;
int camera_state = CAMERA_IDLE;
float camera_pan = 0, camera_tilt = 89.0, camera_distance = MIN_RANGE;
float camera_z_offset = 0.0;
int last_mouse_x, last_mouse_y;
float camera_x_offset = 0, camera_y_offset = 0;

int num_objects;
glv_object_p *obj;

#define MAX(a,b) (((a)<(b))?(b):(a))

void shutdown_module(int x)
{
  if(x == SIGINT) {
    exit(0);
  }
}

void set_display_mode_3D(int w, int h)
{
  double max_diff;

  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  max_diff = MAX(obj[0]->max.x - obj[0]->min.x, 
		 obj[0]->max.y - obj[0]->min.y) * 10;
  gluPerspective(30.0, w / (float)h, MIN_RANGE / 2.0, max_diff);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void reshape(int w, int h)
{
  window_width = w;
  window_height = h;
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  set_display_mode_3D(w, h);
}

void keyboard(unsigned char key, int x __attribute__ ((unused)), 
	      int y __attribute__ ((unused)))
{
  switch(key) {
  case 27: case 'q': case 'Q':
    shutdown_module(SIGINT);
    exit(0);
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

int same_color(glv_color_t c1, glv_color_t c2)
{
  if(c1.r == c2.r && c1.g == c2.g && c1.b == c2.b)
    return 1;
  else
    return 0;
}

void display(void)
{
  float cpan, ctilt, camera_x, camera_y, camera_z;
  glv_color_t current_color;
  int i, n;

  cpan = camera_pan * M_PI / 180.0;
  ctilt = camera_tilt * M_PI / 180.0;

  camera_x = camera_distance * cos(cpan) * cos(ctilt);
  camera_y = camera_distance * sin(cpan) * cos(ctilt);
  camera_z = camera_distance * sin(ctilt);

  glClearColor(0.2, 0.2, 0.2, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  set_display_mode_3D(window_width, window_height);

  gluLookAt(camera_x + camera_x_offset, 
	    camera_y + camera_y_offset,
	    camera_z + camera_z_offset, 
	    camera_x_offset, 
	    camera_y_offset,
	    camera_z_offset, 0, 0, 1);

  glPushMatrix();

  current_color.r = 255;
  current_color.g = 255;
  current_color.b = 255;
  glColor3f(current_color.r / 255.0, current_color.g / 255.0,
	    current_color.b / 255.0);

  glTranslatef(-obj[0]->centroid.x, -obj[0]->centroid.y, 0);

  for(n = 0; n < num_objects; n++) {
    glBegin(GL_POINTS);
    for(i = 0; i < obj[n]->num_points; i++) {
      if(!same_color(obj[n]->point[i].c, current_color)) {
	current_color = obj[n]->point[i].c;
	glColor3f(current_color.r / 255.0, current_color.g / 255.0,
		  current_color.b / 255.0);
      }
      glVertex3f(obj[n]->point[i].x, obj[n]->point[i].y, obj[n]->point[i].z);
    }
    glEnd();
    
    glBegin(GL_LINES);
    for(i = 0; i < obj[n]->num_lines; i++) {
      if(!same_color(obj[n]->line[i].c, current_color)) {
	current_color = obj[n]->line[i].c;
	glColor3f(current_color.r / 255.0, current_color.g / 255.0,
		  current_color.b / 255.0);
      }
      glVertex3f(obj[n]->line[i].p1.x, obj[n]->line[i].p1.y, 
		 obj[n]->line[i].p1.z);
      glVertex3f(obj[n]->line[i].p2.x, obj[n]->line[i].p2.y, 
		 obj[n]->line[i].p2.z);
    }
    glEnd();
    
    if(obj[n]->num_faces > 0) {
      glEnable(GL_LIGHTING);
      glBegin(GL_TRIANGLES);
      for(i = 0; i < obj[n]->num_faces; i++) {
	if(!same_color(obj[n]->face[i].c, current_color)) {
	  current_color = obj[n]->face[i].c;
	  glColor3f(current_color.r / 255.0, current_color.g / 255.0,
		    current_color.b / 255.0);
	}
	glNormal3f(obj[n]->face[i].normal.x, obj[n]->face[i].normal.y, 
		   obj[n]->face[i].normal.z);
	glVertex3f(obj[n]->face[i].p1.x, obj[n]->face[i].p1.y, 
		   obj[n]->face[i].p1.z);
	glVertex3f(obj[n]->face[i].p2.x, obj[n]->face[i].p2.y, 
		   obj[n]->face[i].p2.z);
	glVertex3f(obj[n]->face[i].p3.x, obj[n]->face[i].p3.y, 
		   obj[n]->face[i].p3.z);
      }
      glEnd();
      glDisable(GL_LIGHTING);
    }
  }
   
  glPopMatrix();

  glPushMatrix();
  glTranslatef(camera_x_offset, camera_y_offset, 0);
  glColor3f(0, 0, 1);
  glBegin(GL_LINES);
  glVertex3f(-1, 0, 0);
  glVertex3f(1, 0, 0);
  glVertex3f(0, 1, 0);
  glVertex3f(0, -1, 0);
  glEnd();
  glPopMatrix();

  glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
  if(state == GLUT_DOWN) {
    last_mouse_x = x;
    last_mouse_y = y;
    if(button == GLUT_LEFT_BUTTON)
      camera_state = CAMERA_ROTATING;
    else if(button == GLUT_MIDDLE_BUTTON)
      camera_state = CAMERA_MOVING;
    else if(button == GLUT_RIGHT_BUTTON)
      camera_state = CAMERA_ZOOMING;
  }
  else if(state == GLUT_UP)
    camera_state = CAMERA_IDLE;
}

void motion(int x, int y)
{
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
  else if(camera_state == CAMERA_ZOOMING)
    camera_distance -= (y - last_mouse_y) * ZOOM_SENSITIVITY;

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

void idle(void)
{
  static double last_update = 0;
  double current_time;

  current_time = carmen_get_time();
  if(current_time - last_update > 1.0 / 20.0) {
    glutPostRedisplay();
    last_update = current_time;
  }
}

void initialize(int argc, char **argv)
{
  float light_ambient[] = { 0, 0, 0, 0 };
  float light_diffuse[] = { 1, 1, 1, 1 };
  float light_specular[] = { 0, 0, 0, 0 };
  float light_position[] = { 0, 0, 100, 0 };

  /* initialize glut */
  glutInit(&argc, argv);

  /* setup the window */
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(10, 10);
  glutCreateWindow(argv[0]);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(idle);

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  glDisable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
}

int main(int argc, char **argv)
{
  int i;

  if(argc < 2)
    carmen_die("Error: missing arguments\n"
	    "Usage: %s filename\n", argv[0]);

  /* read all models from the command line */
  num_objects = argc - 1;
  obj = (glv_object_p *)calloc(num_objects, sizeof(glv_object_t));
  carmen_test_alloc(obj);
  for(i = 1; i < argc; i++)
    obj[i - 1] = glv_object_read(argv[i]);

  camera_distance = (obj[0]->max.x - obj[0]->min.x) * 5;

  initialize(argc, argv);
  signal(SIGINT, shutdown_module);
  glutMainLoop();
  return 0;
}
