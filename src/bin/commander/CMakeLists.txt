remake_project_get(CONFIGURATION_DESTINATION)
remake_define(CONFIGURATION_DESTINATION QUOTED "${CONFIGURATION_DESTINATION}")

remake_include(../../lib/commander)

remake_add_executables(LINK commander_core)
remake_add_scripts(commander_alternatives)
