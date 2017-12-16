project(Plugins)

include_directories(../EvolutionLib)

add_subdirectory(Objectives)
add_subdirectory(Operators)

set(PLUGINS ${PLUGINS_OPERATORS} ${PLUGINS_OBJECTIVES} PARENT_SCOPE)
