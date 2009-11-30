#include <carmen/global.h>
#include <carmen/my_stdio.h>

#include "logread.h"

void read_nsick_logfile(char *filename, logdata_p logdata) {
  int buffer_pos, buffer_length, offset = 0;
  int linecount = 0, mark, n, i;
  long int nread, log_bytes = 0;
  my_FILE *log_fp = NULL;
  char *current_pos;
  char buffer[10000];
  int laser_num_readings;

  /* initialize logdata structure */
  logdata->num_laser = 0;
  logdata->max_laser = 1000;
  logdata->laser = (laser_scan_p)calloc(logdata->max_laser,
    sizeof(laser_scan_t));
  carmen_test_alloc(logdata->laser);
  logdata->num_pos = 0;
  logdata->max_pos = 1000;
  logdata->pos = (laser_pos_p)calloc(logdata->max_pos, sizeof(laser_pos_t));
  carmen_test_alloc(logdata->pos);

  /* compute total number of bytes in logfile */
  log_fp = my_fopen(filename, "r");
  if (log_fp == NULL)
    carmen_die("Error: could not open file %s for reading.\n", filename);

  do {
    nread = my_fread(buffer, 1, 10000, log_fp);
    log_bytes += nread;
  }
  while (nread > 0);
  my_fseek(log_fp, 0L, SEEK_SET);

  /* read the logfile */
  buffer_pos = 0;
  buffer_length = my_fread(buffer, 1, 10000, log_fp);

  while (buffer_length > 0) {
    mark = buffer_pos;
    while ((mark < buffer_length) && (buffer[mark] != '\n'))
      ++mark;

    if (mark == buffer_length) {
      memmove(buffer, buffer+buffer_pos, buffer_length-buffer_pos);
      buffer_length -= buffer_pos;
      offset += buffer_pos;
      buffer_pos = 0;
      n = my_fread(buffer+buffer_length, 1, 10000-buffer_length-1, log_fp);
      buffer_length += n;
    }
    else {
      ++linecount;
      if (linecount % 100 == 0)
        fprintf(stderr, "\rReading log file %s... (%.0f%%)  ", filename,
        (offset+buffer_pos)/(float)log_bytes*100.0);
      buffer[mark] = '\0';

      if (!strncmp(buffer+buffer_pos, "NSICKLASERPOS", 13)) {
        if (logdata->num_pos == logdata->max_pos) {
          logdata->max_pos += 1000;
          logdata->pos = (laser_pos_p)realloc(logdata->pos,
            logdata->max_pos*sizeof(laser_pos_t));
          carmen_test_alloc(logdata->pos);
        }

        current_pos = buffer+buffer_pos;
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].laser_num = atoi(current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].x = atof(current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].y = atof(current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].z = atof(current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].yaw = atof(current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].pitch = atof(current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].roll = atof(current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->pos[logdata->num_pos].timestamp = atof(current_pos);

        ++logdata->num_pos;
      }
      else if (logdata->num_pos &&
        !strncmp(buffer+buffer_pos, "NSICKLASER", 10) &&
        buffer[buffer_pos+10] ==
          '0'+logdata->pos[logdata->num_pos-1].laser_num) {
        if (logdata->num_laser == logdata->max_laser) {
          logdata->max_laser += 1000;
          logdata->laser = (laser_scan_p)realloc(logdata->laser,
            logdata->max_laser*sizeof(laser_scan_t));
          carmen_test_alloc(logdata->laser);
        }

        current_pos = buffer+buffer_pos;
        logdata->laser[logdata->num_laser].laser_num =
          logdata->pos[logdata->num_pos].laser_num;

        current_pos = carmen_next_n_words(current_pos, 8);
        laser_num_readings = atoi(current_pos);
        logdata->laser[logdata->num_laser].num_readings = laser_num_readings;
        logdata->laser[logdata->num_laser].range =
          (float*)calloc(laser_num_readings, sizeof(float));
        carmen_test_alloc(logdata->laser[logdata->num_laser].range);
        logdata->laser[logdata->num_laser].endpoint_x =
          (float*)calloc(laser_num_readings, sizeof(float));
        carmen_test_alloc(logdata->laser[logdata->num_laser].endpoint_x);
        logdata->laser[logdata->num_laser].endpoint_y =
          (float*)calloc(laser_num_readings, sizeof(float));
        carmen_test_alloc(logdata->laser[logdata->num_laser].endpoint_y);
        logdata->laser[logdata->num_laser].endpoint_z =
          (float*)calloc(laser_num_readings, sizeof(float));
        carmen_test_alloc(logdata->laser[logdata->num_laser].endpoint_z);
        current_pos = carmen_next_word(current_pos);
        for (i = 0; i < laser_num_readings; i++) {
          logdata->laser[logdata->num_laser].range[i] = atof(current_pos);
          current_pos = carmen_next_word(current_pos);
        }

        current_pos = carmen_next_word(current_pos);
        laser_num_readings = atoi(current_pos);
        logdata->laser[logdata->num_laser].timestamp = atof(current_pos);

        logdata->num_laser++;
      }

      buffer_pos = mark+1;
    }
  }

  fprintf(stderr, "\nRead %d LASER - %d POS\n",
	  logdata->num_laser, logdata->num_pos);
}
