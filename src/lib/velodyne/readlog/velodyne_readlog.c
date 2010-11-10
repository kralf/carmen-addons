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
 * either version 2 of the License, or (at your option)
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

#include <carmen/global.h>
#include <carmen/carmen_stdio.h>

#include "velodyne_readlog.h"

int first_wordlength(char *str)
{
  char* c_enter = strchr(str, '\n'); // check also for newline
  char* c = strchr(str, ' ');

  if (c_enter == NULL && c == NULL) // it is the last word in the string
    return strlen(str);

  if (c_enter != NULL && c == NULL) // there is no space but a newline
    return c_enter - str;

  if (c_enter == NULL && c != NULL) // there is a space but no newline
    return c - str;

  if (c_enter < c )    // use whatever comes first
    return c_enter - str;
  else
    return c - str;
}

void copy_filename_string(char **filename, char **string)
{
  int l;
  while(*string[0] == ' ')
    *string += 1;                      /* advance past spaces */
  l = first_wordlength(*string);
  if(*filename != NULL)
    free(*filename);
  *filename = (char *)calloc(1, l+1);  /* allocate one extra char for the \0 */
  carmen_test_alloc(*filename);
  strncpy(*filename, *string, l);
  (*filename)[l] = '\0';
  *string += l;
}

void read_velodyne_logfile(char *filename, logdata_p logdata) {
  int buffer_pos, buffer_length, offset = 0;
  int linecount = 0, mark, n;
  long int nread, log_bytes = 0;
  carmen_FILE *log_fp = NULL;
  char *current_pos;
  char buffer[10000];

  /* initialize logdata structure */
  logdata->num_packet = 0;
  logdata->max_packet = 1000;
  logdata->packet = (velodyne_packet_p)calloc(logdata->max_packet,
    sizeof(velodyne_packet_t));
  carmen_test_alloc(logdata->packet);

  /* compute total number of bytes in logfile */
  log_fp = carmen_fopen(filename, "r");
  if (log_fp == NULL)
    carmen_die("Error: could not open file %s for reading.\n", filename);

  do {
    nread = carmen_fread(buffer, 1, 10000, log_fp);
    log_bytes += nread;
  }
  while (nread > 0);
  carmen_fseek(log_fp, 0L, SEEK_SET);

  /* read the logfile */
  buffer_pos = 0;
  buffer_length = carmen_fread(buffer, 1, 10000, log_fp);

  while (buffer_length > 0) {
    mark = buffer_pos;
    while ((mark < buffer_length) && (buffer[mark] != '\n'))
      ++mark;

    if (mark == buffer_length) {
      memmove(buffer, buffer+buffer_pos, buffer_length-buffer_pos);
      buffer_length -= buffer_pos;
      offset += buffer_pos;
      buffer_pos = 0;
      n = carmen_fread(buffer+buffer_length, 1, 10000-buffer_length-1, log_fp);
      buffer_length += n;
    }
    else {
      ++linecount;
      if (linecount % 100 == 0)
        fprintf(stderr, "\rReading log file %s... (%.0f%%)  ", filename,
        (offset+buffer_pos)/(float)log_bytes*100.0);
      buffer[mark] = '\0';

      if (!strncmp(buffer+buffer_pos, "VELODYNEPACKET", 12)) {
        if (logdata->num_packet == logdata->max_packet) {
          logdata->max_packet += 1000;
          logdata->packet = (velodyne_packet_p)realloc(logdata->packet,
            logdata->max_packet*sizeof(velodyne_packet_t));
          carmen_test_alloc(logdata->packet);
        }

        current_pos = buffer+buffer_pos;
        current_pos = carmen_next_word(current_pos);
        logdata->packet[logdata->num_packet].laser_id = atoi(current_pos);
        copy_filename_string(&logdata->packet[logdata->num_packet].filename,
          &current_pos);
        current_pos = carmen_next_word(current_pos);
        logdata->packet[logdata->num_packet].timestamp = atof(current_pos);

        ++logdata->num_packet;
      }

      buffer_pos = mark+1;
    }
  }

  fprintf(stderr, "\nRead %d PACKET\n", logdata->num_packet);
}
