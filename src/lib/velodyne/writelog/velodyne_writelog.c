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

#include "velodyne_writelog.h"

void velodyne_writelog_write_header(carmen_FILE *outfile) {
  carmen_fprintf(outfile, "%s\n", CARMEN_LOGFILE_HEADER);
  carmen_fprintf(outfile, "# file format is one message per line\n");
  carmen_fprintf(outfile, "# message_name [message contents] ipc_timestamp ipc_hostname logger_timestamp\n");
  carmen_fprintf(outfile, "# message formats defined: VELODYNEPACKAGE\n");
  carmen_fprintf(outfile, "# VELODYNEPACKAGE laser_id filename filepos\n");
}

void velodyne_writelog_write_velodyne_package(carmen_velodyne_package_message
  *package, carmen_FILE *outfile, double timestamp) {
  carmen_fprintf(outfile, "VELODYNEPACKAGE %d ", package->laser_id);
  carmen_fprintf(outfile, "%s ", package->filename);
  carmen_fprintf(outfile, "%ld ", package->filepos);
  carmen_fprintf(outfile, "%lf %s %lf\n", package->timestamp, package->host,
    timestamp);
}
