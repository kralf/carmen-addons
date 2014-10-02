cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  carmen-addons
  VERSION 0.1
  RELEASE alpha
  SUMMARY "A collection of addons for CARMEN."
  AUTHOR "Ralf Kaestner"
  CONTACT "ralf.kaestner@gmail.com"
  HOME "http://github.com/kralf/carmen-addons"
  LICENSE "GNU Lesser General Public License (LGPL)"
  INSTALL /usr
  PREFIX carmen-
)

remake_pack_deb(
  COMPONENT commander
  DESCRIPTION "process commander"
  DEPENDS libncurses5 bash
)
remake_pack_deb(
  COMPONENT commander-dev
  DESCRIPTION "process commander development headers"
  DEPENDS carmen-addons-commander libncurses5-dev
)

remake_pack_deb(
  COMPONENT gl
  DESCRIPTION "OpenGL visualization"
  DEPENDS carmen-core freeglut3
)
remake_pack_deb(
  COMPONENT gl-dev
  DESCRIPTION "OpenGL visualization development headers"
  DEPENDS carmen-addons-gl carmen-core-dev freeglut3-dev
)

remake_pack_deb(
  COMPONENT glv
  DESCRIPTION "GLV file format support"
  DEPENDS carmen-addons-gl carmen-core
)
remake_pack_deb(
  COMPONENT glv-dev
  DESCRIPTION "GLV development headers"
  DEPENDS carmen-addons-glv carmen-core-dev
)

remake_pack_deb(
  COMPONENT nsick
  DESCRIPTION "nodding SICK driver"
  DEPENDS carmen libnsick tulibs
)
remake_pack_deb(
  COMPONENT nsick-dev
  DESCRIPTION "nodding SICK development headers"
  DEPENDS carmen-addons-nsick carmen-core-dev
)

remake_pack_deb(
  COMPONENT firecam
  DESCRIPTION "FireCAM driver"
  DEPENDS carmen libfirecam
)
remake_pack_deb(
  COMPONENT firecam-dev
  DESCRIPTION "FireCAM development headers"
  DEPENDS carmen-addons-firecam carmen-core-dev
)

remake_pack_deb(
  COMPONENT velodyne
  DESCRIPTION "Velodyne HDL driver"
  DEPENDS carmen-addons-gl carmen libvelodyne
)
remake_pack_deb(
  COMPONENT velodyne-dev
  DESCRIPTION "Velodyne HDL development headers"
  DEPENDS carmen-addons-velodyne carmen-core-dev
)

remake_pack_deb(
  COMPONENT multiparam
  DESCRIPTION "multi-parameter server"
  DEPENDS carmen
)

remake_distribute_deb(
  DISTRIBUTION lucid
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS libncurses5-dev bash carmen-dev freeglut3-dev libnsick-dev
    libfirecam-dev libvelodyne-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE CARMEN_ADDONS_GIT_REVISION
)
remake_distribute_deb(
  DISTRIBUTION precise
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS libncurses5-dev bash carmen-dev freeglut3-dev libnsick-dev
    libfirecam-dev libvelodyne-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE CARMEN_ADDONS_GIT_REVISION
)
remake_distribute_deb(
  DISTRIBUTION trusty
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS libncurses5-dev bash carmen-dev freeglut3-dev libnsick-dev
    libfirecam-dev libvelodyne-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE CARMEN_ADDONS_GIT_REVISION
)
