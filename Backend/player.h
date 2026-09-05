#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include "miniaudio.h"

typedef struct
{
	char current_track_path[1024];
	bool is_playing;
	bool is_paused;
	float volume;
	double total_duration;
	double current_time;
	
	AVFormatContext *format_ctx;
	AVCodecContext *codec_ctx;
	int audio_stream_index;
	
	ma_device audio_device;
	bool ffmpeg_ready;
} PlayerContext;

// All functions her (API)
// Сергей, вызывай их в ГТК отсюда. Буду стараться обновлять их
int player_init(PlayerContext *ctx);
int player_load(PlayerContext *ctx, const char *file_path);
void player_play(PlayerContext *ctx);
void player_pause(PlayerContext *ctx);
void player_stop(PlayerContext *ctx);
void player_set_volume(PlayerContext *ctx, float volume);
void player_seek(PlayerContext *ctx, double seconds);
// Clear data. Important to call this command before closing an app, or your audio in linux will die
void player_uinit(PlayerContext *ctx);

#endif
