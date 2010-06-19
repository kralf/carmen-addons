cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  "CARMEN Addons"
  VERSION 0.1
  RELEASE alpha
  SUMMARY "A collection of addons for CARMEN."
  AUTHOR "Ralf Kaestner"
  CONTACT "ralf.kaestner@gmail.com"
  HOME "http://aslforge.ethz.ch/projects/carmen-addons"
  LICENSE "GNU Lesser General Public License (LGPL)"
  INSTALL /usr
  PREFIX carmen-
)

remake_pack_deb(
  COMPONENT commander
  DESCRIPTION "process commander"
  libncurses bash
)
remake_pack_deb(
  COMPONENT commander-dev
  DESCRIPTION "process commander development headers"
  carmen_addons-commander libncurses-dev
)

remake_pack_deb(
  COMPONENT glv
  DESCRIPTION "GLV file format support"
  carmen-gui
)
remake_pack_deb(
  COMPONENT glv-dev
  DESCRIPTION "GLV development headers"
  carmen_addons-glv carmen-gui-dev
)

remake_pack_deb(
  COMPONENT nsick
  DESCRIPTION "nodding SICK driver"
  carmen-interface libepos
)
remake_pack_deb(
  COMPONENT nsick-dev
  DESCRIPTION "nodding SICK development headers"
  carmen_addons-nsick carmen-interface-dev libepos-dev
)

remake_pack_deb(
  COMPONENT firecam
  DESCRIPTION "FireCAM driver"
  carmen-interface libfirecam
)
remake_pack_deb(
  COMPONENT firecam-dev
  DESCRIPTION "FireCAM development headers"
  carmen_addons-firecam carmen-interface-dev libfirecam-dev
)

remake_pack_deb(
  COMPONENT multiparam
  DESCRIPTION "multi-parameter server"
  carmen-interface
)
