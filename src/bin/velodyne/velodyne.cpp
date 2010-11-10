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

#include <sstream>
#include <fstream>
#include <algorithm>

#include <carmen/global.h>
#include <carmen/param_interface.h>
#include <carmen/camera_interface.h>

#include <libvelodyne/AcquisitionThread.h>
#include <libvelodyne/VelodyneControl.h>
#include <libvelodyne/VelodynePointCloud.h>

#include "velodyne_messages.h"
#include "velodyne_ipc.h"

int quit = 0;
int id = 1;

char* dev_name = 0;
int spin_rate;
char* calib_filename = 0;
int dump_enabled = 0;
char* dump_dirname = 0;
int points_publish = 0;

char calib_path[4096];
char dump_path[4096];

std::ifstream calib_file;
std::ofstream dump_file;

void carmen_velodyne_sigint_handler(int q) {
  quit = 1;
}

void carmen_velodyne_set_spin_rate(int spin_rate) {
  VelodyneControl control(dev_name);

  try {
    control.setRPM(spin_rate);
  }
  catch (IOException& exception) {
    carmen_warn("\nWarning: Failed to set spin rate\n");
  }
  catch (OutOfBoundException& exception) {
    carmen_warn("\nWarning: Spin rate %d is invalid\n", spin_rate);
  }
}

void carmen_velodyne_spin_rate_handler(char* module, char* variable,
    char* value) {
  carmen_velodyne_set_spin_rate(spin_rate);
}

void carmen_velodyne_dump_handler(char* module, char* variable, char* value) {
  if (dump_file.is_open())
    dump_file.close();
  
  if (!realpath(dump_dirname, dump_path)) {
    dump_path[0] = '\0';
    carmen_warn("\nWarning: Dump directory %s is invalid\n", dump_dirname);
  }
}

void carmen_velodyne_points_handler(char* module, char* variable, char* value) {
  // nothing to do here
}

int carmen_velodyne_read_parameters(int argc, char **argv) {
  char module[] = "velodyne";
  int num_params;

  char dev_name_var [256], spin_rate_var[256];
  char calib_filename_var[256], dump_enabled_var[256], dump_dirname_var[256];
  char points_publish_var[256];

  if (argc == 2)
    id = atoi(argv[1]);

  sprintf(dev_name_var, "velodyne%d_dev", id);
  sprintf(spin_rate_var, "velodyne%d_spin_rate", id);
  sprintf(calib_filename_var, "velodyne%d_calib_file", id);
  sprintf(dump_enabled_var, "velodyne%d_dump_enable", id);
  sprintf(dump_dirname_var, "velodyne%d_dump_dir", id);
  sprintf(points_publish_var, "velodyne%d_points_publish", id);

  carmen_param_t params[] = {
    {module, dev_name_var, CARMEN_PARAM_STRING, &dev_name, 1, 0},
    {module, spin_rate_var, CARMEN_PARAM_INT, &spin_rate, 1,
      carmen_velodyne_spin_rate_handler},
    {module, calib_filename_var, CARMEN_PARAM_FILE, &calib_filename, 1, 0},
    {module, dump_enabled_var, CARMEN_PARAM_ONOFF, &dump_enabled, 1,
      carmen_velodyne_dump_handler},
    {module, dump_dirname_var, CARMEN_PARAM_DIR, &dump_dirname, 1,
      carmen_velodyne_dump_handler},
    {module, points_publish_var, CARMEN_PARAM_ONOFF, &points_publish, 1,
      carmen_velodyne_points_handler},
  };

  num_params = sizeof(params)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, params, num_params);

  if (!realpath(calib_filename, calib_path)) {
    calib_path[0] = '\0';
    carmen_warn("\nWarning: Calibration filename %s is invalid\n",
      calib_filename);
  }

  if (!realpath(dump_dirname, dump_path)) {
    dump_path[0] = '\0';
    carmen_warn("\nWarning: Dump directory %s is invalid\n", dump_dirname);
  }

  return num_params;
}

int main(int argc, char *argv[]) {
  AcquisitionThread thread;
  VelodyneCalibration calib;
  boost::shared_ptr<VelodynePacket> packet;
  IPC_RETURN_TYPE err;
  char dump_filename[4096];

  carmen_velodyne_ipc_initialize(argc, argv);

  carmen_velodyne_read_parameters(argc, argv);

  carmen_velodyne_set_spin_rate(spin_rate);
  
  calib_file.open(calib_path);
  if (!calib_file.is_open())
    carmen_die("\nError: Could not open %s for reading\n", calib_filename);
  calib_file >> calib;
  calib_file.close();
  
  signal(SIGINT, carmen_velodyne_sigint_handler);
  thread.run();

  double time;
  double start_time = carmen_get_time();
  int num_packets = 0;
  int start_num_packets = 0;
  int num_lost_packets = 0;
  int start_num_lost_packets = 0;
  
  while (!quit) {
    try {
      while (dump_enabled || points_publish) {
        packet = thread.getPacket();
        ++num_packets;

        if (dump_enabled) {
          if (!dump_file.is_open()) {
            char dump_time[1024];
            time_t local = packet->getTimestamp();
            struct tm* time = localtime(&local);

            strftime(dump_time, sizeof(dump_time), "%Y-%m-%d-%H%M%S", time);
            sprintf(dump_filename, "%s/%s.bin", dump_path, dump_time);
            dump_file.open(dump_filename, std::ios::out | std::ios::binary);
          }

          if (dump_file.is_open()) {
            long dump_filepos = dump_file.tellp();
            dump_file << *packet;
            carmen_velodyne_publish_packet(id, dump_filename,
              dump_filepos, packet->getTimestamp());
          }
          else {
            carmen_warn("\nWarning: Could not open %s for writing\n",
              dump_filename);
          }
        }

        if (points_publish) {
          VelodynePointCloud pointCloud(*packet, calib);
          int num_points = pointCloud.getSize();
          float x[num_points], y[num_points], z[num_points];
          std::vector<Point3D>::const_iterator it;
          int i = 0;

          for (it = pointCloud.getStartIterator();
              it != pointCloud.getEndIterator(); ++it, ++i) {
            x[i] = it->mf64X;
            y[i] = it->mf64Y;
            z[i] = it->mf64Z;
          }
          carmen_velodyne_publish_pointcloud(id, num_points, x, y, z,
            packet->getTimestamp());
        }

        double time = carmen_get_time();
        if (time-start_time >= 1.0) {
          num_lost_packets = thread.getQueueDroppedPackages();
          float period_num_packets = num_packets-start_num_packets;
          float period_num_lost_packets = num_lost_packets-
            start_num_lost_packets;
          
          double packet_loss = period_num_lost_packets/
            (period_num_packets+period_num_lost_packets);
          fprintf(stdout, "\rPacket loss: %6.2f%%", packet_loss*100.0);
          fflush(stdout);

          start_time = time;
          start_num_packets = num_packets;
          start_num_lost_packets = num_lost_packets;          
        }
      }
    }
    catch (IOException& exception) {
      // buffer underrun
    }

    carmen_ipc_sleep(0.0);
  }

  thread.stop();
  if (dump_file.is_open())
    dump_file.close();

  fprintf(stdout, "\n");

  return 0;
}
