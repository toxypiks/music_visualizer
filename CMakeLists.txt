cmake_minimum_required(VERSION 3.16)
project(music_visualizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(MUSIC_VISUALIZER
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  )

find_package(raylib)

add_executable(music_visualizer)
target_sources(music_visualizer
 PRIVATE
  ${MUSIC_VISUALIZER}
  )

target_include_directories(music_visualizer
 PRIVATE
  ${raylib_INCLUDE_DIRS}
  )

target_link_libraries(music_visualizer
  ${raylib_LIBRARIES}
)
