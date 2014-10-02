remake_find_package(carmen-core CONFIG)
remake_find_package(gl CONFIG)
remake_find_package(glu CONFIG)
remake_find_package(GLUT)
remake_find_package(Threads)
remake_find_library(m math.h PACKAGE libm)

if(NOT ${CMAKE_USE_PTHREADS_INIT})
  message(FATAL_ERROR "Missing POSIX thread support!")
endif(NOT ${CMAKE_USE_PTHREADS_INIT})

remake_define(REDHAT_52 ON)
remake_define(REDHAT_6 ON)
remake_define(REDHAT_71 ON)

remake_add_directories()
