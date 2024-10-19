cmake_minimum_required(VERSION 3.16)
project(music_visualizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(MUSIC_VISUALIZER
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  )

set(MUSIC_WAVE_PLOTTER
  ${CMAKE_CURRENT_SOURCE_DIR}/src/music_wave_plotter.c
  )

find_package(raylib)

add_executable(music_visualizer)
target_sources(music_visualizer
 PRIVATE
  ${MUSIC_VISUALIZER}
  )

add_executable(music_wave_plotter)
target_sources(music_wave_plotter
 PRIVATE
  ${MUSIC_WAVE_PLOTTER}
  )

target_include_directories(music_visualizer
 PRIVATE
  ${raylib_INCLUDE_DIRS}
  )

target_include_directories(music_wave_plotter
 PRIVATE
  ${raylib_INCLUDE_DIRS}
  )

target_link_libraries(music_visualizer
  ${raylib_LIBRARIES}
)

target_link_libraries(music_wave_plotter
  ${raylib_LIBRARIES}
)
