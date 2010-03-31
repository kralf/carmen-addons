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

#include <carmen/global.h>
#include <carmen/param_interface.h>
#include <carmen/camera_interface.h>

#include <libfirecam/firecam.h>
#include <libfirecam/utils.h>

#include "firecam_messages.h"
#include "firecam_ipc.h"

int quit = 0;
int id = 1;

uint64_t guid;

FireCAMConfiguration configuration;

int mode_width = 640;
int mode_height = 480;
FireCAMColor::Coding mode_color = FireCAMColor::mono8;
int mode_scalable = 0;

double framerate_fps = 15.0;

int capture_buffer = 16;
FireCAMCapture::Mode capture_mode = FireCAMCapture::legacy;
int capture_speed = 400;

int filter_enabled = 0;
FireCAMColorFilter::Tile filter_tile = FireCAMColorFilter::rggb;
FireCAMColorFilter::Method filter_method = FireCAMColorFilter::nearest;

FireCAMCamera camera;

void carmen_firecam_sigint_handler(int q __attribute__((unused))) {
  quit = 1;
}

int carmen_firecam_read_parameters(int argc, char **argv) {
  int num_params;
  char module[256];

  char* guid_str;
  char* mode_color_str;
  char* capture_mode_str;
  char* filter_tile_str;
  char* filter_method_str;

  if (argc == 2)
    id = atoi(argv[1]);
  sprintf(module, "firecam_firecam%d", id);

  carmen_param_t params[] = {
    {module, "guid", CARMEN_PARAM_STRING, &guid_str, 0, NULL},

    {module, "mode_width", CARMEN_PARAM_INT, &mode_width, 0, NULL},
    {module, "mode_height", CARMEN_PARAM_INT, &mode_height, 0, NULL},
    {module, "mode_color", CARMEN_PARAM_STRING, &mode_color_str, 0, NULL},
    {module, "mode_scalable", CARMEN_PARAM_ONOFF, &mode_scalable, 0, NULL},

    {module, "framerate", CARMEN_PARAM_DOUBLE, &framerate_fps, 0, NULL},

    {module, "capture_buffer", CARMEN_PARAM_INT, &capture_buffer, 0, NULL},
    {module, "capture_mode", CARMEN_PARAM_STRING, &capture_mode_str, 0, NULL},
    {module, "capture_speed", CARMEN_PARAM_INT, &capture_speed, 0, NULL},

    {module, "filter_enabled", CARMEN_PARAM_ONOFF, &filter_enabled, 0, NULL},
    {module, "filter_tile", CARMEN_PARAM_STRING, &filter_tile_str, 0, NULL},
    {module, "filter_method", CARMEN_PARAM_STRING, &filter_method_str, 0, NULL},
  };

  num_params = sizeof(params)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, params, num_params);

  std::istringstream stream(guid_str);
  stream >> std::hex >> guid;

  camera = FireCAM::getInstance().getCamera(guid);
  configuration = camera.getConfiguration();

  mode_color = FireCAMUtils::convert(mode_color_str,
    FireCAMColor::codingStrings);
  FireCAMVideoMode videoMode(mode_width, mode_height, mode_color,
    mode_scalable);
  configuration.setVideoMode(videoMode);

  FireCAMFramerate framerate(framerate_fps);
  configuration.setFramerate(framerate);

  capture_mode = FireCAMUtils::convert(capture_mode_str,
    FireCAMCapture::modeStrings);
  FireCAMCapture capture(capture_buffer, capture_mode, capture_speed);
  configuration.setCapture(capture);

  filter_tile = FireCAMUtils::convert(filter_tile_str,
    FireCAMColorFilter::tileStrings);
  filter_method = FireCAMUtils::convert(filter_method_str,
    FireCAMColorFilter::methodStrings);
  FireCAMColorFilter colorFilter(filter_enabled, filter_tile, filter_method);
  configuration.setColorFilter(colorFilter);

  carmen_param_allow_unfound_variables(1);

  const std::set<FireCAMFeature>& features = camera.getFeatures();
  for (std::set<FireCAMFeature>::const_iterator it = features.begin();
      it != features.end(); ++it) {
    FireCAMFeature feature = *it;
    int num_feature_params;
    char feature_enabled_var[256], feature_mode_var[256],
      feature_values_var[256];
    int feature_enabled = 0;
    char* feature_mode_str = 0;
    char* feature_values_str = 0;
    FireCAMFeature::Mode feature_mode;

    sprintf(feature_enabled_var, "%s_enabled", feature.getName().c_str());
    sprintf(feature_mode_var, "%s_mode", feature.getName().c_str());
    sprintf(feature_values_var, "%s_values", feature.getName().c_str());

    carmen_param_t feature_params[] = {
      {module, feature_enabled_var, CARMEN_PARAM_ONOFF, &feature_enabled,
        0, NULL},
      {module, feature_mode_var, CARMEN_PARAM_STRING, &feature_mode_str,
        0, NULL},
      {module, feature_values_var, CARMEN_PARAM_STRING, &feature_values_str,
        0, NULL},
    };

    num_feature_params = sizeof(feature_params)/sizeof(carmen_param_t);
    carmen_param_install_params(argc, argv, feature_params, num_feature_params);
    num_params += num_feature_params;

    feature.setEnabled(feature_enabled);
    if (feature_mode_str) {
      feature_mode = FireCAMUtils::convert(feature_mode_str,
        FireCAMFeature::modeStrings);
      feature.setMode(feature_mode);
    }
    if (feature_values_str) {
      std::vector<size_t> values;
      FireCAMUtils::convert(feature_values_str, values);
      feature.setValues(values);
    }

    configuration.setFeature(feature);
  }

  camera.setConfiguration(configuration);

  return num_params;
}

void carmen_firecam_publish_image(const FireCAMFrame& frame) {
  carmen_camera_image_message image;
  IPC_RETURN_TYPE err;

  image.width = frame.getWidth();
  image.height = frame.getHeight();
  image.bytes_per_pixel = frame.getColor().getDepth()/8;

  image.image_size = frame.getSize();
  image.image = (char*)frame.getImage();

  image.timestamp = frame.getTimestamp();
  image.host = carmen_get_host();

  err = IPC_publishData(CARMEN_CAMERA_IMAGE_NAME, &image);
  carmen_test_ipc_exit(err, "Could not publish", CARMEN_CAMERA_IMAGE_NAME);

  std::cout << ".";
  std::cout.flush();
}

int main(int argc, char *argv[]) {
  IPC_RETURN_TYPE err;

  carmen_firecam_ipc_initialize(argc, argv);
  err = IPC_defineMsg(CARMEN_CAMERA_IMAGE_NAME, IPC_VARIABLE_LENGTH,
    CARMEN_CAMERA_IMAGE_FMT);
  carmen_test_ipc_exit(err, "Could not define message",
    CARMEN_CAMERA_IMAGE_NAME);

  carmen_firecam_read_parameters(argc, argv);

  camera.connect();
  signal(SIGINT, carmen_firecam_sigint_handler);
  while (!quit) {
    FireCAMFrame frame;
    camera.capture(frame, FireCAMColor::rgb8);
    carmen_firecam_publish_image(frame);
  }
  camera.disconnect();
  std::cout << std::endl;

  return 0;
}
