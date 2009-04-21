#include "glv.h"

void write_color_glv(my_FILE *fp, unsigned char r, unsigned char g, 
		     unsigned char b)
{
  unsigned char message[10];

  message[0] = MESSAGE_ID_COLOR;
  message[1] = r;
  message[2] = g;
  message[3] = b;
  my_fwrite(message, 4, 1, fp);
}

void write_point_glv(my_FILE *fp, float x, float y, float z)
{
  char message[20];
  float *fmessage;

  message[0] = MESSAGE_ID_POINT;
  fmessage = (float *)(message + 1);
  fmessage[0] = x;
  fmessage[1] = y;
  fmessage[2] = z;
  my_fwrite(message, 13, 1, fp);
}

void write_line_glv(my_FILE *fp, float x1, float y1, float z1,
		    float x2, float y2, float z2)
{
  char message[30];
  float *fmessage;

  message[0] = MESSAGE_ID_LINE;
  fmessage = (float *)(message + 1);
  fmessage[0] = x1;
  fmessage[1] = y1;
  fmessage[2] = z1;
  fmessage[3] = x2;
  fmessage[4] = y2;
  fmessage[5] = z2;
  my_fwrite(message, 25, 1, fp);
}

void write_face_glv(my_FILE *fp, float x1, float y1, float z1,
		    float x2, float y2, float z2,
		    float x3, float y3, float z3)
{
  char message[40];
  float *fmessage;

  message[0] = MESSAGE_ID_FACE;
  fmessage = (float *)(message + 1);
  fmessage[0] = x1;
  fmessage[1] = y1;
  fmessage[2] = z1;
  fmessage[3] = x2;
  fmessage[4] = y2;
  fmessage[5] = z2;
  fmessage[6] = x3;
  fmessage[7] = y3;
  fmessage[8] = z3;
  my_fwrite(message, 37, 1, fp);
}

glv_object_p glv_object_init(void)
{
  glv_object_p obj;

  obj = (glv_object_p)calloc(1, sizeof(glv_object_t));
  carmen_test_alloc(obj);
  obj->num_points = 0;
  obj->max_points = 100000;
  obj->point = (glv_point_p)calloc(obj->max_points, sizeof(glv_point_t));
  carmen_test_alloc(obj->point);
  obj->num_lines = 0;
  obj->max_lines = 100000;
  obj->line = (glv_line_p)calloc(obj->max_lines, sizeof(glv_line_t));
  carmen_test_alloc(obj->line);
  obj->num_faces = 0;
  obj->max_faces = 100000;
  obj->face = (glv_face_p)calloc(obj->max_faces, sizeof(glv_face_t));
  carmen_test_alloc(obj->face);
  return obj;
}

void adjust_extrema(glv_point_t p, float *min_x, float *max_x, float *min_y,
		    float *max_y, float *min_z, float *max_z)
{
  if(p.x < *min_x)
    *min_x = p.x;
  else if(p.x > *max_x)
    *max_x = p.x;
  if(p.y < *min_y)
    *min_y = p.y;
  else if(p.y > *max_y)
    *max_y = p.y;
  if(p.z < *min_z)
    *min_z = p.z;
  else if(p.z > *max_z)
    *max_z = p.z;
  
}

void compute_normal(glv_face_p face)
{
  glv_point_t centroid, v1, v2;
  float len;

  centroid.x = (face->p1.x + face->p2.x + face->p3.x) / 3.0;
  centroid.y = (face->p1.y + face->p2.y + face->p3.y) / 3.0;
  centroid.z = (face->p1.z + face->p2.z + face->p3.z) / 3.0;

  v1.x = face->p2.x - face->p1.x;
  v1.y = face->p2.y - face->p1.y;
  v1.z = face->p2.z - face->p1.z;
  v2.x = face->p3.x - face->p1.x;
  v2.y = face->p3.y - face->p1.y;
  v2.z = face->p3.z - face->p1.z;
  face->normal.x = v1.y * v2.z - v1.z * v2.y;
  face->normal.y = - v1.x * v2.z + v1.z * v2.x;
  face->normal.z = v1.x * v2.y - v1.y * v2.x;
  len = sqrt(face->normal.x * face->normal.x + face->normal.y * 
	     face->normal.y + face->normal.z * face->normal.z);
  if(len != 0) {
    face->normal.x /= len;
    face->normal.y /= len;
    face->normal.z /= len;
  }
}

glv_object_p glv_object_read(char *filename)
{
  my_FILE *fp;
  glv_color_t current_color;
  char buffer[10000];
  long int nread, log_bytes = 0;
  int buffer_pos, buffer_length, offset = 0, n;
  glv_object_p obj;
  float *fmessage;
  int done_reading = 0;
  int i, line_count = 0;
  float min_x, max_x, min_y, max_y, min_z, max_z;

  if(strncmp(filename + strlen(filename) - 4, ".glv", 4) &&
     strncmp(filename + strlen(filename) - 7, ".glv.gz", 7) &&
     strncmp(filename + strlen(filename) - 5, ".pmap", 5) &&
     strncmp(filename + strlen(filename) - 8, ".pmap.gz", 8))
    carmen_die("Error: file name must end in .glv, .glv.gz, .pmap, or .pmap.gz\n");
  fp = my_fopen(filename, "r");

 /* compute total number of bytes in logfile */
  do {
    nread = my_fread(buffer, 1, 10000, fp);
    log_bytes += nread;
  } while(nread > 0);
  my_fseek(fp, 0L, SEEK_SET);

  current_color.r = 255;
  current_color.g = 255;
  current_color.b = 255;
  
  obj = glv_object_init();

  buffer_pos = 0;
  buffer_length = my_fread(buffer, 1, 10000, fp);

  while(!done_reading || buffer_length > buffer_pos) {
    line_count++;
    if(line_count % 100000 == 0)
      fprintf(stderr, "\rReading glv file... (%.0f%%)  ", 
	      (offset + buffer_pos) / (float)log_bytes * 100.0);
 
    if(buffer_length - buffer_pos < 50 && !done_reading) {
      memmove(buffer, buffer + buffer_pos, buffer_length - buffer_pos);
      buffer_length -= buffer_pos;
      offset += buffer_pos;
      buffer_pos = 0;
      n = my_fread(buffer + buffer_length, 
                   1, 10000 - buffer_length - 1, fp);
      if(n == 0)
	done_reading = 1;
      else
	buffer_length += n;
    }
    else {
      if(buffer[buffer_pos] == MESSAGE_ID_COLOR) {
	current_color.r = (unsigned char)buffer[buffer_pos + 1];
	current_color.g = (unsigned char)buffer[buffer_pos + 2];
	current_color.b = (unsigned char)buffer[buffer_pos + 3];
	buffer_pos += 4;
      }
      else if(buffer[buffer_pos] == MESSAGE_ID_POINT) {
	if(obj->num_points == obj->max_points) {
	  obj->max_points += 100000;
	  obj->point = (glv_point_p)realloc(obj->point, obj->max_points *
					       sizeof(glv_point_t));
	  carmen_test_alloc(obj->point);
	}
	fmessage = (float *)(buffer + buffer_pos + 1);
	obj->point[obj->num_points].x = fmessage[0];
	obj->point[obj->num_points].y = fmessage[1];
	obj->point[obj->num_points].z = fmessage[2];
	obj->point[obj->num_points].c = current_color;
	obj->num_points++;
	buffer_pos += 13;
      }
      else if(buffer[buffer_pos] == MESSAGE_ID_LINE) {
	if(obj->num_lines == obj->max_lines) {
	  obj->max_lines += 100000;
	  obj->line = (glv_line_p)realloc(obj->line, obj->max_lines *
					     sizeof(glv_line_t));
	  carmen_test_alloc(obj->line);
	}
	fmessage = (float *)(buffer + buffer_pos + 1);
	obj->line[obj->num_lines].p1.x = fmessage[0];
	obj->line[obj->num_lines].p1.y = fmessage[1];
	obj->line[obj->num_lines].p1.z = fmessage[2];
	obj->line[obj->num_lines].p2.x = fmessage[3];
	obj->line[obj->num_lines].p2.y = fmessage[4];
	obj->line[obj->num_lines].p2.z = fmessage[5];
	obj->line[obj->num_lines].c = current_color;
	obj->num_lines++;
	buffer_pos += 25;
      }
      else if(buffer[buffer_pos] == MESSAGE_ID_FACE) {
	if(obj->num_faces == obj->max_faces) {
	  obj->max_faces += 100000;
	  obj->face = (glv_face_p)realloc(obj->face, obj->max_faces *
					     sizeof(glv_face_t));
	  carmen_test_alloc(obj->face);
	}
	fmessage = (float *)(buffer + buffer_pos + 1);
	obj->face[obj->num_faces].p1.x = fmessage[0];
	obj->face[obj->num_faces].p1.y = fmessage[1];
	obj->face[obj->num_faces].p1.z = fmessage[2];
	obj->face[obj->num_faces].p2.x = fmessage[3];
	obj->face[obj->num_faces].p2.y = fmessage[4];
	obj->face[obj->num_faces].p2.z = fmessage[5];
	obj->face[obj->num_faces].p3.x = fmessage[6];
	obj->face[obj->num_faces].p3.y = fmessage[7];
	obj->face[obj->num_faces].p3.z = fmessage[8];
	obj->face[obj->num_faces].c = current_color;
	compute_normal(&obj->face[obj->num_faces]);
	obj->num_faces++;
	buffer_pos += 37;
      }
    }
  }

  min_x = 1e10;
  max_x = -1e10;
  min_y = 1e10;
  max_y = -1e10;
  min_z = 1e10;
  max_z = -1e10;
  for(i = 0; i < obj->num_points; i++)
    adjust_extrema(obj->point[i], &min_x, &max_x, &min_y, &max_y,
		   &min_z, &max_z);
  for(i = 0; i < obj->num_lines; i++) {
    adjust_extrema(obj->line[i].p1, &min_x, &max_x, &min_y, &max_y,
		   &min_z, &max_z); 
    adjust_extrema(obj->line[i].p2, &min_x, &max_x, &min_y, &max_y,
		   &min_z, &max_z); 
  }
  for(i = 0; i < obj->num_faces; i++) {
    adjust_extrema(obj->face[i].p1, &min_x, &max_x, &min_y, &max_y,
		   &min_z, &max_z); 
    adjust_extrema(obj->face[i].p2, &min_x, &max_x, &min_y, &max_y,
		   &min_z, &max_z); 
    adjust_extrema(obj->face[i].p3, &min_x, &max_x, &min_y, &max_y,
		   &min_z, &max_z); 
  }
  obj->centroid.x = (min_x + max_x) / 2;
  obj->centroid.y = (min_y + max_y) / 2;
  obj->centroid.z = (min_z + max_z) / 2;
  obj->min.x = min_x;
  obj->min.y = min_y;
  obj->min.z = min_z;
  obj->max.x = max_x;
  obj->max.y = max_y;
  obj->max.z = max_z;

  my_fclose(fp);
  fprintf(stderr, "\rReading glv file... (100%%)   \n");
  fprintf(stderr, "%d POINTS - %d LINES - %d FACES\n", obj->num_points,
	  obj->num_lines, obj->num_faces);
  return obj;
}
