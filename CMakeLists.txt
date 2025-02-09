cmake_minimum_required(VERSION 3.16)
project(music_visualizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(raylib)

set(SIMPLE_EXAMPLE_FFMPEG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/test_scripts/simple_example_ffmpeg.c
  )

add_executable(simple_example_ffmpeg)
target_sources(simple_example_ffmpeg
 PRIVATE
  ${SIMPLE_EXAMPLE_FFMPEG_SRC}
  )

target_link_libraries(simple_example_ffmpeg
  ${raylib_LIBRARIES}
  )

set(MUSIC_VISUALIZER
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  )

set(MUSIC_WAVE_PLOTTER
  ${CMAKE_CURRENT_SOURCE_DIR}/test_scripts/music_wave_plotter.c
  )

set(FFT_TRYOUT
  ${CMAKE_CURRENT_SOURCE_DIR}/test_scripts/fft_tryout.c
  )

set(PLUG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/plug.c
  )

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

add_executable(fft_tryout)
target_sources(fft_tryout
 PRIVATE
  ${FFT_TRYOUT}
  )

add_library(plug SHARED)
target_sources(plug
 PRIVATE
  ${PLUG_SRC}
  )

target_include_directories(music_visualizer
 PRIVATE
  ${raylib_INCLUDE_DIRS}
  )

target_include_directories(music_wave_plotter
 PRIVATE
 ${raylib_INCLUDE_DIRS}
 include
  )

target_include_directories(fft_tryout
 PRIVATE
  ${raylib_INCLUDE_DIRS}
  )

target_include_directories(plug
 PRIVATE
  ${raylib_INCLUDE_DIRS}
)

# here is HOTRELAOD checked
if(DEFINED HOTRELOAD)
  message("HOTRELOADE enabled (${HOTRELOAD})")
  target_compile_definitions(music_visualizer PRIVATE HOTRELOAD)
else(DEFINED HOTRELOAD)
  set(MUSIC_VISUALIZER_LINK_PLUG_LIB "plug")
  message("lib plug: ${MUSIC_VISUALIZER_LINK_PLUG_LIB}")
endif(DEFINED HOTRELOAD)
# supress cmake warning var is not in use (boilerplate)

target_link_libraries(music_visualizer
  ${raylib_LIBRARIES}
  ${MUSIC_VISUALIZER_LINK_PLUG_LIB}
)

target_link_libraries(music_wave_plotter
  ${raylib_LIBRARIES}
)

target_link_libraries(fft_tryout
  ${raylib_LIBRARIES}
  )

target_link_libraries(plug
  ${raylib_LIBRARIES}
  )

target_link_libraries(fft_tryout m)

target_link_libraries(music_visualizer m)
