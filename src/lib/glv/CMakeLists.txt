remake_add_library(glv LINK ${CARMEN_CORE_LIBRARIES})
remake_add_headers()
remake_pkg_config_generate(REQUIRES carmen-core)
