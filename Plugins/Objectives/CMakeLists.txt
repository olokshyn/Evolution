file(GLOB objectives *.c)

foreach(objective ${objectives})
    get_filename_component(target ${objective} NAME_WE)
    message("Found objective: ${target}")
    set(target ${target}_objective)
    add_library(${target} SHARED ${objective})
    target_link_libraries(${target} EvolutionLib)
    list(APPEND all_objectives ${target})
endforeach(objective)

set(PLUGINS_OBJECTIVES ${all_objectives} PARENT_SCOPE)
