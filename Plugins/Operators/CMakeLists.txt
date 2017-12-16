file(GLOB operators *.c)

foreach(operator ${operators})
    get_filename_component(target ${operator} NAME_WE)
    message("Found operators: ${target}")
    set(target ${target}_operator)
    add_library(${target} SHARED ${operator})
    target_link_libraries(${target} EvolutionLib)
    list(APPEND all_operators ${target})
endforeach(operator)

set(PLUGINS_OPERATORS ${all_operators} PARENT_SCOPE)
