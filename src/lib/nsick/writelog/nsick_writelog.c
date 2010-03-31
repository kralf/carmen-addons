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

#include <carmen/carmen.h>
#include <carmen/laser_messages.h>

#include "nsick_writelog.h"

void nsick_writelog_write_robot_name(char *robot_name, carmen_FILE *outfile) {
  carmen_fprintf(outfile, "# robot: %s\n", robot_name);
}

void nsick_writelog_write_header(carmen_FILE *outfile) {
  carmen_fprintf(outfile, "%s\n", CARMEN_LOGFILE_HEADER);
  carmen_fprintf(outfile, "# file format is one message per line\n");
  carmen_fprintf(outfile, "# message_name [message contents] ipc_timestamp ipc_hostname logger_timestamp\n");
  carmen_fprintf(outfile, "# message formats defined: NSICKSTATUS NSICKLASERPOS NSICKLASER1\n");
  carmen_fprintf(outfile, "# NSICKSTATUS pos\n");
  carmen_fprintf(outfile, "# NSICKLASERPOS laser_id x y z yaw pitch roll\n");
  carmen_fprintf(outfile, "# NSICKLASER1 laser_type start_angle field_of_view angular_resolution maximum_range accuracy remission_mode num_readings [range_readings] num_remissions [remission values]\n");
}

void nsick_writelog_write_nsick_status(carmen_nsick_status_message *status,
  carmen_FILE *outfile, double timestamp) {
  carmen_fprintf(outfile, "NSICKSTATUS %f ", status->pos);
  carmen_fprintf(outfile, "%lf %s %lf\n", status->timestamp, status->host,
    timestamp);
}

void nsick_writelog_write_nsick_laserpos(carmen_nsick_laserpos_message
  *laserpos, carmen_FILE *outfile, double timestamp) {
  carmen_fprintf(outfile, "NSICKLASERPOS %d ", laserpos->laser_id);
  carmen_fprintf(outfile, "%f %f %f ", laserpos->x, laserpos->y, laserpos->z);
  carmen_fprintf(outfile, "%f %f %f ", laserpos->yaw, laserpos->pitch,
    laserpos->roll);
  carmen_fprintf(outfile, "%lf %s %lf\n", laserpos->timestamp, laserpos->host,
    timestamp);
}

void nsick_writelog_write_laser_laser(carmen_laser_laser_message *laser,
  int laser_num, carmen_FILE *outfile, double timestamp) {
  int i;

  carmen_fprintf(outfile, "NSICKLASER%d ", laser_num);
  carmen_fprintf(outfile, "%d %f %f %f %f %f %d ",
		 laser->config.laser_type,
		 laser->config.start_angle,
		 laser->config.fov,
		 laser->config.angular_resolution,
		 laser->config.maximum_range,
		 laser->config.accuracy,
		 laser->config.remission_mode);
  carmen_fprintf(outfile, "%d ", laser->num_readings);
  for(i = 0; i < laser->num_readings; i++)
    carmen_fprintf(outfile, "%.3f ", laser->range[i]);
  carmen_fprintf(outfile, "%d ", laser->num_remissions);
  for(i = 0; i < laser->num_remissions; i++)
    carmen_fprintf(outfile, "%f ", laser->remission[i]);
  carmen_fprintf(outfile, "%f %s %f\n", laser->timestamp,
		 laser->host, timestamp);
}
