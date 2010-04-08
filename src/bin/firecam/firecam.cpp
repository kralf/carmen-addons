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
#include <algorithm>

#include <carmen/global.h>
#include <carmen/param_interface.h>
#include <carmen/camera_interface.h>

#include <libfirecam/firecam.h>
#include <libfirecam/utils.h>

#include "firecam_messages.h"
#include "firecam_ipc.h"

typedef struct {
  int enabled;
  char* mode_str;
  char* values_str;
} carmen_firecam_feature_param_t, *carmen_firecam_feature_param_p;

int quit = 0;
int id = 1;

char* guid_str = 0;
uint64_t guid;

FireCAMConfiguration configuration;

int mode_width = 640;
int mode_height = 480;
char* mode_color_str = 0;
FireCAMColor::Coding mode_color = FireCAMColor::mono8;
int mode_scalable = 0;

double framerate_fps = 15.0;

int capture_buffer = 16;
char* capture_mode_str = 0;
FireCAMCapture::Mode capture_mode = FireCAMCapture::legacy;
int capture_speed = 400;

int filter_enabled = 0;
char* filter_tile_str = 0;
FireCAMColorFilter::Tile filter_tile = FireCAMColorFilter::rggb;
char* filter_method_str = 0;
FireCAMColorFilter::Method filter_method = FireCAMColorFilter::nearest;

int thumb_width = 320;
int thumb_height = 240;
char* thumb_color_str = 0;
FireCAMColor::Coding thumb_color = FireCAMColor::rgb8;

int dump_enabled = 0;
char* dump_dir = 0;
char dump_path[4096];

std::map<std::string, carmen_firecam_feature_param_t> features;

FireCAMCamera camera;

void carmen_firecam_sigint_handler(int q __attribute__((unused))) {
  quit = 1;
}

void carmen_firecam_mode_handler(char* module, char* variable, char* value) {
  try {
    mode_color = FireCAMUtils::convert(mode_color_str,
      FireCAMColor::codingStrings);
    FireCAMVideoMode videoMode(mode_width, mode_height, mode_color,
      mode_scalable);

    if (camera.hasVideoMode(videoMode)) {
      configuration.setVideoMode(videoMode);

      camera.disconnect();
      usleep(10000);
      camera.setConfiguration(configuration);
      camera.connect();
    }
  }
  catch (std::runtime_error error) {
    carmen_warn("\nWarning: %s\n", error.what());
  }
}

void carmen_firecam_framerate_handler(char* module, char* variable,
    char* value) {
  try {
    FireCAMFramerate framerate(framerate_fps);
    configuration.setFramerate(framerate);

    camera.disconnect();
    usleep(10000);
    camera.setConfiguration(configuration);
    camera.connect();
  }
  catch (std::runtime_error error) {
    carmen_warn("\nWarning: %s\n", error.what());
  }
}

void carmen_firecam_capture_handler(char* module, char* variable,
    char* value) {
  try {
    capture_mode = FireCAMUtils::convert(capture_mode_str,
      FireCAMCapture::modeStrings);
    FireCAMCapture capture(capture_buffer, capture_mode, capture_speed);
    configuration.setCapture(capture);

    camera.disconnect();
    usleep(10000);
    camera.setConfiguration(configuration);
    camera.connect();
  }
  catch (std::runtime_error error) {
    carmen_warn("\nWarning: %s\n", error.what());
  }
}

void carmen_firecam_filter_handler(char* module, char* variable, char* value) {
  try {
    filter_tile = FireCAMUtils::convert(filter_tile_str,
      FireCAMColorFilter::tileStrings);
    filter_method = FireCAMUtils::convert(filter_method_str,
      FireCAMColorFilter::methodStrings);
    FireCAMColorFilter colorFilter(filter_enabled, filter_tile, filter_method);
    configuration.setColorFilter(colorFilter);

    camera.setConfiguration(configuration);
  }
  catch (std::runtime_error error) {
    carmen_warn("\nWarning: %s\n", error.what());
  }
}

void carmen_firecam_thumb_handler(char* module, char* variable, char* value) {
  // nothing to do here
}

void carmen_firecam_dump_handler(char* module, char* variable, char* value) {
  if (!realpath(dump_dir, dump_path)) {
    dump_path[0] = '\0';
    carmen_warn("\nWarning: Dump directory %s is invalid\n", dump_dir);
  }
}

void carmen_firecam_feature_handler(char* module, char* variable,
    char* value) {
  try {
    char name[256], option[256];
    std::string var(variable);
    var[var.find_first_of('_')] = ' ';
    var[var.find_last_of('_')] = ' ';

    if (sscanf(var.c_str(), "firecam%d %s %s", &id, name, option) == 3) {
      FireCAMFeature feature = configuration.getFeature(name);

      feature.setEnabled(features[name].enabled);
      if (features[name].mode_str)
        feature.setMode(FireCAMUtils::convert(features[name].mode_str,
          FireCAMFeature::modeStrings));
      if (features[name].values_str) {
        std::vector<size_t> values;
        FireCAMUtils::convert(features[name].values_str, values);
        feature.setValues(values);
      }
      configuration.setFeature(feature);

      camera.setConfiguration(configuration);
    }
  }
  catch (std::runtime_error error) {
    carmen_warn("\nWarning: %s\n", error.what());
  }
}

int carmen_firecam_read_parameters(int argc, char **argv) {
  char module[] = "firecam";
  int num_params;

  char guid_var[256];
  char mode_width_var[256], mode_height_var[256], mode_color_var[256],
    mode_scalable_var[256];
  char framerate_var[256];
  char capture_buffer_var[256], capture_mode_var[256], capture_speed_var[256];
  char filter_enabled_var[256], filter_tile_var[256], filter_method_var[256];
  char thumb_width_var[256], thumb_height_var[256], thumb_color_var[256];
  char dump_enabled_var[256], dump_dir_var[256];

  if (argc == 2)
    id = atoi(argv[1]);

  sprintf(guid_var, "firecam%d_guid", id);

  sprintf(mode_width_var, "firecam%d_mode_width", id);
  sprintf(mode_height_var, "firecam%d_mode_height", id);
  sprintf(mode_color_var, "firecam%d_mode_color", id);
  sprintf(mode_scalable_var, "firecam%d_mode_scalable", id);

  sprintf(framerate_var, "firecam%d_framerate", id);

  sprintf(capture_buffer_var, "firecam%d_capture_buffer", id);
  sprintf(capture_mode_var, "firecam%d_capture_mode", id);
  sprintf(capture_speed_var, "firecam%d_capture_speed", id);

  sprintf(filter_enabled_var, "firecam%d_filter_enabled", id);
  sprintf(filter_tile_var, "firecam%d_filter_tile", id);
  sprintf(filter_method_var, "firecam%d_filter_method", id);

  sprintf(thumb_width_var, "firecam%d_thumb_width", id);
  sprintf(thumb_height_var, "firecam%d_thumb_height", id);
  sprintf(thumb_color_var, "firecam%d_thumb_color", id);

  sprintf(dump_enabled_var, "firecam%d_dump_enabled", id);
  sprintf(dump_dir_var, "firecam%d_dump_dir", id);

  carmen_param_t params[] = {
    {module, guid_var, CARMEN_PARAM_STRING, &guid_str, 0, NULL},

    {module, mode_width_var, CARMEN_PARAM_INT, &mode_width, 1,
      carmen_firecam_mode_handler},
    {module, mode_height_var, CARMEN_PARAM_INT, &mode_height, 1,
      carmen_firecam_mode_handler},
    {module, mode_color_var, CARMEN_PARAM_STRING, &mode_color_str, 1,
      carmen_firecam_mode_handler},
    {module, mode_scalable_var, CARMEN_PARAM_ONOFF, &mode_scalable, 1,
      carmen_firecam_mode_handler},

    {module, framerate_var, CARMEN_PARAM_DOUBLE, &framerate_fps, 1,
      carmen_firecam_framerate_handler},

    {module, capture_buffer_var, CARMEN_PARAM_INT, &capture_buffer, 1,
      carmen_firecam_capture_handler},
    {module, capture_mode_var, CARMEN_PARAM_STRING, &capture_mode_str, 1,
      carmen_firecam_capture_handler},
    {module, capture_speed_var, CARMEN_PARAM_INT, &capture_speed, 1,
      carmen_firecam_capture_handler},

    {module, filter_enabled_var, CARMEN_PARAM_ONOFF, &filter_enabled, 1,
      carmen_firecam_filter_handler},
    {module, filter_tile_var, CARMEN_PARAM_STRING, &filter_tile_str, 1,
      carmen_firecam_filter_handler},
    {module, filter_method_var, CARMEN_PARAM_STRING, &filter_method_str, 1,
      carmen_firecam_filter_handler},

    {module, thumb_width_var, CARMEN_PARAM_INT, &thumb_width, 1,
      carmen_firecam_thumb_handler},
    {module, thumb_height_var, CARMEN_PARAM_INT, &thumb_height, 1,
      carmen_firecam_thumb_handler},
    {module, thumb_color_var, CARMEN_PARAM_STRING, &thumb_color_str, 1,
      carmen_firecam_thumb_handler},

    {module, dump_enabled_var, CARMEN_PARAM_ONOFF, &dump_enabled, 1,
      carmen_firecam_dump_handler},
    {module, dump_dir_var, CARMEN_PARAM_DIR, &dump_dir, 1,
      carmen_firecam_dump_handler},
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

  thumb_color = FireCAMUtils::convert(thumb_color_str,
    FireCAMColor::codingStrings);

  if (!realpath(dump_dir, dump_path)) {
    dump_path[0] = '\0';
    carmen_warn("\nWarning: Dump directory %s is invalid\n", dump_dir);
  }

  carmen_param_allow_unfound_variables(1);

  for (std::set<FireCAMFeature>::const_iterator it =
      camera.getFeatures().begin(); it != camera.getFeatures().end(); ++it) {
    FireCAMFeature feature = *it;
    int num_feature_params;
    char feature_enabled_var[256], feature_mode_var[256],
      feature_values_var[256];

    carmen_firecam_feature_param_t& feature_param =
      features[feature.getName()];
    feature_param.mode_str = 0;
    feature_param.values_str = 0;

    sprintf(feature_enabled_var, "firecam%d_%s_enabled", id,
      feature.getName().c_str());
    sprintf(feature_mode_var, "firecam%d_%s_mode", id,
      feature.getName().c_str());
    sprintf(feature_values_var, "firecam%d_%s_values", id,
      feature.getName().c_str());

    carmen_param_t feature_params[] = {
      {module, feature_enabled_var, CARMEN_PARAM_ONOFF,
        &feature_param.enabled, 1, carmen_firecam_feature_handler},
      {module, feature_mode_var, CARMEN_PARAM_STRING,
        &feature_param.mode_str, 1, carmen_firecam_feature_handler},
      {module, feature_values_var, CARMEN_PARAM_STRING,
        &feature_param.values_str, 1, carmen_firecam_feature_handler},
    };

    num_feature_params = sizeof(feature_params)/sizeof(carmen_param_t);
    carmen_param_install_params(argc, argv, feature_params, num_feature_params);
    num_params += num_feature_params;

    feature.setEnabled(feature_param.enabled);
    if (feature_param.mode_str)
      feature.setMode(FireCAMUtils::convert(feature_param.mode_str,
        FireCAMFeature::modeStrings));
    if (feature_param.values_str) {
      std::vector<size_t> values;
      FireCAMUtils::convert(feature_param.values_str, values);
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
    camera.capture(frame, thumb_color);

    if (dump_enabled) {
      std::string filename = frame.dump(dump_path);
      carmen_firecam_publish_frame(id, (char*)filename.c_str(),
        frame.getTimestamp());
    }
    frame.resize(thumb_width, thumb_height);

    carmen_firecam_publish_image(frame);
    carmen_ipc_sleep(0.0);
  }
  camera.disconnect();
  std::cout << std::endl;

  return 0;
}
