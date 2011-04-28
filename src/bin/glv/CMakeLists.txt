remake_include(
  ../../lib/gl
  ../../lib/glv
)

remake_add_executables(glv2vrml.c LINK glv)
remake_add_executables(glview.c LINK gl glv)
