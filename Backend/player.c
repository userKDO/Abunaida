#include "player.h"
#include <string.h>
#include <libavformat/avformat.h>

void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	(void)pDevice; (void)pOutput; (void)pInput; (void)frameCount;
}

int player_init(PlayerContext *ctx)
{
	if (ctx == NULL) 
	{
		return -1; // No data adress
	}
	
	memset(ctx->current_track_path, 0, sizeof(ctx->current_track_path));
	
	ctx->is_playing = false;
	ctx->is_paused = false;
	
	ctx->volume = 0.5f;
	
	ctx->total_duration = 0.0;
	ctx->current_time = 0.0;
	
	ctx->format_ctx = NULL;
	ctx->codec_ctx = NULL;
	ctx->audio_stream_index = -1;
	ctx->ffmpeg_ready = false;
	
	ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = ma_format_s16;
	deviceConfig.playback.channels = 2;
	deviceConfig.sampleRate = 44100;
	deviceConfig.dataCallback = audio_callback;
	deviceConfig.pUserData = ctx;
	
	if (ma_device_init(NULL, &deviceConfig, &ctx->audio_device) != MA_SUCCESS) 
	{
		return -2; //Audio board did not respond or not available
	}
	
	return 0;
}

int player_load(PlayerContext *ctx, const char *file_path)
{
	if (ctx == NULL || file_path == NULL)
	{
		return -1;
	}
	
	if (ctx->ffmpeg_ready)
	{
		if (ctx->codec_ctx) avcodec_free_context(&ctx->codec_ctx);
		if (ctx->format_ctx) avformat_close_input(&ctx->format_ctx);
		ctx->ffmpeg_ready = false;
	}
	
	strncpy(ctx->current_track_path, file_path, sizeof(ctx->current_track_path) - 1);
	
	if (avformat_open_input(&ctx->format_ctx, file_path, NULL, NULL) < 0)
	{
		return -2;
	}
	
	if (avformat_find_stream_info(ctx->format_ctx, NULL) < 0)
	{
		avformat_close_input(&ctx->format_ctx);
		return -3;
	}
	
	ctx->audio_stream_index = -1;
	
	for (unsigned int i = 0; i < ctx->format_ctx->nb_streams; i++)
	{
		if (ctx->format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			ctx->audio_stream_index = i;
			break;
		}
	}
	
	if (ctx->audio_stream_index == -1)
	{
		avformat_close_input(&ctx->format_ctx);
		return -4;
	}
	
	const AVCodec *codec = avcodec_find_decoder(ctx->format_ctx->streams[ctx->audio_stream_index]->codecpar->codec_id);
	if (!codec) 
	{
		avformat_close_input(&ctx->format_ctx);
		return -5;
	}
	
	ctx->codec_ctx = avcodec_alloc_context3(codec);
	if (avcodec_parameters_to_context(ctx->codec_ctx, ctx->format_ctx->streams[ctx->audio_stream_index]->codecpar) < 0)
	{
		avcodec_free_context(&ctx->codec_ctx);
		avformat_close_input(&ctx->format_ctx);
		return -6;
	}
	
	if (avcodec_open2(ctx->codec_ctx, codec, NULL) < 0)
	{
		avcodec_free_context(&ctx->codec_ctx);
		avformat_close_input(&ctx->format_ctx);
		return -7;
	}
	
	if (ctx->format_ctx->duration != AV_NOPTS_VALUE)
	{
		ctx->total_duration = (double)ctx->format_ctx->duration / AV_TIME_BASE;
	}
	else
	{
		ctx->total_duration = 0.0;
	}
	
	ctx->current_time = 0.0;
	
	ctx->ffmpeg_ready = true;
	
	return 0;
}

void player_uinit(PlayerContext *ctx)
{
	if (ctx == NULL) return;
	
	ma_device_uninit(&ctx->audio_device);
	
	if(ctx->codec_ctx) avcodec_free_context(&ctx->codec_ctx);
	if(ctx->format_ctx) avformat_close_input(&ctx->format_ctx);
	
	ctx->ffmpeg_ready = false;
}
