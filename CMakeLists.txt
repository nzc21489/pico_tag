project(pico_tag)

add_library(pico_tag STATIC
pico_tag.cpp
pico_tag_aac.cpp
pico_tag_file.cpp
pico_tag_flac.cpp
pico_tag_mp3.cpp
pico_tag_no_valid_tag.cpp
pico_tag_opus.cpp
pico_tag_uitl.cpp
pico_tag_wav.cpp
)
