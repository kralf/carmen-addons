#include "glv.h"
#include "vrmlout.h"

glv_object_p obj;
glv_color_p color_list;
int num_colors = 0, max_colors = 10;

inline int same_color(glv_color_t c1, glv_color_t c2)
{
  if(c1.r == c2.r &&
     c1.g == c2.g &&
     c1.b == c2.b)
    return 1;
  return 0;
}

int main(int argc, char **argv)
{
  int i, j, found, vrml_obj;
  char filename[200], vrml_filename[200];
  vrml_file_t vrml_file;

  if(argc < 2)
    carmen_die("Error: missing arguments\n"
	    "Usage: %s filename\n", argv[0]);
  strcpy(filename, argv[1]);
  strcpy(vrml_filename, filename);
  if(strcmp(filename + strlen(filename) - 7, ".glv.gz") == 0)
    strcpy(vrml_filename + strlen(vrml_filename) - 6, "vrml.gz");
  else if(strcmp(filename + strlen(filename) - 4, ".glv") == 0)
    strcpy(vrml_filename + strlen(vrml_filename) - 3, "vrml.gz");
  else
    carmen_die("Error: file must end in .glv or .glv.gz\n");

  /* open the output file */
  vrml_file_open(&vrml_file, vrml_filename);

  /* read the glv model */
  obj = glv_object_read(argv[1]);

  /* allocate the color list */
  color_list = (glv_color_p)calloc(max_colors, sizeof(glv_color_t));
  carmen_test_alloc(color_list);

  /* make a list of the point colors */
  for(i = 0; i < obj->num_points; i++) {
    found = 0;
    for(j = 0; j < num_colors; j++)
      if(same_color(obj->point[i].c, color_list[j]))
	found = 1;
    if(!found) {
      if(num_colors == max_colors) {
	max_colors += 10;
	color_list = (glv_color_p)realloc(color_list,
					  max_colors * sizeof(glv_color_t));
      }
      color_list[num_colors] = obj->point[i].c;
      num_colors++;
    }
  }

  /* write the point list objects to the vrml file */
  fprintf(stderr, "Converting points (%d colors) : ", num_colors);
  for(i = 0; i < num_colors; i++) {
    vrml_obj = vrml_file_new_object(&vrml_file, VRML_POINTSET,
				    color_list[i].r / 255.0,
				    color_list[i].g / 255.0,
				    color_list[i].b / 255.0);
    for(j = 0; j < obj->num_points; j++)
      if(obj->point[j].c.r == color_list[i].r &&
	 obj->point[j].c.g == color_list[i].g &&
	 obj->point[j].c.b == color_list[i].b)
	vrml_add_point(&vrml_file, vrml_obj, obj->point[j].x,
		       obj->point[j].y, obj->point[j].z);
    vrml_file_close_object(&vrml_file, vrml_obj);
    fprintf(stderr, "%d ", i + 1);
  }
  fprintf(stderr, "\n");

  /* start over with the colors */
  num_colors = 0;

  /* make a list of the line colors */
  for(i = 0; i < obj->num_lines; i++) {
    found = 0;
    for(j = 0; j < num_colors; j++)
      if(same_color(obj->line[i].c, color_list[j]))
	found = 1;
    if(!found) {
      if(num_colors == max_colors) {
	max_colors += 10;
	color_list = (glv_color_p)realloc(color_list,
					  max_colors * sizeof(glv_color_t));
      }
      color_list[num_colors] = obj->line[i].c;
      num_colors++;
    }
  }

  /* write the line list objects to the vrml file */
  fprintf(stderr, "Converting lines (%d colors) : ", num_colors);
  for(i = 0; i < num_colors; i++) {
    vrml_obj = vrml_file_new_object(&vrml_file, VRML_LINESET,
				    color_list[i].r / 255.0,
				    color_list[i].g / 255.0,
				    color_list[i].b / 255.0);
    for(j = 0; j < obj->num_lines; j++)
      if(obj->line[j].c.r == color_list[i].r &&
	 obj->line[j].c.g == color_list[i].g &&
	 obj->line[j].c.b == color_list[i].b)
	vrml_add_line(&vrml_file, vrml_obj,
		      obj->line[j].p1.x,
		      obj->line[j].p1.y,
		      obj->line[j].p1.z,
		      obj->line[j].p2.x,
		      obj->line[j].p2.y,
		      obj->line[j].p2.z);
    vrml_file_close_object(&vrml_file, vrml_obj);
    fprintf(stderr, "%d ", i + 1);
  }
  fprintf(stderr, "\n");

  /* start over with the colors */
  num_colors = 0;

  /* make a list of the line colors */
  for(i = 0; i < obj->num_faces; i++) {
    found = 0;
    for(j = 0; j < num_colors; j++)
      if(same_color(obj->face[i].c, color_list[j]))
	found = 1;
    if(!found) {
      if(num_colors == max_colors) {
	max_colors += 10;
	color_list = (glv_color_p)realloc(color_list,
					  max_colors * sizeof(glv_color_t));
      }
      color_list[num_colors] = obj->face[i].c;
      num_colors++;
    }
  }

  /* write the line list objects to the vrml file */
  fprintf(stderr, "Converting faces (%d colors) : ", num_colors);
  for(i = 0; i < num_colors; i++) {
    if(color_list[i].r == 255 && color_list[i].g == 255 &&
       color_list[i].b == 255)
      vrml_obj = vrml_file_new_object(&vrml_file, VRML_FACESET,
				      -1, -1, -1);
    else
      vrml_obj = vrml_file_new_object(&vrml_file, VRML_FACESET,
				      color_list[i].r / 255.0,
				      color_list[i].g / 255.0,
				      color_list[i].b / 255.0);
    for(j = 0; j < obj->num_faces; j++)
      if(obj->face[j].c.r == color_list[i].r &&
	 obj->face[j].c.g == color_list[i].g &&
	 obj->face[j].c.b == color_list[i].b) {
	vrml_add_face(&vrml_file, vrml_obj,
		      obj->face[j].p1.x,
		      obj->face[j].p1.y,
		      obj->face[j].p1.z,
		      obj->face[j].p2.x,
		      obj->face[j].p2.y,
		      obj->face[j].p2.z,
		      obj->face[j].p3.x,
		      obj->face[j].p3.y,
		      obj->face[j].p3.z);
      }
    vrml_file_close_object(&vrml_file, vrml_obj);
    fprintf(stderr, "%d ", i + 1);
  }
  fprintf(stderr, "\n");

  vrml_file_close(&vrml_file);
  return 0;
}
