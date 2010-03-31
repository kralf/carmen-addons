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

remake_pack_deb(COMPONENT glv carmen-gui)
remake_pack_deb(COMPONENT nsick libepos tulibs carmen-interface ubash)
remake_pack_deb(COMPONENT firecam carmen-interface)
