#include <carmen/glv.h>

#include "logread.h"

/* 3D model parameters */
#define      CLASSIFY_POINTS         0
#define      GENERATE_FACES          1
#define      CONNECT_DIST            0.05
#define      MAX_RANGE               60.0

/* scan classification parameters */
#define      TYPE_UNKNOWN            0
#define      TYPE_IGNORE             1
#define      TYPE_SAFE               2
#define      TYPE_OBSTACLE           3
#define      OBSTACLE_WINDOW         0.2
#define      MIN_OBSTACLE            0.08

#define sqr(a) ((a)*(a))
#define hypot3(a, b, c) (sqrt(sqr(a)+sqr(b)+sqr(c)))

typedef struct {
  int meshed;
  float range, x, y, z;
} point3D_t, *point3D_p;

void classify_scan(int num_readings, point3D_p scan_points, float *local_x,
  char *pointclass) {
  int i, j, min_neighbor, max_neighbor;
  float min_height[360], max_height[360];

  for (i = 0; i < num_readings; i++) {
    if (pointclass[i] == TYPE_UNKNOWN) {
      min_neighbor = i;
      while (min_neighbor >= 0 &&
        (fabs(local_x[i]-local_x[min_neighbor]) < OBSTACLE_WINDOW ||
        pointclass[min_neighbor] == TYPE_IGNORE))
        min_neighbor--;
      min_neighbor++;
      max_neighbor = i;

      while (max_neighbor < num_readings &&
        (fabs(local_x[i]-local_x[max_neighbor]) < OBSTACLE_WINDOW ||
        pointclass[max_neighbor] == TYPE_IGNORE))
        max_neighbor++;
      max_neighbor--;
      min_height[i] = scan_points[i].z;
      max_height[i] = scan_points[i].z;

      for (j = min_neighbor; j <= max_neighbor; j++) {
        if (pointclass[j] != TYPE_IGNORE && scan_points[j].z < min_height[i])
          min_height[i] = scan_points[j].z;
        else if (pointclass[j] != TYPE_IGNORE &&
          scan_points[j].z > max_height[i])
          max_height[i] = scan_points[j].z;
      }

      if (max_height[i]-min_height[i] > MIN_OBSTACLE)
        pointclass[i] = TYPE_OBSTACLE;
      else
        pointclass[i] = TYPE_SAFE;
    }
  }
}

double interpolate_yaw(double yaw1, double yaw2, double f) {
  double result;
  
  if (yaw1 > 0 && yaw2 < 0 && yaw1-yaw2 > M_PI) {
    yaw2 += 2*M_PI;
    result = yaw1+f*(yaw2-yaw1);
    if (result > M_PI)
      result -= 2*M_PI;
    return result;
  }
  else if (yaw1 < 0 && yaw2 > 0 && yaw2-yaw1 > M_PI) {
    yaw1 += 2*M_PI;
    result = yaw1+f*(yaw2-yaw1);
    if (result > M_PI)
      result -= 2*M_PI;
    return result;
  }
  else
    return yaw1+f*(yaw2-yaw1);
}

void interpolate_laser_pos(logdata_p logdata) {
  int i, mark = 0;
  float f;

  for (i = 0; i < logdata->num_laser; i++) {
    if (logdata->laser[i].timestamp < logdata->pos[0].timestamp ||
      logdata->laser[i].timestamp > logdata->pos[logdata->num_pos-1].timestamp)
      logdata->laser[i].ignore = 1;
    else {
      logdata->laser[i].ignore = 0;
      while (logdata->pos[mark+1].timestamp < logdata->laser[i].timestamp)
        ++mark;

      f = (logdata->laser[i].timestamp-logdata->pos[mark].timestamp)/
        (logdata->pos[mark+1].timestamp-logdata->pos[mark].timestamp);

      logdata->laser[i].x = logdata->pos[mark].x+
        f*(logdata->pos[mark+1].x-logdata->pos[mark].x);
      logdata->laser[i].y = logdata->pos[mark].y+
        f*(logdata->pos[mark+1].y-logdata->pos[mark].y);
      logdata->laser[i].z = logdata->pos[mark].z+
        f*(logdata->pos[mark+1].z-logdata->pos[mark].z);
      logdata->laser[i].roll = logdata->pos[mark].roll+
        f*(logdata->pos[mark+1].roll-logdata->pos[mark].roll);
      logdata->laser[i].pitch = logdata->pos[mark].pitch+
        f*(logdata->pos[mark+1].pitch-logdata->pos[mark].pitch);
      logdata->laser[i].yaw = interpolate_yaw(logdata->pos[mark].yaw,
        logdata->pos[mark+1].yaw, f);
    }
  }
}

void write_glv_output(logdata_p logdata, char *out_filename) {
  int i, j, first_scan = 1;
  transform_t t;
  double angle, p_x, p_y, p_z;
  float local_x[361];
  point3D_t last_scan[361], current_scan[361];
  double d1, d2, d3, d4;
  char pointclass[361], last_pointclass[361];
  my_FILE *out_fp;

  out_fp = my_fopen(out_filename, "w");

  /* write path to glv file */
  write_color_glv(out_fp, 255, 255, 0);
  for (i = 1; i < logdata->num_pos; i++)
    write_line_glv(out_fp,
      logdata->pos[i-1].x, logdata->pos[i-1].y, logdata->pos[i-1].z,
      logdata->pos[i].x, logdata->pos[i].y, logdata->pos[i].z);

  /* write scan to glv file */
  for (i = 0; i < logdata->num_laser; i++) {
    if (!logdata->laser[i].ignore) {
      if(i % 100 == 0)
        fprintf(stderr, "\rProjecting points... (%.0f%%)  ",
        i/(float)logdata->num_laser*100.0);

      create_transform_identity(t);
      rotate_transform_x(t, logdata->laser[i].roll);
      rotate_transform_y(t, -logdata->laser[i].pitch);
      rotate_transform_z(t, logdata->laser[i].yaw);
      translate_transform(t, logdata->laser[i].x, 
			  logdata->laser[i].y, logdata->laser[i].z);

      for (j = 0; j < logdata->laser[i].num_readings; j++) {
        angle = -M_PI_2+j/(float)logdata->laser[i].num_readings*M_PI;
        p_x = logdata->laser[i].range[j] * cos(angle);
        p_y = logdata->laser[i].range[j] * sin(angle);
        p_z = 0.0;
        transform_point(&p_x, &p_y, &p_z, t);

      	pointclass[j] = TYPE_UNKNOWN;
        if (CLASSIFY_POINTS) {
          if (logdata->laser[i].range[j] > MAX_RANGE)
            pointclass[j] = TYPE_IGNORE;
          else if (p_z > 0.0)
            pointclass[j] = TYPE_OBSTACLE;
          if (pointclass[j] == TYPE_UNKNOWN)
          local_x[j] = hypot(p_x-logdata->laser[i].x, p_y-logdata->laser[i].y);
        }
        
        last_scan[j] = current_scan[j];
        last_pointclass[j] = pointclass[j];
        current_scan[j].x = p_x;
        current_scan[j].y = p_y;
        current_scan[j].z = p_z;
        current_scan[j].meshed = 0;
        current_scan[j].range = logdata->laser[i].range[j];
      }
	
      if (CLASSIFY_POINTS)
        classify_scan(logdata->laser[i].num_readings, current_scan,
		    local_x, pointclass);

      write_color_glv(out_fp, 255, 255, 255);
      for (j = 0; j < logdata->laser[i].num_readings; j++) {
      	if (GENERATE_FACES && !first_scan && j > 0) {
          d1 = logdata->laser[i].range[j-1]-logdata->laser[i-1].range[j-1];
	        d2 = logdata->laser[i].range[j]-logdata->laser[i].range[j-1];
	        d3 = logdata->laser[i-1].range[j]-logdata->laser[i].range[j];
          d4 = logdata->laser[i-1].range[j-1]-logdata->laser[i-1].range[j];

          if (fabs(d1) < CONNECT_DIST && fabs(d2) < CONNECT_DIST &&
            fabs(d3) < CONNECT_DIST && fabs(d4) < CONNECT_DIST &&
            logdata->laser[i].range[j-1] < MAX_RANGE &&
            logdata->laser[i-1].range[j-1] < MAX_RANGE &&
            logdata->laser[i].range[j] < MAX_RANGE &&
            logdata->laser[i-1].range[j] < MAX_RANGE) {

            if (pointclass[j] == TYPE_OBSTACLE ||
              pointclass[j-1] == TYPE_OBSTACLE)
              write_color_glv(out_fp, 100, 100, 255);
            else
              write_color_glv(out_fp, 255, 255, 255);

            write_face_glv(out_fp,
              last_scan[j-1].x, last_scan[j-1].y, last_scan[j-1].z,
              current_scan[j-1].x, current_scan[j-1].y, current_scan[j-1].z,
              current_scan[j].x, current_scan[j].y, current_scan[j].z);
            write_face_glv(out_fp,
              last_scan[j-1].x, last_scan[j-1].y, last_scan[j-1].z,
              current_scan[j].x, current_scan[j].y, current_scan[j].z,
              last_scan[j].x, last_scan[j].y, last_scan[j].z);

            last_scan[j].meshed = 1;
            last_scan[j-1].meshed = 1;
            current_scan[j].meshed = 1;
            current_scan[j-1].meshed = 1;
          }
        }
      }

      if (GENERATE_FACES) {
        write_color_glv(out_fp, 255, 255, 255);
        for (j = 0; j < logdata->laser[i].num_readings; j++) {
          if (last_pointclass[j] == TYPE_SAFE &&
            !last_scan[j].meshed && !first_scan &&
            last_scan[j].range < MAX_RANGE)
            write_point_glv(out_fp, last_scan[j].x, last_scan[j].y,
				    last_scan[j].z);
        }
	
        write_color_glv(out_fp, 100, 100, 255);
        for (j = 0; j < logdata->laser[i].num_readings; j++) {
          if (last_pointclass[j] == TYPE_OBSTACLE &&
            !last_scan[j].meshed && !first_scan &&
            last_scan[j].range < MAX_RANGE)
            write_point_glv(out_fp, last_scan[j].x, last_scan[j].y,
            last_scan[j].z);
        }
      }
      else {
      	write_color_glv(out_fp, 255, 255, 255);
        for (j = 0; j < logdata->laser[i].num_readings; j++) {
          if (pointclass[j] == TYPE_SAFE)
          write_point_glv(out_fp, current_scan[j].x, current_scan[j].y,
			    current_scan[j].z);
        }

        write_color_glv(out_fp, 100, 100, 255);
        for (j = 0; j < logdata->laser[i].num_readings; j++) {
          if (pointclass[j] == TYPE_OBSTACLE)
          write_point_glv(out_fp, current_scan[j].x, current_scan[j].y,
			    current_scan[j].z);
        }
      }

      first_scan = 0;
    }
  }

  fprintf(stderr, "\rProjecting points... (100%%)    \n");

  my_fclose(out_fp);
}

void compute_distance(logdata_p logdata) {
  float distance = 0;
  int i;

  for (i = 1; i < logdata->num_pos; i++)
    distance += hypot3(
    logdata->pos[i].x-logdata->pos[i-1].x,
    logdata->pos[i].y-logdata->pos[i-1].y,
    logdata->pos[i].z-logdata->pos[i-1].z);

  fprintf(stderr, "Trajectory length = %.2f meters.\n", distance);
}

int main(int argc, char **argv) {
  char filename[200], out_filename[200];
  logdata_t logdata;

  if(argc < 2)
    carmen_die("Error: missing arguments\n"
	    "Usage: %s filename\n", argv[0]);
  strcpy(filename, argv[1]);
  strcpy(out_filename, filename);
  if (!strcmp(out_filename+strlen(out_filename)-4, ".rec"))
    strcpy(out_filename+strlen(out_filename)-3, "glv.gz");
  else if (!strcmp(out_filename + strlen(out_filename)-7, ".rec.gz"))
    strcpy(out_filename+strlen(out_filename)-6, "glv.gz");
  else if (!strcmp(out_filename + strlen(out_filename)-7, ".log.gz"))
    strcpy(out_filename + strlen(out_filename)-6, "glv.gz");
  else if (!strcmp(out_filename + strlen(out_filename)-4, ".log"))
    strcpy(out_filename + strlen(out_filename)-3, "glv.gz");
  else
    carmen_die("Error: input file must be a .rec .rec.gz"
	    ".log or .log.gz file.\n");

  read_nsick_logfile(argv[1], &logdata);
  interpolate_laser_pos(&logdata);
  write_glv_output(&logdata, out_filename);
  compute_distance(&logdata);

  return 0;
}