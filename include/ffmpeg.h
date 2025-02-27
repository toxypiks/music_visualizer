#ifndef FFMPEG_H
#define FFMPEG_H

int ffmpeg_start_rendering(size_t width, size_t height, size_t fps, const char *sound_file_path);
void ffmpeg_send_frame(int pipe, void *data, size_t width, size_t height);
void ffmpeg_end_rendering(int pipe);

#endif // FFMPEG_H
