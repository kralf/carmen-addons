#include "vrmlout.h"

int vrml_file_open(vrml_file_p vrml_file, char *filename)
{
  carmen_FILE *fp;

  strcpy(vrml_file->filename, filename);
  fp = carmen_fopen(filename, "w");
  if(fp == NULL) {
    fprintf(stderr, "Error: could not open file %s for writing.\n", filename);
    return -1;
  }
  vrml_file->num_objects = 0;
  vrml_file->object = NULL;
  carmen_fprintf(fp,
	     "#VRML V2.0 utf8\n"
	     "Separator {\n"
	     "  Rotation { rotation 1 0 0 -1.5708 }\n\n");
  carmen_fclose(fp);
  return 0;
}

int vrml_file_new_object(vrml_file_p vrml_file, int object_type,
			 float r, float g, float b)
{
  int i;
  unsigned char rhex, ghex, bhex;
  char strcolor[255];

  vrml_file->num_objects++;
  vrml_file->object = (vrml_object_p)realloc(vrml_file->object,
					     vrml_file->num_objects *
					     sizeof(vrml_object_t));
  carmen_test_alloc(vrml_file->object);
  i = vrml_file->num_objects - 1;
  if(strncmp(carmen_file_extension(vrml_file->filename), ".gz", 3) == 0)
    sprintf(vrml_file->object[i].filename, "/tmp/vrmlobject%d.tmp.gz", i);
  else
    sprintf(vrml_file->object[i].filename, "/tmp/vrmlobject%d.tmp", i);
  vrml_file->object[i].fp = carmen_fopen(vrml_file->object[i].filename, "w");
  if(vrml_file->object[i].fp == NULL) {
    fprintf(stderr, "Error: could not open file %s for writing.\n",
	    vrml_file->object[i].filename);
    return -1;
  }
  vrml_file->object[i].closed = 0;
  vrml_file->object[i].object_type = object_type;
  if(object_type == VRML_POINTSET) {
    carmen_fprintf(vrml_file->object[i].fp,
	       "  Shape {\n"
	       "    appearance Appearance {\n"
	       "      material Material {\n"
	       "        diffuseColor %f %f %f\n"
	       "      }\n"
	       "    }\n\n"
	       "    geometry PointSet {\n"
	       "      coord Coordinate {\n"
	       "        point [\n", r, g, b);
  }
  else if(object_type == VRML_LINESET) {
    carmen_fprintf(vrml_file->object[i].fp,
	       "  Shape {\n"
	       "    appearance Appearance {\n"
	       "      material Material {\n"
	       "        diffuseColor %f %f %f\n"
	       "      }\n"
	       "    }\n\n"
	       "    geometry IndexedLineSet {\n"
	       "      coord Coordinate {\n"
	       "        point [\n", r, g, b);
    vrml_file->object[i].num_points = 0;
  }
  else if(object_type == VRML_FACESET || object_type == VRML_3NORMFACESET) {
    rhex = (unsigned char)(r * 255);
    ghex = (unsigned char)(g * 255);
    bhex = (unsigned char)(b * 255);
    sprintf(strcolor, "0x%02x%02x%02x", rhex, ghex, bhex);
    if(r != -1)
      carmen_fprintf(vrml_file->object[i].fp,
		 "  Shape {\n"
		 "    appearance Appearance {\n"
		 "      texture PixelTexture {\n"
		 "        image 2 2 3 %s %s %s %s\n"
		 "      }\n"
		 "    }\n\n"
		 "    geometry IndexedFaceSet {\n"
		 "      coord Coordinate {\n"
		 "        point [\n", strcolor, strcolor, strcolor, strcolor);
    else
      carmen_fprintf(vrml_file->object[i].fp,
		 "  Shape {\n"
		 "    appearance Appearance {\n"
		 "      material Material {\n"
		 "        diffuseColor 1.0 1.0 1.0\n"
		 "      }\n"
		 "    }\n\n"
		 "    geometry IndexedFaceSet {\n"
		 "      coord Coordinate {\n"
		 "        point [\n");
    vrml_file->object[i].num_points = 0;
    if(object_type == VRML_3NORMFACESET) {
      sprintf(vrml_file->object[i].normal_filename,
	      "/tmp/vrmlobject%d-norm.tmp", i);
      vrml_file->object[i].normal_fp =
	carmen_fopen(vrml_file->object[i].normal_filename, "w");
      if(vrml_file->object[i].normal_fp == NULL) {
	fprintf(stderr, "Error: could not open file %s for writing.\n",
		vrml_file->object[i].normal_filename);
	return -1;
      }
      carmen_fprintf(vrml_file->object[i].normal_fp,
		 "\n      normalPerVertex TRUE\n"
		 "      normal Normal {\n"
		 "        vector [\n");
    }
  }
  return i;
}

void vrml_append_object(vrml_file_p vrml_file, int num)
{
  char buffer[10001];
  FILE *main_fp, *object_fp;
  int n, r;

  main_fp = fopen(vrml_file->filename, "a");
  object_fp = fopen(vrml_file->object[num].filename, "r");
  do {
    n = fread(buffer, 1, 10000, object_fp);
    if(n > 0)
      r = fwrite(buffer, 1, n, main_fp);
  } while(n == 10000);
  fclose(main_fp);
  fclose(object_fp);
}

void vrml_append_normal(vrml_file_p vrml_file, int num)
{
  char buffer[10001];
  FILE *main_fp, *object_fp;
  int n, r;

  main_fp = fopen(vrml_file->filename, "a");
  object_fp = fopen(vrml_file->object[num].normal_filename, "r");
  do {
    n = fread(buffer, 1, 10000, object_fp);
    if(n > 0)
      r = fwrite(buffer, 1, n, main_fp);
  } while(n == 10000);
  fclose(main_fp);
  fclose(object_fp);
}

void vrml_file_close_object(vrml_file_p vrml_file, int num)
{
  int i;

  if(vrml_file->object[num].object_type == VRML_POINTSET) {
    carmen_fprintf(vrml_file->object[num].fp,
	       "        ]\n"
	       "      }\n"
	       "    }\n"
	       "  }\n\n");
  }
  else if(vrml_file->object[num].object_type == VRML_LINESET) {
    carmen_fprintf(vrml_file->object[num].fp,
	       "        ]\n"
	       "      }\n\n"
	       "      coordIndex [\n");
    for(i = 0; i < vrml_file->object[num].num_points / 2; i++)
      carmen_fprintf(vrml_file->object[num].fp,
		 "        %d %d -1\n", i * 2, i * 2 + 1);
    carmen_fprintf(vrml_file->object[num].fp,
	       "      ]\n"
	       "    }\n"
	       "  }\n");
  }
  else if(vrml_file->object[num].object_type == VRML_FACESET) {
    carmen_fprintf(vrml_file->object[num].fp,
	       "        ]\n"
	       "      }\n\n"
	       "      coordIndex [\n");
    for(i = 0; i < vrml_file->object[num].num_points / 3; i++)
      carmen_fprintf(vrml_file->object[num].fp,
		 "        %d %d %d -1\n", i * 3, i * 3 + 1, i * 3 + 2);
    carmen_fprintf(vrml_file->object[num].fp,
	       "      ]\n"
	       "      solid FALSE\n"
	       "    }\n"
    	       "  }\n");
  }
  else if(vrml_file->object[num].object_type == VRML_3NORMFACESET) {
    carmen_fprintf(vrml_file->object[num].fp,
	       "        ]\n"
	       "      }\n\n"
	       "      coordIndex [\n");
    for(i = 0; i < vrml_file->object[num].num_points / 3; i++)
      carmen_fprintf(vrml_file->object[num].fp,
		 "        %d %d %d -1\n", i * 3, i * 3 + 1, i * 3 + 2);
    carmen_fprintf(vrml_file->object[num].fp,
	       "      ]\n"
	       "      solid FALSE\n");
  }
  carmen_fclose(vrml_file->object[num].fp);
  vrml_append_object(vrml_file, num);
  if(vrml_file->object[num].object_type == VRML_3NORMFACESET) {
    carmen_fprintf(vrml_file->object[num].normal_fp,
	       "        ]\n"
	       "      }\n\n"
	       "    }\n"
	       "  }\n");
    carmen_fclose(vrml_file->object[num].normal_fp);
    vrml_append_normal(vrml_file, num);
  }
  vrml_file->object[num].closed = 1;
}

void vrml_add_point(vrml_file_p vrml_file, int num, float x, float y, float z)
{
  carmen_fprintf(vrml_file->object[num].fp,
	     "          %.3f %.3f %.3f\n", x, y, z);
  vrml_file->object[num].num_points++;
}

void vrml_add_line(vrml_file_p vrml_file, int num, float x1, float y1,
		   float z1, float x2, float y2, float z2)
{
  if(vrml_file->object[num].object_type != VRML_LINESET)
    carmen_die("Error: attempting to add line to non-lineset\n");
  vrml_add_point(vrml_file, num, x1, y1, z1);
  vrml_add_point(vrml_file, num, x2, y2, z2);
}

void vrml_compute_normal(float x1, float y1, float z1,
			 float x2, float y2, float z2,
			 float x3, float y3, float z3,
			 float *nx, float *ny, float *nz)
{
  float v1x, v1y, v1z, v2x, v2y, v2z;
  float len;

  v1x = x2 - x1;
  v1y = y2 - y1;
  v1z = z2 - z1;
  v2x = x3 - x1;
  v2y = y3 - y1;
  v2z = z3 - z1;
  *nx = v1y * v2z - v1z * v2y;
  *ny = -v1x * v2z + v1z * v2x;
  *nz = v1x * v2y - v1y * v2x;
  len = sqrt((*nx)*(*nx) + (*ny)*(*ny) + (*nz)*(*nz));
  if(len != 0) {
    *nx /= len;
    *ny /= len;
    *nz /= len;
  }
}

void vrml_add_face(vrml_file_p vrml_file, int num,
		   float x1, float y1, float z1,
		   float x2, float y2, float z2,
		   float x3, float y3, float z3)
{
  if(vrml_file->object[num].object_type != VRML_FACESET)
    carmen_die("Error: attempting to add face to non-faceset\n");
  vrml_add_point(vrml_file, num, x1, y1, z1);
  vrml_add_point(vrml_file, num, x2, y2, z2);
  vrml_add_point(vrml_file, num, x3, y3, z3);
}

void vrml_add_face_with_normals(vrml_file_p vrml_file, int num,
				float x1, float y1, float z1,
				float n1x, float n1y, float n1z,
				float x2, float y2, float z2,
				float n2x, float n2y, float n2z,
				float x3, float y3, float z3,
				float n3x, float n3y, float n3z)
{
  if(vrml_file->object[num].object_type != VRML_3NORMFACESET)
    carmen_die("Error: attempting to add to wrong object type.\n");
  vrml_add_point(vrml_file, num, x1, y1, z1);
  vrml_add_point(vrml_file, num, x2, y2, z2);
  vrml_add_point(vrml_file, num, x3, y3, z3);
  carmen_fprintf(vrml_file->object[num].normal_fp,
	     "          %.2f %.2f %.2f,\n", n1x, n1y, n1z);
  carmen_fprintf(vrml_file->object[num].normal_fp,
	     "          %.2f %.2f %.2f,\n", n2x, n2y, n2z);
  carmen_fprintf(vrml_file->object[num].normal_fp,
	     "          %.2f %.2f %.2f,\n", n3x, n3y, n3z);
}


void vrml_file_close(vrml_file_p vrml_file)
{
  int i;
  carmen_FILE *fp;

  fprintf(stderr, "\rFinishing writing vrml file %s... (0%%)    ",
	  vrml_file->filename);
  for(i = 0; i < vrml_file->num_objects; i++)
    if(!vrml_file->object[i].closed) {
      vrml_file_close_object(vrml_file, i);
      fprintf(stderr, "\rFinishing writing vrml file %s... (%.0f%%)    ",
	      vrml_file->filename,
	      (i + 1) / (float)vrml_file->num_objects * 100);
    }
  fp = carmen_fopen(vrml_file->filename, "a");
  carmen_fprintf(fp, "}\n");
  carmen_fclose(fp);
  fprintf(stderr, "\rFinishing writing vrml file %s... (100%%)    \n",
	  vrml_file->filename);
}
