remake_project_get(CONFIGURATION_DESTINATION)
add_definitions(-DCONFIGURATION_DESTINATION="${CONFIGURATION_DESTINATION}")

remake_include(../../lib/commander)

remake_add_executables(LINK commander_core)
remake_add_scripts(commander_alternatives)
