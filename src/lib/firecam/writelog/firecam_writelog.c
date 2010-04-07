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

#include "firecam_writelog.h"

void firecam_writelog_write_robot_name(char *robot_name,
    carmen_FILE *outfile) {
  carmen_fprintf(outfile, "# robot: %s\n", robot_name);
}

void firecam_writelog_write_header(carmen_FILE *outfile) {
  carmen_fprintf(outfile, "%s\n", CARMEN_LOGFILE_HEADER);
  carmen_fprintf(outfile, "# file format is one message per line\n");
  carmen_fprintf(outfile, "# message_name [message contents] ipc_timestamp ipc_hostname logger_timestamp\n");
  carmen_fprintf(outfile, "# message formats defined: FIRECAMFRAME\n");
  carmen_fprintf(outfile, "# FIRECAMFRAME camera_id filename\n");
}

void firecam_writelog_write_firecam_frame(carmen_firecam_frame_message *frame,
    carmen_FILE *outfile, double timestamp) {
  carmen_fprintf(outfile, "FIRECAMFRAME %d ", frame->cam_id);
  carmen_fprintf(outfile, "%s ", frame->filename);
  carmen_fprintf(outfile, "%lf %s %lf\n", frame->timestamp, frame->host,
    timestamp);
}
