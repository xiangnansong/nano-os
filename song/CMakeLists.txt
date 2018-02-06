include_directories(../main)
include_directories(../base)
include_directories(../song)

aux_source_directory(. src)
add_library(song STATIC ${src})
