include_directories(../base)
include_directories(../song)
include_directories(../gui)

aux_source_directory(. src)
add_executable(song.elf ${src} )
target_link_libraries(song.elf base song gui)
